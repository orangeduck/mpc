#include "ptest.h"
#include "../mpc.h"

void test_grammar(void) {

  mpc_parser_t *Expr, *Prod, *Value, *Maths;
  mpc_ast_t *t0, *t1, *t2;
  
  Expr  = mpc_new("expression");
  Prod  = mpc_new("product");
  Value = mpc_new("value");
  Maths = mpc_new("maths");
  
  mpc_define(Expr,  mpca_grammar(MPCA_LANG_DEFAULT, " <product> (('+' | '-') <product>)* ", Prod));
  mpc_define(Prod,  mpca_grammar(MPCA_LANG_DEFAULT, " <value>   (('*' | '/')   <value>)* ", Value));
  mpc_define(Value, mpca_grammar(MPCA_LANG_DEFAULT, " /[0-9]+/ | '(' <expression> ')' ", Expr));
  mpc_define(Maths, mpca_total(Expr));
  
  t0 = mpc_ast_new("product|value|regex", "24");
  t1 = mpc_ast_build(1, "product|>",
    mpc_ast_build(3, "value|>",
      mpc_ast_new("char", "("),
      mpc_ast_new("expression|product|value|regex", "5"),
      mpc_ast_new("char", ")")));
  
  t2 = mpc_ast_build(3, ">",
      
      mpc_ast_build(3, "value|>", 
        mpc_ast_new("char", "("),
        mpc_ast_build(3, "expression|>",
          
          mpc_ast_build(5, "product|>", 
            mpc_ast_new("value|regex", "4"),
            mpc_ast_new("char", "*"),
            mpc_ast_new("value|regex", "2"),
            mpc_ast_new("char", "*"),
            mpc_ast_new("value|regex", "11")),
            
          mpc_ast_new("char", "+"),
          mpc_ast_new("product|value|regex", "2")),
        mpc_ast_new("char", ")")),
      
      mpc_ast_new("char", "+"),
      mpc_ast_new("product|value|regex", "5"));
  
  PT_ASSERT(mpc_test_pass(Maths, "  24 ", t0, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
  PT_ASSERT(mpc_test_pass(Maths, "(5)", t1, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
  PT_ASSERT(mpc_test_pass(Maths, "(4 * 2 * 11 + 2) + 5", t2, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
  PT_ASSERT(mpc_test_fail(Maths, "a", t0, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
  PT_ASSERT(mpc_test_fail(Maths, "2b+4", t0, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));

  mpc_ast_delete(t0);
  mpc_ast_delete(t1);
  mpc_ast_delete(t2);
  
  mpc_cleanup(4, Expr, Prod, Value, Maths);
  
}

void test_language(void) {
  
  mpc_parser_t *Expr, *Prod, *Value, *Maths;
  
  Expr  = mpc_new("expression");
  Prod  = mpc_new("product");
  Value = mpc_new("value");
  Maths = mpc_new("maths");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    " expression : <product> (('+' | '-') <product>)*; "
    " product : <value>   (('*' | '/')   <value>)*;    "
    " value : /[0-9]+/ | '(' <expression> ')';         "
    " maths : /^/ <expression> /$/;                    ",
    Expr, Prod, Value, Maths);
  
  mpc_cleanup(4, Expr, Prod, Value, Maths);
}

void test_language_file(void) {
  
  mpc_parser_t *Expr, *Prod, *Value, *Maths;
  
  Expr  = mpc_new("expression");
  Prod  = mpc_new("product");
  Value = mpc_new("value");
  Maths = mpc_new("maths");
  
  mpca_lang_contents(MPCA_LANG_DEFAULT, "./tests/maths.grammar", Expr, Prod, Value, Maths);
  
  mpc_cleanup(4, Expr, Prod, Value, Maths);
  
}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
  pt_add_test(test_language, "Test Language", "Suite Grammar");
  pt_add_test(test_language_file, "Test Language File", "Suite Grammar");
}
