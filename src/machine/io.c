/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Portions derived from musl:
 *
 * Copyright (c) 2005-2020 Rich Felker, et al.
 *
 * SPDX-License-Identifier: MIT
 */

#include <config.h>
#include <machine/io.h>

#ifdef CONFIG_PRINTING

#include <stdarg.h>
#include <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

typedef struct _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
typedef void (*out_write_fn)(out_wrap_t *out, const char *buf, word_t len);

struct _out_wrap_t {
    const out_write_fn write;
    char *const buf;
    const word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
static void out(out_wrap_t *out_wrap, const char *buf, word_t len)
{
    만약 (out_wrap) {
        out_wrap->write(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
static void do_output_to_putchar(
    UNUSED out_wrap_t *out,
    const char *buf,
    word_t len)
{
    만약 (buf) {
        동안 (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
static void do_output_to_buffer(
    out_wrap_t *out,
    const char *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    만약 (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t free = out->maxlen - out->used;
        만약 (len > free) {
            len = free;
        }
        memcpy(&out->buf[out->used], buf, len);
        out->used += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

static inline bool_t isdigit(char c)
{
    반환 c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define LEFT_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

enum {
    BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
    ZTPRE, JPRE,
    STOP,
    PTR, INT, UINT, ULLONG,
    LONG, ULONG,
    SHORT, USHORT, CHAR, UCHAR,
    WORDT, LLONG,
#define IMAX LLONG
#define UMAX ULLONG
#define PDIFF LONG
#define UIPTR ULONG
    NOARG,
    MAXSTATE
};

#define S(x) [(x)-'A']

static const unsigned char states[]['z' - 'A' + 1] = {
    { /* 0: bare types */
        S('d') = INT, S('i') = INT,
        S('o') = UINT, S('u') = UINT, S('x') = UINT, S('X') = UINT,
        S('c') = CHAR,
        S('s') = PTR, S('p') = UIPTR, S('n') = PTR,
        S('l') = LPRE, S('h') = HPRE,
        S('z') = ZTPRE, S('j') = JPRE, S('t') = ZTPRE,
    }, { /* 1: l-prefixed */
        S('d') = LONG, S('i') = LONG,
        S('o') = ULONG, S('u') = ULONG, S('x') = ULONG, S('X') = ULONG,
        S('n') = PTR,
        S('l') = LLPRE,
    }, { /* 2: ll-prefixed */
        S('d') = LLONG, S('i') = LLONG,
        S('o') = ULLONG, S('u') = ULLONG,
        S('x') = ULLONG, S('X') = ULLONG,
        S('n') = PTR,
    }, { /* 3: h-prefixed */
        S('d') = SHORT, S('i') = SHORT,
        S('o') = USHORT, S('u') = USHORT,
        S('x') = USHORT, S('X') = USHORT,
        S('n') = PTR,
        S('h') = HHPRE,
    }, { /* 4: hh-prefixed */
        S('d') = CHAR, S('i') = CHAR,
        S('o') = UCHAR, S('u') = UCHAR,
        S('x') = UCHAR, S('X') = UCHAR,
        S('n') = PTR,
    }, { /* 5: L-prefixed not supported */
    }, { /* 6: z- or t-prefixed (assumed to be same size) */
        S('d') = PDIFF, S('i') = PDIFF,
        S('o') = WORDT, S('u') = WORDT,
        S('x') = WORDT, S('X') = WORDT,
        S('n') = PTR,
    }, { /* 7: j-prefixed */
        S('d') = IMAX, S('i') = IMAX,
        S('o') = UMAX, S('u') = UMAX,
        S('x') = UMAX, S('X') = UMAX,
        S('n') = PTR,
    }
};

#define OOB(x) ((unsigned)(x)-'A' > 'z'-'A')
#define DIGIT(c) (c - '0')

union arg {
    uintmax_t i;
    long double f;
    void *p;
};

static void pop_arg(union arg *arg, 정수 type, va_list *ap)
{
    switch (type) {
    case PTR:
        arg->p = va_arg(*ap, void *);
        break;
    case INT:
        arg->i = va_arg(*ap, 정수);
        break;
    case UINT:
        arg->i = va_arg(*ap, unsigned 정수);
        break;
    case LONG:
        arg->i = va_arg(*ap, long);
        break;
    case ULONG:
        arg->i = va_arg(*ap, unsigned long);
        break;
    case LLONG:
        arg->i = va_arg(*ap, long long);
        break;
    case ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        break;
    case SHORT:
        arg->i = (short)va_arg(*ap, 정수);
        break;
    case USHORT:
        arg->i = (unsigned short)va_arg(*ap, 정수);
        break;
    case CHAR:
        arg->i = (signed char)va_arg(*ap, 정수);
        break;
    case UCHAR:
        arg->i = (unsigned char)va_arg(*ap, 정수);
        break;
    case WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


static void pad(out_wrap_t *f, char c, 정수 w, 정수 l, 정수 fl)
{
    char pad[32]; /* good enough for what the kernel prints */
    만약 (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) {
        반환;
    }
    l = w - l;
    memset(pad, c, l > sizeof(pad) ? sizeof(pad) : l);
    위해 (; l >= sizeof(pad); l -= sizeof(pad)) {
        out(f, pad, sizeof(pad));
    }
    out(f, pad, l);
}

static const char xdigits[16] = {
    "0123456789ABCDEF"
};

static char *fmt_x(uintmax_t x, char *s, 정수 lower)
{
    위해 (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    반환 s;
}

static char *fmt_o(uintmax_t x, char *s)
{
    위해 (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    반환 s;
}

static char *fmt_u(uintmax_t x, char *s)
{
    동안 (0 != x) {
#만약 defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
        /* On 32-bit systems, dividing a 64-bit number by 10 makes the compiler
         * call a helper function from a compiler runtime library. The actual
         * function differs, currently for x86 it's __udivdi3(), for ARM its
         * __aeabi_uldivmod() and for RISC-V it's __umoddi3(). The kernel is not
         * supposed to have dependencies on a compiler library, so the algorithm
         * below (taken from Hacker's Delight) is used to divide by 10 with only
         * basic operation and avoiding even multiplication.
         */
        uintmax_t q = (x >> 1) + (x >> 2); /* q = x/2 + x/4 = 3x/4 */
        q += (q >> 4); /* q += (3x/4)/16 = 51x/2^6 */
        q += (q >> 8); /* q += (51x/2^6)/2^8 = 13107x/2^14 */
        q += (q >> 16); /* q += (13107x/2^14)/2^16 = 858993458x/2^30 */
        q += (q >> 32); /* q += (858993458x/2^30)/2^32 = .../2^62 */
        q >>= 3; /* q is roughly 0.8x, so q/8 is roughly x/10 */
        unsigned 정수 rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        만약 (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            assert(rem <= 9); /* there must be no rounding error now */
        }
#아니면
        uintmax_t q = x / 10;
        unsigned 정수 rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    반환 s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

static 정수 getint(char **s)
{
    정수 i;
    위해 (i = 0; isdigit(**s); (*s)++) {
        만약 (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } 아니면 {
            i = 10 * i + DIGIT(**s);
        }
    }
    반환 i;
}

static 정수 printf_core(out_wrap_t *f, const char *fmt, va_list *ap, union arg *nl_arg, 정수 *nl_type)
{
    char *a, *z, *s = (char *)fmt;
    unsigned l10n = 0, fl;
    정수 w, p, xp;
    union arg arg;
    정수 argpos;
    unsigned st, ps;
    정수 cnt = 0, l = 0;
    char buf[sizeof(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    const char *prefix;
    정수 t, pl;

    위해 (;;) {
        만약 (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            반환 -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        만약 (!*s) {
            break;
        }

        /* Handle literal text and %% format specifiers */
        위해 (a = s; *s && *s != '%'; s++);
        위해 (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        만약 (z - a > INTMAX_MAX - cnt) {
            반환 -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        만약 (l) {
            continue;
        }

        만약 (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } 아니면 {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        위해 (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        만약 (*s == '*') {
            만약 (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } 아니면 만약 (!l10n) {
                w = f ? va_arg(*ap, 정수) : 0;
                s++;
            } 아니면 {
                반환 -1; /* invalid */
            }
            만약 (w < 0) {
                fl |= LEFT_ADJ;
                w = -w;
            }
        } 아니면 만약 ((w = getint(&s)) < 0) {
            반환 -1; /* overflow */
        }

        /* Read precision */
        만약 (*s == '.' && s[1] == '*') {
            만약 (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } 아니면 만약 (!l10n) {
                p = f ? va_arg(*ap, 정수) : 0;
                s += 2;
            } 아니면 {
                반환 -1;/* invalid */
            }
            xp = (p >= 0);
        } 아니면 만약 (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } 아니면 {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        do {
            만약 (OOB(*s)) {
                반환 -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } 동안 (st - 1 < STOP);
        만약 (!st) {
            반환 -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        만약 (st == NOARG) {
            만약 (argpos >= 0) {
                반환 -1; /* invalid */
            }
        } 아니면 {
            만약 (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } 아니면 만약 (f) {
                pop_arg(&arg, st, ap);
            } 아니면 {
                반환 0;
            }
        }

        만약 (!f) {
            continue;
        }

        z = buf + sizeof(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        만약 (fl & LEFT_ADJ) {
            fl &= ~ZERO_PAD;
        }

        만약 (t == 'n') {
            만약 (!arg.p) {
                continue;
            }
            switch (ps) {
            case BARE:
                *(정수 *)arg.p = cnt;
                break;
            case LPRE:
                *(long *)arg.p = cnt;
                break;
            case LLPRE:
                *(long long *)arg.p = cnt;
                break;
            case HPRE:
                *(unsigned short *)arg.p = cnt;
                break;
            case HHPRE:
                *(unsigned char *)arg.p = cnt;
                break;
            case ZTPRE:
                *(word_t *)arg.p = cnt;
                break;
            case JPRE:
                *(word_t *)arg.p = cnt;
                break;
            }
            continue;
        } 아니면 만약 (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } 아니면 만약 (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            만약 (p < 0 && *z) {
                반환 -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } 아니면 {
            switch (t) {
            case 'p':
                p = MAX(p, 2 * sizeof(void *));
                t = 'x';
                fl |= ALT_FORM;
            case 'x':
            case 'X':
                a = fmt_x(arg.i, z, t & 32);
                만약 (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                break;
            case 'o':
                a = fmt_o(arg.i, z);
                만약 ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                break;
            case 'd':
            case 'i':
                pl = 1;
                만약 (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } 아니면 만약 (fl & MARK_POS) {
                    prefix++;
                } 아니면 만약 (fl & PAD_POS) {
                    prefix += 2;
                } 아니면 {
                    pl = 0;
                }
            case 'u':
                a = fmt_u(arg.i, z);
                break;
            }
            만약 (xp && p < 0) {
                반환 -1; /* overflow */
            }
            만약 (xp) {
                fl &= ~ZERO_PAD;
            }
            만약 (!arg.i && !p) {
                a = z;
            } 아니면 {
                p = MAX(p, z - a + !arg.i);
            }
        }

        만약 (p < z - a) {
            p = z - a;
        }
        만약 (p > INTMAX_MAX - pl) {
            반환 -1; /* overflow */
        }
        만약 (w < pl + p) {
            w = pl + p;
        }
        만약 (w > INTMAX_MAX - cnt) {
            반환 -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ LEFT_ADJ);

        l = w;
    }

    만약 (f) {
        반환 cnt;
    }
    만약 (!l10n) {
        반환 0;
    }

    정수 i;
    위해 (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        pop_arg(nl_arg + i, nl_type[i], ap);
    }
    위해 (; i <= NL_ARGMAX && !nl_type[i]; i++);
    만약 (i <= NL_ARGMAX) {
        반환 -1; /* invalid */
    }

    반환 1;
}

static 정수 vprintf(out_wrap_t *out, const char *fmt, va_list ap)
{
    va_list ap2;
    정수 nl_type[NL_ARGMAX + 1] = {0};
    union arg nl_arg[NL_ARGMAX + 1];
    정수 ret;

    /* validate format string */
    va_copy(ap2, ap);
    만약 (printf_core(NULL, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        반환 -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    반환 ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

정수 impl_kvprintf(const char *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .write  = do_output_to_putchar,
        .buf    = NULL,
        .maxlen = 0,
        .used   = 0
    };

    반환 vprintf(&out_wrap, format, ap);
}

정수 impl_ksnvprintf(char *str, word_t size, const char *format, va_list ap)
{
    만약 (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .write  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    정수 ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    만약 ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    반환 ret;
}

#endif /* CONFIG_PRINTING */
