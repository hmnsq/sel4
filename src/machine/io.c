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

#جے_تعریف_شدہ CONFIG_PRINTING

#ضمن <stdarg.h>
#ضمن <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

تعريفنوع بنية باہرلا_wrap_t  باہرلا_wrap_t_2;

/* handler defining how/where to actually output a buffer */
تعريفنوع فراغ (*باہرلا_write_fn)(باہرلا_wrap_t_2 *باہرلا, ثابت حرف *buf, لفظ_t len);

بنية باہرلا_wrap_t {
    ثابت باہرلا_write_fn اكتب;
    حرف *ثابت buf;
    ثابت لفظ_t maxlen;
    لفظ_t ورتیا_2;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ساكن فراغ باہرلا(باہرلا_wrap_t_2 *باہرلا_wrap, ثابت حرف *buf, لفظ_t len)
{
    إذا (باہرلا_wrap) {
        باہرلا_wrap->اكتب(باہرلا_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساكن فراغ do_output_منزل_putchar(
    بن_ورتیا باہرلا_wrap_t_2 *باہرلا,
    ثابت حرف *buf,
    لفظ_t len)
{
    إذا (buf) {
        طالما (len-- > 0) {
            حرف_لکھو(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساكن فراغ do_output_منزل_بفر(
    باہرلا_wrap_t_2 *باہرلا,
    ثابت حرف *buf,
    لفظ_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    إذا (buf && (باہرلا->ورتیا_2 < باہرلا->maxlen)) {
        /* there is still space in the buffer*/
        لفظ_t حرر = باہرلا->maxlen - باہرلا->ورتیا_2;
        إذا (len > حرر) {
            len = حرر;
        }
        حافظہ_نقل(&باہرلا->buf[باہرلا->ورتیا_2], buf, len);
        باہرلا->ورتیا_2 += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

ساكن مضمن bool_t ہندسہ_اے(حرف c)
{
    أرجع c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#تعریف_کرو نقاب_TYPE(a) (1U<<( a -' '))

#تعریف_کرو ALT_FORM     (1U<<('#'-' '))
#تعریف_کرو ZERO_PAD     (1U<<('0'-' '))
#تعریف_کرو بایاں_ADJ     (1U<<('-'-' '))
#تعریف_کرو PAD_POS      (1U<<(' '-' '))
#تعریف_کرو MARK_POS     (1U<<('+'-' '))
#تعریف_کرو GROUPED      (1U<<('\''-' '))

#تعریف_کرو FLAGMASK (ALT_FORM|ZERO_PAD|بایاں_ADJ|PAD_POS|MARK_POS|GROUPED)

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
#تعریف_کرو IMAX LLONG
#تعریف_کرو UMAX ULLONG
#تعریف_کرو PDIFF LONG
#تعریف_کرو UIPTR ULONG
    NOARG,
    MAXSTATE
};

#تعریف_کرو S(x) [(x)-'A']

ساكن ثابت بنانشانی حرف حالتاں[]['z' - 'A' + 1] = {
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

#تعریف_کرو OOB(x) ((بنانشانی)(x)-'A' > 'z'-'A')
#تعریف_کرو DIGIT(c) (c - '0')

اتحاد arg {
    uintmax_t i;
    لما مزدوج f;
    فراغ *p;
};

ساكن فراغ نکالو_arg(اتحاد arg *arg, صحيح type, va_list *ap)
{
    بدل (type) {
    حالة PTR:
        arg->p = va_arg(*ap, فراغ *);
        اكسر;
    حالة INT:
        arg->i = va_arg(*ap, صحيح);
        اكسر;
    حالة UINT:
        arg->i = va_arg(*ap, بنانشانی صحيح);
        اكسر;
    حالة LONG:
        arg->i = va_arg(*ap, لما);
        اكسر;
    حالة ULONG:
        arg->i = va_arg(*ap, بنانشانی لما);
        اكسر;
    حالة LLONG:
        arg->i = va_arg(*ap, لما لما);
        اكسر;
    حالة ULLONG:
        arg->i = va_arg(*ap, بنانشانی لما لما);
        اكسر;
    حالة SHORT:
        arg->i = (چھوٹا)va_arg(*ap, صحيح);
        اكسر;
    حالة USHORT:
        arg->i = (بنانشانی چھوٹا)va_arg(*ap, صحيح);
        اكسر;
    حالة CHAR:
        arg->i = (نشانیوالا حرف)va_arg(*ap, صحيح);
        اكسر;
    حالة UCHAR:
        arg->i = (بنانشانی حرف)va_arg(*ap, صحيح);
        اكسر;
    حالة WORDT:
        arg->i = va_arg(*ap, لفظ_t);
    }
}


ساكن فراغ pad(باہرلا_wrap_t_2 *f, حرف c, صحيح w, صحيح l, صحيح fl)
{
    حرف pad[32]; /* good enough for what the kernel prints */
    إذا (fl & (بایاں_ADJ | ZERO_PAD) || l >= w) {
        أرجع;
    }
    l = w - l;
    memset(pad, c, l > حجم(pad) ? حجم(pad) : l);
    لكل (; l >= حجم(pad); l -= حجم(pad)) {
        باہرلا(f, pad, حجم(pad));
    }
    باہرلا(f, pad, l);
}

ساكن ثابت حرف xdigits[16] = {
    "0123456789ABCDEF"
};

ساكن حرف *fmt_x(uintmax_t x, حرف *s, صحيح ہیٹھلا)
{
    لكل (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | ہیٹھلا;
    }
    أرجع s;
}

ساكن حرف *fmt_o(uintmax_t x, حرف *s)
{
    لكل (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    أرجع s;
}

ساكن حرف *fmt_u(uintmax_t x, حرف *s)
{
    طالما (0 != x) {
#إذا defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        بنانشانی صحيح rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        إذا (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            أكد(rem <= 9); /* there must be no rounding error now */
        }
#وإلا
        uintmax_t q = x / 10;
        بنانشانی صحيح rem = x % 10;
#جے_انت
        *--s = '0' + rem;
        x = q;
    }
    أرجع s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#تعریف_کرو LDBL_MANT_DIG 113

#تعریف_کرو NL_ARGMAX 9

ساكن صحيح getint(حرف **s)
{
    صحيح i;
    لكل (i = 0; ہندسہ_اے(**s); (*s)++) {
        إذا (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } وإلا {
            i = 10 * i + DIGIT(**s);
        }
    }
    أرجع i;
}

ساكن صحيح printf_core(باہرلا_wrap_t_2 *f, ثابت حرف *fmt, va_list *ap, اتحاد arg *nl_arg, صحيح *nl_type)
{
    حرف *a, *z, *s = (حرف *)fmt;
    بنانشانی l10n = 0, fl;
    صحيح w, p, xp;
    اتحاد arg arg;
    صحيح argpos;
    بنانشانی st, ps;
    صحيح cnt = 0, l = 0;
    حرف buf[حجم(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ثابت حرف *اگیتر;
    صحيح t, pl;

    لكل (;;) {
        إذا (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            أرجع -1; /*حد_توں_ودھ*/
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        إذا (!*s) {
            اكسر;
        }

        /* Handle literal text and %% format specifiers */
        لكل (a = s; *s && *s != '%'; s++);
        لكل (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        إذا (z - a > INTMAX_MAX - cnt) {
            أرجع -1; /*حد_توں_ودھ*/
        }
        l = z - a;
        باہرلا(f, a, l);
        إذا (l) {
            استمر;
        }

        إذا (ہندسہ_اے(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } وإلا {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        لكل (fl = 0; (بنانشانی)*s - ' ' < 32 && (FLAGMASK & نقاب_TYPE(*s)); s++) {
            fl |= نقاب_TYPE(*s);
        }

        /* Read field width */
        إذا (*s == '*') {
            إذا (ہندسہ_اے(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } وإلا إذا (!l10n) {
                w = f ? va_arg(*ap, صحيح) : 0;
                s++;
            } وإلا {
                أرجع -1; /* invalid */
            }
            إذا (w < 0) {
                fl |= بایاں_ADJ;
                w = -w;
            }
        } وإلا إذا ((w = getint(&s)) < 0) {
            أرجع -1; /*حد_توں_ودھ*/
        }

        /* Read precision */
        إذا (*s == '.' && s[1] == '*') {
            إذا (ہندسہ_اے(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } وإلا إذا (!l10n) {
                p = f ? va_arg(*ap, صحيح) : 0;
                s += 2;
            } وإلا {
                أرجع -1;/* invalid */
            }
            xp = (p >= 0);
        } وإلا إذا (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } وإلا {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        افعل {
            إذا (OOB(*s)) {
                أرجع -1; /* invalid */
            }
            ps = st;
            st = حالتاں[st]S(*s++);
        } طالما (st - 1 < STOP);
        إذا (!st) {
            أرجع -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        إذا (st == NOARG) {
            إذا (argpos >= 0) {
                أرجع -1; /* invalid */
            }
        } وإلا {
            إذا (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } وإلا إذا (f) {
                نکالو_arg(&arg, st, ap);
            } وإلا {
                أرجع 0;
            }
        }

        إذا (!f) {
            استمر;
        }

        z = buf + حجم(buf);
        اگیتر = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        إذا (fl & بایاں_ADJ) {
            fl &= ~ZERO_PAD;
        }

        إذا (t == 'n') {
            إذا (!arg.p) {
                استمر;
            }
            بدل (ps) {
            حالة BARE:
                *(صحيح *)arg.p = cnt;
                اكسر;
            حالة LPRE:
                *(لما *)arg.p = cnt;
                اكسر;
            حالة LLPRE:
                *(لما لما *)arg.p = cnt;
                اكسر;
            حالة HPRE:
                *(بنانشانی چھوٹا *)arg.p = cnt;
                اكسر;
            حالة HHPRE:
                *(بنانشانی حرف *)arg.p = cnt;
                اكسر;
            حالة ZTPRE:
                *(لفظ_t *)arg.p = cnt;
                اكسر;
            حالة JPRE:
                *(لفظ_t *)arg.p = cnt;
                اكسر;
            }
            استمر;
        } وإلا إذا (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } وإلا إذا (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            إذا (p < 0 && *z) {
                أرجع -1; /*حد_توں_ودھ*/
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } وإلا {
            بدل (t) {
            حالة 'p':
                p = MAX(p, 2 * حجم(فراغ *));
                t = 'x';
                fl |= ALT_FORM;
            حالة 'x':
            حالة 'X':
                a = fmt_x(arg.i, z, t & 32);
                إذا (arg.i && (fl & ALT_FORM)) {
                    اگیتر += (t >> 4);
                    pl = 2;
                }
                اكسر;
            حالة 'o':
                a = fmt_o(arg.i, z);
                إذا ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                اكسر;
            حالة 'd':
            حالة 'i':
                pl = 1;
                إذا (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } وإلا إذا (fl & MARK_POS) {
                    اگیتر++;
                } وإلا إذا (fl & PAD_POS) {
                    اگیتر += 2;
                } وإلا {
                    pl = 0;
                }
            حالة 'u':
                a = fmt_u(arg.i, z);
                اكسر;
            }
            إذا (xp && p < 0) {
                أرجع -1; /*حد_توں_ودھ*/
            }
            إذا (xp) {
                fl &= ~ZERO_PAD;
            }
            إذا (!arg.i && !p) {
                a = z;
            } وإلا {
                p = MAX(p, z - a + !arg.i);
            }
        }

        إذا (p < z - a) {
            p = z - a;
        }
        إذا (p > INTMAX_MAX - pl) {
            أرجع -1; /*حد_توں_ودھ*/
        }
        إذا (w < pl + p) {
            w = pl + p;
        }
        إذا (w > INTMAX_MAX - cnt) {
            أرجع -1; /*حد_توں_ودھ*/
        }

        pad(f, ' ', w, pl + p, fl);
        باہرلا(f, اگیتر, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        باہرلا(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ بایاں_ADJ);

        l = w;
    }

    إذا (f) {
        أرجع cnt;
    }
    إذا (!l10n) {
        أرجع 0;
    }

    صحيح i;
    لكل (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        نکالو_arg(nl_arg + i, nl_type[i], ap);
    }
    لكل (; i <= NL_ARGMAX && !nl_type[i]; i++);
    إذا (i <= NL_ARGMAX) {
        أرجع -1; /* invalid */
    }

    أرجع 1;
}

ساكن صحيح vprintf(باہرلا_wrap_t_2 *باہرلا, ثابت حرف *fmt, va_list ap)
{
    va_list ap2;
    صحيح nl_type[NL_ARGMAX + 1] = {0};
    اتحاد arg nl_arg[NL_ARGMAX + 1];
    صحيح ret;

    /* validate format string */
    va_copy(ap2, ap);
    إذا (printf_core(عدم, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_سرا(ap2);
        أرجع -1;
    }

    ret = printf_core(باہرلا, fmt, &ap2, nl_arg, nl_type);
    va_سرا(ap2);
    أرجع ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

صحيح impl_kvprintf(ثابت حرف *format, va_list ap)
{
    باہرلا_wrap_t_2 باہرلا_wrap =  {
        .اكتب  = do_output_منزل_putchar,
        .buf    = عدم,
        .maxlen = 0,
        .ورتیا_2   = 0
    };

    أرجع vprintf(&باہرلا_wrap, format, ap);
}

صحيح impl_ksnvprintf(حرف *str, لفظ_t ناپ, ثابت حرف *format, va_list ap)
{
    إذا (!str) {
        ناپ = 0;
    }

    باہرلا_wrap_t_2 باہرلا_wrap =  {
        .اكتب  = do_output_منزل_بفر,
        .buf    = str,
        .maxlen = ناپ,
        .ورتیا_2   = 0
    };

    صحيح ret = vprintf(&باہرلا_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    إذا ((ret > 0) && (ناپ > 0)) {
        str[(ret < ناپ) ? ret : ناپ - 1] = '\0';
    }

    أرجع ret;
}

#جے_انت /* CONFIG_PRINTING */
