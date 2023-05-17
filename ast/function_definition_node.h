#ifndef __MML_AST_FUNCTION_DEFINITION_NODE_H__ 
#define __MML_AST_FUNCTION_DEFINITION_NODE_H__ 

#include <cdk/ast/sequence_node.h>

namespace mml {

   /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::expression_node {
    int _access;
    cdk::sequence_node *_parameters;
    mml::block_node *_block;
    bool _isMain;
    cdk::functional_type *_functionalType;

  public:
    inline function_definition_node(int lineno, int access, const std::vector<std::shared_ptr<cdk::basic_type>> &inputTypes, 
            const std::shared_ptr<cdk::basic_type> &outputType, const std::string &identifier,
            cdk::sequence_node *parameters, mml::block_node *block, bool isMain == false) :
        cdk::typed_node(lineno), _access(access), _identifier(identifier), _parameters(parameters), _block(block), _isMain(isMain) {
        std::shared_ptr<cdk::functional_type> functionalType = _functionalType->create(inputTypes, outputType);
        type(functionalType);
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