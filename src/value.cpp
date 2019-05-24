#include "value.h"
#include <unordered_set>
#include <memory>
#include <cstring>

//
// Basic Type Tags
//

static const char* const TagStr[] = {
    "NUMBER",
    "NIL",
    "FUNCTION",
    "SYMBOL",
    "STRING",
};

const char* tagtostr(size_t tag) {
    if (tag > TAG_STR) {
        return "INVALID TAG";
    }
    return TagStr[tag];
}

static const char* TokenStrings[] = {
    "#<function:define>",
    "#<function:if>",
    "#<function:+>",
    "#<function:->",
    "#<function:*>",
    "#<function:/>",
    "#<function:gt>",
    "#<function:lt>",
    "#<function:gte>",
    "#<function:lte>",
    "#<function:quote>",

    "dot",
    "lparen",
    "rparen",
    "symbol",
    "number",
    "string",
    "nil",

    "eof",
    "error",
};

//
// Tokens
//

const char* funtostr(size_t f) {
    if (f > F_QUOTE) {
        assert(0);
        return "INVALID FUNCTION";
    }
    return TokenStrings[f];
}

const char* toktostr(size_t t) {
    if (t > T_ERROR)  {
        assert(0);
        return "INVALID TOKEN";
    }
    return TokenStrings[t];
}

//
// String Interning
//

static std::unordered_set<std::string> strtab;

const std::string* strintern(std::string s)
{
    auto result = strtab.emplace(std::move(s));
    auto iterator = result.first;
    return &*iterator;
}

void dump_strtab()
{
    printf("DUMPING STRING TABLE:\n");
    for (auto&& s : strtab) {
        printf("%s\n", s.c_str());
    }
    printf("END STRING TABLE.\n");
}

//
// Value -> Basic Type
//

u64 tou64(const void* v) { return reinterpret_cast<u64>(v); }

s64 totag(Value v) { return v & TAG_MSK; }

u64 isneg(Value v) { return (v & SIGN_BIT) != 0 ? ~u64(0) : u64(0); }

s64 tonum(Value v)
{
    assert(totag(v) == TAG_NUM);
    return (SIGN_MSK & isneg(v)) | (v >> TAG_BITS);
}

u64 tofun(Value v)
{
    assert(totag(v) == TAG_FUN);
    return v >> TAG_BITS;
}

const std::string& tostr(Value v)
{
    assert(totag(v) == TAG_STR);
    return *(std::string*)(v >> TAG_BITS);
}

const std::string& tosym(Value v)
{
    assert(totag(v) == TAG_SYM);
    return *(std::string*)(v >> TAG_BITS);
}

//
// Make Value
//

Value mknum(u64 v) { return (v << TAG_BITS) | TAG_NUM; }

Value mknil() { return TAG_NIL; }

Value mkfun(int f) { return (f << TAG_BITS) | TAG_FUN; }

Value mksym(const char* begin, const char* end)
{
    std::string s{begin, static_cast<size_t>(end - begin)};
    return mksym(std::move(s));
}

Value mksym(std::string s)
{
    return (tou64(strintern(s)) << TAG_BITS) | TAG_SYM;
}

Value mkstr(std::string s) { return (tou64(strintern(std::move(s))) << TAG_BITS) | TAG_STR; }

//
// Utilities
//

std::string valprint(Value v)
{
    std::string result;
    switch (totag(v)) {
        case TAG_NUM: return std::to_string(tonum(v));
        case TAG_NIL: return "nil";
        case TAG_FUN: // return funtostr(tofun(v));
            return "<function>";
        case TAG_SYM: return tosym(v);
        case TAG_STR:
            result = "\"";
            result += tostr(v);
            result += "\"";
            return result;
    }
    return "Invalid Value";
}

bool istrue(Value v)
{
    switch (totag(v))
    {
        case TAG_NUM: return tonum(v) != 0;
        case TAG_NIL: return false;
        case TAG_FUN: return true;
        case TAG_SYM: return true;
        case TAG_STR: return !tostr(v).empty();
    }
    assert(0 && "invalid tag");
    return false;
}
