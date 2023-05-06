#ifndef __MML_AST_ADDRESS_NODE_H__
#define __MML_AST_ADDRESS_NODE_H__

#include <cdk/ast/expression_node.h>

namespace mml {

  /**
   * Class for describing left value address nodes.
   */
  class address_node : public cdk::expression_node {

    cdk::lvalue_node * _leftValue;

  public:
    inline address_node(int lineno) :
        cdk::expression_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }

  };

} // mml

#endif