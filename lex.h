// lexer
struct mpc_lexer_t;
typedef struct mpc_lexer_t mpc_lexer_t;

// macro for defining a function pointer used by the lexer
#define MPC_LEX_ACTION(name) int(* name)(mpc_lexer_t *, mpc_result_t *)

// macro for defining a function for use by the lexer
#define mpc_lexer_action(x) int x(mpc_lexer_t * self, mpc_result_t * x)

struct mpc_lexer_t {
	char * name;
	mpc_parser_t * parser;
	MPC_LEX_ACTION(action);
	int count;
	struct mpc_lexer_t * self;
	struct mpc_lexer_pool_t * mpc_lexer_pool;
	mpc_lexer_t ** mode;
};

// lexer pool

struct mpc_lexer_pool_t {
	struct mpc_lexer_t *** lexer;
	int count;
} * mpc_lexer_pool;

void mpc_lexer_pool_undefined(void);
void mpc_lexer_pool_add(mpc_lexer_t *** l);
void mpc_lexer_pool_remove(mpc_lexer_t ** l);
void mpc_lexer_pool_print(void);
void mpc_lexer_pool_free(void);

mpc_lexer_t **mpc_lexer_undefined(void);
mpc_lexer_t **mpc_lexer_new(const char *name);
void mpc_lexer_add(mpc_lexer_t *** l, mpc_parser_t * p, MPC_LEX_ACTION(action), mpc_lexer_t *** m);
void mpc_lexer_print(mpc_lexer_t ** l);
mpc_lexer_t * mpc_lexer_find(char * lexer, char * name);
void mpc_lexer_free__(mpc_lexer_t ** l);
#define mpc_lexer_free(x) mpc_lexer_free__(x); x = NULL
int mpc_lexer(char * input, mpc_lexer_t ** list, mpc_result_t * result);

// lexers
mpc_lexer_t ** mpcl_line(MPC_LEX_ACTION(EOL_ACTION), MPC_LEX_ACTION(LINE_ACTION));
mpc_lexer_t ** mpcl_shline(MPC_LEX_ACTION(EOL_ACTION), MPC_LEX_ACTION(LINE_ACTION));

// xs
struct mpc_xs_t;
typedef struct mpc_xs_t mpc_xs_t;
mpc_xs_t *mpc_xs_undefined(void);
void mpc_xs_add(mpc_xs_t **x, mpc_result_t * result);
void mpc_xs_free(mpc_xs_t * x);
void mpc_xs_print(mpc_xs_t * x);
