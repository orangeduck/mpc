Parce Que
=========

Version 0.9.0


About
-----

_pcq_ is a lightweight and powerful Parser Combinator library for C.

Using _pcq_ might be of interest to you if you are...

* Building a new programming language
* Building a new data format
* Parsing an existing programming language
* Parsing an existing data format
* Embedding a Domain Specific Language
* Implementing [Greenspun's Tenth Rule](http://en.wikipedia.org/wiki/Greenspun%27s_tenth_rule)


Features
--------

* Type-Generic
* Predictive, Recursive Descent
* Easy to Integrate (One Source File in ANSI C)
* Automatic Error Message Generation
* Regular Expression Parser Generator
* Language/Grammar Parser Generator


Alternatives
------------

The current main alternative for a C based parser combinator library is a branch of [Cesium3](https://github.com/wbhart/Cesium3/tree/combinators).

_pcq_ provides a number of features that this project does not offer, and also overcomes a number of potential downsides:

* _pcq_ Works for Generic Types
* _pcq_ Doesn't rely on Boehm-Demers-Weiser Garbage Collection
* _pcq_ Doesn't use `setjmp` and `longjmp` for errors
* _pcq_ Doesn't pollute the namespace


Quickstart
==========

Here is how one would use _pcq_ to create a parser for a basic mathematical expression language.

```c
pcq_parser_t *Expr  = pcq_new("expression");
pcq_parser_t *Prod  = pcq_new("product");
pcq_parser_t *Value = pcq_new("value");
pcq_parser_t *Maths = pcq_new("maths");

pcqa_lang(PCQA_LANG_DEFAULT,
  " expression : <product> (('+' | '-') <product>)*; "
  " product    : <value>   (('*' | '/')   <value>)*; "
  " value      : /[0-9]+/ | '(' <expression> ')';    "
  " maths      : /^/ <expression> /$/;               ",
  Expr, Prod, Value, Maths, NULL);

pcq_result_t r;

if (pcq_parse("input", input, Maths, &r)) {
  pcq_ast_print(r.output);
  pcq_ast_delete(r.output);
} else {
  pcq_err_print(r.error);
  pcq_err_delete(r.error);
}

pcq_cleanup(4, Expr, Prod, Value, Maths);
```

If you were to set `input` to the string `(4 * 2 * 11 + 2) - 5`, the printed output would look like this.

```
>
  regex
  expression|>
    value|>
      char:1:1 '('
      expression|>
        product|>
          value|regex:1:2 '4'
          char:1:4 '*'
          value|regex:1:6 '2'
          char:1:8 '*'
          value|regex:1:10 '11'
        char:1:13 '+'
        product|value|regex:1:15 '2'
      char:1:16 ')'
    char:1:18 '-'
    product|value|regex:1:20 '5'
  regex
```

Getting Started
===============

Introduction
------------

Parser Combinators are structures that encode how to parse particular languages. They can be combined using intuitive operators to create new parsers of increasing complexity. Using these operators detailed grammars and languages can be parsed and processed in a quick, efficient, and easy way.

The trick behind Parser Combinators is the observation that by structuring the library in a particular way, one can make building parser combinators look like writing a grammar itself. Therefore instead of describing _how to parse a language_, a user must only specify _the language itself_, and the library will work out how to parse it ... as if by magic!

_pcq_ can be used in this mode, or, as shown in the above example, you can specify the grammar directly as a string or in a file.

Basic Parsers
-------------

### String Parsers

All the following functions construct new basic parsers of the type `pcq_parser_t *`. All of those parsers return a newly allocated `char *` with the character(s) they manage to match. If unsuccessful they will return an error. They have the following functionality.

* * *

```c
pcq_parser_t *pcq_any(void);
```

Matches any individual character

* * *

```c
pcq_parser_t *pcq_char(char c);
```

Matches a single given character `c`

* * *

```c
pcq_parser_t *pcq_range(char s, char e);
```

Matches any single given character in the range `s` to `e` (inclusive)

* * *

```c
pcq_parser_t *pcq_oneof(const char *s);
```

Matches any single given character in the string  `s`

* * *

```c
pcq_parser_t *pcq_noneof(const char *s);
```

Matches any single given character not in the string `s`

* * *

```c
pcq_parser_t *pcq_satisfy(int(*f)(char));
```

Matches any single given character satisfying function `f`

* * *

```c
pcq_parser_t *pcq_string(const char *s);
```

Matches exactly the string `s`


### Other Parsers

Several other functions exist that construct parsers with some other special functionality.

* * *

```c
pcq_parser_t *pcq_pass(void);
```

Consumes no input, always successful, returns `NULL`

* * *

```c
pcq_parser_t *pcq_fail(const char *m);
pcq_parser_t *pcq_failf(const char *fmt, ...);
```

Consumes no input, always fails with message `m` or formatted string `fmt`.

* * *

```c
pcq_parser_t *pcq_lift(pcq_ctor_t f);
```

Consumes no input, always successful, returns the result of function `f`

* * *

```c
pcq_parser_t *pcq_lift_val(pcq_val_t *x);
```

Consumes no input, always successful, returns `x`

* * *

```c
pcq_parser_t *pcq_state(void);
```

Consumes no input, always successful, returns a copy of the parser state as a `pcq_state_t *`. This state is newly allocated and so needs to be released with `free` when finished with.

* * *

```c
pcq_parser_t *pcq_anchor(int(*f)(char,char));
```

Consumes no input. Successful when function `f` returns true. Always returns `NULL`.

Function `f` is a _anchor_ function. It takes as input the last character parsed, and the next character in the input, and returns success or failure. This function can be set by the user to ensure some condition is met. For example to test that the input is at a boundary between words and non-words.

At the start of the input the first argument is set to `'\0'`. At the end of the input the second argument is set to `'\0'`.



Parsing
-------

Once you've build a parser, you can run it on some input using one of the following functions. These functions return `1` on success and `0` on failure. They output either the result, or an error to a `pcq_result_t` variable. This type is defined as follows.

```c
typedef union {
  pcq_err_t *error;
  pcq_val_t *output;
} pcq_result_t;
```

where `pcq_val_t *` is synonymous with `void *` and simply represents some pointer to data - the exact type of which is dependant on the parser.


* * *

```c
int pcq_parse(const char *filename, const char *string, pcq_parser_t *p, pcq_result_t *r);
```

Run a parser on some string.

* * *

```c
int pcq_parse_file(const char *filename, FILE *file, pcq_parser_t *p, pcq_result_t *r);
```

Run a parser on some file.

* * *

```c
int pcq_parse_pipe(const char *filename, FILE *pipe, pcq_parser_t *p, pcq_result_t *r);
```

Run a parser on some pipe (such as `stdin`).

* * *

```c
int pcq_parse_contents(const char *filename, pcq_parser_t *p, pcq_result_t *r);
```

Run a parser on the contents of some file.


Combinators
-----------

Combinators are functions that take one or more parsers and return a new parser of some given functionality.

These combinators work independently of exactly what data type the parser(s) supplied as input return. In languages such as Haskell ensuring you don't input one type of data into a parser requiring a different type is done by the compiler. But in C we don't have that luxury. So it is at the discretion of the programmer to ensure that he or she deals correctly with the outputs of different parser types.

A second annoyance in C is that of manual memory management. Some parsers might get half-way and then fail. This means they need to clean up any partial result that has been collected in the parse. In Haskell this is handled by the Garbage Collector, but in C these combinators will need to take _destructor_ functions as input, which say how clean up any partial data that has been collected.

Here are the main combinators and how to use then.

* * *

```c
pcq_parser_t *pcq_expect(pcq_parser_t *a, const char *e);
pcq_parser_t *pcq_expectf(pcq_parser_t *a, const char *fmt, ...);
```

Returns a parser that runs `a`, and on success returns the result of `a`, while on failure reports that `e` was expected.

* * *

```c
pcq_parser_t *pcq_apply(pcq_parser_t *a, pcq_apply_t f);
pcq_parser_t *pcq_apply_to(pcq_parser_t *a, pcq_apply_to_t f, void *x);
```

Returns a parser that applies function `f` (optionality taking extra input `x`) to the result of parser `a`.

* * *

```c
pcq_parser_t *pcq_check(pcq_parser_t *a, pcq_dtor_t da, pcq_check_t f, const char *e);
pcq_parser_t *pcq_check_with(pcq_parser_t *a, pcq_dtor_t da, pcq_check_with_t f, void *x, const char *e);
pcq_parser_t *pcq_checkf(pcq_parser_t *a, pcq_dtor_t da, pcq_check_t f, const char *fmt, ...);
pcq_parser_t *pcq_check_withf(pcq_parser_t *a, pcq_dtor_t da, pcq_check_with_t f, void *x, const char *fmt, ...);
```

Returns a parser that applies function `f` (optionally taking extra input `x`) to the result of parser `a`. If `f` returns non-zero, then the parser succeeds and returns the value of `a` (possibly modified by `f`). If `f` returns zero, then the parser fails with message `e`, and the result of `a` is destroyed with the destructor `da`.

* * *

```c
pcq_parser_t *pcq_not(pcq_parser_t *a, pcq_dtor_t da);
pcq_parser_t *pcq_not_lift(pcq_parser_t *a, pcq_dtor_t da, pcq_ctor_t lf);
```

Returns a parser with the following behaviour. If parser `a` succeeds, then it fails and consumes no input. If parser `a` fails, then it succeeds, consumes no input and returns `NULL` (or the result of lift function `lf`). Destructor `da` is used to destroy the result of `a` on success.

* * *

```c
pcq_parser_t *pcq_maybe(pcq_parser_t *a);
pcq_parser_t *pcq_maybe_lift(pcq_parser_t *a, pcq_ctor_t lf);
```

Returns a parser that runs `a`. If `a` is successful then it returns the result of `a`. If `a` is unsuccessful then it succeeds, but returns `NULL` (or the result of `lf`).

* * *

```c
pcq_parser_t *pcq_many(pcq_fold_t f, pcq_parser_t *a);
```

Runs `a` zero or more times until it fails. Results are combined using fold function `f`. See the _Function Types_ section for more details.

* * *

```c
pcq_parser_t *pcq_many1(pcq_fold_t f, pcq_parser_t *a);
```

Runs `a` one or more times until it fails. Results are combined with fold function `f`.

* * *

```c
pcq_parser_t *pcq_count(int n, pcq_fold_t f, pcq_parser_t *a, pcq_dtor_t da);
```

Runs `a` exactly `n` times. If this fails, any partial results are destructed with `da`. If successful results of `a` are combined using fold function `f`.

* * *

```c
pcq_parser_t *pcq_or(int n, ...);
```

Attempts to run `n` parsers in sequence, returning the first one that succeeds. If all fail, returns an error.

* * *

```c
pcq_parser_t *pcq_and(int n, pcq_fold_t f, ...);
```

Attempts to run `n` parsers in sequence, returning the fold of the results using fold function `f`. First parsers must be specified, followed by destructors for each parser, excluding the final parser. These are used in case of partial success. For example: `pcq_and(3, pcqf_strfold, pcq_char('a'), pcq_char('b'), pcq_char('c'), free, free);` would attempt to match `'a'` followed by `'b'` followed by `'c'`, and if successful would concatenate them using `pcqf_strfold`. Otherwise would use `free` on the partial results.

* * *

```c
pcq_parser_t *pcq_predictive(pcq_parser_t *a);
```

Returns a parser that runs `a` with backtracking disabled. This means if `a` consumes more than one character, it will not be reverted, even on failure. Turning backtracking off has good performance benefits for grammars which are `LL(1)`. These are grammars where the first character completely determines the parse result - such as the decision of parsing either a C identifier, number, or string literal. This option should not be used for non `LL(1)` grammars or it will produce incorrect results or crash the parser.

Another way to think of `pcq_predictive` is that it can be applied to a parser (for a performance improvement) if either successfully parsing the first character will result in a completely successful parse, or all of the referenced sub-parsers are also `LL(1)`.


Function Types
--------------

The combinator functions take a number of special function types as function pointers. Here is a short explanation of those types are how they are expected to behave. It is important that these behave correctly otherwise it is easy to introduce memory leaks or crashes into the system.

* * *

```c
typedef void(*pcq_dtor_t)(pcq_val_t*);
```

Given some pointer to a data value it will ensure the memory it points to is freed correctly.

* * *

```c
typedef pcq_val_t*(*pcq_ctor_t)(void);
```

Returns some data value when called. It can be used to create _empty_ versions of data types when certain combinators have no known default value to return. For example it may be used to return a newly allocated empty string.

* * *

```c
typedef pcq_val_t*(*pcq_apply_t)(pcq_val_t*);
typedef pcq_val_t*(*pcq_apply_to_t)(pcq_val_t*,void*);
```

This takes in some pointer to data and outputs some new or modified pointer to data, ensuring to free the input data if it is no longer used. The `apply_to` variation takes in an extra pointer to some data such as global state.

* * *

```c
typedef int(*pcq_check_t)(pcq_val_t**);
typedef int(*pcq_check_with_t)(pcq_val_t**,void*);
```

This takes in some pointer to data and outputs 0 if parsing should stop with an error. Additionally, this may change or free the input data. The `check_with` variation takes in an extra pointer to some data such as global state.

* * *

```c
typedef pcq_val_t*(*pcq_fold_t)(int,pcq_val_t**);
```

This takes a list of pointers to data values and must return some combined or folded version of these data values. It must ensure to free any input data that is no longer used once the combination has taken place.


Case Study - Identifier
=======================

Combinator Method
-----------------

Using the above combinators we can create a parser that matches a C identifier.

When using the combinators we need to supply a function that says how to combine two `char *`.

For this we build a fold function that will concatenate zero or more strings together. For this sake of this tutorial we will write it by hand, but this (as well as many other useful fold functions), are actually included in _pcq_ under the `pcqf_*` namespace, such as `pcqf_strfold`.

```c
pcq_val_t *strfold(int n, pcq_val_t **xs) {
  char *x = calloc(1, 1);
  int i;
  for (i = 0; i < n; i++) {
    x = realloc(x, strlen(x) + strlen(xs[i]) + 1);
    strcat(x, xs[i]);
    free(xs[i]);
  }
  return x;
}
```

We can use this to specify a C identifier, making use of some combinators to say how the basic parsers are combined.

```c
pcq_parser_t *alpha = pcq_or(2, pcq_range('a', 'z'), pcq_range('A', 'Z'));
pcq_parser_t *digit = pcq_range('0', '9');
pcq_parser_t *underscore = pcq_char('_');

pcq_parser_t *ident = pcq_and(2, strfold,
  pcq_or(2, alpha, underscore),
  pcq_many(strfold, pcq_or(3, alpha, digit, underscore)),
  free);

/* Do Some Parsing... */

pcq_delete(ident);
```

Notice that previous parsers are used as input to new parsers we construct from the combinators. Note that only the final parser `ident` must be deleted. When we input a parser into a combinator we should consider it to be part of the output of that combinator.

Because of this we shouldn't create a parser and input it into multiple places, or it will be doubly freed.


Regex Method
------------

There is an easier way to do this than the above method. _pcq_ comes with a handy regex function for constructing parsers using regex syntax. We can specify an identifier using a regex pattern as shown below.

```c
pcq_parser_t *ident = pcq_re("[a-zA-Z_][a-zA-Z_0-9]*");

/* Do Some Parsing... */

pcq_delete(ident);
```


Library Method
--------------

Although if we really wanted to create a parser for C identifiers, a function for creating this parser comes included in _pcq_ along with many other common parsers.

```c
pcq_parser_t *ident = pcq_ident();

/* Do Some Parsing... */

pcq_delete(ident);
```

Parser References
=================

Building parsers in the above way can have issues with self-reference or cyclic-reference. To overcome this we can separate the construction of parsers into two different steps. Construction and Definition.

* * *

```c
pcq_parser_t *pcq_new(const char *name);
```

This will construct a parser called `name` which can then be used as input to others, including itself, without fear of being deleted. Any parser created using `pcq_new` is said to be _retained_. This means it will behave differently to a normal parser when referenced. When deleting a parser that includes a _retained_ parser, the _retained_ parser will not be deleted along with it. To delete a retained parser `pcq_delete` must be used on it directly.

A _retained_ parser can then be _defined_ using...

* * *

```c
pcq_parser_t *pcq_define(pcq_parser_t *p, pcq_parser_t *a);
```

This assigns the contents of parser `a` to `p`, and deletes `a`. With this technique parsers can now reference each other, as well as themselves, without trouble.

* * *

```c
pcq_parser_t *pcq_undefine(pcq_parser_t *p);
```

A final step is required. Parsers that reference each other must all be undefined before they are deleted. It is important to do any undefining before deletion. The reason for this is that to delete a parser it must look at each sub-parser that is used by it. If any of these have already been deleted a segfault is unavoidable - even if they were retained beforehand.

* * *

```c
void pcq_cleanup(int n, ...);
```

To ease the task of undefining and then deleting parsers `pcq_cleanup` can be used. It takes `n` parsers as input, and undefines them all, before deleting them all.

* * *

```c
pcq_parser_t *pcq_copy(pcq_parser_t *a);
```

This function makes a copy of a parser `a`. This can be useful when you want to
use a parser as input for some other parsers multiple times without retaining
it.

* * *

```c
pcq_parser_t *pcq_re(const char *re);
pcq_parser_t *pcq_re_mode(const char *re, int mode);
```

This function takes as input the regular expression `re` and builds a parser
for it. With the `pcq_re_mode` function optional mode flags can also be given.
Available flags are `PCQ_RE_MULTILINE` / `PCQ_RE_M` where the start of input
character `^` also matches the beginning of new lines and the end of input `$`
character also matches new lines, and `PCQ_RE_DOTALL` / `PCQ_RE_S` where the
any character token `.` also matches newlines (by default it doesn't).


Library Reference
=================

Common Parsers
--------------


<table>

  <tr><td><code>pcq_soi</code></td><td>Matches only the start of input, returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_eoi</code></td><td>Matches only the end of input, returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_boundary</code></td><td>Matches only the boundary between words, returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_boundary_newline</code></td><td>Matches the start of a new line, returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_whitespace</code></td><td>Matches any whitespace character <code>" \f\n\r\t\v"</code></td></tr>
  <tr><td><code>pcq_whitespaces</code></td><td>Matches zero or more whitespace characters</td></tr>
  <tr><td><code>pcq_blank</code></td><td>Matches whitespaces and frees the result, returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_newline</code></td><td>Matches <code>'\n'</code></td></tr>
  <tr><td><code>pcq_tab</code></td><td>Matches <code>'\t'</code></td></tr>
  <tr><td><code>pcq_escape</code></td><td>Matches a backslash followed by any character</td></tr>
  <tr><td><code>pcq_digit</code></td><td>Matches any character in the range <code>'0'</code> - <code>'9'</code></td></tr>
  <tr><td><code>pcq_hexdigit</code></td><td>Matches any character in the range <code>'0</code> - <code>'9'</code> as well as <code>'A'</code> - <code>'F'</code> and <code>'a'</code> - <code>'f'</code></td></tr>
  <tr><td><code>pcq_octdigit</code></td><td>Matches any character in the range <code>'0'</code> - <code>'7'</code></td></tr>
  <tr><td><code>pcq_digits</code></td><td>Matches one or more digit</td></tr>
  <tr><td><code>pcq_hexdigits</code></td><td>Matches one or more hexdigit</td></tr>
  <tr><td><code>pcq_octdigits</code></td><td>Matches one or more octdigit</td></tr>
  <tr><td><code>pcq_lower</code></td><td>Matches any lower case character</td></tr>
  <tr><td><code>pcq_upper</code></td><td>Matches any upper case character</td></tr>
  <tr><td><code>pcq_alpha</code></td><td>Matches any alphabet character</td></tr>
  <tr><td><code>pcq_underscore</code></td><td>Matches <code>'_'</code></td></tr>
  <tr><td><code>pcq_alphanum</code></td><td>Matches any alphabet character, underscore or digit</td></tr>
  <tr><td><code>pcq_int</code></td><td>Matches digits and returns an <code>int*</code></td></tr>
  <tr><td><code>pcq_hex</code></td><td>Matches hexdigits and returns an <code>int*</code></td></tr>
  <tr><td><code>pcq_oct</code></td><td>Matches octdigits and returns an <code>int*</code></td></tr>
  <tr><td><code>pcq_number</code></td><td>Matches <code>pcq_int</code>, <code>pcq_hex</code> or <code>pcq_oct</code></td></tr>
  <tr><td><code>pcq_real</code></td><td>Matches some floating point number as a string</td></tr>
  <tr><td><code>pcq_float</code></td><td>Matches some floating point number and returns a <code>float*</code></td></tr>
  <tr><td><code>pcq_char_lit</code></td><td>Matches some character literal surrounded by <code>'</code></td></tr>
  <tr><td><code>pcq_string_lit</code></td><td>Matches some string literal surrounded by <code>"</code></td></tr>
  <tr><td><code>pcq_regex_lit</code></td><td>Matches some regex literal surrounded by <code>/</code></td></tr>
  <tr><td><code>pcq_ident</code></td><td>Matches a C style identifier</td></tr>

</table>


Useful Parsers
--------------

<table>

  <tr><td><code>pcq_startswith(pcq_parser_t *a);</code></td><td>Matches the start of input followed by <code>a</code></td></tr>
  <tr><td><code>pcq_endswith(pcq_parser_t *a, pcq_dtor_t da);</code></td><td>Matches <code>a</code> followed by the end of input</td></tr>
  <tr><td><code>pcq_whole(pcq_parser_t *a, pcq_dtor_t da);</code></td><td>Matches the start of input, <code>a</code>, and the end of input</td></tr>
  <tr><td><code>pcq_stripl(pcq_parser_t *a);</code></td><td>Matches <code>a</code> first consuming any whitespace to the left</td></tr>
  <tr><td><code>pcq_stripr(pcq_parser_t *a);</code></td><td>Matches <code>a</code> then consumes any whitespace to the right</td></tr>
  <tr><td><code>pcq_strip(pcq_parser_t *a);</code></td><td>Matches <code>a</code> consuming any surrounding whitespace</td></tr>
  <tr><td><code>pcq_tok(pcq_parser_t *a);</code></td><td>Matches <code>a</code> and consumes any trailing whitespace</td></tr>
  <tr><td><code>pcq_sym(const char *s);</code></td><td>Matches string <code>s</code> and consumes any trailing whitespace</td></tr>
  <tr><td><code>pcq_total(pcq_parser_t *a, pcq_dtor_t da);</code></td><td>Matches the whitespace consumed <code>a</code>, enclosed in the start and end of input</td></tr>
  <tr><td><code>pcq_between(pcq_parser_t *a, pcq_dtor_t ad, <br /> const char *o, const char *c);</code></td><td> Matches <code>a</code> between strings <code>o</code> and <code>c</code></td></tr>
  <tr><td><code>pcq_parens(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between <code>"("</code> and <code>")"</code></td></tr>
  <tr><td><code>pcq_braces(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between <code>"<"</code> and <code>">"</code></td></tr>
  <tr><td><code>pcq_brackets(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between <code>"{"</code> and <code>"}"</code></td></tr>
  <tr><td><code>pcq_squares(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between <code>"["</code> and <code>"]"</code></td></tr>
  <tr><td><code>pcq_tok_between(pcq_parser_t *a, pcq_dtor_t ad, <br /> const char *o, const char *c);</code></td><td>Matches <code>a</code> between <code>o</code> and <code>c</code>, where <code>o</code> and <code>c</code> have their trailing whitespace striped.</td></tr>
  <tr><td><code>pcq_tok_parens(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between trailing whitespace consumed <code>"("</code> and <code>")"</code></td></tr>
  <tr><td><code>pcq_tok_braces(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between trailing whitespace consumed <code>"<"</code> and <code>">"</code></td></tr>
  <tr><td><code>pcq_tok_brackets(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between trailing whitespace consumed <code>"{"</code> and <code>"}"</code></td></tr>
  <tr><td><code>pcq_tok_squares(pcq_parser_t *a, pcq_dtor_t ad);</code></td><td>Matches <code>a</code> between trailing whitespace consumed <code>"["</code> and <code>"]"</code></td></tr>

</table>


Apply Functions
---------------

<table>

  <tr><td><code>void pcqf_dtor_null(pcq_val_t *x);</code></td><td>Empty destructor. Does nothing</td></tr>
  <tr><td><code>pcq_val_t *pcqf_ctor_null(void);</code></td><td>Returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_ctor_str(void);</code></td><td>Returns <code>""</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_free(pcq_val_t *x);</code></td><td>Frees <code>x</code> and returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_int(pcq_val_t *x);</code></td><td>Converts a decimal string <code>x</code> to an <code>int*</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_hex(pcq_val_t *x);</code></td><td>Converts a hex string <code>x</code> to an <code>int*</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_oct(pcq_val_t *x);</code></td><td>Converts a oct string <code>x</code> to an <code>int*</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_float(pcq_val_t *x);</code></td><td>Converts a string <code>x</code> to a <code>float*</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_escape(pcq_val_t *x);</code></td><td>Converts a string <code>x</code> to an escaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_escape_regex(pcq_val_t *x);</code></td><td>Converts a regex <code>x</code> to an escaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_escape_string_raw(pcq_val_t *x);</code></td><td>Converts a raw string <code>x</code> to an escaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_escape_char_raw(pcq_val_t *x);</code></td><td>Converts a raw character <code>x</code> to an escaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_unescape(pcq_val_t *x);</code></td><td>Converts a string <code>x</code> to an unescaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_unescape_regex(pcq_val_t *x);</code></td><td>Converts a regex <code>x</code> to an unescaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_unescape_string_raw(pcq_val_t *x);</code></td><td>Converts a raw string <code>x</code> to an unescaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_unescape_char_raw(pcq_val_t *x);</code></td><td>Converts a raw character <code>x</code> to an unescaped version</td></tr>
  <tr><td><code>pcq_val_t *pcqf_strtriml(pcq_val_t *x);</code></td><td>Trims whitespace from the left of string <code>x</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_strtrimr(pcq_val_t *x);</code></td><td>Trims whitespace from the right of string <code>x</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_strtrim(pcq_val_t *x);</code></td><td>Trims whitespace from either side of string <code>x</code></td></tr>
</table>


Fold Functions
--------------

<table>


  <tr><td><code>pcq_val_t *pcqf_null(int n, pcq_val_t** xs);</code></td><td>Returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_fst(int n, pcq_val_t** xs);</code></td><td>Returns first element of <code>xs</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_snd(int n, pcq_val_t** xs);</code></td><td>Returns second element of <code>xs</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_trd(int n, pcq_val_t** xs);</code></td><td>Returns third element of <code>xs</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_fst_free(int n, pcq_val_t** xs);</code></td><td>Returns first element of <code>xs</code> and calls <code>free</code> on others</td></tr>
  <tr><td><code>pcq_val_t *pcqf_snd_free(int n, pcq_val_t** xs);</code></td><td>Returns second element of <code>xs</code> and calls <code>free</code> on others</td></tr>
  <tr><td><code>pcq_val_t *pcqf_trd_free(int n, pcq_val_t** xs);</code></td><td>Returns third element of <code>xs</code> and calls <code>free</code> on others</td></tr>
  <tr><td><code>pcq_val_t *pcqf_freefold(int n, pcq_val_t** xs);</code></td><td>Calls <code>free</code> on all elements of <code>xs</code> and returns <code>NULL</code></td></tr>
  <tr><td><code>pcq_val_t *pcqf_strfold(int n, pcq_val_t** xs);</code></td><td>Concatenates all <code>xs</code> together as strings and returns result </td></tr>

</table>


Case Study - Maths Language
===========================

Combinator Approach
-------------------

Passing around all these function pointers might seem clumsy, but having parsers be type-generic is important as it lets users define their own output types for parsers. For example we could design our own syntax tree type to use. We can also use this method to do some specific house-keeping or data processing in the parsing phase.

As an example of this power, we can specify a simple maths grammar, that outputs `int *`, and computes the result of the expression as it goes along.

We start with a fold function that will fold two `int *` into a new `int *` based on some `char *` operator.

```c
pcq_val_t *fold_maths(int n, pcq_val_t **xs) {

  int **vs = (int**)xs;

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
pcq_parser_t *Expr   = pcq_new("expr");
pcq_parser_t *Factor = pcq_new("factor");
pcq_parser_t *Term   = pcq_new("term");
pcq_parser_t *Maths  = pcq_new("maths");

pcq_define(Expr, pcq_or(2,
  pcq_and(3, fold_maths,
    Factor, pcq_oneof("+-"), Factor,
    free, free),
  Factor
));

pcq_define(Factor, pcq_or(2,
  pcq_and(3, fold_maths,
    Term, pcq_oneof("*/"), Term,
    free, free),
  Term
));

pcq_define(Term, pcq_or(2, pcq_int(), pcq_parens(Expr, free)));
pcq_define(Maths, pcq_whole(Expr, free));

/* Do Some Parsing... */

pcq_delete(Maths);
```

If we supply this function with something like `(4*2)+5`, we can expect it to output `13`.


Language Approach
-----------------

It is possible to avoid passing in and around all those function pointers, if you don't care what type is output by _pcq_. For this, a generic Abstract Syntax Tree type `pcq_ast_t` is included in _pcq_. The combinator functions which act on this don't need information on how to destruct or fold instances of the result as they know it will be a `pcq_ast_t`. So there are a number of combinator functions which work specifically (and only) on parsers that return this type. They reside under `pcqa_*`.

Doing things via this method means that all the data processing must take place after the parsing. In many instances this is not an issue, or even preferable.

It also allows for one more trick. As all the fold and destructor functions are implicit, the user can simply specify the grammar of the language in some nice way and the system can try to build a parser for the AST type from this alone. For this there are a few functions supplied which take in a string, and output a parser. The format for these grammars is simple and familiar to those who have used parser generators before. It looks something like this.

```
number "number" : /[0-9]+/ ;
expression      : <product> (('+' | '-') <product>)* ;
product         : <value>   (('*' | '/')   <value>)* ;
value           : <number> | '(' <expression> ')' ;
maths           : /^/ <expression> /$/ ;
```

The syntax for this is defined as follows.

<table class='table'>
  <tr><td><code>"ab"</code></td><td>The string <code>ab</code> is required.</td></tr>
  <tr><td><code>'a'</code></td><td>The character <code>a</code> is required.</td></tr>
  <tr><td><code>'a' 'b'</code></td><td>First <code>'a'</code> is required, then <code>'b'</code> is required..</td></tr>
  <tr><td><code>'a' | 'b'</code></td><td>Either <code>'a'</code> is required, or <code>'b'</code> is required.</td></tr>
  <tr><td><code>'a'*</code></td><td>Zero or more <code>'a'</code> are required.</td></tr>
  <tr><td><code>'a'+</code></td><td>One or more <code>'a'</code> are required.</td></tr>
  <tr><td><code>'a'?</code></td><td>Zero or one <code>'a'</code> is required.</td></tr>
  <tr><td><code>'a'{x}</code></td><td>Exactly <code>x</code> (integer) copies of <code>'a'</code> are required.</td></tr>
  <tr><td><code>&lt;abba&gt;</code></td><td>The rule called <code>abba</code> is required.</td></tr>
</table>

Rules are specified by rule name, optionally followed by an _expected_ string, followed by a colon `:`, followed by the definition, and ending in a semicolon `;`. Multiple rules can be specified. The _rule names_ must match the names given to any parsers created by `pcq_new`, otherwise the function will crash.

The flags variable is a set of flags `PCQA_LANG_DEFAULT`, `PCQA_LANG_PREDICTIVE`, or `PCQA_LANG_WHITESPACE_SENSITIVE`. For specifying if the language is predictive or whitespace sensitive.

Like with the regular expressions, this user input is parsed by existing parts of the _pcq_ library. It provides one of the more powerful features of the library.

* * *

```c
pcq_parser_t *pcqa_grammar(int flags, const char *grammar, ...);
```

This takes in some single right hand side of a rule, as well as a list of any of the parsers referenced, and outputs a parser that does what is specified by the rule. The list of parsers referenced can be terminated with `NULL` to get an error instead of a crash when a parser required is not supplied.

* * *

```c
pcq_err_t *pcqa_lang(int flags, const char *lang, ...);
```

This takes in a full language (zero or more rules) as well as any parsers referred to by either the right or left hand sides. Any parsers specified on the left hand side of any rule will be assigned a parser equivalent to what is specified on the right. On valid user input this returns `NULL`, while if there are any errors in the user input it will return an instance of `pcq_err_t` describing the issues. The list of parsers referenced can be terminated with `NULL` to get an error instead of a crash when a parser required is not supplied.

* * *

```c
pcq_err_t *pcqa_lang_file(int flags, FILE* f, ...);
```

This reads in the contents of file `f` and inputs it into `pcqa_lang`.

* * *

```c
pcq_err_t *pcqa_lang_contents(int flags, const char *filename, ...);
```

This opens and reads in the contents of the file given by `filename` and passes it to `pcqa_lang`.

Case Study - Tokenizer
======================

Another common task we might be interested in doing is tokenizing some block of
text (splitting the text into individual elements) and performing some function
on each one of these elements as it is read. We can do this with `pcq` too.

First, we can build a regular expression which parses an individual token. For
example if our tokens are identifiers, integers, commas, periods and colons we
could build something like this `pcq_re("\\s*([a-zA-Z_]+|[0-9]+|,|\\.|:)")`.
Next we can strip any whitespace, and add a callback function using `pcq_apply`
which gets called every time this regex is parsed successfully
`pcq_apply(pcq_strip(pcq_re("\\s*([a-zA-Z_]+|[0-9]+|,|\\.|:)")), print_token)`.
Finally we can surround all of this in `pcq_many` to parse it zero or more
times. The final code might look something like this:

```c
static pcq_val_t *print_token(pcq_val_t *x) {
  printf("Token: '%s'\n", (char*)x);
  return x;
}

int main(int argc, char **argv) {

  const char *input = "  hello 4352 ,  \n foo.bar   \n\n  test:ing   ";

  pcq_parser_t* Tokens = pcq_many(
    pcqf_all_free,
    pcq_apply(pcq_strip(pcq_re("\\s*([a-zA-Z_]+|[0-9]+|,|\\.|:)")), print_token));

  pcq_result_t r;
  pcq_parse("input", input, Tokens, &r);

  pcq_delete(Tokens);

  return 0;
}
```

Running this program will produce an output something like this:

```
Token: 'hello'
Token: '4352'
Token: ','
Token: 'foo'
Token: '.'
Token: 'bar'
Token: 'test'
Token: ':'
Token: 'ing'
```

By extending the regex we can easily extend this to parse many more types of
tokens and quickly and easily build a tokenizer for whatever language we are
interested in.


Error Reporting
===============

_pcq_ provides some automatic generation of error messages. These can be enhanced by the user, with use of `pcq_expect`, but many of the defaults should provide both useful and readable. An example of an error message might look something like this:

```
<test>:0:3: error: expected one or more of 'a' or 'd' at 'k'
```

Misc
====

Here are some other misc functions that pcq provides. These functions are susceptible to change between versions so use them with some care.

* * *

```c
void pcq_print(pcq_parser_t *p);
```

Prints out a parser in some weird format. This is generally used for debugging so don't expect to be able to understand the output right away without looking at the source code a little bit.

* * *

```c
void pcq_stats(pcq_parser_t *p);
```

Prints out some basic stats about a parser. Again used for debugging and optimisation.

* * *

```c
void pcq_optimise(pcq_parser_t *p);
```

Performs some basic optimisations on a parser to reduce it's size and increase its running speed.


Limitations & FAQ
=================

### Why would you _do_ this?!?

« Parce que je peux. »


### Does _pcq_ support Unicode?

_pcq_ Only supports ASCII. Sorry! Writing a parser library that supports Unicode is pretty difficult. I welcome contributions!


### Is _pcq_ binary safe?

No. Sorry! Including NULL characters in a string or a file will probably break it. Avoid this if possible.


### The Parser is going into an infinite loop!

While it is certainly possible there is an issue with _pcq_, it is probably the case that your grammar contains _left recursion_. This is something _pcq_ cannot deal with. _Left recursion_ is when a rule directly or indirectly references itself on the left hand side of a derivation. For example consider this left recursive grammar intended to parse an expression.

```
expr : <expr> '+' (<expr> | <int> | <string>);
```

When the rule `expr` is called, it looks the first rule on the left. This happens to be the rule `expr` again. So again it looks for the first rule on the left. Which is `expr` again. And so on. To avoid left recursion this can be rewritten (for example) as the following. Note that rewriting as follows also changes the operator associativity.

```
value : <int> | <string> ;
expr  : <value> ('+' <expr>)* ;
```

Avoiding left recursion can be tricky, but is easy once you get a feel for it. For more information you can look on [wikipedia](http://en.wikipedia.org/wiki/Left_recursion) which covers some common techniques and more examples. Possibly in the future _pcq_ will support functionality to warn the user or re-write grammars which contain left recursion, but it wont for now.


### Backtracking isn't working!

_pcq_ supports backtracking, but it may not work as you expect. It isn't a silver bullet, and you still must structure your grammar to be unambiguous. To demonstrate this behaviour examine the following erroneous grammar, intended to parse either a C style identifier, or a C style function call.

```
factor : <ident>
       | <ident> '('  <expr>? (',' <expr>)* ')' ;
```

This grammar will never correctly parse a function call because it will always first succeed parsing the initial identifier and return a factor. At this point it will encounter the parenthesis of the function call, give up, and throw an error. Even if it were to try and parse a factor again on this failure it would never reach the correct function call option because it always tries the other options first, and always succeeds with the identifier.

The solution to this is to always structure grammars with the most specific clause first, and more general clauses afterwards. This is the natural technique used for avoiding left-recursive grammars and unambiguity, so is a good habit to get into anyway.

Now the parser will try to match a function first, and if this fails backtrack and try to match just an identifier.

```
factor : <ident> '('  <expr>? (',' <expr>)* ')'
       | <ident> ;
```

An alternative, and better option is to remove the ambiguity completely by factoring out the first identifier. This is better because it removes any need for backtracking at all! Now the grammar is predictive!

```
factor : <ident> ('('  <expr>? (',' <expr>)* ')')? ;
```


### How can I avoid the maximum string literal length?

Some compilers limit the maximum length of string literals. If you have a huge language string in the source file to be passed into `pcqa_lang` you might encounter this. The ANSI standard says that 509 is the maximum length allowed for a string literal. Most compilers support greater than this. Visual Studio supports up to 2048 characters, while gcc allocates memory dynamically and so has no real limit.

There are a couple of ways to overcome this issue if it arises. You could instead use `pcqa_lang_contents` and load the language from file or you could use a string literal for each line and let the preprocessor automatically concatenate them together, avoiding the limit. The final option is to upgrade your compiler. In C99 this limit has been increased to 4095.


### The automatic tags in the AST are annoying!

When parsing from a grammar, the abstract syntax tree is tagged with different tags for each primitive type it encounters. For example a regular expression will be automatically tagged as `regex`. Character literals as `char` and strings as `string`. This is to help people wondering exactly how they might need to convert the node contents.

If you have a rule in your grammar called `string`, `char` or `regex`, you may encounter some confusion. This is because nodes will be tagged with (for example) `string` _either_ if they are a string primitive, _or_ if they were parsed via your `string` rule. If you are detecting node type using something like `strstr`, in this situation it might break. One solution to this is to always check that `string` is the innermost tag to test for string primitives, or to rename your rule called `string` to something that doesn't conflict.

Yes it is annoying but its probably not going to change!
