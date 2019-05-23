#include <cstdio>
#include <cassert>
#include <list>
#include <vector>
#include <initializer_list>
#include "value.h" // TODO: this is evil, must be included _before_ lex.cpp
#include "lex.cpp"

enum Status { OK, ERROR, DONE };

struct Tokens
{
    Tokens(FILE* f) noexcept : input(f), token(Token::ERROR) {}

    void next()
    {
        if (token == Token::FINISHED) {
            return;
        }
        token = lex(input, &value);
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

    bool match(std::initializer_list<Token> tokens)
    {
        for (auto t : tokens) {
            if (match(t)) {
                return true;
            }
        }
        return false;
    }

    bool match(Token t)
    {
        if (token == t) {
            next();
            return true;
        }
        return false;
    }

    Value* peek() {
        return value;
    }

    Input  input;
    Token  token;
    Value* value;
};

// Value mklist(const std::vector<Value>& values)
// {
//     if (values.empty()) {
//         return NIL;
//     }
//     Value* cur = new Value(mkpair(nullptr, &NIL));
//     for (auto rit = values.rbegin(); rit != values.rend(); ++rit) {
//         cur->p.car = 
//     }
// }

#if 0
Status readsexpr(Tokens& tokens, Value& result)
{
    std::vector<Value> stk;
    Value& value = tokens.peek();
    while (!tokens.match(Token::RPAREN)) {
        if (tokens.match({ Token::FINISHED, Token::ERROR })) {
            result = mkstring("missing ')'");
            return Status::ERROR;
        }
        Value value;
        Status ok = eval(tokens, value);
        if (ok != Status::OK) {
            result = mkstring("error reading s-expression");
            return Status::ERROR;
        }
        stk.push_back(value);
    }
    result = mklist(stk);
    return Status::OK;
}
#endif

Status eval(Tokens& tokens, Value** result)
{
    Value* value = tokens.peek();
    if (tokens.match(Token::FINISHED)) {
        return Status::DONE;
    } else if (tokens.match({
                Token::NUMBER,
                Token::BOOL,
                Token::CHAR,
                Token::STRING,
                Token::SYMBOL,
                })) {
        *result = new Value(*value);
        return Status::OK;
    } else if (tokens.match(Token::LPAREN)) {
        // Status ok = readsexpr(tokens, result);
        // if (ok != Status::OK) {
        //     return ok;
        // }
        // TODO: evaluate s-expression
        return Status::OK;
    } else {
        *result = mkstring("invalid input");
        return Status::ERROR;
    }
}

int main(int argc, char** argv)
{
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
    Value* val;
    for (;;) {
        auto ok = eval(tokens, &val);
        if (ok == DONE) {
            break;
        } else if (ok == ERROR) {
            fprintf(stderr, "error: failed to evaluate input!");
            break;
        }
        std::cout << "> " << *val << std::endl;
    }
    printf("\n");

    fclose(file);

    return 0;
}
