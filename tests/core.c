#include "ptest.h"
#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static int int_eq(const void* x, const void* y) { return (*(int*)x == *(int*)y); }
static void int_print(const void* x) { printf("'%i'", *((int*)x)); }
static int streq(const void* x, const void* y) { return (strcmp(x, y) == 0); }
static void strprint(const void* x) { printf("'%s'", (char*)x); }

void test_ident(void) {

  /* ^[a-zA-Z_][a-zA-Z0-9_]*$ */
  
  mpc_parser_t* Ident = mpc_whole(
    mpc_and(2, mpcf_strfold,
      mpc_or(2, mpc_alpha(), mpc_underscore()),
      mpc_many1(mpcf_strfold, mpc_or(3, mpc_alpha(), mpc_underscore(), mpc_digit())),
      free),
    free
  );
  
  PT_ASSERT(mpc_test_pass(Ident, "test", "test", streq, free, strprint));
  PT_ASSERT(mpc_test_fail(Ident, "  blah", "", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(Ident, "anoth21er", "anoth21er", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(Ident, "du__de", "du__de", streq, free, strprint));
  PT_ASSERT(mpc_test_fail(Ident, "some spaces", "", streq, free, strprint));
  PT_ASSERT(mpc_test_fail(Ident, "", "", streq, free, strprint));
  PT_ASSERT(mpc_test_fail(Ident, "18nums", "", streq, free, strprint));
  
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

void test_strip(void) {
  
  mpc_parser_t *Stripperl = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtriml);
  mpc_parser_t *Stripperr = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtrimr);
  mpc_parser_t *Stripper  = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtrim);
  
  PT_ASSERT(mpc_test_pass(Stripperl, " asdmlm dasd  ", "asdmlm dasd  ", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(Stripperr, " asdmlm dasd  ", " asdmlm dasd", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(Stripper,  " asdmlm dasd  ", "asdmlm dasd", streq, free, strprint));
  
  mpc_delete(Stripperl);
  mpc_delete(Stripperr);
  mpc_delete(Stripper);
  
}

void test_repeat(void) {
  
  int success;
  mpc_result_t r;
  mpc_parser_t *p = mpc_count(3, mpcf_strfold, mpc_digit(), free);
  
  success = mpc_parse("test", "046", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);
  
  success = mpc_parse("test", "046aa", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);
  
  success = mpc_parse("test", "04632", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);
  
  success = mpc_parse("test", "04", p, &r);
  PT_ASSERT(!success);
  mpc_err_delete(r.error);
  
  mpc_delete(p);
  
}

void test_copy(void) {
  
  int success;
  mpc_result_t r;
  mpc_parser_t* p = mpc_or(2, mpc_char('a'), mpc_char('b'));
  mpc_parser_t* q = mpc_and(2, mpcf_strfold, p, mpc_copy(p), free);
  
  success = mpc_parse("test", "aa", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "aa");
  free(r.output);

  success = mpc_parse("test", "bb", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "bb");
  free(r.output);
  
  success = mpc_parse("test", "ab", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "ab");
  free(r.output);
  
  success = mpc_parse("test", "ba", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "ba");
  free(r.output);
  
  success = mpc_parse("test", "c", p, &r);
  PT_ASSERT(!success);
  mpc_err_delete(r.error);
  
  mpc_delete(mpc_copy(p));
  mpc_delete(mpc_copy(q));
  
  mpc_delete(q);
  
}

void suite_core(void) {
  pt_add_test(test_ident,  "Test Ident",  "Suite Core");
  pt_add_test(test_maths,  "Test Maths",  "Suite Core");
  pt_add_test(test_strip,  "Test Strip",  "Suite Core");
  pt_add_test(test_repeat, "Test Repeat", "Suite Core");
  pt_add_test(test_copy,   "Test Copy",   "Suite Core");
}
