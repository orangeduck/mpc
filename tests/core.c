#include "ptest.h"
#include "../pcq.h"

#include <stdlib.h>
#include <string.h>

static int int_eq(const void* x, const void* y) { return (*(int*)x == *(int*)y); }
static void int_print(const void* x) { printf("'%i'", *((int*)x)); }
static int streq(const void* x, const void* y) { return (strcmp(x, y) == 0); }
static void strprint(const void* x) { printf("'%s'", (char*)x); }

void test_ident(void) {

  /* ^[a-zA-Z_][a-zA-Z0-9_]*$ */

  pcq_parser_t* Ident = pcq_whole(
    pcq_and(2, pcqf_strfold,
      pcq_or(2, pcq_alpha(), pcq_underscore()),
      pcq_many1(pcqf_strfold, pcq_or(3, pcq_alpha(), pcq_underscore(), pcq_digit())),
      free),
    free
  );

  PT_ASSERT(pcq_test_pass(Ident, "test", "test", streq, free, strprint));
  PT_ASSERT(pcq_test_fail(Ident, "  blah", "", streq, free, strprint));
  PT_ASSERT(pcq_test_pass(Ident, "anoth21er", "anoth21er", streq, free, strprint));
  PT_ASSERT(pcq_test_pass(Ident, "du__de", "du__de", streq, free, strprint));
  PT_ASSERT(pcq_test_fail(Ident, "some spaces", "", streq, free, strprint));
  PT_ASSERT(pcq_test_fail(Ident, "", "", streq, free, strprint));
  PT_ASSERT(pcq_test_fail(Ident, "18nums", "", streq, free, strprint));

  pcq_delete(Ident);

}

void test_maths(void) {

  pcq_parser_t *Expr, *Factor, *Term, *Maths;
  int r0 = 1, r1 = 5, r2 = 13, r3 = 0, r4 = 2;

  Expr   = pcq_new("expr");
  Factor = pcq_new("factor");
  Term   = pcq_new("term");
  Maths  = pcq_new("maths");

  pcq_define(Expr, pcq_or(2,
    pcq_and(3, pcqf_maths, Factor, pcq_oneof("*/"), Factor, free, free),
    Factor
  ));

  pcq_define(Factor, pcq_or(2,
    pcq_and(3, pcqf_maths, Term, pcq_oneof("+-"), Term, free, free),
    Term
  ));

  pcq_define(Term, pcq_or(2,
    pcq_int(),
    pcq_parens(Expr, free)
  ));

  pcq_define(Maths, pcq_whole(Expr, free));

  PT_ASSERT(pcq_test_pass(Maths, "1", &r0, int_eq, free, int_print));
  PT_ASSERT(pcq_test_pass(Maths, "(5)", &r1, int_eq, free, int_print));
  PT_ASSERT(pcq_test_pass(Maths, "(4*2)+5", &r2, int_eq, free, int_print));
  PT_ASSERT(pcq_test_fail(Maths, "a", &r3, int_eq, free, int_print));
  PT_ASSERT(pcq_test_fail(Maths, "2b+4", &r4, int_eq, free, int_print));

  pcq_cleanup(4, Expr, Factor, Term, Maths);
}

void test_strip(void) {

  pcq_parser_t *Stripperl = pcq_apply(pcq_many(pcqf_strfold, pcq_any()), pcqf_strtriml);
  pcq_parser_t *Stripperr = pcq_apply(pcq_many(pcqf_strfold, pcq_any()), pcqf_strtrimr);
  pcq_parser_t *Stripper  = pcq_apply(pcq_many(pcqf_strfold, pcq_any()), pcqf_strtrim);

  PT_ASSERT(pcq_test_pass(Stripperl, " asdmlm dasd  ", "asdmlm dasd  ", streq, free, strprint));
  PT_ASSERT(pcq_test_pass(Stripperr, " asdmlm dasd  ", " asdmlm dasd", streq, free, strprint));
  PT_ASSERT(pcq_test_pass(Stripper,  " asdmlm dasd  ", "asdmlm dasd", streq, free, strprint));

  pcq_delete(Stripperl);
  pcq_delete(Stripperr);
  pcq_delete(Stripper);

}

void test_repeat(void) {

  int success;
  pcq_result_t r;
  pcq_parser_t *p = pcq_count(3, pcqf_strfold, pcq_digit(), free);

  success = pcq_parse("test", "046", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);

  success = pcq_parse("test", "046aa", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);

  success = pcq_parse("test", "04632", p, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "046");
  free(r.output);

  success = pcq_parse("test", "04", p, &r);
  PT_ASSERT(!success);
  pcq_err_delete(r.error);

  pcq_delete(p);

}

void test_copy(void) {

  int success;
  pcq_result_t r;
  pcq_parser_t* p = pcq_or(2, pcq_char('a'), pcq_char('b'));
  pcq_parser_t* q = pcq_and(2, pcqf_strfold, p, pcq_copy(p), free);

  success = pcq_parse("test", "aa", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "aa");
  free(r.output);

  success = pcq_parse("test", "bb", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "bb");
  free(r.output);

  success = pcq_parse("test", "ab", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "ab");
  free(r.output);

  success = pcq_parse("test", "ba", q, &r);
  PT_ASSERT(success);
  PT_ASSERT_STR_EQ(r.output, "ba");
  free(r.output);

  success = pcq_parse("test", "c", p, &r);
  PT_ASSERT(!success);
  pcq_err_delete(r.error);

  pcq_delete(pcq_copy(p));
  pcq_delete(pcq_copy(q));

  pcq_delete(q);

}

static int line_count = 0;

static pcq_val_t* read_line(pcq_val_t* line) {
  line_count++;
  return line;
}

void test_reader(void) {

  pcq_parser_t* Line = pcq_many(
    pcqf_strfold,
    pcq_apply(pcq_re("[^\\n]*(\\n|$)"), read_line));

  line_count = 0;

  PT_ASSERT(pcq_test_pass(Line,
    "hello\nworld\n\nthis\nis\ndan",
    "hello\nworld\n\nthis\nis\ndan", streq, free, strprint));

  PT_ASSERT(line_count == 6);

  line_count = 0;

  PT_ASSERT(pcq_test_pass(Line,
    "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg",
    "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg", streq, free, strprint));

  PT_ASSERT(line_count == 7);

  pcq_delete(Line);

}

static int token_count = 0;

static pcq_val_t *print_token(pcq_val_t *x) {
  /*printf("Token: '%s'\n", (char*)x);*/
  token_count++;
  return x;
}

void test_tokens(void) {

  pcq_parser_t* Tokens = pcq_many(
    pcqf_strfold,
    pcq_apply(pcq_strip(pcq_re("\\s*([a-zA-Z_]+|[0-9]+|,|\\.|:)")), print_token));

  token_count = 0;

  PT_ASSERT(pcq_test_pass(Tokens,
    "  hello 4352 ,  \n foo.bar   \n\n  test:ing   ",
    "hello4352,foo.bartest:ing", streq, free, strprint));

  PT_ASSERT(token_count == 9);

  pcq_delete(Tokens);

}

void test_eoi(void) {

  pcq_parser_t* Line = pcq_re("[^\\n]*$");

  PT_ASSERT(pcq_test_pass(Line, "blah", "blah", streq, free, strprint));
  PT_ASSERT(pcq_test_pass(Line, "blah\n", "blah\n", streq, free, strprint));

  pcq_delete(Line);

}

void suite_core(void) {
  pt_add_test(test_ident,  "Test Ident",  "Suite Core");
  pt_add_test(test_maths,  "Test Maths",  "Suite Core");
  pt_add_test(test_strip,  "Test Strip",  "Suite Core");
  pt_add_test(test_repeat, "Test Repeat", "Suite Core");
  pt_add_test(test_copy,   "Test Copy",   "Suite Core");
  pt_add_test(test_reader, "Test Reader", "Suite Core");
  pt_add_test(test_tokens, "Test Tokens", "Suite Core");
  pt_add_test(test_eoi,    "Test EOI",    "Suite Core");
}
