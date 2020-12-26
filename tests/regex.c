#include "ptest.h"
#include "../pcq.h"

#include <string.h>
#include <stdlib.h>

static int string_eq(const void* x, const void* y) { return (strcmp(x, y) == 0); }
static void string_print(const void* x) { printf("'%s'", (char*)x); }

int regex_test_pass(pcq_parser_t *p, const char* value, const char* match) {
  return pcq_test_pass(p, value, match, string_eq, free, string_print);
}

int regex_test_fail(pcq_parser_t *p, const char* value, const char* match) {
  return pcq_test_fail(p, value, match, string_eq, free, string_print);
}

void test_regex_basic(void) {

  pcq_parser_t *re0, *re1, *re2, *re3, *re4, *re5;

  re0 = pcq_re("abc|bcd");
  re1 = pcq_re("abc|bcd|e");
  re2 = pcq_re("ab()c(ab)*");
  re3 = pcq_re("abc(abdd)?");
  re4 = pcq_re("ab|c(abdd)?");
  re5 = pcq_re("abc(ab|dd)+g$");

  PT_ASSERT(regex_test_pass(re0, "abc", "abc"));
  PT_ASSERT(regex_test_pass(re0, "bcd", "bcd"));
  PT_ASSERT(regex_test_fail(re0, "bc", "bc"));
  PT_ASSERT(regex_test_fail(re0, "ab", "ab"));
  PT_ASSERT(regex_test_pass(re1, "e", "e"));
  PT_ASSERT(regex_test_pass(re2, "abc", "abc"));
  PT_ASSERT(regex_test_pass(re2, "abcabab", "abcabab"));
  PT_ASSERT(regex_test_pass(re2, "abcababd", "abcabab"));
  PT_ASSERT(regex_test_pass(re5, "abcddg", "abcddg"));

  pcq_delete(re0);
  pcq_delete(re1);
  pcq_delete(re2);
  pcq_delete(re3);
  pcq_delete(re4);
  pcq_delete(re5);

}

void test_regex_boundary(void) {

  pcq_parser_t *re0, *re1, *re2;

  re0 = pcq_re("\\bfoo\\b");
  re1 = pcq_re("(w| )?\\bfoo\\b");
  re2 = pcq_re("py\\B.*");

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

  pcq_delete(re0);
  pcq_delete(re1);
  pcq_delete(re2);

}

void test_regex_range(void) {

  pcq_parser_t *re0, *re1, *re2, *re3;

  re0 = pcq_re("abg[abcdef]");
  re1 = pcq_re("y*[a-z]");
  re2 = pcq_re("zz(p+)?[A-Z_0\\]123]*");
  re3 = pcq_re("^[^56hy].*$");

  /* TODO: Testing */

  pcq_delete(re0);
  pcq_delete(re1);
  pcq_delete(re2);
  pcq_delete(re3);

}

void test_regex_string(void) {

  pcq_parser_t *re0 = pcq_re("\"(\\\\.|[^\"])*\"");

  PT_ASSERT(regex_test_pass(re0, "\"there\"", "\"there\""));
  PT_ASSERT(regex_test_pass(re0, "\"hello\"", "\"hello\""));
  PT_ASSERT(regex_test_pass(re0, "\"i am dan\"", "\"i am dan\""));
  PT_ASSERT(regex_test_pass(re0, "\"i a\\\"m dan\"", "\"i a\\\"m dan\""));

  pcq_delete(re0);

}

void test_regex_lisp_comment(void) {

  pcq_parser_t *re0 = pcq_re(";[^\\n\\r]*");

  PT_ASSERT(regex_test_pass(re0, ";comment", ";comment"));
  PT_ASSERT(regex_test_pass(re0, ";i am the\nman", ";i am the"));

  pcq_delete(re0);

}

void test_regex_newline(void) {

  pcq_parser_t *re0 = pcq_re("[a-z]*");

  PT_ASSERT(regex_test_pass(re0, "hi", "hi"));
  PT_ASSERT(regex_test_pass(re0, "hi\nk", "hi"));
  PT_ASSERT(regex_test_fail(re0, "hi\nk", "hi\nk"));

  pcq_delete(re0);

}

void test_regex_multiline(void) {

  pcq_parser_t *re0 = pcq_re_mode("(^[a-z]*$)*", PCQ_RE_MULTILINE);

  PT_ASSERT(regex_test_pass(re0, "hello\nhello", "hello\nhello"));
  PT_ASSERT(regex_test_pass(re0, "hello\nhello\n", "hello\nhello\n"));
  PT_ASSERT(regex_test_pass(re0, "\nblah\n\nblah\n", "\nblah\n\nblah\n"));
  PT_ASSERT(regex_test_fail(re0, "45234", "45234"));
  PT_ASSERT(regex_test_fail(re0, "\n45234", "\n45234"));
  PT_ASSERT(regex_test_pass(re0, "\n45234", "\n"));

  pcq_delete(re0);

}

void test_regex_dotall(void) {

  pcq_parser_t *re0 = pcq_re_mode("^.*$", PCQ_RE_DEFAULT);
  pcq_parser_t *re1 = pcq_re_mode("^.*$", PCQ_RE_DOTALL);

  PT_ASSERT(regex_test_pass(re0, "hello", "hello"));
  PT_ASSERT(regex_test_fail(re0, "hello\n", "hello"));
  PT_ASSERT(regex_test_fail(re0, "he\nllo\n", "he"));
  PT_ASSERT(regex_test_pass(re0, "34njaksdklmasd", "34njaksdklmasd"));
  PT_ASSERT(regex_test_fail(re0, "34njaksd\nklmasd", "34njaksd"));

  PT_ASSERT(regex_test_pass(re1, "hello", "hello"));
  PT_ASSERT(regex_test_pass(re1, "hello\n", "hello\n"));
  PT_ASSERT(regex_test_pass(re1, "he\nllo\n", "he\nllo\n"));
  PT_ASSERT(regex_test_pass(re1, "34njaksdklmasd", "34njaksdklmasd"));
  PT_ASSERT(regex_test_pass(re1, "34njaksd\nklmasd", "34njaksd\nklmasd"));

  pcq_delete(re0);
  pcq_delete(re1);

}

void suite_regex(void) {
  pt_add_test(test_regex_basic, "Test Regex Basic", "Suite Regex");
  pt_add_test(test_regex_range, "Test Regex Range", "Suite Regex");
  pt_add_test(test_regex_string, "Test Regex String", "Suite Regex");
  pt_add_test(test_regex_lisp_comment, "Test Regex Lisp Comment", "Suite Regex");
  pt_add_test(test_regex_boundary, "Test Regex Boundary", "Suite Regex");
  pt_add_test(test_regex_newline, "Test Regex Newline", "Suite Regex");
  pt_add_test(test_regex_multiline, "Test Regex Multiline", "Suite Regex");
  pt_add_test(test_regex_dotall, "Test Regex Dotall", "Suite Regex");
}
