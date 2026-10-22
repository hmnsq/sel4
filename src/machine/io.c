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

प्रकारव्याख्या संरचना _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
प्रकारव्याख्या शून्य (*out_write_fn)(out_wrap_t *out, स्थिरांक अक्षर *buf, word_t len);

संरचना _out_wrap_t {
    स्थिरांक out_write_fn लिख;
    अक्षर *स्थिरांक buf;
    स्थिरांक word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
स्थिर शून्य out(out_wrap_t *out_wrap, स्थिरांक अक्षर *buf, word_t len)
{
    जर (out_wrap) {
        out_wrap->लिख(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
स्थिर शून्य do_output_to_putchar(
    UNUSED out_wrap_t *out,
    स्थिरांक अक्षर *buf,
    word_t len)
{
    जर (buf) {
        पर्यंत (len-- > 0) {
            अक्षर_दो(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
स्थिर शून्य do_output_to_buffer(
    out_wrap_t *out,
    स्थिरांक अक्षर *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    जर (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t मुक्त = out->maxlen - out->used;
        जर (len > मुक्त) {
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

स्थिर ओळीत bool_t है_अंक(अक्षर c)
{
    परत c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#define MASK_TYPE(a) (1U<<( a -' '))

#define ALT_FORM     (1U<<('#'-' '))
#define ZERO_PAD     (1U<<('0'-' '))
#define डावा_ADJ     (1U<<('-'-' '))
#define PAD_POS      (1U<<(' '-' '))
#define MARK_POS     (1U<<('+'-' '))
#define GROUPED      (1U<<('\''-' '))

#define FLAGMASK (ALT_FORM|ZERO_PAD|डावा_ADJ|PAD_POS|MARK_POS|GROUPED)

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

स्थिर स्थिरांक अचिन्हित अक्षर states[]['z' - 'A' + 1] = {
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

संघ arg {
    uintmax_t i;
    दीर्घ द्विगुण f;
    शून्य *p;
};

स्थिर शून्य काढा_arg(संघ arg *arg, पूर्णांक type, बहु_सूची *ap)
{
    निवड (type) {
    स्थिती PTR:
        arg->p = बहु_तर्क(*ap, शून्य *);
        थांबा;
    स्थिती INT:
        arg->i = बहु_तर्क(*ap, पूर्णांक);
        थांबा;
    स्थिती UINT:
        arg->i = बहु_तर्क(*ap, अचिन्हित पूर्णांक);
        थांबा;
    स्थिती LONG:
        arg->i = बहु_तर्क(*ap, दीर्घ);
        थांबा;
    स्थिती ULONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ);
        थांबा;
    स्थिती LLONG:
        arg->i = बहु_तर्क(*ap, दीर्घ दीर्घ);
        थांबा;
    स्थिती ULLONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ दीर्घ);
        थांबा;
    स्थिती SHORT:
        arg->i = (लघु)बहु_तर्क(*ap, पूर्णांक);
        थांबा;
    स्थिती USHORT:
        arg->i = (अचिन्हित लघु)बहु_तर्क(*ap, पूर्णांक);
        थांबा;
    स्थिती CHAR:
        arg->i = (चिन्हित अक्षर)बहु_तर्क(*ap, पूर्णांक);
        थांबा;
    स्थिती UCHAR:
        arg->i = (अचिन्हित अक्षर)बहु_तर्क(*ap, पूर्णांक);
        थांबा;
    स्थिती WORDT:
        arg->i = बहु_तर्क(*ap, word_t);
    }
}


स्थिर शून्य pad(out_wrap_t *f, अक्षर c, पूर्णांक w, पूर्णांक l, पूर्णांक fl)
{
    अक्षर pad[32]; /* good enough for what the kernel prints */
    जर (fl & (डावा_ADJ | ZERO_PAD) || l >= w) {
        परत;
    }
    l = w - l;
    स_रखो(pad, c, l > आकार(pad) ? आकार(pad) : l);
    साठी (; l >= आकार(pad); l -= आकार(pad)) {
        out(f, pad, आकार(pad));
    }
    out(f, pad, l);
}

स्थिर स्थिरांक अक्षर xdigits[16] = {
    "0123456789ABCDEF"
};

स्थिर अक्षर *fmt_x(uintmax_t x, अक्षर *s, पूर्णांक lower)
{
    साठी (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    परत s;
}

स्थिर अक्षर *fmt_o(uintmax_t x, अक्षर *s)
{
    साठी (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    परत s;
}

स्थिर अक्षर *fmt_u(uintmax_t x, अक्षर *s)
{
    पर्यंत (0 != x) {
#जर defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        अचिन्हित पूर्णांक rem = x - (((q << 2) + q) << 1); // rem = x - 10q */
        जर (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            प्रतिज्ञा(rem <= 9); /* there must be no rounding error now */
        }
#अन्यथा
        uintmax_t q = x / 10;
        अचिन्हित पूर्णांक rem = x % 10;
#endif
        *--s = '0' + rem;
        x = q;
    }
    परत s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#define द_द्वि_पूर्ण 113

#define NL_ARGMAX 9

स्थिर पूर्णांक getint(अक्षर **s)
{
    पूर्णांक i;
    साठी (i = 0; है_अंक(**s); (*s)++) {
        जर (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } अन्यथा {
            i = 10 * i + DIGIT(**s);
        }
    }
    परत i;
}

स्थिर पूर्णांक printf_core(out_wrap_t *f, स्थिरांक अक्षर *fmt, बहु_सूची *ap, संघ arg *nl_arg, पूर्णांक *nl_type)
{
    अक्षर *a, *z, *s = (अक्षर *)fmt;
    अचिन्हित l10n = 0, fl;
    पूर्णांक w, p, xp;
    संघ arg arg;
    पूर्णांक argpos;
    अचिन्हित st, ps;
    पूर्णांक cnt = 0, l = 0;
    अक्षर buf[आकार(uintmax_t) * 3 + 3 + द_द्वि_पूर्ण / 4];
    स्थिरांक अक्षर *prefix;
    पूर्णांक t, pl;

    साठी (;;) {
        जर (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            परत -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        जर (!*s) {
            थांबा;
        }

        /* Handle literal text and %% format specifiers */
        साठी (a = s; *s && *s != '%'; s++);
        साठी (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        जर (z - a > INTMAX_MAX - cnt) {
            परत -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        जर (l) {
            सुरूठेवा;
        }

        जर (है_अंक(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } अन्यथा {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        साठी (fl = 0; (अचिन्हित)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        जर (*s == '*') {
            जर (है_अंक(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } अन्यथा जर (!l10n) {
                w = f ? बहु_तर्क(*ap, पूर्णांक) : 0;
                s++;
            } अन्यथा {
                परत -1; /* invalid */
            }
            जर (w < 0) {
                fl |= डावा_ADJ;
                w = -w;
            }
        } अन्यथा जर ((w = getint(&s)) < 0) {
            परत -1; /* overflow */
        }

        /* Read precision */
        जर (*s == '.' && s[1] == '*') {
            जर (है_अंक(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } अन्यथा जर (!l10n) {
                p = f ? बहु_तर्क(*ap, पूर्णांक) : 0;
                s += 2;
            } अन्यथा {
                परत -1;/* invalid */
            }
            xp = (p >= 0);
        } अन्यथा जर (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } अन्यथा {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        करा {
            जर (OOB(*s)) {
                परत -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } पर्यंत (st - 1 < STOP);
        जर (!st) {
            परत -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        जर (st == NOARG) {
            जर (argpos >= 0) {
                परत -1; /* invalid */
            }
        } अन्यथा {
            जर (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } अन्यथा जर (f) {
                काढा_arg(&arg, st, ap);
            } अन्यथा {
                परत 0;
            }
        }

        जर (!f) {
            सुरूठेवा;
        }

        z = buf + आकार(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        जर (fl & डावा_ADJ) {
            fl &= ~ZERO_PAD;
        }

        जर (t == 'n') {
            जर (!arg.p) {
                सुरूठेवा;
            }
            निवड (ps) {
            स्थिती BARE:
                *(पूर्णांक *)arg.p = cnt;
                थांबा;
            स्थिती LPRE:
                *(दीर्घ *)arg.p = cnt;
                थांबा;
            स्थिती LLPRE:
                *(दीर्घ दीर्घ *)arg.p = cnt;
                थांबा;
            स्थिती HPRE:
                *(अचिन्हित लघु *)arg.p = cnt;
                थांबा;
            स्थिती HHPRE:
                *(अचिन्हित अक्षर *)arg.p = cnt;
                थांबा;
            स्थिती ZTPRE:
                *(word_t *)arg.p = cnt;
                थांबा;
            स्थिती JPRE:
                *(word_t *)arg.p = cnt;
                थांबा;
            }
            सुरूठेवा;
        } अन्यथा जर (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } अन्यथा जर (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            जर (p < 0 && *z) {
                परत -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } अन्यथा {
            निवड (t) {
            स्थिती 'p':
                p = MAX(p, 2 * आकार(शून्य *));
                t = 'x';
                fl |= ALT_FORM;
            स्थिती 'x':
            स्थिती 'X':
                a = fmt_x(arg.i, z, t & 32);
                जर (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                थांबा;
            स्थिती 'o':
                a = fmt_o(arg.i, z);
                जर ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                थांबा;
            स्थिती 'd':
            स्थिती 'i':
                pl = 1;
                जर (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } अन्यथा जर (fl & MARK_POS) {
                    prefix++;
                } अन्यथा जर (fl & PAD_POS) {
                    prefix += 2;
                } अन्यथा {
                    pl = 0;
                }
            स्थिती 'u':
                a = fmt_u(arg.i, z);
                थांबा;
            }
            जर (xp && p < 0) {
                परत -1; /* overflow */
            }
            जर (xp) {
                fl &= ~ZERO_PAD;
            }
            जर (!arg.i && !p) {
                a = z;
            } अन्यथा {
                p = MAX(p, z - a + !arg.i);
            }
        }

        जर (p < z - a) {
            p = z - a;
        }
        जर (p > INTMAX_MAX - pl) {
            परत -1; /* overflow */
        }
        जर (w < pl + p) {
            w = pl + p;
        }
        जर (w > INTMAX_MAX - cnt) {
            परत -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ डावा_ADJ);

        l = w;
    }

    जर (f) {
        परत cnt;
    }
    जर (!l10n) {
        परत 0;
    }

    पूर्णांक i;
    साठी (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        काढा_arg(nl_arg + i, nl_type[i], ap);
    }
    साठी (; i <= NL_ARGMAX && !nl_type[i]; i++);
    जर (i <= NL_ARGMAX) {
        परत -1; /* invalid */
    }

    परत 1;
}

स्थिर पूर्णांक भ_लिखो(out_wrap_t *out, स्थिरांक अक्षर *fmt, बहु_सूची ap)
{
    बहु_सूची ap2;
    पूर्णांक nl_type[NL_ARGMAX + 1] = {0};
    संघ arg nl_arg[NL_ARGMAX + 1];
    पूर्णांक ret;

    /* validate format string */
    va_copy(ap2, ap);
    जर (printf_core(रिक्त, fmt, &ap2, nl_arg, nl_type) < 0) {
        बहु_पूर्ण(ap2);
        परत -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    बहु_पूर्ण(ap2);
    परत ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

पूर्णांक impl_kvprintf(स्थिरांक अक्षर *format, बहु_सूची ap)
{
    out_wrap_t out_wrap =  {
        .लिख  = do_output_to_putchar,
        .buf    = रिक्त,
        .maxlen = 0,
        .used   = 0
    };

    परत भ_लिखो(&out_wrap, format, ap);
}

पूर्णांक impl_ksnvprintf(अक्षर *str, word_t size, स्थिरांक अक्षर *format, बहु_सूची ap)
{
    जर (!str) {
        size = 0;
    }

    out_wrap_t out_wrap =  {
        .लिख  = do_output_to_buffer,
        .buf    = str,
        .maxlen = size,
        .used   = 0
    };

    पूर्णांक ret = भ_लिखो(&out_wrap, format, ap);

    /* We return the number of characters written into the buffer, excluding the
     * terminating null char. However, we do never write more than 'size' bytes,
     * that includes the terminating null char. If the output was truncated due
     * to this limit, then the return value is the number of chars excluding the
     * terminating null byte, which would have been written to the buffer, if
     * enough space had been available. Thus, a return value of 'size' or more
     * means that the output was truncated.
     */
    जर ((ret > 0) && (size > 0)) {
        str[(ret < size) ? ret : size - 1] = '\0';
    }

    परत ret;
}

#endif /* CONFIG_PRINTING */
