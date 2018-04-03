#include "ptest.h"
#include "../mpc.h"

static int check_is_a(mpc_val_t** x) {
  return strcmp(*x, "a") == 0;
}

static int check_is(mpc_val_t** x, void* t) {
  return strcmp(*x, t) == 0;
}

void test_check(void) {
  int           success;
  mpc_result_t  r;
  mpc_parser_t* p = mpc_check(mpc_or(2, mpc_char('a'), mpc_char('b')), check_is_a, "Expected 'a'");

  success = mpc_parse("test", "a", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else mpc_err_delete(r.error);

  success = mpc_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else mpc_err_delete(r.error);

  mpc_delete(p);
}

void test_check_with(void) {
  int           success;
  mpc_result_t  r;
  mpc_parser_t* p = mpc_check_with(mpc_or(2, mpc_char('a'), mpc_char('b')), check_is, "a", "Expected 'a'");

  success = mpc_parse("test", "a", p, &r);
  PT_ASSERT(success);
  if (success) PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else mpc_err_delete(r.error);

  success = mpc_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  if (!success) PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else mpc_err_delete(r.error);

  mpc_delete(p);
}

void test_checkf(void) {
  int           success;
  mpc_result_t  r;
  mpc_parser_t* p = mpc_checkf(mpc_or(2, mpc_char('a'), mpc_char('b')), check_is_a, "Expected '%s'", "a");

  success = mpc_parse("test", "a", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else mpc_err_delete(r.error);

  success = mpc_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else mpc_err_delete(r.error);

  mpc_delete(p);
}

void test_check_withf(void) {
  int           success;
  mpc_result_t  r;
  mpc_parser_t* p = mpc_check_withf(mpc_or(2, mpc_char('a'), mpc_char('b')), check_is, "a", "Expected '%s'", "a");

  success = mpc_parse("test", "a", p, &r);
  PT_ASSERT(success);
  if (success) PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else mpc_err_delete(r.error);

  success = mpc_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  if (!success) PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else mpc_err_delete(r.error);

  mpc_delete(p);
}

void suite_combinators(void) {
  pt_add_test(test_check,       "Test Check",        "Suite Combinators");
  pt_add_test(test_check_with,  "Test Check with",   "Suite Combinators");
  pt_add_test(test_checkf,      "Test Check F",      "Suite Combinators");
  pt_add_test(test_check_withf, "Test Check with F", "Suite Combinators");
}
