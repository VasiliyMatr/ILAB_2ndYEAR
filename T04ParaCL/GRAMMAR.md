
## ParaCL grammar:

```

EXPR ->
    ASSIGN_EXPR |
    LOGIC_EXPR

IDENT -> [A-z_][A-z0-9]* .
INT_NUM -> [1-9][0-9]* .

VALUE ->
    IDENT |
    INT_NUM |
    '(' EXPR ')' .

ASSIGN_EXPR ->
    IDENT ASSIGN_OP EXPR .

ASSIGN_OP -> '=' .

LOGIC_EXPR ->
    PLUS_MINUS_EXPR LOGIC_REC .

LOGIC_REC ->
    LOGIC_OP LOGIC_EXPR |
     .

LOGIC_OP -> '>' | '<' | '==' | '||' | '&&' .

PLUS_MINUS_EXPR ->
    MUL_DIV_EXPR PLUS_MINUS_REC .

PLUS_MINUS_REC ->
    PLUS_MINUS_OP MUL_DIV_EXPR |
     .

PLUS_MINUS_OP -> '+' | '-' .

MUL_DIV_EXPR ->
    VALUE MUL_DIV_REC .

MUL_DIV_REC ->
    MUL_DIV_OP MUL_DIV_EXPR |
     .

MUL_DIV_OP -> '*' | '/' .

```