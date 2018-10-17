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

mpc_lexer_action(lexerchange1) {
	printf("lexing test mode char a '%s'\n", (char *) lexerchange1->output);
	return 0;
}

mpc_lexer_action(lexerchange1a) {
	printf("lexing test mode char d '%s'\n", (char *) lexerchange1a->output);
	return 0;
}

mpc_lexer_action(lexerchange1b) {
	printf("lexing test mode char e '%s'\n", (char *) lexerchange1b->output);
	return 0;
}

mpc_lexer_action(lexerchange2) {
	printf("lexing test mode any char '%s'\n", (char *) lexerchange2->output);
	return 0;
}

mpc_lexer_action(lexerchange3) {
	printf("lexing test mode char c '%s'\n", (char *) lexerchange3->output);
	return 0;
}

int main(int argc, char **argv) {
	char * st = "abcHVwufvyuevuy3y436782\n\n\nrehre\nrew\n-ql.;qa\neg";
	
	mpc_lexer_t ** lexer = mpcl_line(outEOL, outLINE);
	mpc_lexer_t ** lexera = mpc_lexer_new("l1");
	mpc_lexer_t ** lexerb = mpc_lexer_new("l2");
	
	mpc_result_t r;
	mpc_lexer_pool_print();
	mpc_lexer(st, lexer, &r);
	mpc_lexer_free(lexera);
	mpc_lexer_free(lexerb);
	mpc_lexer_free(lexer);
	
	
	// lets attempt to lex something that will change
	
	globalch = 'a';
	
	mpc_parser_t * Line = mpc_new("Line");
	mpc_define(Line, mpc_char(globalch));
		
	lexer = mpc_lexer_new("lexer");
	mpc_lexer_add(&lexer, Line, outCH, NULL);

	mpc_lexer_pool_print();
	mpc_lexer("ab", lexer, &r);
	mpc_lexer_free(lexer);
	
	
	
	
	
	
	
	
	
	
	
	
	// now lets attempt a mode change
	
	// a mode shall be interperated as another lexer
	
	// to verify a mode change, we first create a trigger, to do this we need two lexers
	
	
	
	// lexer 1
	mpc_lexer_t ** lexer1 = mpc_lexer_new("lexer1");
	mpc_parser_t * L1 = mpc_new("lexer1 a");
	mpc_define(L1, mpc_char('a'));
	mpc_parser_t * L1a = mpc_new("lexer1 d");
	mpc_define(L1a, mpc_char('d'));
	mpc_parser_t * L1b = mpc_new("lexer1 e");
	mpc_define(L1b, mpc_char('e'));
	
	
	
	// lexer 2
	
	
	mpc_lexer_t ** lexer2 = mpc_lexer_new("lexer2");
	mpc_parser_t * L2 = mpc_new("lexer2 any");
	mpc_define(L2, mpc_any());
	mpc_parser_t * L3 = mpc_new("lexer2 c");
	mpc_define(L3, mpc_char('c'));
	
	// construct the test lexer
	
	/*
	
	first we define a parser and a mode change, simulating the following grammer, as per lemonex parser/lexer
	
	L1 ::= "a" [lexer2] {
	// contents of action lexerchange
	}
	
	classically a mode is associated from the grammer itself, not the function, and is handled internally by the lexer
	
	*/
	
	// mpc_lexer_add(lexer, parser, action, mode);
	
	mpc_lexer_add(&lexer1, L1, lexerchange1, &lexer2);
	mpc_lexer_add(&lexer1, L1a, lexerchange1a, NULL);
	mpc_lexer_add(&lexer1, L1b, lexerchange1b, NULL);
	
	/*
	
	second, we define another parser followed by a parser with a mode return that will return to the normal lexer, grammer is usually noted by a [<] and follows the same rules
	
	*/
	
	mpc_lexer_add(&lexer2, L3, lexerchange3, &lexer1); // [<]
	mpc_lexer_add(&lexer2, L2, lexerchange2, NULL); // no mode change
	
	/*
	
	now that the test lexers have been constructed, we need to test them
	first we need to define the string based on what we know

	first our lexer looks for character a, then changes from lexer1 to lexer2
	in lexer1 if a is not found it looks for char d then char e
	next lexer2 looks for character c, then changes from lexer2 back to lexer1
	in lexer2, if c is not found it finds any other character
	
	*/
	mpc_lexer_print(lexer1);
	mpc_lexer_print(lexer2);
	mpc_lexer("abcde", lexer1, &r);
	
	mpc_lexer_free(lexer1);
	mpc_lexer_free(lexer2);
	
	
	return 0;
	
}
