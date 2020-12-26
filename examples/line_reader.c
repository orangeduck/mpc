#include "../pcq.h"

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
		
  pcq_parser_t* Line = pcq_many(
    pcqf_strfold, 
    pcq_apply(pcq_re("[^\\n]*(\\n|$)"), read_line));
  
  pcq_result_t r;

  (void)argc; (void)argv;

  pcq_parse("input", input, Line, &r);
  printf("\nParsed String: %s", (char*)r.output);
  free(r.output);
  
  pcq_delete(Line);
  
  return 0;
}
