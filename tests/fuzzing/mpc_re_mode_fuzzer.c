#include <stdint.h>
#include <string.h>
#include "mpc.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
	char *new_str = (char *)malloc(Size+1);
	if (new_str == NULL){
		return 0;
	}
	memcpy(new_str, Data, Size);
	new_str[Size] = '\0';
	
	for(int i=0;i<3;i++){
		mpc_parser_t *re0 = mpc_re_mode(new_str, i);
		mpc_delete(re0);
	}
	
	free(new_str);
	return 0;
}
