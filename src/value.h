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

enum
{
    TAG_NUM = 0x0,
    TAG_NIL = 0x1,
    TAG_FUN = 0x2,
    TAG_SYM = 0x3,
    TAG_STR = 0x4,

    TAG_MSK = 0x7,
    TAG_BITS = 3,
    SIGN_BIT = (1ull << 63),
    SIGN_MSK = (SIGN_BIT >> 0) | (SIGN_BIT >> 1) | (SIGN_BIT >> 2),
};

const char* tagtostr(size_t tag);

//
// Tokens
//

enum
{
    // function keywords
    F_DEFINE, F_IF, F_PLUS, F_MINUS,

    // tokens
    T_DOT, T_LPAREN, T_RPAREN, T_SYM, T_NUM, T_QUOTE, Q_DQUOTE,
};

const char* funtostr(size_t f);

//
// String Interning
//

const std::string* strintern(std::string s);
void dump_strtab();

//
// Value -> Basic Type
//

s64 totag(Value v);
u64 isneg(Value v);
s64 tonum(Value v);
u64 tofun(Value v);
const std::string& tostr(Value v);
const std::string& tosym(Value v);

//
// Make Value
//

Value mknum(u64 v);
Value mknil();
Value mkfun(int f);
Value mksym(const char* s);
Value mkstr(const char* s);

//
// Utilities
//

std::string valprint(Value v);
