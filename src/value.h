#pragma once

#include <string>
#include <cassert>

enum class Token : int
{
    LPAREN,
    RPAREN,
    DOT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    GT,
    LT,
    GTE,
    LTE,
    NUMBER,
    BOOL,
    CHAR,
    STRING,
    SYMBOL,

    IF,
    COND,
    DEFINE,
    NIL,

    // result
    FINISHED,
    ERROR,
};

const char* TokenStrings[] = {
    "LPAREN",
    "RPAREN",
    "DOT",
    "PLUS",
    "MINUS",
    "MULTIPLY",
    "DIVIDE",
    "GT",
    "LT",
    "GTE",
    "LTE",
    "NUMBER",
    "BOOL",
    "CHAR",
    "STRING",
    "SYMBOL",
    "IF",
    "COND",
    "DEFINE",
    "NIL",
    "FINISHED",
    "ERROR",
};

struct Value;
struct Pair
{
    Value* car;
    Value* cdr;
};

// TODO: use tagged pointer
struct Value
{
    enum Kind
    {
        VOID   = 0x0,
        NUMBER = 0x1,
        BOOL   = 0x2,
        STRING = 0x3,
        CHAR   = 0x4,
        PAIR   = 0x5,
        PROC   = 0x6,
        SYMBOL = 0x7,
    };
    Kind kind;

    // TODO: fit into union or variant
    std::string str;
    double      num;
    char        ch;
    Pair        p;
    // TODO: how to represent pair?
    // TODO: how to represent procedure?
};

Value mkvoid() noexcept {
    return { Value::VOID };
}

Value mknumber(double v) noexcept {
    return { Value::NUMBER, "", v };
}

Value mkbool(bool v) noexcept {
    return { Value::BOOL, "", v ? 1.0 : 0.0 };
}

Value mkstring(std::string s) noexcept {
    return { Value::STRING, std::move(s) };
}

Value mkchar(char ch) noexcept {
    return { Value::CHAR, "", 0.0, ch };
}

Value mkpair(Value* car, Value* cdr) noexcept {
    return { Value::PAIR, "", 0.0, 0, { car, cdr} };
}

Value cons(Value* car, Value* cdr) noexcept {
    return mkpair(car, cdr);
}

Value NIL = mkvoid();

bool istruthy(Value v) noexcept {
    switch (v.kind) {
        case Value::VOID:   return false;
        case Value::NUMBER: return v.num != 0.0;
        case Value::BOOL:   return v.num != 0.0;
        case Value::STRING: return !v.str.empty();
        case Value::CHAR:   return true;
        case Value::PAIR:   return true;
        case Value::PROC:   return true;
        case Value::SYMBOL: return true;
    }
    assert(false);
    return false;
}

