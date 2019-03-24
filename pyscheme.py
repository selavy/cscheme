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
        super().__init__("<+>")

    def execute(self, env, params):
        result = 0.
        for x in params:
            result += x
        return result


class Lambda:
    def __init__(self, params, body):
        self.params = params
        self.body = body
        self.name = '<lambda>'
        # TEMP TEMP
        print(f"Lambda(name={self.name}, params={self.params}, body={self.body})")

    def execute(self, env, params):
        if len(params) != len(self.params):
            raise Exception(f"Lambda expected {len(self.params)} arguments, but received {len(params)}.")
        inner = dict(zip(self.params, params))
        inner['__parent'] = env
        print("Lambda.execute()")
        print(env)
        return evaluate(inner, self.body)

    def __repr__(self):
        return "Lambda"

    def __str__(self):
        return self.__repr__()


def eval_symbol(env, x):
    while env is not None:
        try:
            return env[x]
            break
        except KeyError:
            env = env.get('__parent', None)
    raise Exception(f"Unbound symbol '{self.value}'")


def evaluate(env, x):
    # TEMP TEMP
    print(f"evaluate({env}, {x})")
    if isinstance(x, str):
        result = eval_symbol(env, x)
    elif isinstance(x, Builtin):
        result = x
    elif isinstance(x, Lambda):
        result = x
    elif isinstance(x, list):
        xs = [evaluate(env, v) for v in x]
        result = xs[0].execute(env, xs[1:])
    else:
        result = x
    print(f">>> returns = {result}")
    return result


def eval_sexpr(x, env, params):
    return x.execute(env, params)


class Interpreter:
    def __init__(self, text):
        self._i = 0
        self._token = None
        self._value = None
        self._text = text
        self._advance()

    def _advance(self):
        self._i, self._token, self._value = lex(self._text, self._i)

    # TODO: default allow_eof to False?
    def _accept(self, ttype, allow_eof=True):
        if self._token == ttype:
            self._advance()
            return True
        elif not allow_eof and self._token == Token.EOF:
            raise Exception("Unexpected end of input")
        else:
            return False

    def _expect(self, ttype):
        if not self._accept(ttype):
            raise ValueError(f"Expected {ttype}, received {self._token}: '{self._value}'")

    def run(self):
        env = {}
        return evaluate(env, self.sexpr())

    def readparams(self):
        rv = []
        self._expect(Token.LPAREN)
        while not self._accept(Token.RPAREN, allow_eof=False):
            symbol = str(self._value)
            rv.append(symbol)
            self._expect(Token.IDENT)
        return rv

    def sexpr(self):
        value = self._value
        if self._accept(Token.LPAREN):
            result = []
            while not self._accept(Token.RPAREN, allow_eof=False):
                result.append(self.sexpr())
            # XXX: not sure this is correct in all cases -- unwrap lambdas
            if len(result) == 1 and isinstance(result[0], Lambda):
                result = result[0]
        elif self._accept(Token.NUMBER):
            result = float(value)
        elif self._accept(Token.PLUS):
            result = BuiltinPlus()
        elif self._accept(Token.LAMBDA):
            params = self.readparams()
            body = self.sexpr()
            result = Lambda(params, body)
        elif self._accept(Token.IDENT):
            result = str(value)
        else:
            raise Exception("Unexpected token: %s of type %s" %
                            (self._value, self._token))
        return result
