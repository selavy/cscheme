#!/usr/bin/env python3

from enum import Enum, auto
import re


class Token(Enum):
    LPAREN = auto()
    RPAREN = auto()
    PLUS = auto()
    NUMBER = auto()
    IDENT = auto()
    DEFINE = auto()
    LAMBDA = auto()
    EOF = auto()


IDENTPAT = re.compile('[a-zA-Z_]')


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
        while i < n and IDENTPAT.match(s[i]):
            c += s[i]
            i += 1
        if c == 'define':
            kind = Token.DEFINE
        elif c == 'lambda':
            kind = Token.LAMBDA
        else:
            kind = Token.IDENT
        return i, kind, c


def lexall(s):
    i = 0
    t = None
    v = None
    rval = []
    while t != Token.EOF:
        i, t, v = lex(s, i)
        rval.append((t, v))
    return rval


class Builtin:
    def __init__(self, name):
        self.name = name


class BuiltinPlus(Builtin):
    def __init__(self):
        super().__init__(self, "<+>")

    def execute(self, params):
        result = 0.
        for x in params:
            result += x
        return result


class Symbol:
    def __init__(self, value):
        self.value = value


class Parser:
    def __init__(self, text):
        self._i = 0
        self._token = None
        self._value = None
        self._text = text
        self.advance()

    def accept(self, ttype):
        if self._token == ttype:
            self.advance()
            return True
        else:
            return False

    def cur(self):
        return (self._token, self._value)

    def advance(self):
        self._i, self._token, self._value = lex(self._text, self._i)

    def expect(self, ttype):
        if not self.accept(ttype):
            raise ValueError("Expected {}, received {}".format(
                ttype, self.tokens))

    def parse(self):
        return self.expr()

    def statement(self):
        value = self._value
        if self.accept(Token.NUMBER):
            result = float(value)
        elif self.accept(Token.PLUS):
            result = BuiltinPlus()
        elif self.accept(Token.IDENT):
            result = Symbol(value)
        else:
            raise Exception("Unexpected token: %s of type %s" %
                            (self._value, self._token))
        return result

    def expr(self):
        if self.accept(Token.LPAREN):
            result = self.expr()
            self.expect(Token.RPAREN)
        else:
            result = self.statement()
        return result
