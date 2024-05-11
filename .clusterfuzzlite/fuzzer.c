#include <stdint.h>
#include "mpc.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 1) {
        return 0;
    }

    // Copy the input data to a null-terminated string
    char *grammar = (char *)malloc(size + 1);
    memcpy(grammar, data, size);
    grammar[size] = '\0';

    // Call the target function with appropriate types
    int flags = 0;
    const char *grammar_arg = (const char *)grammar;
    void *dummy_arg = NULL;
    mpc_parser_t *result = mpca_grammar(flags, grammar_arg, dummy_arg);

    free(grammar);
    mpc_delete(result); // Cleanup

    return 0;
}
