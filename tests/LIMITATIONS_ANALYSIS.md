# SubC Compiler - Análise Detalhada de Limitações

## Versão: 2025-12-22 (Cross-compilation support)

Este documento analisa em profundidade as limitações do compilador SubC,
especialmente relacionadas a parâmetros de função, va_args e convenções de chamada.

---

## 1. Constantes de Limite Definidas em `defs.h`

```c
#define TEXTLEN     512      // Tamanho máximo de texto/token
#define NAMELEN     16       // Tamanho máximo de identificadores
#define MAXFILES    32       // Máximo de arquivos de entrada
#define MAXINCDIRS  16       // Máximo de diretórios de include
#define MAXIFDEF    16       // Profundidade máxima de #ifdef aninhados
#define MAXNMAC     32       // Máximo de macros em expansão simultânea
#define MAXCASE     256      // Máximo de cases em um switch
#define MAXBREAK    16       // Profundidade máxima de loops/switches aninhados
#define MAXLOCINIT  32       // Máximo de inicializadores locais
#define MAXFNARGS   32       // Máximo de argumentos por função
#define NSYMBOLS    1024     // Máximo de símbolos na tabela
#define POOLSIZE    16384    // Tamanho do pool de nomes
#define NODEPOOLSZ  4096     // Tamanho do pool de nós AST (em ints)
```

---

## 2. Limitação de Parâmetros de Função (MAXFNARGS = 32)

### 2.1 Onde é Aplicada

**Em `decl.c:489-493` (função `signature`):**
```c
static void signature(int fn, int from, int to) {
    int types[MAXFNARGS+1], i;

    if (to - from > MAXFNARGS)
        error("too many function parameters", Names[fn]);
    // ...
}
```

**Em `expr.c:137-159` (função `fnargs`):**
```c
static node *fnargs(int fn, int *na) {
    int lv[LV];
    int *types;
    char msg[100];
    int sgn[MAXFNARGS+1];  // Array fixo para assinatura
    // ...
    if (*na < MAXFNARGS) sgn[*na] = lv[LVPRIM], sgn[*na+1] = 0;
    // ...
}
```

### 2.2 Problemas

1. **Limite arbitrário de 32 parâmetros** - muito baixo para C89 que não especifica limite
2. **Array de assinatura de tamanho fixo** - não há alocação dinâmica
3. **Erro silencioso após 32 args** - a assinatura é truncada sem aviso

### 2.3 Impacto

- Funções com mais de 32 parâmetros compilam mas a verificação de tipos é truncada
- Não há erro fatal, apenas warning "too many function parameters"
- A assinatura armazenada em `Mtext[fn]` fica incompleta

---

## 3. Implementação de va_args - PROBLEMAS CRÍTICOS

### 3.1 Declaração em `varargs.h`

```c
void  **_va_start(void *last);
void   *_va_arg(void **ap);
void    _va_end(void **ap);
```

### 3.2 Implementação em `varargs.c`

```c
void **_va_start(void *last) {
    return (void **) last + 1;
}

void *_va_arg(void **ap) {
    return *((void **) *ap)++;
}

void _va_end(void **ap) {}
```

### 3.3 PROBLEMAS IDENTIFICADOS

#### Problema 1: Incompatibilidade com x86-64 ABI
- **x86-64 System V ABI** passa os primeiros 6 argumentos inteiros em registradores (rdi, rsi, rdx, rcx, r8, r9)
- A implementação de `_va_start` assume que TODOS os argumentos estão na stack
- **Resultado**: va_args não funciona corretamente em x86-64!

#### Problema 2: Aritmética de ponteiro incorreta
```c
void *_va_arg(void **ap) {
    return *((void **) *ap)++;  // PROBLEMA: incrementa *ap, não ap
}
```
O código incrementa o valor apontado por `*ap`, não o ponteiro `ap` em si.
Isso causa comportamento indefinido.

#### Problema 3: Tamanho de argumento fixo
- Assume que todos os argumentos têm tamanho `sizeof(void*)`
- Não funciona para tipos menores (char, short) que são promovidos
- Não funciona para structs (que SubC não suporta passar por valor de qualquer forma)

#### Problema 4: Alinhamento
- Não considera alinhamento de stack
- x86-64 requer alinhamento de 16 bytes para a stack

### 3.4 Código Gerado para Chamadas de Função

**i386 (`cg_i386.c`):**
```c
static void i386_cgpush(void)       { gen("pushl\t%eax"); }
static void i386_cgstack(int n)     { ngen("%s\t$%d,%%esp", "addl", n); }
```
- Argumentos são empilhados da direita para esquerda
- Caller limpa a stack após a chamada
- Convenção cdecl padrão - **funciona corretamente**

**x86-64 (`cg_x86_64.c`):**
```c
static void x64_cgpush(void)        { gen("pushq\t%rax"); }
static void x64_cgstack(int n)      { ngen("%s\t$%d,%%rsp", "addq", n); }
```
- **PROBLEMA**: Também empilha todos os argumentos na stack
- **NÃO SEGUE** a System V AMD64 ABI que usa registradores
- va_args quebra porque `_va_start` espera argumentos na stack

---

## 4. Convenção de Chamada - Análise Detalhada

### 4.1 Como SubC Gera Chamadas de Função

**Em `tree.c:328-342`:**
```c
case OP_CALL:   emitargs(a->left);      // Empilha argumentos
                commit();
                spill();
                gencall(a->args[0]);     // Chama função
                genstack((a->args[1]) * BPW);  // Limpa stack
                break;
```

**Em `tree.c:204-208`:**
```c
void emitargs(node *a) {
    if (NULL == a) return;
    emittree1(a->right);    // Processa argumento atual
    emitargs(a->left);      // Recursão para próximos (direita->esquerda)
}
```

### 4.2 Problema: Todos os Argumentos na Stack

O SubC **sempre** empilha todos os argumentos, independente da arquitetura:

```c
// Em gen.c:576
void genstack(int n) {
    if (n) {
        gentext();
        cgstack(n);  // Ajusta stack após chamada
    }
}
```

Isso significa que:
- **i386**: Funciona corretamente (cdecl usa stack)
- **x86-64**: **INCORRETO** - deveria usar rdi, rsi, rdx, rcx, r8, r9
- **ARM**: **INCORRETO** - deveria usar r0-r3

### 4.3 Declaração de Convenção (Não Utilizada)

Em `cgtarget.h`:
```c
enum cg_call_conv {
    CC_CDECL = 0,
    CC_STDCALL,
    CC_FASTCALL,
    CC_SYSV_AMD64,   // Declarado mas não implementado!
    CC_WIN64,
    CC_AAPCS,
    CC_AAPCS64,
    CC_HLASM
};
```

A arquitetura x86-64 declara `CC_SYSV_AMD64` mas o código gerado **não segue** essa convenção!

---

## 5. Outras Limitações Importantes

### 5.1 Profundidade de Loops/Switches (MAXBREAK = 16)

**Em `stmt.c:31-40`:**
```c
static void pushbrk(int id) {
    if (Bsp >= MAXBREAK)
        fatal("too many nested loops/switches");
    Breakstk[Bsp++] = id;
}
```

### 5.2 Cases em Switch (MAXCASE = 256)

**Em `stmt.c:200-214`:**
```c
static void switch_block(void) {
    int cval[MAXCASE];
    int clab[MAXCASE];
    int nc = 0;
    // ...
    if ((CASE == Token || DEFAULT == Token) && nc >= MAXCASE) {
        error("too many 'case's in 'switch'", NULL);
        nc = 0;  // Reset silencioso!
    }
}
```

### 5.3 Pool de Nós AST (NODEPOOLSZ = 4096)

**Em `tree.c:22-23`:**
```c
if (Ndtop + hdrlen + na >= NODEPOOLSZ)
    fatal("expression too complex (out of nodes)");
```

### 5.4 Inicializadores Locais (MAXLOCINIT = 32)

**Em `decl.c:464-468`:**
```c
if (Nli >= MAXLOCINIT) {
    error("too many local initializers", NULL);
    Nli = 0;
}
```

---

## 6. Problemas de Tipo no Sistema de Tipos

### 6.1 Apenas Dois Tipos Primitivos

- `PCHAR` (unsigned char)
- `PINT` (signed int)

Não há: `long`, `short`, `unsigned int`, `float`, `double`

### 6.2 Máximo de 2 Níveis de Indireção

**Em `decl.c:214-218`:**
```c
if (CHARPP == prim || INTPP == prim || VOIDPP == prim ||
    FUNPTR == prim ||
    (prim & STCMASK) == STCPP || (prim & STCMASK) == UNIPP
)
    error("too many levels of indirection", NULL);
```

### 6.3 Arrays Apenas 1D

Não suporta `int a[10][20]`

---

## 7. Resumo dos Problemas Críticos

| Problema | Severidade | Impacto |
|----------|------------|---------|
| va_args em x86-64 | **CRÍTICO** | Não funciona |
| Convenção de chamada x86-64 | **CRÍTICO** | Incompatível com ABI |
| MAXFNARGS = 32 | Médio | Limita funções complexas |
| _va_arg implementação | **CRÍTICO** | Comportamento indefinido |
| MAXCASE = 256 | Baixo | Limita switches grandes |
| MAXBREAK = 16 | Baixo | Limita aninhamento |

---

---

## 9. ~~LIMITAÇÃO CRÍTICA: Stack Direction Hardcoded~~ ✅ CORRIGIDO

### 9.1 O Problema (RESOLVIDO)

O compilador SubC tem campos na estrutura `cg_arch` para configurar a direção
da stack e offsets de parâmetros/locais:

```c
// Em cgtarget.h
struct cg_arch {
    // ...
    int  stack_dir;           /* STACK_DOWN ou STACK_UP */
    int  param_offset_base;   /* Offset inicial para parâmetros */
    int  param_offset_dir;    /* Direção do offset de parâmetros: 1 ou -1 */
    int  local_offset_base;   /* Offset inicial para variáveis locais */
    int  local_offset_dir;    /* Direção do offset de locais: 1 ou -1 */
};
```

**✅ Esses campos AGORA SÃO UTILIZADOS! (Corrigido em 2025-12-23)**

### 9.2 Código Corrigido em `decl.c`

**Parâmetros de função (`pmtrdecls()`):**
```c
static int pmtrdecls(void) {
    // ...
    addr = CG_PARAM_OFFSET_BASE;  // Usa campo da arquitetura
    for (;;) {
        // ...
        addloc(name, prim, type, CAUTO, size, addr, 0);
        addr += CG_PARAM_OFFSET_DIR * BPW;  // Usa direção da arquitetura
        // ...
    }
}
```

**Variáveis locais (`localdecls()`):**
```c
static int localdecls(void) {
    addr = CG_LOCAL_OFFSET_BASE;  // Usa campo da arquitetura
    // ...
    addr += CG_LOCAL_OFFSET_DIR * rsize;  // Usa direção da arquitetura
    addloc(name, prim, type, CAUTO, size, addr, 0);
}
```

### 9.3 Suporte para Arquiteturas STACK_UP (ex: S/370, z/Architecture)

Agora é possível implementar arquiteturas onde a stack cresce para CIMA:

| Aspecto | STACK_DOWN (x86) | STACK_UP (S/370) |
|---------|------------------|------------------|
| `param_offset_dir` | 1 (positivo) | -1 (negativo) |
| `local_offset_dir` | -1 (negativo) | 1 (positivo) |
| Parâmetros | Offsets positivos do FP | Offsets negativos do FP |
| Locais | Offsets negativos do FP | Offsets positivos do FP |

### 9.4 Macros de Acesso Disponíveis

```c
#define CG_PARAM_OFFSET_BASE  (CG->arch->param_offset_base)
#define CG_PARAM_OFFSET_DIR   (CG->arch->param_offset_dir)
#define CG_LOCAL_OFFSET_BASE  (CG->arch->local_offset_base)
#define CG_LOCAL_OFFSET_DIR   (CG->arch->local_offset_dir)
```

### 9.5 Configuração por Arquitetura

Cada code generator agora define os valores corretos:

```c
// i386: params em 8, 12, 16...; locals em -4, -8, -12...
struct cg_arch cg_arch_i386 = {
    // ...
    8,   /* param_offset_base */
    1,   /* param_offset_dir */
    0,   /* local_offset_base */
    -1   /* local_offset_dir */
};

// x86-64: params em 16, 24, 32...; locals em -8, -16, -24...
struct cg_arch cg_arch_x86_64 = {
    // ...
    16,  /* param_offset_base */
    1,   /* param_offset_dir */
    0,   /* local_offset_base */
    -1   /* local_offset_dir */
};

// 8086: params em 4, 6, 8...; locals em -2, -4, -6...
struct cg_arch cg_arch_8086 = {
    // ...
    4,   /* param_offset_base */
    1,   /* param_offset_dir */
    0,   /* local_offset_base */
    -1   /* local_offset_dir */
};
```

---

## 10. Outras Limitações de Arquitetura

### 10.1 Campos Definidos - Status de Uso

| Campo | Definido em | Usado? | Notas |
|-------|-------------|--------|-------|
| `stack_dir` | `cg_arch` | Parcial | Usado indiretamente via offset_dir |
| `param_offset_base` | `cg_arch` | ✅ **SIM** | Corrigido em 2025-12-23 |
| `param_offset_dir` | `cg_arch` | ✅ **SIM** | Corrigido em 2025-12-23 |
| `local_offset_base` | `cg_arch` | ✅ **SIM** | Corrigido em 2025-12-23 |
| `local_offset_dir` | `cg_arch` | ✅ **SIM** | Adicionado em 2025-12-23 |
| `call_conv` | `cg_arch` | **NÃO** | Declarado mas não implementado |
| `align_stack` | `cg_arch` | **NÃO** | Declarado mas não implementado |
| `align_data` | `cg_arch` | **NÃO** | Declarado mas não implementado |
| `align_func` | `cg_arch` | **NÃO** | Declarado mas não implementado |

### 10.2 Macros Disponíveis

```c
// Em cgtarget.h - macros de acesso:
#define CG_STACK_DIR          (CG->arch->stack_dir)
#define CG_CALL_CONV          (CG->arch->call_conv)
#define CG_PARAM_OFFSET_BASE  (CG->arch->param_offset_base)  // ✅ USADO
#define CG_PARAM_OFFSET_DIR   (CG->arch->param_offset_dir)   // ✅ USADO
#define CG_LOCAL_OFFSET_BASE  (CG->arch->local_offset_base)  // ✅ USADO
#define CG_LOCAL_OFFSET_DIR   (CG->arch->local_offset_dir)   // ✅ USADO
```

---

## 11. Resumo: Arquiteturas e Limitações Restantes

### 11.1 Arquiteturas STACK_UP - ✅ AGORA POSSÍVEIS

Com as correções de 2025-12-23, arquiteturas com stack crescendo para cima
**AGORA PODEM** ser implementadas configurando:

```c
struct cg_arch cg_arch_s370 = {
    // ...
    STACK_UP,           /* stack_dir */
    0,                  /* param_offset_base */
    -1,                 /* param_offset_dir (negativo para STACK_UP) */
    72,                 /* local_offset_base (após save area) */
    1                   /* local_offset_dir (positivo para STACK_UP) */
};
```

### 11.2 Limitações Restantes

| Arquitetura | Limitação Restante |
|-------------|-------------------|
| IBM S/370 | Symbol transform (HLASM 8 chars), labels |
| IBM z/Architecture | Symbol transform (HLASM 8 chars), labels |
| x86-64 (ABI correto) | Não usa registradores para args (rdi, rsi...) |
| ARM (ABI correto) | Não usa r0-r3 para args |
| RISC-V | Convenção de chamada com registradores |

**Nota**: As limitações de convenção de chamada com registradores afetam
apenas a compatibilidade com ABIs padrão. O código gerado ainda funciona
corretamente usando a convenção stack-based do SubC.

---

## 12. Recomendações de Correção

### 12.1 Para Stack Direction - ✅ IMPLEMENTADO

~~1. Modificar `pmtrdecls()` para usar `CG->arch->param_offset_base` e `stack_dir`~~
~~2. Modificar `localdecls()` para usar `CG->arch->local_offset_base` e `stack_dir`~~
~~3. Criar funções helper: `calc_param_offset()`, `calc_local_offset()`~~

**Status**: Implementado em 2025-12-23. O código agora usa:
- `CG_PARAM_OFFSET_BASE` e `CG_PARAM_OFFSET_DIR` em `pmtrdecls()`
- `CG_LOCAL_OFFSET_BASE` e `CG_LOCAL_OFFSET_DIR` em `localdecls()`

### 12.2 Para va_args (PENDENTE)

1. Implementar va_args específico por arquitetura
2. Para x86-64: salvar registradores de argumento na stack no prólogo
3. Usar estrutura va_list adequada para cada ABI

### 12.3 Para Convenção de Chamada (PENDENTE)

1. Criar abstração para passagem de argumentos
2. Implementar `emit_arg_to_register()` vs `emit_arg_to_stack()`
3. Usar `CG->arch->call_conv` para decidir

### 12.4 Para Limites (PENDENTE)

1. Aumentar MAXFNARGS para pelo menos 127 (limite comum)
2. Usar alocação dinâmica para assinaturas de função
3. Considerar aumentar MAXCASE para 1024

---

## 13. LIMITAÇÃO: Transformação de Símbolos Hardcoded

### 13.1 O Problema

A função `gsym()` em `gen.c` é responsável por transformar nomes de símbolos
para o formato do assembler alvo. Porém, ela usa uma transformação **fixa**:

```c
// Em gen.c:102-108
char *gsym(char *s) {
    static char name[NAMELEN+2];

    name[0] = PREFIX;           // PREFIX = 'C' (hardcoded em defs.h)
    copyname(&name[1], s);
    return name;
}
```

### 13.2 Limitações da Implementação Atual

1. **PREFIX fixo**: Sempre adiciona 'C' antes do nome
2. **Sem transformação de case**: Não converte para maiúsculas (necessário para HLASM)
3. **Sem limite de tamanho**: HLASM limita símbolos a 8 caracteres
4. **Sem hash para nomes longos**: Nomes > 8 chars precisam ser truncados com hash
5. **Sem callback por arquitetura**: Não há como customizar por target

### 13.3 Campos Existentes mas Não Utilizados

Em `cg_os_config`:
```c
int underscore_sym;   /* Prefix symbols with underscore? */
```

Este campo existe mas **NÃO É USADO** pela função `gsym()`!

### 13.4 Solução Proposta

Adicionar callback de transformação de símbolos na estrutura `cg_arch`:

```c
// Em cgtarget.h
struct cg_arch {
    // ...
    char *(*symbol_transform)(char *s);  /* Custom symbol transformation */
};

#define CG_SYMBOL_TRANSFORM   (CG->arch->symbol_transform)
```

Modificar `gsym()`:

```c
char *gsym(char *s) {
    static char name[NAMELEN+2];

    /* Check if architecture provides custom symbol transformation */
    if (CG_SYMBOL_TRANSFORM != NULL) {
        return CG_SYMBOL_TRANSFORM(s);
    }
    
    /* When using system runtime, don't add prefix */
    if (O_sysrt) {
        copyname(name, s);
        return name;
    }
    
    /* Default transformation: PREFIX + name */
    name[0] = PREFIX;
    copyname(&name[1], s);
    return name;
}
```

### 13.5 Exemplo para S/370 (HLASM)

```c
char *s370_symbol_transform(char *s) {
    static char name[12];  /* 8 chars + null + margin */
    char *p;
    int i, len;
    unsigned int hash;
    
    /* Skip 'C' prefix if present (e.g., Cmain -> main) */
    p = s;
    if (*p == 'C' && p[1] >= 'a' && p[1] <= 'z') {
        p++;
    }
    
    /* Calculate length */
    for (len = 0; p[len]; len++);
    
    if (len <= 8) {
        /* Short enough, just convert to uppercase */
        for (i = 0; p[i] && i < 8; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                name[i] = p[i] - 'a' + 'A';
            } else {
                name[i] = p[i];
            }
        }
        name[i] = '\0';
    } else {
        /* Too long: use first 4 chars + 4-digit hash */
        hash = s370_sym_hash(p) % 10000;
        for (i = 0; i < 4 && p[i]; i++) {
            if (p[i] >= 'a' && p[i] <= 'z') {
                name[i] = p[i] - 'a' + 'A';
            } else {
                name[i] = p[i];
            }
        }
        sprintf(&name[4], "%04u", hash);
    }
    return name;
}
```

---

## 14. LIMITAÇÃO: Stack Slot Size e Parameter Offset Base

### 14.1 O Problema

O código assume que cada slot de stack tem tamanho `BPW` (Bytes Per Word),
mas algumas arquiteturas têm requisitos diferentes:

**Em `decl.c:pmtrdecls()`:**
```c
addr = 2*BPW;      // Assume offset base = 2*BPW (return addr + saved FP)
addr += BPW;       // Assume cada parâmetro ocupa exatamente BPW
```

### 14.2 Problemas para Diferentes Arquiteturas

| Arquitetura | Stack Slot | Param Offset Base | Problema |
|-------------|------------|-------------------|----------|
| i386 | 4 bytes | 8 (ret + ebp) | OK |
| x86-64 | 8 bytes | 16 (ret + rbp) | OK |
| S/370 | 4 bytes | 0 (via R11) | **ERRADO** |
| ARM | 4 bytes | 8 (r11 + lr) | Parcial |

### 14.3 Campos Necessários

Adicionar em `cg_arch`:

```c
struct cg_arch {
    // ...
    int  stack_slot_size;     /* Size of each stack slot */
    int  param_offset_base;   /* Initial offset for first parameter */
    int  param_offset_dir;    /* Direction: +1 or -1 */
    int  local_offset_base;   /* Initial offset for local variables */
    int  local_offset_dir;    /* Direction: +1 or -1 */
};
```

### 14.4 Macros de Acesso

```c
#define CG_STACK_SLOT_SIZE    (CG->arch->stack_slot_size)
#define CG_PARAM_OFFSET_BASE  (CG->arch->param_offset_base)
#define CG_PARAM_OFFSET_DIR   (CG->arch->param_offset_dir)
#define CG_LOCAL_OFFSET_BASE  (CG->arch->local_offset_base)
#define CG_LOCAL_OFFSET_DIR   (CG->arch->local_offset_dir)
```

### 14.5 Exemplo de Uso para S/370

```c
// S/370: parâmetros passados via R11 em offsets 0, 4, 8, ...
struct cg_arch cg_arch_s370 = {
    "s370",
    32,                 /* bits */
    1,                  /* char_size */
    4,                  /* int_size */
    4,                  /* ptr_size */
    4,                  /* bpw */
    ENDIAN_BIG,         /* endian */
    STACK_UP,           /* stack_dir */
    ASM_HLASM,          /* asm_syntax */
    CC_HLASM,           /* call_conv */
    FLOAT_IBM_HEX,      /* float_format */
    4,                  /* align_stack */
    4,                  /* align_data */
    8,                  /* align_func (doubleword) */
    1,                  /* has_mul */
    1,                  /* has_div */
    1,                  /* has_mod */
    1,                  /* has_byte_ops */
    1,                  /* needs_alignment */
    4,                  /* stack_slot_size */
    0,                  /* param_offset_base (R11+0) */
    1,                  /* param_offset_dir (+4, +8, ...) */
    72,                 /* local_offset_base (after save area) */
    1,                  /* local_offset_dir (+4, +8, ...) */
    s370_symbol_transform  /* symbol_transform */
};
```

### 14.6 Código Modificado em `decl.c`

```c
static int pmtrdecls(void) {
    // ...
    addr = CG_PARAM_OFFSET_BASE;
    for (;;) {
        // ...
        addloc(name, prim, type, CAUTO, size, addr, 0);
        addr += CG_PARAM_OFFSET_DIR * CG_STACK_SLOT_SIZE;
        // ...
    }
}

static int localdecls(void) {
    int addr = CG_LOCAL_OFFSET_BASE;
    // ...
    addr += CG_LOCAL_OFFSET_DIR * rsize;
    addloc(name, prim, type, CAUTO, size, addr, 0);
}
```

---

## 15. LIMITAÇÃO: Label Prefix Hardcoded

### 15.1 O Problema

O prefixo de labels (`LPREFIX = 'L'`) também é hardcoded:

```c
// Em defs.h
#define LPREFIX     'L'

// Em gen.c
fprintf(Outfile, "%c%d:\n", LPREFIX, id);  // Gera "L123:"
```

### 15.2 Problema para HLASM

HLASM tem regras específicas para labels:
- Máximo 8 caracteres
- Deve começar com letra ou @, #, $
- Labels locais podem usar formato diferente

### 15.3 Solução

Adicionar callback para geração de labels:

```c
struct cg_arch {
    // ...
    char *(*label_transform)(int id);  /* Custom label generation */
};
```

---

## 16. Resumo: Limitações Restantes

| Limitação | Arquivo | Impacto | Status |
|-----------|---------|---------|--------|
| `gsym()` hardcoded | gen.c | Não suporta HLASM (8 chars, uppercase) | PENDENTE |
| `PREFIX` fixo | defs.h | Sempre 'C', não configurável | PENDENTE |
| `LPREFIX` fixo | defs.h | Sempre 'L', não configurável | PENDENTE |
| `underscore_sym` não usado | cgtarget.h | Campo existe mas é ignorado | PENDENTE |
| Stack slot size fixo | decl.c | Assume sempre BPW | PENDENTE |
| ~~Param offset base fixo~~ | decl.c | ~~Assume sempre 2*BPW~~ | ✅ CORRIGIDO |
| ~~Local offset dir fixo~~ | decl.c | ~~Assume sempre negativo~~ | ✅ CORRIGIDO |

---

## 17. Arquiteturas Afetadas (Atualizado 2025-12-23)

| Arquitetura | Status | Limitações Restantes |
|-------------|--------|---------------------|
| IBM S/370 | ✅ Possível | symbol_transform, HLASM labels |
| IBM z/Architecture | ✅ Possível | symbol_transform, HLASM labels |
| HP PA-RISC | ✅ Possível | Nenhuma crítica |
| x86-64 (ABI correto) | Parcial | Registradores para args (usa stack) |
| ARM (ABI correto) | Parcial | Registradores para args (usa stack) |
| RISC-V | Parcial | Convenção de chamada (usa stack) |
| Qualquer HLASM | ✅ Possível | Symbol transform, label format |

**Legenda**:
- ✅ **Possível**: Pode ser implementado com as correções de stack direction
- **Parcial**: Funciona mas não segue ABI padrão (usa convenção stack-based)

