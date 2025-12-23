# SubC Compiler - Limitações Pendentes

## Versão: 2025-12-23

Este documento lista as limitações ainda existentes no compilador SubC.

---

## 1. Constantes de Limite em `defs.h`

```c
#define TEXTLEN     512      // Tamanho máximo de texto/token
#define NAMELEN     32       // Tamanho máximo de identificadores
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

## 3. Limitações de Linguagem

### 3.1 Máximo de 2 Níveis de Indireção

```c
// Válido: int *, int **
// Inválido: int ***
```

### 3.2 Arrays Apenas 1D

Não suporta `int a[10][20]`

### 3.3 Sem goto

O keyword `goto` não é reconhecido.

---

## 4. Campos de Arquitetura Não Utilizados

| Campo | Definido em | Status |
|-------|-------------|--------|
| `align_stack` | `cg_arch` | Declarado mas não implementado |
| `align_data` | `cg_arch` | Declarado mas não implementado |
| `align_func` | `cg_arch` | Declarado mas não implementado |

---

## 5. Label Prefix Hardcoded

O prefixo de labels (`LPREFIX = 'L'`) é hardcoded em `defs.h`:

```c
#define LPREFIX     'L'
```

Para arquiteturas como HLASM que têm regras específicas para labels
(máximo 8 caracteres), seria necessário um callback `label_transform`.

---

## 6. Struct/Union por Valor

O SubC não suporta passagem de struct/union por valor em parâmetros de função
nem retorno de struct/union por valor.

```c
/* NÃO SUPORTADO */
void print_point(struct point p);      /* erro na declaração */
struct point get_origin(void);         /* erro no retorno */

/* SUPORTADO - usar ponteiros */
void print_point(struct point *p);     /* OK */
void get_origin(struct point *result); /* OK */
```

**Localização**: `src/decl.c` na função `pmtrdecls()`

**Motivo**: Implementar passagem por valor requer:
- Cópia de estrutura na pilha (memcpy implícito)
- Conhecimento do tamanho da estrutura em tempo de chamada
- Convenções de chamada específicas por arquitetura

---

## 7. Cast de Ponteiro para Inteiro

O SubC não suporta cast direto de ponteiro para inteiro em plataformas de 64 bits.
Isso requer um workaround com macro condicional:

```c
#ifdef __SUBC__
 #define PTR_INT_CAST	(int)
#else
 #define PTR_INT_CAST	(int) (long)
#endif
```

**Problema**: Em compiladores padrão C (gcc, clang), converter um ponteiro de 64 bits
diretamente para `int` (32 bits) gera warning/erro. A conversão correta é
`ponteiro → long → int`. Porém, o SubC não aceita essa sintaxe de cast encadeado.

**Localização**: `src/sym.c` na função `galloc()`

**Solução futura**: Implementar suporte a cast encadeado `(tipo1)(tipo2)expr` ou
adicionar tipo `intptr_t` / `uintptr_t` para conversões ponteiro↔inteiro.

---

## 8. Resumo das Limitações

| Limitação | Severidade | Arquivo |
|-----------|------------|---------|
| _va_arg aritmética de ponteiro | Médio | varargs.c |
| Struct/union por valor | Médio | decl.c |
| Cast ponteiro→inteiro (64-bit) | Médio | sym.c |
| `LPREFIX` fixo | Baixo | defs.h |
| Campos align_* não usados | Baixo | cgtarget.h |
| Máximo 2 níveis de indireção | Baixo | decl.c |
| Arrays apenas 1D | Baixo | decl.c |
| Sem goto | Baixo | - |
