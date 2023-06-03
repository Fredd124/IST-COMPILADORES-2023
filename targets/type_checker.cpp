#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void mml::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl + 2);
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void mml::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

void mml::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  processIntUnaryExpression(node, lvl);
}
void mml::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;
  processIntBinaryExpression(node, lvl);
}
void mml::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;
  processIntBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void mml::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------


void mml::type_checker::processIntDoubleBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputl = dynamic_cast<mml::input_node*>(node->left());
    mml::input_node *inputr = dynamic_cast<mml::input_node*>(node->right());

    if(inputl != nullptr && inputr != nullptr) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else
      throw std::string("Unknown node with unspecified type.");
  }
  else if(node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputl = dynamic_cast<mml::input_node*>(node->left());

    if(inputl != nullptr) {
      node->left()->type(node->right()->type());
      node->type(node->right()->type());
    }
    else
      throw std::string("Unknown node with unspecified type.");
  }
  else if(node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputr = dynamic_cast<mml::input_node*>(node->right());

    if(inputr != nullptr) {
      node->right()->type(node->left()->type());
      node->type(node->left()->type());
    }
    else
      throw std::string("Unknown node with unspecified type.");
  }
  else
      throw std::string("Wrong types in binary expression.");
}

void mml::type_checker::processIntBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if(node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputl = dynamic_cast<mml::input_node*>(node->left());

    if(inputl != nullptr)
      node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    else
      throw std::string("Unknown node with unspecified type.");
  }

  node->right()->accept(this, lvl + 2);
  if(node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputl = dynamic_cast<mml::input_node*>(node->left());

    if(inputl != nullptr)
      node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    else
      throw std::string("Unknown node with unspecified type.");

  }
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
    throw std::string("Integer expression expected in (left and right) binary operators.");
}

void mml::type_checker::processIntUnaryExpression(cdk::unary_operation_node *const node, int lvl) { 
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *input = dynamic_cast<mml::input_node*>(node->argument());

    if(input != nullptr) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else
      throw std::string("Unknown node with unspecified type.");
  }
  else
    throw std::string("Wrong type in argument of unary expression (Integer expected).");
}

void mml::type_checker::processIntDoubleUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *input = dynamic_cast<mml::input_node*>(node->argument());

    if(input != nullptr) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else
      throw std::string("Unknown node with unspecified type.");
  }
  else
    throw std::string("Wrong type in argument of unary expression (Integer or double expected).");
}

void mml::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processIntDoubleUnaryExpression(node, lvl);
}

void mml::type_checker::do_identity_node(mml::identity_node * const node, int lvl) {
    processIntDoubleUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  //processIntBinaryExpression(node, lvl);
}
void mml::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  //processIntBinaryExpression(node, lvl);
}
void mml::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void mml::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void mml::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processIntBinaryExpression(node, lvl);
}
void mml::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void mml::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void mml::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}
void mml::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processIntDoubleBinaryExpression(node, lvl);
}

void mml::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  //processBinaryExpression(node, lvl);
}
void mml::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  //processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<mml::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void mml::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void mml::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl);
  } catch (const std::string &id) {
    auto symbol = std::make_shared<mml::symbol>(cdk::primitive_type::create(4, cdk::TYPE_INT), id, 0);
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
    node->lvalue()->accept(this, lvl);  //DAVID: bah!
  }

  if (!node->lvalue()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in left argument of assignment expression");

  node->rvalue()->accept(this, lvl + 2);
  if (!node->rvalue()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in right argument of assignment expression");

  // in MML, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_evaluation_node(mml::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void mml::type_checker::do_print_node(mml::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_while_node(mml::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_if_node(mml::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void mml::type_checker::do_if_else_node(mml::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//--------------------------------------------------------------------------

void mml::type_checker::do_input_node(mml::input_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//--------------------------------------------------------------------------

void mml::type_checker::do_next_node(mml::next_node * const node, int lvl) {
    //EMPTY
}

//--------------------------------------------------------------------------

void mml::type_checker::do_stop_node(mml::stop_node * const node, int lvl) {
    //EMPTY
}

//--------------------------------------------------------------------------

void mml::type_checker::do_return_node(mml::return_node * const node, int lvl) {
  node->returnVal()->accept(this, lvl + 2);
  if (node->returnVal()->type() != _function->type()) throw std::string("wrong type in return expression");
}

//--------------------------------------------------------------------------

void mml::type_checker::do_variable_declaration_node(
            mml::variable_declaration_node * const node, int lvl) {
  if (node->initialValue() != nullptr) {
    node->initialValue()->accept(this, lvl + 2);
    if (node->initialValue()->is_typed(cdk::TYPE_UNSPEC)) {  
      mml::input_node *input = dynamic_cast<mml::input_node*>(node->initialValue());
      mml::stack_alloc_node *stack = dynamic_cast<mml::stack_alloc_node*>(node->initialValue()); 
      if(input != nullptr) {                                                   
        if(node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE)    ) 
          node->initialValue()->type(node->type());
        else
          throw std::string("Unable to read input.");
      }
      else if (stack != nullptr) {
        if (node->is_typed(cdk::TYPE_POINTER))
          node->initialValue()->type(node->type());
        else
          throw std::string("Unable to allocate memory.");
      }
      else
        throw std::string("Unknown node with unspecified type.");
    }
    else if (node->is_typed(cdk::TYPE_INT)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_INT)) 
        throw std::string("wrong type for initializer (integer expected).");
    } 
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_INT) && !node->initialValue()->is_typed(cdk::TYPE_DOUBLE)) 
        throw std::string("wrong type for initializer (integer or double expected).");
    } 
    else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_STRING)) {
        throw std::string("wrong type for initializer (string expected).");
      }  
    } 
    else if (node->is_typed(cdk::TYPE_POINTER)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_POINTER)) {
        auto in = (cdk::literal_node<int>*)node->initialValue();
        if (in == nullptr || in->value() != 0) throw std::string("wrong type for initializer (pointer expected).");
      }
    } 
    else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("wrong type for initializer (function expected).");
      }
      auto init_type = cdk::functional_type::cast(node->initialValue()->type());
      auto var_type = cdk::functional_type::cast(node->type());
      for (size_t i = 0; i < init_type->input_length(); i++) {;
        if (init_type->input(i) != var_type->input(i)) {
          if (init_type->input(i)->name() == cdk::TYPE_DOUBLE && var_type->input(i)->name() == cdk::TYPE_INT) continue;
          throw std::string("parameter type mismatch in function initializer");
        }
      }
      if (init_type->output() != var_type->output()) {
/*         if (init_type->output()->name() == cdk::TYPE_DOUBLE && var_type->output()->name() == cdk::TYPE_INT) */ // FIXME : not sure about covariant types
        throw std::string("return type mismatch in function initializer");
      }
    } 
    else {     
      throw std::string("unknown type for initializer.");
    }
  }
  const std::string &id = node->identifier();
  auto symbol = mml::make_symbol(node->type(), id, (long)node->initialValue(), false); // FIXME : should make symbol be like this?
  if (_symtab.insert(id, symbol)) {
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  } else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

//--------------------------------------------------------------------------

void mml::type_checker::do_stack_alloc_node(mml::stack_alloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in stack alloc expression");
  /* auto mtype = cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_UNSPEC)); // FIXME : should be unspec ??
  node->type(mtype); */
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//--------------------------------------------------------------------------

void mml::type_checker::do_block_node(mml::block_node * const node, int lvl) {
    //EMPTY
}


//--------------------------------------------------------------------------

void mml::type_checker::do_pointer_indexation_node(mml::pointer_indexation_node * const node, int lvl) {
  ASSERT_UNSPEC;
  std::shared_ptr <cdk::reference_type> btype;

  node->basePos()->accept(this, lvl + 2);
  btype = cdk::reference_type::cast(node->basePos()->type());
  if (!node->basePos()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in base position expression");
  
  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in indexation expression");

  node->type(btype->referenced());
}

//--------------------------------------------------------------------------

void mml::type_checker::do_function_call_node(mml::function_call_node * const node, int lvl) {
  ASSERT_UNSPEC;
  std::string id;
  cdk::rvalue_node * func_var = (dynamic_cast<cdk::rvalue_node*> (node->function()));
  mml::function_definition_node * definition = dynamic_cast<mml::function_definition_node*>(node->function());
  if (func_var != nullptr) {   
    id =  (dynamic_cast<cdk::variable_node*>(func_var->lvalue()))->name();
  }
  else if (definition != nullptr) {
    id = "_func" + std::to_string(_funcCount);      //FIXME : is this legal?
  }
  else throw std::string("unidentified function caller");
  auto symbol = _symtab.find(id);
  if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");
  if (!symbol->isFunction()) throw std::string("symbol '" + id + "' is not a function.");
  if (node->parameters()->size() == symbol->number_of_arguments()) throw std::string("wrong number of arguments in function call expression");
  node->parameters()->accept(this, lvl + 4);
  for (size_t i = 0; i < node->parameters()->size(); i++) {
    cdk::typed_node* typedNode = dynamic_cast<cdk::typed_node*> (node->parameters()->node(i));
    if (typedNode->type() == symbol->argument_type(i)) continue;
    if (typedNode->is_typed(cdk::TYPE_INT) && symbol->is_argument_typed(i, cdk::TYPE_DOUBLE)) continue; 
    throw std::string("wrong type in function call expression");
  }
}

void mml::type_checker::do_function_definition_node(mml::function_definition_node * const node, int lvl) {
  std::string id;

  if (node->isMain()) {
    id = "_main";
  }
  else {
    id = "_func" + std::to_string(++_funcCount); // hack for function naming
  }
  auto function = mml::make_symbol(node->type(), id, 0, true);
  auto declaration_types = cdk::functional_type::cast(node->type());

  std::vector < std::shared_ptr < cdk::basic_type >> argtypes;
  for (size_t ax = 0; ax < node->parameters()->size(); ax++)
    argtypes.push_back(declaration_types->input(ax));
  function->set_argument_types(argtypes);
  _parent->set_new_symbol(function);
}

//---------------------------------------------------------------------------

void mml::type_checker::do_null_node(mml::null_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

void mml::type_checker::do_address_of_node(mml::address_of_node * const node, int lvl) { //probably need change
  ASSERT_UNSPEC;
  node->leftValue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->leftValue()->type()));
}

//---------------------------------------------------------------------------

