#!/usr/bin/env python3

from enum import Enum, auto
import re
import sys


class Token(Enum):
    LPAREN = auto()
    RPAREN = auto()
    NUMBER = auto()
    IDENT = auto()
    DEFINE = auto()
    LAMBDA = auto()
    IF = auto()
    EOF = auto()


IDENTPAT = re.compile('[a-zA-Z_+\\-*]')


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
        elif c == 'if':
            kind = Token.IF
        else:
            kind = Token.IDENT
        return i, kind, c


class Builtin:
    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return self.name

    def __str__(self):
        return self.name


class BuiltinPlus(Builtin):
    def __init__(self):
        super().__init__("<+>")

    def execute(self, args):
        result = 0.
        for x in args:
            result += x
        return result

    def __repr__(self):
        return 'Plus'

    def __str__(self):
        return self.__repr__()


class BuiltinSub(Builtin):
    def __init__(self):
        super().__init__("<->")

    def execute(self, args):
        if not args:
            result = 0.
        elif len(args) == 1:
            result = -1 * float(args[0])
        else:
            result = float(args[0])
            for x in args[1:]:
                result -= float(x)
        return result


class BuiltinEq(Builtin):
    def __init__(self):
        super().__init__("=")

    def execute(self, params):
        assert len(params) >= 1, "arity mismatch; expected at least 1 argument"
        result = True
        value = params[0]
        # TODO: coerce to correct type?
        for x in params[1:]:
            result &= (x == value)
        return result


class BuiltinNot(Builtin):
    def __init__(self):
        super().__init__("not")

    def execute(self, params):
        assert len(params) == 1, "arity mismatch; expected 1 argument"
        return not bool(params[0])


class BuiltinPrintln(Builtin):
    def __init__(self):
        super().__init__("print")

    def execute(self, params):
        xs = [str(x) for x in params]
        print(' '.join(xs))
        return None


class Lambda:
    def __init__(self, params, body):
        self.params = params
        self.body = body
        self.name = '<lambda>'
        # TEMP TEMP
        # print(f"Lambda(name={self.name}, params={self.params}, body={self.body})")

    def execute(self, env, params):
        if len(params) != len(self.params):
            raise Exception(f"Lambda expected {len(self.params)} arguments, but received {len(params)}.")
        inner = dict(zip(self.params, params))
        inner['__parent'] = env
        # print("Lambda.execute()")
        # print(env)
        return evaluate(inner, self.body)

    def __repr__(self):
        return "Lambda"

    def __str__(self):
        return self.__repr__()


class IfStmt:
    def __init__(self, result):
        self.result = result

    def execute(self, env, args):
        assert len(args) == 0
        return self.result


class Define:
    def __init__(self, symbol, value):
        self.symbol = str(symbol)
        self.value = value

    def execute(self, env, params):
        assert not params
        env[self.symbol] = self.value
        return None

    def __repr__(self):
        return f'Define({self.symbol} = {self.value})'

    def __str__(self):
        return self.__repr__()


def eval_symbol(env, x):
    while env is not None:
        try:
            return env[x]
            break
        except KeyError:
            env = env.get('__parent', None)
    raise Exception(f"Unbound symbol '{x}'")


def evaluate(env, x):
    # TEMP TEMP
    # print(f"evaluate({env}, {x})")
    if isinstance(x, str):
        result = eval_symbol(env, x)
    elif isinstance(x, float):
        result = x
    elif isinstance(x, bool):
        result = x
    elif isinstance(x, Builtin):
        result = x
    elif isinstance(x, Lambda):
        result = x
    elif isinstance(x, list):
        xs = [evaluate(env, v) for v in x]
        result = xs[0].execute(env, xs[1:])
    else:
        result = x
    # print(f">>> returns = {result}")
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

    def _accept(self, ttype):
        if self._token == ttype:
            self._advance()
            return True
        elif self._token == Token.EOF:
            raise Exception("Unexpected end of input")
        else:
            return False

    def _expect(self, ttype):
        if not self._accept(ttype):
            raise ValueError(f"Expected {ttype}, received {self._token}: '{self._value}'")

    def run(self):
        env = {
            '+': BuiltinPlus,
            '-': BuiltinSub,
            '=': BuiltinEq,
            'not': BuiltinNot,
            'println': BuiltinPrintln,
        }
        while self._token != Token.EOF:
            result = evaluate(env, self.sexpr(env))
        return result

    def readparams(self):
        rv = []
        self._expect(Token.LPAREN)
        while not self._accept(Token.RPAREN):
            symbol = str(self._value)
            rv.append(symbol)
            self._expect(Token.IDENT)
        return rv

    def sexpr(self, env):
        value = self._value
        if self._accept(Token.LPAREN):
            result = []
            while not self._accept(Token.RPAREN):
                result.append(self.sexpr(env))
            # XXX: not sure this is correct in all cases -- unwrap lambdas
            if len(result) == 1 and isinstance(result[0], Lambda):
                result = result[0]
        elif self._accept(Token.NUMBER):
            result = float(value)
        elif self._accept(Token.LAMBDA):
            params = self.readparams()
            body = self.sexpr(env)
            result = Lambda(params, body)
        elif self._accept(Token.DEFINE):
            symbol = str(self._value)
            self._expect(Token.IDENT)
            value = self.sexpr(env)
            result = Define(symbol, value)
        elif self._accept(Token.IF):
            cond = self.sexpr(env)
            if_branch = self.sexpr(env)
            else_branch = self.sexpr(env)
            cond = evaluate(env, cond)
            if bool(cond):
                rv = evaluate(env, if_branch)
            else:
                rv = evaluate(env, else_branch)
            result = IfStmt(rv)
        elif self._accept(Token.IDENT):
            result = str(value)
        else:
            raise Exception("Unexpected token: %s of type %s" %
                            (self._value, self._token))
        return result


if __name__ == '__main__':
    with open(sys.argv[1], 'r') as f:
        inputs = f.read()
    print(f"inputs = '{inputs}'")
    interp = Interpreter(inputs)
    print(interp.run())
