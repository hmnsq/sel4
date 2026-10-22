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

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/io.h>

#ifdef CONFIG_PRINTING

#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

प्रकारनिर्देश संरचना _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
प्रकारनिर्देश शून्य (*out_write_fn)(out_wrap_t *out, ध्रुव वर्ण *buf, word_t len);

संरचना _out_wrap_t {
    ध्रुव out_write_fn लिख;
    वर्ण *ध्रुव buf;
    ध्रुव word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
स्थिर शून्य out(out_wrap_t *out_wrap, ध्रुव वर्ण *buf, word_t len)
{
    यदि (out_wrap) {
        out_wrap->लिख(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
स्थिर शून्य do_output_to_putchar(
    UNUSED out_wrap_t *out,
    ध्रुव वर्ण *buf,
    word_t len)
{
    यदि (buf) {
        यावत् (len-- > 0) {
            putchar(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
स्थिर शून्य do_output_to_buffer(
    out_wrap_t *out,
    ध्रुव वर्ण *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    यदि (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t मुक्त = out->maxlen - out->used;
        यदि (len > मुक्त) {
            len = मुक्त;
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

स्थिर अन्तःस्थ bool_t isdigit(वर्ण c)
{
    प्रत्यागम c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define बायाँ_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|बायाँ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

गणना {
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

स्थिर ध्रुव unsigned वर्ण states[]['z' - 'A' + 1] = {
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

सङ्घ arg {
    uintmax_t i;
    long द्विगुण f;
    शून्य *p;
};

स्थिर शून्य निकालो_arg(सङ्घ arg *arg, पूर्णाङ्क type, va_list *ap)
{
    विकल्प (type) {
    पक्ष PTR:
        arg->p = va_arg(*ap, शून्य *);
        भङ्ग;
    पक्ष INT:
        arg->i = va_arg(*ap, पूर्णाङ्क);
        भङ्ग;
    पक्ष UINT:
        arg->i = va_arg(*ap, unsigned पूर्णाङ्क);
        भङ्ग;
    पक्ष LONG:
        arg->i = va_arg(*ap, long);
        भङ्ग;
    पक्ष ULONG:
        arg->i = va_arg(*ap, unsigned long);
        भङ्ग;
    पक्ष LLONG:
        arg->i = va_arg(*ap, long long);
        भङ्ग;
    पक्ष ULLONG:
        arg->i = va_arg(*ap, unsigned long long);
        भङ्ग;
    पक्ष SHORT:
        arg->i = (short)va_arg(*ap, पूर्णाङ्क);
        भङ्ग;
    पक्ष USHORT:
        arg->i = (unsigned short)va_arg(*ap, पूर्णाङ्क);
        भङ्ग;
    पक्ष CHAR:
        arg->i = (signed वर्ण)va_arg(*ap, पूर्णाङ्क);
        भङ्ग;
    पक्ष UCHAR:
        arg->i = (unsigned वर्ण)va_arg(*ap, पूर्णाङ्क);
        भङ्ग;
    पक्ष WORDT:
        arg->i = va_arg(*ap, word_t);
    }
}


स्थिर शून्य pad(out_wrap_t *f, वर्ण c, पूर्णाङ्क w, पूर्णाङ्क l, पूर्णाङ्क fl)
{
    वर्ण pad[32]; /* good enough for what the kernel prints */
    यदि (fl & (बायाँ_ADJ | ZERO_PAD) || l >= w) {
        प्रत्यागम;
    }
    l = w - l;
    memset(pad, c, l > परिमाण(pad) ? परिमाण(pad) : l);
    प्रति (; l >= परिमाण(pad); l -= परिमाण(pad)) {
        out(f, pad, परिमाण(pad));
    }
    out(f, pad, l);
}

स्थिर ध्रुव वर्ण xdigits[16] = {
    "0123456789ABCDEF"
};

स्थिर वर्ण *fmt_x(uintmax_t x, वर्ण *s, पूर्णाङ्क lower)
{
    प्रति (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    प्रत्यागम s;
}

स्थिर वर्ण *fmt_o(uintmax_t x, वर्ण *s)
{
    प्रति (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    प्रत्यागम s;
}

स्थिर वर्ण *fmt_u(uintmax_t x, वर्ण *s)
{
    यावत् (0 != x) {
#यदि defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        unsigned पूर्णाङ्क rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        यदि (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            प्रतिज्ञा(rem <= 9); /* there must be no rounding error now */
        }
#अन्यथा
        uintmax_t q = x / 10;
        unsigned पूर्णाङ्क rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    प्रत्यागम s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define LDBL_MANT_DIG 113

#define NL_ARGMAX 9

स्थिर पूर्णाङ्क getint(वर्ण **s)
{
    पूर्णाङ्क i;
    प्रति (i = 0; isdigit(**s); (*s)++) {
        यदि (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } अन्यथा {
            i = 10 * i + DIGIT(**s);
        }
    }
    प्रत्यागम i;
}

स्थिर पूर्णाङ्क printf_core(out_wrap_t *f, ध्रुव वर्ण *fmt, va_list *ap, सङ्घ arg *nl_arg, पूर्णाङ्क *nl_type)
{
    वर्ण *a, *z, *s = (वर्ण *)fmt;
    unsigned l10n = 0, fl;
    पूर्णाङ्क w, p, xp;
    सङ्घ arg arg;
    पूर्णाङ्क argpos;
    unsigned st, ps;
    पूर्णाङ्क cnt = 0, l = 0;
    वर्ण buf[परिमाण(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
    ध्रुव वर्ण *prefix;
    पूर्णाङ्क t, pl;

    प्रति (;;) {
        यदि (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            प्रत्यागम -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        यदि (!*s) {
            भङ्ग;
        }

        /* Handle literal text and %% format specifiers */
        प्रति (a = s; *s && *s != '%'; s++);
        प्रति (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        यदि (z - a > INTMAX_MAX - cnt) {
            प्रत्यागम -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        यदि (l) {
            अनुवर्त;
        }

        यदि (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } अन्यथा {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        प्रति (fl = 0; (unsigned)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        यदि (*s == '*') {
            यदि (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } अन्यथा यदि (!l10n) {
                w = f ? va_arg(*ap, पूर्णाङ्क) : 0;
                s++;
            } अन्यथा {
                प्रत्यागम -1; /* invalid */
            }
            यदि (w < 0) {
                fl |= बायाँ_ADJ;
                w = -w;
            }
        } अन्यथा यदि ((w = getint(&s)) < 0) {
            प्रत्यागम -1; /* overflow */
        }

        /* Read precision */
        यदि (*s == '.' && s[1] == '*') {
            यदि (isdigit(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } अन्यथा यदि (!l10n) {
                p = f ? va_arg(*ap, पूर्णाङ्क) : 0;
                s += 2;
            } अन्यथा {
                प्रत्यागम -1;/* invalid */
            }
            xp = (p >= 0);
        } अन्यथा यदि (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } अन्यथा {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        कुरु {
            यदि (OOB(*s)) {
                प्रत्यागम -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } यावत् (st - 1 < STOP);
        यदि (!st) {
            प्रत्यागम -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        यदि (st == NOARG) {
            यदि (argpos >= 0) {
                प्रत्यागम -1; /* invalid */
            }
        } अन्यथा {
            यदि (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } अन्यथा यदि (f) {
                निकालो_arg(&arg, st, ap);
            } अन्यथा {
                प्रत्यागम 0;
            }
        }

        यदि (!f) {
            अनुवर्त;
        }

        z = buf + परिमाण(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        यदि (fl & बायाँ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        यदि (t == 'n') {
            यदि (!arg.p) {
                अनुवर्त;
            }
            विकल्प (ps) {
            पक्ष BARE:
                *(पूर्णाङ्क *)arg.p = cnt;
                भङ्ग;
            पक्ष LPRE:
                *(long *)arg.p = cnt;
                भङ्ग;
            पक्ष LLPRE:
                *(long long *)arg.p = cnt;
                भङ्ग;
            पक्ष HPRE:
                *(unsigned short *)arg.p = cnt;
                भङ्ग;
            पक्ष HHPRE:
                *(unsigned वर्ण *)arg.p = cnt;
                भङ्ग;
            पक्ष ZTPRE:
                *(word_t *)arg.p = cnt;
                भङ्ग;
            पक्ष JPRE:
                *(word_t *)arg.p = cnt;
                भङ्ग;
            }
            अनुवर्त;
        } अन्यथा यदि (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } अन्यथा यदि (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            यदि (p < 0 && *z) {
                प्रत्यागम -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } अन्यथा {
            विकल्प (t) {
            पक्ष 'p':
                p = MAX(p, 2 * परिमाण(शून्य *));
                t = 'x';
                fl |= ALT_FORM;
            पक्ष 'x':
            पक्ष 'X':
                a = fmt_x(arg.i, z, t & 32);
                यदि (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                भङ्ग;
            पक्ष 'o':
                a = fmt_o(arg.i, z);
                यदि ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                भङ्ग;
            पक्ष 'd':
            पक्ष 'i':
                pl = 1;
                यदि (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } अन्यथा यदि (fl & MARK_POS) {
                    prefix++;
                } अन्यथा यदि (fl & PAD_POS) {
                    prefix += 2;
                } अन्यथा {
                    pl = 0;
                }
            पक्ष 'u':
                a = fmt_u(arg.i, z);
                भङ्ग;
            }
            यदि (xp && p < 0) {
                प्रत्यागम -1; /* overflow */
            }
            यदि (xp) {
                fl &= ~ZERO_PAD;
            }
            यदि (!arg.i && !p) {
                a = z;
            } अन्यथा {
                p = MAX(p, z - a + !arg.i);
            }
        }

        यदि (p < z - a) {
            p = z - a;
        }
        यदि (p > INTMAX_MAX - pl) {
            प्रत्यागम -1; /* overflow */
        }
        यदि (w < pl + p) {
            w = pl + p;
        }
        यदि (w > INTMAX_MAX - cnt) {
            प्रत्यागम -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ बायाँ_ADJ);

        l = w;
    }

    यदि (f) {
        प्रत्यागम cnt;
    }
    यदि (!l10n) {
        प्रत्यागम 0;
    }

    पूर्णाङ्क i;
    प्रति (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        निकालो_arg(nl_arg + i, nl_type[i], ap);
    }
    प्रति (; i <= NL_ARGMAX && !nl_type[i]; i++);
    यदि (i <= NL_ARGMAX) {
        प्रत्यागम -1; /* invalid */
    }

    प्रत्यागम 1;
}

स्थिर पूर्णाङ्क vprintf(out_wrap_t *out, ध्रुव वर्ण *fmt, va_list ap)
{
    va_list ap2;
    पूर्णाङ्क nl_type[NL_ARGMAX + 1] = {0};
    सङ्घ arg nl_arg[NL_ARGMAX + 1];
    पूर्णाङ्क ret;

    /* validate format string */
    va_copy(ap2, ap);
    यदि (printf_core(रिक्त, fmt, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        प्रत्यागम -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);
    प्रत्यागम ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

पूर्णाङ्क impl_kvprintf(ध्रुव वर्ण *format, va_list ap)
{
    out_wrap_t out_wrap =  {
        .लिख  = do_output_to_putchar,
        .buf    = रिक्त,
        .maxlen = 0,
        .used   = 0
    };

    प्रत्यागम vprintf(&out_wrap, format, ap);
}

पूर्णाङ्क impl_ksnvprintf(वर्ण *str, word_t size, ध्रुव वर्ण *format, va_list ap)
{
    यदि (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .लिख  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    पूर्णाङ्क ret = vprintf(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    यदि ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    प्रत्यागम ret;
}

#endif /* CONFIG_PRINTING */
