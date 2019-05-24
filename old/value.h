#pragma once

#include <iostream>
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
    QUOTE,

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
    "QUOTE",
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

struct Proc
{
    std::string name;
    int builtin;
    // TODO: implement
};

// TODO: use tagged pointer
struct Value
{
    enum Kind
    {
        VOID    = 0x0,
        NUMBER  = 0x1,
        BOOL    = 0x2,
        STRING  = 0x3,
        CHAR    = 0x4,
        PAIR    = 0x5,
        PROC    = 0x6,
        BUILTIN = 0x7,
        SYMBOL  = 0x8,
    };
    Kind kind;

    // TODO: fit into union or variant
    std::string str;
    double      num;
    char        ch;
    Pair        p;
    Proc        proc;
    // TODO: how to represent pair?
    // TODO: how to represent procedure?
};

bool isvoid(const Value* v)    { return v->kind == Value::VOID; }
bool isnumber(const Value* v)  { return v->kind == Value::NUMBER; }
bool isbool(const Value* v)    { return v->kind == Value::BOOL; }
bool isstring(const Value* v)  { return v->kind == Value::STRING; }
bool ischar(const Value* v)    { return v->kind == Value::CHAR; }
bool ispair(const Value* v)    { return v->kind == Value::PAIR; }
bool isproc(const Value* v)    { return v->kind == Value::PROC; }
bool isbuiltin(const Value* v) { return v->kind == Value::BUILTIN; }
bool issymbol(const Value* v)  { return v->kind == Value::SYMBOL; }


Value* mkvoid() noexcept {
    return new Value{ Value::VOID };
}

Value* mknumber(double v) noexcept {
    return new Value{ Value::NUMBER, "", v };
}

Value* mkbool(bool v) noexcept {
    return new Value{ Value::BOOL, "", v ? 1.0 : 0.0 };
}

Value* mkstring(std::string s) noexcept {
    return new Value{ Value::STRING, std::move(s) };
}

Value* mkchar(char ch) noexcept {
    return new Value{ Value::CHAR, "", 0.0, ch };
}

Value* mkpair(Value* car, Value* cdr) noexcept {
    return new Value{ Value::PAIR, "", 0.0, 0, { car, cdr} };
}

Value* cons(Value* car, Value* cdr) noexcept {
    return mkpair(car, cdr);
}

Value* mkbuiltin(std::string name, int builtin) noexcept {
    Value* v = new Value;
    v->kind = Value::BUILTIN;
    v->proc.name = std::move(name);
    v->proc.builtin = builtin;
    return v;
}

Value* NIL = mkvoid();

inline bool istruthy(const Value* v) noexcept {
    switch (v->kind) {
        case Value::VOID:   return false;
        case Value::NUMBER: return v->num != 0.0;
        case Value::BOOL:   return v->num != 0.0;
        case Value::STRING: return !v->str.empty();
        case Value::CHAR:
        case Value::PAIR:
        case Value::PROC:
        case Value::BUILTIN:
        case Value::SYMBOL:
            return true;
    }
    assert(false);
    return false;
}

inline std::ostream& operator<<(std::ostream& os, const Value& v)
{
    switch (v.kind)
    {
        case Value::VOID:    os << "#<void>"; break;
        case Value::NUMBER:  os << v.num; break;
        case Value::BOOL:    os << (istruthy(&v) ? "#t" : "#f"); break;
        case Value::STRING:  os << "\"" << v.str << "\""; break;
        case Value::CHAR:    os << "#\\" << v.ch; break;
        case Value::PAIR:    os << "( " << *v.p.car << " . " << *v.p.cdr << " )"; break;
        case Value::PROC:    os << "#<procedure>";
        case Value::BUILTIN: os << "#<builtin:" << v.proc.name << ">"; break;
        case Value::SYMBOL:  os << "'" << v.str; break;
    }
    return os;
}

