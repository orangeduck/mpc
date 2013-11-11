Micro Parser Combinators
========================

_mpc_ is a lightweight but powerful Parser Combinator library for C.

Using _mpc_ might be of interest to you if you are...

* Building a new programming language
* Building a new data format
* Parsing an existing data format
* Embedding a Domain Specific Language
* Implementing [Greenspun's Tenth Rule](http://en.wikipedia.org/wiki/Greenspun%27s_tenth_rule)


Features
--------

* Type-Generic Parser Combinators
* Predictive Recursive Descent Parsers
* Error Message Support
* Regular Expression Support
* Grammar Support
* Easy to Integrate (One Source File in ANSI C)


Alternatives
------------

The current main alternative C based parser combinator is a branch of [Cesium3](https://github.com/wbhart/Cesium3/tree/combinators).

This project has several downsides which _mpc_ overcomes:

* _mpc_ Works for Generic Types
* _mpc_ Doesn't rely on Boehm-Demers-Weiser Garbage Collection
* _mpc_ Doesn't use `setjmp` and `longjmp` for errors
* _mpc_ Doesn't pollute the namespace


View From the Top
-----------------

In this example I create a parser for a basic maths language. The function `parse_maths` takes as input some mathematical expression and outputs an instance of `mpc_ast_t`.

```c
#include "mpc.h"

mpc_ast_t* parse_maths(const char* input) {

  mpc_parser_t* Expr  = mpc_new("expression");
  mpc_parser_t* Prod  = mpc_new("product");
  mpc_parser_t* Value = mpc_new("value");
  mpc_parser_t* Maths = mpc_new("maths");
  
  mpca_lang(
    "                                                       \
        expression : <product> (('+' | '-') <product>)*;    \
        product :    <value>   (('*' | '/') <value>  )*;    \
        value : /[0-9]+/ | '(' <expression> ')';            \
        maths : /^/ <expression> /$/;                       \
    ",
    Expr, Prod, Value, Maths);
  
  mpc_result_t r;  
  if (!mpc_parse("<parse_maths>", input, Maths, &r)) {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
    exit(EXIT_FAILURE);
  }
  
  mpc_cleanup(4, Expr, Prod, Value, Maths);
  
  return r.output;
}
```

If you were to input something like `"(4 * 2 * 11 + 2) - 5"` into this function the `mpc_ast_t` you get out would look something like this:

```c
>:
  value|>:
    char: '('
    expression|>:
      product|>:
        value|regex: '4'
        char: '*'
        value|regex: '2'
        char: '*'
        value|regex: '11'
      char: '+'
      product|value|regex: '2'
    char: ')'
  char: '+'
  product|value|regex: '5'
```


View From the Bottom
--------------------

Parser Combinators are structures that encode how to parse a particular language. They can be combined using intuitive operators to create new parsers of increasing complexity. Using these operators detailed grammars and languages can be parsed and processed in a quick, efficient, and easy way.

The trick behind Parser Combinators is the observation that by structuring the library in a particular way, one can make building parser combinators look like writing a grammar itself. Therefore instead of describing _how to parse a language_, a user must only specify _the language itself_, and the computer will work out how to parse it ... as if by magic!

As is shown in the above example _mpc_ takes this one step further, and actually allows you to specify the grammar directly, or to built up parsers using library functions.


Parsers
-------

The Parser Combinator type in _mpc_ is `mpc_parser_t`. This encodes a function that attempts to parse some string and, if successful, returns a pointer to some data. Otherwise it returns some error. A parser can be run using `mpc_parse`.

* * *

```c
int mpc_parse(const char* filename, const char* string, mpc_parser_t* p, mpc_result_t* r);
```

This function returns `1` on success and `0` on failure. It takes as input some parser `p`, some input `string`, and some `filename`. It outputs into `r` the result of the parse - which is either a pointer to some data object, or an error. The type `mpc_result_t` is a union type defined as follows.

```c
typedef union {
  mpc_err_t* error;
  mpc_val_t* output;
} mpc_result_t;
```

where `mpc_val_t` is synonymous with `void*` and simply represents some pointer to data - the exact type of which is dependant on the parser. Some variations on the above also exist. For almost all of the built-in and basic parsers the return type for a successful parser will be `char*`. 

* * *

```c
int mpc_fparse(const char* filename, FILE* file, mpc_parser_t* p, mpc_result_t* r);
```

Parses the contents of `file` with parser `p` and returns the result in `r`. Returns `1` on success and `0` on failure. This is also the correct method to parse input from pipes or streams.

* * *

```c
int mpc_fparse_contents(const char* filename, mpc_parser_t* p, mpc_result_t* r);
```

Opens file `filename` and parsers contents with `p`. Returns result in `r`. Returns `1` on success and `0` on failure;


Basic Parsers
-------------

### String Parsers

All the following functions return basic parsers. All of those parsers return a newly allocated `char*` with the character(s) they manage to match or an error on failure. They have the following functionality.

* * * 

```c
mpc_parser_t* mpc_any(void);
```

Matches any individual character

* * * 

```c
mpc_parser_t* mpc_char(char c);
```

Matches a single given character `c`

* * *

```c
mpc_parser_t* mpc_range(char s, char e);
```

Matches any single given character in the range `s` to `e` (inclusive)

* * *

```c
mpc_parser_t* mpc_oneof(const char* s);
```

Matches any single given character in the string  `s`

* * *

```c
mpc_parser_t* mpc_noneof(const char* s);
```
Matches any single given character not in the string `s`

* * *

```c
mpc_parser_t* mpc_satisfy(int(*f)(char));
```

Matches any single given character satisfying function `f`

* * *

```c
mpc_parser_t* mpc_string(const char* s);
```

Matches exactly the string `s`


### Trivial Parsers

Several other functions exist that return basic parsers with some other special functionality.

* * *

```c
mpc_parser_t* mpc_pass(void);
```

Consumes no input, always successful, returns `NULL`

* * *

```c
mpc_parser_t* mpc_fail(const char* m);
```

Consumes no input, always fails with message `m`.

* * *

```c
mpc_parser_t* mpc_failf(const char* fmt, ...);
```

Consumes no input, always fails with formatted message given by `fmt` and following parameters.

* * *

```c
mpc_parser_t* mpc_lift(mpc_ctor_t f);
```

Consumes no input, always successful, returns the result of function `f`

* * *

```c
mpc_parser_t* mpc_lift_val(mpc_val_t* x);
```

Consumes no input, always successful, returns `x`


Combinators
-----------

Combinators are functions that take one or more parsers and return a new parser of some given functionality. These combinators work independent of exactly what data type those input parsers return on success. In languages such as Haskell ensuring you don't input one type of data into a parser requiring a different type of data is done by the compiler. But in C we don't have that luxury. So it is at the discretion of the programmer to ensure that he or she deals correctly with the outputs of different parser types.

A second annoyance in C is that of manual memory management. Some parsers might get half-way and then fail. This means they need to clean up any partial data that has been collected in the parse. In Haskell this is handled by the Garbage Collector, but in C these combinators will need to take _destructor_ functions as input, which say how clean up any partial data that has been collected.

Here are the main combinators and how to use then.

* * *

```c
mpc_parser_t* mpc_expect(mpc_parser_t* a, const char* e);
```

Returns a parser that runs `a`, and on success returns the result of `a`, while on failure reports that `e` was expected.

* * *

```c
mpc_parser_t* mpc_apply(mpc_parser_t* a, mpc_apply_t f);
mpc_parser_t* mpc_apply_to(mpc_parser_t* a, mpc_apply_to_t f, void* x);
```

Returns a parser that applies function `f` (optionality taking extra input `x`) to the result of parser `a`.

* * *

```c
mpc_parser_t* mpc_not(mpc_parser_t* a, mpc_dtor_t da);
mpc_parser_t* mpc_not_lift(mpc_parser_t* a, mpc_dtor_t da, mpc_ctor_t lf);
```

Returns a parser with the following behaviour. If parser `a` succeeds, then it fails and consumes no input. If parser `a` fails, then it succeeds, consumes no input and returns `NULL` (or the result of lift function `lf`). Destructor `da` is used to destroy the result of `a` on success.

* * *

```c
mpc_parser_t* mpc_maybe(mpc_parser_t* a);
mpc_parser_t* mpc_maybe_lift(mpc_parser_t* a, mpc_ctor_t lf);
```

Returns a parser that runs `a`. If `a` is successful then it returns the result of `a`. If `a` is unsuccessful then it succeeds, but returns `NULL` (or the result of `lf`).

* * *

```c
mpc_parser_t* mpc_many(mpc_fold_t f, mpc_parser_t* a);
```

Keeps running `a` until it fails. Results are combined using fold function `f`. See the _Function Types_ section for more details.

* * *

```c
mpc_parser_t* mpc_many1(mpc_fold_t f, mpc_parser_t* a);
```

Attempts to run `a` one or more times. Results are combined with fold function `f`.

* * *

```c
mpc_parser_t* mpc_count(int n, mpc_fold_t f, mpc_parser_t* a, mpc_dtor_t da);
```

Attempts to run `a` exactly `n` times. If this fails, any partial results are destructed with `da`. If successful results of `a` are combined using fold function `f`.

* * *

```c
mpc_parser_t* mpc_or(int n, ...);
```

Attempts to run `n` parsers in sequence, returning the first one that succeeds. If all fail, returns an error.

* * *

```c
mpc_parser_t* mpc_and(int n, mpc_fold_t f, ...);
```

Attempts to run `n` parsers in sequence, returning the fold of the results using fold function `f`. First parsers must be specified, followed by destructors for each parser, excluding the final parser. These are used in case of partial success. For example: `mpc_and(3, mpcf_strfold, mpc_char('a'), mpc_char('b'), mpc_char('c'), free, free);` would attempt to match `'a'` followed by `'b'` followed by `'c'`, and if successful would concatenate them using `mpcf_strfold`. Otherwise would use `free` on the partial results.

* * *

```c
mpc_parser_t* mpc_predictive(mpc_parser_t* a);
```

Returns a parser that runs `a` with backtracking disabled. This means if `a` consumes any input, it will not be reverted, even on failure. Turning backtracking off has good performance benefits for grammars which are `LL(1)`. These are grammars where the first character completely determines the parse result - such as the decision of parsing either a C identifier, number, or string literal. This option should not be used for non `LL(1)` grammars or it will produce incorrect results or crash the parser.

Another way to think of `mpc_predictive` is that it can be applied to a parser (for a performance improvement) if either successfully parsing the first character will result in a completely successful parse, or all of the referenced sub-parsers are also `LL(1)`.



Function Types
--------------

The combinator functions take a number of special function types as function pointers. Here is a short explanation of those types are how they are expected to behave. It is important that these behave correctly otherwise it is exceedingly easy to introduce memory leaks or crashes into the system.

* * *

```c
typedef void(*mpc_dtor_t)(mpc_val_t*);
```

Given some pointer to a data value it will ensure the memory it points to is freed correctly.

* * *

```c
typedef mpc_val_t*(*mpc_ctor_t)(void);
```

Returns some data value when called. It can be used to create _empty_ versions of data types when certain combinators have no known default value to return. For example it may be used to return a newly allocated empty string.

* * *

```c
typedef mpc_val_t*(*mpc_apply_t)(mpc_val_t*);
typedef mpc_val_t*(*mpc_apply_to_t)(mpc_val_t*,void*);
```

This takes in some pointer to data and outputs some new or modified pointer to data, ensuring to free and old data no longer used. The `apply_to` variation takes in an extra pointer to some data such as state of the system.

* * *

```c
typedef mpc_val_t*(*mpc_fold_t)(int,mpc_val_t**);
```

This takes a list of pointers to data values and must return some combined or folded version of these data values. It must ensure to free and old data that is no longer used once after combination has taken place. This will ensure no memory is leaked.


First Example
-------------

Using the above we can create a parser that matches a C identifier with relative ease.

First we build a fold function that will concatenate two strings together - freeing any data we no longer needed. For this sake of this tutorial we will write it by hand, but this (as well as many other useful fold functions) is actually included in _mpc_ as `mpcf_strfold`.

```c
mpc_val_t* strfold(mpc_val_t* x, mpc_val_t* y) {
  char* x = calloc(1, 1);
  int i;
  for (i = 0; i < n; i++) {
    x = realloc(x, strlen(x) + strlen(xs[i]) + 1);
    strcat(x, xs[i]);
    free(xs[i]);
  }
  return x;
}
```

Then we can actually specify the grammar using combinators to say how the basic parsers are combined.

```c
char* parse_ident(char* input) {
  
  mpc_parser_t* alpha = mpc_or(2, mpc_range('a', 'z'), mpc_range('A', 'Z'));
  mpc_parser_t* digit = mpc_range('0', '9');
  mpc_parser_t* underscore = mpc_char('_');
  
  mpc_parser_t* ident0 = mpc_or(2, alpha, underscore);
  mpc_parser_t* ident1 = mpc_many(strfold, mpc_or(3, alpha, digit, underscore));
  mpc_parser_t* ident = mpc_and(2, strfold, ident0, ident1, free);
  
  mpc_result_t r;  
  if (!mpc_parse("<parse_ident>", input, ident, &r)) {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
    exit(EXIT_FAILURE);
  }
  
  mpc_delete(ident);
  
  return r.output;
}
```

Note that only `ident` must be deleted. When we input a parser into a combinator we should consider it to be part of that combinator now. This means we shouldn't create a parser and input it into multiple places of it will be doubly feed.


Self Reference
--------------

Building parsers in the above way can have issues with self-reference or cyclic-reference. To overcome this we can separate the construction of parsers into two different steps. Construction and Definition.

* * *

```c
mpc_parser_t* mpc_new(const char* name);
```

This will construct a parser called `name` which can then be used by others, including itself, without ownership being transfered. Any parser created using `mpc_new` is said to be _retained_. This means it will behave differently to a normal parser when referenced. When deleting a parser that includes a _retained_ parser, the _retained_ parser it will not be deleted along with it. To delete a retained parser `mpc_delete` must be used on it directly.

A _retained_ parser can then be defined using...

* * *

```c
mpc_parser_t* mpc_define(mpc_parser_t* p, mpc_parser_t* a);
```

This assigns the contents of parser `a` to `p`, and deletes `a`. With this technique parsers can now reference each other, as well as themselves, without trouble.

* * *

```c
mpc_parser_t* mpc_undefine(mpc_parser_t* p);
```

A final step is required. Parsers that reference each other must all be undefined before they are deleted. It is important to do any undefining before deletion. The reason for this is that to delete a parser it must look at each sub-parser that is used by it. If any of these have already been deleted a segfault is unavoidable - even if they were retained beforehand.

* * *

```c
void mpc_cleanup(int n, ...);
```

To ease the task of undefining and then deleting parsers `mpc_cleanup` can be used. It takes `n` parsers as input, and undefines them all, before deleting them all.

Note: _mpc_ may have separate stages for construction and definition, but it does not detect [left-recursive grammars](http://en.wikipedia.org/wiki/Left_recursion). These will go into an infinite loop when they attempt to parse input, and so should specified instead in right-recursive form instead.

Common Parsers
---------------

A number of common parsers are included.

* `mpc_parser_t* mpc_soi(void);` Matches only the start of input, returns `NULL`
* `mpc_parser_t* mpc_eoi(void);` Matches only the end of input, returns `NULL`
* `mpc_parser_t* mpc_space(void);` Matches any whitespace character (" \f\n\r\t\v")
* `mpc_parser_t* mpc_spaces(void);` Matches zero or more whitespace characters
* `mpc_parser_t* mpc_whitespace(void);` Matches spaces and frees the result, returns `NULL`
* `mpc_parser_t* mpc_newline(void);` Matches `'\n'`
* `mpc_parser_t* mpc_tab(void);` Matches `'\t'`
* `mpc_parser_t* mpc_escape(void);` Matches a backslash followed by any character
* `mpc_parser_t* mpc_digit(void);` Matches any character in the range `'0'` - `'9'`
* `mpc_parser_t* mpc_hexdigit(void);` Matches any character in the range `'0'` - `'9'` as well as `'A'` - `'F'` and `'a'` - `'f'`
* `mpc_parser_t* mpc_octdigit(void);` Matches any character in the range `'0'` - `'7'`
* `mpc_parser_t* mpc_digits(void);` Matches one or more digit
* `mpc_parser_t* mpc_hexdigits(void);` Matches one or more hexdigit
* `mpc_parser_t* mpc_octdigits(void);` Matches one or more octdigit
* `mpc_parser_t* mpc_lower(void);` Matches and lower case character
* `mpc_parser_t* mpc_upper(void);` Matches any upper case character
* `mpc_parser_t* mpc_alpha(void);` Matches and alphabet character
* `mpc_parser_t* mpc_underscore(void);` Matches `'_'`
* `mpc_parser_t* mpc_alphanum(void);` Matches any alphabet character, underscore or digit
* `mpc_parser_t* mpc_int(void);` Matches digits and returns an `int*`
* `mpc_parser_t* mpc_hex(void);` Matches hexdigits and returns an `int*`
* `mpc_parser_t* mpc_oct(void);` Matches octdigits and returns an `int*`
* `mpc_parser_t* mpc_number(void);` Matches `mpc_int`, `mpc_hex` or `mpc_oct`
* `mpc_parser_t* mpc_real(void);` Matches some floating point number as a string
* `mpc_parser_t* mpc_float(void);` Matches some floating point number and returns a `float*`
* `mpc_parser_t* mpc_char_lit(void);` Matches some character literal surrounded by `'`
* `mpc_parser_t* mpc_string_lit(void);` Matches some string literal surrounded by `"`
* `mpc_parser_t* mpc_regex_lit(void);` Matches some regex literal surrounded by `/`
* `mpc_parser_t* mpc_ident(void);` Matches a C style identifier


Useful Parsers
--------------

* `mpc_parser_t* mpc_start(mpc_parser_t* a);` Matches the start of input followed by `a`
* `mpc_parser_t* mpc_end(mpc_parser_t* a, mpc_dtor_t da);` Matches `a` followed by the end of input
* `mpc_parser_t* mpc_enclose(mpc_parser_t* a, mpc_dtor_t da);` Matches the start of input, `a`, and the end of input  
* `mpc_parser_t* mpc_strip(mpc_parser_t* a);` Matches `a` striping any surrounding whitespace
* `mpc_parser_t* mpc_tok(mpc_parser_t* a);` Matches `a` and strips any trailing whitespace
* `mpc_parser_t* mpc_sym(const char* s);` Matches string `s` and strips any trailing whitespace
* `mpc_parser_t* mpc_total(mpc_parser_t* a, mpc_dtor_t da);` Matches the whitespace stripped `a`, enclosed in the start and end of input
* `mpc_parser_t* mpc_between(mpc_parser_t* a, mpc_dtor_t ad, const char* o, const char* c);` Matches `a` between strings `o` and `c`
* `mpc_parser_t* mpc_parens(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between `"("` and `")"`
* `mpc_parser_t* mpc_braces(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between `"<"` and `">"`
* `mpc_parser_t* mpc_brackets(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between `"{"` and `"}"`
* `mpc_parser_t* mpc_squares(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between `"["` and `"]"`
* `mpc_parser_t* mpc_tok_between(mpc_parser_t* a, mpc_dtor_t ad, const char* o, const char* c);` Matches `a` between `o` and `c`, where `o` and `c` have their trailing whitespace striped.
* `mpc_parser_t* mpc_tok_parens(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between trailing whitespace stripped `"("` and `")"`
* `mpc_parser_t* mpc_tok_braces(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between trailing whitespace stripped `"<"` and `">"`
* `mpc_parser_t* mpc_tok_brackets(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between trailing whitespace stripped `"{"` and `"}"`
* `mpc_parser_t* mpc_tok_squares(mpc_parser_t* a, mpc_dtor_t ad);` Matches `a` between trailing whitespace stripped `"["` and `"]"`


Fold Functions
--------------

A number of common fold functions a user might want are included. They reside under the `mpcf_*` namespace.

* `void mpcf_dtor_null(mpc_val_t* x);` Empty destructor. Does nothing
* `mpc_val_t* mpcf_ctor_null(void);` Returns `NULL`
* `mpc_val_t* mpcf_ctor_str(void);` Returns `""`
* `mpc_val_t* mpcf_free(mpc_val_t* x);` Frees `x` and returns `NULL`
* `mpc_val_t* mpcf_int(mpc_val_t* x);` Converts a decimal string `x` to an `int*`
* `mpc_val_t* mpcf_hex(mpc_val_t* x);` Converts a hex string `x` to an `int*`
* `mpc_val_t* mpcf_oct(mpc_val_t* x);` Converts a oct string `x` to an `int*`
* `mpc_val_t* mpcf_float(mpc_val_t* x);` Converts a string `x` to a `float*`
* `mpc_val_t* mpcf_escape(mpc_val_t* x);` Converts a string `x` to an escaped version
* `mpc_val_t* mpcf_unescape(mpc_val_t* x);` Converts a string `x` to an unescaped version
* `mpc_val_t* mpcf_unescape(mpc_val_t* x);` Converts a string `x` to an unescaped version unescaping `\\/`
* `mpc_val_t* mpcf_fst(int n, mpc_val_t** xs);` Returns first element of `xs`
* `mpc_val_t* mpcf_snd(int n, mpc_val_t** xs);` Returns second element of `xs`
* `mpc_val_t* mpcf_trd(int n, mpc_val_t** xs);` Returns third element of `xs`
* `mpc_val_t* mpcf_fst_free(int n, mpc_val_t** xs);` Returns first element of `xs` and frees others
* `mpc_val_t* mpcf_snd_free(int n, mpc_val_t** xs);` Returns second element of `xs` and frees others
* `mpc_val_t* mpcf_trd_free(int n, mpc_val_t** xs);` Returns third element of `xs` and frees others
* `mpc_val_t* mpcf_strfold(int n, mpc_val_t** xs);` Concatenates all `xs` together as strings and returns result 
* `mpc_val_t* mpcf_maths(int n, mpc_val_t** xs);` Examines second argument as string to see which operator it is, then operators on first and third argument as if they are `int*`.


Second Example
--------------

Passing around all these function pointers might seem clumsy, but having parsers be type-generic is important as it lets users define their own syntax tree types, as well as allows them perform specific house-keeping or data processing in the parsing phase. For example we can specify a simple maths grammar that computes the result of the expression as it goes along.

We start with a fold function that will fold two `int*` into a new `int*` based on some `char*` operator.

```c
mpc_val_t* mpcf_maths(int n, mpc_val_t** xs) {
  
  int** vs = (int**)xs;
    
  if (strcmp(xs[1], "*") == 0) { *vs[0] *= *vs[2]; }
  if (strcmp(xs[1], "/") == 0) { *vs[0] /= *vs[2]; }
  if (strcmp(xs[1], "%") == 0) { *vs[0] %= *vs[2]; }
  if (strcmp(xs[1], "+") == 0) { *vs[0] += *vs[2]; }
  if (strcmp(xs[1], "-") == 0) { *vs[0] -= *vs[2]; }
  
  free(xs[1]); free(xs[2]);
  
  return xs[0];
}
```

And then we use this to specify a basic grammar, which folds together any results.

```c
int parse_maths(char* input) {

  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* Factor = mpc_new("factor");
  mpc_parser_t* Term   = mpc_new("term");
  mpc_parser_t* Maths  = mpc_new("maths");

  mpc_define(Expr, mpc_or(2, 
    mpc_and(3, mpcf_maths, Factor, mpc_oneof("*/"), Factor, free, free),
    Factor
  ));
  
  mpc_define(Factor, mpc_or(2, 
    mpc_and(3, mpcf_maths, Term, mpc_oneof("+-"), Term, free, free),
    Term
  ));
  
  mpc_define(Term, mpc_or(2, mpc_int(), mpc_parens(Expr, free)));
  mpc_define(Maths, mpc_enclose(Expr, free));
  
  mpc_result_t r;
  if (!mpc_parse("parse_maths", input, Maths, &r)) {
    mpc_err_print(r.error);
    abort();
  }

  int result = *r.output;
  free(r.output);
  
  return result;
}
```

If we supply this function with something like `(4*2)+5`, we can expect it to output `13`.


Regular Expressions
-------------------

Even with all that has been shown above, specifying parts of text can be a tedious task requiring many lines of code. So _mpc_ provides a simple regular expression matcher.

* * *

```c
mpc_parser_t* mpc_re(const char* re);
```

This returns a parser that will attempt to match the given regular expression pattern, and return the matched string on success. It does not have support for groups and match objects, but should be sufficient for simple tasks.

A cute thing about this is that it uses previous parts of the library to parse the user input string - and because _mpc_ is type generic, the parser spits out a new `mpc_parser_t` that matches the regular expression directly! It even uses many of the combinator functions indirectly as fold functions! This is a great case study in learning how to use _mpc_, so those curious are encouraged to find it in the source code.


Abstract Syntax Tree
--------------------

One can avoid passing in and around all those clumbsy function pointer if they don't care what type is output by _mpc_. For this a generic Abstract Syntax Tree type `mpc_ast_t` is included in _mpc_. The combinator functions which act on this don't need information on how to destruct or fold instances of the result as they know it will be a `mpc_ast_t`. So there are a number of combinator functions which work specifically (and only) on parsers that return this type. They reside under `mpca_*`.

Doing things via this method means that all the data processing must take place after the parsing. In many instances this is no problem or even preferable.

It also allows for one more trick. As all the fold and destructor functions are implicit, the user can simply specify the grammar of the language in some nice way and the system can try to build a parser for the AST type from this alone. For this there are two functions supplied which take in a string and output a parser. The format for these grammars is simple and familar to those who have used parser generators before. It looks something like this.

```
expression : <product> (('+' | '-') <product>)*;

product : <value>   (('*' | '/')   <value>)*;

value : /[0-9]+/ | '(' <expression> ')';

maths : /^/ <expression> /$/;
```

String literals are surrounded in double quotes `"`. Character literals in single quotes `'` and regex literals in slashes `/`. References to other parsers are surrounded in braces `<>` and referred to by name.

Parts specified one after another are parsed in order (like `mpc_and`), while parts separated by a pipe `|` are alternatives (like `mpc_or`). Parenthesis `()` are used to specify precidence. `*` can be used to mean zero or more of. `+` for one or more of. `?` for zero or one of. `!` for negation. And a number inside braces `{5}` to mean N counts of.

Rules are specified by rule name followed by a colon `:`, followed by the definition, and ending in a semicolon `;`.

Like with the regular expressions, this user input is parsed by existing parts of the _mpc_ library. It provides one of the more powerful features of the library.

* * *

```c
mpc_parser_t* mpca_grammar(const char* grammar, ...);
```

This takes in some single right hand side of a rule, as well as a list of any of the parsers it refers to, and outputs a parser that does exactly what is specified by the rule.

* * *

```c
mpc_err_t* mpca_lang(const char* lang, ...);
```

This takes in a full language (one or more rules) as well as any parsers referred to by either the right or left hand sides. Any parsers specified on the left hand side of any rule will be assigned a parser equivalent to what is specified on the right. On valid user input this returns `NULL`, while if there are any errors in the user input it will return an instance of `mpc_err_t` describing the issues.

* * *

```c
mpc_err_t* mpca_lang_file(FILE* f, ...);
```

This reads in the contents of file `f` and inputs it into `mpca_lang`.

* * *

```c
mpc_err_t* mpca_lang_filename(const char* filename, ...);
```

This opens and reads in the contents of the file given by `filename` and passes it to `mpca_lang`.


Error Reporting
---------------

_mpc_ provides some automatic generation of error messages. These can be enhanced by the user, with use of `mpc_expect`, but many of the defaults should provide both useful and readable. An example of an error message might look something like this:

```
<test>:0:3: error: expected one or more of 'a' or 'd' at 'k'
```







