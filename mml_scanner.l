%option c++ prefix="mml_scanner_" outfile="mml_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "mml_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT X_SPECIAL_CHARACTER X_STRING_IGNORE

%%
  yydebug=1; set_debug(1);


">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;
"!!"                   return tPRINTLN;
"->"                   return tARROW;
"@"                    return tRECURSION;

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

\"                          yy_push_state(X_STRING);
<X_STRING>\\                yy_push_state(X_SPECIAL_CHARACTER);
<X_STRING>\"                {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                            }
<X_STRING>.                 strlit << *yytext;
<X_STRING>\n                yyerror("Error in string");

<X_SPECIAL_CHARACTER>n              { strlit << '\n'; yy_pop_state(); }
<X_SPECIAL_CHARACTER>r              { strlit << '\r'; yy_pop_state(); }
<X_SPECIAL_CHARACTER>t              { strlit << '\t'; yy_pop_state(); }
<X_SPECIAL_CHARACTER>\"             { strlit << '"'; yy_pop_state();  }
<X_SPECIAL_CHARACTER>\\             { strlit << '\\'; yy_pop_state(); }
<X_SPECIAL_CHARACTER>[0-7]{1,3} {
                                    strlit << (char)(unsigned char)std::stoul(yytext, NULL, 8);
                                    yy_pop_state();
                                }
<X_SPECIAL_CHARACTER>.			    { strlit << *yytext; yy_pop_state(); }


"//".*$                ; /* ignore line comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>.|\n        ; /* ignore comments */

0[0-7]+                try { yylval.i = std::stoi(yytext, nullptr, 8); return tINTEGER; } catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }
0[0-9]+                yyerror("Error : invalid octal number.");

[0-9]+                 try { yylval.i = std::stoi(yytext, nullptr, 10); return tINTEGER; } catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?  yylval.d = strtod(yytext, NULL); return tDOUBLE;
([0-9]+)([eE][-+]?[0-9]+) yylval.d = strtod(yytext, NULL); return tDOUBLE;

[-()<>=+*/%;{}.?,!\[\]@~]       return *yytext;

[ \t\n\r]+               ; /* ignore whitespace */

.                      yyerror("Error : Unknown character");

%%

