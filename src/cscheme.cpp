#include <cstdio>
#include <cstdlib>
#include "value.h"

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
    // Value e = mkfun(F_PLUS);
    Value f = mknum(-55);
    Value g = mknum(-100000000111ll);
    Value h = mknum( 100000000111ll);

    printf("valprint(a): %s\n", valprint(a).c_str());
    printf("valprint(b): %s\n", valprint(b).c_str());
    printf("valprint(c): %s\n", valprint(c).c_str());
    printf("valprint(d): %s\n", valprint(d).c_str());
    // printf("valprint(e): %s\n", valprint(e).c_str());
    printf("valprint(f): %s\n", valprint(f).c_str());
    printf("valprint(g): %s\n", valprint(g).c_str());
    printf("valprint(h): %s\n", valprint(h).c_str());

    return 0;
}
