#pragma once

#include <cstdint>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>

enum Kind {
    LV_INT    = 0x1u,
    LV_NIL    = 0x2u,
    LV_TRUE   = 0x3u,
    LV_FALSE  = 0x4u,
    LV_STR    = 0x5u,
    LV_TAB    = 0x6u,
    LV_FUN    = 0x7u,
    LV_THREAD = 0x8u,
    LV_LUDATA = 0x9u,
    LV_UDATA  = 0xau,

    LV_NTYPES = LV_UDATA,
    LV_NBITS = 4,
};
static_assert(LV_NTYPES < (1u << LV_NBITS), "Types won't fit in tag bits");

static_assert(sizeof(double)    == 8, "only 64-bit doubles supported");
static_assert(sizeof(uintptr_t) == 8, "only 64-bit pointers supported");

constexpr uint64_t LV_DBLVAL = 0xfff8000000000000ull;
constexpr uint64_t INDEXMASK = 0x00007fffffffffffull;

//----------------------------------------------------------
// Value representation:
//        |--------+------------+-------------------+
//        | <sign> | <exponent> |     <fraction>    |
//        |--------+------------+-------------------+
//        | 1-bit  |   11-bits  |      52-bits      |
// -------|--------+------------+-------------------+
// size:  |    1   |     11     | 1 |  4  |   47    |
// -------|--------+------------+-------------------+
// value: |    1   |    1..1    | 1 | tag | data    |
// -------|--------+------------+-------------------+
//----------------------------------------------------------

struct Value
{
    union {
        uint64_t uval;
        double   dval;
        // XXX: only works on little endian platform
        struct {
            uint32_t lo;
            uint32_t hi;
        } b;
    };
};

// TODO: add string interning
// TODO: integrate with GC
struct String
{
    static constexpr size_t MaxSmallSize = 28;
    uint32_t len;
    union {
        char* ptr;
        char  str[MaxSmallSize];
    };
};
std::vector<String*> strtab;
std::vector<size_t>  strtabidx;

const char* str2cstr(const String& s) {
    if (s.len < String::MaxSmallSize) {
        return &s.str[0];
    } else {
        return s.ptr;
    }
}

uint32_t mktag(uint32_t tag)
{
    assert((tag < (1u << LV_NBITS)) && "invalid tag");
    return 0xfff80000u | (tag << 15);
}

Value mkdouble(double x)
{
    assert((x == x) || *reinterpret_cast<uint64_t*>(&x) == 0xfff8000000000000ull);
    Value v;
    v.dval = x;
    return v;
}

Value mkint(int x)
{
    Value v;
    v.b.hi = mktag(LV_INT);
    v.b.lo = static_cast<uint32_t>(x);
    return v;
}

Value mknil()
{
    Value v;
    v.b.hi = mktag(LV_NIL);
    v.b.lo = 0u;
    return v;
}

Value mktrue()
{
    Value v;
    v.b.hi = mktag(LV_TRUE);
    v.b.lo = 0u;
    return v;
}


Value mkfalse()
{
    Value v;
    v.b.hi = mktag(LV_FALSE);
    v.b.lo = 0u;
    return v;
}

Value mkstr(const char* str, size_t len)
{
    // TODO: GC needs to know about this allocation
    String* s = (String*) malloc(sizeof(*s));
    s->len = len;
    if (len < String::MaxSmallSize) {
        memcpy(&s->str[0], str, len);
        s->str[len] = '\0';
    } else {
        s->ptr = (char*) malloc(len + 1);
        memcpy(s->ptr, str, len);
        s->ptr[len] = '\0';
    }

    size_t index;
    if (strtabidx.empty()) {
        index = strtab.size();
        strtab.push_back(s);
    } else {
        index = strtabidx.back();
        strtabidx.pop_back();
        assert(index < strtab.size() && strtab[index] == nullptr);
        strtab[index] = s;
    }

    Value v;
    v.b.hi = mktag(LV_STR);
    v.uval |= index;
    return v;
}

Value mkstr(const char* str) { return mkstr(str, strlen(str)); }

uint32_t totag(Value v) { return (v.b.hi >> 15) & 0x00fu; }
bool isint(Value v) { return totag(v) == LV_INT; }
bool isdouble(Value v) { return v.uval <= LV_DBLVAL; }
bool isnil(Value v) { return totag(v) == LV_NIL; }
bool istrue(Value v) { return totag(v) == LV_TRUE; }
bool isfalse(Value v) { return totag(v) == LV_FALSE; }
bool isstr(Value v) { return totag(v) == LV_STR; }

int unsafe_toint(Value v) { assert(isint(v)); return v.b.lo; }
double unsafe_todouble(Value v) { assert(isdouble(v)); return v.dval; }
String* unsafe_tostr(Value v) { assert(isstr(v)); return strtab[v.uval & INDEXMASK]; }
