#ifndef __MML_AST_STOP_NODE_H__
#define __MML_AST_STOP_NODE_H__

#include <cdk/ast/basic_node.h>

namespace mml {

  /**
   * Class for describing stop nodes.
   */
  class stop_node: public cdk::basic_node {
      int _cicleNumber;

  public:
    inline stop_node(int lineno, int cicleNumber) :
      cdk::basic_node(lineno), _cicleNumber(cicleNumber) {
    }

  public:
    inline int cicleNumber() const {
        return _cicleNumber;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_stop_node(this, level);
    }

  };

} // mml

#endif