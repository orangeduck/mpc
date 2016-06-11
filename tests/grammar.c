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

void test_doge(void) {
  
  mpc_ast_t *t0;  
  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun      = mpc_new("noun");
  mpc_parser_t* Phrase    = mpc_new("phrase");
  mpc_parser_t* Doge      = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT,
    " adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 "
    " noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; "
    " phrase    : <adjective> <noun>;                                     "
    " doge      : /^/ <phrase>* /$/;                                      ",
    Adjective, Noun, Phrase, Doge, NULL);
  
  t0 = 
      mpc_ast_build(4, ">", 
          mpc_ast_new("regex", ""),
          mpc_ast_build(2, "phrase|>", 
            mpc_ast_new("adjective|string", "so"),
            mpc_ast_new("noun|string", "c")),
          mpc_ast_build(2, "phrase|>", 
            mpc_ast_new("adjective|string", "so"),
            mpc_ast_new("noun|string", "c")),
          mpc_ast_new("regex", "")
        );
            
  PT_ASSERT(mpc_test_pass(Doge, "so c so c", t0, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
 
  PT_ASSERT(mpc_test_fail(Doge, "so a so c", t0, (int(*)(const void*,const void*))mpc_ast_eq, (mpc_dtor_t)mpc_ast_delete, (void(*)(const void*))mpc_ast_print));
  
  mpc_ast_delete(t0);
  
  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);
  
}

void test_partial(void) {
  
  mpc_ast_t *t0;
  mpc_err_t *err;

  mpc_parser_t *Line = mpc_new("line");
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *QuotedString = mpc_new("quoted_string");
  mpc_parser_t *LinePragma = mpc_new("linepragma");
  mpc_parser_t *Parser = mpc_new("parser");
  
  mpc_define(Line, mpca_tag(mpc_apply(mpc_sym("#line"), mpcf_str_ast), "string"));
  
  err = mpca_lang(MPCA_LANG_PREDICTIVE,
    "number        : /[0-9]+/ ;\n"
    "quoted_string : /\"(\\.|[^\"])*\"/ ;\n"
    "linepragma    : <line> <number> <quoted_string>;\n"
    "parser        : /^/ (<linepragma>)* /$/ ;\n",
    Line, Number, QuotedString, LinePragma, Parser, NULL);
  
  PT_ASSERT(err == NULL);
  
  t0 = mpc_ast_build(3, ">", 
          mpc_ast_new("regex", ""),
          mpc_ast_build(3, "linepragma|>", 
            mpc_ast_new("line|string", "#line"),
            mpc_ast_new("number|regex", "10"),
            mpc_ast_new("quoted_string|regex", "\"test\"")),
          mpc_ast_new("regex", ""));
  
  PT_ASSERT(mpc_test_pass(Parser, "#line 10 \"test\"", t0, 
    (int(*)(const void*,const void*))mpc_ast_eq, 
    (mpc_dtor_t)mpc_ast_delete, 
    (void(*)(const void*))mpc_ast_print));
  
  mpc_cleanup(5, Line, Number, QuotedString, LinePragma, Parser);

}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
  pt_add_test(test_language, "Test Language", "Suite Grammar");
  pt_add_test(test_language_file, "Test Language File", "Suite Grammar");
  pt_add_test(test_doge, "Test Doge", "Suite Grammar");
  pt_add_test(test_partial, "Test Partial", "Suite Grammar");
}
