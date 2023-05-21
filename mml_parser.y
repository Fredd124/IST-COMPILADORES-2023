%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;    /* double value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;

  std::vector<std::shared_ptr<cdk::basic_type>> *vector;
  
  mml::block_node       *block;
};

%token <i> tINTEGER
%token <d> tDOUBLE
%token <s> tIDENTIFIER tSTRING
%token tPUBLIC tPRIVATE tFOREIGN tFORWARD
%token tTYPE_STRING tTYPE_INTEGER tTYPE_REAL tTYPE_AUTO tTYPE_VOID
%token tWHILE tIF tINPUT tBEGIN tEND tNEXT tSTOP tPRINTLN tRETURN tSIZEOF tNULL 

%type <node> instruction iffalse
%type <sequence> file instructions opt_instrs 
%type <sequence> exprs
%type <expression> expr program opt_initializer
%type <lvalue> lval
%type <block> block

%type <node> declaration vardec
%type <sequence> opt_decls declarations;
%type <vector> data_types;
%type <s> string
%type <i> opt_integer
%type<type> data_type data_type_with_auto function_type

%nonassoc tIFX
%nonassoc tELIF tELSE

%right '='
%left tOR
%left tAND
%right '~'
%left tNE tEQ
%left '<' tLE tGE '>'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : opt_decls         { compiler->ast( $$ = new cdk::sequence_node(LINE, $1)); }     
     | opt_decls program { compiler->ast( $$ = new cdk::sequence_node(LINE, $2, $1)); }
     ;

program : tBEGIN opt_decls opt_instrs tEND {$$ = new mml::function_definition_node(LINE, tPRIVATE, cdk::primitive_type::create(4, cdk::TYPE_INT), new cdk::sequence_node(LINE), new mml::block_node(LINE, $3, $3) , true); } ;

opt_instrs     : /* empty */   { $$ = new cdk::sequence_node(LINE); }
               | instructions { $$ = $1; }
               ;

instructions   : instruction	               { $$ = new cdk::sequence_node(LINE, $1); }
	          | instructions instruction    { $$ = new cdk::sequence_node(LINE, $2, $1); }
	          ;

instruction    : expr ';'                                        { $$ = new mml::evaluation_node(LINE, $1); }
               | exprs '!'                                       { $$ = new mml::print_node(LINE, $1, false); }
               | exprs '!''!'                                    { $$ = new mml::print_node(LINE, $1, true); }
               | tWHILE '(' expr ')' instruction                 { $$ = new mml::while_node(LINE, $3, $5); }
               | tIF '(' expr ')' instruction %prec tIFX         { $$ = new mml::if_node(LINE, $3, $5); }
               | tIF '(' expr ')' instruction iffalse            { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
               | tNEXT opt_integer ';'                           { $$ = new mml::next_node(LINE, $2); }
               | tSTOP opt_integer ';'                           { $$ = new mml::stop_node(LINE, $2); }
               | block                                           { $$ = $1; }
               | tRETURN expr ';'                                { $$ = new mml::return_node(LINE, $2); }
               ;

iffalse   : tELSE instruction                                               { $$ = $2; }  
          | tELIF '(' expr ')' instruction            %prec tELIF           { $$ = new mml::if_node(LINE, $3, $5); }           
          | tELIF '(' expr ')' instruction iffalse                          { $$ = new mml::if_else_node(LINE, $3, $5, $6); }
          ;

opt_integer    : /*empty*/  { $$ = 1; }
               | tINTEGER   { $$ = $1; }
               ;

opt_decls : /* empty */ { $$ = new cdk::sequence_node(LINE); }
          | declarations { $$ = $1; }
          ;

block     : '{' opt_decls opt_instrs '}'                         { $$ = new mml::block_node(LINE, $2, $3); }

declarations   : declaration              { $$ = new cdk::sequence_node(LINE, $1);     }
               | declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
               ;

declaration    : vardec { $$ = $1; }
               ;

vardec    : tFORWARD data_type tIDENTIFIER ';'                                  { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, $2, *$3, nullptr); }
          | tPUBLIC data_type_with_auto tIDENTIFIER opt_initializer ';'         { $$ = new mml::variable_declaration_node(LINE, tPUBLIC, $2, *$3, $4); }
          | data_type_with_auto tIDENTIFIER opt_initializer ';'                 { $$ = new mml::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $3); }
          ;

opt_initializer     : /* empty */  { $$ = NULL; }
                    | '=' expr     { $$ = $2; }
                    ;

data_type : tTYPE_STRING      { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
          | tTYPE_INTEGER     { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT);   }
          | tTYPE_REAL        { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
          | tTYPE_VOID        { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
          | '[' data_type ']' { $$ = cdk::reference_type::create(4, $2); }
          | function_type     { $$ = cdk::reference_type::create(4, $1); }
          ;

data_type_with_auto : data_type    { $$ = $1; }
                    | tTYPE_AUTO   { $$ = cdk::reference_type::create(4, nullptr); }
                    ;

function_type       : data_type '<' '>'                { $$ = cdk::functional_type::create($1) ;}
                    | data_type '<' data_types '>'     { $$ = cdk::functional_type::create(*$3, $1); }
                    ;

data_types          : data_type                        { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }
                    | data_types data_type             { $$ = $1; $$->push_back($2); }

exprs     : expr                   { $$ = new cdk::sequence_node(LINE, $1);     }
          | exprs ',' expr         { $$ = new cdk::sequence_node(LINE, $3, $1); }

expr : tINTEGER                    { $$ = new cdk::integer_node(LINE, $1); }
     | tDOUBLE                     { $$ = new cdk::double_node(LINE, $1);}
     | string                      { $$ = new cdk::string_node(LINE, $1); }
     | tNULL                       { $$ = new mml::null_node(LINE); }
     | tINPUT                      { $$ = new mml::input_node(LINE); }
     | '-' expr %prec tUNARY       { $$ = new cdk::neg_node(LINE, $2); }
     | '+' expr %prec tUNARY       { $$ = new mml::identity_node(LINE, $2); }
     | expr '+' expr	          { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	          { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	          { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	          { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	          { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr	          { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	          { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	          { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr               { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	          { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	          { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tAND expr              { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr               { $$ = new cdk::or_node(LINE, $1, $3); }
     | '~' expr                    { $$ = new cdk::not_node(LINE, $2); }
     | tSIZEOF '(' expr ')'        { $$ = new mml::sizeof_node(LINE, $3); }
     | lval '?'                    { $$ = new mml::address_of_node(LINE, $1); }
     | '(' expr ')'                { $$ = $2; }
     | lval                        { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
     | lval '=' expr               { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

string    : tSTRING                  { $$ = $1; }
          | string tSTRING           { $$ = $1; $$->append(*$2); delete $2; }
          ;

lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     ;

%%
