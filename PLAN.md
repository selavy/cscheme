################################################################################
# Lua 5.0
################################################################################
1. Basic Types
** Nil
** Boolean
*** `true` and `false`
*** Conditionals consider false and nil as false and anything else as true.
** Number
*** Always double-precision floating-point
*** e.g.  4     0.4     4.57e-3     0.3e12     5e+20
** String
*** can delimit literal strings by matching single or double quotes
*** Escape sequences:
	\a	bell
	\b	back space
	\f	form feed
	\n	newline
	\r	carriage return
	\t	horizontal tab
	\v	vertical tab
	\\	backslash
	\"	double quote
	\'	single quote
	\[	left square bracket
	\]	right square bracket
*** specify a character in a string also by its numeric value through the escape sequence \ddd, where ddd is a sequence of up to three decimal digits
*** can delimit literal strings also by matching double square brackets [[...]]
**** Literals in this bracketed form may run for several lines, may nest, and do not interpret escape sequences.
**** Moreover, this form ignores the first character of the string when this character is a newline
** Table
*** packages are represented as tables
** Function
*** Written in Lua or C
** Userdata
** Light Userdata
** Thread

======================================================
| Scheme - http://www.r6rs.org/final/r6rs.pdf
======================================================

* Basic Types
** Boolean
*** "#t" = true, "#f" = false
** Number
** Character
*** e.g. "#\A"
*** TODO: support unicode?
** String
*** 2 symbols with the same spelling are indistinguishable
** Pair
*** implements singly-linked lists (car + crd)
** Vector
** Procedure

* Keyword Tokens
** `define`
** `if`
** `let`
** `lambda`

* Variable lookup:
** Bindings follow the lexical structure of the program: Whenseveral bindings with the same name exist, a variable refersto the binding that is closest to it, starting with its occur-rence in the program and going from inside to outside, andreferring to a top-level binding if no local binding can befound along the way

