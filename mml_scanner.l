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


  /* ================================================================ */
  /* ====[                      Operators                         ]==== */
  /* ================================================================ */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;
[-+?\[\]*/%<>~=]       return *yytext;

  /* ================================================================ */
  /* ====[                      Types                           ]==== */
  /* ================================================================ */

"int"                  return tTYPE_INTEGER;
"double"               return tTYPE_REAL;
"string"               return tTYPE_STRING;
"void"                 return tTYPE_VOID;

  /* ================================================================ */
  /* ====[                      Declarations                    ]==== */
  /* ================================================================ */

"auto"                 return tTYPE_AUTO;
"foreign"              return tFOREIGN;
"forward"              return tFORWARD;
"public"               return tPUBLIC;

  /* ================================================================ */
  /* ====[                      Instructions                    ]==== */
  /* ================================================================ */

"while"                return tWHILE; 
"if"                   return tIF; 
"elif"                 return tELIF;
"else"                 return tELSE; 
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;

  /* ================================================================ */
  /* ====[                      Expresions                      ]==== */
  /* ====[                      Pointers                        ]==== */
  /* ================================================================ */

"input"                return tINPUT; 
"null"                 return tNULL;
"sizeof"               return tSIZEOF;

  /* ================================================================ */
  /* ====[                      Delimiters                      ]==== */
  /* ====[                      Terminators                     ]==== */
  /* ================================================================ */

"!"                    return tPRINT;
"!!"                   return tPRINTLN;
[,;(){}\.]              return *yytext;
"begin"                return tBEGIN; 
"end"                  return tEND; 

  /* ================================================================ */
  /* ====[                      Identifier                      ]==== */
  /* ================================================================ */

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

  /* ================================================================ */
  /* ====[                      Strings                         ]==== */
  /* ================================================================ */

\"                          yy_push_state(X_STRING);
<X_STRING>\\                yy_push_state(X_SPECIAL_CHARACTER);
<X_STRING>\"                {
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                            }
<X_STRING>\n                yyerror("Error: Newline in string");
<X_STRING>\0                yyerror("Error: Null character found");
<X_STRING>.                 strlit << *yytext;

<X_SPECIAL_CHARACTER>0	            { yy_pop_state(); yy_pop_state(); yy_push_state(X_STRING_IGNORE); }
<X_SPECIAL_CHARACTER>00	            { yy_pop_state(); yy_pop_state(); yy_push_state(X_STRING_IGNORE); }
<X_SPECIAL_CHARACTER>000	        { yy_pop_state(); yy_pop_state(); yy_push_state(X_STRING_IGNORE); }
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

<X_STRING_IGNORE>\"			{
                                yylval.s = new std::string(strlit.str());
                                strlit.str("");
                                yy_pop_state();
                                return tSTRING;
                            }
<X_STRING_IGNORE>.			; /* ignore everything else */
<X_STRING_IGNORE>\n			yyerror("Error: Newline in string");

  /* ================================================================ */
  /* ====[                      Comments                        ]==== */
  /* ================================================================ */

"//".*$                ; /* ignore line comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ; /* ignore comments */

  /* ================================================================ */
  /* ====[                      Integers numbers                ]==== */
  /* ================================================================ */

0[0-7]+                try { yylval.i = std::stoi(yytext, nullptr, 8); return tINTEGER; } catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }
0[0-9]+                yyerror("Error : Invalid octal number.");

[0-9]+                 try { yylval.i = std::stoi(yytext, nullptr, 10); return tINTEGER; } catch(std::out_of_range const&) { yyerror("Error : Overflow detected"); }

  /* ================================================================ */
  /* ====[                      Real Numbers                    ]==== */
  /* ================================================================ */
  
([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?  yylval.d = strtod(yytext, NULL); return tDOUBLE;

([0-9]+)([eE][-+]?[0-9]+) yylval.d = strtod(yytext, NULL); return tDOUBLE;


  /* ================================================================ */
  /* ====[                      Other Characters                ]==== */
  /* ================================================================ */

"->"                   return tARROW;
"@"                    return tRECURSION;
[ \t\n\r]+             ; /* ignore whitespace */
.                      yyerror("Error : Unknown character"); 

  /* ================================================================ */
  /* ====[                      The End                         ]==== */
  /* ================================================================ */

%%

