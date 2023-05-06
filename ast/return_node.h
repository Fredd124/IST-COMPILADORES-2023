#ifndef __MML_AST_RETURN_NODE_H__
#define __MML_AST_RETURN_NODE_H__

#include <cdk/ast/expression_node.h>

namespace mml {

  /**
   * Class for describing return nodes.
   */
  class return_node: public cdk::expression_node {
    cdk::expression_node *_returnval;

  public:
    inline return_node(int lineno, cdk::expression_node *returnval) :
        cdk::expression_node(lineno), _returnval(returnval) {
    }

  public:
    inline cdk::expression_node *returnval() {
      return _returnval;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // mml

#endif