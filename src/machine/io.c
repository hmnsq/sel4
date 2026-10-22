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

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <machine/io.h>

#ifdef CONFIG_PRINTING

#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

প্রকারনির্দেশ সংরচনা _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
প্রকারনির্দেশ শূন্য (*out_write_fn)(out_wrap_t *out, ধ্রুব বর্ণ *buf, word_t len);

সংরচনা _out_wrap_t {
    ধ্রুব out_write_fn লিখ;
    বর্ণ *ধ্রুব buf;
    ধ্রুব word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
স্থির শূন্য out(out_wrap_t *out_wrap, ধ্রুব বর্ণ *buf, word_t len)
{
    যদি (out_wrap) {
        out_wrap->লিখ(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
স্থির শূন্য do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ধ্রুব বর্ণ *buf,
    word_t len)
{
    যদি (buf) {
        যাবত্ (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
স্থির শূন্য do_output_to_buffer(
    out_wrap_t *out,
    ধ্রুব বর্ণ *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    যদি (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t মুক্ত = out->maxlen - out->used;
        যদি (len > মুক্ত) {
            len = মুক্ত;
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

স্থির অন্তঃস্থ bool_t isdigit(বর্ণ c)
{
    প্রত্যাগম c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define বাম_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|বাম_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

গণনা {
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

স্থির ধ্রুব unsigned বর্ণ states[]['z' - 'A' + 1] = {
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

সঙ্ঘ arg {
    uintmax_t i;
    long দ্বিগুণ f;
    শূন্য *p;
};

স্থির শূন্য বের_করো_arg(সঙ্ঘ arg *arg, পূর্ণাঙ্ক type, va_list *ap)
{
    বিকল্প (type) {
    পক্ষ PTR:
        arg->p = va_arg(*ap, শূন্য *);
        ভঙ্গ;
    পক্ষ INT:
        arg->i = va_arg(*ap, পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ UINT:
        arg->i = va_arg(*ap, unsigned পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ LONG:
        arg->i = va_arg(*ap, long);
        ভঙ্গ;
    পক্ষ ULONG:
        arg->i = va_arg(*ap, unsigned long);
        ভঙ্গ;
    পক্ষ LLONG:
        arg->i = va_arg(*ap, long long);
        ভঙ্গ;
    পক্ষ ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        ভঙ্গ;
    পক্ষ SHORT:
        arg->i = (short)va_arg(*ap, পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ USHORT:
        arg->i = (unsigned short)va_arg(*ap, পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ CHAR:
        arg->i = (signed বর্ণ)va_arg(*ap, পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ UCHAR:
        arg->i = (unsigned বর্ণ)va_arg(*ap, পূর্ণাঙ্ক);
        ভঙ্গ;
    পক্ষ WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


স্থির শূন্য pad(out_wrap_t *f, বর্ণ c, পূর্ণাঙ্ক w, পূর্ণাঙ্ক l, পূর্ণাঙ্ক fl)
{
    বর্ণ pad[32]; /* good enough for what the kernel prints */
    যদি (fl & (বাম_ADJ | ZERO_PAD) || l >= w) {
        প্রত্যাগম;
    }
    l = w - l;
    memset(pad, c, l > পরিমাণ(pad) ? পরিমাণ(pad) : l);
    প্রতি (; l >= পরিমাণ(pad); l -= পরিমাণ(pad)) {
        out(f, pad, পরিমাণ(pad));
    }
    out(f, pad, l);
}

স্থির ধ্রুব বর্ণ xdigits[16] = {
    "0123456789ABCDEF"
};

স্থির বর্ণ *fmt_x(uintmax_t x, বর্ণ *s, পূর্ণাঙ্ক lower)
{
    প্রতি (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    প্রত্যাগম s;
}

স্থির বর্ণ *fmt_o(uintmax_t x, বর্ণ *s)
{
    প্রতি (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    প্রত্যাগম s;
}

স্থির বর্ণ *fmt_u(uintmax_t x, বর্ণ *s)
{
    যাবত্ (0 != x) {
#যদি defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned পূর্ণাঙ্ক rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        যদি (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            প্রতিজ্ঞা(rem <= 9); /* there must be no rounding error now */
        }
#অন্যথা
        uintmax_t q = x / 10;
        unsigned পূর্ণাঙ্ক rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    প্রত্যাগম s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

স্থির পূর্ণাঙ্ক getint(বর্ণ **s)
{
    পূর্ণাঙ্ক i;
    প্রতি (i = 0; isdigit(**s); (*s)++) {
        যদি (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } অন্যথা {
            i = 10 * i + DIGIT(**s);
        }
    }
    প্রত্যাগম i;
}

স্থির পূর্ণাঙ্ক printf_core(out_wrap_t *f, ধ্রুব বর্ণ *fmt, va_list *ap, সঙ্ঘ arg *nl_arg, পূর্ণাঙ্ক *nl_type)
{
    বর্ণ *a, *z, *s = (বর্ণ *)fmt;
    unsigned l10n = 0, fl;
    পূর্ণাঙ্ক w, p, xp;
    সঙ্ঘ arg arg;
    পূর্ণাঙ্ক argpos;
    unsigned st, ps;
    পূর্ণাঙ্ক cnt = 0, l = 0;
    বর্ণ buf[পরিমাণ(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ধ্রুব বর্ণ *prefix;
    পূর্ণাঙ্ক t, pl;

    প্রতি (;;) {
        যদি (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            প্রত্যাগম -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        যদি (!*s) {
            ভঙ্গ;
        }

        /* Handle literal text and %% format specifiers */
        প্রতি (a = s; *s && *s != '%'; s++);
        প্রতি (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        যদি (z - a > INTMAX_MAX - cnt) {
            প্রত্যাগম -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        যদি (l) {
            অনুবর্ত;
        }

        যদি (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } অন্যথা {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        প্রতি (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        যদি (*s == '*') {
            যদি (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } অন্যথা যদি (!l10n) {
                w = f ? va_arg(*ap, পূর্ণাঙ্ক) : 0;
                s++;
            } অন্যথা {
                প্রত্যাগম -1; /* invalid */
            }
            যদি (w < 0) {
                fl |= বাম_ADJ;
                w = -w;
            }
        } অন্যথা যদি ((w = getint(&s)) < 0) {
            প্রত্যাগম -1; /* overflow */
        }

        /* Read precision */
        যদি (*s == '.' && s[1] == '*') {
            যদি (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } অন্যথা যদি (!l10n) {
                p = f ? va_arg(*ap, পূর্ণাঙ্ক) : 0;
                s += 2;
            } অন্যথা {
                প্রত্যাগম -1;/* invalid */
            }
            xp = (p >= 0);
        } অন্যথা যদি (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } অন্যথা {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        কুরু {
            যদি (OOB(*s)) {
                প্রত্যাগম -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } যাবত্ (st - 1 < STOP);
        যদি (!st) {
            প্রত্যাগম -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        যদি (st == NOARG) {
            যদি (argpos >= 0) {
                প্রত্যাগম -1; /* invalid */
            }
        } অন্যথা {
            যদি (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } অন্যথা যদি (f) {
                বের_করো_arg(&arg, st, ap);
            } অন্যথা {
                প্রত্যাগম 0;
            }
        }

        যদি (!f) {
            অনুবর্ত;
        }

        z = buf + পরিমাণ(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        যদি (fl & বাম_ADJ) {
            fl &= ~ZERO_PAD;
        }

        যদি (t == 'n') {
            যদি (!arg.p) {
                অনুবর্ত;
            }
            বিকল্প (ps) {
            পক্ষ BARE:
                *(পূর্ণাঙ্ক *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ LPRE:
                *(long *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ LLPRE:
                *(long long *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ HPRE:
                *(unsigned short *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ HHPRE:
                *(unsigned বর্ণ *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ ZTPRE:
                *(word_t *)arg.p = cnt;
                ভঙ্গ;
            পক্ষ JPRE:
                *(word_t *)arg.p = cnt;
                ভঙ্গ;
            }
            অনুবর্ত;
        } অন্যথা যদি (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } অন্যথা যদি (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            যদি (p < 0 && *z) {
                প্রত্যাগম -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } অন্যথা {
            বিকল্প (t) {
            পক্ষ 'p':
                p = MAX(p, 2 * পরিমাণ(শূন্য *));
                t = 'x';
                fl |= ALT_FORM;
            পক্ষ 'x':
            পক্ষ 'X':
                a = fmt_x(arg.i, z, t & 32);
                যদি (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                ভঙ্গ;
            পক্ষ 'o':
                a = fmt_o(arg.i, z);
                যদি ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ভঙ্গ;
            পক্ষ 'd':
            পক্ষ 'i':
                pl = 1;
                যদি (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } অন্যথা যদি (fl & MARK_POS) {
                    prefix++;
                } অন্যথা যদি (fl & PAD_POS) {
                    prefix += 2;
                } অন্যথা {
                    pl = 0;
                }
            পক্ষ 'u':
                a = fmt_u(arg.i, z);
                ভঙ্গ;
            }
            যদি (xp && p < 0) {
                প্রত্যাগম -1; /* overflow */
            }
            যদি (xp) {
                fl &= ~ZERO_PAD;
            }
            যদি (!arg.i && !p) {
                a = z;
            } অন্যথা {
                p = MAX(p, z - a + !arg.i);
            }
        }

        যদি (p < z - a) {
            p = z - a;
        }
        যদি (p > INTMAX_MAX - pl) {
            প্রত্যাগম -1; /* overflow */
        }
        যদি (w < pl + p) {
            w = pl + p;
        }
        যদি (w > INTMAX_MAX - cnt) {
            প্রত্যাগম -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ বাম_ADJ);

        l = w;
    }

    যদি (f) {
        প্রত্যাগম cnt;
    }
    যদি (!l10n) {
        প্রত্যাগম 0;
    }

    পূর্ণাঙ্ক i;
    প্রতি (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        বের_করো_arg(nl_arg + i, nl_type[i], ap);
    }
    প্রতি (; i <= NL_ARGMAX && !nl_type[i]; i++);
    যদি (i <= NL_ARGMAX) {
        প্রত্যাগম -1; /* invalid */
    }

    প্রত্যাগম 1;
}

স্থির পূর্ণাঙ্ক vprintf(out_wrap_t *out, ধ্রুব বর্ণ *fmt, va_list ap)
{
    va_list ap2;
    পূর্ণাঙ্ক nl_type[NL_ARGMAX + 1] = {0};
    সঙ্ঘ arg nl_arg[NL_ARGMAX + 1];
    পূর্ণাঙ্ক ret;

    /* validate format string */
    va_copy(ap2, ap);
    যদি (printf_core(রিক্ত, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        প্রত্যাগম -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    প্রত্যাগম ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

পূর্ণাঙ্ক impl_kvprintf(ধ্রুব বর্ণ *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .লিখ  = do_output_to_putchar,
        .buf    = রিক্ত,
        .maxlen = 0,
        .used   = 0
    };

    প্রত্যাগম vprintf(&out_wrap, format, ap);
}

পূর্ণাঙ্ক impl_ksnvprintf(বর্ণ *str, word_t size, ধ্রুব বর্ণ *format, va_list ap)
{
    যদি (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .লিখ  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    পূর্ণাঙ্ক ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    যদি ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    প্রত্যাগম ret;
}

#endif /* CONFIG_PRINTING */
