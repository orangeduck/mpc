#ifndef MPC_INTERNAL_H
# define MPC_INTERNAL_H

# ifdef __cplusplus
extern "C" {
# endif

# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <ctype.h>
# include <fcntl.h>
# include <unistd.h>
# include "../mpc.h"

# ifndef va_copy
#  ifndef __va_copy
#   define va_copy(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
#  else
#   define va_copy __va_copy
#  endif
# endif

/*
** Input Type
*/

enum {
  MPC_INPUT_STRING = 0,
  MPC_INPUT_FILE   = 1,
  MPC_INPUT_PIPE   = 2
};

enum {
  MPC_INPUT_MARKS_MIN = 32
};

enum {
  MPC_INPUT_MEM_NUM = 512
};

typedef struct {
  char mem[64];
} mpc_mem_t;

typedef struct {
  int type;
  char *filename;
  mpc_state_t state;

  char *string;
  char *buffer;
  FILE *file;

  int suppress;
  int backtrack;
  int marks_slots;
  int marks_num;
  mpc_state_t *marks;

  char *lasts;
  char last;

  size_t mem_index;
  char mem_full[MPC_INPUT_MEM_NUM];
  mpc_mem_t mem[MPC_INPUT_MEM_NUM];
} mpc_input_t;

/*
** Parser Type
*/

enum {
  MPC_TYPE_UNDEFINED  = 0,
  MPC_TYPE_PASS       = 1,
  MPC_TYPE_FAIL       = 2,
  MPC_TYPE_LIFT       = 3,
  MPC_TYPE_LIFT_VAL   = 4,
  MPC_TYPE_EXPECT     = 5,
  MPC_TYPE_ANCHOR     = 6,
  MPC_TYPE_STATE      = 7,

  MPC_TYPE_ANY        = 8,
  MPC_TYPE_SINGLE     = 9,
  MPC_TYPE_ONEOF      = 10,
  MPC_TYPE_NONEOF     = 11,
  MPC_TYPE_RANGE      = 12,
  MPC_TYPE_SATISFY    = 13,
  MPC_TYPE_STRING     = 14,

  MPC_TYPE_APPLY      = 15,
  MPC_TYPE_APPLY_TO   = 16,
  MPC_TYPE_PREDICT    = 17,
  MPC_TYPE_NOT        = 18,
  MPC_TYPE_MAYBE      = 19,
  MPC_TYPE_MANY       = 20,
  MPC_TYPE_MANY1      = 21,
  MPC_TYPE_COUNT      = 22,

  MPC_TYPE_OR         = 23,
  MPC_TYPE_AND        = 24,

  MPC_TYPE_CHECK      = 25,
  MPC_TYPE_CHECK_WITH = 26,

  MPC_TYPE_SOI        = 27,
  MPC_TYPE_EOI        = 28,

  MPC_TYPE_SEPBY1     = 29
};

typedef struct { char *m; } mpc_pdata_fail_t;
typedef struct { mpc_ctor_t lf; void *x; } mpc_pdata_lift_t;
typedef struct { mpc_parser_t *x; char *m; } mpc_pdata_expect_t;
typedef struct { int(*f)(char,char); } mpc_pdata_anchor_t;
typedef struct { char x; } mpc_pdata_single_t;
typedef struct { char x; char y; } mpc_pdata_range_t;
typedef struct { int(*f)(char); } mpc_pdata_satisfy_t;
typedef struct { char *x; } mpc_pdata_string_t;
typedef struct { mpc_parser_t *x; mpc_apply_t f; } mpc_pdata_apply_t;
typedef struct { mpc_parser_t *x; mpc_apply_to_t f; void *d; } mpc_pdata_apply_to_t;
typedef struct { mpc_parser_t *x; mpc_dtor_t dx; mpc_check_t f; char *e; } mpc_pdata_check_t;
typedef struct { mpc_parser_t *x; mpc_dtor_t dx; mpc_check_with_t f; void *d; char *e; } mpc_pdata_check_with_t;
typedef struct { mpc_parser_t *x; } mpc_pdata_predict_t;
typedef struct { mpc_parser_t *x; mpc_dtor_t dx; mpc_ctor_t lf; } mpc_pdata_not_t;
typedef struct { int n; mpc_fold_t f; mpc_parser_t *x; mpc_dtor_t dx; } mpc_pdata_repeat_t;
typedef struct { int n; mpc_parser_t **xs; } mpc_pdata_or_t;
typedef struct { int n; mpc_fold_t f; mpc_parser_t **xs; mpc_dtor_t *dxs;  } mpc_pdata_and_t;
typedef struct { int n; mpc_fold_t f; mpc_parser_t *x; mpc_parser_t *sep; } mpc_pdata_sepby1;

typedef union {
  mpc_pdata_fail_t fail;
  mpc_pdata_lift_t lift;
  mpc_pdata_expect_t expect;
  mpc_pdata_anchor_t anchor;
  mpc_pdata_single_t single;
  mpc_pdata_range_t range;
  mpc_pdata_satisfy_t satisfy;
  mpc_pdata_string_t string;
  mpc_pdata_apply_t apply;
  mpc_pdata_apply_to_t apply_to;
  mpc_pdata_check_t check;
  mpc_pdata_check_with_t check_with;
  mpc_pdata_predict_t predict;
  mpc_pdata_not_t not;
  mpc_pdata_repeat_t repeat;
  mpc_pdata_and_t and;
  mpc_pdata_or_t or;
  mpc_pdata_sepby1 sepby1;
} mpc_pdata_t;

struct mpc_parser_t {
  char *name;
  mpc_pdata_t data;
  char type;
  char retained;
};

enum {
  MPC_PARSE_STACK_MIN = 4
};

#define MPC_SUCCESS(x) r->output = x; return 1
#define MPC_FAILURE(x) r->error = x; return 0
#define MPC_PRIMITIVE(x) \
  if (x) { MPC_SUCCESS(r->output); } \
  else { MPC_FAILURE(NULL); }

#define MPC_MAX_RECURSION_DEPTH 1000

/*
** Grammar Parser
*/

typedef struct {
  int create_new;
  int parsers_num;
  mpc_parser_t **parsers;
  int flags;
} mpca_grammar_st_t;

typedef struct {
  char *ident;
  char *name;
  mpc_parser_t *grammar;
} mpca_stmt_t;

# ifdef __cplusplus
}
# endif

#endif