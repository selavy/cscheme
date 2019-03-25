from pyscheme import lex, Token, Interpreter


def lexall(s):
    i = 0
    t = None
    v = None
    rval = []
    while t != Token.EOF:
        i, t, v = lex(s, i)
        rval.append((t, v))
    return rval


def test_lex_single_token():
    ts = lexall("+")
    assert ts == [(Token.IDENT, '+'), (Token.EOF, None)]


def test_lex_one_level_expression():
    ts = lexall('(+ 1 2)')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.IDENT,  '+'),
        (Token.NUMBER, '1'),
        (Token.NUMBER, '2'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
    ]


def test_lex_nested_expression():
    ts = lexall('(+ (+ 1 2) (+ 3 4))')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.IDENT,  '+'),
        (Token.LPAREN, '('),
        (Token.IDENT,  '+'),
        (Token.NUMBER, '1'),
        (Token.NUMBER, '2'),
        (Token.RPAREN, ')'),
        (Token.LPAREN, '('),
        (Token.IDENT,  '+'),
        (Token.NUMBER, '3'),
        (Token.NUMBER, '4'),
        (Token.RPAREN, ')'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
        ]


def test_lex_lambda_expression():
    ts = lexall('''
        (define foo
            (lambda (x y)
                (+ x y)
            )
        )
        ''')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.DEFINE, 'define'),
        (Token.IDENT,  'foo'),
        (Token.LPAREN, '('),
        (Token.LAMBDA, 'lambda'),
        (Token.LPAREN, '('),
        (Token.IDENT,  'x'),
        (Token.IDENT,  'y'),
        (Token.RPAREN, ')'),
        (Token.LPAREN, '('),
        (Token.IDENT,  '+'),
        (Token.IDENT,  'x'),
        (Token.IDENT,  'y'),
        (Token.RPAREN, ')'),
        (Token.RPAREN, ')'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
    ]

    ts2 = lexall('(define foo (lambda (x y) (+ x y)))')
    assert ts == ts2


def test_parser_advance():
    text = '(+ x y)'
    p = Interpreter(text)

    def cur():
        return (p._token, p._value)

    assert cur() == (Token.LPAREN, '(')
    p._advance()
    assert cur() == (Token.IDENT, '+')
    p._advance()
    assert cur() == (Token.IDENT, 'x')
    p._advance()
    assert cur() == (Token.IDENT, 'y')
    p._advance()
    assert cur() == (Token.RPAREN, ')')
    p._advance()
    assert cur() == (Token.EOF, None)

    # repeat advance while at EOF should be safe
    for i in range(10):
        p._advance()
        assert cur() == (Token.EOF, None)


def test_parse_number():
    values = [
        '1',
        '42',
        '100000',
        '1010101',
        '4123435',
    ]
    for value in values:
        p = Interpreter(value)
        assert p.run() == float(value)


def test_parse_plus_expression():
    p = Interpreter('(+ 1 2)')
    assert p.run() == 3.

    p = Interpreter('(+ 1 2 3 4 5)')
    assert p.run() == 15.

    p = Interpreter('(- 1)')
    assert p.run() == -1.

    p = Interpreter('(+)')
    assert p.run() == 0.

    p = Interpreter('(-)')
    assert p.run() == 0.

    p = Interpreter('(- 3 2)')
    assert p.run() == 1.

    p = Interpreter('(- 5 1 1)')
    assert p.run() == 3.

    p = Interpreter('5')
    assert p.run() == 5


def test_eval_nested_plus_expr():
    p = Interpreter('(+ (+ 1 2) (+ 3 4))')
    assert p.run() == 10.

    p = Interpreter('(+ (+ 1 1) (+ 1 1) (+ 1 1))')
    assert p.run() == 6.

    p = Interpreter('(= 1 1)')
    assert p.run() == True

    p = Interpreter('(= 1 1 1 1 1)')
    assert p.run() == True

    p = Interpreter('(= 1 2)')
    assert p.run() == False

    p = Interpreter('(not (= 1 2))')
    assert p.run() == True


def test_eval_lambda():
    p = Interpreter("((lambda (x) 5) 1)")
    assert p.run() == 5

    p = Interpreter("((lambda (x) (+ x 1)) 5)")
    assert p.run() == 6

    p = Interpreter("((lambda (x y) (+ x y)) 3 4)")
    assert p.run() == 7

    p = Interpreter("((lambda (x y) (+ x y)) ((lambda (x) 3) 1) ((lambda (x) 4) 1))")
    assert p.run() == 7

    p = Interpreter("(((lambda (x) (lambda (x) 5)) 4) 3)")
    assert p.run() == 5


def test_eval_multiple_exprs():
    p = Interpreter("5 4 3 2 1 0")
    assert p.run() == 0

    p = Interpreter("(define x 5) x")
    assert p.run() == 5


def test_eval_define():
    p = Interpreter("(define addone (lambda (x) (+ x 1))) (addone 2)")
    assert p.run() == 3

    p = Interpreter("""
(define myadd (lambda (x y) (+ x y)))
(define addone (lambda (x y) (x y 1)))
(addone myadd 2)
""")
    assert p.run() == 3


def test_eval_conditional():
    p = Interpreter("(if (= 1 1) 1 2)")
    assert p.run() == 1

    p = Interpreter("(if (not (= 1 1)) 1 2)")
    assert p.run() == 2


def test_eval_println():
    p = Interpreter("(println 1)")
    assert p.run() == None
