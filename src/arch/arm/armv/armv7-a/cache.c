/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/machine/hardware.h>

स्थिर ओळीत शून्य cleanByWSL(word_t wsl)
{
    यंत्र अस्थिर("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

स्थिर ओळीत शून्य cleanInvalidateByWSL(word_t wsl)
{
    यंत्र अस्थिर("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


स्थिर ओळीत word_t readCLID(शून्य)
{
    word_t CLID;
    यंत्र अस्थिर("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    परत CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

गणना arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


स्थिर ओळीत word_t readCacheSize(पूर्णांक level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    यंत्र अस्थिर("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    यंत्र अस्थिर("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    यंत्र अस्थिर("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    यंत्र अस्थिर("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    परत size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


शून्य clean_D_PoU(शून्य)
{
    पूर्णांक clid = readCLID();
    पूर्णांक lou = LOUU(clid);
    पूर्णांक l;

    साठी (l = 0; l < lou; l++) {
        जर (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            पूर्णांक lbits = LINEBITS(s);
            पूर्णांक assoc = ASSOC(s);
            पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
            पूर्णांक nsets = NSETS(s);
            पूर्णांक w;

            साठी (w = 0; w < assoc; w++) {
                पूर्णांक v;

                साठी (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

स्थिर ओळीत शून्य cleanInvalidate_D_by_level(पूर्णांक l)
{
    word_t s = readCacheSize(l, 0);
    पूर्णांक lbits = LINEBITS(s);
    पूर्णांक assoc = ASSOC(s);
    पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
    पूर्णांक nsets = NSETS(s);
    पूर्णांक w;

    साठी (w = 0; w < assoc; w++) {
        पूर्णांक v;

        साठी (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

शून्य cleanInvalidate_D_PoC(शून्य)
{
    पूर्णांक clid = readCLID();
    पूर्णांक loc = LOC(clid);
    पूर्णांक l;

    साठी (l = 0; l < loc; l++) {
        जर (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

शून्य cleanInvalidate_L1D(शून्य)
{
    cleanInvalidate_D_by_level(0);
}
