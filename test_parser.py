from pyscheme import Token, lexall, Interpreter


def test_lex_single_token():
    ts = lexall("+")
    assert ts == [(Token.PLUS, '+'), (Token.EOF, None)]


def test_lex_one_level_expression():
    ts = lexall('(+ 1 2)')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.PLUS,   '+'),
        (Token.NUMBER, '1'),
        (Token.NUMBER, '2'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
    ]


def test_lex_nested_expression():
    ts = lexall('(+ (+ 1 2) (+ 3 4))')
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
        (Token.PLUS,   '+'),
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
    assert p.cur() == (Token.LPAREN, '(')
    p.advance()
    assert p.cur() == (Token.PLUS, '+')
    p.advance()
    assert p.cur() == (Token.IDENT, 'x')
    p.advance()
    assert p.cur() == (Token.IDENT, 'y')
    p.advance()
    assert p.cur() == (Token.RPAREN, ')')
    p.advance()
    assert p.cur() == (Token.EOF, None)

    # repeat advance while at EOF should be safe
    for i in range(10):
        p.advance()
        assert p.cur() == (Token.EOF, None)


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


def test_eval_nested_plus_expr():
    p = Interpreter('(+ (+ 1 2) (+ 3 4))')
    assert p.run() == 10.

    p = Interpreter('(+ (+ 1 1) (+ 1 1) (+ 1 1))')
    assert p.run() == 6.


# def test_eval_lambda():
#     p = Interpreter("((lambda (x) 5) 1)")
#     assert p.run() == 5
# 
#     # p = Interpreter("((lambda (x) (+ x 1)) 5)")
#     # assert p.run() == 6
