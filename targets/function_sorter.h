#ifndef __mml_TARGET_FUNCTION_SORTER_H__
#define __mml_TARGET_FUNCTION_SORTER_H__

#include "targets/basic_ast_visitor.h"
#include <vector>
#include <stack>

namespace mml {

  class function_sorter: public basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    std::stack<std::shared_ptr<mml::symbol>> _functions;
    int _funcCount;
    std::vector<mml::function_definition_node *> _functions_to_define;

  public:
    function_sorter(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,
      std::stack<std::shared_ptr<mml::symbol>> funcs, int funcCount) :
        basic_ast_visitor(compiler), _symtab(symtab), _functions(funcs), _funcCount(funcCount) {
    }

  public:
    ~function_sorter();

  public:
    std::vector<mml::function_definition_node *> functions_to_define() {
      return _functions_to_define;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // mml

#endif

