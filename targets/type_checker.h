#ifndef __MML_TARGETS_TYPE_CHECKER_H__
#define __MML_TARGETS_TYPE_CHECKER_H__

#include "targets/basic_ast_visitor.h"
#include <cdk/types/reference_type.h>
#include <stack>

namespace mml {

  /**
   * Print nodes as XML elements to the output stream.
   */
  class type_checker: public basic_ast_visitor {
    cdk::symbol_table<mml::symbol> &_symtab;
    std::stack<std::shared_ptr<mml::symbol>> _functions;
    int _funcCount;
    basic_ast_visitor *_parent;

  public:
    type_checker(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<mml::symbol> &symtab,  
      std::stack<std::shared_ptr<mml::symbol>> funcs, int funcCount, basic_ast_visitor *parent) :
        basic_ast_visitor(compiler), _symtab(symtab), _functions(funcs), _funcCount(funcCount), _parent(parent) {
    }

  public:
    ~type_checker() {
      os().flush();
    }

  protected:
    void processIntBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void processIntDoubleBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void processIntDoublePointerBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    bool processFunctionalType(std::shared_ptr<cdk::functional_type> leftPtr, std::shared_ptr<cdk::functional_type> rightPtr);
    std::shared_ptr<cdk::basic_type> processPointerForBoolean(std::shared_ptr<cdk::reference_type> leftPtr, std::shared_ptr<cdk::reference_type> rightPtr);
    std::shared_ptr<cdk::basic_type> processPointer(std::shared_ptr<cdk::reference_type> leftPtr, std::shared_ptr<cdk::reference_type> rightPtr);
    std::shared_ptr<cdk::basic_type> processAssignmentPointer(std::shared_ptr<cdk::reference_type> leftPtr, std::shared_ptr<cdk::reference_type> rightPtr);
    void processIntUnaryExpression(cdk::unary_operation_node *const node, int lvl);
    void processIntDoubleUnaryExpression(cdk::unary_operation_node *const node, int lvl);
    void processBinaryExpression(cdk::binary_operation_node *const node, int lvl);
    void assertCallParameters(mml::function_call_node * const node, std::shared_ptr<mml::symbol> function_symbol, int lvl);
    template<typename T>
    void process_literal(cdk::literal_node<T> *const node, int lvl) {
    }

  public:
    // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
    // do not edit these lines: end

  };

} // mml

//---------------------------------------------------------------------------
//     HELPER MACRO FOR TYPE CHECKING
//---------------------------------------------------------------------------

#define CHECK_TYPES(compiler, symtab, functions, funcCount, node) { \
  try { \
    mml::type_checker checker(compiler, symtab, functions, funcCount, this ); \
    (node)->accept(&checker, 0); \
  } \
  catch (const std::string &problem) { \
    std::cerr << (node)->lineno() << ": " << problem << std::endl; \
    return; \
  } \
}

#define ASSERT_SAFE_EXPRESSIONS CHECK_TYPES(_compiler, _symtab, _functions, _funcCount, node)

#endif
