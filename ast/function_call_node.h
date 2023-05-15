#ifndef __MML_AST_FUNCTION_CALL_NODE_H__ 
#define __MML_AST_FUNCTION_CALL_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

  /**
   * Class for describing function call nodes.
   */
  class function_call_node: public cdk::expression_node {
    cdk::expression_node *_function;
    cdk::sequence_node *_parameters;

  public:
    /**
     * Constructor for a function call with parameters 
     * */
    inline function_call_node(int lineno, cdk::expression_node *function, cdk::sequence_node *parameters) :
        cdk::expression_node(lineno), _function(function), _parameters(parameters) {
    }
    /**
     * Constructor for a function call without parameters
    */
    inline function_call_node(int lineno, cdk::expression_node *function) :
        cdk::expression_node(lineno),   _function(function){
    }

  public:
    inline cdk::sequence_node *parameters() {
      return _parameters;
    }

    inline cdk::expression_node *function() {
      return _function;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_call_node(this, level);
    }

  };

} // mml

#endif