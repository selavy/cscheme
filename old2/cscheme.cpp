#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include <map>
#include "lex.h"
#include "value.h"

struct Env {
    std::map<Value, Value> e;
    Env* parent = nullptr;
};

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

int ap = 0;
Value argstk[128];
void pusharg(Value v) { argstk[ap++] = v; }
Value poparg() { assert(ap > 0); --ap; return argstk[ap]; }
Value aget(int n) { assert(ap > n); return argstk[ap-n-1]; }
void aset(int n, Value v) { assert(ap > n); argstk[ap-n-1] = v; }
void clearargs() { ap = 0; }

int sp = 0;
Value stack[4096];
void push(Value v) { /* printf("PUSH(%s)\n", vprint(v)); */ /*TEMP TEMP*/ stack[sp++] = v; }
void pusherr(const char* s) { push(mkstr(s)); }
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

int sexpr(Env& env);

bool allnumeric(int nargs)
{
    for (int i = 0; i < nargs; ++i) {
        if (!isnum(aget(i))) {
            return false;
        }
    }
    return true;
}

int b_define(Env& env, int nargs)
{
    if (nargs != 2) {
        push(mkstr("incorrect number of arguments, expected 2"));
        return ERROR;
    }
    Value sym = poparg();
    if (!issym(sym)) {
        push(mkstr("can only bind values to symbols"));
        return  ERROR;
    }
    env.e[sym] = poparg();
    push(mknil());
    return OK;
}

int b_if(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_plus(Env& env, int nargs)
{
    if (!allnumeric(nargs)) {
        push(mkstr("invalid argument to #<function:+>: non-numeric argument"));
        return ERROR;
    }
    s64 result = 0;
    for (int i = 0; i < nargs; ++i) {
        result += tonum(aget(i));
    }
    push(mknum(result));
    return OK;
}

int b_minus(Env& env, int nargs)
{
    if (!allnumeric(nargs)) {
        push(mkstr("invalid argument to #<function:+>: non-numeric argument"));
        return ERROR;
    }
    s64 result;
    if (nargs == 0) {
        result = 0;
    } else if (nargs == 1) {
        result = -1 * tonum(aget(0));
    } else {
        result = tonum(aget(0));
        for (int i = 1; i < nargs; ++i) {
            result -= tonum(aget(i));
        }
    }
    push(mknum(result));
    return OK;
}

int b_multiply(Env& env, int nargs)
{
    if (!allnumeric(nargs)) {
        push(mkstr("invalid argument to #<function:*>: non-numeric argument"));
        return ERROR;
    }
    s64 result = 1;
    for (int i = 0; i < nargs; ++i) {
        result *= tonum(aget(i));
    }
    push(mknum(result));
    return OK;
}

int b_divide(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_gt(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_lt(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_gte(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_lte(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int b_quote(Env& env, int nargs)
{
    push(mkstr("unimplemented error"));
    return ERROR;
}

int eval(Env& env)
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
        printf("Function token: %s\n", vprint(p));
        push(p);
        return OK;
    } else if (match(T_LPAREN)) {
        return sexpr(env);
    } else {
        printf("error: invalid token: %s\n", toktostr(t)); // TEMP TEMP
        push(mkstr("error: invalid token"));
        return ERROR;
    }
}

bool isspecial(Value f)
{
    if (!isfun(f)) {
        return false;
    }
    int fn = tofun(f);
    return fn == F_IF || fn == F_DEFINE;
}

bool lookup(const Env& env_, Value& v) {
    if (!issym(v)) {
        return false;
    }
    const Env* env = &env_;
    while (env) {
        auto found = env->e.find(v);
        if (found != env->e.end()) {
            v = found->second;
            return true;
        }
        env = env->parent;
    }
    return false;
}

int evalfn(Env& env, int nargs)
{
    assert(nargs > 0);

    --nargs;
    for (int i = 0; i < nargs; ++i) {
        pusharg(pop());
    }
    Value f = pop();

    // XXX: this is awkward for `if`, `define`, `cond`, etc
    if (!isspecial(f)) {
        for (int i = 0; i < nargs; ++i) {
            Value val = aget(i);
            if (issym(val)) {
                if (!lookup(env, val)) {
                    pusherr("unable to find symbol");
                    return ERROR;
                }
                aset(i, val);
            }
        }
    }

    if (issym(f)) {
        if (!lookup(env, f)) {
            pusherr("unable to find symbol");
            return ERROR;
        }
    }

    if (isfun(f)) {
        switch(tofun(f)) {
            case F_DEFINE:   return b_define(env, nargs);
            // TODO: this needs to be special cased
            case F_IF:       return b_if(env, nargs);
            case F_PLUS:     return b_plus(env, nargs);
            case F_MINUS:    return b_minus(env, nargs);
            case F_MULTIPLY: return b_multiply(env, nargs);
            case F_DIVIDE:   return b_divide(env, nargs);
            case F_GT:       return b_gt(env, nargs);
            case F_LT:       return b_lt(env, nargs);
            case F_GTE:      return b_gte(env, nargs);
            case F_LTE:      return b_lte(env, nargs);
            case F_QUOTE:    return b_quote(env, nargs);
        }
        push(mkstr("invalid function"));
        return ERROR;
    } else {
        push(mkstr("evalfn: invalid s-expression"));
        return ERROR;
    }
}

int sexpr(Env& env)
{
    int nargs = 0;
    while (!match(T_RPAREN)) {
        if (matchv(2, T_EOF, T_ERROR)) {
            push(mkstr("missing closing ')' for s-expression"));
            return ERROR;
        }
        if (eval(env) != OK) {
            return ERROR;
        }
        ++nargs;
    }

    if (nargs == 0) {
        push(mkstr("sexpr: invalid s-expression, no arguments"));
        return ERROR;
    }

    return evalfn(env, nargs);
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

    Env env;
    env.e = {
        { mksym("define"), mkfun(F_DEFINE)   },
        { mksym("+"),      mkfun(F_PLUS)     },
        { mksym("-"),      mkfun(F_MINUS)    },
        { mksym("*"),      mkfun(F_MULTIPLY) },
        { mksym("/"),      mkfun(F_DIVIDE)   },
        { mksym(">"),      mkfun(F_GT)       },
        { mksym("<"),      mkfun(F_LT)       },
        { mksym(">="),     mkfun(F_GTE)      },
        { mksym("<="),     mkfun(F_LTE)      },
        { mksym("quote"),  mkfun(F_QUOTE)    },
    };
    Input input(fp);
    in = &input;
    next();
    for (;;) {
        auto ok = eval(env);
        // dumpstk();
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
