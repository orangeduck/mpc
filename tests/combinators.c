#include "ptest.h"
#include "../pcq.h"

static int check_is_a(pcq_val_t** x) {
  return strcmp(*x, "a") == 0;
}

static int check_is(pcq_val_t** x, void* t) {
  return strcmp(*x, t) == 0;
}

void test_check(void) {
  int success;
  pcq_result_t  r;
  pcq_parser_t* p = pcq_check(pcq_or(2, pcq_char('a'), pcq_char('b')), free, check_is_a, "Expected 'a'");

  success = pcq_parse("test", "a", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else pcq_err_delete(r.error);

  success = pcq_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else pcq_err_delete(r.error);

  pcq_delete(p);
}

void test_check_with(void) {
  int success;
  pcq_result_t  r;
  pcq_parser_t* p = pcq_check_with(pcq_or(2, pcq_char('a'), pcq_char('b')), free, check_is, (void*)"a", "Expected 'a'");

  success = pcq_parse("test", "a", p, &r);
  PT_ASSERT(success);
  if (success) PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else pcq_err_delete(r.error);

  success = pcq_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  if (!success) PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else pcq_err_delete(r.error);

  pcq_delete(p);
}

void test_checkf(void) {
  int success;
  pcq_result_t  r;
  pcq_parser_t* p = pcq_checkf(pcq_or(2, pcq_char('a'), pcq_char('b')), free, check_is_a, "Expected '%s'", "a");

  success = pcq_parse("test", "a", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else pcq_err_delete(r.error);

  success = pcq_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else pcq_err_delete(r.error);

  pcq_delete(p);
}

void test_check_withf(void) {
  int success;
  pcq_result_t  r;
  pcq_parser_t* p = pcq_check_withf(pcq_or(2, pcq_char('a'), pcq_char('b')), free, check_is, (void*)"a", "Expected '%s'", "a");

  success = pcq_parse("test", "a", p, &r);
  PT_ASSERT(success);
  if (success) PT_ASSERT_STR_EQ(r.output, "a");
  if (success) free(r.output); else pcq_err_delete(r.error);

  success = pcq_parse("test", "b", p, &r);
  PT_ASSERT(!success);
  if (!success) PT_ASSERT_STR_EQ(r.error->failure, "Expected 'a'");
  if (success) free(r.output); else pcq_err_delete(r.error);

  pcq_delete(p);
}

void suite_combinators(void) {
  pt_add_test(test_check,       "Test Check",        "Suite Combinators");
  pt_add_test(test_check_with,  "Test Check with",   "Suite Combinators");
  pt_add_test(test_checkf,      "Test Check F",      "Suite Combinators");
  pt_add_test(test_check_withf, "Test Check with F", "Suite Combinators");
}
