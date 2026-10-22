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

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <machine/io.h>

#ifdef CONFIG_PRINTING

#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

வகைவரை கட்டமைப்பு _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
வகைவரை வெறுமை (*out_write_fn)(out_wrap_t *out, மாறிலி எழுத்து *buf, word_t len);

கட்டமைப்பு _out_wrap_t {
    மாறிலி out_write_fn லிக;
    எழுத்து *மாறிலி buf;
    மாறிலி word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
நிலையான வெறுமை out(out_wrap_t *out_wrap, மாறிலி எழுத்து *buf, word_t len)
{
    எனில் (out_wrap) {
        out_wrap->லிக(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
நிலையான வெறுமை do_output_to_putchar(
    UNUSED out_wrap_t *out,
    மாறிலி எழுத்து *buf,
    word_t len)
{
    எனில் (buf) {
        போது (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
நிலையான வெறுமை do_output_to_buffer(
    out_wrap_t *out,
    மாறிலி எழுத்து *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    எனில் (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t விடுவி = out->maxlen - out->used;
        எனில் (len > விடுவி) {
            len = விடுவி;
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

நிலையான உள்வரி bool_t isdigit(எழுத்து c)
{
    திரும்பு c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define இடது_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|இடது_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

எண்ணிக்கை {
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

நிலையான மாறிலி unsigned எழுத்து states[]['z' - 'A' + 1] = {
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

ஒன்றிப்பு arg {
    uintmax_t i;
    long இரட்டை f;
    வெறுமை *p;
};

நிலையான வெறுமை எடு_arg(ஒன்றிப்பு arg *arg, முழு type, va_list *ap)
{
    தேர்வு (type) {
    நிலை PTR:
        arg->p = va_arg(*ap, வெறுமை *);
        நிறுத்து;
    நிலை INT:
        arg->i = va_arg(*ap, முழு);
        நிறுத்து;
    நிலை UINT:
        arg->i = va_arg(*ap, unsigned முழு);
        நிறுத்து;
    நிலை LONG:
        arg->i = va_arg(*ap, long);
        நிறுத்து;
    நிலை ULONG:
        arg->i = va_arg(*ap, unsigned long);
        நிறுத்து;
    நிலை LLONG:
        arg->i = va_arg(*ap, long long);
        நிறுத்து;
    நிலை ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        நிறுத்து;
    நிலை SHORT:
        arg->i = (short)va_arg(*ap, முழு);
        நிறுத்து;
    நிலை USHORT:
        arg->i = (unsigned short)va_arg(*ap, முழு);
        நிறுத்து;
    நிலை CHAR:
        arg->i = (signed எழுத்து)va_arg(*ap, முழு);
        நிறுத்து;
    நிலை UCHAR:
        arg->i = (unsigned எழுத்து)va_arg(*ap, முழு);
        நிறுத்து;
    நிலை WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


நிலையான வெறுமை pad(out_wrap_t *f, எழுத்து c, முழு w, முழு l, முழு fl)
{
    எழுத்து pad[32]; /* good enough for what the kernel prints */
    எனில் (fl & (இடது_ADJ | ZERO_PAD) || l >= w) {
        திரும்பு;
    }
    l = w - l;
    memset(pad, c, l > அளவு(pad) ? அளவு(pad) : l);
    க்கு (; l >= அளவு(pad); l -= அளவு(pad)) {
        out(f, pad, அளவு(pad));
    }
    out(f, pad, l);
}

நிலையான மாறிலி எழுத்து xdigits[16] = {
    "0123456789ABCDEF"
};

நிலையான எழுத்து *fmt_x(uintmax_t x, எழுத்து *s, முழு lower)
{
    க்கு (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    திரும்பு s;
}

நிலையான எழுத்து *fmt_o(uintmax_t x, எழுத்து *s)
{
    க்கு (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    திரும்பு s;
}

நிலையான எழுத்து *fmt_u(uintmax_t x, எழுத்து *s)
{
    போது (0 != x) {
#எனில் defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned முழு rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        எனில் (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            ப்ரதிஜ்ஞா(rem <= 9); /* there must be no rounding error now */
        }
#இல்லையெனில்
        uintmax_t q = x / 10;
        unsigned முழு rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    திரும்பு s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

நிலையான முழு getint(எழுத்து **s)
{
    முழு i;
    க்கு (i = 0; isdigit(**s); (*s)++) {
        எனில் (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } இல்லையெனில் {
            i = 10 * i + DIGIT(**s);
        }
    }
    திரும்பு i;
}

நிலையான முழு printf_core(out_wrap_t *f, மாறிலி எழுத்து *fmt, va_list *ap, ஒன்றிப்பு arg *nl_arg, முழு *nl_type)
{
    எழுத்து *a, *z, *s = (எழுத்து *)fmt;
    unsigned l10n = 0, fl;
    முழு w, p, xp;
    ஒன்றிப்பு arg arg;
    முழு argpos;
    unsigned st, ps;
    முழு cnt = 0, l = 0;
    எழுத்து buf[அளவு(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    மாறிலி எழுத்து *prefix;
    முழு t, pl;

    க்கு (;;) {
        எனில் (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            திரும்பு -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        எனில் (!*s) {
            நிறுத்து;
        }

        /* Handle literal text and %% format specifiers */
        க்கு (a = s; *s && *s != '%'; s++);
        க்கு (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        எனில் (z - a > INTMAX_MAX - cnt) {
            திரும்பு -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        எனில் (l) {
            தொடர்;
        }

        எனில் (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } இல்லையெனில் {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        க்கு (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        எனில் (*s == '*') {
            எனில் (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } இல்லையெனில் எனில் (!l10n) {
                w = f ? va_arg(*ap, முழு) : 0;
                s++;
            } இல்லையெனில் {
                திரும்பு -1; /* invalid */
            }
            எனில் (w < 0) {
                fl |= இடது_ADJ;
                w = -w;
            }
        } இல்லையெனில் எனில் ((w = getint(&s)) < 0) {
            திரும்பு -1; /* overflow */
        }

        /* Read precision */
        எனில் (*s == '.' && s[1] == '*') {
            எனில் (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } இல்லையெனில் எனில் (!l10n) {
                p = f ? va_arg(*ap, முழு) : 0;
                s += 2;
            } இல்லையெனில் {
                திரும்பு -1;/* invalid */
            }
            xp = (p >= 0);
        } இல்லையெனில் எனில் (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } இல்லையெனில் {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        செய் {
            எனில் (OOB(*s)) {
                திரும்பு -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } போது (st - 1 < STOP);
        எனில் (!st) {
            திரும்பு -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        எனில் (st == NOARG) {
            எனில் (argpos >= 0) {
                திரும்பு -1; /* invalid */
            }
        } இல்லையெனில் {
            எனில் (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } இல்லையெனில் எனில் (f) {
                எடு_arg(&arg, st, ap);
            } இல்லையெனில் {
                திரும்பு 0;
            }
        }

        எனில் (!f) {
            தொடர்;
        }

        z = buf + அளவு(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        எனில் (fl & இடது_ADJ) {
            fl &= ~ZERO_PAD;
        }

        எனில் (t == 'n') {
            எனில் (!arg.p) {
                தொடர்;
            }
            தேர்வு (ps) {
            நிலை BARE:
                *(முழு *)arg.p = cnt;
                நிறுத்து;
            நிலை LPRE:
                *(long *)arg.p = cnt;
                நிறுத்து;
            நிலை LLPRE:
                *(long long *)arg.p = cnt;
                நிறுத்து;
            நிலை HPRE:
                *(unsigned short *)arg.p = cnt;
                நிறுத்து;
            நிலை HHPRE:
                *(unsigned எழுத்து *)arg.p = cnt;
                நிறுத்து;
            நிலை ZTPRE:
                *(word_t *)arg.p = cnt;
                நிறுத்து;
            நிலை JPRE:
                *(word_t *)arg.p = cnt;
                நிறுத்து;
            }
            தொடர்;
        } இல்லையெனில் எனில் (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } இல்லையெனில் எனில் (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            எனில் (p < 0 && *z) {
                திரும்பு -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } இல்லையெனில் {
            தேர்வு (t) {
            நிலை 'p':
                p = MAX(p, 2 * அளவு(வெறுமை *));
                t = 'x';
                fl |= ALT_FORM;
            நிலை 'x':
            நிலை 'X':
                a = fmt_x(arg.i, z, t & 32);
                எனில் (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                நிறுத்து;
            நிலை 'o':
                a = fmt_o(arg.i, z);
                எனில் ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                நிறுத்து;
            நிலை 'd':
            நிலை 'i':
                pl = 1;
                எனில் (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } இல்லையெனில் எனில் (fl & MARK_POS) {
                    prefix++;
                } இல்லையெனில் எனில் (fl & PAD_POS) {
                    prefix += 2;
                } இல்லையெனில் {
                    pl = 0;
                }
            நிலை 'u':
                a = fmt_u(arg.i, z);
                நிறுத்து;
            }
            எனில் (xp && p < 0) {
                திரும்பு -1; /* overflow */
            }
            எனில் (xp) {
                fl &= ~ZERO_PAD;
            }
            எனில் (!arg.i && !p) {
                a = z;
            } இல்லையெனில் {
                p = MAX(p, z - a + !arg.i);
            }
        }

        எனில் (p < z - a) {
            p = z - a;
        }
        எனில் (p > INTMAX_MAX - pl) {
            திரும்பு -1; /* overflow */
        }
        எனில் (w < pl + p) {
            w = pl + p;
        }
        எனில் (w > INTMAX_MAX - cnt) {
            திரும்பு -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ இடது_ADJ);

        l = w;
    }

    எனில் (f) {
        திரும்பு cnt;
    }
    எனில் (!l10n) {
        திரும்பு 0;
    }

    முழு i;
    க்கு (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        எடு_arg(nl_arg + i, nl_type[i], ap);
    }
    க்கு (; i <= NL_ARGMAX && !nl_type[i]; i++);
    எனில் (i <= NL_ARGMAX) {
        திரும்பு -1; /* invalid */
    }

    திரும்பு 1;
}

நிலையான முழு vprintf(out_wrap_t *out, மாறிலி எழுத்து *fmt, va_list ap)
{
    va_list ap2;
    முழு nl_type[NL_ARGMAX + 1] = {0};
    ஒன்றிப்பு arg nl_arg[NL_ARGMAX + 1];
    முழு ret;

    /* validate format string */
    va_copy(ap2, ap);
    எனில் (printf_core(வெற்று, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        திரும்பு -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    திரும்பு ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

முழு impl_kvprintf(மாறிலி எழுத்து *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .லிக  = do_output_to_putchar,
        .buf    = வெற்று,
        .maxlen = 0,
        .used   = 0
    };

    திரும்பு vprintf(&out_wrap, format, ap);
}

முழு impl_ksnvprintf(எழுத்து *str, word_t size, மாறிலி எழுத்து *format, va_list ap)
{
    எனில் (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .லிக  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    முழு ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    எனில் ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    திரும்பு ret;
}

#endif /* CONFIG_PRINTING */
