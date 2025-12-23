# SubC Compiler - Limitações Pendentes

## Versão: 2025-12-23 (atualizado)

Este documento lista as limitações ainda existentes no compilador SubC.

---

## 0. Status do Suporte a Ponto Flutuante

**IMPLEMENTADO** (2025-12-23)

A infraestrutura de código para ponto flutuante foi completamente implementada:

| Componente | Status |
|------------|--------|
| `cg_fpu_type` enum | ✅ Implementado |
| `fpu_type` em `cg_arch` | ✅ Implementado |
| 46 funções FP na vtable | ✅ Definidas |
| 8086 x87 (8087) | ✅ Implementado |
| 8086 emulação IEEE 754 | ✅ Implementado (float e double) |
| i386 x87 | ✅ Implementado |
| x86-64 SSE2 | ✅ Implementado |
| ARM VFP | ✅ Implementado |
| Integração em expr.c/gen.c | ⏳ Pendente |

**Próximos passos:**
1. Integrar chamadas FP em `expr.c` e `gen.c` usando `IS_FLOATTYPE()`

---

## 1. Constantes de Limite em `defs.h`

```c
#define TEXTLEN     512      // Tamanho máximo de texto/token
#define NAMELEN     16       // Tamanho máximo de identificadores
#define MAXFILES    32       // Máximo de arquivos de entrada
#define MAXINCDIRS  16       // Máximo de diretórios de include
#define MAXIFDEF    16       // Profundidade máxima de #ifdef aninhados
#define MAXNMAC     32       // Máximo de macros em expansão simultânea
#define MAXCASE     1024     // Máximo de cases em um switch
#define MAXBREAK    64       // Profundidade máxima de loops/switches aninhados
#define MAXLOCINIT  128      // Máximo de inicializadores locais
#define MAXFNARGS   127      // Máximo de argumentos por função
#define NSYMBOLS    1024     // Máximo de símbolos na tabela
#define POOLSIZE    16384    // Tamanho do pool de nomes
#define NODEPOOLSZ  4096     // Tamanho do pool de nós AST
```

---

## 2. Implementação de va_args

### 2.1 Problema: Aritmética de ponteiro incorreta

```c
void *_va_arg(void **ap) {
    return *((void **) *ap)++;  // PROBLEMA: incrementa *ap, não ap
}
```

O código incrementa o valor apontado por `*ap`, não o ponteiro `ap` em si.

### 2.2 Problema: Tamanho de argumento fixo

- Assume que todos os argumentos têm tamanho `sizeof(void*)`
- Não funciona para tipos menores (char, short) que são promovidos

### 2.3 Problema: Alinhamento

- Não considera alinhamento de stack
- x86-64 requer alinhamento de 16 bytes para a stack

---

## 3. Aritmética de Ponto Flutuante

**RESOLVIDO** - Ver seção 0 acima.

Os tipos `float` e `double` são parseados e armazenados corretamente.
A infraestrutura de code generation foi completamente implementada (vtable com 46 funções).

**Implementado:**
- 8086: Suporte completo a x87 (8087/80287)
- 8086: Emulação IEEE 754 em software (float e double)
- i386: Suporte completo a x87 FPU
- x86-64: Suporte completo a SSE2
- ARM: Suporte completo a VFP
- Targets: `dos-8086` (emulação), `dos-8086-x87` (hardware), todos os outros

**Pendente:**
- Integração no parser (expr.c/gen.c)

---

## 4. Limitações de Linguagem

### 4.1 Máximo de 2 Níveis de Indireção

```c
// Válido: int *, int **
// Inválido: int ***
```

### 4.2 Arrays Apenas 1D

Não suporta `int a[10][20]`

### 4.3 Sem goto

O keyword `goto` não é reconhecido.

---

## 5. Campos de Arquitetura Não Utilizados

| Campo | Definido em | Status |
|-------|-------------|--------|
| `align_stack` | `cg_arch` | Declarado mas não implementado |
| `align_data` | `cg_arch` | Declarado mas não implementado |
| `align_func` | `cg_arch` | Declarado mas não implementado |

---

## 6. Label Prefix Hardcoded

O prefixo de labels (`LPREFIX = 'L'`) é hardcoded em `defs.h`:

```c
#define LPREFIX     'L'
```

Para arquiteturas como HLASM que têm regras específicas para labels
(máximo 8 caracteres), seria necessário um callback `label_transform`.

---

## 7. Resumo das Limitações Pendentes

| Limitação | Severidade | Arquivo | Status |
|-----------|------------|---------|--------|
| Integração FP em expr.c/gen.c | Médio | expr.c, gen.c | Pendente |
| FP para i386/x86-64/ARM | Médio | cg_*.c | ✅ Resolvido |
| Emulação double 8086 | Médio | fpemu.c | ✅ Resolvido |
| _va_arg aritmética de ponteiro | Médio | varargs.c | Bug |
| `LPREFIX` fixo | Baixo | defs.h | - |
| Campos align_* não usados | Baixo | cgtarget.h | - |
| Máximo 2 níveis de indireção | Baixo | decl.c | Design |
| Arrays apenas 1D | Baixo | decl.c | Design |
| Sem goto | Baixo | - | Design |
