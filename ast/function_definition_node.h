#ifndef __MML_AST_FUNCTION_DEFINITION_NODE_H__ 
#define __MML_AST_FUNCTION_DEFINITION_NODE_H__ 

#include <cdk/ast/sequence_node.h>
#include<cdk/types/functional_type.h>

namespace mml {

   /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::expression_node {
    int _access;
    cdk::sequence_node *_parameters;
    mml::block_node *_block;
    bool _isMain;

  public:
    inline function_definition_node(int lineno, const std::shared_ptr<cdk::basic_type> &outputType,
            cdk::sequence_node *parameters, mml::block_node *block, bool isMain = false) :
        cdk::expression_node(lineno), _parameters(parameters), _block(block), _isMain(isMain) {
        std::vector<std::shared_ptr<cdk::basic_type>> inputTypes;
        for (size_t i = 0; i < _parameters->size(); i++) {
            cdk::typed_node *node = dynamic_cast<cdk::typed_node*>(_parameters->node(i));
            inputTypes.insert(inputTypes.end(), node->type());
        }
        std::shared_ptr<cdk::functional_type> functionalType = cdk::functional_type::create(inputTypes, outputType);
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