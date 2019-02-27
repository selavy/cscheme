from pyscheme import Token, lex


def test_lex_single_token():
    ts = lex("+")
    assert ts == [(Token.PLUS, '+'), (Token.EOF, None)]


def test_lex_one_level_expression():
    ts = lex('(+ 1 2)')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.PLUS,   '+'),
        (Token.NUMBER, '1'),
        (Token.NUMBER, '2'),
        (Token.RPAREN, ')'),
        (Token.EOF,    None),
    ]


def test_lex_nested_expression():
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
        ]


def test_lex_lambda_expression():
    ts = lex('''
        (define foo
            (lambda (x y)
                (+ x y)
            )
        )
        ''')
    assert ts == [
        (Token.LPAREN, '('),
        (Token.IDENT,  'define'),
        (Token.IDENT,  'foo'),
        (Token.LPAREN, '('),
        (Token.IDENT,  'lambda'),
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

    ts2 = lex('(define foo (lambda (x y) (+ x y)))')
    assert ts == ts2
