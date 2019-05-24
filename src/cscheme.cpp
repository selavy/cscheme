#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <unordered_set>
#include <memory>

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

const char* const TagStr[] = {
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

enum
{
    // function keywords
    F_DEFINE, F_IF, F_PLUS, F_MINUS,

    // tokens
    T_DOT, T_LPAREN, T_RPAREN, T_SYM, T_NUM, T_QUOTE, Q_DQUOTE,
};

const char* FunctionNames[] = {
    "#<function:define>",
    "#<function:if>",
    "#<function:plus>",
    "#<function:minus>",
};
const char* funtostr(size_t f) {
    if (f > F_MINUS) {
        return "INVALID FUNCTION";
    }
    return FunctionNames[f];
}

using s64 = int64_t;
using u64 = uint64_t;
using Value = u64;

std::unordered_set<std::string> strtab;
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

u64 tou64(const void* v) { return reinterpret_cast<u64>(v); }
s64 totag(Value v) { return v & TAG_MSK; }
u64 isneg(Value v) { return (v & SIGN_BIT) != 0 ? ~u64(0) : u64(0); }
s64 tonum(Value v) { assert(totag(v) == TAG_NUM); return (SIGN_MSK & isneg(v)) | (v >> TAG_BITS); }
u64 tofun(Value v) { assert(totag(v) == TAG_FUN); return v >> TAG_BITS; }
const std::string& tostr(Value v) { assert(totag(v) == TAG_STR); return *(std::string*)(v >> TAG_BITS); }
const std::string& tosym(Value v) { assert(totag(v) == TAG_SYM); return *(std::string*)(v >> TAG_BITS); }
Value mknum(u64 v) { return (v << TAG_BITS) | TAG_NUM; }
Value mknil() { return TAG_NIL; }
Value mkfun(int f) { return (f << TAG_BITS) | TAG_FUN; }
Value mksym(const char* s) { return (tou64(strintern(s)) << TAG_BITS) | TAG_SYM; }
Value mkstr(const char* s) { return (tou64(strintern(s)) << TAG_BITS) | TAG_STR; }

std::string valprint(Value v)
{
    std::string result;
    switch (totag(v)) {
        case TAG_NUM: return std::to_string(tonum(v));
        case TAG_NIL: return "nil";
        case TAG_FUN: return funtostr(tofun(v));
        case TAG_SYM: return tosym(v);
        case TAG_STR:
            result = "\"";
            result += tostr(v);
            result += "\"";
            return result;
    }
    return "Invalid Value";
}

int main(int argc, char** argv)
{
    Value a = mkstr("Hello, World");
    Value b = mkstr("Hello, World");
    Value c = mkstr("A different string");
    dump_strtab();
    printf("a == b? %s\n", a == b ? "TRUE" : "FALSE");
    printf("a == c? %s\n", a == c ? "TRUE" : "FALSE");

    printf("A's type: %s\n", tagtostr(totag(a)));
    printf("B's type: %s\n", tagtostr(totag(b)));
    printf("C's type: %s\n", tagtostr(totag(b)));

    printf("Value of a: '%s'\n", tostr(a).c_str());
    printf("Value of b: '%s'\n", tostr(b).c_str());
    printf("Value of c: '%s'\n", tostr(c).c_str());

    Value d = mknum(42);
    Value e = mkfun(F_PLUS);
    Value f = mknum(-55);
    Value g = mknum(-100000000111ll);
    Value h = mknum( 100000000111ll);

    printf("valprint(a): %s\n", valprint(a).c_str());
    printf("valprint(b): %s\n", valprint(b).c_str());
    printf("valprint(c): %s\n", valprint(c).c_str());
    printf("valprint(d): %s\n", valprint(d).c_str());
    printf("valprint(e): %s\n", valprint(e).c_str());
    printf("valprint(f): %s\n", valprint(f).c_str());
    printf("valprint(g): %s\n", valprint(g).c_str());
    printf("valprint(h): %s\n", valprint(h).c_str());

    return 0;
}
