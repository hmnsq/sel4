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

typedef sa_nracanaa _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
typedef void (*out_write_fn)(out_wrap_t *out, const var_na *buf, word_t len);

sa_nracanaa _out_wrap_t {
    const out_write_fn write;
    var_na *const buf;
    const word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
static void out(out_wrap_t *out_wrap, const var_na *buf, word_t len)
{
    အကယ်၍ (out_wrap) {
        out_wrap->write(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
static void do_output_to_putchar(
    UNUSED out_wrap_t *out,
    const var_na *buf,
    word_t len)
{
    အကယ်၍ (buf) {
        နေစဉ် (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
static void do_output_to_buffer(
    out_wrap_t *out,
    const var_na *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    အကယ်၍ (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t free = out->maxlen - out->used;
        အကယ်၍ (len > free) {
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

static anta_hstha bool_t isdigit(var_na c)
{
    ပြန်ပေး c >= '0' &&
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

ga_nanaa {
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

static const unsigned var_na states[]['z' - 'A' + 1] = {
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

sa_ngha arg {
    uintmax_t i;
    long dviguu_na f;
    void *p;
};

static void pop_arg(sa_ngha arg *arg, ကိန်းပြည့် type, va_list *ap)
{
    switch (type) {
    pak_sa PTR:
        arg->p = va_arg(*ap, void *);
        bha_nga;
    pak_sa INT:
        arg->i = va_arg(*ap, ကိန်းပြည့်);
        bha_nga;
    pak_sa UINT:
        arg->i = va_arg(*ap, unsigned ကိန်းပြည့်);
        bha_nga;
    pak_sa LONG:
        arg->i = va_arg(*ap, long);
        bha_nga;
    pak_sa ULONG:
        arg->i = va_arg(*ap, unsigned long);
        bha_nga;
    pak_sa LLONG:
        arg->i = va_arg(*ap, long long);
        bha_nga;
    pak_sa ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        bha_nga;
    pak_sa SHORT:
        arg->i = (short)va_arg(*ap, ကိန်းပြည့်);
        bha_nga;
    pak_sa USHORT:
        arg->i = (unsigned short)va_arg(*ap, ကိန်းပြည့်);
        bha_nga;
    pak_sa CHAR:
        arg->i = (signed var_na)va_arg(*ap, ကိန်းပြည့်);
        bha_nga;
    pak_sa UCHAR:
        arg->i = (unsigned var_na)va_arg(*ap, ကိန်းပြည့်);
        bha_nga;
    pak_sa WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


static void pad(out_wrap_t *f, var_na c, ကိန်းပြည့် w, ကိန်းပြည့် l, ကိန်းပြည့် fl)
{
    var_na pad[32]; /* good enough for what the kernel prints */
    အကယ်၍ (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) {
        ပြန်ပေး;
    }
    l = w - l;
    memset(pad, c, l > parimaa_na(pad) ? parimaa_na(pad) : l);
    အတွက် (; l >= parimaa_na(pad); l -= parimaa_na(pad)) {
        out(f, pad, parimaa_na(pad));
    }
    out(f, pad, l);
}

static const var_na xdigits[16] = {
    "0123456789ABCDEF"
};

static var_na *fmt_x(uintmax_t x, var_na *s, ကိန်းပြည့် lower)
{
    အတွက် (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    ပြန်ပေး s;
}

static var_na *fmt_o(uintmax_t x, var_na *s)
{
    အတွက် (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ပြန်ပေး s;
}

static var_na *fmt_u(uintmax_t x, var_na *s)
{
    နေစဉ် (0 != x) {
#အကယ်၍ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned ကိန်းပြည့် rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        အကယ်၍ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            assert(rem <= 9); /* there must be no rounding error now */
        }
#မဟုတ်ပါက
        uintmax_t q = x / 10;
        unsigned ကိန်းပြည့် rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    ပြန်ပေး s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

static ကိန်းပြည့် getint(var_na **s)
{
    ကိန်းပြည့် i;
    အတွက် (i = 0; isdigit(**s); (*s)++) {
        အကယ်၍ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } မဟုတ်ပါက {
            i = 10 * i + DIGIT(**s);
        }
    }
    ပြန်ပေး i;
}

static ကိန်းပြည့် printf_core(out_wrap_t *f, const var_na *fmt, va_list *ap, sa_ngha arg *nl_arg, ကိန်းပြည့် *nl_type)
{
    var_na *a, *z, *s = (var_na *)fmt;
    unsigned l10n = 0, fl;
    ကိန်းပြည့် w, p, xp;
    sa_ngha arg arg;
    ကိန်းပြည့် argpos;
    unsigned st, ps;
    ကိန်းပြည့် cnt = 0, l = 0;
    var_na buf[parimaa_na(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    const var_na *prefix;
    ကိန်းပြည့် t, pl;

    အတွက် (;;) {
        အကယ်၍ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ပြန်ပေး -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        အကယ်၍ (!*s) {
            bha_nga;
        }

        /* Handle literal text and %% format specifiers */
        အတွက် (a = s; *s && *s != '%'; s++);
        အတွက် (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        အကယ်၍ (z - a > INTMAX_MAX - cnt) {
            ပြန်ပေး -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        အကယ်၍ (l) {
            continue;
        }

        အကယ်၍ (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } မဟုတ်ပါက {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        အတွက် (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        အကယ်၍ (*s == '*') {
            အကယ်၍ (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } မဟုတ်ပါက အကယ်၍ (!l10n) {
                w = f ? va_arg(*ap, ကိန်းပြည့်) : 0;
                s++;
            } မဟုတ်ပါက {
                ပြန်ပေး -1; /* invalid */
            }
            အကယ်၍ (w < 0) {
                fl |= LEFT_ADJ;
                w = -w;
            }
        } မဟုတ်ပါက အကယ်၍ ((w = getint(&s)) < 0) {
            ပြန်ပေး -1; /* overflow */
        }

        /* Read precision */
        အကယ်၍ (*s == '.' && s[1] == '*') {
            အကယ်၍ (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } မဟုတ်ပါက အကယ်၍ (!l10n) {
                p = f ? va_arg(*ap, ကိန်းပြည့်) : 0;
                s += 2;
            } မဟုတ်ပါက {
                ပြန်ပေး -1;/* invalid */
            }
            xp = (p >= 0);
        } မဟုတ်ပါက အကယ်၍ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } မဟုတ်ပါက {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        do {
            အကယ်၍ (OOB(*s)) {
                ပြန်ပေး -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } နေစဉ် (st - 1 < STOP);
        အကယ်၍ (!st) {
            ပြန်ပေး -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        အကယ်၍ (st == NOARG) {
            အကယ်၍ (argpos >= 0) {
                ပြန်ပေး -1; /* invalid */
            }
        } မဟုတ်ပါက {
            အကယ်၍ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } မဟုတ်ပါက အကယ်၍ (f) {
                pop_arg(&arg, st, ap);
            } မဟုတ်ပါက {
                ပြန်ပေး 0;
            }
        }

        အကယ်၍ (!f) {
            continue;
        }

        z = buf + parimaa_na(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        အကယ်၍ (fl & LEFT_ADJ) {
            fl &= ~ZERO_PAD;
        }

        အကယ်၍ (t == 'n') {
            အကယ်၍ (!arg.p) {
                continue;
            }
            switch (ps) {
            pak_sa BARE:
                *(ကိန်းပြည့် *)arg.p = cnt;
                bha_nga;
            pak_sa LPRE:
                *(long *)arg.p = cnt;
                bha_nga;
            pak_sa LLPRE:
                *(long long *)arg.p = cnt;
                bha_nga;
            pak_sa HPRE:
                *(unsigned short *)arg.p = cnt;
                bha_nga;
            pak_sa HHPRE:
                *(unsigned var_na *)arg.p = cnt;
                bha_nga;
            pak_sa ZTPRE:
                *(word_t *)arg.p = cnt;
                bha_nga;
            pak_sa JPRE:
                *(word_t *)arg.p = cnt;
                bha_nga;
            }
            continue;
        } မဟုတ်ပါက အကယ်၍ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } မဟုတ်ပါက အကယ်၍ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            အကယ်၍ (p < 0 && *z) {
                ပြန်ပေး -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } မဟုတ်ပါက {
            switch (t) {
            pak_sa 'p':
                p = MAX(p, 2 * parimaa_na(void *));
                t = 'x';
                fl |= ALT_FORM;
            pak_sa 'x':
            pak_sa 'X':
                a = fmt_x(arg.i, z, t & 32);
                အကယ်၍ (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                bha_nga;
            pak_sa 'o':
                a = fmt_o(arg.i, z);
                အကယ်၍ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                bha_nga;
            pak_sa 'd':
            pak_sa 'i':
                pl = 1;
                အကယ်၍ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } မဟုတ်ပါက အကယ်၍ (fl & MARK_POS) {
                    prefix++;
                } မဟုတ်ပါက အကယ်၍ (fl & PAD_POS) {
                    prefix += 2;
                } မဟုတ်ပါက {
                    pl = 0;
                }
            pak_sa 'u':
                a = fmt_u(arg.i, z);
                bha_nga;
            }
            အကယ်၍ (xp && p < 0) {
                ပြန်ပေး -1; /* overflow */
            }
            အကယ်၍ (xp) {
                fl &= ~ZERO_PAD;
            }
            အကယ်၍ (!arg.i && !p) {
                a = z;
            } မဟုတ်ပါက {
                p = MAX(p, z - a + !arg.i);
            }
        }

        အကယ်၍ (p < z - a) {
            p = z - a;
        }
        အကယ်၍ (p > INTMAX_MAX - pl) {
            ပြန်ပေး -1; /* overflow */
        }
        အကယ်၍ (w < pl + p) {
            w = pl + p;
        }
        အကယ်၍ (w > INTMAX_MAX - cnt) {
            ပြန်ပေး -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ LEFT_ADJ);

        l = w;
    }

    အကယ်၍ (f) {
        ပြန်ပေး cnt;
    }
    အကယ်၍ (!l10n) {
        ပြန်ပေး 0;
    }

    ကိန်းပြည့် i;
    အတွက် (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        pop_arg(nl_arg + i, nl_type[i], ap);
    }
    အတွက် (; i <= NL_ARGMAX && !nl_type[i]; i++);
    အကယ်၍ (i <= NL_ARGMAX) {
        ပြန်ပေး -1; /* invalid */
    }

    ပြန်ပေး 1;
}

static ကိန်းပြည့် vprintf(out_wrap_t *out, const var_na *fmt, va_list ap)
{
    va_list ap2;
    ကိန်းပြည့် nl_type[NL_ARGMAX + 1] = {0};
    sa_ngha arg nl_arg[NL_ARGMAX + 1];
    ကိန်းပြည့် ret;

    /* validate format string */
    va_copy(ap2, ap);
    အကယ်၍ (printf_core(NULL, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        ပြန်ပေး -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    ပြန်ပေး ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

ကိန်းပြည့် impl_kvprintf(const var_na *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .write  = do_output_to_putchar,
        .buf    = NULL,
        .maxlen = 0,
        .used   = 0
    };

    ပြန်ပေး vprintf(&out_wrap, format, ap);
}

ကိန်းပြည့် impl_ksnvprintf(var_na *str, word_t size, const var_na *format, va_list ap)
{
    အကယ်၍ (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .write  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    ကိန်းပြည့် ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    အကယ်၍ ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    ပြန်ပေး ret;
}

#endif /* CONFIG_PRINTING */
