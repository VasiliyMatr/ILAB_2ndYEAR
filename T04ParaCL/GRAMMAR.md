
## Brief

This file describes ParaCL language grammar.

---

## ParaCL terminals
```

ident := [A-z_][A-z0-9]* .
int-imm := [1-9][0-9]* .

while := 'while' .
input := '?'
output := 'print'

open-round-br := '(' .
close-round-br := ')' .
open-figure-br := '{' .
close-figure-br := '}' .

semicolon := ';' .

assign := '=' .
logic-op := '>' | '<' | '==' | '||' | '&&' .
pm-op := '+' | '-' .
md-op := '*' | '/' .

```
---

## ParaCL grammar

```

PROGRAMM ->
    INSTRUCTION_BLOCK* .

INSTRUCTION_BLOCK ->
    CYCLE |
    INSTRUCTION .

CYCLE ->
    WHILE .

WHILE ->
    while open-round-br EXPR close-round-br
    open-figure-br
        INSTRUCTION_BLOCK*
    close-figure-br .

INSTRUCTION ->
    PRINT semicolon |
    EXPR semicolon .

PRINT ->
    output EXPR .

EXPR ->
    ASSIGN_EXPR |
    LOGIC_EXPR .

ASSIGN_EXPR ->
    ident assign EXPR .

LOGIC_EXPR ->
    PLUS_MINUS_EXPR LOGIC_REC .

LOGIC_REC ->
    logic-op LOGIC_EXPR |
     .

PLUS_MINUS_EXPR ->
    MUL_DIV_EXPR PLUS_MINUS_REC .

PLUS_MINUS_REC ->
    pm-op MUL_DIV_EXPR |
     .

MUL_DIV_EXPR ->
    VALUE MUL_DIV_REC .

MUL_DIV_REC ->
    md-op MUL_DIV_EXPR |
     .

VALUE ->
    input |
    ident |
    int-imm |
    '(' EXPR ')' .

```
---