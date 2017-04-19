#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include "php2json.h"


#ifdef DEBUG
#define debug(_fmt, ...) fprintf(stderr, (_fmt "\n"), ##__VA_ARGS__)
#else
#define debug(_fmt, ...)
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)


struct parser {
    char *src;
    char *dst;
    jmp_buf jump;
    char *err;
    int err_len;
};


static void parse_simple_value(struct parser *p);
static void parse_value(struct parser *p);


static void raise_err(struct parser *p, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(p->err, p->err_len, fmt, ap);
    va_end(ap);

    longjmp(p->jump, 1);
}


static void expect(struct parser *p, char c)
{
    char cc = *(p->src++);
    if (unlikely(cc != c))
        raise_err(p, "expected '%c', got '%c'", c, cc);
}


static void copy_until(struct parser *p, char c)
{
    char *x = strchr(p->src, c);
    if (unlikely(!x))
        raise_err(p, "EOF");

    int len = x - p->src;
    strncpy(p->dst, p->src, len);
    p->src += len + 1;  /* skip "c" too */
    p->dst += len;
}


static void emit_c(struct parser *p, char c)
{
    *p->dst = c;
    p->dst++;
}


static void emit_s(struct parser *p, char *s, int s_len)
{
    strcpy(p->dst, s);
    p->dst += s_len;
}


static int parse_length(struct parser *p)
{
    int val = 0;

    if (unlikely(*p->src == ':'))
        raise_err(p, "invalid number when parsing length");
    for (; *p->src != ':'; p->src++) {
        if (unlikely(!isdigit(*p->src)))
            raise_err(p, "invalid number when parsing length");
        val = val * 10 + (*p->src - '0');
    }

    return val;
}


static void parse_int(struct parser *p)
{
    ++p->src;
    expect(p, ':');
    copy_until(p, ';');
}


static void parse_null(struct parser *p)
{
    ++p->src;
    expect(p, ';');
    emit_s(p, "null", 4);
}


static void parse_bool(struct parser *p)
{
    ++p->src;
    expect(p, ':');
    char val = *p->src++;
    switch (val) {
    case '0':
        emit_s(p, "false", 5);
        break;
    case '1':
        emit_s(p, "true", 4);
        break;
    default:
        raise_err(p, "invalid bool value: '%c'", val);
    }
    expect(p, ';');
}


static void parse_string(struct parser *p)
{
    ++p->src;
    expect(p, ':');

    int len = parse_length(p);

    expect(p, ':');
    expect(p, '"');

    emit_c(p, '"');

    for (; len > 0; len--, p->src++) {
        switch (*p->src) {
        case '\0':
            raise_err(p, "EOF");
        case '\b':
            emit_c(p, '\\');
            emit_c(p, 'b');
            break;
        case '\f':
            emit_c(p, '\\');
            emit_c(p, 'f');
            break;
        case '\n':
            emit_c(p, '\\');
            emit_c(p, 'n');
            break;
        case '\r':
            emit_c(p, '\\');
            emit_c(p, 'r');
            break;
        case '\t':
            emit_c(p, '\\');
            emit_c(p, 't');
            break;
        case '\\':
        case '"':
            emit_c(p, '\\');
            emit_c(p, *p->src);
            break;
        default:
            emit_c(p, *p->src);
            break;
        }
    }

    expect(p, '"');
    emit_c(p, '"');
    expect(p, ';');
}


static void parse_array(struct parser *p)
{
    ++p->src;
    expect(p, ':');

    int len = parse_length(p);

    expect(p, ':');
    expect(p, '{');

    emit_c(p, '{');

    for (int i = 0; i < len; i++) {
        if (*p->src != 's') {
            /* stringify non-string values */
            emit_c(p, '"');
            parse_simple_value(p);
            emit_c(p, '"');
        } else
            parse_string(p);
        emit_c(p, ':');
        parse_value(p);
        if (i != len - 1)
            emit_c(p, ',');
    }

    emit_c(p, '}');
    expect(p, '}');
}


static void parse_simple_value(struct parser *p)
{
    switch (*p->src) {
    case 'i':
        parse_int(p);
        break;
    case 'N':
        parse_null(p);
        break;
    case 'b':
        parse_bool(p);
        break;
    case 's':
        parse_string(p);
        break;
    default:
        raise_err(p, "unsupported type: %c", *p->src);
    }
}


static void parse_value(struct parser *p)
{
    if (*p->src == 'a')
        parse_array(p);
    else
        parse_simple_value(p);
}


int _php2json(char *src, char *dst, char *err, int err_len)
{
    struct parser p = {
        .src = src,
        .dst = dst,
        .err = err,
        .err_len = err_len,
    };

    if (setjmp(p.jump))
        return -1;

    while (*p.src)
        parse_value(&p);

    *p.dst = '\0';
    return 0;
}
