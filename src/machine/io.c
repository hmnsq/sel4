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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಹೊರಹೋಗು_wrap_t  ಹೊರಹೋಗು_wrap_t_2;

/* handler defining how/where to actually output a buffer */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*ಹೊರಹೋಗು_write_fn)(ಹೊರಹೋಗು_wrap_t_2 *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, ಶಬ್ದ_t len);

ರಚನೆ ಹೊರಹೋಗು_wrap_t {
    ಸ್ಥಿರಾಂಕ ಹೊರಹೋಗು_write_fn ಲಿಖ;
    ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ buf;
    ಸ್ಥಿರಾಂಕ ಶಬ್ದ_t maxlen;
    ಶಬ್ದ_t ಬಳಸಿದ_2;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ಹೊರಹೋಗು(ಹೊರಹೋಗು_wrap_t_2 *ಹೊರಹೋಗು_wrap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, ಶಬ್ದ_t len)
{
    ಆದರೆ (ಹೊರಹೋಗು_wrap) {
        ಹೊರಹೋಗು_wrap->ಲಿಖ(ಹೊರಹೋಗು_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ಸ್ಥಿರ ಶೂನ್ಯ do_output_ಗಮ್ಯ_putchar(
    ಬಳಸದ ಹೊರಹೋಗು_wrap_t_2 *ಹೊರಹೋಗು,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
    ಶಬ್ದ_t len)
{
    ಆದರೆ (buf) {
        ಆಗ (len-- > 0) {
            ಅಕ್ಷರ_ಬರೆ(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ಸ್ಥಿರ ಶೂನ್ಯ do_output_ಗಮ್ಯ_ಬಫರ್(
    ಹೊರಹೋಗು_wrap_t_2 *ಹೊರಹೋಗು,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
    ಶಬ್ದ_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ಆದರೆ (buf && (ಹೊರಹೋಗು->ಬಳಸಿದ_2 < ಹೊರಹೋಗು->maxlen)) {
        /* there is still space in the buffer*/
        ಶಬ್ದ_t ಬಿಡುಗಡೆ = ಹೊರಹೋಗು->maxlen - ಹೊರಹೋಗು->ಬಳಸಿದ_2;
        ಆದರೆ (len > ಬಿಡುಗಡೆ) {
            len = ಬಿಡುಗಡೆ;
        }
        ಸ್ಮರಣೆ_ನಕಲು(&ಹೊರಹೋಗು->buf[ಹೊರಹೋಗು->ಬಳಸಿದ_2], buf, len);
        ಹೊರಹೋಗು->ಬಳಸಿದ_2 += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ಅಂಕಿಯೇ(ಅಕ್ಷರ c)
{
    ಹಿಂತಿರುಗಿ c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_TYPE(a) (1U<<( a -' '))

#ವ್ಯಾಖ್ಯಾನಿಸು ALT_FORM     (1U<<('#'-' '))
#ವ್ಯಾಖ್ಯಾನಿಸು ZERO_PAD     (1U<<('0'-' '))
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಡ_ADJ     (1U<<('-'-' '))
#ವ್ಯಾಖ್ಯಾನಿಸು PAD_POS      (1U<<(' '-' '))
#ವ್ಯಾಖ್ಯಾನಿಸು MARK_POS     (1U<<('+'-' '))
#ವ್ಯಾಖ್ಯಾನಿಸು GROUPED      (1U<<('\''-' '))

#ವ್ಯಾಖ್ಯಾನಿಸು FLAGMASK (ALT_FORM|ZERO_PAD|ಎಡ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

ಎಣಿಕೆ {
    BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
    ZTPRE, JPRE,
    STOP,
    PTR, INT, UINT, ULLONG,
    LONG, ULONG,
    SHORT, USHORT, CHAR, UCHAR,
    WORDT, LLONG,
#ವ್ಯಾಖ್ಯಾನಿಸು IMAX LLONG
#ವ್ಯಾಖ್ಯಾನಿಸು UMAX ULLONG
#ವ್ಯಾಖ್ಯಾನಿಸು PDIFF LONG
#ವ್ಯಾಖ್ಯಾನಿಸು UIPTR ULONG
    NOARG,
    MAXSTATE
};

#ವ್ಯಾಖ್ಯಾನಿಸು S(x) [(x)-'A']

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಸ್ಥಿತಿಗಳು[]['z' - 'A' + 1] = {
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

#ವ್ಯಾಖ್ಯಾನಿಸು OOB(x) ((ಚಿಹ್ನೆರಹಿತ)(x)-'A' > 'z'-'A')
#ವ್ಯಾಖ್ಯಾನಿಸು DIGIT(c) (c - '0')

ಒಕ್ಕೂಟ arg {
    uintmax_t i;
    ದೀರ್ಘ ದ್ವಿಗುಣ f;
    ಶೂನ್ಯ *p;
};

ಸ್ಥಿರ ಶೂನ್ಯ ತೆಗೆ_arg(ಒಕ್ಕೂಟ arg *arg, ಪೂರ್ಣಾಂಕ type, va_list *ap)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ PTR:
        arg->p = va_arg(*ap, ಶೂನ್ಯ *);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ INT:
        arg->i = va_arg(*ap, ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UINT:
        arg->i = va_arg(*ap, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ LONG:
        arg->i = va_arg(*ap, ದೀರ್ಘ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ULONG:
        arg->i = va_arg(*ap, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ LLONG:
        arg->i = va_arg(*ap, ದೀರ್ಘ ದೀರ್ಘ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ULLONG:
        arg->i = va_arg(*ap, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SHORT:
        arg->i = (ಗಿಡ್ಡ)va_arg(*ap, ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ USHORT:
        arg->i = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)va_arg(*ap, ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CHAR:
        arg->i = (ಚಿಹ್ನೆಯುಳ್ಳ ಅಕ್ಷರ)va_arg(*ap, ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UCHAR:
        arg->i = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)va_arg(*ap, ಪೂರ್ಣಾಂಕ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ WORDT:
        arg->i = va_arg(*ap, ಶಬ್ದ_t);
    }
}


ಸ್ಥಿರ ಶೂನ್ಯ pad(ಹೊರಹೋಗು_wrap_t_2 *f, ಅಕ್ಷರ c, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ l, ಪೂರ್ಣಾಂಕ fl)
{
    ಅಕ್ಷರ pad[32]; /* good enough for what the kernel prints */
    ಆದರೆ (fl & (ಎಡ_ADJ | ZERO_PAD) || l >= w) {
        ಹಿಂತಿರುಗಿ;
    }
    l = w - l;
    memset(pad, c, l > ಗಾತ್ರ(pad) ? ಗಾತ್ರ(pad) : l);
    ಗಾಗಿ (; l >= ಗಾತ್ರ(pad); l -= ಗಾತ್ರ(pad)) {
        ಹೊರಹೋಗು(f, pad, ಗಾತ್ರ(pad));
    }
    ಹೊರಹೋಗು(f, pad, l);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ xdigits[16] = {
    "0123456789ABCDEF"
};

ಸ್ಥಿರ ಅಕ್ಷರ *fmt_x(uintmax_t x, ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ ಅಧೋ)
{
    ಗಾಗಿ (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | ಅಧೋ;
    }
    ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ ಅಕ್ಷರ *fmt_o(uintmax_t x, ಅಕ್ಷರ *s)
{
    ಗಾಗಿ (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ ಅಕ್ಷರ *fmt_u(uintmax_t x, ಅಕ್ಷರ *s)
{
    ಆಗ (0 != x) {
#ಆದರೆ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ಆದರೆ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ಪ್ರತಿಜ್ಞಾ(rem <= 9); /* there must be no rounding error now */
        }
#ಇಲ್ಲದಿದ್ದರೆ
        uintmax_t q = x / 10;
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ rem = x % 10;
#ಆದರೆ_ಅಂತ್ಯ
        *--s = '0' + rem;
        x = q;
    }
    ಹಿಂತಿರುಗಿ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LDBL_MANT_DIG 113

#ವ್ಯಾಖ್ಯಾನಿಸು NL_ARGMAX 9

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ getint(ಅಕ್ಷರ **s)
{
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 0; ಅಂಕಿಯೇ(**s); (*s)++) {
        ಆದರೆ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            i = 10 * i + DIGIT(**s);
        }
    }
    ಹಿಂತಿರುಗಿ i;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ printf_core(ಹೊರಹೋಗು_wrap_t_2 *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list *ap, ಒಕ್ಕೂಟ arg *nl_arg, ಪೂರ್ಣಾಂಕ *nl_type)
{
    ಅಕ್ಷರ *a, *z, *s = (ಅಕ್ಷರ *)fmt;
    ಚಿಹ್ನೆರಹಿತ l10n = 0, fl;
    ಪೂರ್ಣಾಂಕ w, p, xp;
    ಒಕ್ಕೂಟ arg arg;
    ಪೂರ್ಣಾಂಕ argpos;
    ಚಿಹ್ನೆರಹಿತ st, ps;
    ಪೂರ್ಣಾಂಕ cnt = 0, l = 0;
    ಅಕ್ಷರ buf[ಗಾತ್ರ(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ;
    ಪೂರ್ಣಾಂಕ t, pl;

    ಗಾಗಿ (;;) {
        ಆದರೆ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ಆದರೆ (!*s) {
            ನಿಲ್ಲಿಸು;
        }

        /* Handle literal text and %% format specifiers */
        ಗಾಗಿ (a = s; *s && *s != '%'; s++);
        ಗಾಗಿ (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ಆದರೆ (z - a > INTMAX_MAX - cnt) {
            ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
        }
        l = z - a;
        ಹೊರಹೋಗು(f, a, l);
        ಆದರೆ (l) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (ಅಂಕಿಯೇ(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ಇಲ್ಲದಿದ್ದರೆ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        ಗಾಗಿ (fl = 0; (ಚಿಹ್ನೆರಹಿತ)*s - ' ' < 32 && (FLAGMASK & ಮುಖವಾಡ_TYPE(*s)); s++) {
            fl |= ಮುಖವಾಡ_TYPE(*s);
        }

        /* Read field width */
        ಆದರೆ (*s == '*') {
            ಆದರೆ (ಅಂಕಿಯೇ(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!l10n) {
                w = f ? va_arg(*ap, ಪೂರ್ಣಾಂಕ) : 0;
                s++;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ -1; /* invalid */
            }
            ಆದರೆ (w < 0) {
                fl |= ಎಡ_ADJ;
                w = -w;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((w = getint(&s)) < 0) {
            ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
        }

        /* Read precision */
        ಆದರೆ (*s == '.' && s[1] == '*') {
            ಆದರೆ (ಅಂಕಿಯೇ(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!l10n) {
                p = f ? va_arg(*ap, ಪೂರ್ಣಾಂಕ) : 0;
                s += 2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ -1;/* invalid */
            }
            xp = (p >= 0);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        ಮಾಡು {
            ಆದರೆ (OOB(*s)) {
                ಹಿಂತಿರುಗಿ -1; /* invalid */
            }
            ps = st;
            st = ಸ್ಥಿತಿಗಳು[st]S(*s++);
        } ಆಗ (st - 1 < STOP);
        ಆದರೆ (!st) {
            ಹಿಂತಿರುಗಿ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ಆದರೆ (st == NOARG) {
            ಆದರೆ (argpos >= 0) {
                ಹಿಂತಿರುಗಿ -1; /* invalid */
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f) {
                ತೆಗೆ_arg(&arg, st, ap);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ 0;
            }
        }

        ಆದರೆ (!f) {
            ಮುಂದುವರಿ;
        }

        z = buf + ಗಾತ್ರ(buf);
        ಪೂರ್ವಪ್ರತ್ಯಯ = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ಆದರೆ (fl & ಎಡ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ಆದರೆ (t == 'n') {
            ಆದರೆ (!arg.p) {
                ಮುಂದುವರಿ;
            }
            ಆಯ್ಕೆ (ps) {
            ಸ್ಥಿತಿ BARE:
                *(ಪೂರ್ಣಾಂಕ *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ LPRE:
                *(ದೀರ್ಘ *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ LLPRE:
                *(ದೀರ್ಘ ದೀರ್ಘ *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ HPRE:
                *(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ HHPRE:
                *(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ ZTPRE:
                *(ಶಬ್ದ_t *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ JPRE:
                *(ಶಬ್ದ_t *)arg.p = cnt;
                ನಿಲ್ಲಿಸು;
            }
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ಆದರೆ (p < 0 && *z) {
                ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಯ್ಕೆ (t) {
            ಸ್ಥಿತಿ 'p':
                p = MAX(p, 2 * ಗಾತ್ರ(ಶೂನ್ಯ *));
                t = 'x';
                fl |= ALT_FORM;
            ಸ್ಥಿತಿ 'x':
            ಸ್ಥಿತಿ 'X':
                a = fmt_x(arg.i, z, t & 32);
                ಆದರೆ (arg.i && (fl & ALT_FORM)) {
                    ಪೂರ್ವಪ್ರತ್ಯಯ += (t >> 4);
                    pl = 2;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 'o':
                a = fmt_o(arg.i, z);
                ಆದರೆ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 'd':
            ಸ್ಥಿತಿ 'i':
                pl = 1;
                ಆದರೆ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fl & MARK_POS) {
                    ಪೂರ್ವಪ್ರತ್ಯಯ++;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fl & PAD_POS) {
                    ಪೂರ್ವಪ್ರತ್ಯಯ += 2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    pl = 0;
                }
            ಸ್ಥಿತಿ 'u':
                a = fmt_u(arg.i, z);
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (xp && p < 0) {
                ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
            }
            ಆದರೆ (xp) {
                fl &= ~ZERO_PAD;
            }
            ಆದರೆ (!arg.i && !p) {
                a = z;
            } ಇಲ್ಲದಿದ್ದರೆ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ಆದರೆ (p < z - a) {
            p = z - a;
        }
        ಆದರೆ (p > INTMAX_MAX - pl) {
            ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
        }
        ಆದರೆ (w < pl + p) {
            w = pl + p;
        }
        ಆದರೆ (w > INTMAX_MAX - cnt) {
            ಹಿಂತಿರುಗಿ -1; /*ಅತಿಪ್ರವಾಹ*/
        }

        pad(f, ' ', w, pl + p, fl);
        ಹೊರಹೋಗು(f, ಪೂರ್ವಪ್ರತ್ಯಯ, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        ಹೊರಹೋಗು(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ಎಡ_ADJ);

        l = w;
    }

    ಆದರೆ (f) {
        ಹಿಂತಿರುಗಿ cnt;
    }
    ಆದರೆ (!l10n) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        ತೆಗೆ_arg(nl_arg + i, nl_type[i], ap);
    }
    ಗಾಗಿ (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ಆದರೆ (i <= NL_ARGMAX) {
        ಹಿಂತಿರುಗಿ -1; /* invalid */
    }

    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vprintf(ಹೊರಹೋಗು_wrap_t_2 *ಹೊರಹೋಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap)
{
    va_list ap2;
    ಪೂರ್ಣಾಂಕ nl_type[NL_ARGMAX + 1] = {0};
    ಒಕ್ಕೂಟ arg nl_arg[NL_ARGMAX + 1];
    ಪೂರ್ಣಾಂಕ ret;

    /* validate format string */
    va_copy(ap2, ap);
    ಆದರೆ (printf_core(ಖಾಲಿ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_ಕೊನೆ(ap2);
        ಹಿಂತಿರುಗಿ -1;
    }

    ret = printf_core(ಹೊರಹೋಗು, fmt, &ap2, nl_arg, nl_type);
    va_ಕೊನೆ(ap2);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

ಪೂರ್ಣಾಂಕ impl_kvprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ap)
{
    ಹೊರಹೋಗು_wrap_t_2 ಹೊರಹೋಗು_wrap =  {
        .ಲಿಖ  = do_output_ಗಮ್ಯ_putchar,
        .buf    = ಖಾಲಿ,
        .maxlen = 0,
        .ಬಳಸಿದ_2   = 0
    };

    ಹಿಂತಿರುಗಿ vprintf(&ಹೊರಹೋಗು_wrap, format, ap);
}

ಪೂರ್ಣಾಂಕ impl_ksnvprintf(ಅಕ್ಷರ *str, ಶಬ್ದ_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ap)
{
    ಆದರೆ (!str) {
        ಗಾತ್ರ_2 = 0;
    }

    ಹೊರಹೋಗು_wrap_t_2 ಹೊರಹೋಗು_wrap =  {
        .ಲಿಖ  = do_output_ಗಮ್ಯ_ಬಫರ್,
        .buf    = str,
        .maxlen = ಗಾತ್ರ_2,
        .ಬಳಸಿದ_2   = 0
    };

    ಪೂರ್ಣಾಂಕ ret = vprintf(&ಹೊರಹೋಗು_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ಆದರೆ ((ret > 0) && (ಗಾತ್ರ_2 > 0)) {
        str[(ret < ಗಾತ್ರ_2) ? ret : ಗಾತ್ರ_2 - 1] = '\0';
    }

    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */
