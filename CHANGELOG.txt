# Oct. 14, 2014; Dalton Woodard
- Fixed all compilation warnings in `mpc.h` and `mpc.c`; both now compile successfully on OS X under clang-600.0.51 
and gcc 4.9.1 with the following flags:
```
-std=c11 -O3 -g -Werror -Wall -Wextra -Wformat=2 -Wshadow -Wno-format-nonliteral -Wcast-align -Wwrite-strings
-Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wswitch-default
```
- Changed compilation standard from ansi to c11.
- Further small changes in source (documented in-line).

