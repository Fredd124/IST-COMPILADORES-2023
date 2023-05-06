#ifndef __MML_AST_FUNCTION_DECLARATION_NODE_H__ 
#define __MML_AST_FUNCTION_DECLARATION_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

   /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::expression_node {
    int _access;
    std::string _identifier;
    cdk::sequence_node *_parameters;

  public:
    inline function_declaration_node(int lineno, int access, const std::string &identifier, cdk::sequence_node *parameters) :
        cdk::expression_node(lineno), _access(access), _identifier(identifier), _parameters(parameters) {
    }

  public:
    inline cdk::sequence_node * parameters() {
      return _parameters;
    }

    inline int access() {
        return _access;
    }

    inline std::string& identifier() {
      return _identifier;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // mml

#endif