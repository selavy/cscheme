#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include "lex.h"
#include "value.h"

Token t = F_DEFINE;
Value v = mknil();
Value p = mknil();
Input* in = nullptr;

bool finished() { return t == T_EOF || t == T_ERROR; }

bool next()
{
    if (finished()) {
        return false;
    }
    p = v;
    t = lex(*in, v);
    return !finished();
}

bool match(int tok)
{
    if (t == tok) {
        next();
        return true;
    }
    return false;
}

bool matchv(int count, ...)
{
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        if (match(va_arg(args, int))) {
            return true;
        }
    }
    return false;
}

void expect(int tok)
{
    if (!match(tok)) {
        // TODO: use setjmp / longjmp instead
        throw std::runtime_error("expected different token");
    }
}

int sp = 0;
Value stack[4096];
void push(Value v)
{
    // TEMP TEMP
    printf("PUSH(%s)\n", vprint(v));
    stack[sp++] = v;
}
Value pop() { assert(sp > 0); --sp; return stack[sp]; }
void popn(int n) { assert(sp >= n); sp -= n; }
Value peek() { assert(sp > 0); return stack[sp-1]; }
Value sget(int n) { assert(sp > n); return stack[sp-n-1]; }
void dumpstk()
{
    printf("DUMPING STACK:\n");
    for (int i = 0; i < sp; ++i) {
        printf("%d: %s\n", i, vprint(stack[i]));
    }
    printf("END STACK\n");
}

enum { OK, DONE, ERROR };

int sexpr();

bool allnumeric(int nargs)
{
    while (nargs-- > 0) {
        if (!isnum(sget(nargs))) {
            return false;
        }
    }
    return true;
}

int b_define(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_if(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_plus(int nargs)
{
    if (!allnumeric(nargs)) {
        popn(nargs + 1);
        push(mkstr("invalid argument to #<function:+>: non-numeric argument"));
        return ERROR;
    }
    s64 result = 0;
    for (int i = 0; i < nargs; ++i) {
        result += tonum(pop());
    }
    push(mknum(result));
    return OK;
}

int b_minus(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_multiply(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_divide(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_gt(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_lt(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_gte(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_lte(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_quote(int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int eval()
{
    if (match(T_EOF)) {
        return DONE;
    } else if (match(T_ERROR)) {
        push(p);
        return ERROR;
    } else if (matchv(4, T_SYM, T_NUM, T_STR, T_NIL)) {
        push(p);
        return OK;
    } else if (matchv(11, F_DEFINE, F_IF, F_PLUS, F_MINUS,
                F_MULTIPLY, F_DIVIDE, F_GT, F_LT, F_GTE,
                F_LTE, F_QUOTE)) {
        push(p);
        return OK;
    } else if (match(T_LPAREN)) {
        return sexpr();
    } else {
        printf("error: invalid token: %s\n", toktostr(t)); // TEMP TEMP
        push(mkstr("error: invalid token"));
        return ERROR;
    }
}

int evalfn(int nargs)
{
    assert(nargs > 0);

    Value f = sget(nargs - 1);
    nargs--;

    if (isfun(f)) {
        switch(tofun(f)) {
            case F_DEFINE:   return b_define(nargs);
            // TODO: this needs to be special cased
            case F_IF:       return b_if(nargs);
            case F_PLUS:     return b_plus(nargs);
            case F_MINUS:    return b_minus(nargs);
            case F_MULTIPLY: return b_multiply(nargs);
            case F_DIVIDE:   return b_divide(nargs);
            case F_GT:       return b_gt(nargs);
            case F_LT:       return b_lt(nargs);
            case F_GTE:      return b_gte(nargs);
            case F_LTE:      return b_lte(nargs);
            case F_QUOTE:    return b_quote(nargs);
        }
        push(mkstr("invalid function"));
        return ERROR;
    } else if (issym(f)) {
        push(mkstr("unimplemented: symbols not supported yet"));
        return ERROR;
    } else {
        push(mkstr("invalid s-expression"));
        return ERROR;
    }
}

int sexpr()
{
    int nargs = 0;
    while (!match(T_RPAREN)) {
        if (matchv(2, T_EOF, T_ERROR)) {
            push(mkstr("missing closing ')' for s-expression"));
            return ERROR;
        }
        if (eval() != OK) {
            return ERROR;
        }
        ++nargs;
    }

    if (nargs == 0) {
        push(mkstr("invalid s-expression, no arguments"));
        return ERROR;
    }

    return evalfn(nargs);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <FILE>\n", argv[0]);
        exit(1);
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    Input input(fp);
    in = &input;
    next();
    for (;;) {
        auto ok = eval();
        dumpstk();
        if (ok == DONE) {
            break;
        } else if (ok == ERROR) {
            fprintf(stderr, "error: %s\n", vprint(peek()));
            break;
        }
        printf("result: %s\n", vprint(peek()));
    }
    fclose(fp);

    printf("Bye.\n");
    return 0;
}
