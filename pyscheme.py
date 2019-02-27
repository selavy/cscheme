#!/usr/bin/env python3

from enum import Enum, auto
import re


class Token(Enum):
    LPAREN = auto()
    RPAREN = auto()
    PLUS = auto()
    NUMBER = auto()
    IDENT = auto()
    EOF = auto()


def lex(s, i):
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
        # TODO: handle floating point
        while i < n and s[i].isdigit():
            c += s[i]
            i += 1
        return i, Token.NUMBER, c
    else:
        while i < n and re.match('[a-zA-Z_]', s[i]):
            c += s[i]
            i += 1
        return i, Token.IDENT, c


def lexall(s):
    i = 0
    t = None
    v = None
    rval = []
    while t != Token.EOF:
        i, t, v = lex(s, i)
        rval.append((t, v))
    return rval


class Parser(object):
    def __init__(self, text):
        self._i = 0
        self._token = None
        self._text = text
        self.advance()

    def accept(self, ttype):
        if self.cur() == ttype:
            self.advance()
            return True
        else:
            return False

    def cur(self):
        return self._token

    def advance(self):
        self._i, tok, val = lex(self._text, self._i)
        self._token = (tok, val)

    def expect(self, ttype):
        if not self.accept(ttype):
            raise ValueError("Expected {}, received {}".format(
                ttype, self.tokens))

    def parse(self):
        self.expr()

    def expr(self):
        if self.accept(Token.LPAREN):
            self.expr()
            self.expect(Token.RPAREN)
        self.statement()
