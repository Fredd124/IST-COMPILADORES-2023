#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>
#include <iostream>

#include "mml_parser.tab.h"

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

void propagate_type(cdk::typename_type type, cdk::expression_node * node) {
  cdk::binary_operation_node *binary = dynamic_cast<cdk::binary_operation_node*>(node);
  cdk::unary_operation_node *unary = dynamic_cast<cdk::unary_operation_node*>(node);
  if (binary != nullptr) {
    if (binary->left()->is_typed(cdk::TYPE_UNSPEC)) {
      binary->left()->type(cdk::primitive_type::create(4, type));
      propagate_type(type, binary->left());
    }
    if (binary->right()->is_typed(cdk::TYPE_UNSPEC)) {
      binary->right()->type(cdk::primitive_type::create(4, type));
      propagate_type(type, binary->right());
    }
  }
  else if (unary != nullptr) {
    if (unary->argument()->is_typed(cdk::TYPE_UNSPEC)) {
      unary->argument()->type(cdk::primitive_type::create(4, type));
      propagate_type(type, unary->argument());
    }
  }

}

std::shared_ptr<cdk::basic_type> mml::type_checker::processPointerForBoolean(std::shared_ptr<cdk::reference_type> leftPtr, std::shared_ptr<cdk::reference_type> rightPtr){
    std::shared_ptr<cdk::basic_type> left, right;
    left = leftPtr;
    right = rightPtr;
    while (left->name() == cdk::TYPE_POINTER && right->name() == cdk::TYPE_POINTER) {
      left = cdk::reference_type::cast(left)->referenced();
      right = cdk::reference_type::cast(right)->referenced();
    }
    if (left->name() == cdk::TYPE_POINTER || right->name() == cdk::TYPE_POINTER)
      throw std::string("Wrong pointer type.");
    if ((left->name() == cdk::TYPE_INT && right->name() == cdk::TYPE_INT) ||
            (left->name() == cdk::TYPE_DOUBLE && right->name() == cdk::TYPE_DOUBLE) ||
            (left->name() == cdk::TYPE_STRING && right->name() == cdk::TYPE_STRING) ||
            (left->name() == cdk::TYPE_VOID && right->name() == cdk::TYPE_VOID))
      return cdk::primitive_type::create(4, cdk::TYPE_INT);
    else 
      throw std::string("Wrong pointer type.");
}

bool mml::type_checker::processFunctionalType(std::shared_ptr<cdk::functional_type> leftType, std::shared_ptr<cdk::functional_type> rightType) {
    if (leftType->input_length() != rightType->input_length())
      return false;
    for (size_t i = 0; i < rightType->input_length(); i++) {;
      if (leftType->input(i) != rightType->input(i)) {
        if (rightType->input(i)->name() == cdk::TYPE_DOUBLE && leftType->input(i)->name() == cdk::TYPE_INT) continue;
        return false;
      }
    }

    if (rightType->output(0) != leftType->output(0)) {
      if (!(rightType->output(0)->name() == cdk::TYPE_INT && leftType->output(0)->name() == cdk::TYPE_DOUBLE)) // FIXME : not sure about covariant types
        return false;
    }
    return true;
}

std::shared_ptr<cdk::basic_type> mml::type_checker::processPointer(std::shared_ptr<cdk::reference_type> leftPtr, std::shared_ptr<cdk::reference_type> rightPtr) {
    std::shared_ptr<cdk::basic_type> left, right;
    left = leftPtr;
    right = rightPtr;
    while (left->name() == cdk::TYPE_POINTER && right != nullptr && right->name() == cdk::TYPE_POINTER) {
      left = cdk::reference_type::cast(left)->referenced();
      right = cdk::reference_type::cast(right)->referenced();
    }
    if (right == nullptr) { // right value is null
      return leftPtr;
    }
    else if (left->name() == cdk::TYPE_VOID || right->name() == cdk::TYPE_VOID) {
      return leftPtr;
    }
    else if (left->name() == cdk::TYPE_POINTER || right->name() == cdk::TYPE_POINTER)
      throw std::string("Wrong pointer type.");
    else if (left->name() == cdk::TYPE_FUNCTIONAL || right->name() == cdk::TYPE_FUNCTIONAL) {
      auto leftType = cdk::functional_type::cast(left);
      auto rightType = cdk::functional_type::cast(right);
      if (!processFunctionalType(leftType, rightType))
        throw std::string("Wrong functional type.");
      return leftPtr;
    }
    else if (left->name() == cdk::TYPE_INT && right->name() == cdk::TYPE_INT) {
      return cdk::primitive_type::create(4, cdk::TYPE_INT);
    }
    else if (left->name() == cdk::TYPE_DOUBLE && right->name() == cdk::TYPE_DOUBLE) {
      return cdk::primitive_type::create(8, cdk::TYPE_DOUBLE);
    }
    else if (left->name() == cdk::TYPE_STRING && right->name() == cdk::TYPE_STRING)  {
      return cdk::primitive_type::create(4, cdk::TYPE_STRING);
    }
    else 
      throw std::string("Wrong pointer type.");
}

bool processUnspecUnary(cdk::unary_operation_node *const node) {
  if(node->argument()->is_typed(cdk::TYPE_UNSPEC) ) {
    node->type(node->argument()->type());
    return true;
  }
  
  return false;
}

bool processUnspecBinary(cdk::binary_operation_node *const node) {
  if(node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(node->left()->type());
    return true;
  }
  else if(node->left()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputl = dynamic_cast<mml::input_node*>(node->left());
    mml::stack_alloc_node *stackl = dynamic_cast<mml::stack_alloc_node*>(node->left());

    if(inputl != nullptr) {
      if(node->right()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_INT)) {
        node->left()->type(node->right()->type());
      }
      else if (node->right()->is_typed(cdk::TYPE_POINTER)) { // sub and add with integer
        auto add_node = dynamic_cast<cdk::add_node*>(node);
        auto sub_node = dynamic_cast<cdk::sub_node*>(node);
        if (add_node != nullptr || sub_node != nullptr) {
          node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        }
        else {
          throw std::string("Invalid expression in left argument of binary expression.");
        }
      }
      else 
        throw std::string("Invalid expression in left argument of binary expression.");
      node->type(node->right()->type());
    }
    else if (stackl != nullptr) {
      if (node->right()->is_typed(cdk::TYPE_INT)) {
        auto add_node = dynamic_cast<cdk::add_node*>(node);
        auto sub_node = dynamic_cast<cdk::sub_node*>(node);
        if (add_node == nullptr && sub_node == nullptr) {
          throw std::string("Invalid expression in left argument of binary expression.");
        }
      }
      else {
        throw std::string("Invalid expression in left argument of binary expression.");
      }
      node->type(node->left()->type());
    }
    else {
      node->left()->type(node->right()->type());
      propagate_type(node->right()->type()->name(), node->left());
      node->type(node->right()->type());
    }
    return true;
  }
  else if(node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputr = dynamic_cast<mml::input_node*>(node->right());
    mml::stack_alloc_node *stackr = dynamic_cast<mml::stack_alloc_node*>(node->right());
    if(inputr != nullptr) {
      if(node->left()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_INT)) {
        node->right()->type(node->left()->type());
      }
      else if (node->left()->is_typed(cdk::TYPE_POINTER)) { // sub and add with integer
        auto add_node = dynamic_cast<cdk::add_node*>(node);
        auto sub_node = dynamic_cast<cdk::sub_node*>(node);
        if (add_node != nullptr || sub_node != nullptr) {
          node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        }
        else {
          throw std::string("Invalid expression in right argument of binary expression.");
        }
      }
      else
        throw std::string("Invalid expression in right argument of binary expression.");
      node->type(node->left()->type());
    }
    else if (stackr != nullptr) {
      if (node->left()->is_typed(cdk::TYPE_INT)) {
        auto add_node = dynamic_cast<cdk::add_node*>(node);
        auto sub_node = dynamic_cast<cdk::sub_node*>(node);
        if (add_node == nullptr && sub_node == nullptr) {
          throw std::string("Invalid expression in right argument of binary expression.");
        }
      }
      else {
        throw std::string("Invalid expression in right argument of binary expression.");
      }
      node->type(node->right()->type());
    }
    else {
      node->right()->type(node->left()->type());
      propagate_type(node->right()->type()->name(), node->left());
      node->type(node->left()->type());
    }
    return true;
  }
  return false;
}

void mml::type_checker::processIntDoublePointerBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (processUnspecBinary(node)) return;
  if(node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(processPointerForBoolean(cdk::reference_type::cast(node->left()->type()),cdk::reference_type::cast(node->right()->type())));
    return;
  }
  if(node->left()->type()->name() != node->right()->type()->name()) {
    if(!((node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))||
    (node->left()->is_typed(cdk::TYPE_INT)  &&  node->right()->is_typed(cdk::TYPE_INT))))
        throw std::string("Operator has incompatible types.");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void mml::type_checker::processIntDoubleBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (processUnspecBinary(node)) return;
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
      throw std::string("Wrong types in binary expression.");
}

void mml::type_checker::processIntBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (processUnspecBinary(node)) return;
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
    throw std::string("Integer expression expected in (left and right) binary operators.");
}

void mml::type_checker::processIntUnaryExpression(cdk::unary_operation_node *const node, int lvl) { 
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (processUnspecUnary(node)) return;
  else if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else
    throw std::string("Wrong type in argument of unary expression (Integer expected).");
}

void mml::type_checker::processIntDoubleUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (processUnspecUnary(node)) return;
  else if (node->argument()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
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
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (processUnspecBinary(node)) {
    return;
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
    node->type(node->right()->type());
  else
    throw std::string("Wrong types in binary expression.");
}

void mml::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
 ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (processUnspecBinary(node)) return;
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  }
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    processPointer(cdk::reference_type::cast(node->left()->type()),cdk::reference_type::cast(node->right()->type()));
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else
    throw std::string("Wrong types in binary expression.");
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
  processIntDoublePointerBinaryExpression(node, lvl);
}
void mml::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processIntDoublePointerBinaryExpression(node, lvl);
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
  node->lvalue()->accept(this, lvl + 2);
  node->rvalue()->accept(this, lvl + 2);
  if(node->lvalue()->is_typed(cdk::TYPE_UNSPEC))
    throw std::string("Left value must have a type.");

  if(node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *inputr = dynamic_cast<mml::input_node *>(node->rvalue());
    mml::stack_alloc_node *stackr = dynamic_cast<mml::stack_alloc_node *>(node->rvalue());

    if(inputr != nullptr) {
      if(node->lvalue()->is_typed(cdk::TYPE_INT) || node->lvalue()->is_typed(cdk::TYPE_DOUBLE))
        node->rvalue()->type(node->lvalue()->type());
      else
        throw std::string("Invalid expression for lvalue node.");
    }
    else if(stackr != nullptr) {
      if(node->lvalue()->is_typed(cdk::TYPE_POINTER))
        node->rvalue()->type(node->lvalue()->type());
      else 
        throw std::string("A pointer is required to allocate.");
    }
    else {
      node->rvalue()->type(node->lvalue()->type());
      propagate_type(node->lvalue()->type()->name(), node->rvalue());
    }
    node->type(node->lvalue()->type());

  }
  else if(node->lvalue()->is_typed(cdk::TYPE_INT) && node->rvalue()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if(node->lvalue()->is_typed(cdk::TYPE_DOUBLE) && 
    (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT))) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else if(node->lvalue()->is_typed(cdk::TYPE_STRING) && node->rvalue()->is_typed(cdk::TYPE_STRING)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
  }
  else if(node->lvalue()->is_typed(cdk::TYPE_POINTER) && node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
    if(processPointer(
      cdk::reference_type::cast(node->lvalue()->type()), 
      cdk::reference_type::cast(node->rvalue()->type())))
      node->type(node->lvalue()->type());
  }
  else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL) && node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
    auto right_type = cdk::functional_type::cast(node->rvalue()->type());
    auto left_type = cdk::functional_type::cast(node->lvalue()->type());
    if(!processFunctionalType(left_type, right_type))
      throw std::string("wrong functional types in assignment");
    node->type(node->lvalue()->type()); 
  }
  else {
    throw std::string("wrong types in assignment");
  }

}

//---------------------------------------------------------------------------

void mml::type_checker::do_evaluation_node(mml::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void mml::type_checker::do_print_node(mml::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
  auto seq = node->arguments();
  for (size_t i = 0; i < seq->size(); i++) {
    auto arg = dynamic_cast<cdk::typed_node*>(seq->node(i));
    if (arg->type()->name() == cdk::TYPE_UNSPEC)
      arg->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_while_node(mml::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
    node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    propagate_type(cdk::TYPE_INT, node->condition());
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_if_node(mml::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
    node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    propagate_type(cdk::TYPE_INT, node->condition());
  }
}

void mml::type_checker::do_if_else_node(mml::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (node->condition()->is_typed(cdk::TYPE_UNSPEC)) {
    node->condition()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    propagate_type(cdk::TYPE_INT, node->condition());
  }
}

//---------------------------------------------------------------------------

void mml::type_checker::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  if (node->expression()->is_typed(cdk::TYPE_UNSPEC)) {
    node->expression()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    propagate_type(cdk::TYPE_INT, node->expression());
  }
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
  auto function_type = cdk::functional_type::cast(_functions.top()->type());
  if (node->returnVal() == nullptr) {
    if (function_type->output(0)->name() != cdk::TYPE_VOID) {
      throw std::string("empty return only allowed in void functions, function returns ") + function_type->output(0)->to_string();
    }
    return;
  }
  node->returnVal()->accept(this, lvl + 2);
  if (node->returnVal()->is_typed(cdk::TYPE_UNSPEC)) {
    if (function_type->output(0)->name() == cdk::TYPE_DOUBLE || function_type->output(0)->name() == cdk::TYPE_INT) {
      node->returnVal()->type(function_type->output(0));
      propagate_type(function_type->output(0)->name(), node->returnVal());
    }
    else
      throw std::string("wrong type in return expression");
  }
  else if (node->returnVal()->type() != function_type->output(0)) // only one output type
    if(! (node->returnVal()->is_typed(cdk::TYPE_INT) && function_type->output(0)->name() == cdk::TYPE_DOUBLE))
      throw std::string("wrong type in return expression");
}

//--------------------------------------------------------------------------

void mml::type_checker::do_variable_declaration_node(
            mml::variable_declaration_node * const node, int lvl) {
  if (node->initialValue() != nullptr) {
    node->initialValue()->accept(this, lvl + 2);
    if (node->type() == nullptr) { // auto type
      if (node->initialValue()->is_typed(cdk::TYPE_UNSPEC)) {
        if (dynamic_cast<mml::stack_alloc_node *>(node->initialValue()) != nullptr) {
          node->type(cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_INT)));
        }
        node->initialValue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        propagate_type(cdk::TYPE_INT, node->initialValue());
      }
      node->type(node->initialValue()->type());
    }
    else if (node->initialValue()->is_typed(cdk::TYPE_UNSPEC)) {  
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
      else {
        node->initialValue()->type(node->type());
        propagate_type(node->type()->name(), node->initialValue());
      }
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
        throw std::string("wrong type for initializer (pointer expected).");
      }
      try {
        processPointer(cdk::reference_type::cast(node->type()), cdk::reference_type::cast(node->initialValue()->type()));
      }
      catch(std::string &s) {
        throw std::string("wrong type for initializer ") + node->type()->to_string().c_str() + std::string(" expected.");
      }
    } 
    else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
      if (!node->initialValue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("wrong type for initializer (function expected).");
      }
      auto init_type = cdk::functional_type::cast(node->initialValue()->type());
      auto var_type = cdk::functional_type::cast(node->type());
      if(!processFunctionalType(var_type, init_type)) {
        throw std::string("wrong type for initializer (function expected).");      }
    } 
    else {     
      throw std::string("unknown type for initializer.");
    }
  }
  const std::string &id = node->identifier();
  bool isFunction = node->is_typed(cdk::TYPE_FUNCTIONAL);
  bool isForward = node->qualifier() == tFORWARD;
  bool isForeign = node->qualifier() == tFOREIGN;
  auto symbol = mml::make_symbol(node->type(), id, (long)node->initialValue(), isFunction, isForward, isForeign); // FIXME : should make symbol be like this?
  
  if (_symtab.insert(id, symbol) || _symtab.find(id)->forward()) {
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  } else {
    throw std::string("variable '" + id + "' redeclared");
  }
}

//--------------------------------------------------------------------------

void mml::type_checker::do_stack_alloc_node(mml::stack_alloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if(node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    mml::input_node *input = dynamic_cast<mml::input_node *>(node->argument());

    if(input != nullptr)
      node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    else {
      node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      propagate_type(cdk::TYPE_INT, node->argument());
    }
  }
  else if (!node->argument()->is_typed(cdk::TYPE_INT))
    throw std::string("Integer expression expected in allocation expression.");

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
  if (node->basePos()->is_typed(cdk::TYPE_UNSPEC)){ // stack alloc
    node->basePos()->type(cdk::reference_type::create(4, cdk::primitive_type::create(4, cdk::TYPE_INT)));
    btype = cdk::reference_type::cast(node->basePos()->type());
    propagate_type(cdk::TYPE_POINTER, node->basePos());
  }
  if (!node->basePos()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in base position expression");
  
  node->index()->accept(this, lvl + 2);
  if (node->index()->is_typed(cdk::TYPE_UNSPEC)){ // input
    node->index()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    propagate_type(cdk::TYPE_INT, node->index());
  }
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in indexation expression");

  node->type(btype->referenced());
}

//--------------------------------------------------------------------------

void mml::type_checker::assertCallParameters(mml::function_call_node * const node, std::shared_ptr<mml::symbol> function_symbol, int lvl) {
  if (node->parameters()->size() != function_symbol->number_of_arguments()) 
    throw std::string("wrong number of arguments in function call expression " + function_symbol->label() + " : expected=" 
    + std::to_string(function_symbol->number_of_arguments()) + ", found=" + std::to_string(node->parameters()->size()) + ".");
  node->parameters()->accept(this, lvl + 4);
  for (size_t i = 0; i < node->parameters()->size(); i++) {
    cdk::typed_node* typedNode = dynamic_cast<cdk::typed_node*> (node->parameters()->node(i));
    if (typedNode->is_typed(cdk::TYPE_FUNCTIONAL) && function_symbol->argument_type(i)->name() == cdk::TYPE_FUNCTIONAL) {
      auto val_type = cdk::functional_type::cast(typedNode->type());
      auto arg_type = cdk::functional_type::cast(function_symbol->argument_type(i));
      if(!processFunctionalType(arg_type, val_type))
        throw std::string("wrong type in functional type call expression");
      continue; 
    }
    else if (typedNode->is_typed(cdk::TYPE_POINTER) && function_symbol->argument_type(i)->name() == cdk::TYPE_POINTER) {
      auto val_type = cdk::reference_type::cast(typedNode->type());
      auto arg_type = cdk::reference_type::cast(function_symbol->argument_type(i));
      if(!processPointer(arg_type, val_type))
        throw std::string("wrong type in pointer type call expression");
      continue;
    }
    else if (typedNode->is_typed(cdk::TYPE_UNSPEC)) {
      mml::input_node *input = dynamic_cast<mml::input_node*>(typedNode);
      if(input != nullptr) {
        if (function_symbol->is_argument_typed(i, cdk::TYPE_DOUBLE))
          typedNode->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        else if (function_symbol->is_argument_typed(i, cdk::TYPE_INT))
          typedNode->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        else throw std::string("Function parameter type not valid for input");
      }
      else {
        typedNode->type(function_symbol->argument_type(i));
        auto expression = dynamic_cast<cdk::expression_node*>(typedNode);
        if (!expression) {
          throw std::string("Expression expected in function call argument");
        }
        propagate_type(node->type()->name(), expression);
      }
      continue;
    }
    else if (typedNode->type() == function_symbol->argument_type(i)) continue;
    else if (typedNode->is_typed(cdk::TYPE_INT) && function_symbol->is_argument_typed(i, cdk::TYPE_DOUBLE)) continue; 
    throw std::string("wrong type in function call expression");
  }
}

void mml::type_checker::do_function_call_node(mml::function_call_node * const node, int lvl) {

  if (node->function() == nullptr) {
    if (_functions.top()->isMain()) {
        throw std::string("Error: can not make recursion on main");
    }
    assertCallParameters(node, _functions.top(), lvl);
    node->type(cdk::functional_type::cast(_functions.top()->type())->output()->component(0));
    return; // recursion case
  }
  node->function()->accept(this, lvl + 2);
  std::string id;
  cdk::rvalue_node * func_var = (dynamic_cast<cdk::rvalue_node*> (node->function()));
  mml::function_definition_node * definition = dynamic_cast<mml::function_definition_node*>(node->function());
  mml::function_call_node * call = dynamic_cast<mml::function_call_node*>(node->function());
  mml::pointer_indexation_node * index = dynamic_cast<mml::pointer_indexation_node*>(node->function());
  std::shared_ptr<mml::symbol> symbol = _symtab.find(id);
  if (func_var != nullptr) {   
    id =  (dynamic_cast<cdk::variable_node*>(func_var->lvalue()))->name();
    symbol = _symtab.find(id);
    if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");
    if (!symbol->isFunction()) throw std::string("symbol '" + id + "' is not a function."); 
  }
  else if (definition != nullptr ) {            // new definition
    id = "_func" + std::to_string(_funcCount);
  }
  else if (call != nullptr || index != nullptr) { // previous declared
    id = "_func" + std::to_string(_funcCount - 1);
  }
  else {
    std::cerr << "ERROR: unexpected node type in function call" << std::endl;
    exit(1);
  }
  auto function_symbol = _symtab.find(id);
  auto output_type = cdk::functional_type::cast(function_symbol->type())->output()->component(0);
  node->type(output_type);
  assertCallParameters(node, function_symbol, lvl);
}

void mml::type_checker::do_function_definition_node(mml::function_definition_node * const node, int lvl) {
  std::string id;
  if (node->isMain()) {
    id = "_main";
  }
  else {
    id = "_func" + std::to_string(_funcCount); // hack for function naming
  }
  auto function = mml::make_symbol(node->type(), id, 0, true);
  function->label(id);
  function->isMain(node->isMain());
  _symtab.insert(id, function);
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

