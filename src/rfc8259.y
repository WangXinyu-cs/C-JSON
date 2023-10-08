%define api.pure
%parse-param { Json *&json }
%parse-param { std::string &err }
%parse-param { yyscan_t scan }
%lex-param { yyscan_t scan }

%{

#include "json.h"
#include "parse.h"
#include "stdio.h"

void yyerror(Json *&jo, std::string &err, yyscan_t scan, const char *msg)
{
    if (!err.empty())
      err += "\n";
    err += std::string("error: ") + msg;
}

%}

%union
{
    long    integer;
    double  real;
    char    *str;
    JsonObject  *jo;
    JsonArray   *ja;
    Json        *json;
}

%token <integer> INTEGER
%token <real> REAL
%token <str>  STRING ILLEGAL
%token JNULL JTRUE JFALSE

%type <json> json
%type <jo> jsonobject objectlist
%type <ja> jsonarray arraylist

%start  json

%destructor { delete $$; } <jo> <ja>

%%

json
  : JNULL   { json = $$ = new JsonNull(); }
  | JTRUE   { json = $$ = new JsonBool(true); }
  | JFALSE  { json = $$ = new JsonBool(false); }
  | INTEGER { json = $$ = new JsonInteger($1); }
  | REAL    { json = $$ = new JsonReal($1); }
  | STRING  { json = $$ = new JsonString($1, JsonString::JSONSTR); delete $1; }
  | jsonobject  { json = $$ = $1; }
  | jsonarray   { json = $$ = $1; }
  | error { json = $$ = NULL; yyerror(json, err, scan, yyget_text(scan)); YYABORT; }
  ;

jsonobject
  : '{' '}'
    { $$ = new JsonObject(); }
  | '{' objectlist '}'
    { $$ = $2; }
  ;

objectlist
  : STRING ':' json
    { $$ = new JsonObject(); $$->add($1, $3); }
  | objectlist ','  STRING ':' json
    { $1->add($3, $5); $$ = $1; }
  ;

jsonarray
  : '[' ']'
    { $$ = new JsonArray(); }
  | '[' arraylist ']'
    { $$ = $2; }
  ;

arraylist
  : json
    { $$ = new JsonArray(); $$->add($1); }
  | arraylist ',' json
    { $1->add($3); $$ = $1; }
  ;
