#ifndef __MML_AST_FUNCTION_CALL_NODE_H__ 
#define __MML_AST_FUNCTION_CALL_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

  /**
   * Class for describing sizeof nodes.
   */
  class function_call_node: public cdk::expression_node {
    std::string _functionName;
    cdk::sequence_node *_parameters;

  public:
    /**
     * Constructor for a function call with parameters 
     * */
    inline function_call_node(int lineno,const std::string &functionName, cdk::sequence_node *parameters) :
        cdk::expression_node(lineno), _functionName(functionName), _parameters(parameters) {
    }

    inline function_call_node(int lineno, const std::string &functionName) :
        cdk::expression_node(lineno), _functionName(functionName) {

    }

  public:
    inline cdk::sequence_node * parameters() {
      return _parameters;
    }

    inline std::string& functionName() {
        return _functionName;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // mml

#endif