#include "../mpc.h"

int main(int argc, char * argv[]) {

  mpc_result_t r;
  
  mpc_parser_t* Adjective = mpc_new("adjective");
  mpc_parser_t* Noun      = mpc_new("noun");
  mpc_parser_t* Phrase    = mpc_new("phrase");
  mpc_parser_t* Doge      = mpc_new("doge");

  FILE * fp = 0x0 ;
  if (argc == 2) {
	  fp = fopen(argv[1], "rb") ;
  }
  else {
	  return 0 ;
  }

  mpca_lang_file(MPCA_LANG_DEFAULT, fp, Adjective, Noun, Phrase, Doge, NULL) ;
  fclose(fp) ;
  
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

