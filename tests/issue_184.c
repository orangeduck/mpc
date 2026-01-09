#include "ptest.h"
#include "../mpc.h"

/*  FIX:  Use the macro version to ensure NULL sentinel is added 
 *  Converts error to string for inspection
 *  Fuzzy checks the suggestions
*/
void test_missing_parser_detection(void) {
  mpc_parser_t* Lispy;
  mpc_err_t* err;
  char* err_msg;

  Lispy = mpc_new("lispy");
  
  err = mpca_lang(MPCA_LANG_DEFAULT, " rule : <Lispy> ; ", Lispy);
  
  PT_ASSERT(err != NULL);
  err_msg = mpc_err_string(err);
  PT_ASSERT(strstr(err_msg, "Unknown Parser 'Lispy'!") != NULL);
  PT_ASSERT(strstr(err_msg, "Did you mean 'lispy'") != NULL);
  
  /* Cleanup */
  free(err_msg);
  mpc_err_delete(err);
  mpc_cleanup(1, Lispy);
}

void suite_issue_184(void) {
  pt_add_test(test_missing_parser_detection, "Test Missing Parser Detection (#184)", "Suite Issue 184");
}
