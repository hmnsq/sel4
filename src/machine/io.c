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

#ضمن <config.h>
#ضمن <machine/io.h>

#ifdef CONFIG_PRINTING

#ضمن <stdarg.h>
#ضمن <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

تعريفنوع بنية _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
تعريفنوع فراغ (*out_write_fn)(out_wrap_t *out, ثابت حرف *buf, word_t len);

بنية _out_wrap_t {
    ثابت out_write_fn اكتب;
    حرف *ثابت buf;
    ثابت word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ساكن فراغ out(out_wrap_t *out_wrap, ثابت حرف *buf, word_t len)
{
    ܐܢ (out_wrap) {
        out_wrap->اكتب(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساكن فراغ do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ثابت حرف *buf,
    word_t len)
{
    ܐܢ (buf) {
        ܟܡܐ (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساكن فراغ do_output_to_buffer(
    out_wrap_t *out,
    ثابت حرف *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ܐܢ (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t حرر = out->maxlen - out->used;
        ܐܢ (len > حرر) {
            len = حرر;
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

ساكن مضمن bool_t isdigit(حرف c)
{
    ܦܢܐ c >= '0' &&
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

تعداد {
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

ساكن ثابت unsigned حرف states[]['z' - 'A' + 1] = {
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

اتحاد arg {
    uintmax_t i;
    long مزدوج f;
    فراغ *p;
};

ساكن فراغ pop_arg(اتحاد arg *arg, صحيح type, va_list *ap)
{
    بدل (type) {
    حالة PTR:
        arg->p = va_arg(*ap, فراغ *);
        اكسر;
    حالة INT:
        arg->i = va_arg(*ap, صحيح);
        اكسر;
    حالة UINT:
        arg->i = va_arg(*ap, unsigned صحيح);
        اكسر;
    حالة LONG:
        arg->i = va_arg(*ap, long);
        اكسر;
    حالة ULONG:
        arg->i = va_arg(*ap, unsigned long);
        اكسر;
    حالة LLONG:
        arg->i = va_arg(*ap, long long);
        اكسر;
    حالة ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        اكسر;
    حالة SHORT:
        arg->i = (short)va_arg(*ap, صحيح);
        اكسر;
    حالة USHORT:
        arg->i = (unsigned short)va_arg(*ap, صحيح);
        اكسر;
    حالة CHAR:
        arg->i = (signed حرف)va_arg(*ap, صحيح);
        اكسر;
    حالة UCHAR:
        arg->i = (unsigned حرف)va_arg(*ap, صحيح);
        اكسر;
    حالة WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


ساكن فراغ pad(out_wrap_t *f, حرف c, صحيح w, صحيح l, صحيح fl)
{
    حرف pad[32]; /* good enough for what the kernel prints */
    ܐܢ (fl & (LEFT_ADJ | ZERO_PAD) || l >= w) {
        ܦܢܐ;
    }
    l = w - l;
    memset(pad, c, l > حجم(pad) ? حجم(pad) : l);
    لكل (; l >= حجم(pad); l -= حجم(pad)) {
        out(f, pad, حجم(pad));
    }
    out(f, pad, l);
}

ساكن ثابت حرف xdigits[16] = {
    "0123456789ABCDEF"
};

ساكن حرف *fmt_x(uintmax_t x, حرف *s, صحيح lower)
{
    لكل (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    ܦܢܐ s;
}

ساكن حرف *fmt_o(uintmax_t x, حرف *s)
{
    لكل (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ܦܢܐ s;
}

ساكن حرف *fmt_u(uintmax_t x, حرف *s)
{
    ܟܡܐ (0 != x) {
#ܐܢ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned صحيح rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ܐܢ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            أكد(rem <= 9); /* there must be no rounding error now */
        }
#ܐܚܪܢܐ
        uintmax_t q = x / 10;
        unsigned صحيح rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    ܦܢܐ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

ساكن صحيح getint(حرف **s)
{
    صحيح i;
    لكل (i = 0; isdigit(**s); (*s)++) {
        ܐܢ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ܐܚܪܢܐ {
            i = 10 * i + DIGIT(**s);
        }
    }
    ܦܢܐ i;
}

ساكن صحيح printf_core(out_wrap_t *f, ثابت حرف *fmt, va_list *ap, اتحاد arg *nl_arg, صحيح *nl_type)
{
    حرف *a, *z, *s = (حرف *)fmt;
    unsigned l10n = 0, fl;
    صحيح w, p, xp;
    اتحاد arg arg;
    صحيح argpos;
    unsigned st, ps;
    صحيح cnt = 0, l = 0;
    حرف buf[حجم(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ثابت حرف *prefix;
    صحيح t, pl;

    لكل (;;) {
        ܐܢ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ܦܢܐ -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ܐܢ (!*s) {
            اكسر;
        }

        /* Handle literal text and %% format specifiers */
        لكل (a = s; *s && *s != '%'; s++);
        لكل (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ܐܢ (z - a > INTMAX_MAX - cnt) {
            ܦܢܐ -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        ܐܢ (l) {
            استمر;
        }

        ܐܢ (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ܐܚܪܢܐ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        لكل (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        ܐܢ (*s == '*') {
            ܐܢ (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ܐܚܪܢܐ ܐܢ (!l10n) {
                w = f ? va_arg(*ap, صحيح) : 0;
                s++;
            } ܐܚܪܢܐ {
                ܦܢܐ -1; /* invalid */
            }
            ܐܢ (w < 0) {
                fl |= LEFT_ADJ;
                w = -w;
            }
        } ܐܚܪܢܐ ܐܢ ((w = getint(&s)) < 0) {
            ܦܢܐ -1; /* overflow */
        }

        /* Read precision */
        ܐܢ (*s == '.' && s[1] == '*') {
            ܐܢ (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ܐܚܪܢܐ ܐܢ (!l10n) {
                p = f ? va_arg(*ap, صحيح) : 0;
                s += 2;
            } ܐܚܪܢܐ {
                ܦܢܐ -1;/* invalid */
            }
            xp = (p >= 0);
        } ܐܚܪܢܐ ܐܢ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ܐܚܪܢܐ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        ܥܒܕ {
            ܐܢ (OOB(*s)) {
                ܦܢܐ -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } ܟܡܐ (st - 1 < STOP);
        ܐܢ (!st) {
            ܦܢܐ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ܐܢ (st == NOARG) {
            ܐܢ (argpos >= 0) {
                ܦܢܐ -1; /* invalid */
            }
        } ܐܚܪܢܐ {
            ܐܢ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ܐܚܪܢܐ ܐܢ (f) {
                pop_arg(&arg, st, ap);
            } ܐܚܪܢܐ {
                ܦܢܐ 0;
            }
        }

        ܐܢ (!f) {
            استمر;
        }

        z = buf + حجم(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ܐܢ (fl & LEFT_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ܐܢ (t == 'n') {
            ܐܢ (!arg.p) {
                استمر;
            }
            بدل (ps) {
            حالة BARE:
                *(صحيح *)arg.p = cnt;
                اكسر;
            حالة LPRE:
                *(long *)arg.p = cnt;
                اكسر;
            حالة LLPRE:
                *(long long *)arg.p = cnt;
                اكسر;
            حالة HPRE:
                *(unsigned short *)arg.p = cnt;
                اكسر;
            حالة HHPRE:
                *(unsigned حرف *)arg.p = cnt;
                اكسر;
            حالة ZTPRE:
                *(word_t *)arg.p = cnt;
                اكسر;
            حالة JPRE:
                *(word_t *)arg.p = cnt;
                اكسر;
            }
            استمر;
        } ܐܚܪܢܐ ܐܢ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ܐܚܪܢܐ ܐܢ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ܐܢ (p < 0 && *z) {
                ܦܢܐ -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ܐܚܪܢܐ {
            بدل (t) {
            حالة 'p':
                p = MAX(p, 2 * حجم(فراغ *));
                t = 'x';
                fl |= ALT_FORM;
            حالة 'x':
            حالة 'X':
                a = fmt_x(arg.i, z, t & 32);
                ܐܢ (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                اكسر;
            حالة 'o':
                a = fmt_o(arg.i, z);
                ܐܢ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                اكسر;
            حالة 'd':
            حالة 'i':
                pl = 1;
                ܐܢ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ܐܚܪܢܐ ܐܢ (fl & MARK_POS) {
                    prefix++;
                } ܐܚܪܢܐ ܐܢ (fl & PAD_POS) {
                    prefix += 2;
                } ܐܚܪܢܐ {
                    pl = 0;
                }
            حالة 'u':
                a = fmt_u(arg.i, z);
                اكسر;
            }
            ܐܢ (xp && p < 0) {
                ܦܢܐ -1; /* overflow */
            }
            ܐܢ (xp) {
                fl &= ~ZERO_PAD;
            }
            ܐܢ (!arg.i && !p) {
                a = z;
            } ܐܚܪܢܐ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ܐܢ (p < z - a) {
            p = z - a;
        }
        ܐܢ (p > INTMAX_MAX - pl) {
            ܦܢܐ -1; /* overflow */
        }
        ܐܢ (w < pl + p) {
            w = pl + p;
        }
        ܐܢ (w > INTMAX_MAX - cnt) {
            ܦܢܐ -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ LEFT_ADJ);

        l = w;
    }

    ܐܢ (f) {
        ܦܢܐ cnt;
    }
    ܐܢ (!l10n) {
        ܦܢܐ 0;
    }

    صحيح i;
    لكل (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        pop_arg(nl_arg + i, nl_type[i], ap);
    }
    لكل (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ܐܢ (i <= NL_ARGMAX) {
        ܦܢܐ -1; /* invalid */
    }

    ܦܢܐ 1;
}

ساكن صحيح vprintf(out_wrap_t *out, ثابت حرف *fmt, va_list ap)
{
    va_list ap2;
    صحيح nl_type[NL_ARGMAX + 1] = {0};
    اتحاد arg nl_arg[NL_ARGMAX + 1];
    صحيح ret;

    /* validate format string */
    va_copy(ap2, ap);
    ܐܢ (printf_core(عدم, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        ܦܢܐ -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    ܦܢܐ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

صحيح impl_kvprintf(ثابت حرف *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .اكتب  = do_output_to_putchar,
        .buf    = عدم,
        .maxlen = 0,
        .used   = 0
    };

    ܦܢܐ vprintf(&out_wrap, format, ap);
}

صحيح impl_ksnvprintf(حرف *str, word_t size, ثابت حرف *format, va_list ap)
{
    ܐܢ (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .اكتب  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    صحيح ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ܐܢ ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    ܦܢܐ ret;
}

#endif /* CONFIG_PRINTING */
