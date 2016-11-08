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
      
      mpc_ast_build(3, "product|value|>",
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

void test_qscript(void) {
  
  mpc_ast_t *t0;
  mpc_parser_t *Qscript = mpc_new("qscript");
  mpc_parser_t *Comment = mpc_new("comment");
  mpc_parser_t *Resource = mpc_new("resource");
  mpc_parser_t *Rtype = mpc_new("rtype");
  mpc_parser_t *Rname = mpc_new("rname");
  mpc_parser_t *InnerBlock = mpc_new("inner_block");
  mpc_parser_t *Statement = mpc_new("statement");
  mpc_parser_t *Function = mpc_new("function");
  mpc_parser_t *Parameter = mpc_new("parameter");
  mpc_parser_t *Literal = mpc_new("literal");
  mpc_parser_t *Block = mpc_new("block");
  mpc_parser_t *Seperator = mpc_new("seperator");
  mpc_parser_t *Qstring = mpc_new("qstring");
  mpc_parser_t *SimpleStr = mpc_new("simplestr");
  mpc_parser_t *ComplexStr = mpc_new("complexstr");
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Float = mpc_new("float");
  mpc_parser_t *Int = mpc_new("int");
  
  mpc_err_t *err = mpca_lang(0,
    "  qscript        : /^/ (<comment> | <resource>)* /$/ ;\n"
    "   comment     : '#' /[^\\n]*/ ;\n"
    "resource       : '[' (<rtype> <rname>) ']' <inner_block> ;\n"
    "   rtype       : /[*]*/ ;\n"
    "   rname       : <qstring> ;\n"
    "\n"
    "inner_block    : (<comment> | <statement>)* ;\n"
    "   statement   : <function> '(' (<comment> | <parameter> | <block>)* ')'  <seperator> ;\n"
    "   function    : <qstring> ;\n"
    "   parameter   : (<statement> | <literal>) ;\n"
    "      literal  : (<number> | <qstring>) <seperator> ;\n"
    "   block       : '{' <inner_block> '}' ;\n"
    "   seperator   : ',' | \"\" ;\n"
    "\n"
    "qstring        : (<complexstr> | <simplestr>) <qstring>* ;\n"
    "   simplestr   : /[a-zA-Z0-9_!@#$%^&\\*_+\\-\\.=\\/<>]+/ ;\n"
    "   complexstr  : (/\"[^\"]*\"/ | /'[^']*'/) ;\n"
    "\n"
    "number         : (<float> | <int>) ;\n"
    "   float       : /[-+]?[0-9]+\\.[0-9]+/ ;\n"
    "   int         : /[-+]?[0-9]+/ ;\n",
  Qscript, Comment, Resource, Rtype, Rname, InnerBlock, Statement, Function,
  Parameter, Literal, Block, Seperator, Qstring, SimpleStr, ComplexStr, Number,
  Float, Int, NULL);
 
  PT_ASSERT(err == NULL);
  
  t0 = mpc_ast_build(3, ">",
          mpc_ast_new("regex", ""),
          mpc_ast_build(5, "resource|>",
            mpc_ast_new("char", "["),
            mpc_ast_new("rtype|regex", ""),
            mpc_ast_new("rname|qstring|simplestr|regex", "my_func"),
            mpc_ast_new("char", "]"),
            mpc_ast_build(5, "inner_block|statement|>",
              mpc_ast_new("function|qstring|simplestr|regex", "echo"),
              mpc_ast_new("char", "("),
              mpc_ast_build(2, "parameter|literal|>",
                mpc_ast_build(2, "qstring|>",
                  mpc_ast_new("simplestr|regex", "a"),
                  mpc_ast_build(2, "qstring|>",
                    mpc_ast_new("simplestr|regex", "b"),
                    mpc_ast_new("qstring|simplestr|regex", "c")
                  )
                ),
                mpc_ast_new("seperator|string", "")
              ),
              mpc_ast_new("char", ")"),
              mpc_ast_new("seperator|string", "")
            )
          ),
          mpc_ast_new("regex", ""));
  
  PT_ASSERT(mpc_test_pass(Qscript, "[my_func]\n  echo (a b c)\n", t0,
    (int(*)(const void*,const void*))mpc_ast_eq,
    (mpc_dtor_t)mpc_ast_delete,
    (void(*)(const void*))mpc_ast_print));
  
  mpc_cleanup(18, Qscript, Comment, Resource, Rtype, Rname, InnerBlock,
  Statement, Function, Parameter, Literal, Block, Seperator, Qstring,
  SimpleStr, ComplexStr, Number, Float, Int);
  
}

void test_missingrule(void) {
  
  int result;
  mpc_err_t *err;
  mpc_result_t r;
  mpc_parser_t *Parser = mpc_new("parser");
  
  err = mpca_lang(MPCA_LANG_DEFAULT,
    "parser        : /^/ (<missing>)* /$/ ;\n",
    Parser, NULL);
  
  PT_ASSERT(err == NULL);
  
  result = mpc_parse("<stdin>", "test", Parser, &r);
  
  PT_ASSERT(result == 0);
  PT_ASSERT(r.error != NULL);
  PT_ASSERT(strcmp(r.error->failure, "Unknown Parser 'missing'!") == 0);
  
  mpc_err_delete(r.error);
  mpc_cleanup(1, Parser);

}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
  pt_add_test(test_language, "Test Language", "Suite Grammar");
  pt_add_test(test_language_file, "Test Language File", "Suite Grammar");
  pt_add_test(test_doge, "Test Doge", "Suite Grammar");
  pt_add_test(test_partial, "Test Partial", "Suite Grammar");
  pt_add_test(test_qscript, "Test QScript", "Suite Grammar");
  pt_add_test(test_missingrule, "Test Missing Rule", "Suite Grammar");
}
