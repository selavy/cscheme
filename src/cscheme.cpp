#include <cstdio>
#include <cassert>
#include "value.h" // TODO: this is evil, must be included _before_ lex.cpp
#include "lex.cpp"

void tests();

enum Result { OK, ERROR, DONE };

Result eval(Input& in, Value& val)
{
    Token tok = lex(in, val);
    if (tok == Token::ERROR) {
        fprintf(stderr, "error: invalid input!");
        return ERROR;
    } else if (tok == Token::FINISHED) {
        return DONE;
    }

    switch (tok) {
        case Token::NUMBER:
        case Token::BOOL:
        case Token::CHAR:
        case Token::STRING:
        case Token::SYMBOL:
            return OK;
        default:
            break;
    }

    // /if (tok == Token::LPAREN) {
    // /    sexpr()
    // /}

    fprintf(stderr, "error: invalid token in context: %s\n", TokenStrings[(int)tok]);
    return ERROR;
}

int main(int argc, char** argv)
{
    tests();

    if (argc != 2) {
        printf ("usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("error: cannot open file: %s\n", argv[1]);
        return 1;
    }

    Input in(file);
    Value val;
    for (;;) {
        auto ok = eval(in, val);
        if (ok == DONE) {
            break;
        } else if (ok == ERROR) {
            fprintf(stderr, "error: failed to evaluate input!");
            break;
        }
        std::cout << "> " << val << std::endl;
    }
    printf("\n");

    fclose(file);

    return 0;
}

void tests()
{
    {
        auto v = mkvoid();
        assert(v.kind == Value::VOID);
    }

    {
        auto n = mknumber(42);
        assert(n.kind == Value::NUMBER);
        assert(n.num  == 42.0);
        assert(n.str.empty());
        assert(istruthy(n) == true);
        n = mknumber(0);
        assert(istruthy(n) == false);
    }

    {
        auto b = mkbool(true);
        assert(istruthy(b) == true);
        b = mkbool(false);
        assert(istruthy(b) == false);
    }

    {
        auto s = mkstring("Hello, World");
        assert(s.str == "Hello, World");
        assert(istruthy(s) == true);
        s = mkstring("");
        assert(s.str.empty());
        assert(istruthy(s) == false);
    }

    {
        auto c = mkchar('c');
        assert(c.ch == 'c');
        assert(istruthy(c) == true);
    }

    {
        auto v = mknumber(42);
        auto p = cons(&v, &NIL);
        assert(p.kind == Value::PAIR);
        assert(istruthy(p) == true);
    }

    printf("TESTS PASSED!\n\n");

}

