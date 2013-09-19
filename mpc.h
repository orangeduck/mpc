/*
** mpc - Micro Parser Combinator library for C
** https://github.com/orangeduck/mpc
** Daniel Holden - contact@daniel-holden.com
** Licensed under BSD3
*/
#ifndef mpc_h
#define mpc_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/*
** Error Type
*/

struct mpc_err_t;
typedef struct mpc_err_t mpc_err_t;

int mpc_err_line(mpc_err_t* x);
int mpc_err_column(mpc_err_t* x);
char mpc_err_unexpected(mpc_err_t* x);
char** mpc_err_expected(mpc_err_t* x, int* num);

void mpc_err_delete(mpc_err_t* x);
void mpc_err_print(mpc_err_t* x);
void mpc_err_print_to(mpc_err_t* x, FILE* f);
void mpc_err_msg(mpc_err_t* x, char* out, int* outn, int outmax);

/*
** Parsing
*/

typedef void mpc_val_t;

typedef union {
  mpc_err_t* error;
  mpc_val_t* output;
} mpc_result_t;

struct mpc_parser_t;
typedef struct mpc_parser_t mpc_parser_t;

bool mpc_parse(const char* s, mpc_parser_t* p, mpc_result_t* r);
bool mpc_parse_file(FILE* f, mpc_parser_t* p, mpc_result_t* r);
bool mpc_parse_filename(const char* filename, mpc_parser_t* p, mpc_result_t* r);

/*
** Building a Parser
*/

void mpc_delete(mpc_parser_t* p);
mpc_parser_t* mpc_new(void);
mpc_parser_t* mpc_assign(mpc_parser_t* p, mpc_parser_t* a);
mpc_parser_t* mpc_define(mpc_parser_t* p, mpc_parser_t* a);
mpc_parser_t* mpc_retain(mpc_parser_t* p);
mpc_parser_t* mpc_expect(mpc_parser_t* a, const char* expected);

mpc_parser_t* mpc_pass(void);
mpc_parser_t* mpc_fail(void);
mpc_parser_t* mpc_lift(mpc_val_t* x);

/*
** Basic Parsers
*/

mpc_parser_t* mpc_any(void);
mpc_parser_t* mpc_char(char c);
mpc_parser_t* mpc_range(char s, char e);
mpc_parser_t* mpc_oneof(const char* s);
mpc_parser_t* mpc_noneof(const char* s);
mpc_parser_t* mpc_satisfy(bool(*f)(char));
mpc_parser_t* mpc_string(const char* s);

/*
** Function Types
*/

typedef void (*mpc_dtor_t)(mpc_val_t*);
typedef mpc_val_t*(*mpc_apply_t)(mpc_val_t*);
typedef mpc_val_t*(*mpc_fold_t)(mpc_val_t*,mpc_val_t*);
typedef mpc_val_t*(*mpc_afold_t)(int,mpc_val_t**);

void mpc_dtor_null(mpc_val_t* x);

/*
** Core Parsers
*/

mpc_parser_t* mpc_apply(mpc_parser_t* a, mpc_apply_t f);
mpc_parser_t* mpc_maybe(mpc_parser_t* a);
mpc_parser_t* mpc_many(mpc_parser_t* a, mpc_fold_t f);
mpc_parser_t* mpc_many1(mpc_parser_t* a, mpc_fold_t f);
mpc_parser_t* mpc_count(mpc_parser_t* a, mpc_dtor_t da, mpc_fold_t f, int n);
mpc_parser_t* mpc_either(mpc_parser_t* a, mpc_parser_t* b);
mpc_parser_t* mpc_also(mpc_parser_t* a, mpc_parser_t* b, mpc_dtor_t da, mpc_fold_t f);
mpc_parser_t* mpc_bind(mpc_parser_t* a, mpc_parser_t* b, mpc_dtor_t da, mpc_fold_t f);
mpc_parser_t* mpc_or(int n, ...);
mpc_parser_t* mpc_and(int n, mpc_afold_t f, ...);

/*
** Common Parsers
*/

mpc_parser_t* mpc_space(void);
mpc_parser_t* mpc_spaces(void);
mpc_parser_t* mpc_whitespace(void);

mpc_parser_t* mpc_newline(void);
mpc_parser_t* mpc_tab(void);
mpc_parser_t* mpc_eoi(void);
mpc_parser_t* mpc_escape(void);

mpc_parser_t* mpc_digit(void);
mpc_parser_t* mpc_hexdigit(void);
mpc_parser_t* mpc_octdigit(void);
mpc_parser_t* mpc_digits(void);
mpc_parser_t* mpc_hexdigits(void);
mpc_parser_t* mpc_octdigits(void);

mpc_parser_t* mpc_lower(void);
mpc_parser_t* mpc_upper(void);
mpc_parser_t* mpc_alpha(void);
mpc_parser_t* mpc_underscore(void);

mpc_parser_t* mpc_int(void);
mpc_parser_t* mpc_hex(void);
mpc_parser_t* mpc_oct(void);
mpc_parser_t* mpc_number(void);

mpc_parser_t* mpc_float(void);

mpc_parser_t* mpc_semi(void);
mpc_parser_t* mpc_comma(void);
mpc_parser_t* mpc_colon(void);
mpc_parser_t* mpc_dot(void);

mpc_parser_t* mpc_char_lit(void);
mpc_parser_t* mpc_string_lit(void);

mpc_parser_t* mpc_ident(void);

/*
** Useful Parsers
*/

mpc_parser_t* mpc_ends(mpc_parser_t* a, mpc_dtor_t da);
mpc_parser_t* mpc_skip_many(mpc_parser_t* a, mpc_fold_t f);
mpc_parser_t* mpc_skip_many1(mpc_parser_t* a, mpc_fold_t f);
mpc_parser_t* mpc_tok(mpc_parser_t* a); 
mpc_parser_t* mpc_sym(const char* s);
mpc_parser_t* mpc_between(mpc_parser_t* a, mpc_dtor_t ad, const char* o, const char* c);
mpc_parser_t* mpc_parens(mpc_parser_t* a, mpc_dtor_t ad);
mpc_parser_t* mpc_braces(mpc_parser_t* a, mpc_dtor_t ad);
mpc_parser_t* mpc_brackets(mpc_parser_t* a, mpc_dtor_t ad);
mpc_parser_t* mpc_squares(mpc_parser_t* a, mpc_dtor_t ad);


/*
** Regular Expression Parsers
*/

mpc_parser_t* mpc_re(const char* re);

/*
** Common Fold Functions
*/

mpc_val_t* mpcf_free(mpc_val_t* x);
mpc_val_t* mpcf_int(mpc_val_t* x);
mpc_val_t* mpcf_hex(mpc_val_t* x);
mpc_val_t* mpcf_oct(mpc_val_t* x);
mpc_val_t* mpcf_escape(mpc_val_t* x);

mpc_val_t* mpcf_fst(mpc_val_t* x, mpc_val_t* y);
mpc_val_t* mpcf_snd(mpc_val_t* x, mpc_val_t* y);

mpc_val_t* mpcf_fst_free(mpc_val_t* x, mpc_val_t* y);
mpc_val_t* mpcf_snd_free(mpc_val_t* x, mpc_val_t* y);

mpc_val_t* mpcf_freefold(mpc_val_t* t, mpc_val_t* x);
mpc_val_t* mpcf_strfold(mpc_val_t* t, mpc_val_t* x);

mpc_val_t* mpcf_between_free(int n, mpc_val_t** xs);
mpc_val_t* mpcf_maths(int n, mpc_val_t** xs);


/*
** Printing
*/

void mpc_print(mpc_parser_t* p);

/*
** Testing
*/

bool mpc_test(mpc_parser_t* p, const char* input, void* data,
  bool(*tester)(void*, void*), 
  void(*destructor)(void*), 
  void(*printer)(void*));

#endif