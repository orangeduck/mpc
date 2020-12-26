#include "ptest.h"
#include "../pcq.h"

void test_grammar(void) {

  pcq_parser_t *Expr, *Prod, *Value, *Maths;
  pcq_ast_t *t0, *t1, *t2;
  
  Expr  = pcq_new("expression");
  Prod  = pcq_new("product");
  Value = pcq_new("value");
  Maths = pcq_new("maths");
  
  pcq_define(Expr,  pcqa_grammar(PCQA_LANG_DEFAULT, " <product> (('+' | '-') <product>)* ", Prod));
  pcq_define(Prod,  pcqa_grammar(PCQA_LANG_DEFAULT, " <value>   (('*' | '/')   <value>)* ", Value));
  pcq_define(Value, pcqa_grammar(PCQA_LANG_DEFAULT, " /[0-9]+/ | '(' <expression> ')' ", Expr));
  pcq_define(Maths, pcqa_total(Expr));
  
  t0 = pcq_ast_new("product|value|regex", "24");
  t1 = pcq_ast_build(1, "product|>",
    pcq_ast_build(3, "value|>",
      pcq_ast_new("char", "("),
      pcq_ast_new("expression|product|value|regex", "5"),
      pcq_ast_new("char", ")")));
  
  t2 = pcq_ast_build(3, ">",
      
      pcq_ast_build(3, "product|value|>",
        pcq_ast_new("char", "("),
        pcq_ast_build(3, "expression|>",
          
          pcq_ast_build(5, "product|>", 
            pcq_ast_new("value|regex", "4"),
            pcq_ast_new("char", "*"),
            pcq_ast_new("value|regex", "2"),
            pcq_ast_new("char", "*"),
            pcq_ast_new("value|regex", "11")),
            
          pcq_ast_new("char", "+"),
          pcq_ast_new("product|value|regex", "2")),
        pcq_ast_new("char", ")")),
      
      pcq_ast_new("char", "+"),
      pcq_ast_new("product|value|regex", "5"));
  
  PT_ASSERT(pcq_test_pass(Maths, "  24 ", t0, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
  PT_ASSERT(pcq_test_pass(Maths, "(5)", t1, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
  PT_ASSERT(pcq_test_pass(Maths, "(4 * 2 * 11 + 2) + 5", t2, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
  PT_ASSERT(pcq_test_fail(Maths, "a", t0, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
  PT_ASSERT(pcq_test_fail(Maths, "2b+4", t0, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));

  pcq_ast_delete(t0);
  pcq_ast_delete(t1);
  pcq_ast_delete(t2);
  
  pcq_cleanup(4, Expr, Prod, Value, Maths);
  
}

void test_language(void) {
  
  pcq_parser_t *Expr, *Prod, *Value, *Maths;
  
  Expr  = pcq_new("expression");
  Prod  = pcq_new("product");
  Value = pcq_new("value");
  Maths = pcq_new("maths");
  
  pcqa_lang(PCQA_LANG_DEFAULT,
    " expression : <product> (('+' | '-') <product>)*; "
    " product : <value>   (('*' | '/')   <value>)*;    "
    " value : /[0-9]+/ | '(' <expression> ')';         "
    " maths : /^/ <expression> /$/;                    ",
    Expr, Prod, Value, Maths);
  
  pcq_cleanup(4, Expr, Prod, Value, Maths);
}

void test_language_file(void) {
  
  pcq_parser_t *Expr, *Prod, *Value, *Maths;
  
  Expr  = pcq_new("expression");
  Prod  = pcq_new("product");
  Value = pcq_new("value");
  Maths = pcq_new("maths");
  
  pcqa_lang_contents(PCQA_LANG_DEFAULT, "./tests/maths.grammar", Expr, Prod, Value, Maths);
  
  pcq_cleanup(4, Expr, Prod, Value, Maths);
  
}

void test_doge(void) {
  
  pcq_ast_t *t0;  
  pcq_parser_t* Adjective = pcq_new("adjective");
  pcq_parser_t* Noun      = pcq_new("noun");
  pcq_parser_t* Phrase    = pcq_new("phrase");
  pcq_parser_t* Doge      = pcq_new("doge");

  pcqa_lang(PCQA_LANG_DEFAULT,
    " adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 "
    " noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; "
    " phrase    : <adjective> <noun>;                                     "
    " doge      : /^/ <phrase>* /$/;                                      ",
    Adjective, Noun, Phrase, Doge, NULL);
  
  t0 = 
      pcq_ast_build(4, ">", 
          pcq_ast_new("regex", ""),
          pcq_ast_build(2, "phrase|>", 
            pcq_ast_new("adjective|string", "so"),
            pcq_ast_new("noun|string", "c")),
          pcq_ast_build(2, "phrase|>", 
            pcq_ast_new("adjective|string", "so"),
            pcq_ast_new("noun|string", "c")),
          pcq_ast_new("regex", "")
        );
            
  PT_ASSERT(pcq_test_pass(Doge, "so c so c", t0, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
 
  PT_ASSERT(pcq_test_fail(Doge, "so a so c", t0, (int(*)(const void*,const void*))pcq_ast_eq, (pcq_dtor_t)pcq_ast_delete, (void(*)(const void*))pcq_ast_print));
  
  pcq_ast_delete(t0);
  
  pcq_cleanup(4, Adjective, Noun, Phrase, Doge);
  
}

void test_partial(void) {
  
  pcq_ast_t *t0;
  pcq_err_t *err;

  pcq_parser_t *Line = pcq_new("line");
  pcq_parser_t *Number = pcq_new("number");
  pcq_parser_t *QuotedString = pcq_new("quoted_string");
  pcq_parser_t *LinePragma = pcq_new("linepragma");
  pcq_parser_t *Parser = pcq_new("parser");
  
  pcq_define(Line, pcqa_tag(pcq_apply(pcq_sym("#line"), pcqf_str_ast), "string"));
  
  err = pcqa_lang(PCQA_LANG_PREDICTIVE,
    "number        : /[0-9]+/ ;\n"
    "quoted_string : /\"(\\.|[^\"])*\"/ ;\n"
    "linepragma    : <line> <number> <quoted_string>;\n"
    "parser        : /^/ (<linepragma>)* /$/ ;\n",
    Line, Number, QuotedString, LinePragma, Parser, NULL);
  
  PT_ASSERT(err == NULL);
  
  t0 = pcq_ast_build(3, ">", 
          pcq_ast_new("regex", ""),
          pcq_ast_build(3, "linepragma|>", 
            pcq_ast_new("line|string", "#line"),
            pcq_ast_new("number|regex", "10"),
            pcq_ast_new("quoted_string|regex", "\"test\"")),
          pcq_ast_new("regex", ""));
  
  PT_ASSERT(pcq_test_pass(Parser, "#line 10 \"test\"", t0, 
    (int(*)(const void*,const void*))pcq_ast_eq, 
    (pcq_dtor_t)pcq_ast_delete, 
    (void(*)(const void*))pcq_ast_print));
    
  pcq_ast_delete(t0);

  pcq_cleanup(5, Line, Number, QuotedString, LinePragma, Parser);

}

void test_qscript(void) {
  
  pcq_ast_t *t0;
  pcq_parser_t *Qscript = pcq_new("qscript");
  pcq_parser_t *Comment = pcq_new("comment");
  pcq_parser_t *Resource = pcq_new("resource");
  pcq_parser_t *Rtype = pcq_new("rtype");
  pcq_parser_t *Rname = pcq_new("rname");
  pcq_parser_t *InnerBlock = pcq_new("inner_block");
  pcq_parser_t *Statement = pcq_new("statement");
  pcq_parser_t *Function = pcq_new("function");
  pcq_parser_t *Parameter = pcq_new("parameter");
  pcq_parser_t *Literal = pcq_new("literal");
  pcq_parser_t *Block = pcq_new("block");
  pcq_parser_t *Seperator = pcq_new("seperator");
  pcq_parser_t *Qstring = pcq_new("qstring");
  pcq_parser_t *SimpleStr = pcq_new("simplestr");
  pcq_parser_t *ComplexStr = pcq_new("complexstr");
  pcq_parser_t *Number = pcq_new("number");
  pcq_parser_t *Float = pcq_new("float");
  pcq_parser_t *Int = pcq_new("int");
  
  pcq_err_t *err = pcqa_lang(0,
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
  
  t0 = pcq_ast_build(3, ">",
          pcq_ast_new("regex", ""),
          pcq_ast_build(5, "resource|>",
            pcq_ast_new("char", "["),
            pcq_ast_new("rtype|regex", ""),
            pcq_ast_new("rname|qstring|simplestr|regex", "my_func"),
            pcq_ast_new("char", "]"),
            pcq_ast_build(5, "inner_block|statement|>",
              pcq_ast_new("function|qstring|simplestr|regex", "echo"),
              pcq_ast_new("char", "("),
              pcq_ast_build(2, "parameter|literal|>",
                pcq_ast_build(2, "qstring|>",
                  pcq_ast_new("simplestr|regex", "a"),
                  pcq_ast_build(2, "qstring|>",
                    pcq_ast_new("simplestr|regex", "b"),
                    pcq_ast_new("qstring|simplestr|regex", "c")
                  )
                ),
                pcq_ast_new("seperator|string", "")
              ),
              pcq_ast_new("char", ")"),
              pcq_ast_new("seperator|string", "")
            )
          ),
          pcq_ast_new("regex", ""));
  
  PT_ASSERT(pcq_test_pass(Qscript, "[my_func]\n  echo (a b c)\n", t0,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));
  
  pcq_ast_delete(t0);

  pcq_cleanup(18, Qscript, Comment, Resource, Rtype, Rname, InnerBlock,
  Statement, Function, Parameter, Literal, Block, Seperator, Qstring,
  SimpleStr, ComplexStr, Number, Float, Int);
  
}

void test_missingrule(void) {
  
  int result;
  pcq_err_t *err;
  pcq_result_t r;
  pcq_parser_t *Parser = pcq_new("parser");
  
  err = pcqa_lang(PCQA_LANG_DEFAULT,
    "parser        : /^/ (<missing>)* /$/ ;\n",
    Parser, NULL);
  
  PT_ASSERT(err == NULL);
  
  result = pcq_parse("<stdin>", "test", Parser, &r);
  
  PT_ASSERT(result == 0);
  PT_ASSERT(r.error != NULL);
  PT_ASSERT(strcmp(r.error->failure, "Unknown Parser 'missing'!") == 0);
  
  pcq_err_delete(r.error);
  pcq_cleanup(1, Parser);

}

void test_regex_mode(void) {
  
  pcq_parser_t *Line0, *Line1, *Line2, *Line3;
  pcq_ast_t *t0, *t1, *t2, *t3, *t4;
  
  Line0 = pcq_new("line0");
  Line1 = pcq_new("line1");
  Line2 = pcq_new("line2");
  Line3 = pcq_new("line3");
  
  pcqa_lang(PCQA_LANG_DEFAULT, " line0 : /.*/; ", Line0);
  pcqa_lang(PCQA_LANG_DEFAULT, " line1 : /.*/s; ", Line1);
  pcqa_lang(PCQA_LANG_DEFAULT, " line2 : /(^[a-z]*$)*/; ", Line2);
  pcqa_lang(PCQA_LANG_DEFAULT, " line3 : /(^[a-z]*$)*/m; ", Line3);
  
  t0 = pcq_ast_new("regex", "blah");
  t1 = pcq_ast_new("regex", "blah\nblah");
  t2 = pcq_ast_new("regex", "");
  t3 = pcq_ast_new("regex", "blah");
  t4 = pcq_ast_new("regex", "blah\nblah");
  
  PT_ASSERT(pcq_test_pass(Line0, "blah\nblah", t0,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));
  
  PT_ASSERT(pcq_test_pass(Line1, "blah\nblah", t1,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));
  
  PT_ASSERT(pcq_test_pass(Line2, "blah\nblah", t2,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));

  PT_ASSERT(pcq_test_pass(Line2, "blah", t3,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));
    
  PT_ASSERT(pcq_test_pass(Line3, "blah\nblah", t4,
    (int(*)(const void*,const void*))pcq_ast_eq,
    (pcq_dtor_t)pcq_ast_delete,
    (void(*)(const void*))pcq_ast_print));
  
  pcq_ast_delete(t0);
  pcq_ast_delete(t1);
  pcq_ast_delete(t2);
  pcq_ast_delete(t3);
  pcq_ast_delete(t4);
  
  pcq_cleanup(4, Line0, Line1, Line2, Line3);
}

void test_digits_file(void) {
  
  FILE *f;
  pcq_result_t r;
  pcq_parser_t *Digit = pcq_new("digit");
  pcq_parser_t *Program = pcq_new("program");
  pcq_ast_t* t0;
  
  pcq_err_t* err = pcqa_lang(PCQA_LANG_DEFAULT,
    " digit   : /[0-9]/ ;"
    " program : /^/ <digit>+ /$/ ;"
    , Digit, Program, NULL);
  
  PT_ASSERT(err == NULL);

  t0 = pcq_ast_build(5, ">", 
    pcq_ast_new("regex", ""),
    pcq_ast_new("digit|regex", "1"),
    pcq_ast_new("digit|regex", "2"),
    pcq_ast_new("digit|regex", "3"),
    pcq_ast_new("regex", ""));
  
  if (pcq_parse_contents("tests/digits.txt", Program, &r)) {
    PT_ASSERT(1);
    PT_ASSERT(pcq_ast_eq(t0, r.output));
    pcq_ast_delete(r.output);
  } else {
    PT_ASSERT(0);
    pcq_err_print(r.error);
    pcq_err_delete(r.error);
  }
  
  f = fopen("tests/digits.txt", "r");
  PT_ASSERT(f != NULL);

  if (pcq_parse_file("tests/digits.txt", f, Program, &r)) {
    PT_ASSERT(1);
    PT_ASSERT(pcq_ast_eq(t0, r.output));
    pcq_ast_delete(r.output);
  } else {
    PT_ASSERT(0);
    pcq_err_print(r.error);
    pcq_err_delete(r.error);
  }
    
  fclose(f);
    
  if (pcq_parse("tests/digits.txt", "123", Program, &r)) {
    PT_ASSERT(1);
    PT_ASSERT(pcq_ast_eq(t0, r.output));
    pcq_ast_delete(r.output);
  } else {
    PT_ASSERT(0);
    pcq_err_print(r.error);
    pcq_err_delete(r.error);
  }
  
  pcq_ast_delete(t0);
    
  pcq_cleanup(2, Digit, Program);
  
}

void suite_grammar(void) {
  pt_add_test(test_grammar, "Test Grammar", "Suite Grammar");
  pt_add_test(test_language, "Test Language", "Suite Grammar");
  pt_add_test(test_language_file, "Test Language File", "Suite Grammar");
  pt_add_test(test_doge, "Test Doge", "Suite Grammar");
  pt_add_test(test_partial, "Test Partial", "Suite Grammar");
  pt_add_test(test_qscript, "Test QScript", "Suite Grammar");
  pt_add_test(test_missingrule, "Test Missing Rule", "Suite Grammar");
  pt_add_test(test_regex_mode, "Test Regex Mode", "Suite Grammar");
  pt_add_test(test_digits_file, "Test Digits File", "Suite Grammar");
}
