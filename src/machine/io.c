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

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <machine/io.h>

#ifdef CONFIG_PRINTING

#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

પ્રકારવ્યાખ્યા સંરચના _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
પ્રકારવ્યાખ્યા શૂન્ય (*out_write_fn)(out_wrap_t *out, અચલ અક્ષર *buf, word_t len);

સંરચના _out_wrap_t {
    અચલ out_write_fn લિખ;
    અક્ષર *અચલ buf;
    અચલ word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
સ્થિર શૂન્ય out(out_wrap_t *out_wrap, અચલ અક્ષર *buf, word_t len)
{
    જો (out_wrap) {
        out_wrap->લિખ(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
સ્થિર શૂન્ય do_output_to_putchar(
    UNUSED out_wrap_t *out,
    અચલ અક્ષર *buf,
    word_t len)
{
    જો (buf) {
        જ્યારે (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
સ્થિર શૂન્ય do_output_to_buffer(
    out_wrap_t *out,
    અચલ અક્ષર *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    જો (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t મુક્ત = out->maxlen - out->used;
        જો (len > મુક્ત) {
            len = મુક્ત;
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

સ્થિર અંતઃરેખ bool_t isdigit(અક્ષર c)
{
    પાછા c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define ડાબું_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|ડાબું_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

ગણના {
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

સ્થિર અચલ unsigned અક્ષર states[]['z' - 'A' + 1] = {
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

સંઘ arg {
    uintmax_t i;
    long દ્વિગુણ f;
    શૂન્ય *p;
};

સ્થિર શૂન્ય કાઢો_arg(સંઘ arg *arg, પૂર્ણાંક type, va_list *ap)
{
    પસંદગી (type) {
    સ્થિતિ PTR:
        arg->p = va_arg(*ap, શૂન્ય *);
        તોડો;
    સ્થિતિ INT:
        arg->i = va_arg(*ap, પૂર્ણાંક);
        તોડો;
    સ્થિતિ UINT:
        arg->i = va_arg(*ap, unsigned પૂર્ણાંક);
        તોડો;
    સ્થિતિ LONG:
        arg->i = va_arg(*ap, long);
        તોડો;
    સ્થિતિ ULONG:
        arg->i = va_arg(*ap, unsigned long);
        તોડો;
    સ્થિતિ LLONG:
        arg->i = va_arg(*ap, long long);
        તોડો;
    સ્થિતિ ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        તોડો;
    સ્થિતિ SHORT:
        arg->i = (short)va_arg(*ap, પૂર્ણાંક);
        તોડો;
    સ્થિતિ USHORT:
        arg->i = (unsigned short)va_arg(*ap, પૂર્ણાંક);
        તોડો;
    સ્થિતિ CHAR:
        arg->i = (signed અક્ષર)va_arg(*ap, પૂર્ણાંક);
        તોડો;
    સ્થિતિ UCHAR:
        arg->i = (unsigned અક્ષર)va_arg(*ap, પૂર્ણાંક);
        તોડો;
    સ્થિતિ WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


સ્થિર શૂન્ય pad(out_wrap_t *f, અક્ષર c, પૂર્ણાંક w, પૂર્ણાંક l, પૂર્ણાંક fl)
{
    અક્ષર pad[32]; /* good enough for what the kernel prints */
    જો (fl & (ડાબું_ADJ | ZERO_PAD) || l >= w) {
        પાછા;
    }
    l = w - l;
    memset(pad, c, l > કદ(pad) ? કદ(pad) : l);
    માટે (; l >= કદ(pad); l -= કદ(pad)) {
        out(f, pad, કદ(pad));
    }
    out(f, pad, l);
}

સ્થિર અચલ અક્ષર xdigits[16] = {
    "0123456789ABCDEF"
};

સ્થિર અક્ષર *fmt_x(uintmax_t x, અક્ષર *s, પૂર્ણાંક lower)
{
    માટે (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    પાછા s;
}

સ્થિર અક્ષર *fmt_o(uintmax_t x, અક્ષર *s)
{
    માટે (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    પાછા s;
}

સ્થિર અક્ષર *fmt_u(uintmax_t x, અક્ષર *s)
{
    જ્યારે (0 != x) {
#જો defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned પૂર્ણાંક rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        જો (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            પ્રતિજ્ઞા(rem <= 9); /* there must be no rounding error now */
        }
#નહીંતર
        uintmax_t q = x / 10;
        unsigned પૂર્ણાંક rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    પાછા s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

સ્થિર પૂર્ણાંક getint(અક્ષર **s)
{
    પૂર્ણાંક i;
    માટે (i = 0; isdigit(**s); (*s)++) {
        જો (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } નહીંતર {
            i = 10 * i + DIGIT(**s);
        }
    }
    પાછા i;
}

સ્થિર પૂર્ણાંક printf_core(out_wrap_t *f, અચલ અક્ષર *fmt, va_list *ap, સંઘ arg *nl_arg, પૂર્ણાંક *nl_type)
{
    અક્ષર *a, *z, *s = (અક્ષર *)fmt;
    unsigned l10n = 0, fl;
    પૂર્ણાંક w, p, xp;
    સંઘ arg arg;
    પૂર્ણાંક argpos;
    unsigned st, ps;
    પૂર્ણાંક cnt = 0, l = 0;
    અક્ષર buf[કદ(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    અચલ અક્ષર *prefix;
    પૂર્ણાંક t, pl;

    માટે (;;) {
        જો (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            પાછા -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        જો (!*s) {
            તોડો;
        }

        /* Handle literal text and %% format specifiers */
        માટે (a = s; *s && *s != '%'; s++);
        માટે (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        જો (z - a > INTMAX_MAX - cnt) {
            પાછા -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        જો (l) {
            ચાલુ;
        }

        જો (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } નહીંતર {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        માટે (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        જો (*s == '*') {
            જો (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } નહીંતર જો (!l10n) {
                w = f ? va_arg(*ap, પૂર્ણાંક) : 0;
                s++;
            } નહીંતર {
                પાછા -1; /* invalid */
            }
            જો (w < 0) {
                fl |= ડાબું_ADJ;
                w = -w;
            }
        } નહીંતર જો ((w = getint(&s)) < 0) {
            પાછા -1; /* overflow */
        }

        /* Read precision */
        જો (*s == '.' && s[1] == '*') {
            જો (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } નહીંતર જો (!l10n) {
                p = f ? va_arg(*ap, પૂર્ણાંક) : 0;
                s += 2;
            } નહીંતર {
                પાછા -1;/* invalid */
            }
            xp = (p >= 0);
        } નહીંતર જો (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } નહીંતર {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        કરો {
            જો (OOB(*s)) {
                પાછા -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } જ્યારે (st - 1 < STOP);
        જો (!st) {
            પાછા -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        જો (st == NOARG) {
            જો (argpos >= 0) {
                પાછા -1; /* invalid */
            }
        } નહીંતર {
            જો (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } નહીંતર જો (f) {
                કાઢો_arg(&arg, st, ap);
            } નહીંતર {
                પાછા 0;
            }
        }

        જો (!f) {
            ચાલુ;
        }

        z = buf + કદ(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        જો (fl & ડાબું_ADJ) {
            fl &= ~ZERO_PAD;
        }

        જો (t == 'n') {
            જો (!arg.p) {
                ચાલુ;
            }
            પસંદગી (ps) {
            સ્થિતિ BARE:
                *(પૂર્ણાંક *)arg.p = cnt;
                તોડો;
            સ્થિતિ LPRE:
                *(long *)arg.p = cnt;
                તોડો;
            સ્થિતિ LLPRE:
                *(long long *)arg.p = cnt;
                તોડો;
            સ્થિતિ HPRE:
                *(unsigned short *)arg.p = cnt;
                તોડો;
            સ્થિતિ HHPRE:
                *(unsigned અક્ષર *)arg.p = cnt;
                તોડો;
            સ્થિતિ ZTPRE:
                *(word_t *)arg.p = cnt;
                તોડો;
            સ્થિતિ JPRE:
                *(word_t *)arg.p = cnt;
                તોડો;
            }
            ચાલુ;
        } નહીંતર જો (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } નહીંતર જો (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            જો (p < 0 && *z) {
                પાછા -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } નહીંતર {
            પસંદગી (t) {
            સ્થિતિ 'p':
                p = MAX(p, 2 * કદ(શૂન્ય *));
                t = 'x';
                fl |= ALT_FORM;
            સ્થિતિ 'x':
            સ્થિતિ 'X':
                a = fmt_x(arg.i, z, t & 32);
                જો (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                તોડો;
            સ્થિતિ 'o':
                a = fmt_o(arg.i, z);
                જો ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                તોડો;
            સ્થિતિ 'd':
            સ્થિતિ 'i':
                pl = 1;
                જો (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } નહીંતર જો (fl & MARK_POS) {
                    prefix++;
                } નહીંતર જો (fl & PAD_POS) {
                    prefix += 2;
                } નહીંતર {
                    pl = 0;
                }
            સ્થિતિ 'u':
                a = fmt_u(arg.i, z);
                તોડો;
            }
            જો (xp && p < 0) {
                પાછા -1; /* overflow */
            }
            જો (xp) {
                fl &= ~ZERO_PAD;
            }
            જો (!arg.i && !p) {
                a = z;
            } નહીંતર {
                p = MAX(p, z - a + !arg.i);
            }
        }

        જો (p < z - a) {
            p = z - a;
        }
        જો (p > INTMAX_MAX - pl) {
            પાછા -1; /* overflow */
        }
        જો (w < pl + p) {
            w = pl + p;
        }
        જો (w > INTMAX_MAX - cnt) {
            પાછા -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ ડાબું_ADJ);

        l = w;
    }

    જો (f) {
        પાછા cnt;
    }
    જો (!l10n) {
        પાછા 0;
    }

    પૂર્ણાંક i;
    માટે (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        કાઢો_arg(nl_arg + i, nl_type[i], ap);
    }
    માટે (; i <= NL_ARGMAX && !nl_type[i]; i++);
    જો (i <= NL_ARGMAX) {
        પાછા -1; /* invalid */
    }

    પાછા 1;
}

સ્થિર પૂર્ણાંક vprintf(out_wrap_t *out, અચલ અક્ષર *fmt, va_list ap)
{
    va_list ap2;
    પૂર્ણાંક nl_type[NL_ARGMAX + 1] = {0};
    સંઘ arg nl_arg[NL_ARGMAX + 1];
    પૂર્ણાંક ret;

    /* validate format string */
    va_copy(ap2, ap);
    જો (printf_core(ખાલી, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        પાછા -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    પાછા ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

પૂર્ણાંક impl_kvprintf(અચલ અક્ષર *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .લિખ  = do_output_to_putchar,
        .buf    = ખાલી,
        .maxlen = 0,
        .used   = 0
    };

    પાછા vprintf(&out_wrap, format, ap);
}

પૂર્ણાંક impl_ksnvprintf(અક્ષર *str, word_t size, અચલ અક્ષર *format, va_list ap)
{
    જો (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .લિખ  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    પૂર્ણાંક ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    જો ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    પાછા ret;
}

#endif /* CONFIG_PRINTING */
