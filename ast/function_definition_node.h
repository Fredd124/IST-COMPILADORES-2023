#ifndef __MML_AST_FUNCTION_DEFINITION_NODE_H__ 
#define __MML_AST_FUNCTION_DEFINITION_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

   /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::typed_node {
    int _access;
    cdk::sequence_node *_parameters;
    mml::block_node *_block;
    bool _isMain;

  public:
    inline function_definition_node(int lineno, int access, std::shared_ptr<cdk::basic_type> functionType,
            cdk::sequence_node *parameters, mml::block_node *block, bool isMain = false) :
        cdk::typed_node(lineno), _access(access), _parameters(parameters), _block(block), _isMain(isMain) {
      type(functionType);
    }

  public:
    inline cdk::sequence_node *parameters() {
      return _parameters;
    }

    inline int access() const {
        return _access;
    }
    
    inline mml::block_node *block() {
        return _block;
    }
   

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // mml

#endif