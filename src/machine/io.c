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

#समावेश <config.h>
#समावेश <machine/io.h>

#अगर_घोषित CONFIG_PRINTING

#समावेश <stdarg.h>
#समावेश <stdint.h>

/*
 *------------------------------------------------------------------------------
 * printf() core output channel management
 *------------------------------------------------------------------------------
 */

प्रकार काष्ठा _out_wrap_t  out_wrap_t;

/* handler defining how/where to actually output a buffer */
प्रकार व्योम (*out_write_fn)(out_wrap_t *out, स्थिर अक्षर *buf, word_t len);

काष्ठा _out_wrap_t {
    स्थिर out_write_fn ग_लिखो;
    अक्षर *स्थिर buf;
    स्थिर word_t maxlen;
    word_t used;
};

/* printf_core() and its helpers call this to actually output something. The
 * parameter 'out_wrap' cam be NULL, e.g. when printf_core() is just caller to
 * validate the format string. In this case we do nothing.
 */
अटल व्योम out(out_wrap_t *out_wrap, स्थिर अक्षर *buf, word_t len)
{
    अगर (out_wrap) {
        out_wrap->ग_लिखो(out_wrap, buf, len);
    }
}

/* An out_write_fn implementation to print the characters via putchar(). It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
अटल व्योम do_output_to_putchar(
    UNUSED out_wrap_t *out,
    स्थिर अक्षर *buf,
    word_t len)
{
    अगर (buf) {
        जबतक (len-- > 0) {
            अक्षर_दो(*buf++);
        }
    }
}

/* An out_write_fn implementation to copy the buffer into the out buffer. It is
 * guaranteed here that 'out' is not NULL. The current implementation also never
 * passes NULL for 'buf'. */
अटल व्योम do_output_to_बफर(
    out_wrap_t *out,
    स्थिर अक्षर *buf,
    word_t len)
{
    /* It's guaranteed here that 'out' is not NULL. The current implementation
     * also never passes NULL for 'buf'. */
    अगर (buf && (out->used < out->maxlen)) {
        /* there is still space in the buffer*/
        word_t मुक्त = out->maxlen - out->used;
        अगर (len > मुक्त) {
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

अटल अंतरभूत bool_t है_अंक(अक्षर c)
{
    वापस c >= '0' &&
           c <= '9';
}

/* Convenient bit representation for modifier flags, which all fall within 31
 * codepoints of the space character.
 */
#घोषणा MASK_TYPE(a) (1U<<( a -' '))

#घोषणा ALT_FORM     (1U<<('#'-' '))
#घोषणा ZERO_PAD     (1U<<('0'-' '))
#घोषणा बायाँ_ADJ     (1U<<('-'-' '))
#घोषणा PAD_POS      (1U<<(' '-' '))
#घोषणा MARK_POS     (1U<<('+'-' '))
#घोषणा GROUPED      (1U<<('\''-' '))

#घोषणा FLAGMASK (ALT_FORM|ZERO_PAD|बायाँ_ADJ|PAD_POS|MARK_POS|GROUPED)

/* State machine to accept length modifiers + conversion specifiers.
 * Result is 0 on failure, or an argument type to pop on success.
 */

क्रमागत {
    BARE, LPRE, LLPRE, HPRE, HHPRE, BIGLPRE,
    ZTPRE, JPRE,
    STOP,
    PTR, INT, UINT, ULLONG,
    LONG, ULONG,
    SHORT, USHORT, CHAR, UCHAR,
    WORDT, LLONG,
#घोषणा IMAX LLONG
#घोषणा UMAX ULLONG
#घोषणा PDIFF LONG
#घोषणा UIPTR ULONG
    NOARG,
    MAXSTATE
};

#घोषणा S(x) [(x)-'A']

अटल स्थिर अचिन्हित अक्षर states[]['z' - 'A' + 1] = {
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

#घोषणा OOB(x) ((अचिन्हित)(x)-'A' > 'z'-'A')
#घोषणा DIGIT(c) (c - '0')

जोड़ arg {
    uintmax_t i;
    दीर्घ द्विगुन f;
    व्योम *p;
};

अटल व्योम निकालो_arg(जोड़ arg *arg, पूर्णांक type, बहु_सूची *ap)
{
    चयन (type) {
    हाल PTR:
        arg->p = बहु_तर्क(*ap, व्योम *);
        अवरोध;
    हाल INT:
        arg->i = बहु_तर्क(*ap, पूर्णांक);
        अवरोध;
    हाल UINT:
        arg->i = बहु_तर्क(*ap, अचिन्हित पूर्णांक);
        अवरोध;
    हाल LONG:
        arg->i = बहु_तर्क(*ap, दीर्घ);
        अवरोध;
    हाल ULONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ);
        अवरोध;
    हाल LLONG:
        arg->i = बहु_तर्क(*ap, दीर्घ दीर्घ);
        अवरोध;
    हाल ULLONG:
        arg->i = बहु_तर्क(*ap, अचिन्हित दीर्घ दीर्घ);
        अवरोध;
    हाल SHORT:
        arg->i = (लघु)बहु_तर्क(*ap, पूर्णांक);
        अवरोध;
    हाल USHORT:
        arg->i = (अचिन्हित लघु)बहु_तर्क(*ap, पूर्णांक);
        अवरोध;
    हाल CHAR:
        arg->i = (चिन्हित अक्षर)बहु_तर्क(*ap, पूर्णांक);
        अवरोध;
    हाल UCHAR:
        arg->i = (अचिन्हित अक्षर)बहु_तर्क(*ap, पूर्णांक);
        अवरोध;
    हाल WORDT:
        arg->i = बहु_तर्क(*ap, word_t);
    }
}


अटल व्योम pad(out_wrap_t *f, अक्षर c, पूर्णांक w, पूर्णांक l, पूर्णांक fl)
{
    अक्षर pad[32]; /* good enough for what the kernel prints */
    अगर (fl & (बायाँ_ADJ | ZERO_PAD) || l >= w) {
        वापस;
    }
    l = w - l;
    स_रखो(pad, c, l > माप(pad) ? माप(pad) : l);
    क्रम (; l >= माप(pad); l -= माप(pad)) {
        out(f, pad, माप(pad));
    }
    out(f, pad, l);
}

अटल स्थिर अक्षर xdigits[16] = {
    "0123456789ABCDEF"
};

अटल अक्षर *fmt_x(uintmax_t x, अक्षर *s, पूर्णांक lower)
{
    क्रम (; x; x >>= 4) {
        *--s = xdigits[(x & 15)] | lower;
    }
    वापस s;
}

अटल अक्षर *fmt_o(uintmax_t x, अक्षर *s)
{
    क्रम (; x; x >>= 3) {
        *--s = '0' + (x & 7);
    }
    वापस s;
}

अटल अक्षर *fmt_u(uintmax_t x, अक्षर *s)
{
    जबतक (0 != x) {
#अगर defined(CONFIG_ARCH_AARCH32) || defined(CONFIG_ARCH_RISCV32) || defined(CONFIG_ARCH_IA32)
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
        अगर (rem > 9) { /* handle rounding issues */
            q += 1;
            rem = x - (((q << 2) + q) << 1); /* recalculate reminder */
            निश्चित(rem <= 9); /* there must be no rounding error now */
        }
#अन्यथा
        uintmax_t q = x / 10;
        अचिन्हित पूर्णांक rem = x % 10;
#पूर्ण_अगर
        *--s = '0' + rem;
        x = q;
    }
    वापस s;
}

/* Maximum buffer size taken to ensure correct adaptation. However, it could be
 * reduced/removed if we could measure the buf length under all code paths
 */
#घोषणा द_द्वि_पूर्ण 113

#घोषणा NL_ARGMAX 9

अटल पूर्णांक getint(अक्षर **s)
{
    पूर्णांक i;
    क्रम (i = 0; है_अंक(**s); (*s)++) {
        अगर (i > INTMAX_MAX / 10U || DIGIT(**s) > INTMAX_MAX - 10 * i) {
            i = -1;
        } अन्यथा {
            i = 10 * i + DIGIT(**s);
        }
    }
    वापस i;
}

अटल पूर्णांक printf_core(out_wrap_t *f, स्थिर अक्षर *fmt, बहु_सूची *ap, जोड़ arg *nl_arg, पूर्णांक *nl_type)
{
    अक्षर *a, *z, *s = (अक्षर *)fmt;
    अचिन्हित l10n = 0, fl;
    पूर्णांक w, p, xp;
    जोड़ arg arg;
    पूर्णांक argpos;
    अचिन्हित st, ps;
    पूर्णांक cnt = 0, l = 0;
    अक्षर buf[माप(uintmax_t) * 3 + 3 + द_द्वि_पूर्ण / 4];
    स्थिर अक्षर *prefix;
    पूर्णांक t, pl;

    क्रम (;;) {
        अगर (l > INTMAX_MAX - cnt) {
            /* This error is only specified for snprintf, for other function
             * from the printf() family the behavior is unspecified. Stopping
             * immediately here also seems sane, otherwise %n could produce
             * wrong results.
             */
            वापस -1; /* overflow */
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        अगर (!*s) {
            अवरोध;
        }

        /* Handle literal text and %% format specifiers */
        क्रम (a = s; *s && *s != '%'; s++);
        क्रम (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        अगर (z - a > INTMAX_MAX - cnt) {
            वापस -1; /* overflow */
        }
        l = z - a;
        out(f, a, l);
        अगर (l) {
            जारी;
        }

        अगर (है_अंक(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = DIGIT(s[1]);
            s += 3;
        } अन्यथा {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        क्रम (fl = 0; (अचिन्हित)*s - ' ' < 32 && (FLAGMASK & MASK_TYPE(*s)); s++) {
            fl |= MASK_TYPE(*s);
        }

        /* Read field width */
        अगर (*s == '*') {
            अगर (है_अंक(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[DIGIT(s[1])] = INT;
                w = nl_arg[DIGIT(s[1])].i;
                s += 3;
            } अन्यथा अगर (!l10n) {
                w = f ? बहु_तर्क(*ap, पूर्णांक) : 0;
                s++;
            } अन्यथा {
                वापस -1; /* invalid */
            }
            अगर (w < 0) {
                fl |= बायाँ_ADJ;
                w = -w;
            }
        } अन्यथा अगर ((w = getint(&s)) < 0) {
            वापस -1; /* overflow */
        }

        /* Read precision */
        अगर (*s == '.' && s[1] == '*') {
            अगर (है_अंक(s[2]) && s[3] == '$') {
                nl_type[DIGIT(s[2])] = INT;
                p = nl_arg[DIGIT(s[2])].i;
                s += 4;
            } अन्यथा अगर (!l10n) {
                p = f ? बहु_तर्क(*ap, पूर्णांक) : 0;
                s += 2;
            } अन्यथा {
                वापस -1;/* invalid */
            }
            xp = (p >= 0);
        } अन्यथा अगर (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } अन्यथा {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        करो {
            अगर (OOB(*s)) {
                वापस -1; /* invalid */
            }
            ps = st;
            st = states[st]S(*s++);
        } जबतक (st - 1 < STOP);
        अगर (!st) {
            वापस -1; /* invalid */
        }

        /* Check validity of argument type (nl/normal) */
        अगर (st == NOARG) {
            अगर (argpos >= 0) {
                वापस -1; /* invalid */
            }
        } अन्यथा {
            अगर (argpos >= 0) {
                nl_type[argpos] = st;
                arg = nl_arg[argpos];
            } अन्यथा अगर (f) {
                निकालो_arg(&arg, st, ap);
            } अन्यथा {
                वापस 0;
            }
        }

        अगर (!f) {
            जारी;
        }

        z = buf + माप(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* - and 0 flags are mutually exclusive */
        अगर (fl & बायाँ_ADJ) {
            fl &= ~ZERO_PAD;
        }

        अगर (t == 'n') {
            अगर (!arg.p) {
                जारी;
            }
            चयन (ps) {
            हाल BARE:
                *(पूर्णांक *)arg.p = cnt;
                अवरोध;
            हाल LPRE:
                *(दीर्घ *)arg.p = cnt;
                अवरोध;
            हाल LLPRE:
                *(दीर्घ दीर्घ *)arg.p = cnt;
                अवरोध;
            हाल HPRE:
                *(अचिन्हित लघु *)arg.p = cnt;
                अवरोध;
            हाल HHPRE:
                *(अचिन्हित अक्षर *)arg.p = cnt;
                अवरोध;
            हाल ZTPRE:
                *(word_t *)arg.p = cnt;
                अवरोध;
            हाल JPRE:
                *(word_t *)arg.p = cnt;
                अवरोध;
            }
            जारी;
        } अन्यथा अगर (t == 'c') {
            p = 1;
            a = z - p;
            *a = arg.i;
            fl &= ~ZERO_PAD;
        } अन्यथा अगर (t == 's') {
            a = arg.p ? arg.p : "(null)";
            z = a + strnlen(a, p < 0 ? UINTPTR_MAX : p);
            अगर (p < 0 && *z) {
                वापस -1; /* overflow */
            }
            p = z - a;
            fl &= ~ZERO_PAD;
        } अन्यथा {
            चयन (t) {
            हाल 'p':
                p = MAX(p, 2 * माप(व्योम *));
                t = 'x';
                fl |= ALT_FORM;
            हाल 'x':
            हाल 'X':
                a = fmt_x(arg.i, z, t & 32);
                अगर (arg.i && (fl & ALT_FORM)) {
                    prefix += (t >> 4);
                    pl = 2;
                }
                अवरोध;
            हाल 'o':
                a = fmt_o(arg.i, z);
                अगर ((fl & ALT_FORM) && p < (z - a + 1)) {
                    p = z - a + 1;
                }
                अवरोध;
            हाल 'd':
            हाल 'i':
                pl = 1;
                अगर (arg.i > INTMAX_MAX) {
                    arg.i = -arg.i;
                } अन्यथा अगर (fl & MARK_POS) {
                    prefix++;
                } अन्यथा अगर (fl & PAD_POS) {
                    prefix += 2;
                } अन्यथा {
                    pl = 0;
                }
            हाल 'u':
                a = fmt_u(arg.i, z);
                अवरोध;
            }
            अगर (xp && p < 0) {
                वापस -1; /* overflow */
            }
            अगर (xp) {
                fl &= ~ZERO_PAD;
            }
            अगर (!arg.i && !p) {
                a = z;
            } अन्यथा {
                p = MAX(p, z - a + !arg.i);
            }
        }

        अगर (p < z - a) {
            p = z - a;
        }
        अगर (p > INTMAX_MAX - pl) {
            वापस -1; /* overflow */
        }
        अगर (w < pl + p) {
            w = pl + p;
        }
        अगर (w > INTMAX_MAX - cnt) {
            वापस -1; /* overflow */
        }

        pad(f, ' ', w, pl + p, fl);
        out(f, prefix, pl);
        pad(f, '0', w, pl + p, fl ^ ZERO_PAD);
        pad(f, '0', p, z - a, 0);
        out(f, a, z - a);
        pad(f, ' ', w, pl + p, fl ^ बायाँ_ADJ);

        l = w;
    }

    अगर (f) {
        वापस cnt;
    }
    अगर (!l10n) {
        वापस 0;
    }

    पूर्णांक i;
    क्रम (i = 1; i <= NL_ARGMAX && nl_type[i]; i++) {
        निकालो_arg(nl_arg + i, nl_type[i], ap);
    }
    क्रम (; i <= NL_ARGMAX && !nl_type[i]; i++);
    अगर (i <= NL_ARGMAX) {
        वापस -1; /* invalid */
    }

    वापस 1;
}

अटल पूर्णांक भ_लिखो(out_wrap_t *out, स्थिर अक्षर *fmt, बहु_सूची ap)
{
    बहु_सूची ap2;
    पूर्णांक nl_type[NL_ARGMAX + 1] = {0};
    जोड़ arg nl_arg[NL_ARGMAX + 1];
    पूर्णांक ret;

    /* validate format string */
    va_copy(ap2, ap);
    अगर (printf_core(शून्य, fmt, &ap2, nl_arg, nl_type) < 0) {
        बहु_पूर्ण(ap2);
        वापस -1;
    }

    ret = printf_core(out, fmt, &ap2, nl_arg, nl_type);
    बहु_पूर्ण(ap2);
    वापस ret;
}

/*
 *------------------------------------------------------------------------------
 * Kernel printing API
 *------------------------------------------------------------------------------
 */

पूर्णांक impl_kvprintf(स्थिर अक्षर *format, बहु_सूची ap)
{
    out_wrap_t out_wrap =  {
        .ग_लिखो  = do_output_to_putchar,
        .buf    = शून्य,
        .maxlen = 0,
        .used   = 0
    };

    वापस भ_लिखो(&out_wrap, format, ap);
}

पूर्णांक impl_ksnvprintf(अक्षर *str, word_t आकार, स्थिर अक्षर *format, बहु_सूची ap)
{
    अगर (!str) {
        आकार = 0;
    }

    out_wrap_t out_wrap =  {
        .ग_लिखो  = do_output_to_बफर,
        .buf    = str,
        .maxlen = आकार,
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
    अगर ((ret > 0) && (आकार > 0)) {
        str[(ret < आकार) ? ret : आकार - 1] = '\0';
    }

    वापस ret;
}

#पूर्ण_अगर /* CONFIG_PRINTING */
