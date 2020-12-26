#include "../pcq.h"

int main(int argc, char **argv) {

  pcq_parser_t* Ident     = pcq_new("ident");
  pcq_parser_t* Number    = pcq_new("number");
  pcq_parser_t* Character = pcq_new("character");
  pcq_parser_t* String    = pcq_new("string");
  pcq_parser_t* Factor    = pcq_new("factor");
  pcq_parser_t* Term      = pcq_new("term");
  pcq_parser_t* Lexp      = pcq_new("lexp");
  pcq_parser_t* Stmt      = pcq_new("stmt");
  pcq_parser_t* Exp       = pcq_new("exp");
  pcq_parser_t* Typeident = pcq_new("typeident");
  pcq_parser_t* Decls     = pcq_new("decls");
  pcq_parser_t* Args      = pcq_new("args");
  pcq_parser_t* Body      = pcq_new("body");
  pcq_parser_t* Procedure = pcq_new("procedure");
  pcq_parser_t* Main      = pcq_new("main");
  pcq_parser_t* Includes  = pcq_new("includes");
  pcq_parser_t* Smallc    = pcq_new("smallc");

  pcq_err_t* err = pcqa_lang(PCQA_LANG_DEFAULT,
    " ident     : /[a-zA-Z_][a-zA-Z0-9_]*/ ;                           \n"
    " number    : /[0-9]+/ ;                                           \n"
    " character : /'.'/ ;                                              \n"
    " string    : /\"(\\\\.|[^\"])*\"/ ;                               \n"
    "                                                                  \n"
    " factor    : '(' <lexp> ')'                                       \n"
    "           | <number>                                             \n"
    "           | <character>                                          \n"
    "           | <string>                                             \n"
    "           | <ident> '(' <lexp>? (',' <lexp>)* ')'                \n"
    "           | <ident> ;                                            \n"
    "                                                                  \n"
    " term      : <factor> (('*' | '/' | '%') <factor>)* ;             \n"
    " lexp      : <term> (('+' | '-') <term>)* ;                       \n"
    "                                                                  \n"
    " stmt      : '{' <stmt>* '}'                                      \n"
    "           | \"while\" '(' <exp> ')' <stmt>                       \n"
    "           | \"if\"    '(' <exp> ')' <stmt>                       \n"
    "           | <ident> '=' <lexp> ';'                               \n"
    "           | \"print\" '(' <lexp>? ')' ';'                        \n"
    "           | \"return\" <lexp>? ';'                               \n"
    "           | <ident> '(' <ident>? (',' <ident>)* ')' ';' ;        \n"
    "                                                                  \n"
    " exp       : <lexp> '>' <lexp>                                    \n"
    "           | <lexp> '<' <lexp>                                    \n"
    "           | <lexp> \">=\" <lexp>                                 \n"
    "           | <lexp> \"<=\" <lexp>                                 \n"
    "           | <lexp> \"!=\" <lexp>                                 \n"
    "           | <lexp> \"==\" <lexp> ;                               \n"
    "                                                                  \n"
    " typeident : (\"int\" | \"char\") <ident> ;                       \n"
    " decls     : (<typeident> ';')* ;                                 \n"
    " args      : <typeident>? (',' <typeident>)* ;                    \n"
    " body      : '{' <decls> <stmt>* '}' ;                            \n"
    " procedure : (\"int\" | \"char\") <ident> '(' <args> ')' <body> ; \n"
    " main      : \"main\" '(' ')' <body> ;                            \n"
    " includes  : (\"#include\" <string>)* ;                           \n"
    " smallc    : /^/ <includes> <decls> <procedure>* <main> /$/ ;     \n",
    Ident, Number, Character, String, Factor, Term, Lexp, Stmt, Exp, 
    Typeident, Decls, Args, Body, Procedure, Main, Includes, Smallc, NULL);
  
  if (err != NULL) {
    pcq_err_print(err);
    pcq_err_delete(err);
    exit(1);
  }
    
  if (argc > 1) {
    
    pcq_result_t r;
    if (pcq_parse_contents(argv[1], Smallc, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
    
  } else {
    
    pcq_result_t r;
    if (pcq_parse_pipe("<stdin>", stdin, Smallc, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
  
  }

  pcq_cleanup(17, Ident, Number, Character, String, Factor, Term, Lexp, Stmt, Exp,
                  Typeident, Decls, Args, Body, Procedure, Main, Includes, Smallc);
  
  return 0;
  
}

