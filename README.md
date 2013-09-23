Micro Parser Combinators
========================

_mpc_ is a lightweight Parser Combinator library for C.

Features
--------

* Full Type Generic Parser Combinator
* Error Message Support
* Regular Expression Support
* Packaged with AST generator
* Easy to including in source
* Written in clean ANSI C

Alternatives
------------

The current main alternative is a branch of (https://github.com/wbhart/Cesium3)[Cesium3].

The main advantages of _mpc_ over this are:

* Works for Generic Types
* Doesn't rely on Boehm-Demers-Weiser Garbage Collection
* Doesn't use `setjmp` and `longjmp` for errors
* Doesn't pollute namespace

Example
-------

```c



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

If you want to do all the data processing after the parsing stage _mpc_ comes packaged with a basic AST type which makes the grammar declaration much cleaner as you don't have to pass around destructors and fold functions. All these functions reside under `mpca_*`.

This also allows for the use of parser grammars that can be declared directly in C strings similarly to regular expressions.

```c


```




Reference
---------






