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
प्रकारनिर्देश शून्य (*out_write_fn)(out_wrap_t *out, अचर वर्ण *buf, word_t len);

संरचना _out_wrap_t {
    अचर out_write_fn लिख;
    वर्ण *अचर buf;
    अचर word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
स्थिर शून्य out(out_wrap_t *out_wrap, अचर वर्ण *buf, word_t len)
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
    अचर वर्ण *buf,
    word_t len)
{
    यदि (buf) {
        जबसम्म (len-- > 0) {
            अक्षर_दो(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
स्थिर शून्य do_output_to_buffer(
    out_wrap_t *out,
    अचर वर्ण *buf,
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
        स_नकल(&out->buf[out->used], buf, len);
        out->used += len;
    }
}

/*
 *------------------------------------------------------------------------------
 * printf() core implementation
 *------------------------------------------------------------------------------
 */

स्थिर अन्तःस्थ bool_t है_अंक(वर्ण c)
{
    फर्काऊ c >= '0' &&
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

स्थिर अचर अचिन्हित वर्ण states[]['z' - 'A' + 1] = {
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

#define OOB(x) ((अचिन्हित)(x)-'A' > 'z'-'A')
#define DIGIT(c) (c - '0')

सङ्घ arg {
    uintmax_t i;
    दीर्घ द्विगुण f;
    शून्य *p;
};

स्थिर शून्य झिक_arg(सङ्घ arg *arg, पूर्णाङ्क type, बहु_सूची *ap)
{
    विकल्प (type) {
    पक्ष PTR:
        arg->p = बहु_तर्क(*ap, शून्य *);
        रोक;
    पक्ष INT:
        arg->i = बहु_तर्क(*ap, पूर्णाङ्क);
        रोक;
    पक्ष UINT:
        arg->i = बहु_तर्क(*ap, अचिन्हित पूर्णाङ्क);
        रोक;
    पक्ष LONG:
        arg->i = बहु_तर्क(*ap, दीर्घ);
        रोक;
    पक्ष ULONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ);
        रोक;
    पक्ष LLONG:
        arg->i = बहु_तर्क(*ap, दीर्घ दीर्घ);
        रोक;
    पक्ष ULLONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ दीर्घ);
        रोक;
    पक्ष SHORT:
        arg->i = (लघु)बहु_तर्क(*ap, पूर्णाङ्क);
        रोक;
    पक्ष USHORT:
        arg->i = (अचिन्हित लघु)बहु_तर्क(*ap, पूर्णाङ्क);
        रोक;
    पक्ष CHAR:
        arg->i = (चिन्हित वर्ण)बहु_तर्क(*ap, पूर्णाङ्क);
        रोक;
    पक्ष UCHAR:
        arg->i = (अचिन्हित वर्ण)बहु_तर्क(*ap, पूर्णाङ्क);
        रोक;
    पक्ष WORDT:
        arg->i = बहु_तर्क(*ap, word_t);
    }
}


स्थिर शून्य pad(out_wrap_t *f, वर्ण c, पूर्णाङ्क w, पूर्णाङ्क l, पूर्णाङ्क fl)
{
    वर्ण pad[32]; /* good enough for what the kernel prints */
    यदि (fl & (बायाँ_ADJ | ZERO_PAD) || l >= w) {
        फर्काऊ;
    }
    l = w - l;
    स_रखो(pad, c, l > परिमाण(pad) ? परिमाण(pad) : l);
    लागि (; l >= परिमाण(pad); l -= परिमाण(pad)) {
        out(f, pad, परिमाण(pad));
    }
    out(f, pad, l);
}

स्थिर अचर वर्ण xdigits[16] = {
    "0123456789ABCDEF"
};

स्थिर वर्ण *fmt_x(uintmax_t x, वर्ण *s, पूर्णाङ्क lower)
{
    लागि (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    फर्काऊ s;
}

स्थिर वर्ण *fmt_o(uintmax_t x, वर्ण *s)
{
    लागि (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    फर्काऊ s;
}

स्थिर वर्ण *fmt_u(uintmax_t x, वर्ण *s)
{
    जबसम्म (0 != x) {
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
        अचिन्हित पूर्णाङ्क rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        यदि (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            प्रतिज्ञा(rem <= 9); /* there must be no rounding error now */
        }
#अन्यथा
        uintmax_t q = x / 10;
        अचिन्हित पूर्णाङ्क rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    फर्काऊ s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define द_द्वि_पूर्ण 113

#define NL_ARGMAX 9

स्थिर पूर्णाङ्क getint(वर्ण **s)
{
    पूर्णाङ्क i;
    लागि (i = 0; है_अंक(**s); (*s)++) {
        यदि (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } अन्यथा {
            i = 10 * i + DIGIT(**s);
        }
    }
    फर्काऊ i;
}

स्थिर पूर्णाङ्क printf_core(out_wrap_t *f, अचर वर्ण *fmt, बहु_सूची *ap, सङ्घ arg *nl_arg, पूर्णाङ्क *nl_type)
{
    वर्ण *a, *z, *s = (वर्ण *)fmt;
    अचिन्हित l10n = 0, fl;
    पूर्णाङ्क w, p, xp;
    सङ्घ arg arg;
    पूर्णाङ्क argpos;
    अचिन्हित st, ps;
    पूर्णाङ्क cnt = 0, l = 0;
    वर्ण buf[परिमाण(uintmax_t) * 3 + 3 + द_द्वि_पूर्ण / 4];
    अचर वर्ण *prefix;
    पूर्णाङ्क t, pl;

    लागि (;;) {
        यदि (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            फर्काऊ -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        यदि (!*s) {
            रोक;
        }

        /* Handle literal text and %% format specifiers */
        लागि (a = s; *s && *s != '%'; s++);
        लागि (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        यदि (z - a > INTMAX_MAX - cnt) {
            फर्काऊ -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        यदि (l) {
            जारी;
        }

        यदि (है_अंक(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } अन्यथा {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        लागि (fl = 0; (अचिन्हित)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        यदि (*s == '*') {
            यदि (है_अंक(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } अन्यथा यदि (!l10n) {
                w = f ? बहु_तर्क(*ap, पूर्णाङ्क) : 0;
                s++;
            } अन्यथा {
                फर्काऊ -1; /* invalid */
            }
            यदि (w < 0) {
                fl |= बायाँ_ADJ;
                w = -w;
            }
        } अन्यथा यदि ((w = getint(&s)) < 0) {
            फर्काऊ -1; /* overflow */
        }

        /* Read precision */
        यदि (*s == '.' && s[1] == '*') {
            यदि (है_अंक(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } अन्यथा यदि (!l10n) {
                p = f ? बहु_तर्क(*ap, पूर्णाङ्क) : 0;
                s += 2;
            } अन्यथा {
                फर्काऊ -1;/* invalid */
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
        गर {
            यदि (OOB(*s)) {
                फर्काऊ -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } जबसम्म (st - 1 < STOP);
        यदि (!st) {
            फर्काऊ -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        यदि (st == NOARG) {
            यदि (argpos >= 0) {
                फर्काऊ -1; /* invalid */
            }
        } अन्यथा {
            यदि (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } अन्यथा यदि (f) {
                झिक_arg(&arg, st, ap);
            } अन्यथा {
                फर्काऊ 0;
            }
        }

        यदि (!f) {
            जारी;
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
                जारी;
            }
            विकल्प (ps) {
            पक्ष BARE:
                *(पूर्णाङ्क *)arg.p = cnt;
                रोक;
            पक्ष LPRE:
                *(दीर्घ *)arg.p = cnt;
                रोक;
            पक्ष LLPRE:
                *(दीर्घ दीर्घ *)arg.p = cnt;
                रोक;
            पक्ष HPRE:
                *(अचिन्हित लघु *)arg.p = cnt;
                रोक;
            पक्ष HHPRE:
                *(अचिन्हित वर्ण *)arg.p = cnt;
                रोक;
            पक्ष ZTPRE:
                *(word_t *)arg.p = cnt;
                रोक;
            पक्ष JPRE:
                *(word_t *)arg.p = cnt;
                रोक;
            }
            जारी;
        } अन्यथा यदि (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } अन्यथा यदि (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            यदि (p < 0 && *z) {
                फर्काऊ -1; /* overflow */
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
                रोक;
            पक्ष 'o':
                a = fmt_o(arg.i, z);
                यदि ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                रोक;
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
                रोक;
            }
            यदि (xp && p < 0) {
                फर्काऊ -1; /* overflow */
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
            फर्काऊ -1; /* overflow */
        }
        यदि (w < pl + p) {
            w = pl + p;
        }
        यदि (w > INTMAX_MAX - cnt) {
            फर्काऊ -1; /* overflow */
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
        फर्काऊ cnt;
    }
    यदि (!l10n) {
        फर्काऊ 0;
    }

    पूर्णाङ्क i;
    लागि (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        झिक_arg(nl_arg + i, nl_type[i], ap);
    }
    लागि (; i <= NL_ARGMAX && !nl_type[i]; i++);
    यदि (i <= NL_ARGMAX) {
        फर्काऊ -1; /* invalid */
    }

    फर्काऊ 1;
}

स्थिर पूर्णाङ्क भ_लिखो(out_wrap_t *out, अचर वर्ण *fmt, बहु_सूची ap)
{
    बहु_सूची ap2;
    पूर्णाङ्क nl_type[NL_ARGMAX + 1] = {0};
    सङ्घ arg nl_arg[NL_ARGMAX + 1];
    पूर्णाङ्क ret;

    /* validate format string */
    va_copy(ap2, ap);
    यदि (printf_core(रिक्त, fmt, &ap2, nl_arg, nl_type) < 0) {
        बहु_पूर्ण(ap2);
        फर्काऊ -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    बहु_पूर्ण(ap2);
    फर्काऊ ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

पूर्णाङ्क impl_kvprintf(अचर वर्ण *format, बहु_सूची ap)
{
    out_wrap_t out_wrap =  {
        .लिख  = do_output_to_putchar,
        .buf    = रिक्त,
        .maxlen = 0,
        .used   = 0
    };

    फर्काऊ भ_लिखो(&out_wrap, format, ap);
}

पूर्णाङ्क impl_ksnvprintf(वर्ण *str, word_t size, अचर वर्ण *format, बहु_सूची ap)
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

    पूर्णाङ्क ret = भ_लिखो(&out_wrap, format, ap);

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

    फर्काऊ ret;
}

#endif /* CONFIG_PRINTING */
