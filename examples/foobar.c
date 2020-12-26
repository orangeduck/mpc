#include "../pcq.h"

int main(int argc, char** argv) {

    pcq_result_t r;
    pcq_parser_t* Foobar;

    if (argc != 2) {
      printf("Usage: ./foobar <foo/bar>\n");
      exit(0);
    }

    Foobar = pcq_new("foobar");
    pcqa_lang(PCQA_LANG_DEFAULT, "foobar : \"foo\" | \"bar\";", Foobar);
    
    if (pcq_parse("<stdin>", argv[1], Foobar, &r)) {
        pcq_ast_print(r.output);
        pcq_ast_delete(r.output);
    } else {
        pcq_err_print(r.error);
        pcq_err_delete(r.error);
    }
    
    pcq_cleanup(1, Foobar);
    
    return 0;
}

