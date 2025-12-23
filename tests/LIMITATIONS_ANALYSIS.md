# SubC Compiler - Limitações Pendentes

## Versão: 2025-12-23

Este documento lista as limitações ainda existentes no compilador SubC.

---

## 1. Implementação de va_args

### 1.1 Problema: Aritmética de ponteiro incorreta

```c
void *_va_arg(void **ap) {
    return *((void **) *ap)++;  // PROBLEMA: incrementa *ap, não ap
}
```

O código incrementa o valor apontado por `*ap`, não o ponteiro `ap` em si.

### 1.2 Problema: Tamanho de argumento fixo

- Assume que todos os argumentos têm tamanho `sizeof(void*)`
- Não funciona para tipos menores (char, short) que são promovidos

### 1.3 Problema: Alinhamento

- Não considera alinhamento de stack
- x86-64 requer alinhamento de 16 bytes para a stack

---

## 2. Limitações de Linguagem

### 2.1 Máximo de 2 Níveis de Indireção

```c
// Válido: int *, int **
// Inválido: int ***
```

### 2.2 Arrays Apenas 1D

Não suporta `int a[10][20]`

### 2.3 Sem goto

O keyword `goto` não é reconhecido.

---

## 3. Campos de Arquitetura Não Utilizados

| Campo | Definido em | Status |
|-------|-------------|--------|
| `align_stack` | `cg_arch` | Declarado mas não implementado |
| `align_data` | `cg_arch` | Declarado mas não implementado |
| `align_func` | `cg_arch` | Declarado mas não implementado |

---

## 4. Struct/Union por Valor

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

## 5. Cast de Ponteiro para Inteiro

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

## 6. Bootstrap (Auto-compilação)

O SubC atualmente **não consegue se auto-compilar** devido a refatorações no código
que introduziram features não suportadas pelo próprio SubC:

### 6.1 Ponteiros de Função Tipados

O `cgtarget.h` usa vtables com ponteiros de função tipados:

```c
struct cg_vtable {
    void (*cgdata)(void);      /* SubC só aceita int (*)() */
    void (*cgtext)(void);
    void (*cglit)(int v);      /* parâmetros tipados não suportados */
    ...
};
```

O SubC original só suporta `int (*)()` para ponteiros de função.

### 6.2 Arquivos Necessários

O Makefile original compilava arquivos mais simples:
- `cg386.c` (code generator único)
- Sem vtables, sem cross-compilation

A arquitetura atual usa:
- `cgtarget.c` + `targets/arch/*/cg_*.c`
- Vtables para seleção de target em runtime

### 6.3 Solução Futura

Para restaurar o bootstrap, seria necessário:

1. **Opção A**: Modificar `cgtarget.h` para usar `int (*)()` em todos os ponteiros
2. **Opção B**: Manter uma versão "bootstrap" separada com code generator único
3. **Opção C**: Implementar suporte a ponteiros de função tipados no SubC

---

## 7. Resumo das Limitações

| Limitação | Severidade | Arquivo |
|-----------|------------|---------|
| Bootstrap não funciona | Alto | cgtarget.h |
| _va_arg aritmética de ponteiro | Médio | varargs.c |
| Struct/union por valor | Médio | decl.c |
| Cast ponteiro→inteiro (64-bit) | Médio | sym.c |
| Campos align_* não usados | Baixo | cgtarget.h |
| Máximo 2 níveis de indireção | Baixo | decl.c |
| Arrays apenas 1D | Baixo | decl.c |
| Sem goto | Baixo | - |
