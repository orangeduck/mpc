// lexer

// global lexer pool

void mpc_lexer_pool_undefined(void) {
  mpc_lexer_pool = calloc(1, sizeof(struct mpc_lexer_pool_t));
  mpc_lexer_pool->lexer = calloc(1, sizeof(mpc_lexer_t));
  mpc_lexer_pool->count = 1;
}

void mpc_lexer_pool_add(mpc_lexer_t *** l) {
	
	if (!mpc_lexer_pool) {
		mpc_lexer_pool_undefined();
	}
	
	
	if (mpc_lexer_pool->count != 1 || *mpc_lexer_pool->lexer) {
// 		printf("adding %s to pool %d\n", (**l)->name, mpc_lexer_pool->count);
		for (int i = 0; i < mpc_lexer_pool->count; i++) if (strcmp(mpc_lexer_pool->lexer[i][0][0].name, (**l)->name) == 0) {
			printf("name conflict detected: '%s' already exists in pool %d\n", (**l)->name, i);
			abort();
		}
		mpc_lexer_t *** lexertmp = realloc(mpc_lexer_pool->lexer, sizeof(mpc_lexer_t)*(mpc_lexer_pool->count+1));
		if (lexertmp != NULL) mpc_lexer_pool->lexer = lexertmp;
		else abort();
		mpc_lexer_pool->count++;
	}
// 	else printf("adding %s to pool %d\n", (**l)->name, mpc_lexer_pool->count-1);
	mpc_lexer_pool->lexer[mpc_lexer_pool->count-1] = *l;
}

void mpc_lexer_pool_remove(mpc_lexer_t ** l) {
	if (!mpc_lexer_pool) {
		mpc_lexer_pool_undefined();
	}
	
	for (int i = 0; i < mpc_lexer_pool->count; i++) {
		if (strcmp(mpc_lexer_pool->lexer[i][0][0].name, (*l)->name) == 0) {
// 			printf("removing %s from pool %d\n", (*l)->name, i);
			int count = 0;
			for (int ii = i; ii < mpc_lexer_pool->count; ii++) {
				if (ii+2 <= mpc_lexer_pool->count) {
// 					printf("moving pool %d to pool %d\n", ii+1, ii);
					mpc_lexer_pool->lexer[ii] = mpc_lexer_pool->lexer[ii+1];
				} else {
// 					printf("nulling pool %d\n", ii);
					mpc_lexer_pool->lexer[ii] = NULL;
					count++;
				}
			}
// 			printf("pool count %d changed to %d, decreased by %d\n", mpc_lexer_pool->count, mpc_lexer_pool->count-count, count);
			mpc_lexer_pool->count-=count;
		}
	}
	if (!mpc_lexer_pool->count) mpc_lexer_pool = NULL;
}

void mpc_lexer_pool_print(void) {
	if (!mpc_lexer_pool) {
		puts("pool is not defined");
		return;
	}
	for (int i = 0; i < mpc_lexer_pool->count; i++) {
		for (int ii = 0; ii < mpc_lexer_pool->lexer[i][0]->count; ii++) {
			printf("mpc_lexer_pool->lexer[%d][0][%d].name: %s\n", i, ii, mpc_lexer_pool->lexer[i][0][ii].name);
			if (mpc_lexer_pool->lexer[i][0][ii].parser) printf("mpc_lexer_pool->lexer[%d][0][%d].parser->name: %s\n", i, ii, mpc_lexer_pool->lexer[i][0][ii].parser->name);
			else printf("mpc_lexer_pool->lexer[%d][0][%d].parser: no parser associated yet\n", i, ii);
		}
	}
}

void mpc_lexer_pool_free(void) {
	if (!mpc_lexer_pool) {
		return;
	}
	for (int i = 0; i < mpc_lexer_pool->count; i++) {
		mpc_lexer_pool->lexer[i] = NULL;
	}
	free(mpc_lexer_pool->lexer);
	mpc_lexer_pool->count = 0;
	free(mpc_lexer_pool);
	mpc_lexer_pool = NULL;
}

// lexer itself

mpc_lexer_t **mpc_lexer_undefined(void) {
  mpc_lexer_t **l = calloc(1, sizeof(mpc_lexer_t));
  *l = calloc(1, sizeof(mpc_lexer_t));
  (*l)->name = NULL;
  (*l)->parser = NULL;
  (*l)->action = NULL;
  (*l)->mode = NULL;
  (*l)->count = 1;
  (*l)->self = (*l);
  (*l)->mpc_lexer_pool = mpc_lexer_pool;
  return l;
}

mpc_lexer_t **mpc_lexer_new(const char *name) {
  mpc_lexer_t **l = mpc_lexer_undefined();
  (*l)->name = realloc((*l)->name, strlen(name) + 1);
  strcpy((*l)->name, name);
  mpc_lexer_pool_add(&l);
  return l;
}

void mpc_lexer_add(mpc_lexer_t *** l, mpc_parser_t * p, MPC_LEX_ACTION(action), mpc_lexer_t *** m) {
	if (!p->name) {
		return;
	}
	
	mpc_lexer_t ** lexer;
	
	if (!l) {
		lexer = mpc_lexer_undefined();
	} else lexer = *l;
	
	if ((*lexer)->count != 1 || (*lexer)->parser) {
		mpc_lexer_t * lexertmp = realloc((*lexer), sizeof(mpc_lexer_t)*((*lexer)->count+1));
		if (lexertmp != NULL) (*lexer) = lexertmp;
		else return;
		(*lexer)->count++;
		**l = (*lexer);
	}

	(*lexer)[(*lexer)->count-1].self = **l;
	(*lexer)[(*lexer)->count-1].mpc_lexer_pool = mpc_lexer_pool;
	(*lexer)[(*lexer)->count-1].name = (*lexer)->name;
	(*lexer)[(*lexer)->count-1].parser = p;
	if (m) (*lexer)[(*lexer)->count-1].mode = *m;
	else (*lexer)[(*lexer)->count-1].mode = NULL;
	
	if (action) (*lexer)[(*lexer)->count-1].action = action;
	else (*lexer)[(*lexer)->count-1].action = NULL;
}

void mpc_lexer_print(mpc_lexer_t ** l) {
	if (!(*l)) {
		puts("lexer is not defined");
		return;
	}
	for (int i = 0; i < (*l)->count; i++) {
		printf("%s: parser: %s\n%s: action: %p\n%s: mode: %p\n\n",
			   (*l)->name?(*l)->name:"Undefined", (*l)[i].parser?(*l)[i].parser->name?(*l)[i].parser->name:"Undefined":"Undefined",
			   
			   (*l)->name?(*l)->name:"Undefined", (*l)[i].action,
			   
			   (*l)->name?(*l)->name:"Undefined", (*l)[i].mode
		);
	}
}

mpc_lexer_t * mpc_lexer_find(char * lexer, char * name) {
	for (int i = 0; i < mpc_lexer_pool->count; i++) {
		for (int ii = 0; ii < mpc_lexer_pool->lexer[i][0]->count; ii++) {
			if (lexer != NULL) {
				if (strcmp(mpc_lexer_pool->lexer[i][0][ii].name, lexer) == 0) {
						if (strcmp(mpc_lexer_pool->lexer[i][0][ii].parser->name, name) == 0) return &mpc_lexer_pool->lexer[i][0][ii];
				}
			} else if (strcmp(mpc_lexer_pool->lexer[i][0][ii].parser->name, name) == 0) return &mpc_lexer_pool->lexer[i][0][ii];
		}
	}
	return NULL;
}

void mpc_lexer_free__(mpc_lexer_t ** l) {
	if (!(*l)) {
		return;
	}
	if ((*l)->name) {
		// attempt to remove the lexer from the pool
		mpc_lexer_pool_remove(l);
		free((*l)->name);
	}
	for (int i = 0; i < (*l)->count; i++) {
		(*l)[i].parser = NULL;
		(*l)[i].action = NULL;
		(*l)[i].self = NULL;
		(*l)[i].mpc_lexer_pool = NULL;
	}
	(*l)->count = 0;
	free(l);
	(*l) = NULL;
}

// what really happens when a lexer initiates a mode change?

/*

 first, since this is undocumented behaviour in which is difficult to find actual documentation for,
 lets start on the assumption that when the lexer changes modes the following changes happen:
	
	1. the entire lexer grammer changes
	2. the loop is restarted
	3. the input buffer is not reset
	
*/
int mpc_lexer(char * input, mpc_lexer_t ** list, mpc_result_t * result) {
	int parsed_succesfully = 0;
	while(strcmp(input,"")!=0) {
		// parser loop
		parsed_succesfully = 0; // reset parsed_succesfully
		for (int i = 0; i < (*list)->count; i++) {
			// parse the parsers
			if (mpc_parse("lexer", input, (*list)[i].parser, result)) {
				// output the result
				if (strcmp(result->output,"") != 0) {
					parsed_succesfully = 1;
					int len = strlen(result->output);
					input+=len;
					// &list[i] is the address of the structure at index i of the array list
					if ((*list)[i].action) (*list)[i].action(&(*list)[i], result);
					
					// if a mode change is associated, change the mode
					if ((*list)[i].mode) {
						puts("changing modes");
						list = (*list)[i].mode;
					}
					
					// if matches the lexer should reset to parse from the top of the list, if this is the case is the while loop needed? and could lexing bugs appear if two parsers happen to match the same input, for example, given "abab", and the parsers mpc_char('a'), mpc_any(), mpc_string("ab"), in that order, if we dont reset, it will parse as "a" > "b" > "ab", and if we do reset it will parse as "a" > "b" > "a" > "b"
					i = -1;
				}
			}
			// do not do error checking for every parser
		}
		if (parsed_succesfully == 0) return -1; // return if the all of the parsers could not match anything to avoid an infinite while loop if the input has not been emptied
	}
	return parsed_succesfully;
}

// a simple line lexer

mpc_lexer_t ** mpcl_line(MPC_LEX_ACTION(EOL_ACTION), MPC_LEX_ACTION(LINE_ACTION)) {
	mpc_parser_t * EOL = mpc_new("EOL");
	mpc_define(EOL, mpc_or(2, mpc_newline(), mpc_eoi));
	mpc_parser_t * Line = mpc_new("Line");
	mpc_define(Line, mpc_re("[^\n]*"));
	
	mpc_lexer_t ** lexer = mpc_lexer_new("lexer");
	mpc_lexer_add(&lexer, EOL, EOL_ACTION, NULL);
	mpc_lexer_add(&lexer, Line, LINE_ACTION, NULL);
	return lexer;
}

mpc_lexer_t ** mpcl_shline(MPC_LEX_ACTION(EOL_ACTION), MPC_LEX_ACTION(LINE_ACTION)) {
	mpc_parser_t * EOL = mpc_new("EOL");
	mpc_define(EOL, mpc_or(3, mpc_char(';'), mpc_newline(), mpc_eoi));
	mpc_parser_t * Line = mpc_new("Line");
	mpc_define(Line, mpc_re(".[^;\n]*"));
	
	mpc_lexer_t ** lexer = mpc_lexer_new("lexer");
	mpc_lexer_add(&lexer, EOL, EOL_ACTION, NULL);
	mpc_lexer_add(&lexer, Line, LINE_ACTION, NULL);
	return lexer;
}

// xs functions

struct mpc_xs_t {
	mpc_val_t **xs;
	int count;
};

mpc_xs_t *mpc_xs_undefined(void) {
  mpc_xs_t *xs = calloc(1, sizeof(mpc_xs_t));
  xs->xs = calloc(1, sizeof(mpc_val_t **));
  return xs;
}

void mpc_xs_add(mpc_xs_t **x, mpc_result_t * result) {
	mpc_xs_t * xs;
	
	if (!x) {
		xs = mpc_xs_undefined();
	} else xs = *x;
	
	if (xs->count != 1 || xs->xs) {
		mpc_val_t ** xstmp = realloc(xs->xs, sizeof(mpc_val_t **)*(xs->count+1));
		if (xstmp != NULL) xs->xs = xstmp;
		else return;
		xs->count++;
		*x = xs;
	}

	xs->xs[xs->count-1] = result->output;
}

void mpc_xs_print(mpc_xs_t * x) {
	if (!x) {
		puts("xs is not defined");
		return;
	}
	for (int i = 0; i < x->count; i++) {
		printf("x->xs[%d]: %s\n", i, (char *) x->xs[i]);
	}
}

void mpc_xs_free(mpc_xs_t * x) {
	if (!x) {
		return;
	}
	for (int i = 0; i < x->count; i++) {
		x[i].xs = NULL;
	}
	x->count = 0;
	free(x);
	x = NULL;
}
