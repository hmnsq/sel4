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

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ifdef CONFIG_PRINTING

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*out_write_fn)(out_wrap_t *out, ಧ್ರುವ ವರ್ಣ *buf, word_t len);

ಸಂರಚನಾ _out_wrap_t {
    ಧ್ರುವ out_write_fn ಲಿಖ;
    ವರ್ಣ *ಧ್ರುವ buf;
    ಧ್ರುವ word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ಸ್ಥಿರ ಶೂನ್ಯ out(out_wrap_t *out_wrap, ಧ್ರುವ ವರ್ಣ *buf, word_t len)
{
    ಯದಿ (out_wrap) {
        out_wrap->ಲಿಖ(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ಸ್ಥಿರ ಶೂನ್ಯ do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ಧ್ರುವ ವರ್ಣ *buf,
    word_t len)
{
    ಯದಿ (buf) {
        ಯಾವತ್ (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ಸ್ಥಿರ ಶೂನ್ಯ do_output_to_buffer(
    out_wrap_t *out,
    ಧ್ರುವ ವರ್ಣ *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ಯದಿ (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t ಮುಕ್ತ = out->maxlen - out->used;
        ಯದಿ (len > ಮುಕ್ತ) {
            len = ಮುಕ್ತ;
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

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t isdigit(ವರ್ಣ c)
{
    ಪ್ರತ್ಯಾಗಮ c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define ಎಡ_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|ಎಡ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

ಗಣನಾ {
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

ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ states[]['z' - 'A' + 1] = {
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

ಸಙ್ಘ arg {
    uintmax_t i;
    long ದ್ವಿಗುಣ f;
    ಶೂನ್ಯ *p;
};

ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_arg(ಸಙ್ಘ arg *arg, ಪೂರ್ಣಾಙ್ಕ type, va_list *ap)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ PTR:
        arg->p = va_arg(*ap, ಶೂನ್ಯ *);
        ಭಙ್ಗ;
    ಪಕ್ಷ INT:
        arg->i = va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ UINT:
        arg->i = va_arg(*ap, unsigned ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ LONG:
        arg->i = va_arg(*ap, long);
        ಭಙ್ಗ;
    ಪಕ್ಷ ULONG:
        arg->i = va_arg(*ap, unsigned long);
        ಭಙ್ಗ;
    ಪಕ್ಷ LLONG:
        arg->i = va_arg(*ap, long long);
        ಭಙ್ಗ;
    ಪಕ್ಷ ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        ಭಙ್ಗ;
    ಪಕ್ಷ SHORT:
        arg->i = (short)va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ USHORT:
        arg->i = (unsigned short)va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ CHAR:
        arg->i = (signed ವರ್ಣ)va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ UCHAR:
        arg->i = (unsigned ವರ್ಣ)va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ);
        ಭಙ್ಗ;
    ಪಕ್ಷ WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


ಸ್ಥಿರ ಶೂನ್ಯ pad(out_wrap_t *f, ವರ್ಣ c, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ l, ಪೂರ್ಣಾಙ್ಕ fl)
{
    ವರ್ಣ pad[32]; /* good enough for what the kernel prints */
    ಯದಿ (fl & (ಎಡ_ADJ | ZERO_PAD) || l >= w) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    l = w - l;
    memset(pad, c, l > ಪರಿಮಾಣ(pad) ? ಪರಿಮಾಣ(pad) : l);
    ಪ್ರತಿ (; l >= ಪರಿಮಾಣ(pad); l -= ಪರಿಮಾಣ(pad)) {
        out(f, pad, ಪರಿಮಾಣ(pad));
    }
    out(f, pad, l);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ xdigits[16] = {
    "0123456789ABCDEF"
};

ಸ್ಥಿರ ವರ್ಣ *fmt_x(uintmax_t x, ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ lower)
{
    ಪ್ರತಿ (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ ವರ್ಣ *fmt_o(uintmax_t x, ವರ್ಣ *s)
{
    ಪ್ರತಿ (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ ವರ್ಣ *fmt_u(uintmax_t x, ವರ್ಣ *s)
{
    ಯಾವತ್ (0 != x) {
#ಯದಿ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned ಪೂರ್ಣಾಙ್ಕ rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ಯದಿ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ಪ್ರತಿಜ್ಞಾ(rem <= 9); /* there must be no rounding error now */
        }
#ಅನ್ಯಥಾ
        uintmax_t q = x / 10;
        unsigned ಪೂರ್ಣಾಙ್ಕ rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ getint(ವರ್ಣ **s)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 0; isdigit(**s); (*s)++) {
        ಯದಿ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ಅನ್ಯಥಾ {
            i = 10 * i + DIGIT(**s);
        }
    }
    ಪ್ರತ್ಯಾಗಮ i;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ printf_core(out_wrap_t *f, ಧ್ರುವ ವರ್ಣ *fmt, va_list *ap, ಸಙ್ಘ arg *nl_arg, ಪೂರ್ಣಾಙ್ಕ *nl_type)
{
    ವರ್ಣ *a, *z, *s = (ವರ್ಣ *)fmt;
    unsigned l10n = 0, fl;
    ಪೂರ್ಣಾಙ್ಕ w, p, xp;
    ಸಙ್ಘ arg arg;
    ಪೂರ್ಣಾಙ್ಕ argpos;
    unsigned st, ps;
    ಪೂರ್ಣಾಙ್ಕ cnt = 0, l = 0;
    ವರ್ಣ buf[ಪರಿಮಾಣ(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ಧ್ರುವ ವರ್ಣ *prefix;
    ಪೂರ್ಣಾಙ್ಕ t, pl;

    ಪ್ರತಿ (;;) {
        ಯದಿ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ಯದಿ (!*s) {
            ಭಙ್ಗ;
        }

        /* Handle literal text and %% format specifiers */
        ಪ್ರತಿ (a = s; *s && *s != '%'; s++);
        ಪ್ರತಿ (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ಯದಿ (z - a > INTMAX_MAX - cnt) {
            ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        ಯದಿ (l) {
            ಅನುವರ್ತ;
        }

        ಯದಿ (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ಅನ್ಯಥಾ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        ಪ್ರತಿ (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        ಯದಿ (*s == '*') {
            ಯದಿ (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ಅನ್ಯಥಾ ಯದಿ (!l10n) {
                w = f ? va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ) : 0;
                s++;
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ -1; /* invalid */
            }
            ಯದಿ (w < 0) {
                fl |= ಎಡ_ADJ;
                w = -w;
            }
        } ಅನ್ಯಥಾ ಯದಿ ((w = getint(&s)) < 0) {
            ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
        }

        /* Read precision */
        ಯದಿ (*s == '.' && s[1] == '*') {
            ಯದಿ (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ಅನ್ಯಥಾ ಯದಿ (!l10n) {
                p = f ? va_arg(*ap, ಪೂರ್ಣಾಙ್ಕ) : 0;
                s += 2;
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ -1;/* invalid */
            }
            xp = (p >= 0);
        } ಅನ್ಯಥಾ ಯದಿ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ಅನ್ಯಥಾ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        ಕುರು {
            ಯದಿ (OOB(*s)) {
                ಪ್ರತ್ಯಾಗಮ -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } ಯಾವತ್ (st - 1 < STOP);
        ಯದಿ (!st) {
            ಪ್ರತ್ಯಾಗಮ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ಯದಿ (st == NOARG) {
            ಯದಿ (argpos >= 0) {
                ಪ್ರತ್ಯಾಗಮ -1; /* invalid */
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ಅನ್ಯಥಾ ಯದಿ (f) {
                ತೆಗೆ_arg(&arg, st, ap);
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        }

        ಯದಿ (!f) {
            ಅನುವರ್ತ;
        }

        z = buf + ಪರಿಮಾಣ(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ಯದಿ (fl & ಎಡ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ಯದಿ (t == 'n') {
            ಯದಿ (!arg.p) {
                ಅನುವರ್ತ;
            }
            ವಿಕಲ್ಪ (ps) {
            ಪಕ್ಷ BARE:
                *(ಪೂರ್ಣಾಙ್ಕ *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ LPRE:
                *(long *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ LLPRE:
                *(long long *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ HPRE:
                *(unsigned short *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ HHPRE:
                *(unsigned ವರ್ಣ *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ ZTPRE:
                *(word_t *)arg.p = cnt;
                ಭಙ್ಗ;
            ಪಕ್ಷ JPRE:
                *(word_t *)arg.p = cnt;
                ಭಙ್ಗ;
            }
            ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ ಯದಿ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ಅನ್ಯಥಾ ಯದಿ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ಯದಿ (p < 0 && *z) {
                ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ಅನ್ಯಥಾ {
            ವಿಕಲ್ಪ (t) {
            ಪಕ್ಷ 'p':
                p = MAX(p, 2 * ಪರಿಮಾಣ(ಶೂನ್ಯ *));
                t = 'x';
                fl |= ALT_FORM;
            ಪಕ್ಷ 'x':
            ಪಕ್ಷ 'X':
                a = fmt_x(arg.i, z, t & 32);
                ಯದಿ (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ 'o':
                a = fmt_o(arg.i, z);
                ಯದಿ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ 'd':
            ಪಕ್ಷ 'i':
                pl = 1;
                ಯದಿ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ಅನ್ಯಥಾ ಯದಿ (fl & MARK_POS) {
                    prefix++;
                } ಅನ್ಯಥಾ ಯದಿ (fl & PAD_POS) {
                    prefix += 2;
                } ಅನ್ಯಥಾ {
                    pl = 0;
                }
            ಪಕ್ಷ 'u':
                a = fmt_u(arg.i, z);
                ಭಙ್ಗ;
            }
            ಯದಿ (xp && p < 0) {
                ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
            }
            ಯದಿ (xp) {
                fl &= ~ZERO_PAD;
            }
            ಯದಿ (!arg.i && !p) {
                a = z;
            } ಅನ್ಯಥಾ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ಯದಿ (p < z - a) {
            p = z - a;
        }
        ಯದಿ (p > INTMAX_MAX - pl) {
            ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
        }
        ಯದಿ (w < pl + p) {
            w = pl + p;
        }
        ಯದಿ (w > INTMAX_MAX - cnt) {
            ಪ್ರತ್ಯಾಗಮ -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ಎಡ_ADJ);

        l = w;
    }

    ಯದಿ (f) {
        ಪ್ರತ್ಯಾಗಮ cnt;
    }
    ಯದಿ (!l10n) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        ತೆಗೆ_arg(nl_arg + i, nl_type[i], ap);
    }
    ಪ್ರತಿ (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ಯದಿ (i <= NL_ARGMAX) {
        ಪ್ರತ್ಯಾಗಮ -1; /* invalid */
    }

    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vprintf(out_wrap_t *out, ಧ್ರುವ ವರ್ಣ *fmt, va_list ap)
{
    va_list ap2;
    ಪೂರ್ಣಾಙ್ಕ nl_type[NL_ARGMAX + 1] = {0};
    ಸಙ್ಘ arg nl_arg[NL_ARGMAX + 1];
    ಪೂರ್ಣಾಙ್ಕ ret;

    /* validate format string */
    va_copy(ap2, ap);
    ಯದಿ (printf_core(ರಿಕ್ತ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        ಪ್ರತ್ಯಾಗಮ -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

ಪೂರ್ಣಾಙ್ಕ impl_kvprintf(ಧ್ರುವ ವರ್ಣ *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .ಲಿಖ  = do_output_to_putchar,
        .buf    = ರಿಕ್ತ,
        .maxlen = 0,
        .used   = 0
    };

    ಪ್ರತ್ಯಾಗಮ vprintf(&out_wrap, format, ap);
}

ಪೂರ್ಣಾಙ್ಕ impl_ksnvprintf(ವರ್ಣ *str, word_t size, ಧ್ರುವ ವರ್ಣ *format, va_list ap)
{
    ಯದಿ (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .ಲಿಖ  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    ಪೂರ್ಣಾಙ್ಕ ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ಯದಿ ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

#endif /* CONFIG_PRINTING */
