#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated
#include "mml_parser.tab.h"

//---------------------------------------------------------------------------

void mml::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void mml::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void mml::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.DOUBLE(node->value()); // load number to the stack
  } else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
  }
}
void mml::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(0); // 1 if 0, 0 otherwise
  _pf.EQ();
}
void mml::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void mml::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  } else {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void mml::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  if (_functions.size() > 0) {
    /* leave the address on the stack */
    _pf.TEXT(); // return to the TEXT segment
    _pf.ADDR(mklbl(lbl1)); // the string to be printed
  }
  else {
    /* global variable */
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }
  node->right()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }
  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DADD();
  else
    _pf.ADD();
}
void mml::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }
  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == cdk::TYPE_POINTER && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.INT(3);
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();
}
void mml::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL(); 
}
void mml::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) _pf.I2D();

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}
void mml::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}
void mml::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.LT();
}
void mml::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.LE();
}
void mml::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.GE();
}
void mml::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.GT();
}
void mml::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.NE();
}
void mml::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  if (node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  node->right()->accept(this, lvl + 2);
  if (node->right()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE) _pf.I2D();

  _pf.EQ();
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (symbol->global()) {
    _pf.ADDR(symbol->name());
  } else {
    _pf.LOCAL(symbol->offset());
  }
}

void mml::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
   if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _pf.LDDOUBLE();
  } else {
    // integers, pointers, and strings
    _pf.LDINT();
  }
}

void mml::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    if (node->rvalue()->type()->name() == cdk::TYPE_INT) _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }
  node->lvalue()->accept(this, lvl); 
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_evaluation_node(mml::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT) || node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4); // delete the evaluated value
  } 
  else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.TRASH(8);
  }
  else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } 
  else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
      // do nothing
  }
  else if (node->argument()->is_typed(cdk::TYPE_FUNCTIONAL)) {
    _pf.TRASH(4);
  }
  else {
    std::cerr << "ERROR: THIS SHOULDN'T HAPPEN EVALUATION" << std::endl;
    exit(1);
  }
}

void mml::postfix_writer::do_print_node(mml::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    auto child = dynamic_cast<cdk::expression_node*> (node->arguments()->node(i));
    child->accept(this, lvl); // determine the value to print
    if (child->is_typed(cdk::TYPE_INT)) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // delete the printed value
    }
    else if (child->is_typed(cdk::TYPE_DOUBLE)) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // delete the printed value
    } 
    else if (child->is_typed(cdk::TYPE_STRING)) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // delete the printed value's address
    } 
    else {
      std::cerr << "ERROR: THIS SHOULDN'T HAPPEN PRINT" << std::endl;
      exit(1);
    }
    if (node->newline()) {
      _functions_to_declare.insert("println");
      _pf.CALL("println"); // print a newline
    }
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_while_node(mml::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _whileStartLabels.push(lbl1 = ++_lbl);
  _pf.LABEL(mklbl(lbl1));
  node->condition()->accept(this, lvl);
  _whileEndLabels.push(lbl2 = ++_lbl);
  _pf.JZ(mklbl(lbl2));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_node(mml::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_if_else_node(mml::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_sizeof_node(mml::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_input_node(mml::input_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->is_typed(cdk::TYPE_INT))
  {
    _pf.CALL("readi");
    _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE))
  {
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else {
    std::cerr << "ERROR: THIS SHOULDN'T HAPPEN. CANNOT READ TYPE." << std::endl;
    exit(1);
  }
}


//--------------------------------------------------------------------------

void mml::postfix_writer::do_next_node(mml::next_node * const node, int lvl) {
    int lbl;
    std::vector<int> temp; 
    int i;
    for (i = 0; i < node->cicleNumber(); i++) { // FIXME : cycle
      temp.push_back(_whileStartLabels.top());
      _whileStartLabels.pop();
    }
    lbl = temp[i - 1];
    for (i = 0; i < node->cicleNumber(); i++) {
      _whileStartLabels.push(temp[i]); // FIXME : maybe using vector as stack is better
    }
    _pf.JMP(mklbl(lbl));
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_stop_node(mml::stop_node * const node, int lvl) {
    int lbl;
    std::vector<int> temp; 
    int i;
    for (i = 0; i < node->cicleNumber(); i++) { 
      temp.push_back(_whileEndLabels.top());
      _whileEndLabels.pop();
    }
    lbl = temp[i - 1];
    for (i = 0; i < node->cicleNumber(); i++) {
      _whileEndLabels.push(temp[i]); // FIXME : maybe using vector as stack is better
    }
    _pf.JMP(mklbl(lbl));
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_return_node(mml::return_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    // should not reach here without returning a value (if not void)
    auto return_type = cdk::functional_type::cast(_functions.top()->type())->output(0);
    if (return_type->name() != cdk::TYPE_VOID) {
      node->returnVal()->accept(this, lvl + 2);

      if (return_type->name() == cdk::TYPE_INT || return_type->name() == cdk::TYPE_STRING
          || return_type->name() == cdk::TYPE_POINTER) {
        _pf.STFVAL32();
      } else if (return_type->name() == cdk::TYPE_DOUBLE) {
        if (node->returnVal()->type()->name() == cdk::TYPE_INT) _pf.I2D();
        _pf.STFVAL64();
      } else {
        std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
      }
    }

    _pf.JMP(_currentBodyRetLabels.top());
    _returnSeen = true;
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_variable_declaration_node(
                    mml::variable_declaration_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    if (node->is_typed(cdk::TYPE_FUNCTIONAL) && node->qualifier() == tFORWARD)
      return;
    auto id = node->identifier();
    int offset = 0, typesize = node->type()->size(); // in bytes
    if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else if ( _inFunctionArgs ) {
    offset = _offset;
    _offset += typesize;
  } else {
    offset = 0; // global variable
  }
  auto symbol = new_symbol();
  if (symbol) {
    symbol->offset(offset);
    reset_new_symbol();
  }
  if (_inFunctionBody) {
    // if we are dealing with local variables, then no action is needed
    // unless an initializer exists
    if (node->initialValue()) {
      node->initialValue()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initialValue()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else {
        std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
        /* _errors = true; */ // FIXME
      }
    }
  }
  else {
    if (!_functions.size()) {
      if (node->initialValue() == nullptr) {
        _pf.BSS();
        _pf.ALIGN();
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      } else {
        if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);

          if (node->is_typed(cdk::TYPE_INT)) {
            node->initialValue()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_POINTER)) {
            node->initialValue()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
            if (node->initialValue()->is_typed(cdk::TYPE_DOUBLE)) {
              node->initialValue()->accept(this, lvl);
            } else if (node->initialValue()->is_typed(cdk::TYPE_INT)) {
              cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initialValue());
              cdk::double_node ddi(dclini->lineno(), dclini->value());
              ddi.accept(this, lvl);
            } else {
              std::cerr << node->lineno() << ": '" << id << "' has bad initializer for double value\n";
              /* _errors = true; */ // FIXME
            }
          }
        } else if (node->is_typed(cdk::TYPE_STRING)) {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);
          node->initialValue()->accept(this, lvl);
        } else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
            _pf.DATA();
            _pf.ALIGN();
            _pf.LABEL(id);
          node->initialValue()->accept(this, lvl);
        } else {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
          /* _errors = true; */ // FIXME
        }

      }

    }
  }

}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_stack_alloc_node(mml::stack_alloc_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->argument()->accept(this, lvl);
    _pf.INT(3);
    _pf.SHTL();
    _pf.ALLOC(); // allocate space
    _pf.SP(); // put stack pointer on stack
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_block_node(mml::block_node * const node, int lvl) {
    _symtab.push();
    node->declarations()->accept(this, lvl);
    node->instructions()->accept(this, lvl);
    _symtab.pop();
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_pointer_indexation_node(mml::pointer_indexation_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    node->basePos()->accept(this, lvl);
    node->index()->accept(this, lvl);
    _pf.INT(3);
    _pf.SHTL();
    _pf.ADD();
}

//--------------------------------------------------------------------------

void mml::postfix_writer::do_function_call_node(mml::function_call_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<mml::symbol> symbol = nullptr;
  if (node->function() == nullptr) {
    symbol = _functions.top();
  } else {

    auto var = dynamic_cast<cdk::variable_node*>((dynamic_cast<cdk::rvalue_node*> (node->function()))->lvalue());
    auto definition = dynamic_cast<mml::function_definition_node*>(node->function());
    if (var != nullptr) {
      symbol = _symtab.find(var->name());
    }
    else if (definition) {
      
    }
    else {
      std::cerr << "ERROR: unknown way to call function" << std::endl;
      exit(1);
    }  

  }

  size_t argsSize = 0;
  if (node->parameters()->size() > 0) {
    for (int ax = node->parameters()->size() - 1; ax >= 0; ax--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->parameters()->node(ax));
      arg->accept(this, lvl + 2);
      if (symbol->is_argument_typed(ax, cdk::TYPE_DOUBLE) && arg->is_typed(cdk::TYPE_INT)) {
        _pf.I2D();
      }
      argsSize += symbol->argument_size(ax);
    }
  }

  if (node->function() == nullptr) {
    _pf.CALL(symbol->name());
  }
  else {
    auto var = (dynamic_cast<cdk::rvalue_node*> (node->function()));
    auto definition = dynamic_cast<mml::function_definition_node*>(node->function());

    if (var != nullptr) {
      /* symbol = _symtab.find(var->name()); */
      /* symbol = _symtab.find(symbol->label()); */
      /* var->accept(this, lvl); */
      var->accept(this, lvl);
      /* _pf.SADDR(var->name()); */
      
    }
    else if (definition) {
      
    }
    else {
      std::cerr << "ERROR: unknown way to call function" << std::endl;
      exit(1);
    }  

    _pf.BRANCH();
    /* _pf.CALL(symbol->label()); */
  }

  if (argsSize > 0) {
    _pf.TRASH(argsSize);
  }
  auto output_type = cdk::functional_type::cast(symbol->type())->output(0);
  if (output_type->name() == cdk::TYPE_INT || output_type->name() == cdk::TYPE_POINTER || output_type->name() ==  cdk::TYPE_STRING) {
    _pf.LDFVAL32();
  } else if (output_type->name() == cdk::TYPE_DOUBLE) {
    _pf.LDFVAL64();
  } else if (output_type->name() == cdk::TYPE_VOID) {
    // do nothing
  } else {
    std::cerr << "ERROR: cannot call function with unknown type" << std::endl;
    exit(1);
  }

}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_function_definition_node(mml::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string id = "_func" + std::to_string(_funcCount);
  if (!node->isMain()) _funcCount ++;

  // remember symbol so that args and body know
  _functions.push(new_symbol());
  _functions.top()->isFunction(true);
  _functions.top()->type(node->type()); // FIXME : should this be here?

  reset_new_symbol();
  _currentBodyRetLabels.push(mklbl(++_lbl));

  _offset = 8;
  _symtab.push(); // scope of args

  if (node->parameters()->size() > 0) {
    _inFunctionArgs = true;
    for (size_t ix = 0; ix < node->parameters()->size(); ix++) {
      cdk::basic_node *arg = node->parameters()->node(ix);
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (node->isMain()){
    _pf.GLOBAL(_functions.top()->name(), _pf.FUNC());
  }
  _pf.LABEL(_functions.top()->name());

  frame_size_calculator lsc(_compiler, _symtab, _functions, _funcCount);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize());
  
  _offset = 0;
  _inFunctionBody = true;

   node->block()->accept(this, lvl + 4); // block has its own scope
  _inFunctionBody = false;
  _pf.LABEL(_currentBodyRetLabels.top());
  _currentBodyRetLabels.pop();
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop(); // scope of arguments
  _functions.pop();
  if (node->isMain()) {
    for (std::string s : _functions_to_declare)
      _pf.EXTERN(s);
  }
  else {
    if (_functions.size() > 0) {
      /* leave the address on the stack */
      _pf.TEXT(); // return to the TEXT segment
      _pf.ADDR(id); // the string to be printed
    }
    else {
      /* global variable */
      _pf.DATA();
      _pf.SADDR(id);
    }
  }

}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_null_node(mml::null_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_address_of_node(mml::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->leftValue()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void mml::postfix_writer::do_identity_node(mml::identity_node * const node, int lvl) {
  node->argument()->accept(this, lvl);
}