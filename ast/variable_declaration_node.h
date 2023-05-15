#ifndef __MML_AST_VARIABLE_DECLARATION_NODE_H__
#define __MML_AST_VARIABLE_DECLARATION_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>

namespace mml {

  /**
   * Class for describing variable declaration nodes.
   */
  class variable_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::expression_node *_initialValue;

  public:
    inline variable_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> varType,
        const std::string &identifier, cdk::expression_node *initialValue) :
      cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier) {
        type(varType);
    }

  public:
    inline int qualifier() const {
      return _qualifier;
    }

    inline std::string &identifier() const {
      return _identifier;
    }

    inline cdk::expression_node *initialValue() {
      return _initialValue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_declaration_node(this, level);
    }

  };

} // mml

#endif