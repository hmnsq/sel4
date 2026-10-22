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

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>

#ifdef CONFIG_PRINTING

#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*out_write_fn)(out_wrap_t *out, ଅଚଳ ଅକ୍ଷର *buf, word_t len);

ସଂରଚନା _out_wrap_t {
    ଅଚଳ out_write_fn ଲିଖ;
    ଅକ୍ଷର *ଅଚଳ buf;
    ଅଚଳ word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ସ୍ଥିର ଶୂନ୍ୟ out(out_wrap_t *out_wrap, ଅଚଳ ଅକ୍ଷର *buf, word_t len)
{
    ଯଦି (out_wrap) {
        out_wrap->ଲିଖ(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ସ୍ଥିର ଶୂନ୍ୟ do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ଅଚଳ ଅକ୍ଷର *buf,
    word_t len)
{
    ଯଦି (buf) {
        ଯେତେବେଳେ (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ସ୍ଥିର ଶୂନ୍ୟ do_output_to_buffer(
    out_wrap_t *out,
    ଅଚଳ ଅକ୍ଷର *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ଯଦି (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t ମୁକ୍ତ = out->maxlen - out->used;
        ଯଦି (len > ମୁକ୍ତ) {
            len = ମୁକ୍ତ;
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

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t isdigit(ଅକ୍ଷର c)
{
    ଫେରନ୍ତୁ c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define ବାମ_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|ବାମ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

ଗଣନା {
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

ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର states[]['z' - 'A' + 1] = {
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

ସଂଘ arg {
    uintmax_t i;
    long ଦ୍ୱିଗୁଣ f;
    ଶୂନ୍ୟ *p;
};

ସ୍ଥିର ଶୂନ୍ୟ କାଢ଼_arg(ସଂଘ arg *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, va_list *ap)
{
    ବାଛ (type) {
    ସ୍ଥିତି PTR:
        arg->p = va_arg(*ap, ଶୂନ୍ୟ *);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି INT:
        arg->i = va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UINT:
        arg->i = va_arg(*ap, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି LONG:
        arg->i = va_arg(*ap, long);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ULONG:
        arg->i = va_arg(*ap, unsigned long);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି LLONG:
        arg->i = va_arg(*ap, long long);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି SHORT:
        arg->i = (short)va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି USHORT:
        arg->i = (unsigned short)va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି CHAR:
        arg->i = (signed ଅକ୍ଷର)va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UCHAR:
        arg->i = (unsigned ଅକ୍ଷର)va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


ସ୍ଥିର ଶୂନ୍ୟ pad(out_wrap_t *f, ଅକ୍ଷର c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fl)
{
    ଅକ୍ଷର pad[32]; /* good enough for what the kernel prints */
    ଯଦି (fl & (ବାମ_ADJ | ZERO_PAD) || l >= w) {
        ଫେରନ୍ତୁ;
    }
    l = w - l;
    memset(pad, c, l > ଆକାର(pad) ? ଆକାର(pad) : l);
    ପାଇଁ (; l >= ଆକାର(pad); l -= ଆକାର(pad)) {
        out(f, pad, ଆକାର(pad));
    }
    out(f, pad, l);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର xdigits[16] = {
    "0123456789ABCDEF"
};

ସ୍ଥିର ଅକ୍ଷର *fmt_x(uintmax_t x, ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lower)
{
    ପାଇଁ (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    ଫେରନ୍ତୁ s;
}

ସ୍ଥିର ଅକ୍ଷର *fmt_o(uintmax_t x, ଅକ୍ଷର *s)
{
    ପାଇଁ (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    ଫେରନ୍ତୁ s;
}

ସ୍ଥିର ଅକ୍ଷର *fmt_u(uintmax_t x, ଅକ୍ଷର *s)
{
    ଯେତେବେଳେ (0 != x) {
#ଯଦି defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ଯଦି (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ପ୍ରତିଜ୍ଞା(rem <= 9); /* there must be no rounding error now */
        }
#ଅନ୍ୟଥା
        uintmax_t q = x / 10;
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    ଫେରନ୍ତୁ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା getint(ଅକ୍ଷର **s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 0; isdigit(**s); (*s)++) {
        ଯଦି (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ଅନ୍ୟଥା {
            i = 10 * i + DIGIT(**s);
        }
    }
    ଫେରନ୍ତୁ i;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା printf_core(out_wrap_t *f, ଅଚଳ ଅକ୍ଷର *fmt, va_list *ap, ସଂଘ arg *nl_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *nl_type)
{
    ଅକ୍ଷର *a, *z, *s = (ଅକ୍ଷର *)fmt;
    unsigned l10n = 0, fl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, p, xp;
    ସଂଘ arg arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argpos;
    unsigned st, ps;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = 0, l = 0;
    ଅକ୍ଷର buf[ଆକାର(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ଅଚଳ ଅକ୍ଷର *prefix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା t, pl;

    ପାଇଁ (;;) {
        ଯଦି (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            ଫେରନ୍ତୁ -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ଯଦି (!*s) {
            ଭାଙ୍ଗ;
        }

        /* Handle literal text and %% format specifiers */
        ପାଇଁ (a = s; *s && *s != '%'; s++);
        ପାଇଁ (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ଯଦି (z - a > INTMAX_MAX - cnt) {
            ଫେରନ୍ତୁ -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        ଯଦି (l) {
            ଜାରି;
        }

        ଯଦି (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ଅନ୍ୟଥା {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        ପାଇଁ (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        ଯଦି (*s == '*') {
            ଯଦି (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ଅନ୍ୟଥା ଯଦି (!l10n) {
                w = f ? va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) : 0;
                s++;
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ -1; /* invalid */
            }
            ଯଦି (w < 0) {
                fl |= ବାମ_ADJ;
                w = -w;
            }
        } ଅନ୍ୟଥା ଯଦି ((w = getint(&s)) < 0) {
            ଫେରନ୍ତୁ -1; /* overflow */
        }

        /* Read precision */
        ଯଦି (*s == '.' && s[1] == '*') {
            ଯଦି (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ଅନ୍ୟଥା ଯଦି (!l10n) {
                p = f ? va_arg(*ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) : 0;
                s += 2;
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ -1;/* invalid */
            }
            xp = (p >= 0);
        } ଅନ୍ୟଥା ଯଦି (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ଅନ୍ୟଥା {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        କର {
            ଯଦି (OOB(*s)) {
                ଫେରନ୍ତୁ -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } ଯେତେବେଳେ (st - 1 < STOP);
        ଯଦି (!st) {
            ଫେରନ୍ତୁ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ଯଦି (st == NOARG) {
            ଯଦି (argpos >= 0) {
                ଫେରନ୍ତୁ -1; /* invalid */
            }
        } ଅନ୍ୟଥା {
            ଯଦି (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ଅନ୍ୟଥା ଯଦି (f) {
                କାଢ଼_arg(&arg, st, ap);
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ 0;
            }
        }

        ଯଦି (!f) {
            ଜାରି;
        }

        z = buf + ଆକାର(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ଯଦି (fl & ବାମ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ଯଦି (t == 'n') {
            ଯଦି (!arg.p) {
                ଜାରି;
            }
            ବାଛ (ps) {
            ସ୍ଥିତି BARE:
                *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି LPRE:
                *(long *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି LLPRE:
                *(long long *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି HPRE:
                *(unsigned short *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି HHPRE:
                *(unsigned ଅକ୍ଷର *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି ZTPRE:
                *(word_t *)arg.p = cnt;
                ଭାଙ୍ଗ;
            ସ୍ଥିତି JPRE:
                *(word_t *)arg.p = cnt;
                ଭାଙ୍ଗ;
            }
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ଅନ୍ୟଥା ଯଦି (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ଯଦି (p < 0 && *z) {
                ଫେରନ୍ତୁ -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ଅନ୍ୟଥା {
            ବାଛ (t) {
            ସ୍ଥିତି 'p':
                p = MAX(p, 2 * ଆକାର(ଶୂନ୍ୟ *));
                t = 'x';
                fl |= ALT_FORM;
            ସ୍ଥିତି 'x':
            ସ୍ଥିତି 'X':
                a = fmt_x(arg.i, z, t & 32);
                ଯଦି (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି 'o':
                a = fmt_o(arg.i, z);
                ଯଦି ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି 'd':
            ସ୍ଥିତି 'i':
                pl = 1;
                ଯଦି (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ଅନ୍ୟଥା ଯଦି (fl & MARK_POS) {
                    prefix++;
                } ଅନ୍ୟଥା ଯଦି (fl & PAD_POS) {
                    prefix += 2;
                } ଅନ୍ୟଥା {
                    pl = 0;
                }
            ସ୍ଥିତି 'u':
                a = fmt_u(arg.i, z);
                ଭାଙ୍ଗ;
            }
            ଯଦି (xp && p < 0) {
                ଫେରନ୍ତୁ -1; /* overflow */
            }
            ଯଦି (xp) {
                fl &= ~ZERO_PAD;
            }
            ଯଦି (!arg.i && !p) {
                a = z;
            } ଅନ୍ୟଥା {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ଯଦି (p < z - a) {
            p = z - a;
        }
        ଯଦି (p > INTMAX_MAX - pl) {
            ଫେରନ୍ତୁ -1; /* overflow */
        }
        ଯଦି (w < pl + p) {
            w = pl + p;
        }
        ଯଦି (w > INTMAX_MAX - cnt) {
            ଫେରନ୍ତୁ -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ବାମ_ADJ);

        l = w;
    }

    ଯଦି (f) {
        ଫେରନ୍ତୁ cnt;
    }
    ଯଦି (!l10n) {
        ଫେରନ୍ତୁ 0;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        କାଢ଼_arg(nl_arg + i, nl_type[i], ap);
    }
    ପାଇଁ (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ଯଦି (i <= NL_ARGMAX) {
        ଫେରନ୍ତୁ -1; /* invalid */
    }

    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vprintf(out_wrap_t *out, ଅଚଳ ଅକ୍ଷର *fmt, va_list ap)
{
    va_list ap2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nl_type[NL_ARGMAX + 1] = {0};
    ସଂଘ arg nl_arg[NL_ARGMAX + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    /* validate format string */
    va_copy(ap2, ap);
    ଯଦି (printf_core(ଖାଲି, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        ଫେରନ୍ତୁ -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    ଫେରନ୍ତୁ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା impl_kvprintf(ଅଚଳ ଅକ୍ଷର *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .ଲିଖ  = do_output_to_putchar,
        .buf    = ଖାଲି,
        .maxlen = 0,
        .used   = 0
    };

    ଫେରନ୍ତୁ vprintf(&out_wrap, format, ap);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା impl_ksnvprintf(ଅକ୍ଷର *str, word_t size, ଅଚଳ ଅକ୍ଷର *format, va_list ap)
{
    ଯଦି (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .ଲିଖ  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ଯଦି ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    ଫେରନ୍ତୁ ret;
}

#endif /* CONFIG_PRINTING */
