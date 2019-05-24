#include <cstdio>
#include <cstdlib>
#include "lex.h"
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

    if (argc != 2) {
        fprintf(stderr, "usage: %s <FILE>\n", argv[0]);
        exit(1);
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    Input in(fp);

    Token t;
    Value v;
    for (;;) {
        t = lex(in, v);
        if (t == T_ERROR) {
            fprintf(stderr, "error: %s", tostr(v).c_str());
            break;
        } else if (t == T_EOF) {
            break;
        }
        printf("Token: %s, Value: %s\n", toktostr(t), valprint(v).c_str());
    }

    fclose(fp);
    printf("Bye.\n");

    return 0;
}
