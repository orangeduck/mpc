#include "ptest.h"
#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static int int_eq(void* x, void* y) { return (*(int*)x == *(int*)y); }
static void int_print(void* x) { printf("'%i'", *((int*)x)); }
static int string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }

void test_ident(void) {

  /* ^[a-zA-Z_][a-zA-Z0-9_]*$ */
  
  mpc_parser_t* Ident = mpc_whole(
    mpc_and(2, mpcf_strfold,
      mpc_or(2, mpc_alpha(), mpc_underscore()),
      mpc_many1(mpcf_strfold, mpc_or(3, mpc_alpha(), mpc_underscore(), mpc_digit())),
      free),
    free
  );
  
  PT_ASSERT(mpc_test_pass(Ident, "test", "test", string_eq, free, string_print));
  PT_ASSERT(mpc_test_fail(Ident, "  blah", "", string_eq, free, string_print));
  PT_ASSERT(mpc_test_pass(Ident, "anoth21er", "anoth21er", string_eq, free, string_print));
  PT_ASSERT(mpc_test_pass(Ident, "du__de", "du__de", string_eq, free, string_print));
  PT_ASSERT(mpc_test_fail(Ident, "some spaces", "", string_eq, free, string_print));
  PT_ASSERT(mpc_test_fail(Ident, "", "", string_eq, free, string_print));
  PT_ASSERT(mpc_test_fail(Ident, "18nums", "", string_eq, free, string_print));
  
  mpc_delete(Ident);

}

void test_maths(void) {
  
  mpc_parser_t *Expr, *Factor, *Term, *Maths; 
  int r0 = 1, r1 = 5, r2 = 13, r3 = 0, r4 = 2;
  
  Expr   = mpc_new("expr");
  Factor = mpc_new("factor");
  Term   = mpc_new("term");
  Maths  = mpc_new("maths");

  mpc_define(Expr, mpc_or(2, 
    mpc_and(3, mpcf_maths, Factor, mpc_oneof("*/"), Factor, free, free),
    Factor
  ));
  
  mpc_define(Factor, mpc_or(2, 
    mpc_and(3, mpcf_maths, Term, mpc_oneof("+-"), Term, free, free),
    Term
  ));
  
  mpc_define(Term, mpc_or(2, 
    mpc_int(),
    mpc_parens(Expr, free)
  ));
  
  mpc_define(Maths, mpc_whole(Expr, free));
  
  PT_ASSERT(mpc_test_pass(Maths, "1", &r0, int_eq, free, int_print));
  PT_ASSERT(mpc_test_pass(Maths, "(5)", &r1, int_eq, free, int_print));
  PT_ASSERT(mpc_test_pass(Maths, "(4*2)+5", &r2, int_eq, free, int_print));
  PT_ASSERT(mpc_test_fail(Maths, "a", &r3, int_eq, free, int_print));
  PT_ASSERT(mpc_test_fail(Maths, "2b+4", &r4, int_eq, free, int_print));
  
  mpc_cleanup(4, Expr, Factor, Term, Maths);
}

void suite_core(void) {
  pt_add_test(test_ident, "Test Ident", "Suite Core");
  pt_add_test(test_maths, "Test Maths", "Suite Core");
}
