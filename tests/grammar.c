#include "ptest.h"
#include "../mpc.h"

void test_grammar(void) {

  mpc_parser_t* Expr  = mpc_new("expression");
  mpc_parser_t* Prod  = mpc_new("product");
  mpc_parser_t* Value = mpc_new("value");
  mpc_parser_t* Maths = mpc_new("maths");
  
  mpc_define(Expr,  mpca_grammar(" <product> (('+' | '-') <product>)* ", Prod));
  mpc_define(Prod,  mpca_grammar(" <value>   (('*' | '/')   <value>)* ", Value));
  mpc_define(Value, mpca_grammar(" /[0-9]+/ | '(' <expression> ')' ", Expr));
  mpc_define(Maths, mpca_total(Expr));
  
  mpc_ast_t* t0 = mpc_ast_build(1, "root", mpc_ast_new("value", "24"));
  mpc_ast_t* t1 = mpc_ast_build(1, "root",
    mpc_ast_build(3, "value",
      mpc_ast_new("char", "("),
      mpc_ast_new("value", "5"),
      mpc_ast_new("char", ")")));
  
  mpc_ast_t* t2 = mpc_ast_build(3, "root",
      
      mpc_ast_build(3, "value", 
        mpc_ast_new("char", "("),
        mpc_ast_build(3, "expression",
          
          mpc_ast_build(5, "product", 
            mpc_ast_new("value", "4"),
            mpc_ast_new("char", "*"),
            mpc_ast_new("value", "2"),
            mpc_ast_new("char", "*"),
            mpc_ast_new("value", "11")),
            
          mpc_ast_new("char", "+"),
          mpc_ast_new("value", "2")),
        mpc_ast_new("char", ")")),
      
      mpc_ast_new("char", "+"),
      mpc_ast_new("value", "5"));
  
  PT_ASSERT(mpc_match(Maths, "  24 ", t0, (bool(*)(void*,void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Maths, "(5)", t1, (bool(*)(void*,void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_match(Maths, "(4 * 2 * 11 + 2) + 5", t2, (bool(*)(void*,void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_unmatch(Maths, "a", t0, (bool(*)(void*,void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  PT_ASSERT(mpc_unmatch(Maths, "2b+4", t0, (bool(*)(void*,void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(void*))mpc_ast_print));
  
  mpc_ast_delete(t0);
  mpc_ast_delete(t1);
  mpc_ast_delete(t2);
  
  mpc_cleanup(4, Expr, Prod, Value, Maths);
}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
}