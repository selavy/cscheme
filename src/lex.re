#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cfloat>
#include <climits>

namespace cscheme { namespace lex {

/*!max:re2c*/
// static constexpr size_t SIZE = 64 * 1024;
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
static bool adddgt(uint64_t &u, uint64_t d)
{
    if (u > (ULONG_MAX - d) / base) {
        return false;
    }
    u = u * base + d;
    return true;
}

static bool lex_dec(const unsigned char *s, const unsigned char *e, uint64_t &u)
{
    auto* tok = s;
    for (u = 0; s < e; ++s) {
        if (!adddgt<10>(u, *s - 0x30u)) {
            // TODO: error handling
            fprintf(stderr, "lexer error: decimal overflow: '%.*s'\n", static_cast<int>(e - tok), (const char*)tok);
            return false;
        }
    }
    return true;
}

static bool lex_hex(const unsigned char *s, const unsigned char *e, uint64_t &u)
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

static bool lex_flt(const unsigned char *s)
{
    double d = 0;
    double x = 1;
    int e = 0;
    /*!re2c
        re2c:yyfill:enable = 0;
        re2c:define:YYCURSOR = s;
    */
mant_int:
    /*!re2c
        "."   { goto mant_frac; }
        [eE]  { goto exp_sign; }
        *     { d = (d * 10) + (s[-1] - '0'); goto mant_int; }
    */
mant_frac:
    /*!re2c
        ""    { goto sfx; }
        [eE]  { goto exp_sign; }
        [0-9] { d += (x /= 10) * (s[-1] - '0'); goto mant_frac; }
    */
exp_sign:
    /*!re2c
        "+"?  { x = 1e+1; goto exp; }
        "-"   { x = 1e-1; goto exp; }
    */
exp:
    /*!re2c
        ""    { for (; e > 0; --e) d *= x;    goto sfx; }
        [0-9] { e = (e * 10) + (s[-1] - '0'); goto exp; }
    */
sfx:
    /*!re2c
        *     { goto end; }
        [fF]
        {
            if (d > FLT_MAX) {
                // TODO: error handling
                fprintf(stderr, "lexer error: number overflow\n");
                return false;
            }
            goto end;
        }
    */
end:
    printf("NUMBER: %f\n", d);
    return true;
}

bool lex(Input &in) noexcept
{
    uint64_t ival;
    for (;;) {
        in.tok = in.cur;
        /*!re2c
            re2c:define:YYCURSOR = in.cur;
            re2c:define:YYMARKER = in.mar;
            re2c:define:YYLIMIT = in.lim;
            re2c:yyfill:enable = 1;
            re2c:define:YYFILL = "if (!in.fill(@@)) return false;";
            re2c:define:YYFILL:naked = 1;

            end = "\x00";
            *   { return false; }
            end { return in.lim - in.tok == YYMAXFILL; }

            // whitespaces
            mcm = "/*" ([^*] | ("*" [^/]))* "*""/";
            scm = "//" [^\n]* "\n";
            wsp = ([ \t\v\n\r] | scm | mcm)+;
            wsp { printf("WHITESPACE\n"); continue; }

            // integer literals
            dec = [1-9][0-9]*;
            hex = '0x' [0-9a-fA-F]+;
            dec
            {
                if (!lex_dec(in.tok, in.cur, ival)) {
                    return false;
                }
                printf("DECIMAL INTEGER: %lu\n", ival);
				continue;
            }
            hex
            {
                if (!lex_hex(in.tok, in.cur, ival)) {
                    return false;
                }
                printf("HEX INTEGER: %lu\n", ival);
				continue;
            }

            // floating literals
            frc = [0-9]* "." [0-9]+ | [0-9]+ ".";
            exp = 'e' [+-]? [0-9]+;
            flt = (frc exp? | [0-9]+ exp) [fFlL]?;
            flt
            {
                if (!lex_flt(in.tok)) {
                    fprintf(stderr, "lexer error: failed to parse float\n");
                    return false;
                }
                continue;
            }

            // boolean literals
            "#f" { printf("BOOLEAN: #f\n"); continue; }
            "#t" { printf("BOOLEAN: #t\n"); continue; }

            // operators
            "("  { printf("LPAREN\n"); continue; }
            ")"  { printf("RPAREN\n"); continue; }
            "+"  { printf("PLUS\n"); continue; }
            "-"  { printf("MINUS\n"); continue; }
            "*"  { printf("MULTIPLY\n"); continue; }
            "/"  { printf("DIVIDE\n"); continue; }
            ">"  { printf("GT\n"); continue; }
            "<"  { printf("LT\n"); continue; }
            ">=" { printf("GTE\n"); continue; }
            "<=" { printf("LTE\n"); continue; }

            // keywords
            "if"     { printf("IF\n"); continue; }
            "cond"   { printf("COND\n"); continue; }
            "define" { printf("DEFINE\n"); continue; }

            // identifiers
            id = [a-zA-Z_][a-zA-Z_0-9]*;
            id { printf("IDENT: '%.*s'\n", static_cast<int>(in.cur - in.tok), in.tok); continue; }
        */
    }
}

} /*lex*/ } /*cscheme*/
