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

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/io.h>

#నిర్వచితమైతే CONFIG_PRINTING

#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

రకనిర్వచనం నిర్మాణం బహిర్గతం_wrap_t  బహిర్గతం_wrap_t_2;

/* handler defining how/where to actually output a buffer */
రకనిర్వచనం శూన్య (*బహిర్గతం_write_fn)(బహిర్గతం_wrap_t_2 *బహిర్గతం, స్థిరరాశి అక్షరం *buf, మాట_t len);

నిర్మాణం బహిర్గతం_wrap_t {
    స్థిరరాశి బహిర్గతం_write_fn లిఖ;
    అక్షరం *స్థిరరాశి buf;
    స్థిరరాశి మాట_t maxlen;
    మాట_t వాడిన_2;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
స్థిర శూన్య బహిర్గతం(బహిర్గతం_wrap_t_2 *బహిర్గతం_wrap, స్థిరరాశి అక్షరం *buf, మాట_t len)
{
    ఒకవేళ (బహిర్గతం_wrap) {
        బహిర్గతం_wrap->లిఖ(బహిర్గతం_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
స్థిర శూన్య do_output_గమ్యం_putchar(
    వాడని బహిర్గతం_wrap_t_2 *బహిర్గతం,
    స్థిరరాశి అక్షరం *buf,
    మాట_t len)
{
    ఒకవేళ (buf) {
        అయితే (len-- > 0) {
            అక్షరం_రాయి(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
స్థిర శూన్య do_output_గమ్యం_బఫరు(
    బహిర్గతం_wrap_t_2 *బహిర్గతం,
    స్థిరరాశి అక్షరం *buf,
    మాట_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    ఒకవేళ (buf && (బహిర్గతం->వాడిన_2 < బహిర్గతం->maxlen)) {
        /* there is still space in the buffer*/
        మాట_t విడుదల = బహిర్గతం->maxlen - బహిర్గతం->వాడిన_2;
        ఒకవేళ (len > విడుదల) {
            len = విడుదల;
        }
        జ్ఞాపకం_నకలు(&బహిర్గతం->buf[బహిర్గతం->వాడిన_2], buf, len);
        బహిర్గతం->వాడిన_2 += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

స్థిర అంతఃవరుస bool_t అంకెనా(అక్షరం c)
{
    తిరిగి c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#నిర్వచించు ముసుగు_TYPE(a) (1U<<( a -' '))

#నిర్వచించు ALT_FORM     (1U<<('#'-' '))
#నిర్వచించు ZERO_PAD     (1U<<('0'-' '))
#నిర్వచించు ఎడమ_ADJ     (1U<<('-'-' '))
#నిర్వచించు PAD_POS      (1U<<(' '-' '))
#నిర్వచించు MARK_POS     (1U<<('+'-' '))
#నిర్వచించు GROUPED      (1U<<('\''-' '))

#నిర్వచించు FLAGMASK (ALT_FORM|ZERO_PAD|ఎడమ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

గణన {
    BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
    ZTPRE, JPRE,
    STOP,
    PTR, INT, UINT, ULLONG,
    LONG, ULONG,
    SHORT, USHORT, CHAR, UCHAR,
    WORDT, LLONG,
#నిర్వచించు IMAX LLONG
#నిర్వచించు UMAX ULLONG
#నిర్వచించు PDIFF LONG
#నిర్వచించు UIPTR ULONG
    NOARG,
    MAXSTATE
};

#నిర్వచించు S(x) [(x)-'A']

స్థిర స్థిరరాశి గుర్తులేని అక్షరం స్థితులు[]['z' - 'A' + 1] = {
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

#నిర్వచించు OOB(x) ((గుర్తులేని)(x)-'A' > 'z'-'A')
#నిర్వచించు DIGIT(c) (c - '0')

సంఘం arg {
    uintmax_t i;
    దీర్ఘ ద్విగుణ f;
    శూన్య *p;
};

స్థిర శూన్య తీయి_arg(సంఘం arg *arg, పూర్ణాంకం type, va_list *ap)
{
    ఎంపిక (type) {
    స్థితి PTR:
        arg->p = va_arg(*ap, శూన్య *);
        ఆపు;
    స్థితి INT:
        arg->i = va_arg(*ap, పూర్ణాంకం);
        ఆపు;
    స్థితి UINT:
        arg->i = va_arg(*ap, గుర్తులేని పూర్ణాంకం);
        ఆపు;
    స్థితి LONG:
        arg->i = va_arg(*ap, దీర్ఘ);
        ఆపు;
    స్థితి ULONG:
        arg->i = va_arg(*ap, గుర్తులేని దీర్ఘ);
        ఆపు;
    స్థితి LLONG:
        arg->i = va_arg(*ap, దీర్ఘ దీర్ఘ);
        ఆపు;
    స్థితి ULLONG:
        arg->i = va_arg(*ap, గుర్తులేని దీర్ఘ దీర్ఘ);
        ఆపు;
    స్థితి SHORT:
        arg->i = (పొట్టి)va_arg(*ap, పూర్ణాంకం);
        ఆపు;
    స్థితి USHORT:
        arg->i = (గుర్తులేని పొట్టి)va_arg(*ap, పూర్ణాంకం);
        ఆపు;
    స్థితి CHAR:
        arg->i = (గుర్తుగల అక్షరం)va_arg(*ap, పూర్ణాంకం);
        ఆపు;
    స్థితి UCHAR:
        arg->i = (గుర్తులేని అక్షరం)va_arg(*ap, పూర్ణాంకం);
        ఆపు;
    స్థితి WORDT:
        arg->i = va_arg(*ap, మాట_t);
    }
}


స్థిర శూన్య pad(బహిర్గతం_wrap_t_2 *f, అక్షరం c, పూర్ణాంకం w, పూర్ణాంకం l, పూర్ణాంకం fl)
{
    అక్షరం pad[32]; /* good enough for what the kernel prints */
    ఒకవేళ (fl & (ఎడమ_ADJ | ZERO_PAD) || l >= w) {
        తిరిగి;
    }
    l = w - l;
    memset(pad, c, l > పరిమాణం(pad) ? పరిమాణం(pad) : l);
    కొరకు (; l >= పరిమాణం(pad); l -= పరిమాణం(pad)) {
        బహిర్గతం(f, pad, పరిమాణం(pad));
    }
    బహిర్గతం(f, pad, l);
}

స్థిర స్థిరరాశి అక్షరం xdigits[16] = {
    "0123456789ABCDEF"
};

స్థిర అక్షరం *fmt_x(uintmax_t x, అక్షరం *s, పూర్ణాంకం క్రింది)
{
    కొరకు (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | క్రింది;
    }
    తిరిగి s;
}

స్థిర అక్షరం *fmt_o(uintmax_t x, అక్షరం *s)
{
    కొరకు (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    తిరిగి s;
}

స్థిర అక్షరం *fmt_u(uintmax_t x, అక్షరం *s)
{
    అయితే (0 != x) {
#ఒకవేళ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        గుర్తులేని పూర్ణాంకం rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        ఒకవేళ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ప్రతిజ్ఞా(rem <= 9); /* there must be no rounding error now */
        }
#లేకపోతే
        uintmax_t q = x / 10;
        గుర్తులేని పూర్ణాంకం rem = x % 10;
#అయితే_ముగింపు
        *--s = '0' + rem;
        x = q;
    }
    తిరిగి s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#నిర్వచించు LDBL_MANT_DIG 113

#నిర్వచించు NL_ARGMAX 9

స్థిర పూర్ణాంకం getint(అక్షరం **s)
{
    పూర్ణాంకం i;
    కొరకు (i = 0; అంకెనా(**s); (*s)++) {
        ఒకవేళ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } లేకపోతే {
            i = 10 * i + DIGIT(**s);
        }
    }
    తిరిగి i;
}

స్థిర పూర్ణాంకం printf_core(బహిర్గతం_wrap_t_2 *f, స్థిరరాశి అక్షరం *fmt, va_list *ap, సంఘం arg *nl_arg, పూర్ణాంకం *nl_type)
{
    అక్షరం *a, *z, *s = (అక్షరం *)fmt;
    గుర్తులేని l10n = 0, fl;
    పూర్ణాంకం w, p, xp;
    సంఘం arg arg;
    పూర్ణాంకం argpos;
    గుర్తులేని st, ps;
    పూర్ణాంకం cnt = 0, l = 0;
    అక్షరం buf[పరిమాణం(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    స్థిరరాశి అక్షరం *ఉపసర్గ;
    పూర్ణాంకం t, pl;

    కొరకు (;;) {
        ఒకవేళ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            తిరిగి -1; /*అధికప్రవాహం*/
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        ఒకవేళ (!*s) {
            ఆపు;
        }

        /* Handle literal text and %% format specifiers */
        కొరకు (a = s; *s && *s != '%'; s++);
        కొరకు (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        ఒకవేళ (z - a > INTMAX_MAX - cnt) {
            తిరిగి -1; /*అధికప్రవాహం*/
        }
        l = z - a;
        బహిర్గతం(f, a, l);
        ఒకవేళ (l) {
            కొనసాగు;
        }

        ఒకవేళ (అంకెనా(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } లేకపోతే {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        కొరకు (fl = 0; (గుర్తులేని)*s - ' ' < 32 && (FLAGMASK & ముసుగు_TYPE(*s)); s++) {
            fl |= ముసుగు_TYPE(*s);
        }

        /* Read field width */
        ఒకవేళ (*s == '*') {
            ఒకవేళ (అంకెనా(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } లేకపోతే ఒకవేళ (!l10n) {
                w = f ? va_arg(*ap, పూర్ణాంకం) : 0;
                s++;
            } లేకపోతే {
                తిరిగి -1; /* invalid */
            }
            ఒకవేళ (w < 0) {
                fl |= ఎడమ_ADJ;
                w = -w;
            }
        } లేకపోతే ఒకవేళ ((w = getint(&s)) < 0) {
            తిరిగి -1; /*అధికప్రవాహం*/
        }

        /* Read precision */
        ఒకవేళ (*s == '.' && s[1] == '*') {
            ఒకవేళ (అంకెనా(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } లేకపోతే ఒకవేళ (!l10n) {
                p = f ? va_arg(*ap, పూర్ణాంకం) : 0;
                s += 2;
            } లేకపోతే {
                తిరిగి -1;/* invalid */
            }
            xp = (p >= 0);
        } లేకపోతే ఒకవేళ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } లేకపోతే {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        చేయి {
            ఒకవేళ (OOB(*s)) {
                తిరిగి -1; /* invalid */
            }
            ps = st;
            st = స్థితులు[st]S(*s++);
        } అయితే (st - 1 < STOP);
        ఒకవేళ (!st) {
            తిరిగి -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        ఒకవేళ (st == NOARG) {
            ఒకవేళ (argpos >= 0) {
                తిరిగి -1; /* invalid */
            }
        } లేకపోతే {
            ఒకవేళ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } లేకపోతే ఒకవేళ (f) {
                తీయి_arg(&arg, st, ap);
            } లేకపోతే {
                తిరిగి 0;
            }
        }

        ఒకవేళ (!f) {
            కొనసాగు;
        }

        z = buf + పరిమాణం(buf);
        ఉపసర్గ = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        ఒకవేళ (fl & ఎడమ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        ఒకవేళ (t == 'n') {
            ఒకవేళ (!arg.p) {
                కొనసాగు;
            }
            ఎంపిక (ps) {
            స్థితి BARE:
                *(పూర్ణాంకం *)arg.p = cnt;
                ఆపు;
            స్థితి LPRE:
                *(దీర్ఘ *)arg.p = cnt;
                ఆపు;
            స్థితి LLPRE:
                *(దీర్ఘ దీర్ఘ *)arg.p = cnt;
                ఆపు;
            స్థితి HPRE:
                *(గుర్తులేని పొట్టి *)arg.p = cnt;
                ఆపు;
            స్థితి HHPRE:
                *(గుర్తులేని అక్షరం *)arg.p = cnt;
                ఆపు;
            స్థితి ZTPRE:
                *(మాట_t *)arg.p = cnt;
                ఆపు;
            స్థితి JPRE:
                *(మాట_t *)arg.p = cnt;
                ఆపు;
            }
            కొనసాగు;
        } లేకపోతే ఒకవేళ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } లేకపోతే ఒకవేళ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            ఒకవేళ (p < 0 && *z) {
                తిరిగి -1; /*అధికప్రవాహం*/
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } లేకపోతే {
            ఎంపిక (t) {
            స్థితి 'p':
                p = MAX(p, 2 * పరిమాణం(శూన్య *));
                t = 'x';
                fl |= ALT_FORM;
            స్థితి 'x':
            స్థితి 'X':
                a = fmt_x(arg.i, z, t & 32);
                ఒకవేళ (arg.i && (fl & ALT_FORM)) {
                    ఉపసర్గ += (t >> 4);
                    pl = 2;
                }
                ఆపు;
            స్థితి 'o':
                a = fmt_o(arg.i, z);
                ఒకవేళ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                ఆపు;
            స్థితి 'd':
            స్థితి 'i':
                pl = 1;
                ఒకవేళ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } లేకపోతే ఒకవేళ (fl & MARK_POS) {
                    ఉపసర్గ++;
                } లేకపోతే ఒకవేళ (fl & PAD_POS) {
                    ఉపసర్గ += 2;
                } లేకపోతే {
                    pl = 0;
                }
            స్థితి 'u':
                a = fmt_u(arg.i, z);
                ఆపు;
            }
            ఒకవేళ (xp && p < 0) {
                తిరిగి -1; /*అధికప్రవాహం*/
            }
            ఒకవేళ (xp) {
                fl &= ~ZERO_PAD;
            }
            ఒకవేళ (!arg.i && !p) {
                a = z;
            } లేకపోతే {
                p = MAX(p, z - a + !arg.i);
            }
        }

        ఒకవేళ (p < z - a) {
            p = z - a;
        }
        ఒకవేళ (p > INTMAX_MAX - pl) {
            తిరిగి -1; /*అధికప్రవాహం*/
        }
        ఒకవేళ (w < pl + p) {
            w = pl + p;
        }
        ఒకవేళ (w > INTMAX_MAX - cnt) {
            తిరిగి -1; /*అధికప్రవాహం*/
        }

        pad(f, ' ', w, pl + p, fl);
        బహిర్గతం(f, ఉపసర్గ, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        బహిర్గతం(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ఎడమ_ADJ);

        l = w;
    }

    ఒకవేళ (f) {
        తిరిగి cnt;
    }
    ఒకవేళ (!l10n) {
        తిరిగి 0;
    }

    పూర్ణాంకం i;
    కొరకు (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        తీయి_arg(nl_arg + i, nl_type[i], ap);
    }
    కొరకు (; i <= NL_ARGMAX && !nl_type[i]; i++);
    ఒకవేళ (i <= NL_ARGMAX) {
        తిరిగి -1; /* invalid */
    }

    తిరిగి 1;
}

స్థిర పూర్ణాంకం vprintf(బహిర్గతం_wrap_t_2 *బహిర్గతం, స్థిరరాశి అక్షరం *fmt, va_list ap)
{
    va_list ap2;
    పూర్ణాంకం nl_type[NL_ARGMAX + 1] = {0};
    సంఘం arg nl_arg[NL_ARGMAX + 1];
    పూర్ణాంకం ret;

    /* validate format string */
    va_copy(ap2, ap);
    ఒకవేళ (printf_core(ఖాళీ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_చివర(ap2);
        తిరిగి -1;
    }

    ret = printf_core(బహిర్గతం, fmt, &ap2, nl_arg, nl_type);
    va_చివర(ap2);
    తిరిగి ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

పూర్ణాంకం impl_kvprintf(స్థిరరాశి అక్షరం *format, va_list ap)
{
    బహిర్గతం_wrap_t_2 బహిర్గతం_wrap =  {
        .లిఖ  = do_output_గమ్యం_putchar,
        .buf    = ఖాళీ,
        .maxlen = 0,
        .వాడిన_2   = 0
    };

    తిరిగి vprintf(&బహిర్గతం_wrap, format, ap);
}

పూర్ణాంకం impl_ksnvprintf(అక్షరం *str, మాట_t కొలత, స్థిరరాశి అక్షరం *format, va_list ap)
{
    ఒకవేళ (!str) {
        కొలత = 0;
    }

    బహిర్గతం_wrap_t_2 బహిర్గతం_wrap =  {
        .లిఖ  = do_output_గమ్యం_బఫరు,
        .buf    = str,
        .maxlen = కొలత,
        .వాడిన_2   = 0
    };

    పూర్ణాంకం ret = vprintf(&బహిర్గతం_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    ఒకవేళ ((ret > 0) && (కొలత > 0)) {
        str[(ret < కొలత) ? ret : కొలత - 1] = '\0';
    }

    తిరిగి ret;
}

#అయితే_ముగింపు /* CONFIG_PRINTING */
