#include <stdio.h>
#include <string.h>
#include "../mpc.h"

int main(void) {
  
  mpc_result_t r;

  mpc_parser_t *Expr  = mpc_new("expression");
  mpc_parser_t *Prod  = mpc_new("product");
  mpc_parser_t *Value = mpc_new("value");
  mpc_parser_t *Maths = mpc_new("maths");
  
/*  mpca_lang(MPCA_LANG_PREDICTIVE,
    " expression : <product> (('+' | '-') <product>)*; "
    " product : <value>   (('*' | '/')   <value>)*;    "
    " value : /[0-9]+/ | '(' <expression> ')';         "
    " maths : /^/ <expression> /$/;                    ",
    Expr, Prod, Value, Maths, NULL);
*/
  char language[1024] = { 0, } ;
  size_t s = fread(language, 1023, 1, stdin) ;
  printf("s: %ld\n", s) ;

  mpca_lang(MPCA_LANG_PREDICTIVE, language, Expr, Prod, Value, Maths, NULL) ;
  
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

