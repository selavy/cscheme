#!/usr/bin/env python3

import sys
from enum import Enum, auto


class Token(Enum):
    LPAREN = auto()
    RPAREN = auto()
    PLUS = auto()
    NUMBER = auto()
    IDENT = auto()
    EOF = auto()


def lex_(s, i):
    n = len(s)
    while i < n and s[i].isspace():
        i += 1
    if not i < n:
        return i, Token.EOF, None
    c = s[i]
    i += 1
    if c == '(':
        return i, Token.LPAREN, c
    elif c == ')':
        return i, Token.RPAREN, c
    elif c == '+':
        return i, Token.PLUS, c
    elif c.isdigit():
        while i < n and s[i].isdigit():
            c += s[i]
            i += 1
        return i, Token.NUMBER, c
    else:
        while i < n and not s[i].isspace():
            c += s[i]
            i += 1
        return i, Token.IDENT, c


def lex(s):
    i = 0
    t = None
    v = None
    rval = []
    while t != Token.EOF:
        i, t, v = lex_(s, i)
        rval.append((t, v))
    return rval


if __name__ == '__main__':
    ts = lex("+")
    assert ts == [(Token.PLUS, '+'), (Token.EOF, None)], f"{ts}"

    ts = lex('(+ 1 2)')
    assert ts == [(Token.LPAREN, '('),
                  (Token.PLUS  , '+'),
                  (Token.NUMBER, '1'),
                  (Token.NUMBER, '2'),
                  (Token.RPAREN, ')'),
                  (Token.EOF   , None),], f"{ts}"

    ts = lex('(+ (+ 1 2) (+ 3 4))')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.PLUS,   '+'),
        (Token.LPAREN, '('),
        (Token.PLUS,   '+'),
        (Token.NUMBER, '1'),
        (Token.NUMBER, '2'),
        (Token.RPAREN, ')'),
        (Token.LPAREN, '('),
        (Token.PLUS,   '+'),
        (Token.NUMBER, '3'),
        (Token.NUMBER, '4'),
        (Token.RPAREN, ')'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
        ], f"{ts}"
