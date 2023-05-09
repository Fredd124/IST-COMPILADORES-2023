#ifndef __MML_AST_POINTER_INDEXATION_NODE_H__
#define __MML_AST_POINTER_INDEXATION_NODE_H__

#include <cdk/ast/expression_node.h>

namespace mml {

  /**
   * Class for describing pointer indexing nodes.
   */
  class pointer_indexation_node : public cdk::lvalue_node {
    cdk::expression_node *_basePos, *_index;

  public:
    inline pointer_indexation_node(int lineno, cdk::expression_node *basePos,
        cdk::expression_node *index) : 
      cdk::lvalue_node(lineno), _basePos(basePos), _index(index) {
    }

  public:
    inline cdk::expression_node *basePos() {
      return _basePos;
    }

    inline cdk::expression_node *index() {
      return _index;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_pointer_indexation_node(this, level);
    }

  };

} // mml

#endif