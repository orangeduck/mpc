#include "../mpc.h"

static void* read_line(void* line) {
  printf("Reading Line: %s", (char*)line);
  return line;
}

int main(int argc, char **argv) {
  
  const char *input = 
    "abcHVwufvyuevuy3y436782\n"
    "\n"
    "\n"
    "rehre\n"
    "rew\n"
    "-ql.;qa\n"
    "eg";
		
  mpc_parser_t* Line = mpc_many(
    mpcf_strfold, 
    mpc_apply(mpc_re("[^\\n]*(\\n|$)"), read_line));
  
  mpc_result_t r;

  (void)argc; (void)argv;

  mpc_parse("input", input, Line, &r);
  printf("\nParsed String: %s", (char*)r.output);
  free(r.output);
  
  mpc_delete(Line);
  
  return 0;
}