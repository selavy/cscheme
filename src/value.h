#pragma once

#include <cstdint>
#include <cassert>
#include <string>

//
// Basic Type Tags
//

using s64 = int64_t;
using u64 = uint64_t;
using Value = u64;
using String = std::string;

enum Tag
{
    TAG_NUM  = 0x0,
    TAG_NIL  = 0x1,
    TAG_FUN  = 0x2,
    TAG_SYM  = 0x3,
    TAG_STR  = 0x4,

    TAG_MSK = 0x7,
    TAG_BITS = 3,
    SIGN_BIT = (1ull << 63),
    SIGN_MSK = (SIGN_BIT >> 0) | (SIGN_BIT >> 1) | (SIGN_BIT >> 2),
};

const char* tagtostr(size_t tag);

//
// Tokens
//

enum Token
{
    // function keywords
    F_DEFINE, F_IF, F_PLUS, F_MINUS, F_MULTIPLY, F_DIVIDE,
    F_GT, F_LT, F_GTE, F_LTE, F_QUOTE,

    // tokens
    T_DOT, T_LPAREN, T_RPAREN, T_SYM, T_NUM, T_STR, T_NIL,

    // finish markers
    T_EOF, T_ERROR,
};

const char* funtostr(u64 f);
const char* toktostr(u64 t);

//
// String Interning
//

const String* strintern(String s);
void dump_strtab();

//
// Value -> Basic Type
//

s64 totag(Value v);
u64 isneg(Value v);
s64 tonum(Value v);
u64 tofun(Value v);
const String& tostr(Value v);
const String& tosym(Value v);

//
// Make Value
//

Value mknum(u64 v);
Value mknil();
Value mkfun(int f);
Value mksym(const char* begin, const char* end);
Value mksym(std::string s);
Value mkstr(std::string s);

//
// Type Checks
//
bool isnum(Value v);
bool isnil(Value v);
bool isfun(Value v);
bool issym(Value v);
bool isstr(Value v);

//
// Utilities
//

std::string valprint(Value v);
const char* vprint(Value v);
bool istrue(Value v);
