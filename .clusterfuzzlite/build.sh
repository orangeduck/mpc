#!/bin/bash

$CC $CFLAGS -c mpc.c
llvm-ar rcs libfuzz.a *.o


$CC $CFLAGS $LIB_FUZZING_ENGINE $SRC/fuzzer.c -Wl,--whole-archive $SRC/mpc/libfuzz.a -Wl,--allow-multiple-definition -I$SRC/mpc/ -I$SRC/mpc/tests  -o $OUT/fuzzer
