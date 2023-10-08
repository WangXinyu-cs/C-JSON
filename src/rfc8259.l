%option noyywrap nodefault
%option reentrant bison-bridge

%{
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "rfc8259.tab.h"
%}

EXP     ([Ee][-+]?[0-9]+)

%%

"{" |
"}" |
"[" |
"]" |
":" |
","     { return yytext[0]; }

null    { return JNULL; }
true    { return JTRUE; }
false   { return JFALSE; }

-?[0-9]+    { yylval->integer = atol(yytext); return INTEGER; }

-?[0-9]+\.[0-9]*{EXP}? |
-?\.?[0-9]+{EXP}?   { yylval->real = atof(yytext); return REAL; }

\"([^"]|\\\")*\"    { yylval->str = strndup(yytext + 1, yyleng - 2); return STRING; }

[ \t\r\n]+  /* ignore whitespace */

.   { return ILLEGAL; }
