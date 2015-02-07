#include "../mpc.h"

int main(int argc, char** argv) {

    mpc_result_t r;
    mpc_parser_t* Foobar;

    if (argc != 2) {
      printf("Usage: ./foobar <foo/bar>\n");
      exit(0);
    }

    Foobar = mpc_new("foobar");
    mpca_lang(MPCA_LANG_DEFAULT, "foobar : \"foo\" | \"bar\";", Foobar);
    
    if (mpc_parse("<stdin>", argv[1], Foobar, &r)) {
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
    
    mpc_cleanup(1, Foobar);
    
    return 0;
}

