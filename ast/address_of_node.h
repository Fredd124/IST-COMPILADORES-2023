#ifndef __MML_AST_ADDRESS_NODE_H__
#define __MML_AST_ADDRESS_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>

namespace mml {

  /**
   * Class for describing left value address nodes.
   */
  class address_of_node : public cdk::expression_node {

    cdk::lvalue_node *_leftValue;

  public:
    inline address_of_node(int lineno, cdk::lvalue_node *leftValue) :
        cdk::expression_node(lineno), _leftValue(leftValue) {
    }

  public:
    inline cdk::lvalue_node *leftValue() {
      return _leftValue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_of_node(this, level);
    }

  };

} // mml

#endif