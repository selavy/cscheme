from pyscheme import Token, lexall, Parser


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

    ts2 = lexall('(define foo (lambda (x y) (+ x y)))')
    assert ts == ts2


def test_parser_advance():
    text = '(+ x y)'
    p = Parser(text)
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
