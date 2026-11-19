#include "ptest.h"
#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static int int_eq(const void* x, const void* y) { return (*(int*)x == *(int*)y); }
static void int_print(const void* x) { printf("'%i'", *((int*)x)); }
static int streq(const void* x, const void* y) { return (strcmp(x, y) == 0); }
static void strprint(const void* x) { printf("'%s'", (char*)x); }
static mpc_val_t *fold_vals(int n, mpc_val_t **xs) {
  char** vals = malloc(sizeof(char*) * n);
  memcpy(vals, xs, sizeof(char*) * n);
  return vals;
}

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

static mpc_val_t *mpcf_maths(int n, mpc_val_t **xs) {
  int **vs = (int**)xs;
  (void) n;

  switch(((char*)xs[1])[0])
  {
    case '*': { *vs[0] *= *vs[2]; }; break;
    case '/': { *vs[0] /= *vs[2]; }; break;
    case '%': { *vs[0] %= *vs[2]; }; break;
    case '+': { *vs[0] += *vs[2]; }; break;
    case '-': { *vs[0] -= *vs[2]; }; break;
    default: break;
  }

  free(xs[1]); free(xs[2]);

  return xs[0];
}

void test_maths(void) {

  mpc_parser_t *Expr, *Factor, *Term, *Maths;
  int r0 = 1, r1 = 5, r2 = 13, r3 = 0, r4 = 2;

  Expr   = mpc_new("expr");
  Factor = mpc_new("factor");
  Term   = mpc_new("term");
  Maths  = mpc_new("maths");

  mpc_define(Expr, mpc_or(2,
    mpc_and(3, mpcf_maths, Factor, mpc_oneof("+-"), Factor, free, free),
    Factor
  ));

  mpc_define(Factor, mpc_or(2,
    mpc_and(3, mpcf_maths, Term, mpc_oneof("*/"), Term, free, free),
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
  PT_ASSERT(mpc_test_pass(Maths, "4*2+5", &r2, int_eq, free, int_print));
  PT_ASSERT(mpc_test_fail(Maths, "a", &r3, int_eq, free, int_print));
  PT_ASSERT(mpc_test_fail(Maths, "2b+4", &r4, int_eq, free, int_print));

  mpc_cleanup(4, Expr, Factor, Term, Maths);
}

void test_strip(void) {

  static const struct {
    const char *in;
    const char *l;
    const char *r;
    const char *lr;
  } strip_cases[] = {
    { " asdmlm dasd  ", "asdmlm dasd  ", " asdmlm dasd", "asdmlm dasd" },
    { "",               "",              "",             ""            },
    { " ",              "",              "",             ""            },
    { "   ",            "",              "",             ""            },
    { "\t \n",          "",              "",             ""            },
    { "abc",            "abc",           "abc",          "abc"         },
    { "abc ",           "abc ",          "abc",          "abc"         },
    { " a ",            "a ",            " a",           "a"           }
  };
  mpc_parser_t *Stripperl = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtriml);
  mpc_parser_t *Stripperr = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtrimr);
  mpc_parser_t *Stripper  = mpc_apply(mpc_many(mpcf_strfold, mpc_any()), mpcf_strtrim);
  char buf1[8];
  char buf2[10];
  size_t k;

  for (k = 0; k < sizeof(strip_cases) / sizeof(strip_cases[0]); k++) {
    PT_ASSERT(mpc_test_pass(Stripperl, strip_cases[k].in, strip_cases[k].l,  streq, free, strprint));
    PT_ASSERT(mpc_test_pass(Stripperr, strip_cases[k].in, strip_cases[k].r,  streq, free, strprint));
    PT_ASSERT(mpc_test_pass(Stripper,  strip_cases[k].in, strip_cases[k].lr, streq, free, strprint));
  }

  /* Verify mpcf_strtrimr does not under-read s[-1] into a preceding buffer byte */
  strcpy(buf1, "HELLO ");
  PT_ASSERT(mpcf_strtrimr(buf1 + 6) == buf1 + 6);
  PT_ASSERT_STR_EQ(buf1, "HELLO ");
  PT_ASSERT(buf1[5] == ' ' && buf1[6] == '\0');

  strcpy(buf2, "HELLO   ");
  PT_ASSERT(mpcf_strtrimr(buf2 + 6) == buf2 + 6);
  PT_ASSERT_STR_EQ(buf2, "HELLO ");
  PT_ASSERT(buf2[5] == ' ' && buf2[6] == '\0');

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

static int line_count = 0;

static mpc_val_t* read_line(mpc_val_t* line) {
  line_count++;
  return line;
}

void test_reader(void) {

  mpc_parser_t* Line = mpc_many(
    mpcf_strfold,
    mpc_apply(mpc_re("[^\\n]*(\\n|$)"), read_line));

  line_count = 0;

  PT_ASSERT(mpc_test_pass(Line,
    "hello\nworld\n\nthis\nis\ndan",
    "hello\nworld\n\nthis\nis\ndan", streq, free, strprint));

  PT_ASSERT(line_count == 6);

  line_count = 0;

  PT_ASSERT(mpc_test_pass(Line,
    "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg",
    "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg", streq, free, strprint));

  PT_ASSERT(line_count == 7);

  mpc_delete(Line);

}

static int token_count = 0;

static mpc_val_t *print_token(mpc_val_t *x) {
  /*printf("Token: '%s'\n", (char*)x);*/
  token_count++;
  return x;
}

void test_tokens(void) {

  mpc_parser_t* Tokens = mpc_many(
    mpcf_strfold,
    mpc_apply(mpc_strip(mpc_re("\\s*([a-zA-Z_]+|[0-9]+|,|\\.|:)")), print_token));

  token_count = 0;

  PT_ASSERT(mpc_test_pass(Tokens,
    "  hello 4352 ,  \n foo.bar   \n\n  test:ing   ",
    "hello4352,foo.bartest:ing", streq, free, strprint));

  PT_ASSERT(token_count == 9);

  mpc_delete(Tokens);

}

void test_eoi(void) {

  mpc_parser_t* Line = mpc_re("[^\\n]*$");

  PT_ASSERT(mpc_test_pass(Line, "blah", "blah", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(Line, "blah\n", "blah\n", streq, free, strprint));

  mpc_delete(Line);

}

void test_sepby(void) {
  mpc_parser_t* CommaSepIdent = mpc_sepby1(mpcf_strfold, mpc_char(','), mpc_ident());

  PT_ASSERT(mpc_test_fail(CommaSepIdent, "", "", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(CommaSepIdent, "one", "one", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(CommaSepIdent, "one,", "one", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(CommaSepIdent, "one,two,", "onetwo", streq, free, strprint));
  PT_ASSERT(mpc_test_pass(CommaSepIdent, "one,two,three", "onetwothree", streq, free, strprint));

  mpc_delete(CommaSepIdent);

  mpc_parser_t* CommaSepIdent1 = mpc_sepby1(fold_vals, mpc_char(','), mpc_ident());
  mpc_result_t r;

  mpc_parse("<test>", "uno,dos,tres,cuatro", CommaSepIdent1, &r);
  char **vals = r.output;
  PT_ASSERT(strcmp("uno",    vals[0]) == 0);
  PT_ASSERT(strcmp("dos",    vals[1]) == 0);
  PT_ASSERT(strcmp("tres",   vals[2]) == 0);
  PT_ASSERT(strcmp("cuatro", vals[3]) == 0);

  free(vals);
  mpc_delete(CommaSepIdent1);
}

static char *make_nested_parens(int n) {
  int i;
  char *s = malloc((size_t)(2 * n + 2));
  for (i = 0; i < n; i++) { s[i] = '('; }
  s[n] = 'a';
  for (i = 0; i < n; i++) { s[n + 1 + i] = ')'; }
  s[2 * n + 1] = '\0';
  return s;
}

static void assert_recursion_depth_case(mpc_parser_t *p, int depth, int expect_pass, int is_ast) {
  char *input = make_nested_parens(depth);
  mpc_result_t r;
  int ok = mpc_parse("test", input, p, &r);
  if (expect_pass) {
    PT_ASSERT(ok);
    if (ok) {
      if (is_ast) {
        mpc_ast_delete(r.output);
      } else {
        PT_ASSERT_STR_EQ(r.output, input);
        free(r.output);
      }
    } else {
      mpc_err_delete(r.error);
    }
  } else {
    PT_ASSERT(!ok);
    if (!ok) {
      PT_ASSERT(r.error != NULL);
      PT_ASSERT(r.error->failure != NULL);
      if (r.error && r.error->failure) {
        PT_ASSERT_STR_EQ(r.error->failure, "Maximum recursion depth exceeded!");
      }
      mpc_err_delete(r.error);
    } else if (is_ast) {
      mpc_ast_delete(r.output);
    } else {
      free(r.output);
    }
  }
  free(input);
}

void test_recursion_depth(void) {

  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *LangExpr = mpc_new("expr");
  mpc_err_t *err;

  /* Direct combinator grammar (2 frames/level + 2 leaf frames: mpc_expectf -> single, so n <= 498 passes and n >= 499 exceeds 1000) */
  mpc_define(Expr, mpc_or(2,
    mpc_and(3, mpcf_strfold, mpc_char('('), Expr, mpc_char(')'), free, free),
    mpc_char('a')
  ));

  assert_recursion_depth_case(Expr, 0,    1, 0);
  assert_recursion_depth_case(Expr, 100,  1, 0);
  assert_recursion_depth_case(Expr, 498,  1, 0);
  assert_recursion_depth_case(Expr, 499,  0, 0);
  assert_recursion_depth_case(Expr, 500,  0, 0);
  assert_recursion_depth_case(Expr, 2000, 0, 0);
  mpc_cleanup(1, Expr);

  /* mpca_lang grammar (~5 parser frames per nesting level) */
  err = mpca_lang(MPCA_LANG_DEFAULT, "expr : '(' <expr> ')' | 'a' ;", LangExpr);
  PT_ASSERT(err == NULL);
  if (err != NULL) { mpc_err_delete(err); }

  assert_recursion_depth_case(LangExpr, 0,    1, 1);
  assert_recursion_depth_case(LangExpr, 100,  1, 1);
  assert_recursion_depth_case(LangExpr, 200,  0, 1);
  assert_recursion_depth_case(LangExpr, 2000, 0, 1);
  mpc_cleanup(1, LangExpr);

}

void suite_core(void) {
  pt_add_test(test_ident,           "Test Ident",           "Suite Core");
  pt_add_test(test_maths,           "Test Maths",           "Suite Core");
  pt_add_test(test_strip,           "Test Strip",           "Suite Core");
  pt_add_test(test_repeat,          "Test Repeat",          "Suite Core");
  pt_add_test(test_copy,            "Test Copy",            "Suite Core");
  pt_add_test(test_reader,          "Test Reader",          "Suite Core");
  pt_add_test(test_tokens,          "Test Tokens",          "Suite Core");
  pt_add_test(test_eoi,             "Test EOI",             "Suite Core");
  pt_add_test(test_sepby,           "Test SepBy",           "Suite Core");
  pt_add_test(test_recursion_depth, "Test Recursion Depth", "Suite Core");
}
