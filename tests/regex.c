#include "ptest.h"
#include "../mpc.h"

#include <string.h>
#include <stdlib.h>

static int string_eq(const void* x, const void* y) { return (strcmp(x, y) == 0); }
static void string_print(const void* x) { printf("'%s'", (char*)x); }

int regex_test_pass(mpc_parser_t *p, const char* value, const char* match) {
  return mpc_test_pass(p, value, match, string_eq, free, string_print);
}

int regex_test_fail(mpc_parser_t *p, const char* value, const char* match) {
  return mpc_test_fail(p, value, match, string_eq, free, string_print);
}

void test_regex_basic(void) {

  mpc_parser_t *re0, *re1, *re2, *re3, *re4, *re5;

  re0 = mpc_re("abc|bcd");
  re1 = mpc_re("abc|bcd|e");
  re2 = mpc_re("ab()c(ab)*");
  re3 = mpc_re("abc(abdd)?");
  re4 = mpc_re("ab|c(abdd)?");
  re5 = mpc_re("abc(ab|dd)+g$");
  
  PT_ASSERT(regex_test_pass(re0, "abc", "abc"));
  PT_ASSERT(regex_test_pass(re0, "bcd", "bcd"));
  PT_ASSERT(regex_test_fail(re0, "bc", "bc"));
  PT_ASSERT(regex_test_fail(re0, "ab", "ab"));
  PT_ASSERT(regex_test_pass(re1, "e", "e"));
  PT_ASSERT(regex_test_pass(re2, "abc", "abc"));
  PT_ASSERT(regex_test_pass(re2, "abcabab", "abcabab"));
  PT_ASSERT(regex_test_pass(re2, "abcababd", "abcabab"));
  PT_ASSERT(regex_test_pass(re5, "abcddg", "abcddg"));
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  mpc_delete(re3);
  mpc_delete(re4);
  mpc_delete(re5);

}

void test_regex_boundary(void) {

  mpc_parser_t *re0, *re1, *re2;

  re0 = mpc_re("\\bfoo\\b");
  re1 = mpc_re("(w| )?\\bfoo\\b");
  re2 = mpc_re("py\\B.*");

  PT_ASSERT(regex_test_pass(re0, "foo", "foo"));
  PT_ASSERT(regex_test_pass(re0, "foo.", "foo"));
  PT_ASSERT(regex_test_pass(re0, "foo)", "foo"));
  PT_ASSERT(regex_test_pass(re0, "foo baz", "foo"));
  
  PT_ASSERT(regex_test_fail(re0, "foobar", "foo"));
  PT_ASSERT(regex_test_fail(re0, "foo3", "foo"));
  
  PT_ASSERT(regex_test_pass(re1, "foo", "foo"));
  PT_ASSERT(regex_test_pass(re1, " foo", " foo"));
  PT_ASSERT(regex_test_fail(re1, "wfoo", "foo"));
  
  PT_ASSERT(regex_test_pass(re2, "python", "python"));
  PT_ASSERT(regex_test_pass(re2, "py3", "py3"));
  PT_ASSERT(regex_test_pass(re2, "py2", "py2"));
  PT_ASSERT(regex_test_fail(re2, "py", "py"));
  PT_ASSERT(regex_test_fail(re2, "py.", "py."));
  PT_ASSERT(regex_test_fail(re2, "py!", "py!"));
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  
}

void test_regex_range(void) {

  mpc_parser_t *re0, *re1, *re2, *re3;
  
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

void test_regex_string(void) {
  
  mpc_parser_t *re0 = mpc_re("\"(\\\\.|[^\"])*\"");

  PT_ASSERT(regex_test_pass(re0, "\"there\"", "\"there\""));
  PT_ASSERT(regex_test_pass(re0, "\"hello\"", "\"hello\""));
  PT_ASSERT(regex_test_pass(re0, "\"i am dan\"", "\"i am dan\""));
  PT_ASSERT(regex_test_pass(re0, "\"i a\\\"m dan\"", "\"i a\\\"m dan\""));

  mpc_delete(re0);

}

void test_regex_lisp_comment(void) {
  
  mpc_parser_t *re0 = mpc_re(";[^\\n\\r]*");

  PT_ASSERT(regex_test_pass(re0, ";comment", ";comment"));
  PT_ASSERT(regex_test_pass(re0, ";i am the\nman", ";i am the"));
  
  mpc_delete(re0);
  
}

void suite_regex(void) {
  pt_add_test(test_regex_basic, "Test Regex Basic", "Suite Regex");
  pt_add_test(test_regex_range, "Test Regex Range", "Suite Regex");
  pt_add_test(test_regex_string, "Test Regex String", "Suite Regex");
  pt_add_test(test_regex_lisp_comment, "Test Regex Lisp Comment", "Suite Regex");
  pt_add_test(test_regex_boundary, "Test Regex Boundary", "Suite Regex");
}
