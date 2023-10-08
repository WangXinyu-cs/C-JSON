#ifndef _PARSE_H_
#define _PARSE_H_

typedef void* yyscan_t;
typedef struct yy_buffer_state* YY_BUFFER_STATE;

#include "json.h"
#include "rfc8259.tab.h"

/**
 * defined in xxx.lex.c generated from xxx.l
 */

int yylex_init(yyscan_t* scan);
int yylex_destroy(yyscan_t scan);

int yylex(YYSTYPE *yylval_param, yyscan_t scan);
YY_BUFFER_STATE yy_scan_string(const char *, yyscan_t);
void yy_delete_buffer(YY_BUFFER_STATE, yyscan_t);
char *yyget_text(yyscan_t);

/**
 * defined in xxx.tab.c generated from xxx.y
 */
int yyparse(Json* &jo, std::string &err, yyscan_t scan);

/**
 * called by yyparse() in xxx.tab.c but defined by user,
 * it's parameter list the same as yyarse() in addition to another formatted char*
 */
void yyerror(Json* &jo, std::string &err, yyscan_t scan, const char *msg);

#endif