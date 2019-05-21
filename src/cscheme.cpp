#include <cstdio>
#include <cassert>
#include "lex.cpp"
#include "value.h"

int main(int argc, char** argv)
{
    // printf("$ cscheme> \n");

    if (argc != 2) {
        printf ("usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("error: cannot open file: %s\n", argv[1]);
        return 1;
    }

    cscheme::lex::Input in(file);
    if (!cscheme::lex::lex(in)) {
        printf("... error\n");
    } else {
        printf("\n");
    }

    fclose(file);

    //
    // Unit Tests
    //

    {
        auto v = mkvoid();
        assert(v.kind == Kind::VOID);
    }

    {
        auto n = mknumber(42);
        assert(n.kind == Kind::NUMBER);
        assert(n.num  == 42.0);
        assert(n.str.empty());
        assert(istruthy(n) == true);
        n = mknumber(0);
        assert(istruthy(n) == false);
    }

    {
        auto b = mkbool(true);
        assert(istruthy(b) == true);
        b = mkbool(false);
        assert(istruthy(b) == false);
    }

    {
        auto s = mkstring("Hello, World");
        assert(s.str == "Hello, World");
        assert(istruthy(s) == true);
        s = mkstring("");
        assert(s.str.empty());
        assert(istruthy(s) == false);
    }

    {
        auto c = mkchar('c');
        assert(c.ch == 'c');
        assert(istruthy(c) == true);
    }

    {
        auto v = mknumber(42);
        auto p = cons(&v, &NIL);
        assert(p.kind == Kind::PAIR);
        assert(istruthy(p) == true);
    }

    printf("PASSED!\n");

    return 0;
}
