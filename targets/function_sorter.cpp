#include <string>
#include <iostream>
#include "targets/function_sorter.h"
#include "targets/type_checker.h"
#include "targets/symbol.h"
#include ".auto/all_nodes.h"

mml::function_sorter::~function_sorter() {
  os().flush();
}

void mml::function_sorter::do_add_node(cdk::add_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_and_node(cdk::and_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  node->rvalue()->accept(this, lvl + 2);
}
void mml::function_sorter::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_div_node(cdk::div_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_double_node(cdk::double_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_eq_node(cdk::eq_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_ge_node(cdk::ge_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_gt_node(cdk::gt_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_variable_node(cdk::variable_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_integer_node(cdk::integer_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_le_node(cdk::le_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_lt_node(cdk::lt_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_mod_node(cdk::mod_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_mul_node(cdk::mul_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_ne_node(cdk::ne_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_neg_node(cdk::neg_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_not_node(cdk::not_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_or_node(cdk::or_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_string_node(cdk::string_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_sub_node(cdk::sub_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_evaluation_node(mml::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}
void mml::function_sorter::do_print_node(mml::print_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_input_node(mml::input_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_address_of_node(mml::address_of_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_function_call_node(mml::function_call_node *const node, int lvl) {
  node->parameters()->accept(this, lvl + 2);
}
void mml::function_sorter::do_pointer_indexation_node(mml::pointer_indexation_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_stop_node(mml::stop_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_null_node(mml::null_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_return_node(mml::return_node *const node, int lvl) {
  node->returnVal()->accept(this, lvl + 2);
}
void mml::function_sorter::do_stack_alloc_node(mml::stack_alloc_node *const node, int lvl) {
  // EMPTY
}
void mml::function_sorter::do_next_node(mml::next_node *const node, int lvl) {
  // EMPTY
}

void mml::function_sorter::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    cdk::basic_node *n = node->node(i);
    if (n == nullptr) break;
    n->accept(this, lvl + 2);
  }
}

void mml::function_sorter::do_block_node(mml::block_node *const node, int lvl) {
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
}

void mml::function_sorter::do_while_node(mml::while_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void mml::function_sorter::do_if_node(mml::if_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
}

void mml::function_sorter::do_if_else_node(mml::if_else_node *const node, int lvl) {
  node->thenblock()->accept(this, lvl + 2);
  if (node->elseblock()) node->elseblock()->accept(this, lvl + 2);
}

void mml::function_sorter::do_variable_declaration_node(mml::variable_declaration_node *const node, int lvl) {
  if (node->initialValue() != nullptr) node->initialValue()->accept(this, lvl + 2);
}

void mml::function_sorter::do_function_definition_node(mml::function_definition_node *const node, int lvl) {
  node->block()->accept(this, lvl + 2);
  std::cerr << "function definition node" << std::endl;
  _functions_to_define.push_back(node);
}

void mml::function_sorter::do_sizeof_node(mml::sizeof_node *const node, int lvl) {
  node->expression()->accept(this, lvl + 2);
}

void mml::function_sorter::do_identity_node(mml::identity_node *const node, int lvl) {
  // EMPTY
}