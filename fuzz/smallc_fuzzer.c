#include "../mpc.h"

int main(void) {

  mpc_parser_t* Ident     = mpc_new("ident");
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Character = mpc_new("character");
  mpc_parser_t* String    = mpc_new("string");
  mpc_parser_t* Factor    = mpc_new("factor");
  mpc_parser_t* Term      = mpc_new("term");
  mpc_parser_t* Lexp      = mpc_new("lexp");
  mpc_parser_t* Stmt      = mpc_new("stmt");
  mpc_parser_t* Exp       = mpc_new("exp");
  mpc_parser_t* Typeident = mpc_new("typeident");
  mpc_parser_t* Decls     = mpc_new("decls");
  mpc_parser_t* Args      = mpc_new("args");
  mpc_parser_t* Body      = mpc_new("body");
  mpc_parser_t* Procedure = mpc_new("procedure");
  mpc_parser_t* Main      = mpc_new("main");
  mpc_parser_t* Includes  = mpc_new("includes");
  mpc_parser_t* Smallc    = mpc_new("smallc");

  mpc_err_t * err ;
  char language[2048] = { 0, } ;
  size_t s = fread(language, 2047, 1, stdin) ;
  printf("s: %ld\n", s) ;

  err = mpca_lang(MPCA_LANG_DEFAULT, language, 
  	Ident, Number, Character, String, Factor, Term, Lexp, Stmt, Exp, 
    Typeident, Decls, Args, Body, Procedure, Main, Includes, Smallc, NULL);
  
  if (err != NULL) {
    mpc_err_print(err);
    mpc_err_delete(err);
    exit(1);
  }
    
  if (1) {
    
    mpc_result_t r;
    if (mpc_parse_contents("./smallc_exprs/minimal.smallc", Smallc, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
  } 

  mpc_cleanup(17, Ident, Number, Character, String, Factor, Term, Lexp, Stmt, Exp,
                  Typeident, Decls, Args, Body, Procedure, Main, Includes, Smallc);
  
  return 0;
  
}

