#ifndef __mml_TARGET_FRAME_SIZE_CALCULATOR_H__
#define __mml_TARGET_FRAME_SIZE_CALCULATOR_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <stack>

namespace mml {

  class frame_size_calculator: public basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    std::stack<std::shared_ptr<mml::symbol>> _functions;
    int _funcCount;

    size_t _localsize;

  public:
    frame_size_calculator(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,
      std::stack<std::shared_ptr<mml::symbol>> funcs, int funcCount) :
        basic_ast_visitor(compiler), _symtab(symtab), _functions(funcs), _funcCount(funcCount), _localsize(0) {
    }

  public:
    ~frame_size_calculator();

  public:
    size_t localsize() const {
      return _localsize;
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
