#include "../mpc.h"

int main(int argc, char **argv) {
  
  mpc_parser_t* Number  = mpc_new("number");
  mpc_parser_t* Symbol  = mpc_new("symbol");
  mpc_parser_t* String  = mpc_new("string");
  mpc_parser_t* Comment = mpc_new("comment");
  mpc_parser_t* Sexpr   = mpc_new("sexpr");
  mpc_parser_t* Qexpr   = mpc_new("qexpr");
  mpc_parser_t* Expr    = mpc_new("expr");
  mpc_parser_t* Lispy   = mpc_new("lispy");

  mpca_lang(MPCA_LANG_PREDICTIVE,
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
    
    mpc_result_t r;
    if (mpc_parse_contents(argv[1], Lispy, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
  } else {
    
    mpc_result_t r;
    if (mpc_parse_pipe("<stdin>", stdin, Lispy, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
  
  }

  mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);
  
  return 0;
  
}

