#include "ptest.h"
#include "../mpc.h"

#include <string.h>
#include <stdlib.h>

static int string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }

void test_regex_basic(void) {

  mpc_parser_t *re0, *re1, *re2, *re3, *re4, *re5;

  re0 = mpc_re("abc|bcd");
  re1 = mpc_re("abc|bcd|e");
  re2 = mpc_re("ab()c(ab)*");
  re3 = mpc_re("abc(abdd)?");
  re4 = mpc_re("ab|c(abdd)?");
  re5 = mpc_re("abc(ab|dd)+g$");
  
  PT_ASSERT(mpc_match(re0, "abc", "abc", string_eq, free, string_print));
  PT_ASSERT(mpc_match(re0, "bcd", "bcd", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(re0, "bc", "bc", string_eq, free, string_print));
  PT_ASSERT(mpc_unmatch(re0, "ab", "ab", string_eq, free, string_print));
  PT_ASSERT(mpc_match(re1, "e", "e", string_eq, free, string_print));
  PT_ASSERT(mpc_match(re2, "abc", "abc", string_eq, free, string_print));
  PT_ASSERT(mpc_match(re2, "abcabab", "abcabab", string_eq, free, string_print));
  PT_ASSERT(mpc_match(re2, "abcababd", "abcabab", string_eq, free, string_print));
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  mpc_delete(re3);
  mpc_delete(re4);
  mpc_delete(re5);

}

void test_regex_range(void) {

  mpc_parser_t *re0, *re1, *re2, *re3, *re4;
  
  re0 = mpc_re("abg[abcdef]");
  re1 = mpc_re("y*[a-z]");
  re2 = mpc_re("zz(p+)?[A-Z_0\\]123]*");
  re3 = mpc_re("^[^56hy].*$");
  
  /* TODO: Testing */
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  mpc_delete(re3);
  
}

void suite_regex(void) {
  pt_add_test(test_regex_basic, "Test Regex Basic", "Suite Regex");
  pt_add_test(test_regex_range, "Test Regex Range", "Suite Regex");
}