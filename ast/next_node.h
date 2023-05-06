#ifndef __MML_AST_NEXT_NODE_H__
#define __MML_AST_NEXT_NODE_H__

#include <cdk/ast/basic_node.h>

namespace mml {

  /**
   * Class for describing next nodes.
   */
  class next_node: public cdk::basic_node {
    int _cicleNumber;        

  public:
    inline next_node(int lineno, int cicleNumber) :
        cdk::basic_node(lineno), _cicleNumber(cicleNumber) {
    }

    inline next_node(int lineno) : 
      cdk::basic_node(lineno), _cicleNumber(1) {
    }

  public:
    inline int cicleNumber() {
        return _cicleNumber;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_next_node(this, level);
    }

  };

} // mml

#endif