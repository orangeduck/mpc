#include "ptest.h"

void suite_core(void);
void suite_regex(void);
void suite_grammar(void);
void suite_combinators(void);

int main(int argc, char** argv) {
  (void) argc; (void) argv;
  pt_add_suite(suite_core);
  pt_add_suite(suite_regex);
  pt_add_suite(suite_grammar);
  pt_add_suite(suite_combinators);
  return pt_run();
}

