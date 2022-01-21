#include <stdio.h>
#include <string.h>
#include "../mpc.h"

int main(int argc, char * argv[]) {
  
  mpc_result_t r; 

  mpc_parser_t *Expr  = mpc_new("expression");
  mpc_parser_t *Prod  = mpc_new("product");
  mpc_parser_t *Value = mpc_new("value");
  mpc_parser_t *Maths = mpc_new("maths");
  
  FILE * fp = 0x0 ;
  if (argc == 2) {
	  fp = fopen(argv[1], "rb") ;
  }
  else {
	  return 0 ;
  }
  
  mpca_lang_file(MPCA_LANG_PREDICTIVE, fp, Expr, Prod, Value, Maths, NULL) ;
  fclose(fp) ;

  mpc_print(Expr);
  mpc_print(Prod);
  mpc_print(Value);
  mpc_print(Maths);
  
  if (mpc_parse_contents("./math_exprs/readme.maths", Maths, &r)) {
  	mpc_ast_print(r.output);
   	mpc_ast_delete(r.output);
  } else {
   	mpc_err_print(r.error);
   	mpc_err_delete(r.error);
  }

  mpc_cleanup(4, Expr, Prod, Value, Maths);
  
  return 0;
  
}

