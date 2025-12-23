# SubC Compiler - Limitações Pendentes

## Versão: 2025-12-23

Este documento lista as limitações ainda existentes no compilador SubC.

---

## 1. Bootstrap (Auto-compilação)

### 1.1 Includes do Sistema

O SubC não consegue compilar arquivos que incluem headers do sistema:
- `stdlib.h`, `stdio.h`, `string.h`, `ctype.h`, `stddef.h`

Para bootstrap, seria necessário usar os headers próprios do SubC em `runtime/include/`.

### 1.2 Macros com Chamadas via Ponteiro

O `cgen_compat.h` usa macros como:
```c
#define cgdata()  (CG->vtable->cgdata())
```

Isso requer parsing de expressões complexas com `->` encadeado e chamada de função.

### 1.3 Retorno de Ponteiro de Função

O SubC trata todos os ponteiros de função como retornando `int` internamente.
Atribuições como `char *str = vtable.fn_retstr("x");` geram erro de tipo.

---

## 2. Limitações de Linguagem (Design)

Estas são limitações intencionais do SubC, não bugs:

| Limitação | Descrição |
|-----------|-----------|
| Máximo 2 níveis de indireção | `int **` válido, `int ***` inválido |
| Arrays apenas 1D | `int a[10]` válido, `int a[10][20]` inválido |
| Sem goto | Keyword `goto` não reconhecido |
| Struct/union por valor | Usar ponteiros: `void fn(struct x *p)` |

---

## 3. Campos de Alinhamento (Futuro)

| Campo | Status |
|-------|--------|
| `align_stack` | Definido, uso futuro para arquiteturas com requisitos rigorosos |
| `align_data` | Definido, uso futuro para alinhamento de dados |
| `align_func` | Definido, uso futuro para alinhamento de funções |

**Prioridade**: Baixa - as arquiteturas atuais (x86, ARM) funcionam sem isso.
Necessário para IBM S/370 / z/Architecture.

---

## 4. Resumo

| Limitação | Severidade | Status |
|-----------|------------|--------|
| Bootstrap (includes) | Alta | Pendente |
| Bootstrap (macros vtable) | Alta | Pendente |
| Retorno fn ptr != int | Média | Pendente |
| Campos align_* | Baixa | Futuro |
| 2 níveis indireção | Baixa | Design |
| Arrays 1D | Baixa | Design |
| Sem goto | Baixa | Design |
| Struct por valor | Baixa | Design |
