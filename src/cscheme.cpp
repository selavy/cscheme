#include <cstdio>
#include <cassert>
#include <list>
#include "value.h" // TODO: this is evil, must be included _before_ lex.cpp
#include "lex.cpp"

void tests();

enum Result { OK, ERROR, DONE };

struct Tokens
{
    Tokens(FILE* f) noexcept : input(f), token(Token::ERROR) {}

    void next()
    {
        if (token == Token::FINISHED) {
            return;
        }
        token = lex(input, value);
        if (token == Token::ERROR) {
            throw std::runtime_error{"error parsing input"};
        }
    }

    void expect(Token t)
    {
        if (token != t) {
            // TODO: need printf-style formatter for exceptions
            throw std::runtime_error{"unexpected token"};
        }
        next();
    }

    bool match(Token t) {
        if (token == t) {
            next();
            return true;
        }
        return false;
    }

    Value& peek() {
        return value;
    }

    Input input;
    Token token;
    Value value;
};

Result eval(Tokens& tokens, Value& result)
{
    Value& value = tokens.peek();
    if (tokens.match(Token::FINISHED)) {
        return Result::DONE;
    } else if (tokens.match(Token::NUMBER)) {
        result = value;
        return Result::OK;
    } else if (tokens.match(Token::BOOL)) {
        result = value;
        return Result::OK;
    } else if (tokens.match(Token::CHAR)) {
        result = value;
        return Result::OK;
    } else if (tokens.match(Token::STRING)) {
        result = value;
        return Result::OK;
    } else if (tokens.match(Token::SYMBOL)) {
        result = value;
        return Result::OK;
    } else {
        result = mkstring("invalid input");
        return Result::ERROR;
    }
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

    Tokens tokens(file);
    tokens.next();
    Value val;
    for (;;) {
        auto ok = eval(tokens, val);
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

