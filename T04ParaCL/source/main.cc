
#include <iostream>
#include "../headers/lexer.hh"

extern "C" int yywrap()
{
    return 1;
}

int main()
{
    FlexLexer *lex = new Lexer;
    while (lex->yylex() != 0)
        ;

    return 0;
}