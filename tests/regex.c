#include "../mpc.h"

#include <string.h>

/*
static bool string_eq(void* x, void* y) { return (strcmp(x, y) == 0); }
static void string_print(void* x) { printf("'%s'", (char*)x); }
*/

bool suite_regex(void) {
  
  mpc_parser_t* re0 = mpc_re("abc|bcd");
  mpc_parser_t* re1 = mpc_re("abc|bcd|e");
  mpc_parser_t* re2 = mpc_re("abc(ab)*");
  mpc_parser_t* re3 = mpc_re("abc(abdd)?");
  mpc_parser_t* re4 = mpc_re("ab|c(abdd)?");
  mpc_parser_t* re5 = mpc_re("abc(ab|dd)+g$");
  
  mpc_print(re0);
  mpc_print(re1);
  mpc_print(re2);
  mpc_print(re3);
  mpc_print(re4);
  mpc_print(re5);
  
  mpc_delete(re0);
  mpc_delete(re1);
  mpc_delete(re2);
  mpc_delete(re3);
  mpc_delete(re4);
  mpc_delete(re5);
  
  return true;
}