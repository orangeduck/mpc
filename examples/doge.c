#include "../mpc.h"

int main(int argc, char **argv) {
  
  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun      = mpc_new("noun");
  mpc_parser_t* Phrase    = mpc_new("phrase");
  mpc_parser_t* Doge      = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT,
    " adjective : \"wow\" | \"many\" | \"so\" | \"such\";                 "
    " noun      : \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\"; "
    " phrase    : <adjective> <noun>;                                     "
    " doge      : /^/ <phrase>* /$/;                                      ",
    Adjective, Noun, Phrase, Doge, NULL);

  if (argc > 1) {
    
    mpc_result_t r;
    if (mpc_parse_contents(argv[1], Doge, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
  } else {

    mpc_result_t r;
    if (mpc_parse_pipe("<stdin>", stdin, Doge, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
  
  }

  mpc_cleanup(4, Adjective, Noun, Phrase, Doge);
  
  return 0;
  
}

