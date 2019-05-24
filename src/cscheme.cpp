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

using s64 = int64_t;
using u64 = uint64_t;
using Value = u64;

// TODO: intern strings
std::unordered_set<std::string> strtab;
const std::string* strintern(std::string s)
{
    auto result = strtab.emplace(std::move(s));
    auto iterator = result.first;
    return &*iterator;
}
u64 tou64(const void* v) { return reinterpret_cast<u64>(v); }

void dump_strtab()
{
    printf("DUMPING STRING TABLE:\n");
    for (auto&& s : strtab) {
        printf("%s\n", s.c_str());
    }
    printf("END STRING TABLE.\n");
}

// TODO: make sure sign bit is saved
Value mknum(int64_t v) { return (v << TAG_BITS) | TAG_NUM; }
Value mknil() { return TAG_NIL; }
Value mkfun(int f) { return (f << TAG_BITS) | TAG_FUN; }
Value mksym(const char* s) { return (tou64(strintern(s)) << TAG_BITS) | TAG_SYM; }
Value mkstr(const char* s) { return (tou64(strintern(s)) << TAG_BITS) | TAG_STR; }

int64_t totag(Value v) { return v & TAG_MSK; }
int64_t tonum(Value v) { assert(totag(v) == TAG_NUM); return v >> TAG_BITS; }
const std::string& tostr(Value v) { assert(totag(v) == TAG_STR); return *(std::string*)(v >> TAG_BITS); }
const std::string& tosym(Value v) { assert(totag(v) == TAG_SYM); return *(std::string*)(v >> TAG_BITS); }

int main(int argc, char** argv)
{
    Value a = mkstr("Hello, World");
    Value b = mkstr("Hello, World");
    dump_strtab();
    printf("a == b? %s\n", a == b ? "TRUE" : "FALSE");

    printf("A's type: %s\n", tagtostr(totag(a)));
    printf("B's type: %s\n", tagtostr(totag(b)));

    printf("Value of a: '%s'\n", tostr(a).c_str());

    return 0;
}
