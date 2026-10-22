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

#اگر_تعریف_شدہ CONFIG_PRINTING

#شامل <stdarg.h>
#شامل <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

قسمتعریف ساخت برآمد_wrap_t  برآمد_wrap_t_2;

/* handler defining how/where to actually output a buffer */
قسمتعریف خالی (*برآمد_write_fn)(برآمد_wrap_t_2 *برآمد, ثابت حرف *buf, لفظ_t len);

ساخت برآمد_wrap_t {
    ثابت برآمد_write_fn اكتب;
    حرف *ثابت buf;
    ثابت لفظ_t maxlen;
    لفظ_t مستعمل_2;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
ساکن خالی برآمد(برآمد_wrap_t_2 *برآمد_wrap, ثابت حرف *buf, لفظ_t len)
{
    اگر (برآمد_wrap) {
        برآمد_wrap->اكتب(برآمد_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساکن خالی do_output_مقصد_putchar(
    غیر_مستعمل برآمد_wrap_t_2 *برآمد,
    ثابت حرف *buf,
    لفظ_t len)
{
    اگر (buf) {
        جب (len-- > 0) {
            حرف_لکھو(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
ساکن خالی do_output_مقصد_بفر(
    برآمد_wrap_t_2 *برآمد,
    ثابت حرف *buf,
    لفظ_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    اگر (buf && (برآمد->مستعمل_2 < برآمد->maxlen)) {
        /* there is still space in the buffer*/
        لفظ_t آزاد = برآمد->maxlen - برآمد->مستعمل_2;
        اگر (len > آزاد) {
            len = آزاد;
        }
        حافظہ_نقل(&برآمد->buf[برآمد->مستعمل_2], buf, len);
        برآمد->مستعمل_2 += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

ساکن اندرونی bool_t ہندسہ_ہے(حرف c)
{
    واپس c >= '0' &&
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

شمار {
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

ساکن ثابت غیرعلامتی حرف حالتیں[]['z' - 'A' + 1] = {
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

#تعریف_کرو OOB(x) ((غیرعلامتی)(x)-'A' > 'z'-'A')
#تعریف_کرو DIGIT(c) (c - '0')

اتحاد arg {
    uintmax_t i;
    طویل دوہرا f;
    خالی *p;
};

ساکن خالی نکالو_arg(اتحاد arg *arg, صحیح type, va_list *ap)
{
    انتخاب (type) {
    حالت PTR:
        arg->p = va_arg(*ap, خالی *);
        توڑو;
    حالت INT:
        arg->i = va_arg(*ap, صحیح);
        توڑو;
    حالت UINT:
        arg->i = va_arg(*ap, غیرعلامتی صحیح);
        توڑو;
    حالت LONG:
        arg->i = va_arg(*ap, طویل);
        توڑو;
    حالت ULONG:
        arg->i = va_arg(*ap, غیرعلامتی طویل);
        توڑو;
    حالت LLONG:
        arg->i = va_arg(*ap, طویل طویل);
        توڑو;
    حالت ULLONG:
        arg->i = va_arg(*ap, غیرعلامتی طویل طویل);
        توڑو;
    حالت SHORT:
        arg->i = (مختصر)va_arg(*ap, صحیح);
        توڑو;
    حالت USHORT:
        arg->i = (غیرعلامتی مختصر)va_arg(*ap, صحیح);
        توڑو;
    حالت CHAR:
        arg->i = (علامتی حرف)va_arg(*ap, صحیح);
        توڑو;
    حالت UCHAR:
        arg->i = (غیرعلامتی حرف)va_arg(*ap, صحیح);
        توڑو;
    حالت WORDT:
        arg->i = va_arg(*ap, لفظ_t);
    }
}


ساکن خالی pad(برآمد_wrap_t_2 *f, حرف c, صحیح w, صحیح l, صحیح fl)
{
    حرف pad[32]; /* good enough for what the kernel prints */
    اگر (fl & (بایاں_ADJ | ZERO_PAD) || l >= w) {
        واپس;
    }
    l = w - l;
    memset(pad, c, l > حجم(pad) ? حجم(pad) : l);
    برائے (; l >= حجم(pad); l -= حجم(pad)) {
        برآمد(f, pad, حجم(pad));
    }
    برآمد(f, pad, l);
}

ساکن ثابت حرف xdigits[16] = {
    "0123456789ABCDEF"
};

ساکن حرف *fmt_x(uintmax_t x, حرف *s, صحیح زیریں)
{
    برائے (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | زیریں;
    }
    واپس s;
}

ساکن حرف *fmt_o(uintmax_t x, حرف *s)
{
    برائے (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    واپس s;
}

ساکن حرف *fmt_u(uintmax_t x, حرف *s)
{
    جب (0 != x) {
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
        غیرعلامتی صحیح rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        اگر (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            أكد(rem <= 9); /* there must be no rounding error now */
        }
#ورنہ
        uintmax_t q = x / 10;
        غیرعلامتی صحیح rem = x % 10;
#اگر_ختم
        *--s = '0' + rem;
        x = q;
    }
    واپس s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#تعریف_کرو LDBL_MANT_DIG 113

#تعریف_کرو NL_ARGMAX 9

ساکن صحیح getint(حرف **s)
{
    صحیح i;
    برائے (i = 0; ہندسہ_ہے(**s); (*s)++) {
        اگر (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } ورنہ {
            i = 10 * i + DIGIT(**s);
        }
    }
    واپس i;
}

ساکن صحیح printf_core(برآمد_wrap_t_2 *f, ثابت حرف *fmt, va_list *ap, اتحاد arg *nl_arg, صحیح *nl_type)
{
    حرف *a, *z, *s = (حرف *)fmt;
    غیرعلامتی l10n = 0, fl;
    صحیح w, p, xp;
    اتحاد arg arg;
    صحیح argpos;
    غیرعلامتی st, ps;
    صحیح cnt = 0, l = 0;
    حرف buf[حجم(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ثابت حرف *سابقہ;
    صحیح t, pl;

    برائے (;;) {
        اگر (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            واپس -1; /*حد_سے_تجاوز*/
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        اگر (!*s) {
            توڑو;
        }

        /* Handle literal text and %% format specifiers */
        برائے (a = s; *s && *s != '%'; s++);
        برائے (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        اگر (z - a > INTMAX_MAX - cnt) {
            واپس -1; /*حد_سے_تجاوز*/
        }
        l = z - a;
        برآمد(f, a, l);
        اگر (l) {
            جاری;
        }

        اگر (ہندسہ_ہے(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } ورنہ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        برائے (fl = 0; (غیرعلامتی)*s - ' ' < 32 && (FLAGMASK & نقاب_TYPE(*s)); s++) {
            fl |= نقاب_TYPE(*s);
        }

        /* Read field width */
        اگر (*s == '*') {
            اگر (ہندسہ_ہے(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } ورنہ اگر (!l10n) {
                w = f ? va_arg(*ap, صحیح) : 0;
                s++;
            } ورنہ {
                واپس -1; /* invalid */
            }
            اگر (w < 0) {
                fl |= بایاں_ADJ;
                w = -w;
            }
        } ورنہ اگر ((w = getint(&s)) < 0) {
            واپس -1; /*حد_سے_تجاوز*/
        }

        /* Read precision */
        اگر (*s == '.' && s[1] == '*') {
            اگر (ہندسہ_ہے(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } ورنہ اگر (!l10n) {
                p = f ? va_arg(*ap, صحیح) : 0;
                s += 2;
            } ورنہ {
                واپس -1;/* invalid */
            }
            xp = (p >= 0);
        } ورنہ اگر (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } ورنہ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        کرو {
            اگر (OOB(*s)) {
                واپس -1; /* invalid */
            }
            ps = st;
            st = حالتیں[st]S(*s++);
        } جب (st - 1 < STOP);
        اگر (!st) {
            واپس -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        اگر (st == NOARG) {
            اگر (argpos >= 0) {
                واپس -1; /* invalid */
            }
        } ورنہ {
            اگر (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } ورنہ اگر (f) {
                نکالو_arg(&arg, st, ap);
            } ورنہ {
                واپس 0;
            }
        }

        اگر (!f) {
            جاری;
        }

        z = buf + حجم(buf);
        سابقہ = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        اگر (fl & بایاں_ADJ) {
            fl &= ~ZERO_PAD;
        }

        اگر (t == 'n') {
            اگر (!arg.p) {
                جاری;
            }
            انتخاب (ps) {
            حالت BARE:
                *(صحیح *)arg.p = cnt;
                توڑو;
            حالت LPRE:
                *(طویل *)arg.p = cnt;
                توڑو;
            حالت LLPRE:
                *(طویل طویل *)arg.p = cnt;
                توڑو;
            حالت HPRE:
                *(غیرعلامتی مختصر *)arg.p = cnt;
                توڑو;
            حالت HHPRE:
                *(غیرعلامتی حرف *)arg.p = cnt;
                توڑو;
            حالت ZTPRE:
                *(لفظ_t *)arg.p = cnt;
                توڑو;
            حالت JPRE:
                *(لفظ_t *)arg.p = cnt;
                توڑو;
            }
            جاری;
        } ورنہ اگر (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } ورنہ اگر (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            اگر (p < 0 && *z) {
                واپس -1; /*حد_سے_تجاوز*/
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } ورنہ {
            انتخاب (t) {
            حالت 'p':
                p = MAX(p, 2 * حجم(خالی *));
                t = 'x';
                fl |= ALT_FORM;
            حالت 'x':
            حالت 'X':
                a = fmt_x(arg.i, z, t & 32);
                اگر (arg.i && (fl & ALT_FORM)) {
                    سابقہ += (t >> 4);
                    pl = 2;
                }
                توڑو;
            حالت 'o':
                a = fmt_o(arg.i, z);
                اگر ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                توڑو;
            حالت 'd':
            حالت 'i':
                pl = 1;
                اگر (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } ورنہ اگر (fl & MARK_POS) {
                    سابقہ++;
                } ورنہ اگر (fl & PAD_POS) {
                    سابقہ += 2;
                } ورنہ {
                    pl = 0;
                }
            حالت 'u':
                a = fmt_u(arg.i, z);
                توڑو;
            }
            اگر (xp && p < 0) {
                واپس -1; /*حد_سے_تجاوز*/
            }
            اگر (xp) {
                fl &= ~ZERO_PAD;
            }
            اگر (!arg.i && !p) {
                a = z;
            } ورنہ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        اگر (p < z - a) {
            p = z - a;
        }
        اگر (p > INTMAX_MAX - pl) {
            واپس -1; /*حد_سے_تجاوز*/
        }
        اگر (w < pl + p) {
            w = pl + p;
        }
        اگر (w > INTMAX_MAX - cnt) {
            واپس -1; /*حد_سے_تجاوز*/
        }

        pad(f, ' ', w, pl + p, fl);
        برآمد(f, سابقہ, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        برآمد(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ بایاں_ADJ);

        l = w;
    }

    اگر (f) {
        واپس cnt;
    }
    اگر (!l10n) {
        واپس 0;
    }

    صحیح i;
    برائے (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        نکالو_arg(nl_arg + i, nl_type[i], ap);
    }
    برائے (; i <= NL_ARGMAX && !nl_type[i]; i++);
    اگر (i <= NL_ARGMAX) {
        واپس -1; /* invalid */
    }

    واپس 1;
}

ساکن صحیح vprintf(برآمد_wrap_t_2 *برآمد, ثابت حرف *fmt, va_list ap)
{
    va_list ap2;
    صحیح nl_type[NL_ARGMAX + 1] = {0};
    اتحاد arg nl_arg[NL_ARGMAX + 1];
    صحیح ret;

    /* validate format string */
    va_copy(ap2, ap);
    اگر (printf_core(تہی, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_انتہا(ap2);
        واپس -1;
    }

    ret = printf_core(برآمد, fmt, &ap2, nl_arg, nl_type);
    va_انتہا(ap2);
    واپس ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

صحیح impl_kvprintf(ثابت حرف *format, va_list ap)
{
    برآمد_wrap_t_2 برآمد_wrap =  {
        .اكتب  = do_output_مقصد_putchar,
        .buf    = تہی,
        .maxlen = 0,
        .مستعمل_2   = 0
    };

    واپس vprintf(&برآمد_wrap, format, ap);
}

صحیح impl_ksnvprintf(حرف *str, لفظ_t سائز, ثابت حرف *format, va_list ap)
{
    اگر (!str) {
        سائز = 0;
    }

    برآمد_wrap_t_2 برآمد_wrap =  {
        .اكتب  = do_output_مقصد_بفر,
        .buf    = str,
        .maxlen = سائز,
        .مستعمل_2   = 0
    };

    صحیح ret = vprintf(&برآمد_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    اگر ((ret > 0) && (سائز > 0)) {
        str[(ret < سائز) ? ret : سائز - 1] = '\0';
    }

    واپس ret;
}

#اگر_ختم /* CONFIG_PRINTING */
