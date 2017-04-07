# Lex++

The goal of this project is to provide a flexible, regex based, C++ lexical 
scanner that takes no more that ten minutes for an
average C++ programmer on your team to download, learn, and use. To that end, I
have tried to make the installation easy, the documentation clear, and the 
interface simple and predictable with sensible defaults built in. If you are
in a hurry, skip to the Installation, and Interface sections below.

## What is a lexical scanner

A lexical scanner, also known as a scanner or a lexer, is a function or program
that separates a sequence of characters into lexical units known as tokens. 
This is a very common operation that would need to be done, for example, before
parsing some text. In many cases the tokens can be identified using a regex. 
For example, a name in C++ matches `[_[:alpha:]][_[:alnum:]]*`. The purpose of 
this library is to provide a class that automates regex based lexing.

## The Reason for this Project

It seems that every program I write needs a lexer in it somewhere. It might be
for something really simple, such as reading a data file, or it could be a 
component of an interpreter for users to interact with. For whatever reason,
I always find myself writing lexers and feeling annoyed that I am duplicating
my work. I wanted the flexibility of a lexer generator that can be configured 
with regular expressions. Though I am aware of two free packages that provide 
this functionality already, I decided to roll my own.

The lex program in various incarnations (such as GNU Flex) has been around
for a long time and is very well known. However, it is written for C and only
understands C code. Also, it does not integrate as easily as I would like into
a project.

The other package I am aware of is Boost Spirit, which includes a lexer 
generator that completely solves the above problems of using Flex in a C++ 
project. Like all Boost libraries, it is freely available, well known, and well 
implemented.  However, I thought I could make a library that was simpler to 
learn and use.  Ideally, I want my library to be as easy to use as a standard 
library class.  Its functionality and interface should be clear from a page 
comparable in size to a page from cppreference.

## Regular Expressions

Unfortunately, the standard regex library cannot be used to support
this library. The reason is that this library requires one to be
able to find the longest or best match at the beginning of a
character sequence that is not necessarily available in memory. For
example, the characters could be coming from stdin or even from raw keyboard
input. Any input iterator can be used to feed characters into the lexer. 
This library reads input one character at a time and responds as soon
as logically possible. This behavior is simple and flexible for the user. 
However, the cost is that the standard regex library cannot be used becuase it 
requires a string to search. It would require the library or the user of the 
library would need to buffer the input somehow. That would be less convenient 
and would be less responsive. For more details and an illustrative example, see 
the Implementation Details section below.

Implementing regular expressions is by far the largest and most complex task
in this library. For the user's convenience, I have tried to duplicate the 
interface of std::basic_regex as closely as possible to make my regex interface 
as predictable as possible.
 
## Interface

The interface to this library is through the following class in the 
`lexpp` namespace.
```c++
template <typename CharT, typename Traits, typename TokenT>
class lexer;

```

## Installation

This is a header only library. You need only download the lex++ directory and
then make sure your compiler can see it. Either put it in your system include
path or add "-I/your/path/to/lex++" to the flags for your compiler.

## Interface

```c++
#include "lex++/lexer.cpp"

using lexpp::lexer;

```
Like Flex and Boost Spirit, the interface for this lexer generator has the
following basic structure. A lexer is created by specifying a list of pairs.

  regex_1    code_1
  regex_2    code_2
  .         .
  .         .
  .         .
  regex_n    code_n



The created lexer then passes over the input text looking for regex matches in
the order in which they are listed. When it finds a match, it executes the 
corresponding code. The lexer provided in this library works in the same 
general way.

Reader, you are probably wondering why I bothered to create a library that
duplicates the functionality that is already easily available. The answer is
that Lex was made for C programming and Boost Spirit is complex. I wanted a 
library with none of the limitations of lex that anyone could start using
after about five minutes of looking over the documentation.

that was extremely easy to learn and


The lex program has significant limitatiions for a C++ programmer. First, it 
only understands C. The code associated to a regex must be a C function. 
Second, the lexer cannot be integrated into your program. Lex reads your
configuration file and creates a corresponding program. To use it in your 
project, it must be invoked as a separate process.

Boost Spirit solves both of those problems. It is quite flexible and well 
implemented. However, with that flexibility comes a certain complexity.

