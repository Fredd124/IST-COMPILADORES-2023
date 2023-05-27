%option c++ prefix="mml_scanner_" outfile="mml_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "mml_parser.tab.h"

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT
%%
  yydebug=1; set_debug(1);


">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;

"auto"                 return tTYPE_AUTO;
"int"                  return tTYPE_INTEGER;
"double"               return tTYPE_REAL;
"string"               return tTYPE_STRING;
"void"                 return tTYPE_VOID;

"null"                 return tNULL;

"foreign"              return tFOREIGN;
"forward"              return tFORWARD;
"public"               return tPUBLIC;

"while"                return tWHILE; 
"if"                   return tIF; 
"elif"                 return tELIF;
"else"                 return tELSE; 
"input"                return tINPUT; 
"next"                 return tNEXT;
"stop"                 return tSTOP;
"begin"                return tBEGIN; 
"end"                  return tEND; 
"return"               return tRETURN;
"sizeof"               return tSIZEOF;

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("Error : newline in string.");

"//".*$                ; /* ignore line comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ; /* ignore comments */

0[0-7]+                try { yylval.i = std::stoi(yytext, nullptr, 8);} catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }
0[0-9]+                yyerror("Error : invalid octal number.");

[0-9]+                 try { yylval.i = std::stoi(yytext, nullptr, 10);} catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?  yylval.d = strtod(yytext, NULL); return tDOUBLE;

[-()<>=+*/%;{}.?,!\[\]@~]       return *yytext;

[ \t\n\r]+               ; /* ignore whitespace */

.                      yyerror("Error : Unknown character");

%%
