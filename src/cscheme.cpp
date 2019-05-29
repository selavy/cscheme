#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "value.h"

void printval(double v)
{
    uint8_t p[8];
    memcpy(&p, &v, sizeof(p));
    for (size_t i = 0; i < 8; ++i) {
        printf("0x%02X ", p[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    printf("cscheme>\n");

    double v = std::nan("");
    printval(v);

    double x = 100.0 * v;
    printval(x);

    printval(NAN);

    printval(0. / 0.);

    return 0;
}
