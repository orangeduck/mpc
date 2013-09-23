#include "ptest.h"
#include "../mpc.h"

#include <string.h>
#include <stdlib.h>

static bool string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }

void test_regex_basic(void) {

  mpc_parser_t* re0 = mpc_re("abc|bcd");
  mpc_parser_t* re1 = mpc_re("abc|bcd|e");
  mpc_parser_t* re2 = mpc_re("ab()c(ab)*");
  mpc_parser_t* re3 = mpc_re("abc(abdd)?");
  mpc_parser_t* re4 = mpc_re("ab|c(abdd)?");
  mpc_parser_t* re5 = mpc_re("abc(ab|dd)+g$");
  
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

  mpc_parser_t* re0 = mpc_re("abg[abcdef]");
  mpc_parser_t* re1 = mpc_re("y*[a-z]");
  mpc_parser_t* re2 = mpc_re("zz(p+)?[A-Z_0\\]123]*");
  mpc_parser_t* re3 = mpc_re("[^56hy].*$");

  mpc_print(re0);
  mpc_print(re1);
  mpc_print(re2);
  mpc_print(re3);
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  mpc_delete(re3);
  
}

void suite_regex(void) {
  pt_add_test(test_regex_basic, "Test Regex Basic", "Suite Regex");
  pt_add_test(test_regex_range, "Test Regex Range", "Suite Regex");
}