#include "ptest.h"
#include "../mpc.h"

bool ast_eq(void* x, void* y) {
  return false;
}

void test_grammar(void) {

  mpc_parser_t* Test = mpc_new();
  
  mpc_define(Test, mpca_grammar("'c'*"));
  
  mpc_print(Test);
  
  mpc_undefine(Test);
  
  mpc_delete(Test);

  mpc_parser_t* Expression = mpc_new();
  mpc_parser_t* Product = mpc_new();
  mpc_parser_t* Value = mpc_new();
  
  mpc_define(Expression, mpca_grammar("<0> (('+' | '-') <0>)*", Product));
  mpc_define(Product,    mpca_grammar("<0> (('*' | '/') <0>)*", Value));
  mpc_define(Value,      mpca_grammar("/[0-9]/ | '(' <0> ')'", Expression));
    
  mpc_print(Expression);
  mpc_print(Product);
  mpc_print(Value);
  
  mpc_ast_t* empty = mpc_ast_empty();

  /*
  PT_ASSERT(mpc_match(Expression, "1", empty, ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Expression, "(5)", empty, ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Expression, "(4*2)+5", empty, ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Expression, "a", empty, ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Expression, "2b+4", empty, ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  */
  
  mpc_ast_delete(empty);
  
  mpc_undefine(Expression);
  mpc_undefine(Product);
  mpc_undefine(Value);
  
  mpc_delete(Expression);
  mpc_delete(Product);
  mpc_delete(Value);

}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
}