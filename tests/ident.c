#include "../mpc.h"

#include <stdlib.h>
#include <string.h>

static bool string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }

bool suite_ident(void) {

  /* ^[a-zA-Z_][a-zA-Z0-9_]*$ */
  
  mpc_parser_t* Ident = mpc_new();
  
  mpc_define(Ident, mpc_ends(
      mpc_also(
        mpc_either(mpc_alpha(), mpc_underscore()),
        mpc_many1(mpc_or(3, mpc_alpha(), mpc_underscore(), mpc_digit()), mpcf_strfold),
        free, mpcf_strfold
      ),
      free)
  );
  
  mpc_print(Ident);
  
  mpc_test(Ident, "test", "test", string_eq, free, string_print);
  mpc_test(Ident, "  blah", "", string_eq, free, string_print);
  mpc_test(Ident, "anoth21er", "anoth21er", string_eq, free, string_print);
  mpc_test(Ident, "du__de", "du__de", string_eq, free, string_print);
  mpc_test(Ident, "some spaces", "", string_eq, free, string_print);
  mpc_test(Ident, "", "", string_eq, free, string_print);
  mpc_test(Ident, "18nums", "", string_eq, free, string_print);
  
  mpc_undefine(Ident);
  mpc_delete(Ident);
  
  return true;
}
