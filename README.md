Micro Parser Combinators
========================

_mpc_ is a lightweight Parser Combinator library for C.

The current main alternative is a branch of (https://github.com/wbhart/Cesium3)[Cesium3].

Features & Advantages
---------------------

* Error Message Support
* Regular Expression Support
* Parser Grammar Support
* Works for Generic Types
* AST Extension
* Single source & header files
* Written in clean ANSI C
* Doesn't rely on Boehm-Demers-Weiser Garbage Collection
* Doesn't use `setjmp` and `longjmp` for errors
* Doesn't pollute namespace

Example
-------

```c

mpc_val_t* combine_maths(int n, mpc_val_t** xs) {
  
  int** vs = (int**)xs;
  
  if (*vs[1] == '*') { *vs[0] *= *vs[2]; }
  if (*vs[1] == '/') { *vs[0] /= *vs[2]; }
  if (*vs[1] == '+') { *vs[0] += *vs[2]; }
  if (*vs[1] == '-') { *vs[0] -= *vs[2]; }
  
  free(vs[1]);
  free(vs[2]);
  
  return vs[0];
}

int main(int argc, char** argv) {

  mpc_parser_t* Expr   = mpc_new();
  mpc_parser_t* Factor = mpc_new();
  mpc_parser_t* Term   = mpc_new();
  mpc_parser_t* Maths  = mpc_new();

  mpc_define(Expr,
    mpc_pc("cmaths ( fact ['*' | '/'] fact ) | fact", 
      combine_maths, Factor, free, Factor, free, Factor),
  );
  
  mpc_define(Factor,
    mpc_pc("cmaths ( term ['+' | '-'] term ) | term", 
      combine_maths, Term, free, Term, free, Term),
  );
  
  mpc_define(Term,
    mpc_pc("num | snd ('(' expr ')')", 
      mpc_int(), mpcf_asnd_free, Expr, free)
  );
  
  mpc_define(Maths, mpc_ends(Expr, free));
  
  mpc_delete(Expr);
  mpc_delete(Factor);
  mpc_delete(Term);
  mpc_delete(Maths);
  
}
```

Parsers
-------


Combinators
-----------


Regular Expressions
-------------------


Combinator Grammars
-------------------


Abstract Syntax Tree
--------------------


Reference
---------






