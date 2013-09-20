#include "mpc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*
** State Type
*/

typedef struct {
  char last;
  char next;
  int pos;
  int row;
  int col;
} mpc_state_t;

/*
** Error Type
*/

struct mpc_err_t {
  mpc_state_t state;
  int expected_num;
  char** expected;
};

static mpc_err_t* mpc_err_new(mpc_state_t s, char* expected) {
  mpc_err_t* x = malloc(sizeof(mpc_err_t));
  x->state = s;
  x->expected_num = 1;
  x->expected = malloc(sizeof(char*));
  x->expected[0] = malloc(strlen(expected) + 1);
  strcpy(x->expected[0], expected);
  return x;
}

void mpc_err_delete(mpc_err_t* x) {

  int i;
  for (i = 0; i < x->expected_num; i++) {
    free(x->expected[i]);
  }

  free(x->expected);
  free(x);
}

static bool mpc_err_contains_expected(mpc_err_t* x, char* expected) {
  
  int i;
  for (i = 0; i < x->expected_num; i++) {
    if (strcmp(x->expected[i], expected) == 0) { return true; }
  }
  
  return false;
  
}

static void mpc_err_add_expected(mpc_err_t* x, char* expected) {
  
  x->expected_num++;
  x->expected = realloc(x->expected, sizeof(char*) * x->expected_num);
  x->expected[x->expected_num-1] = malloc(strlen(expected) + 1);
  strcpy(x->expected[x->expected_num-1], expected);
  
}

void mpc_err_print(mpc_err_t* x) {
  mpc_err_print_to(x, stdout);
}

void mpc_err_print_to(mpc_err_t* x, FILE* f) {
  
  fprintf(f, "<input>:%i:%i: error: expected ", x->state.row, x->state.col);
  
  if (x->expected_num == 0) {
    
    fprintf(f, "ERROR: NOTHING EXPECTED");
    
  } else if (x->expected_num == 1) {
    
    fprintf(f, "%s", x->expected[0]);
    
  } else {
  
    int i;
    for (i = 0; i < x->expected_num-2; i++) {
      fprintf(f, "%s, ", x->expected[i]);
    } 
    
    fprintf(f, "%s or %s", 
      x->expected[x->expected_num-2], 
      x->expected[x->expected_num-1]);
    
  }
  
  printf(" at '%c'\n", x->state.next);
  
}

void mpc_err_msg(mpc_err_t* x, char* out, int* outn, int outmax) {
  /* TODO: Implement */
}

static mpc_err_t* mpc_err_either(mpc_err_t* x, mpc_err_t* y) {
  
  if (x->state.pos > y->state.pos) {
    mpc_err_delete(y);
    return x;
  }
  
  if (x->state.pos < y->state.pos) {
    mpc_err_delete(x);
    return y;
  }
  
  if (x->state.pos == y->state.pos) {  
    
    int i;
    for (i = 0; i < y->expected_num; i++) {
      if (mpc_err_contains_expected(x, y->expected[i])) { continue; }
      else { mpc_err_add_expected(x, y->expected[i]); }      
    }
    
    mpc_err_delete(y);
    return x;
  }
  
  return NULL;
  
}

static mpc_err_t* mpc_err_or(mpc_err_t** x, int n) {
  mpc_err_t* e = x[0];
  
  int i;
  for (i = 1; i < n; i++) {
    e = mpc_err_either(e, x[i]);
  }
  
  return e;
}

static mpc_err_t* mpc_err_many1(mpc_err_t* x) {
  /* TODO: Collapse expected list and add `one or more` prefix */
  return x;
}

static mpc_err_t* mpc_err_count(mpc_err_t* x, int n) {
  /* TODO: Collapse expected list and add `N of` prefix */
  return x;
}

char** mpc_err_expected(mpc_err_t* x, int* num) {
  *num = x->expected_num;
  return x->expected;
}

/*
** Input Type
*/

typedef struct {

  char* str;
  mpc_state_t state;
  
  int marks_num;
  mpc_state_t* marks;
  
} mpc_input_t;

static mpc_input_t* mpc_input_new(const char* str) {

  mpc_input_t* i = malloc(sizeof(mpc_input_t));
  i->str = malloc(strlen(str) + 1);
  strcpy(i->str, str);
  
  i->state.next = i->str[0];
  i->state.last = '\0';
  i->state.pos = 0;
  i->state.row = 0;
  i->state.col = 0;
  
  i->marks_num = 0;
  i->marks = NULL;
  
  return i;
}

static void mpc_input_delete(mpc_input_t* i) {
  free(i->str);
  free(i->marks);
  free(i);
}

static void mpc_input_mark(mpc_input_t* i) {
  i->marks_num++;
  i->marks = realloc(i->marks, sizeof(mpc_state_t) * i->marks_num);
  i->marks[i->marks_num-1] = i->state;
}

static void mpc_input_unmark(mpc_input_t* i) {
  i->marks_num--;
  i->marks = realloc(i->marks, sizeof(mpc_state_t) * i->marks_num);
}

static void mpc_input_rewind(mpc_input_t* i) {
  i->state = i->marks[i->marks_num-1];
  mpc_input_unmark(i);
}

static bool mpc_input_next(mpc_input_t* i, char** o) {

  i->state.last = i->str[i->state.pos];
  i->state.pos++;
  i->state.col++;
  
  if (i->state.last == '\n') {
    i->state.col = 0;
    i->state.row++;
  }
  
  (*o) = malloc(2);
  (*o)[0] = i->state.last;
  (*o)[1] = '\0';
  return true;

}

static bool mpc_input_any(mpc_input_t* i, char** o) {

  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (i->str[i->state.pos] == '\0') {
    i->state.next = i->str[i->state.pos];
    return false;
  }

  return mpc_input_next(i, o);
  
}

static bool mpc_input_char(mpc_input_t* i, char c, char** o) {
  
  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (i->str[i->state.pos] != c) {
    i->state.next = i->str[i->state.pos];
    return false; 
  }
  
  return mpc_input_next(i, o);

}

static bool mpc_input_range(mpc_input_t* i, char c, char d, char** o) {

  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (i->str[i->state.pos] < c ||
      i->str[i->state.pos] > d) {
    i->state.next = i->str[i->state.pos];
    return false;
  }
  
  return mpc_input_next(i, o);

}

static bool char_in_string(char c, const char* x) {
  
  while (*x) {
    if (*x == c) { return true; }
    x++;
  }
  
  return false;
}

static bool mpc_input_oneof(mpc_input_t* i, const char* c, char** o) {
  
  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (!char_in_string(i->str[i->state.pos], c)) {
    i->state.next = i->str[i->state.pos];
    return false;
  }
  
  return mpc_input_next(i, o);

}

static bool mpc_input_noneof(mpc_input_t* i, const char* c, char** o) {
  
  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (char_in_string(i->str[i->state.pos], c)	|| (i->str[i->state.pos] == '\0')) {
    i->state.next = i->str[i->state.pos]; 
    return false;
  }
  
  return mpc_input_next(i, o);

}

static bool mpc_input_satisfy(mpc_input_t* i, bool(*cond)(char), char** o) {
  
  if (i->state.pos > strlen(i->str)) { i->state.next = '\0'; return false; }
  if (!cond(i->str[i->state.pos])) { i->state.next = i->str[i->state.pos]; return false; }
  
  return mpc_input_next(i, o);
  
}

bool mpc_input_string(mpc_input_t* i, const char* c, char** o) {
  
  mpc_input_mark(i);
  char* co = NULL;
  const char* x = c;
  while (*x) {
    if (mpc_input_char(i, *x, &co)) {
      free(co);
    } else {
      mpc_input_rewind(i);
      return false;
    }
    x++;
  }
  mpc_input_unmark(i);
  
  *o = malloc(strlen(c) + 1);
  strcpy(*o, c);
  return true;
}

/*
** Parser Type
*/

enum {
  MPC_TYPE_UNDEFINED = 0,
  MPC_TYPE_PASS      = 1,
  MPC_TYPE_FAIL      = 2,
  MPC_TYPE_LIFT      = 3,
  MPC_TYPE_EXPECT    = 4,
  
  MPC_TYPE_ANY       = 5,
  MPC_TYPE_SINGLE    = 6,
  MPC_TYPE_ONEOF     = 7,
  MPC_TYPE_NONEOF    = 8,
  MPC_TYPE_RANGE     = 9,
  MPC_TYPE_SATISFY   = 10,
  MPC_TYPE_STRING    = 11,
  
  MPC_TYPE_APPLY     = 12,
  
  MPC_TYPE_MAYBE     = 13,
  MPC_TYPE_MANY      = 14,
  MPC_TYPE_MANY1     = 15,
  MPC_TYPE_COUNT     = 16,
  
  MPC_TYPE_EITHER    = 17,
  MPC_TYPE_ALSO      = 18,
  MPC_TYPE_OR        = 19,
  MPC_TYPE_AND       = 20,
};

typedef struct { void* x; } mpc_pdata_lift_t;
typedef struct { mpc_parser_t* x; char* m; } mpc_pdata_expect_t;
typedef struct { char x; } mpc_pdata_single_t;
typedef struct { char x; char y; } mpc_pdata_range_t;
typedef struct { bool(*f)(char); } mpc_pdata_satisfy_t;
typedef struct { char* x; } mpc_pdata_string_t;
typedef struct { mpc_parser_t* x; mpc_apply_t f; } mpc_pdata_apply_t;
typedef struct { mpc_parser_t* x; mpc_fold_t f; int n; mpc_dtor_t dx; } mpc_pdata_repeat_t;
typedef struct { mpc_parser_t* x; mpc_parser_t* y; } mpc_pdata_either_t;
typedef struct { mpc_parser_t* x; mpc_parser_t* y; mpc_dtor_t dx; mpc_fold_t f; } mpc_pdata_also_t;
typedef struct { int n; mpc_parser_t** xs; } mpc_pdata_or_t;
typedef struct { int n; mpc_parser_t** xs; mpc_dtor_t* dxs; mpc_afold_t f; } mpc_pdata_and_t;

typedef union {
  mpc_pdata_lift_t lift;
  mpc_pdata_expect_t expect;
  mpc_pdata_single_t single;
  mpc_pdata_range_t range;
  mpc_pdata_satisfy_t satisfy;
  mpc_pdata_string_t string;
  mpc_pdata_apply_t apply;
  mpc_pdata_repeat_t repeat;
  mpc_pdata_either_t either;
  mpc_pdata_also_t also;
  mpc_pdata_and_t and;
  mpc_pdata_or_t or;
} mpc_pdata_t;

struct mpc_parser_t {
  bool retained;
  uint8_t type;
  mpc_pdata_t data;
};

/*
** This is rather pleasant. The core parsing routine
** is written in about 500 lines of C.
**
** I also love the way in which each parsing type
** concisely matches some construct or idiom in C.
** Particularly nice is are the `either` and `also`
** types which have a broken but mirrored structure
** with return value and error reflected.
**
*/

#define MPC_SUCCESS(x) r->output = x; return true;
#define MPC_FAILURE(x) r->error = x; return false;
#define MPC_TRY(x, f) if (f) { MPC_SUCCESS(x) } else { MPC_FAILURE(mpc_err_new(i->state, "different character")); }

bool mpc_parse_input(mpc_input_t* i, mpc_parser_t* p, mpc_result_t* r) {
  
  memset(r, 0, sizeof(mpc_result_t)); 
  
  if (p->type == MPC_TYPE_UNDEFINED) { fprintf(stderr, "Error: Parser Undefined!\n"); abort(); }
  
  /* Trivial Parsers */
  
  if (p->type == MPC_TYPE_PASS) { MPC_SUCCESS(NULL); }
  if (p->type == MPC_TYPE_FAIL) { MPC_FAILURE(mpc_err_new(i->state, "different character")); }
  if (p->type == MPC_TYPE_LIFT) { MPC_SUCCESS(p->data.lift.x); }
  
  /* Basic Parsers */
  
  char* s = NULL;  
  if (p->type == MPC_TYPE_ANY)     { MPC_TRY(s, mpc_input_any(i, &s)); }
  if (p->type == MPC_TYPE_SINGLE)  { MPC_TRY(s, mpc_input_char(i, p->data.single.x, &s)); }
  if (p->type == MPC_TYPE_RANGE)   { MPC_TRY(s, mpc_input_range(i, p->data.range.x, p->data.range.y, &s)); }
  if (p->type == MPC_TYPE_ONEOF)   { MPC_TRY(s, mpc_input_oneof(i, p->data.string.x, &s)); }
  if (p->type == MPC_TYPE_NONEOF)  { MPC_TRY(s, mpc_input_noneof(i, p->data.string.x, &s)); }
  if (p->type == MPC_TYPE_SATISFY) { MPC_TRY(s, mpc_input_satisfy(i, p->data.satisfy.f, &s)); }
  if (p->type == MPC_TYPE_STRING)  { MPC_TRY(s, mpc_input_string(i, p->data.string.x, &s)); }
  
  /* Advanced Parsers */
  
  int c = 0; 
  mpc_val_t* t = NULL;
  mpc_result_t x, y;
  memset(&x, 0, sizeof(mpc_result_t));
  memset(&y, 0, sizeof(mpc_result_t));

  if (p->type == MPC_TYPE_EXPECT) {
    if (mpc_parse_input(i, p->data.expect.x, &x)) {
      MPC_SUCCESS(x.output);
    } else {
      mpc_err_delete(x.error);
      MPC_FAILURE(mpc_err_new(i->state, p->data.expect.m));
    }
  } 
  
  if (p->type == MPC_TYPE_APPLY) {
    if (mpc_parse_input(i, p->data.apply.x, &x)) {
      MPC_SUCCESS(p->data.apply.f(x.output));
    } else {
      MPC_FAILURE(x.error);
    }
  }
  
  if (p->type == MPC_TYPE_MAYBE) {
    if (mpc_parse_input(i, p->data.repeat.x, &x)) { MPC_SUCCESS(x.output); }
    mpc_err_delete(x.error);
    MPC_SUCCESS(NULL);
  }
  
  if (p->type == MPC_TYPE_MANY) {
    while (mpc_parse_input(i, p->data.repeat.x, &x)) { t = p->data.repeat.f(t, x.output); }
    mpc_err_delete(x.error);
    MPC_SUCCESS(t);
  }
  
  if (p->type == MPC_TYPE_MANY1) {
    
    while (mpc_parse_input(i, p->data.repeat.x, &x)) { t = p->data.repeat.f(t, x.output); c++; }
    
    if (c >= 1) {
      mpc_err_delete(x.error);
      MPC_SUCCESS(t);
    } else {
      MPC_FAILURE(mpc_err_many1(x.error));
    }
    
  }
  
  if (p->type == MPC_TYPE_COUNT) {
    
    mpc_input_mark(i);
    while (mpc_parse_input(i, p->data.repeat.x, &x)) {
      t = p->data.repeat.f(t, x.output);
      c++;
      if (c == p->data.repeat.n) { break; }
    }
    
    if (c == p->data.repeat.n) {
      mpc_input_unmark(i);
      MPC_SUCCESS(t);
    } else {
      p->data.repeat.dx(t);
      mpc_input_rewind(i);
      MPC_FAILURE(mpc_err_count(x.error, p->data.repeat.n));
    }
    
  }
  
  /* Combinatory Parsers */
  
  if (p->type == MPC_TYPE_EITHER) {
    if (mpc_parse_input(i, p->data.either.x, &x)) { MPC_SUCCESS(x.output); }
    if (mpc_parse_input(i, p->data.either.y, &y)) { mpc_err_delete(x.error); MPC_SUCCESS(y.output); }
    MPC_FAILURE(mpc_err_either(x.error, y.error));
  }
  
  if (p->type == MPC_TYPE_ALSO) {
    mpc_input_mark(i);
    if (!mpc_parse_input(i, p->data.also.x, &x)) { mpc_input_rewind(i); MPC_FAILURE(x.error); }
    if (!mpc_parse_input(i, p->data.also.y, &y)) { mpc_input_rewind(i); p->data.also.dx(x.output); MPC_FAILURE(y.error); }
    mpc_input_unmark(i);
    MPC_SUCCESS(p->data.also.f(x.output, y.output));
  }
  
  if (p->type == MPC_TYPE_OR) {
    
    mpc_result_t* rs = malloc(sizeof(mpc_result_t) * p->data.or.n);
    int ri, pri;
    
    for (ri = 0; ri < p->data.or.n; ri++) {
      if (mpc_parse_input(i, p->data.or.xs[ri], &rs[ri])) {
        
        for (pri = 0; pri < ri; pri++) { mpc_err_delete(rs[pri].error); }
        r->output = rs[ri].output;
        free(rs);
        return true;
        
      }
    }
    
    mpc_err_t** vals = malloc(sizeof(mpc_err_t*) * p->data.or.n);
    
    for (ri = 0; ri < p->data.and.n; ri++) {
      vals[ri] = rs[ri].error;
    }
    
    r->error = mpc_err_or(vals, p->data.or.n);
    
    free(vals);
    free(rs);
    return false;
    
  }
  
  if (p->type == MPC_TYPE_AND) {
    
    mpc_input_mark(i);

    mpc_result_t* rs = malloc(sizeof(mpc_result_t) * p->data.and.n);
    int ri, pri;
    
    for (ri = 0; ri < p->data.and.n; ri++) {
      if (!mpc_parse_input(i, p->data.and.xs[ri], &rs[ri])) {
        
        for (pri = 0; pri < ri; pri++) { p->data.and.dxs[pri](rs[pri].output); }
        r->error = rs[ri].error;
        free(rs);
        mpc_input_rewind(i);
        return false;
        
      }
    }
    
    mpc_val_t** vals = malloc(sizeof(mpc_val_t*) * p->data.and.n);
    
    for (ri = 0; ri < p->data.and.n; ri++) {
      vals[ri] = rs[ri].output;
    }
    
    r->output = p->data.and.f(p->data.and.n, vals);
    
    free(rs);
    free(vals);
    mpc_input_unmark(i);
    return true;
  
  }
  
  fprintf(stderr, "Unknown Parser Type Id %i!\n", p->type);
  abort();
  
}

#undef MPC_SUCCESS
#undef MPC_FAILURE
#undef MPC_TRY

bool mpc_parse(const char* s, mpc_parser_t* p, mpc_result_t* r) {
  mpc_input_t* i = mpc_input_new(s);
  bool x = mpc_parse_input(i, p, r);
  mpc_input_delete(i);
  return x;
}

bool mpc_parse_file(FILE* f, mpc_parser_t* p, mpc_result_t* r) {
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* buff = malloc(len + 1);
  fread(buff, 1, len, f);
  buff[len] = '\0';
  
  bool x = mpc_parse(buff, p, r);
  
  free(buff);
  return x;
}

bool mpc_parse_filename(const char* filename, mpc_parser_t* p, mpc_result_t* r) {
  FILE* f = fopen(filename, "r");
  bool res = mpc_parse_file(f, p, r);
  fclose(f);
  return res;
}

/*
** Building a Parser
*/

static void mpc_undefine_unretained(mpc_parser_t* p, bool force);

static void mpc_undefine_or(mpc_parser_t* p) {
  
  int i;
  for (i = 0; i < p->data.or.n; i++) {
    mpc_undefine_unretained(p->data.or.xs[i], false);
  }
  free(p->data.or.xs);
  
}

static void mpc_undefine_and(mpc_parser_t* p) {
  
  int i;
  for (i = 0; i < p->data.and.n; i++) {
    mpc_undefine_unretained(p->data.and.xs[i], false);
  }
  free(p->data.and.xs);
  free(p->data.and.dxs);
  
}

static void mpc_undefine_unretained(mpc_parser_t* p, bool force) {
  
  if (p->retained && !force) { return; }
  
  switch (p->type) {
    
    case MPC_TYPE_ONEOF: 
    case MPC_TYPE_NONEOF:
    case MPC_TYPE_STRING:
      free(p->data.string.x); 
      break;
    
    case MPC_TYPE_APPLY:
      mpc_undefine_unretained(p->data.apply.x, false);
      break;
    
    case MPC_TYPE_EXPECT:
      mpc_undefine_unretained(p->data.expect.x, false);
      free(p->data.expect.m);
      break;

    case MPC_TYPE_MAYBE:
    case MPC_TYPE_MANY:
    case MPC_TYPE_MANY1:
    case MPC_TYPE_COUNT:
      mpc_undefine_unretained(p->data.repeat.x, false);
      break;
    
    case MPC_TYPE_EITHER: 
      mpc_undefine_unretained(p->data.either.x, false); 
      mpc_undefine_unretained(p->data.either.y, false); 
      break;
    
    case MPC_TYPE_ALSO:
      mpc_undefine_unretained(p->data.also.x, false); 
      mpc_undefine_unretained(p->data.also.y, false); 
      break;
    
    case MPC_TYPE_OR:
      mpc_undefine_or(p);
      break;
    
    case MPC_TYPE_AND:
      mpc_undefine_and(p);
      break;
    
    default: break;
  }
  
  if (!force) { free(p); }
  
}

void mpc_delete(mpc_parser_t* p) {

  if (p->retained) {

    if (p->type != MPC_TYPE_UNDEFINED) {
      fprintf(stderr, "Error: Parser still Defined! Use `mpc_undefine` before delete!\n");
      abort();
    } else {
      free(p);
    }
  
  } else {
    
    mpc_undefine_unretained(p, false); 
    
  }


}

static mpc_parser_t* mpc_undefined(void) {
  mpc_parser_t* p = calloc(1, sizeof(mpc_parser_t));
  p->retained = false;
  p->type = MPC_TYPE_UNDEFINED;
  return p;
}

mpc_parser_t* mpc_new(void) {
  mpc_parser_t* p = mpc_undefined();
  p->retained = true;
  return p;
}

mpc_parser_t* mpc_undefine(mpc_parser_t* p) {
  mpc_undefine_unretained(p, true);
  p->type = MPC_TYPE_UNDEFINED;
  return p;
}

mpc_parser_t* mpc_define(mpc_parser_t* p, mpc_parser_t* a) {
  p->type = a->type;
  p->data = a->data;
  free(a);
  return p;  
}

mpc_parser_t* mpc_pass(void) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_PASS;
  return p;
}

mpc_parser_t* mpc_fail(void) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_FAIL;
  return p;
}

mpc_parser_t* mpc_lift(mpc_val_t* x) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_LIFT;
  p->data.lift.x = x;
  return p;
}


mpc_parser_t* mpc_expect(mpc_parser_t* a, const char* expected) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_EXPECT;
  p->data.expect.x = a;
  p->data.expect.m = malloc(strlen(expected) + 1);
  strcpy(p->data.expect.m, expected);
  return p;
}

/*
** Basic Parsers
*/

mpc_parser_t* mpc_any(void) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_ANY;
  return mpc_expect(p, "any character");
}

mpc_parser_t* mpc_char(char c) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_SINGLE;
  p->data.single.x = c;
  return mpc_expect(p, (char[]){ '\'', c, '\'', '\0' } );
}

mpc_parser_t* mpc_range(char s, char e) {
  
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_RANGE;
  p->data.range.x = s;
  p->data.range.y = e;
  
  char expected[30];
  strcpy(expected, "character between '");
  strcat(expected, (char[]){ s, '\0' });
  strcat(expected, "' and '");
  strcat(expected, (char[]){ e, '\0' });
  strcat(expected, "'");
  
  return mpc_expect(p, expected);
}

mpc_parser_t* mpc_oneof(const char* s) {
  
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_ONEOF;
  p->data.string.x = malloc(strlen(s) + 1);
  strcpy(p->data.string.x, s);
  
  char* expected = malloc(strlen(s) + 10);
  strcpy(expected, "one of '");
  strcat(expected, s);
  strcat(expected, "'");
  
  p = mpc_expect(p, expected);
  
  free(expected);
  
  return p;
}

mpc_parser_t* mpc_noneof(const char* s) {

  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_NONEOF;
  p->data.string.x = malloc(strlen(s) + 1);
  strcpy(p->data.string.x, s);
  
  char* expected = malloc(strlen(s) + 11);
  strcpy(expected, "none of '");
  strcat(expected, s);
  strcat(expected, "'");
  
  p = mpc_expect(p, expected);
  
  free(expected);
  
  return p;

}

mpc_parser_t* mpc_satisfy(bool(*f)(char)) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_SATISFY;
  p->data.satisfy.f = f;
  
  return p;
}

mpc_parser_t* mpc_string(const char* s) {

  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_STRING;
  p->data.string.x = malloc(strlen(s) + 1);
  strcpy(p->data.string.x, s);
  
  char* expected = malloc(strlen(s) + 3);
  strcpy(expected, "\"");
  strcat(expected, s);
  strcat(expected, "\"");
  
  p = mpc_expect(p, expected);
  
  free(expected);
  
  return p;

}

void mpc_dtor_null(mpc_val_t* x) { return; }

/*
** Core Parsers
*/

mpc_parser_t* mpc_apply(mpc_parser_t* a, mpc_apply_t f) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_APPLY;
  p->data.apply.x = a;
  p->data.apply.f = f;
  return p;
}

mpc_parser_t* mpc_maybe(mpc_parser_t* a) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_MAYBE;
  p->data.repeat.x = a;
  return p;
}

mpc_parser_t* mpc_many(mpc_parser_t* a, mpc_fold_t f) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_MANY;
  p->data.repeat.x = a;
  p->data.repeat.f = f;
  return p;
}

mpc_parser_t* mpc_many1(mpc_parser_t* a, mpc_fold_t f) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_MANY1;
  p->data.repeat.x = a;
  p->data.repeat.f = f;
  return p;
}

mpc_parser_t* mpc_count(mpc_parser_t* a, mpc_dtor_t da, mpc_fold_t f, int n) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_COUNT;
  p->data.repeat.x = a;
  p->data.repeat.dx = da;
  p->data.repeat.f = f;
  p->data.repeat.n = n;
  return p;
}

mpc_parser_t* mpc_either(mpc_parser_t* a, mpc_parser_t* b) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_EITHER;
  p->data.either.x = a;
  p->data.either.y = b;
  return p;
}

mpc_parser_t* mpc_also(mpc_parser_t* a, mpc_parser_t* b, mpc_dtor_t da, mpc_fold_t f) {
  mpc_parser_t* p = mpc_undefined();
  p->type = MPC_TYPE_ALSO;
  p->data.also.x = a;
  p->data.also.y = b;
  p->data.also.dx = da;
  p->data.also.f = f;
  return p;
}

mpc_parser_t* mpc_bind(mpc_parser_t* a, mpc_parser_t* b, mpc_dtor_t da, mpc_fold_t f) {
  return mpc_also(a, b, da, f);
}

mpc_parser_t* mpc_or(int n, ...) {

  mpc_parser_t* p = mpc_undefined();
  
  p->type = MPC_TYPE_OR;
  p->data.or.n = n;
  p->data.or.xs = malloc(sizeof(mpc_parser_t*) * n);
  
  va_list va;
  va_start(va, n);
  int i;
  for (i = 0; i < n; i++) {
    p->data.or.xs[i] = va_arg(va, mpc_parser_t*);
  }
  va_end(va);
  
  return p;
}

mpc_parser_t* mpc_and(int n, mpc_afold_t f, ...) {

  mpc_parser_t* p = mpc_undefined();
  
  p->type = MPC_TYPE_AND;
  p->data.and.n = n;
  p->data.and.f = f;
  p->data.and.xs = malloc(sizeof(mpc_parser_t*) * n);
  p->data.and.dxs = malloc(sizeof(mpc_dtor_t) * (n-1));
  
  va_list va;
  va_start(va, f);
  int i;
  for (i = 0; i < n; i++) {
    p->data.and.xs[i] = va_arg(va, mpc_parser_t*);
  }
  for (i = 0; i < (n-1); i++) {
    p->data.and.dxs[i] = va_arg(va, mpc_dtor_t);
  }
  va_end(va);
  
  return p;

}

/*
** Common Parsers
*/

mpc_parser_t* mpc_space(void) { return mpc_expect(mpc_oneof(" \f\n\r\t\v"), "space"); }
mpc_parser_t* mpc_spaces(void) { return mpc_expect(mpc_many(mpc_space(), mpcf_strfold), "spaces"); }
mpc_parser_t* mpc_whitespace(void) { return mpc_expect(mpc_apply(mpc_spaces(), mpcf_free), "whitespace"); }

mpc_parser_t* mpc_newline(void) { return mpc_expect(mpc_char('\n'), "newline"); }
mpc_parser_t* mpc_tab(void) { return mpc_expect(mpc_char('\t'), "tab"); }
mpc_parser_t* mpc_eoi(void) { return mpc_expect(mpc_char('\0'), "end of input"); }
mpc_parser_t* mpc_escape(void) { return mpc_also(mpc_char('\\'), mpc_any(), free, mpcf_strfold); }

mpc_parser_t* mpc_digit(void) { return mpc_expect(mpc_oneof("012345689"), "digit"); }
mpc_parser_t* mpc_hexdigit(void) { return mpc_expect(mpc_oneof("0123456789ABCDEFabcdef"), "hex digit"); }
mpc_parser_t* mpc_octdigit(void) { return mpc_expect(mpc_oneof("01234567"), "oct digit"); }
mpc_parser_t* mpc_digits(void) { return mpc_expect(mpc_many1(mpc_digit(), mpcf_strfold), "digits"); }
mpc_parser_t* mpc_hexdigits(void) { return mpc_expect(mpc_many1(mpc_hexdigit(), mpcf_strfold), "hex digits"); }
mpc_parser_t* mpc_octdigits(void) { return mpc_expect(mpc_many1(mpc_octdigit(), mpcf_strfold), "oct digits"); }

mpc_parser_t* mpc_lower(void) { return mpc_expect(mpc_oneof("abcdefghijklmnopqrstuvwxyz"), "lowercase letter"); }
mpc_parser_t* mpc_upper(void) { return mpc_expect(mpc_oneof("ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "uppercase letter"); }
mpc_parser_t* mpc_alpha(void) { return mpc_expect(mpc_either(mpc_lower(), mpc_upper()), "letter"); }
mpc_parser_t* mpc_underscore(void) { return mpc_expect(mpc_char('_'), "underscore"); }

mpc_parser_t* mpc_int(void) { return mpc_expect(mpc_apply(mpc_digits(), mpcf_int), "integer"); }
mpc_parser_t* mpc_hex(void) { return mpc_expect(mpc_apply(mpc_hexdigits(), mpcf_hex), "hexadecimal"); }
mpc_parser_t* mpc_oct(void) { return mpc_expect(mpc_apply(mpc_octdigits(), mpcf_oct), "octadecimal"); }
mpc_parser_t* mpc_number(void) { return mpc_expect(mpc_or(3, mpc_int(), mpc_hex(), mpc_oct()), "number"); }

mpc_parser_t* mpc_float(void);

mpc_parser_t* mpc_semi(void)  { return mpc_char(';'); }
mpc_parser_t* mpc_comma(void) { return mpc_char(','); }
mpc_parser_t* mpc_colon(void) { return mpc_char(':'); }
mpc_parser_t* mpc_dot(void)   { return mpc_char('.'); }

mpc_parser_t* mpc_char_lit(void);
mpc_parser_t* mpc_string_lit(void);

mpc_parser_t* mpc_ident(void);

/*
** Useful Parsers
*/

mpc_parser_t* mpc_ends(mpc_parser_t* a, mpc_dtor_t da) { return mpc_also(a, mpc_eoi(), da, mpcf_fst_free); }
mpc_parser_t* mpc_skip_many(mpc_parser_t* a, mpc_fold_t f) { return mpc_many(a, f); }
mpc_parser_t* mpc_skip_many1(mpc_parser_t* a, mpc_fold_t f) { return mpc_many1(a, f); }
mpc_parser_t* mpc_tok(mpc_parser_t* a) { return mpc_also(a, mpc_whitespace(), mpc_dtor_null, mpcf_fst); }
mpc_parser_t* mpc_sym(const char* s) { return mpc_tok(mpc_string(s)); }

mpc_parser_t* mpc_between(mpc_parser_t* a, mpc_dtor_t ad, const char* o, const char* c) {
  return mpc_and(3, mpcf_between_free,
    mpc_string(o), a, mpc_string(c),
    free, ad);
}

mpc_parser_t* mpc_parens(mpc_parser_t* a, mpc_dtor_t ad)   { return mpc_between(a, ad, "(", ")"); }
mpc_parser_t* mpc_braces(mpc_parser_t* a, mpc_dtor_t ad)   { return mpc_between(a, ad, "<", ">"); }
mpc_parser_t* mpc_brackets(mpc_parser_t* a, mpc_dtor_t ad) { return mpc_between(a, ad, "{", "}"); }
mpc_parser_t* mpc_squares(mpc_parser_t* a, mpc_dtor_t ad)  { return mpc_between(a, ad, "[", "]"); }

/*
** Regular Expression Parsers
*/

/*
** So here is a cute bootstrapping.
**
** I'm using the previously defined
** mpc constructs and functions to
** parse the user regex string and
** construct a parser from it.
**
** As it turns out lots of the standard
** mpc functions look a lot like `fold`
** functions and so can be used indirectly
** by many of the parsing functions to build
** a parser directly - as we parse.
**
** This is certainly something that
** would be less elegant/interesting 
** in a two-phase parser which first
** built an AST and then traversed it
** to generate the object.
**
** This whole thing acts as a great
** case study for how trivial it can be
** to write a great parser in a few
** lines of code using this library.
*/

/*
**
**  ### Regular Expression Grammar
**
**      <regex> : (<term> "|" <regex>) | <term>
**     
**      <term> : <factor>*
**
**      <factor> : <base>
**               | <base> "*"
**               | <base> "+"
**               | <base> "?"
**               | <base> "{" <digits> "}"
**           
**      <base> : <char>
**             | "\" <char>
**             | "."
**             | "(" <regex> ")"
**             | "[" <range> "]"
*/

static mpc_val_t* mpc_re_fold_or(int n, mpc_val_t** xs) {
  free(xs[1]);
  return mpc_either(xs[0], xs[2]);
}

static mpc_val_t* mpc_re_fold_repeat(mpc_val_t* x, mpc_val_t* y) {
  if (strcmp(y, "*") == 0) { free(y); return mpc_many(x, mpcf_strfold); }
  if (strcmp(y, "+") == 0) { free(y); return mpc_many1(x, mpcf_strfold); }
  if (strcmp(y, "?") == 0) { free(y); return mpc_maybe(x); }
  int n = strtol(y, NULL, 10);
  free(y);
  return mpc_count(x, free, mpcf_strfold, n);
}

static mpc_val_t* mpc_re_fold_many(mpc_val_t* t, mpc_val_t* x) {
  if (t == NULL) { return x; }
  return mpc_also(t, x, free, mpcf_strfold);
}

static mpc_val_t* mpc_re_escape(mpc_val_t* x) {
  
  char* s = x;
  
  if (s[0] == '.') { free(x); return mpc_any(); }
  if (s[0] == '$') { free(x); return mpc_eoi(); }
  
  if (s[0] == '\\') {
    mpc_parser_t* p = mpc_char(s[1]);
    free(s);
    return p;
  } else {
    mpc_parser_t* p = mpc_char(s[0]);
    free(s);
    return p;
  }
  
}

static mpc_val_t* mpc_re_range(mpc_val_t* x) {
  /* TODO: Implement proper range scanning */
  return mpc_oneof(x);
}

mpc_parser_t* mpc_re(const char* re) {
  
  mpc_parser_t* Regex  = mpc_new();
  mpc_parser_t* Term   = mpc_new();
  mpc_parser_t* Factor = mpc_new();
  mpc_parser_t* Base   = mpc_new();
  mpc_parser_t* Range  = mpc_new();
  
  mpc_define(Regex, mpc_either(
    mpc_and(3, mpc_re_fold_or, Term, mpc_char('|'), Regex, mpc_delete, free),
    Term
  ));
  
  mpc_define(Term, mpc_many(Factor, mpc_re_fold_many));
  
  mpc_define(Factor, mpc_or(5,
    mpc_also(Base, mpc_char('*'), (mpc_dtor_t)mpc_delete, mpc_re_fold_repeat),
    mpc_also(Base, mpc_char('+'), (mpc_dtor_t)mpc_delete, mpc_re_fold_repeat),
    mpc_also(Base, mpc_char('?'), (mpc_dtor_t)mpc_delete, mpc_re_fold_repeat),
    mpc_also(Base, mpc_braces(mpc_digits(), free), (mpc_dtor_t)mpc_delete, mpc_re_fold_repeat),
    Base
  ));
  
  mpc_define(Base, mpc_or(5,
    mpc_parens(Regex, (mpc_dtor_t)mpc_delete),
    mpc_squares(Range, (mpc_dtor_t)mpc_delete),
    mpc_apply(mpc_oneof(".$"), mpc_re_escape),
    mpc_apply(mpc_escape(), mpc_re_escape),
    mpc_apply(mpc_noneof(")|"), mpc_re_escape)
  ));
  
  mpc_define(Range, mpc_apply(
    mpc_many(mpc_either(mpc_escape(), mpc_noneof("]")), mpcf_strfold),
    mpc_re_range
  ));
  
  mpc_result_t r;
  bool res = mpc_parse(re, Regex, &r);
  
  mpc_undefine(Regex);
  mpc_undefine(Term);
  mpc_undefine(Factor);
  mpc_undefine(Base);
  mpc_undefine(Range);
  
  mpc_delete(Regex);
  mpc_delete(Term);
  mpc_delete(Factor);
  mpc_delete(Base);
  mpc_delete(Range);
  
  if (res) {
    return r.output;
  } else {
    mpc_err_delete(r.error);
    return NULL;
  }
  
}

/*
** Common Fold Functions
*/

mpc_val_t* mpcf_free(mpc_val_t* x) {
  free(x);
  return NULL;
}

mpc_val_t* mpcf_int(mpc_val_t* x) {
  int* y = malloc(sizeof(int));
  *y = strtol(x, NULL, 10);
  free(x);
  return y;
}

mpc_val_t* mpcf_hex(mpc_val_t* x) {
  int* y = malloc(sizeof(int));
  *y = strtol(x, NULL, 16);
  free(x);
  return y;
}

mpc_val_t* mpcf_oct(mpc_val_t* x) {
  int* y = malloc(sizeof(int));
  *y = strtol(x, NULL, 8);
  free(x);
  return y;
}

static mpc_val_t* mpcf_escape_new(mpc_val_t* x) {
  
  char* input  = (char[]){
    '\a', '\b', '\f', '\n', '\r',
    '\t', '\v', '\\', '\'', '\"', '\0'};
    
  char** output = (char*[]){
    "\\a", "\\b", "\\f", "\\n", "\\r", 
    "\\t", "\\v", "\\\\", "\\'", "\\\"", "\\0"};
  
  int i;
  char* s = x;
  char* y = calloc(1, 1);
  
  while (*s) {
    
    bool found = false;
    for (i = 0; i < 11; i++) { 
      if (*s == input[i]) {
        y = realloc(y, strlen(y) + strlen(output[i]) + 1);
        strcat(y, output[i]);
        found = true;
        break;
      }
    }
    
    if (!found) {
      y = realloc(y, strlen(y) + 2);
      strcat(y, (char[]){*s, '\0'});
    }
    
    s++;
  }
  
  
  return y;
}

mpc_val_t* mpcf_escape(mpc_val_t* x) {
  mpc_val_t* y = mpcf_escape_new(x);
  free(x);
  return y;
}

mpc_val_t* mpcf_fst(mpc_val_t* x, mpc_val_t* y) {
  return x;
}

mpc_val_t* mpcf_snd(mpc_val_t* x, mpc_val_t* y) {
  return y;
}

mpc_val_t* mpcf_fst_free(mpc_val_t* x, mpc_val_t* y) {
  free(y);
  return x;
}

mpc_val_t* mpcf_snd_free(mpc_val_t* x, mpc_val_t* y) {
  free(x);
  return y;
}

mpc_val_t* mpcf_freefold(mpc_val_t* t, mpc_val_t* x) {
  free(x);
  return NULL;
}

mpc_val_t* mpcf_strfold(mpc_val_t* t, mpc_val_t* x) {
  
  if (t == NULL) { return x; }
  
  t = realloc(t, strlen(t) + strlen(x) + 1);
  strcat(t, x);
  free(x);
  
  return t;
}

mpc_val_t* mpcf_between_free(int n, mpc_val_t** xs) {
  free(xs[0]);
  free(xs[2]);
  return xs[1];
}

mpc_val_t* mpcf_maths(int n, mpc_val_t** xs) {
  
  int** vs = (int**)xs;
    
  if (strcmp(xs[1], "*") == 0) { *vs[0] *= *vs[2]; }
  if (strcmp(xs[1], "/") == 0) { *vs[0] /= *vs[2]; }
  if (strcmp(xs[1], "%") == 0) { *vs[0] %= *vs[2]; }
  if (strcmp(xs[1], "+") == 0) { *vs[0] += *vs[2]; }
  if (strcmp(xs[1], "-") == 0) { *vs[0] -= *vs[2]; }
  
  free(xs[1]); free(xs[2]);
  
  return xs[0];
}

/*
** Printing
*/

static void mpc_print_unretained(mpc_parser_t* p, bool force) {
  
  if (p->retained && !force) { printf("<retained>"); return; }
  
  if (p->type == MPC_TYPE_UNDEFINED) { printf("<undefined>"); }
  if (p->type == MPC_TYPE_PASS)   { printf("<pass>"); }
  if (p->type == MPC_TYPE_FAIL)   { printf("<fail>"); }
  if (p->type == MPC_TYPE_LIFT)   { printf("<lift>"); }
  if (p->type == MPC_TYPE_EXPECT) { mpc_print_unretained(p->data.expect.x, false); }
  
  if (p->type == MPC_TYPE_ANY) { printf("<any>"); }
  if (p->type == MPC_TYPE_SATISFY) { printf("<satisfy %p>", p->data.satisfy.f); }

  if (p->type == MPC_TYPE_SINGLE) {
    char* s = mpcf_escape_new((char[]){ p->data.single.x, '\0' });
    printf("'%s'", s);
    free(s);
  }
  
  if (p->type == MPC_TYPE_RANGE) {
    char* s = mpcf_escape_new((char[]){ p->data.range.x, '\0' });
    char* e = mpcf_escape_new((char[]){ p->data.range.y, '\0' });
    printf("[%s-%s]", s, e);
    free(s);
    free(e);
  }
  
  if (p->type == MPC_TYPE_ONEOF) {
    char* s = mpcf_escape_new(p->data.string.x);
    printf("[%s]", s);
    free(s);
  }
  
  if (p->type == MPC_TYPE_NONEOF) {
    char* s = mpcf_escape_new(p->data.string.x);
    printf("[^%s]", s);
    free(s);
  }
  
  if (p->type == MPC_TYPE_STRING) {
    char* s = mpcf_escape_new(p->data.string.x);
    printf("\"%s\"", s);
    free(s);
  }
  
  if (p->type == MPC_TYPE_APPLY) { mpc_print_unretained(p->data.apply.x, false); }
  if (p->type == MPC_TYPE_MAYBE) { printf("("); mpc_print_unretained(p->data.repeat.x, false); printf(")?"); }
  if (p->type == MPC_TYPE_MANY)  { printf("("); mpc_print_unretained(p->data.repeat.x, false); printf(")*"); }
  if (p->type == MPC_TYPE_MANY1) { printf("("); mpc_print_unretained(p->data.repeat.x, false); printf(")+"); }
  if (p->type == MPC_TYPE_COUNT) { printf("("); mpc_print_unretained(p->data.repeat.x, false); printf("){%i}", p->data.repeat.n); }
  
  if (p->type == MPC_TYPE_EITHER) {
    printf("(");
    mpc_print_unretained(p->data.either.x, false);
    printf(" | ");
    mpc_print_unretained(p->data.either.y, false);
    printf(")");
  }
  
  if (p->type == MPC_TYPE_ALSO) {
    mpc_print_unretained(p->data.also.x, false);
    printf(" ");
    mpc_print_unretained(p->data.also.y, false);
   }
  
  if (p->type == MPC_TYPE_OR) {
    printf("(");
    int i;
    for(i = 0; i < p->data.or.n-1; i++) {
      mpc_print_unretained(p->data.or.xs[i], false);
      printf(" | ");
    }
    mpc_print_unretained(p->data.or.xs[p->data.or.n-1], false);
    printf(")");
  }
  
  if (p->type == MPC_TYPE_AND) {
    int i;
    for(i = 0; i < p->data.and.n-1; i++) {
      mpc_print_unretained(p->data.and.xs[i], false);
      printf(" ");
    }
    mpc_print_unretained(p->data.and.xs[p->data.and.n-1], false);
  }
  
}

void mpc_print(mpc_parser_t* p) {
  mpc_print_unretained(p, true);
  printf("\n");
}

/*
** Testing
*/

bool mpc_test(mpc_parser_t* p, const char* s, void* data,
  bool(*tester)(void*, void*), 
  void(*destructor)(void*), 
  void(*printer)(void*)) {
  
  printf("== Test ==: '%s'\n", s);
  
  mpc_result_t r;  
  if (mpc_parse(s, p, &r)) {
    
    if (tester(r.output, data)) {
      printf("Passed with "); printer(r.output); printf("\n");
      destructor(r.output);
      return true;
    } else {
      printf("Failed!\n");
      printf("Got "); printer(r.output); printf("\n");
      printf("Expected "); printer(data); printf("\n");
      destructor(r.output);
      return false;
    }
    
  } else {
    
    printf("Failed!\n");
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
    return false;
    
  }
  
}

