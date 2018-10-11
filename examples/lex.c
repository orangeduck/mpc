#include "../mpc.h"
#include "../mpc.c"

mpc_lexer_action(outEOL) {
	printf("lexing End Of Line: '%s'\n", (char *) outEOL->output);
	return 0;
}

mpc_lexer_action(outLINE) {
	printf("lexing Line: '%s'\n", (char *) outLINE->output);
	return 0;
}
char globalch = 0;

mpc_lexer_action(outCH) {
	printf("lexing Line: '%s'\n", (char *) outCH->output);
	puts("changing globalch to 'b'");
	globalch = 'b';
	mpc_define(self->parser, mpc_char(globalch));
	return 0;
}

int main(int argc, char **argv) {
	char * st = "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg";
		
	mpc_lexer_t * lexer = mpcl_line(outEOL, outLINE);
	
	mpc_result_t r;
	mpc_lexer(st, lexer, &r);
	mpc_lexer_free(lexer);
	
	
	// lets attempt to lex something that will change
	
	globalch = 'a';
	
	mpc_parser_t * Line = mpc_new("Line");
	mpc_define(Line, mpc_char(globalch));
		
	lexer = mpc_lexer_new("lexer");
	mpc_lexer_add(&lexer, Line, outCH);

	mpc_lexer("ab", lexer, &r);
	mpc_lexer_free(lexer);
	
	return 0;
	
}
