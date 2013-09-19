#include <stdbool.h>

bool suite_ident(void);
bool suite_math(void);
bool suite_regex(void);

int main(int argc, char** argv) {
  
  suite_ident();
  suite_math();
  suite_regex();
  
  return 0;
  
}