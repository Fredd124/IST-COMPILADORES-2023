#ifndef __MML_AST_FUNCTION_CALL_NODE_H__ 
#define __MML_AST_FUNCTION_CALL_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

  /**
   * Class for describing function call nodes.
   */
  class function_call_node: public cdk::expression_node {
    std::string _identifier;
    cdk::sequence_node *_parameters;

  public:
    /**
     * Constructor for a function call with parameters 
     * */
    inline function_call_node(int lineno,const std::string &identifier, cdk::sequence_node *parameters) :
        cdk::expression_node(lineno), _identifier(identifier), _parameters(parameters) {
    }
    /**
     * Constructor for a function call without parameters
    */
    inline function_call_node(int lineno, const std::string &identifier) :
        cdk::expression_node(lineno), _identifier(identifier) {
    }

  public:
    inline cdk::sequence_node *parameters() {
      return _parameters;
    }

    const inline std::string& identifier() const {
        return _identifier;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // mml

#endif