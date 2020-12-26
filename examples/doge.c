#include "../pcq.h"

int main(int argc, char **argv) {

  pcq_result_t r;
  
  pcq_parser_t* Adjective = pcq_new("adjective");
  pcq_parser_t* Noun      = pcq_new("noun");
  pcq_parser_t* Phrase    = pcq_new("phrase");
  pcq_parser_t* Doge      = pcq_new("doge");

  pcqa_lang(PCQA_LANG_DEFAULT,
    " adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 "
    " noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; "
    " phrase    : <adjective> <noun>;                                     "
    " doge      : /^/ <phrase>* /$/;                                      ",
    Adjective, Noun, Phrase, Doge, NULL);
  
  if (argc > 1) {
    
    if (pcq_parse_contents(argv[1], Doge, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
  
  } else {

    if (pcq_parse_pipe("<stdin>", stdin, Doge, &r)) {
      pcq_ast_print(r.output);
      pcq_ast_delete(r.output);
    } else {
      pcq_err_print(r.error);
      pcq_err_delete(r.error);
    }
  
  }

  pcq_cleanup(4, Adjective, Noun, Phrase, Doge);
  
  return 0;
  
}

