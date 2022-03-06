
%option c++
%option yyclass="Lexer"

%{
    #include "lexer.hh"
    
    using std::cin;
    using std::cout;
    using std::cerr;
%}

ws              [ \t\n\v]+

ident           [A-Za-z][A-Za-z0-9]*
int_num         [1-9][0-9]*

while           "while"
input           "?"
output          "print"

brace           [\(\)\{\}]

semicolon       ';'

assign          "="
logic_op        ("<"|">"|"=="|"||"|"&&")
pm_op           [\+\-]
md_op           [\*\/]

%%

{ws}

{pm_op}             {std::cout << "pm " << yytext << " "; return 1;}
{md_op}             {std::cout << "md " << yytext << " "; return 1;}
{assign}            {std::cout << "asgn " << yytext << " "; return 1;}
{logic_op}          {std::cout << "logic " << yytext << " "; return 1;}

{while}             {std::cout << "while " << yytext << " "; return 1;}
{input}             {std::cout << "input " << yytext << " "; return 1;}
{int_num}           {std::cout << "int " << yytext << " "; return 1;}
{ident}             {std::cout << "ident " << yytext << " "; return 1;}

{brace}             {std::cout << "brace " << yytext[0] << " "; return 1;}
{semicolon}         {std::cout << "; " << yytext << " "; return 1;}

.                   {std::cout << "UNDEF "; return 1;}

%%
