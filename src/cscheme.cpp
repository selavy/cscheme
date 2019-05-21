#include <cstdio>
#include "lex.cpp"

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

    return 0;
}
