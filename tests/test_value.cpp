#include <catch2/catch.hpp>
#include "value.h"

TEST_CASE("Value: double", "[value]")
{
    double d = 42.0;
    Value v = mkdouble(d);
    REQUIRE(isdouble(v));
    REQUIRE(!isnil(v));
    REQUIRE(!isint(v));
    REQUIRE(!istrue(v));
    REQUIRE(!isfalse(v));

    REQUIRE(unsafe_todouble(v) == d);
}

TEST_CASE("Value: int", "[value]")
{
    int i = 4;
    Value v = mkint(i);
    REQUIRE(isint(v));
    REQUIRE(!isdouble(v));
    REQUIRE(!isnil(v));
    REQUIRE(!istrue(v));
    REQUIRE(!isfalse(v));

    REQUIRE(unsafe_toint(v) == i);
}

TEST_CASE("Value: nil", "[value]")
{
    Value v = mknil();
    REQUIRE(isnil(v));
    REQUIRE(!isdouble(v));
    REQUIRE(!isint(v));
    REQUIRE(!istrue(v));
    REQUIRE(!isfalse(v));
}

TEST_CASE("Value: true", "[value]")
{
    Value v = mktrue();
    REQUIRE(istrue(v));
    REQUIRE(!isnil(v));
    REQUIRE(!isdouble(v));
    REQUIRE(!isint(v));
    REQUIRE(!isfalse(v));
}

TEST_CASE("Value: false", "[value]")
{
    Value v = mkfalse();
    REQUIRE(isfalse(v));
    REQUIRE(!isnil(v));
    REQUIRE(!isdouble(v));
    REQUIRE(!isint(v));
    REQUIRE(!istrue(v));
}

TEST_CASE("String", "[string]")
{
    const char* str = "Hello, World";
    Value v = mkstr(str);
    REQUIRE(isstr(v));
    String* s = unsafe_tostr(v);
    REQUIRE(s != nullptr);
    REQUIRE(std::string(str2cstr(*s)) == std::string(str));

    REQUIRE(!isnil(v));
    REQUIRE(!isfalse(v));
    REQUIRE(!istrue(v));
    REQUIRE(!isdouble(v));
    REQUIRE(!isint(v));
}
