#pragma once
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cfloat>
#include <climits>
#include <string>
#include "value.h"

/*!max:re2c*/
static constexpr size_t SIZE = 8 * 1024;

struct Input {
    unsigned char buf[SIZE + YYMAXFILL];
    unsigned char *lim;
    unsigned char *cur;
    unsigned char *mar;
    unsigned char *tok;
    bool eof;

    FILE *const file;

    Input(FILE *f) noexcept
        : buf()
        , lim(buf + SIZE)
        , cur(lim)
        , mar(lim)
        , tok(lim)
        , eof(false)
        , file(f)
    {}

    bool fill(size_t need) noexcept
    {
        if (eof) {
            return false;
        }
        const size_t free = tok - buf;
        if (free < need) {
            return false;
        }
        memmove(buf, tok, lim - tok);
        lim -= free;
        cur -= free;
        mar -= free;
        tok -= free;
        lim += fread(lim, 1, free, file);
        if (lim < buf + SIZE) {
            eof = true;
            memset(lim, 0, YYMAXFILL);
            lim += YYMAXFILL;
        }
        return true;
    }
};

/*!re2c re2c:define:YYCTYPE = "uint8_t"; */

template<int base>
static bool adddgt(int64_t &u, int64_t d)
{
    if (u > (LONG_MAX - d) / base) {
        return false;
    }
    u = u * base + d;
    return true;
}

static bool lex_dec(const unsigned char *s, const unsigned char *e, int64_t &u)
{
    bool neg = false;
    if (*s == '-') {
        neg = true;
        ++s;
    }
    for (u = 0; s < e; ++s) {
        if (!adddgt<10>(u, *s - 0x30u)) {
            return false;
        }
    }
    if (neg) {
        u *= -1;
    }
    return true;
}

static bool lex_hex(const unsigned char *s, const unsigned char *e, int64_t &u)
{
    for (u = 0, s += 2; s < e;) {
    /*!re2c
        re2c:yyfill:enable = 0;
        re2c:define:YYCURSOR = s;
        *     { if (!adddgt<16>(u, s[-1] - 0x30u))      return false; continue; }
        [a-f] { if (!adddgt<16>(u, s[-1] - 0x61u + 10)) return false; continue; }
        [A-F] { if (!adddgt<16>(u, s[-1] - 0x41u + 10)) return false; continue; }
    */
    }
    return true;
}

static bool lex_str(Input &in, uint8_t q, Value& v) noexcept
{
    std::string result;
    for (int64_t u = q; ; result += u) {
        in.tok = in.cur;
        /*!re2c
            re2c:define:YYCURSOR = in.cur;
            re2c:define:YYMARKER = in.mar;
            re2c:define:YYLIMIT = in.lim;
            re2c:yyfill:enable = 1;
            re2c:define:YYFILL = "if (!in.fill(@@)) return false;";
            re2c:define:YYFILL:naked = 1;
            *                    { return false; }
            [^\n\\]              { u = in.tok[0]; if (u == q) break; continue; }
            "\\a"                { u = '\a'; continue; }
            "\\b"                { u = '\b'; continue; }
            "\\f"                { u = '\f'; continue; }
            "\\n"                { u = '\n'; continue; }
            "\\r"                { u = '\r'; continue; }
            "\\t"                { u = '\t'; continue; }
            "\\v"                { u = '\v'; continue; }
            "\\\\"               { u = '\\'; continue; }
            "\\'"                { u = '\''; continue; }
            "\\\""               { u = '"';  continue; }
            "\\?"                { u = '?';  continue; }
            "\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
            "\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
            "\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }
        */
    }
    v = mkstr(result);
    return true;
}

Token lex(Input& in, Value& v) noexcept
{
    int64_t ival;
    for (;;) {
        in.tok = in.cur;
        /*!re2c
            re2c:define:YYCURSOR = in.cur;
            re2c:define:YYMARKER = in.mar;
            re2c:define:YYLIMIT = in.lim;
            re2c:yyfill:enable = 1;
            re2c:define:YYFILL = "if (!in.fill(@@)) return T_ERROR;";
            re2c:define:YYFILL:naked = 1;

            end = "\x00";
            *   { v = mkstr("error parsing input"); return T_ERROR; }
            end
            {
                if (in.lim - in.tok != YYMAXFILL) {
                    v = mkstr("error parsing input");
                    return T_ERROR;
                }
                return T_EOF;
            }

            // whitespaces
            scm = ";" [^\n]* "\n";
            [ \t\v]+ { continue; }
            newline = [\n\r] | scm;
            newline { continue; }

            // TODO: simplify this and do string interpolation of escaped characters later?
            // string literals
            "\""
            {
                if (!lex_str(in, in.cur[-1], v)) {
                    v = mkstr("invalid string literal");
                    return T_ERROR;
                }
                return T_STR;
            }

            // integer literals
            dec = [-]? [1-9][0-9]*;
            hex = '0x' [0-9a-fA-F]+;
            dec
            {
                if (!lex_dec(in.tok, in.cur, ival)) {
                    v = mkstr("invalid number");
                    return T_ERROR;
                }
                v = mknum(ival);
                return T_NUM;
            }
            hex
            {
                if (!lex_hex(in.tok, in.cur, ival)) {
                    v = mkstr("invalid hex number");
                    return T_ERROR;
                }
                v = mknum(ival);
                return T_NUM;
            }

            // boolean literals
            "#f" { v = mknum(0); return T_NUM; }
            "#t" { v = mknum(1); return T_NUM; }

            // operators
            "("   { return T_LPAREN; }
            ")"   { return T_RPAREN; }
            "+"   { v = mkfun(F_PLUS);     return F_PLUS; }
            "-"   { v = mkfun(F_MINUS);    return F_MINUS; }
            "*"   { v = mkfun(F_MULTIPLY); return F_MULTIPLY; }
            "/"   { v = mkfun(F_DIVIDE);   return F_DIVIDE; }
            ">"   { v = mkfun(F_GT);  return F_GT; }
            "<"   { v = mkfun(F_LT);  return F_LT; }
            ">="  { v = mkfun(F_GTE); return F_GTE; }
            "<="  { v = mkfun(F_LTE); return F_LTE; }
            "\."  { return T_DOT; }
            "'"   { return F_QUOTE; }

            // keywords
            "if"     { v = mkfun(F_IF); return F_IF; }
            "define" { v = mkfun(F_DEFINE); return F_DEFINE; }
            "nil"    { v = mknil(); return T_NIL; }

            // symbols
            id = [a-zA-Z_][a-zA-Z_0-9]*;
            id
            {
                std::string symbol{(const char*)in.tok, (size_t)(in.cur - in.tok)};
                v = mksym(std::move(symbol));
                return T_SYM;
            }
        */
    }

    return T_ERROR;
}
