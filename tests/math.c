#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static bool int_eq(void* x, void* y) {
  return (*(int*)x == *(int*)y);
}

static void int_print(void* x) {
  printf("'%i'", *((int*)x));
}

bool suite_math(void) {
  
  mpc_parser_t* Expr   = mpc_new();
  mpc_parser_t* Factor = mpc_new();
  mpc_parser_t* Term   = mpc_new();
  mpc_parser_t* Maths  = mpc_new();

  mpc_define(Expr, mpc_either(
    mpc_and(3, mpcf_maths, Factor, mpc_oneof("*/"), Factor, free, free),
    Factor
  ));
  
  mpc_define(Factor, mpc_either(
    mpc_and(3, mpcf_maths, Term, mpc_oneof("+-"), Term, free, free),
    Term
  ));
  
  mpc_define(Term, mpc_either(
    mpc_int(),
    mpc_parens(Expr, free)
  ));
  
  mpc_define(Maths, mpc_ends(Expr, free));
  
  mpc_print(Expr);
  mpc_print(Factor);
  mpc_print(Term);
  mpc_print(Maths);
  
  mpc_test(Maths, "1", (int[]){ 1 }, int_eq, free, int_print);
  mpc_test(Maths, "(5)", (int[]){ 5 }, int_eq, free, int_print);
  mpc_test(Maths, "(4*2)+5", (int[]){ 13 }, int_eq, free, int_print);
  mpc_test(Maths, "a", (int[]){ 0 }, int_eq, free, int_print);
  mpc_test(Maths, "2b+4", (int[]){ 2 }, int_eq, free, int_print);
  
  mpc_undefine(Expr);
  mpc_undefine(Factor);
  mpc_undefine(Term);
  mpc_undefine(Maths);
  
  mpc_delete(Expr);
  mpc_delete(Factor);
  mpc_delete(Term);
  mpc_delete(Maths);
  
  return true;
  
}
