#pragma once

#include <string>
#include <cassert>

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

struct Value;
struct Pair
{
    Value* car;
    Value* cdr;
};

// TODO: use tagged pointer
struct Value
{
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
    return { Kind::VOID };
}

Value mknumber(double v) noexcept {
    return { Kind::NUMBER, "", v };
}

Value mkbool(bool v) noexcept {
    return { Kind::BOOL, "", v ? 1.0 : 0.0 };
}

Value mkstring(std::string s) noexcept {
    return { Kind::STRING, std::move(s) };
}

Value mkchar(char ch) noexcept {
    return { Kind::CHAR, "", 0.0, ch };
}

Value mkpair(Value* car, Value* cdr) noexcept {
    return { Kind::PAIR, "", 0.0, 0, { car, cdr} };
}

Value cons(Value* car, Value* cdr) noexcept {
    return mkpair(car, cdr);
}

Value NIL = mkvoid();

bool istruthy(Value v) noexcept {
    switch (v.kind) {
        case Kind::VOID:   return false;
        case Kind::NUMBER: return v.num != 0.0;
        case Kind::BOOL:   return v.num != 0.0;
        case Kind::STRING: return !v.str.empty();
        case Kind::CHAR:   return true;
        case Kind::PAIR:   return true;
        case Kind::PROC:   return true;
        case Kind::SYMBOL: return true;
    }
    assert(false);
    return false;
}

