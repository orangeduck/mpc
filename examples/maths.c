#include "../pcq.h"

int main(int argc, char **argv) {
  
  pcq_parser_t *Expr  = pcq_new("expression");
  pcq_parser_t *Prod  = pcq_new("product");
  pcq_parser_t *Value = pcq_new("value");
  pcq_parser_t *Maths = pcq_new("maths");
  
  pcqa_lang(PCQA_LANG_PREDICTIVE,
    " expression : <product> (('+' | '-') <product>)*; "
    " product : <value>   (('*' | '/')   <value>)*;    "
    " value : /[0-9]+/ | '(' <expression> ')';         "
    " maths : /^/ <expression> /$/;                    ",
    Expr, Prod, Value, Maths, NULL);
  
  pcq_print(Expr);
  pcq_print(Prod);
  pcq_print(Value);
  pcq_print(Maths);
  
  if (argc > 1) {
    
    pcq_result_t r;
    if (pcq_parse_contents(argv[1], Maths, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
    
  } else {

    pcq_result_t r;
    if (pcq_parse_pipe("<stdin>", stdin, Maths, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
  
  }

  pcq_cleanup(4, Expr, Prod, Value, Maths);
  
  return 0;
  
}

