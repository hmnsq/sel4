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

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <machine/io.h>

#ifdef CONFIG_PRINTING

#അന്തര്ഭാവ <stdarg.h>
#അന്തര്ഭാവ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

തരനിർവചനം ഘടന _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
തരനിർവചനം ശൂന്യം (*out_write_fn)(out_wrap_t *out, സ്ഥിരം അക്ഷരം *buf, word_t len);

ഘടന _out_wrap_t {
    സ്ഥിരം out_write_fn ലിഖ;
    അക്ഷരം *സ്ഥിരം buf;
    സ്ഥിരം word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
സ്ഥിതം ശൂന്യം out(out_wrap_t *out_wrap, സ്ഥിരം അക്ഷരം *buf, word_t len)
{
    എങ്കിൽ (out_wrap) {
        out_wrap->ലിഖ(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
സ്ഥിതം ശൂന്യം do_output_to_putchar(
    UNUSED out_wrap_t *out,
    സ്ഥിരം അക്ഷരം *buf,
    word_t len)
{
    എങ്കിൽ (buf) {
        ആകുമ്പോൾ (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
സ്ഥിതം ശൂന്യം do_output_to_buffer(
    out_wrap_t *out,
    സ്ഥിരം അക്ഷരം *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    എങ്കിൽ (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t സ്വതന്ത്രം = out->maxlen - out->used;
        എങ്കിൽ (len > സ്വതന്ത്രം) {
            len = സ്വതന്ത്രം;
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

സ്ഥിതം വരിയിൽ bool_t isdigit(അക്ഷരം c)
{
    തിരികെ c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define ഇടത്_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|ഇടത്_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

എണ്ണൽ {
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

സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം states[]['z' - 'A' + 1] = {
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

സംഘം arg {
    uintmax_t i;
    long ഇരട്ട f;
    ശൂന്യം *p;
};

സ്ഥിതം ശൂന്യം എടുക്കുക_arg(സംഘം arg *arg, പൂർണ്ണസംഖ്യ type, va_list *ap)
{
    തിരഞ്ഞെടുപ്പ് (type) {
    അവസ്ഥ PTR:
        arg->p = va_arg(*ap, ശൂന്യം *);
        നിർത്തുക;
    അവസ്ഥ INT:
        arg->i = va_arg(*ap, പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ UINT:
        arg->i = va_arg(*ap, unsigned പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ LONG:
        arg->i = va_arg(*ap, long);
        നിർത്തുക;
    അവസ്ഥ ULONG:
        arg->i = va_arg(*ap, unsigned long);
        നിർത്തുക;
    അവസ്ഥ LLONG:
        arg->i = va_arg(*ap, long long);
        നിർത്തുക;
    അവസ്ഥ ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        നിർത്തുക;
    അവസ്ഥ SHORT:
        arg->i = (short)va_arg(*ap, പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ USHORT:
        arg->i = (unsigned short)va_arg(*ap, പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ CHAR:
        arg->i = (signed അക്ഷരം)va_arg(*ap, പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ UCHAR:
        arg->i = (unsigned അക്ഷരം)va_arg(*ap, പൂർണ്ണസംഖ്യ);
        നിർത്തുക;
    അവസ്ഥ WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


സ്ഥിതം ശൂന്യം pad(out_wrap_t *f, അക്ഷരം c, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ l, പൂർണ്ണസംഖ്യ fl)
{
    അക്ഷരം pad[32]; /* good enough for what the kernel prints */
    എങ്കിൽ (fl & (ഇടത്_ADJ | ZERO_PAD) || l >= w) {
        തിരികെ;
    }
    l = w - l;
    memset(pad, c, l > വലുപ്പം(pad) ? വലുപ്പം(pad) : l);
    വേണ്ടി (; l >= വലുപ്പം(pad); l -= വലുപ്പം(pad)) {
        out(f, pad, വലുപ്പം(pad));
    }
    out(f, pad, l);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം xdigits[16] = {
    "0123456789ABCDEF"
};

സ്ഥിതം അക്ഷരം *fmt_x(uintmax_t x, അക്ഷരം *s, പൂർണ്ണസംഖ്യ lower)
{
    വേണ്ടി (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    തിരികെ s;
}

സ്ഥിതം അക്ഷരം *fmt_o(uintmax_t x, അക്ഷരം *s)
{
    വേണ്ടി (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    തിരികെ s;
}

സ്ഥിതം അക്ഷരം *fmt_u(uintmax_t x, അക്ഷരം *s)
{
    ആകുമ്പോൾ (0 != x) {
#എങ്കിൽ defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned പൂർണ്ണസംഖ്യ rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        എങ്കിൽ (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            പ്രതിജ്ഞാ(rem <= 9); /* there must be no rounding error now */
        }
#അല്ലെങ്കിൽ
        uintmax_t q = x / 10;
        unsigned പൂർണ്ണസംഖ്യ rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    തിരികെ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

സ്ഥിതം പൂർണ്ണസംഖ്യ getint(അക്ഷരം **s)
{
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 0; isdigit(**s); (*s)++) {
        എങ്കിൽ (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } അല്ലെങ്കിൽ {
            i = 10 * i + DIGIT(**s);
        }
    }
    തിരികെ i;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ printf_core(out_wrap_t *f, സ്ഥിരം അക്ഷരം *fmt, va_list *ap, സംഘം arg *nl_arg, പൂർണ്ണസംഖ്യ *nl_type)
{
    അക്ഷരം *a, *z, *s = (അക്ഷരം *)fmt;
    unsigned l10n = 0, fl;
    പൂർണ്ണസംഖ്യ w, p, xp;
    സംഘം arg arg;
    പൂർണ്ണസംഖ്യ argpos;
    unsigned st, ps;
    പൂർണ്ണസംഖ്യ cnt = 0, l = 0;
    അക്ഷരം buf[വലുപ്പം(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    സ്ഥിരം അക്ഷരം *prefix;
    പൂർണ്ണസംഖ്യ t, pl;

    വേണ്ടി (;;) {
        എങ്കിൽ (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            തിരികെ -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        എങ്കിൽ (!*s) {
            നിർത്തുക;
        }

        /* Handle literal text and %% format specifiers */
        വേണ്ടി (a = s; *s && *s != '%'; s++);
        വേണ്ടി (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        എങ്കിൽ (z - a > INTMAX_MAX - cnt) {
            തിരികെ -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        എങ്കിൽ (l) {
            തുടരുക;
        }

        എങ്കിൽ (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } അല്ലെങ്കിൽ {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        വേണ്ടി (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        എങ്കിൽ (*s == '*') {
            എങ്കിൽ (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } അല്ലെങ്കിൽ എങ്കിൽ (!l10n) {
                w = f ? va_arg(*ap, പൂർണ്ണസംഖ്യ) : 0;
                s++;
            } അല്ലെങ്കിൽ {
                തിരികെ -1; /* invalid */
            }
            എങ്കിൽ (w < 0) {
                fl |= ഇടത്_ADJ;
                w = -w;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ ((w = getint(&s)) < 0) {
            തിരികെ -1; /* overflow */
        }

        /* Read precision */
        എങ്കിൽ (*s == '.' && s[1] == '*') {
            എങ്കിൽ (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } അല്ലെങ്കിൽ എങ്കിൽ (!l10n) {
                p = f ? va_arg(*ap, പൂർണ്ണസംഖ്യ) : 0;
                s += 2;
            } അല്ലെങ്കിൽ {
                തിരികെ -1;/* invalid */
            }
            xp = (p >= 0);
        } അല്ലെങ്കിൽ എങ്കിൽ (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } അല്ലെങ്കിൽ {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        ചെയ്യുക {
            എങ്കിൽ (OOB(*s)) {
                തിരികെ -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } ആകുമ്പോൾ (st - 1 < STOP);
        എങ്കിൽ (!st) {
            തിരികെ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        എങ്കിൽ (st == NOARG) {
            എങ്കിൽ (argpos >= 0) {
                തിരികെ -1; /* invalid */
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } അല്ലെങ്കിൽ എങ്കിൽ (f) {
                എടുക്കുക_arg(&arg, st, ap);
            } അല്ലെങ്കിൽ {
                തിരികെ 0;
            }
        }

        എങ്കിൽ (!f) {
            തുടരുക;
        }

        z = buf + വലുപ്പം(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        എങ്കിൽ (fl & ഇടത്_ADJ) {
            fl &= ~ZERO_PAD;
        }

        എങ്കിൽ (t == 'n') {
            എങ്കിൽ (!arg.p) {
                തുടരുക;
            }
            തിരഞ്ഞെടുപ്പ് (ps) {
            അവസ്ഥ BARE:
                *(പൂർണ്ണസംഖ്യ *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ LPRE:
                *(long *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ LLPRE:
                *(long long *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ HPRE:
                *(unsigned short *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ HHPRE:
                *(unsigned അക്ഷരം *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ ZTPRE:
                *(word_t *)arg.p = cnt;
                നിർത്തുക;
            അവസ്ഥ JPRE:
                *(word_t *)arg.p = cnt;
                നിർത്തുക;
            }
            തുടരുക;
        } അല്ലെങ്കിൽ എങ്കിൽ (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } അല്ലെങ്കിൽ എങ്കിൽ (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            എങ്കിൽ (p < 0 && *z) {
                തിരികെ -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } അല്ലെങ്കിൽ {
            തിരഞ്ഞെടുപ്പ് (t) {
            അവസ്ഥ 'p':
                p = MAX(p, 2 * വലുപ്പം(ശൂന്യം *));
                t = 'x';
                fl |= ALT_FORM;
            അവസ്ഥ 'x':
            അവസ്ഥ 'X':
                a = fmt_x(arg.i, z, t & 32);
                എങ്കിൽ (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                നിർത്തുക;
            അവസ്ഥ 'o':
                a = fmt_o(arg.i, z);
                എങ്കിൽ ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                നിർത്തുക;
            അവസ്ഥ 'd':
            അവസ്ഥ 'i':
                pl = 1;
                എങ്കിൽ (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } അല്ലെങ്കിൽ എങ്കിൽ (fl & MARK_POS) {
                    prefix++;
                } അല്ലെങ്കിൽ എങ്കിൽ (fl & PAD_POS) {
                    prefix += 2;
                } അല്ലെങ്കിൽ {
                    pl = 0;
                }
            അവസ്ഥ 'u':
                a = fmt_u(arg.i, z);
                നിർത്തുക;
            }
            എങ്കിൽ (xp && p < 0) {
                തിരികെ -1; /* overflow */
            }
            എങ്കിൽ (xp) {
                fl &= ~ZERO_PAD;
            }
            എങ്കിൽ (!arg.i && !p) {
                a = z;
            } അല്ലെങ്കിൽ {
                p = MAX(p, z - a + !arg.i);
            }
        }

        എങ്കിൽ (p < z - a) {
            p = z - a;
        }
        എങ്കിൽ (p > INTMAX_MAX - pl) {
            തിരികെ -1; /* overflow */
        }
        എങ്കിൽ (w < pl + p) {
            w = pl + p;
        }
        എങ്കിൽ (w > INTMAX_MAX - cnt) {
            തിരികെ -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ഇടത്_ADJ);

        l = w;
    }

    എങ്കിൽ (f) {
        തിരികെ cnt;
    }
    എങ്കിൽ (!l10n) {
        തിരികെ 0;
    }

    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        എടുക്കുക_arg(nl_arg + i, nl_type[i], ap);
    }
    വേണ്ടി (; i <= NL_ARGMAX && !nl_type[i]; i++);
    എങ്കിൽ (i <= NL_ARGMAX) {
        തിരികെ -1; /* invalid */
    }

    തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vprintf(out_wrap_t *out, സ്ഥിരം അക്ഷരം *fmt, va_list ap)
{
    va_list ap2;
    പൂർണ്ണസംഖ്യ nl_type[NL_ARGMAX + 1] = {0};
    സംഘം arg nl_arg[NL_ARGMAX + 1];
    പൂർണ്ണസംഖ്യ ret;

    /* validate format string */
    va_copy(ap2, ap);
    എങ്കിൽ (printf_core(ശൂന്യ, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        തിരികെ -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    തിരികെ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

പൂർണ്ണസംഖ്യ impl_kvprintf(സ്ഥിരം അക്ഷരം *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .ലിഖ  = do_output_to_putchar,
        .buf    = ശൂന്യ,
        .maxlen = 0,
        .used   = 0
    };

    തിരികെ vprintf(&out_wrap, format, ap);
}

പൂർണ്ണസംഖ്യ impl_ksnvprintf(അക്ഷരം *str, word_t size, സ്ഥിരം അക്ഷരം *format, va_list ap)
{
    എങ്കിൽ (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .ലിഖ  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    പൂർണ്ണസംഖ്യ ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    എങ്കിൽ ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    തിരികെ ret;
}

#endif /* CONFIG_PRINTING */
