#include "../pcq.h"

int main(int argc, char **argv) {
  
  pcq_result_t r;  
  
  pcq_parser_t* Number  = pcq_new("number");
  pcq_parser_t* Symbol  = pcq_new("symbol");
  pcq_parser_t* String  = pcq_new("string");
  pcq_parser_t* Comment = pcq_new("comment");
  pcq_parser_t* Sexpr   = pcq_new("sexpr");
  pcq_parser_t* Qexpr   = pcq_new("qexpr");
  pcq_parser_t* Expr    = pcq_new("expr");
  pcq_parser_t* Lispy   = pcq_new("lispy");

  pcqa_lang(PCQA_LANG_PREDICTIVE,
    " number  \"number\"  : /[0-9]+/ ;                         "
    " symbol  \"symbol\"  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; "
    " string  \"string\"  : /\"(\\\\.|[^\"])*\"/ ;             "
    " comment             : /;[^\\r\\n]*/ ;                    "
    " sexpr               : '(' <expr>* ')' ;                  "
    " qexpr               : '{' <expr>* '}' ;                  "
    " expr                : <number>  | <symbol> | <string>    "
    "                     | <comment> | <sexpr>  | <qexpr> ;   "
    " lispy               : /^/ <expr>* /$/ ;                  ",
    Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy, NULL);
  
  if (argc > 1) {

    if (pcq_parse_contents(argv[1], Lispy, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
        
  } else {
    
    if (pcq_parse_pipe("<stdin>", stdin, Lispy, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
  
  }

  pcq_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);
  
  return 0;
  
}

