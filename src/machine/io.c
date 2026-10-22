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

#شامل <config.h>
#شامل <machine/io.h>

#ifdef CONFIG_PRINTING

#شامل <stdarg.h>
#شامل <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

تعریفنوع ساختار _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
تعریفنوع تهی (*out_write_fn)(out_wrap_t *out, ثابت نویسه *buf, word_t len);

ساختار _out_wrap_t {
    ثابت out_write_fn اكتب;
    نویسه *ثابت buf;
    ثابت word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ایستا تهی out(out_wrap_t *out_wrap, ثابت نویسه *buf, word_t len)
{
    اگر (out_wrap) {
        out_wrap->اكتب(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ایستا تهی do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ثابت نویسه *buf,
    word_t len)
{
    اگر (buf) {
        وقتی (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ایستا تهی do_output_to_buffer(
    out_wrap_t *out,
    ثابت نویسه *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    اگر (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t آزاد = out->maxlen - out->used;
        اگر (len > آزاد) {
            len = آزاد;
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

ایستا درونخطی bool_t isdigit(نویسه c)
{
    بازگشت c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define چپ_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|چپ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

شمارش {
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

ایستا ثابت unsigned نویسه states[]['z' - 'A' + 1] = {
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

اجتماع arg {
    uintmax_t i;
    long دوگانه f;
    تهی *p;
};

ایستا تهی برداشت_arg(اجتماع arg *arg, صحیح type, va_list *ap)
{
    انتخاب (type) {
    حالت PTR:
        arg->p = va_arg(*ap, تهی *);
        بشکن;
    حالت INT:
        arg->i = va_arg(*ap, صحیح);
        بشکن;
    حالت UINT:
        arg->i = va_arg(*ap, unsigned صحیح);
        بشکن;
    حالت LONG:
        arg->i = va_arg(*ap, long);
        بشکن;
    حالت ULONG:
        arg->i = va_arg(*ap, unsigned long);
        بشکن;
    حالت LLONG:
        arg->i = va_arg(*ap, long long);
        بشکن;
    حالت ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        بشکن;
    حالت SHORT:
        arg->i = (short)va_arg(*ap, صحیح);
        بشکن;
    حالت USHORT:
        arg->i = (unsigned short)va_arg(*ap, صحیح);
        بشکن;
    حالت CHAR:
        arg->i = (signed نویسه)va_arg(*ap, صحیح);
        بشکن;
    حالت UCHAR:
        arg->i = (unsigned نویسه)va_arg(*ap, صحیح);
        بشکن;
    حالت WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


ایستا تهی pad(out_wrap_t *f, نویسه c, صحیح w, صحیح l, صحیح fl)
{
    نویسه pad[32]; /* good enough for what the kernel prints */
    اگر (fl & (چپ_ADJ | ZERO_PAD) || l >= w) {
        بازگشت;
    }
    l = w - l;
    memset(pad, c, l > اندازه(pad) ? اندازه(pad) : l);
    برای (; l >= اندازه(pad); l -= اندازه(pad)) {
        out(f, pad, اندازه(pad));
    }
    out(f, pad, l);
}

ایستا ثابت نویسه xdigits[16] = {
    "0123456789ABCDEF"
};

ایستا نویسه *fmt_x(uintmax_t x, نویسه *s, صحیح lower)
{
    برای (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    بازگشت s;
}

ایستا نویسه *fmt_o(uintmax_t x, نویسه *s)
{
    برای (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    بازگشت s;
}

ایستا نویسه *fmt_u(uintmax_t x, نویسه *s)
{
    وقتی (0 != x) {
#اگر defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned صحیح rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        اگر (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            أكد(rem <= 9); /* there must be no rounding error now */
        }
#وگرنه
        uintmax_t q = x / 10;
        unsigned صحیح rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    بازگشت s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

ایستا صحیح getint(نویسه **s)
{
    صحیح i;
    برای (i = 0; isdigit(**s); (*s)++) {
        اگر (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } وگرنه {
            i = 10 * i + DIGIT(**s);
        }
    }
    بازگشت i;
}

ایستا صحیح printf_core(out_wrap_t *f, ثابت نویسه *fmt, va_list *ap, اجتماع arg *nl_arg, صحیح *nl_type)
{
    نویسه *a, *z, *s = (نویسه *)fmt;
    unsigned l10n = 0, fl;
    صحیح w, p, xp;
    اجتماع arg arg;
    صحیح argpos;
    unsigned st, ps;
    صحیح cnt = 0, l = 0;
    نویسه buf[اندازه(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ثابت نویسه *prefix;
    صحیح t, pl;

    برای (;;) {
        اگر (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            بازگشت -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        اگر (!*s) {
            بشکن;
        }

        /* Handle literal text and %% format specifiers */
        برای (a = s; *s && *s != '%'; s++);
        برای (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        اگر (z - a > INTMAX_MAX - cnt) {
            بازگشت -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        اگر (l) {
            ادامه;
        }

        اگر (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } وگرنه {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        برای (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        اگر (*s == '*') {
            اگر (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } وگرنه اگر (!l10n) {
                w = f ? va_arg(*ap, صحیح) : 0;
                s++;
            } وگرنه {
                بازگشت -1; /* invalid */
            }
            اگر (w < 0) {
                fl |= چپ_ADJ;
                w = -w;
            }
        } وگرنه اگر ((w = getint(&s)) < 0) {
            بازگشت -1; /* overflow */
        }

        /* Read precision */
        اگر (*s == '.' && s[1] == '*') {
            اگر (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } وگرنه اگر (!l10n) {
                p = f ? va_arg(*ap, صحیح) : 0;
                s += 2;
            } وگرنه {
                بازگشت -1;/* invalid */
            }
            xp = (p >= 0);
        } وگرنه اگر (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } وگرنه {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        انجامبده {
            اگر (OOB(*s)) {
                بازگشت -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } وقتی (st - 1 < STOP);
        اگر (!st) {
            بازگشت -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        اگر (st == NOARG) {
            اگر (argpos >= 0) {
                بازگشت -1; /* invalid */
            }
        } وگرنه {
            اگر (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } وگرنه اگر (f) {
                برداشت_arg(&arg, st, ap);
            } وگرنه {
                بازگشت 0;
            }
        }

        اگر (!f) {
            ادامه;
        }

        z = buf + اندازه(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        اگر (fl & چپ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        اگر (t == 'n') {
            اگر (!arg.p) {
                ادامه;
            }
            انتخاب (ps) {
            حالت BARE:
                *(صحیح *)arg.p = cnt;
                بشکن;
            حالت LPRE:
                *(long *)arg.p = cnt;
                بشکن;
            حالت LLPRE:
                *(long long *)arg.p = cnt;
                بشکن;
            حالت HPRE:
                *(unsigned short *)arg.p = cnt;
                بشکن;
            حالت HHPRE:
                *(unsigned نویسه *)arg.p = cnt;
                بشکن;
            حالت ZTPRE:
                *(word_t *)arg.p = cnt;
                بشکن;
            حالت JPRE:
                *(word_t *)arg.p = cnt;
                بشکن;
            }
            ادامه;
        } وگرنه اگر (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } وگرنه اگر (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            اگر (p < 0 && *z) {
                بازگشت -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } وگرنه {
            انتخاب (t) {
            حالت 'p':
                p = MAX(p, 2 * اندازه(تهی *));
                t = 'x';
                fl |= ALT_FORM;
            حالت 'x':
            حالت 'X':
                a = fmt_x(arg.i, z, t & 32);
                اگر (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                بشکن;
            حالت 'o':
                a = fmt_o(arg.i, z);
                اگر ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                بشکن;
            حالت 'd':
            حالت 'i':
                pl = 1;
                اگر (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } وگرنه اگر (fl & MARK_POS) {
                    prefix++;
                } وگرنه اگر (fl & PAD_POS) {
                    prefix += 2;
                } وگرنه {
                    pl = 0;
                }
            حالت 'u':
                a = fmt_u(arg.i, z);
                بشکن;
            }
            اگر (xp && p < 0) {
                بازگشت -1; /* overflow */
            }
            اگر (xp) {
                fl &= ~ZERO_PAD;
            }
            اگر (!arg.i && !p) {
                a = z;
            } وگرنه {
                p = MAX(p, z - a + !arg.i);
            }
        }

        اگر (p < z - a) {
            p = z - a;
        }
        اگر (p > INTMAX_MAX - pl) {
            بازگشت -1; /* overflow */
        }
        اگر (w < pl + p) {
            w = pl + p;
        }
        اگر (w > INTMAX_MAX - cnt) {
            بازگشت -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ چپ_ADJ);

        l = w;
    }

    اگر (f) {
        بازگشت cnt;
    }
    اگر (!l10n) {
        بازگشت 0;
    }

    صحیح i;
    برای (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        برداشت_arg(nl_arg + i, nl_type[i], ap);
    }
    برای (; i <= NL_ARGMAX && !nl_type[i]; i++);
    اگر (i <= NL_ARGMAX) {
        بازگشت -1; /* invalid */
    }

    بازگشت 1;
}

ایستا صحیح vprintf(out_wrap_t *out, ثابت نویسه *fmt, va_list ap)
{
    va_list ap2;
    صحیح nl_type[NL_ARGMAX + 1] = {0};
    اجتماع arg nl_arg[NL_ARGMAX + 1];
    صحیح ret;

    /* validate format string */
    va_copy(ap2, ap);
    اگر (printf_core(پوچ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        بازگشت -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    بازگشت ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

صحیح impl_kvprintf(ثابت نویسه *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .اكتب  = do_output_to_putchar,
        .buf    = پوچ,
        .maxlen = 0,
        .used   = 0
    };

    بازگشت vprintf(&out_wrap, format, ap);
}

صحیح impl_ksnvprintf(نویسه *str, word_t size, ثابت نویسه *format, va_list ap)
{
    اگر (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .اكتب  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    صحیح ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    اگر ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    بازگشت ret;
}

#endif /* CONFIG_PRINTING */
