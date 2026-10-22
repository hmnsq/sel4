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

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਬਾਹਰਲਾ_wrap_t  ਬਾਹਰਲਾ_wrap_t_2;

/* handler defining how/where to actually output a buffer */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*ਬਾਹਰਲਾ_write_fn)(ਬਾਹਰਲਾ_wrap_t_2 *ਬਾਹਰਲਾ, ਅਚਲ ਅੱਖਰ *buf, ਸ਼ਬਦ_t len);

ਬਣਤਰ ਬਾਹਰਲਾ_wrap_t {
    ਅਚਲ ਬਾਹਰਲਾ_write_fn ਲਿਖ;
    ਅੱਖਰ *ਅਚਲ buf;
    ਅਚਲ ਸ਼ਬਦ_t maxlen;
    ਸ਼ਬਦ_t ਵਰਤਿਆ_2;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ਸਥਿਰ ਖਾਲੀ ਬਾਹਰਲਾ(ਬਾਹਰਲਾ_wrap_t_2 *ਬਾਹਰਲਾ_wrap, ਅਚਲ ਅੱਖਰ *buf, ਸ਼ਬਦ_t len)
{
    ਜੇ (ਬਾਹਰਲਾ_wrap) {
        ਬਾਹਰਲਾ_wrap->ਲਿਖ(ਬਾਹਰਲਾ_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ਸਥਿਰ ਖਾਲੀ do_output_ਮੰਜ਼ਿਲ_putchar(
    ਅਣਵਰਤਿਆ ਬਾਹਰਲਾ_wrap_t_2 *ਬਾਹਰਲਾ,
    ਅਚਲ ਅੱਖਰ *buf,
    ਸ਼ਬਦ_t len)
{
    ਜੇ (buf) {
        ਜਦੋਂ (len-- > 0) {
            ਅੱਖਰ_ਲਿਖੋ(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ਸਥਿਰ ਖਾਲੀ do_output_ਮੰਜ਼ਿਲ_ਬਫਰ(
    ਬਾਹਰਲਾ_wrap_t_2 *ਬਾਹਰਲਾ,
    ਅਚਲ ਅੱਖਰ *buf,
    ਸ਼ਬਦ_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ਜੇ (buf && (ਬਾਹਰਲਾ->ਵਰਤਿਆ_2 < ਬਾਹਰਲਾ->maxlen)) {
        /* there is still space in the buffer*/
        ਸ਼ਬਦ_t ਮੁਕਤ = ਬਾਹਰਲਾ->maxlen - ਬਾਹਰਲਾ->ਵਰਤਿਆ_2;
        ਜੇ (len > ਮੁਕਤ) {
            len = ਮੁਕਤ;
        }
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਬਾਹਰਲਾ->buf[ਬਾਹਰਲਾ->ਵਰਤਿਆ_2], buf, len);
        ਬਾਹਰਲਾ->ਵਰਤਿਆ_2 += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਅੰਕ_ਹੈ(ਅੱਖਰ c)
{
    ਵਾਪਸ c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_TYPE(a) (1U<<( a -' '))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALT_FORM     (1U<<('#'-' '))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZERO_PAD     (1U<<('0'-' '))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੱਬਾ_ADJ     (1U<<('-'-' '))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAD_POS      (1U<<(' '-' '))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MARK_POS     (1U<<('+'-' '))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GROUPED      (1U<<('\''-' '))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLAGMASK (ALT_FORM|ZERO_PAD|ਖੱਬਾ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

ਗਿਣਤੀ {
    BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
    ZTPRE, JPRE,
    STOP,
    PTR, INT, UINT, ULLONG,
    LONG, ULONG,
    SHORT, USHORT, CHAR, UCHAR,
    WORDT, LLONG,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAX LLONG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMAX ULLONG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PDIFF LONG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UIPTR ULONG
    NOARG,
    MAXSTATE
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ S(x) [(x)-'A']

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਹਾਲਤਾਂ[]['z' - 'A' + 1] = {
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OOB(x) ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(x)-'A' > 'z'-'A')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGIT(c) (c - '0')

ਸੰਘ arg {
    uintmax_t i;
    ਲੰਮਾ ਦੁੱਗਣਾ f;
    ਖਾਲੀ *p;
};

ਸਥਿਰ ਖਾਲੀ ਕੱਢੋ_arg(ਸੰਘ arg *arg, ਪੂਰਨ type, va_list *ap)
{
    ਚੋਣ (type) {
    ਹਾਲਤ PTR:
        arg->p = va_arg(*ap, ਖਾਲੀ *);
        ਰੋਕੋ;
    ਹਾਲਤ INT:
        arg->i = va_arg(*ap, ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ UINT:
        arg->i = va_arg(*ap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ LONG:
        arg->i = va_arg(*ap, ਲੰਮਾ);
        ਰੋਕੋ;
    ਹਾਲਤ ULONG:
        arg->i = va_arg(*ap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
        ਰੋਕੋ;
    ਹਾਲਤ LLONG:
        arg->i = va_arg(*ap, ਲੰਮਾ ਲੰਮਾ);
        ਰੋਕੋ;
    ਹਾਲਤ ULLONG:
        arg->i = va_arg(*ap, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ);
        ਰੋਕੋ;
    ਹਾਲਤ SHORT:
        arg->i = (ਛੋਟਾ)va_arg(*ap, ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ USHORT:
        arg->i = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)va_arg(*ap, ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ CHAR:
        arg->i = (ਨਿਸ਼ਾਨੀਵਾਲਾ ਅੱਖਰ)va_arg(*ap, ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ UCHAR:
        arg->i = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)va_arg(*ap, ਪੂਰਨ);
        ਰੋਕੋ;
    ਹਾਲਤ WORDT:
        arg->i = va_arg(*ap, ਸ਼ਬਦ_t);
    }
}


ਸਥਿਰ ਖਾਲੀ pad(ਬਾਹਰਲਾ_wrap_t_2 *f, ਅੱਖਰ c, ਪੂਰਨ w, ਪੂਰਨ l, ਪੂਰਨ fl)
{
    ਅੱਖਰ pad[32]; /* good enough for what the kernel prints */
    ਜੇ (fl & (ਖੱਬਾ_ADJ | ZERO_PAD) || l >= w) {
        ਵਾਪਸ;
    }
    l = w - l;
    memset(pad, c, l > ਆਕਾਰ(pad) ? ਆਕਾਰ(pad) : l);
    ਲਈ (; l >= ਆਕਾਰ(pad); l -= ਆਕਾਰ(pad)) {
        ਬਾਹਰਲਾ(f, pad, ਆਕਾਰ(pad));
    }
    ਬਾਹਰਲਾ(f, pad, l);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ xdigits[16] = {
    "0123456789ABCDEF"
};

ਸਥਿਰ ਅੱਖਰ *fmt_x(uintmax_t x, ਅੱਖਰ *s, ਪੂਰਨ ਹੇਠਲਾ)
{
    ਲਈ (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | ਹੇਠਲਾ;
    }
    ਵਾਪਸ s;
}

ਸਥਿਰ ਅੱਖਰ *fmt_o(uintmax_t x, ਅੱਖਰ *s)
{
    ਲਈ (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ਵਾਪਸ s;
}

ਸਥਿਰ ਅੱਖਰ *fmt_u(uintmax_t x, ਅੱਖਰ *s)
{
    ਜਦੋਂ (0 != x) {
#ਜੇ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ਜੇ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ਪ੍ਰਤਿਜ੍ਞਾ(rem <= 9); /* there must be no rounding error now */
        }
#ਨਹੀਂਤਾਂ
        uintmax_t q = x / 10;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ rem = x % 10;
#ਜੇ_ਅੰਤ
        *--s = '0' + rem;
        x = q;
    }
    ਵਾਪਸ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LDBL_MANT_DIG 113

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NL_ARGMAX 9

ਸਥਿਰ ਪੂਰਨ getint(ਅੱਖਰ **s)
{
    ਪੂਰਨ i;
    ਲਈ (i = 0; ਅੰਕ_ਹੈ(**s); (*s)++) {
        ਜੇ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ਨਹੀਂਤਾਂ {
            i = 10 * i + DIGIT(**s);
        }
    }
    ਵਾਪਸ i;
}

ਸਥਿਰ ਪੂਰਨ printf_core(ਬਾਹਰਲਾ_wrap_t_2 *f, ਅਚਲ ਅੱਖਰ *fmt, va_list *ap, ਸੰਘ arg *nl_arg, ਪੂਰਨ *nl_type)
{
    ਅੱਖਰ *a, *z, *s = (ਅੱਖਰ *)fmt;
    ਬਿਨਾਨਿਸ਼ਾਨੀ l10n = 0, fl;
    ਪੂਰਨ w, p, xp;
    ਸੰਘ arg arg;
    ਪੂਰਨ argpos;
    ਬਿਨਾਨਿਸ਼ਾਨੀ st, ps;
    ਪੂਰਨ cnt = 0, l = 0;
    ਅੱਖਰ buf[ਆਕਾਰ(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ;
    ਪੂਰਨ t, pl;

    ਲਈ (;;) {
        ਜੇ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ਵਾਪਸ -1; /*ਉਛਾਲ*/
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ਜੇ (!*s) {
            ਰੋਕੋ;
        }

        /* Handle literal text and %% format specifiers */
        ਲਈ (a = s; *s && *s != '%'; s++);
        ਲਈ (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ਜੇ (z - a > INTMAX_MAX - cnt) {
            ਵਾਪਸ -1; /*ਉਛਾਲ*/
        }
        l = z - a;
        ਬਾਹਰਲਾ(f, a, l);
        ਜੇ (l) {
            ਜਾਰੀ;
        }

        ਜੇ (ਅੰਕ_ਹੈ(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ਨਹੀਂਤਾਂ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        ਲਈ (fl = 0; (ਬਿਨਾਨਿਸ਼ਾਨੀ)*s - ' ' < 32 && (FLAGMASK & ਨਕਾਬ_TYPE(*s)); s++) {
            fl |= ਨਕਾਬ_TYPE(*s);
        }

        /* Read field width */
        ਜੇ (*s == '*') {
            ਜੇ (ਅੰਕ_ਹੈ(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ਨਹੀਂਤਾਂ ਜੇ (!l10n) {
                w = f ? va_arg(*ap, ਪੂਰਨ) : 0;
                s++;
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ -1; /* invalid */
            }
            ਜੇ (w < 0) {
                fl |= ਖੱਬਾ_ADJ;
                w = -w;
            }
        } ਨਹੀਂਤਾਂ ਜੇ ((w = getint(&s)) < 0) {
            ਵਾਪਸ -1; /*ਉਛਾਲ*/
        }

        /* Read precision */
        ਜੇ (*s == '.' && s[1] == '*') {
            ਜੇ (ਅੰਕ_ਹੈ(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ਨਹੀਂਤਾਂ ਜੇ (!l10n) {
                p = f ? va_arg(*ap, ਪੂਰਨ) : 0;
                s += 2;
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ -1;/* invalid */
            }
            xp = (p >= 0);
        } ਨਹੀਂਤਾਂ ਜੇ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ਨਹੀਂਤਾਂ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        ਕਰੋ {
            ਜੇ (OOB(*s)) {
                ਵਾਪਸ -1; /* invalid */
            }
            ps = st;
            st = ਹਾਲਤਾਂ[st]S(*s++);
        } ਜਦੋਂ (st - 1 < STOP);
        ਜੇ (!st) {
            ਵਾਪਸ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ਜੇ (st == NOARG) {
            ਜੇ (argpos >= 0) {
                ਵਾਪਸ -1; /* invalid */
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ਨਹੀਂਤਾਂ ਜੇ (f) {
                ਕੱਢੋ_arg(&arg, st, ap);
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ 0;
            }
        }

        ਜੇ (!f) {
            ਜਾਰੀ;
        }

        z = buf + ਆਕਾਰ(buf);
        ਅਗੇਤਰ = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ਜੇ (fl & ਖੱਬਾ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ਜੇ (t == 'n') {
            ਜੇ (!arg.p) {
                ਜਾਰੀ;
            }
            ਚੋਣ (ps) {
            ਹਾਲਤ BARE:
                *(ਪੂਰਨ *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ LPRE:
                *(ਲੰਮਾ *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ LLPRE:
                *(ਲੰਮਾ ਲੰਮਾ *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ HPRE:
                *(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ HHPRE:
                *(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ ZTPRE:
                *(ਸ਼ਬਦ_t *)arg.p = cnt;
                ਰੋਕੋ;
            ਹਾਲਤ JPRE:
                *(ਸ਼ਬਦ_t *)arg.p = cnt;
                ਰੋਕੋ;
            }
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ਨਹੀਂਤਾਂ ਜੇ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ਜੇ (p < 0 && *z) {
                ਵਾਪਸ -1; /*ਉਛਾਲ*/
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ਨਹੀਂਤਾਂ {
            ਚੋਣ (t) {
            ਹਾਲਤ 'p':
                p = MAX(p, 2 * ਆਕਾਰ(ਖਾਲੀ *));
                t = 'x';
                fl |= ALT_FORM;
            ਹਾਲਤ 'x':
            ਹਾਲਤ 'X':
                a = fmt_x(arg.i, z, t & 32);
                ਜੇ (arg.i && (fl & ALT_FORM)) {
                    ਅਗੇਤਰ += (t >> 4);
                    pl = 2;
                }
                ਰੋਕੋ;
            ਹਾਲਤ 'o':
                a = fmt_o(arg.i, z);
                ਜੇ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ਰੋਕੋ;
            ਹਾਲਤ 'd':
            ਹਾਲਤ 'i':
                pl = 1;
                ਜੇ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ਨਹੀਂਤਾਂ ਜੇ (fl & MARK_POS) {
                    ਅਗੇਤਰ++;
                } ਨਹੀਂਤਾਂ ਜੇ (fl & PAD_POS) {
                    ਅਗੇਤਰ += 2;
                } ਨਹੀਂਤਾਂ {
                    pl = 0;
                }
            ਹਾਲਤ 'u':
                a = fmt_u(arg.i, z);
                ਰੋਕੋ;
            }
            ਜੇ (xp && p < 0) {
                ਵਾਪਸ -1; /*ਉਛਾਲ*/
            }
            ਜੇ (xp) {
                fl &= ~ZERO_PAD;
            }
            ਜੇ (!arg.i && !p) {
                a = z;
            } ਨਹੀਂਤਾਂ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ਜੇ (p < z - a) {
            p = z - a;
        }
        ਜੇ (p > INTMAX_MAX - pl) {
            ਵਾਪਸ -1; /*ਉਛਾਲ*/
        }
        ਜੇ (w < pl + p) {
            w = pl + p;
        }
        ਜੇ (w > INTMAX_MAX - cnt) {
            ਵਾਪਸ -1; /*ਉਛਾਲ*/
        }

        pad(f, ' ', w, pl + p, fl);
        ਬਾਹਰਲਾ(f, ਅਗੇਤਰ, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        ਬਾਹਰਲਾ(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ਖੱਬਾ_ADJ);

        l = w;
    }

    ਜੇ (f) {
        ਵਾਪਸ cnt;
    }
    ਜੇ (!l10n) {
        ਵਾਪਸ 0;
    }

    ਪੂਰਨ i;
    ਲਈ (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        ਕੱਢੋ_arg(nl_arg + i, nl_type[i], ap);
    }
    ਲਈ (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ਜੇ (i <= NL_ARGMAX) {
        ਵਾਪਸ -1; /* invalid */
    }

    ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ vprintf(ਬਾਹਰਲਾ_wrap_t_2 *ਬਾਹਰਲਾ, ਅਚਲ ਅੱਖਰ *fmt, va_list ap)
{
    va_list ap2;
    ਪੂਰਨ nl_type[NL_ARGMAX + 1] = {0};
    ਸੰਘ arg nl_arg[NL_ARGMAX + 1];
    ਪੂਰਨ ret;

    /* validate format string */
    va_copy(ap2, ap);
    ਜੇ (printf_core(ਨਿਰਾ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_ਸਿਰਾ(ap2);
        ਵਾਪਸ -1;
    }

    ret = printf_core(ਬਾਹਰਲਾ, fmt, &ap2, nl_arg, nl_type);
    va_ਸਿਰਾ(ap2);
    ਵਾਪਸ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

ਪੂਰਨ impl_kvprintf(ਅਚਲ ਅੱਖਰ *format, va_list ap)
{
    ਬਾਹਰਲਾ_wrap_t_2 ਬਾਹਰਲਾ_wrap =  {
        .ਲਿਖ  = do_output_ਮੰਜ਼ਿਲ_putchar,
        .buf    = ਨਿਰਾ,
        .maxlen = 0,
        .ਵਰਤਿਆ_2   = 0
    };

    ਵਾਪਸ vprintf(&ਬਾਹਰਲਾ_wrap, format, ap);
}

ਪੂਰਨ impl_ksnvprintf(ਅੱਖਰ *str, ਸ਼ਬਦ_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *format, va_list ap)
{
    ਜੇ (!str) {
        ਮਾਪ = 0;
    }

    ਬਾਹਰਲਾ_wrap_t_2 ਬਾਹਰਲਾ_wrap =  {
        .ਲਿਖ  = do_output_ਮੰਜ਼ਿਲ_ਬਫਰ,
        .buf    = str,
        .maxlen = ਮਾਪ,
        .ਵਰਤਿਆ_2   = 0
    };

    ਪੂਰਨ ret = vprintf(&ਬਾਹਰਲਾ_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ਜੇ ((ret > 0) && (ਮਾਪ > 0)) {
        str[(ret < ਮਾਪ) ? ret : ਮਾਪ - 1] = '\0';
    }

    ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */
