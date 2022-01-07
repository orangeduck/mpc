#include "../mpc.h"

int main(void) {

  mpc_result_t r;
  
  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun      = mpc_new("noun");
  mpc_parser_t* Phrase    = mpc_new("phrase");
  mpc_parser_t* Doge      = mpc_new("doge");

/*  mpca_lang(MPCA_LANG_DEFAULT,
    " adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 "
    " noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; "
    " phrase    : <adjective> <noun>;                                     "
    " doge      : /^/ <phrase>* /$/;                                      ",
    Adjective, Noun, Phrase, Doge, NULL);
*/
  char language[1024] = { 0, } ;
  size_t s = fread(language, 1023, 1, stdin) ;
  printf("s: %ld\n", s) ;
  printf("%s\n", language) ;

  mpca_lang(MPCA_LANG_DEFAULT, language, Adjective, Noun, Phrase, Doge, NULL) ;
  
  mpc_print(Adjective) ;
  mpc_print(Noun) ;
  mpc_print(Phrase) ;
  mpc_print(Doge) ;
  
  if (mpc_parse_contents("./doge_exprs/book.doge", Doge, &r)) {
    mpc_ast_print(r.output);
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);
  
  return 0;
  
}

