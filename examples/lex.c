#include "../mpc.h"

mpc_lexer_action(outEOL) {
	printf("lexing End Of Line: '%s'\n", (char *) outEOL->output);
	return 0;
}

mpc_lexer_action(outLINE) {
	printf("lexing Line: '%s'\n", (char *) outLINE->output);
	return 0;
}

int main(int argc, char **argv) {
	char * st = "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg";
		
	mpc_lexer_t * lexer = mpcl_line(outEOL, outLINE);
	
	mpc_result_t r;
	mpc_lexer(st, lexer, &r);
	mpc_lexer_free(lexer);
	return 0;
	
}
