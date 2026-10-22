/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanByWSL(word_t wsl)
{
    asm ಚಞ್ಚಲ("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanInvalidateByWSL(word_t wsl)
{
    asm ಚಞ್ಚಲ("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t readCLID(ಶೂನ್ಯ)
{
    word_t CLID;
    asm ಚಞ್ಚಲ("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    ಪ್ರತ್ಯಾಗಮ CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ಗಣನಾ arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t readCacheSize(ಪೂರ್ಣಾಙ್ಕ level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm ಚಞ್ಚಲ("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm ಚಞ್ಚಲ("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm ಚಞ್ಚಲ("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm ಚಞ್ಚಲ("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    ಪ್ರತ್ಯಾಗಮ size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


ಶೂನ್ಯ clean_D_PoU(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ clid = readCLID();
    ಪೂರ್ಣಾಙ್ಕ lou = LOUU(clid);
    ಪೂರ್ಣಾಙ್ಕ l;

    ಪ್ರತಿ (l = 0; l < lou; l++) {
        ಯದಿ (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            ಪೂರ್ಣಾಙ್ಕ lbits = LINEBITS(s);
            ಪೂರ್ಣಾಙ್ಕ assoc = ASSOC(s);
            ಪೂರ್ಣಾಙ್ಕ assoc_bits = wordBits - clzl(assoc - 1);
            ಪೂರ್ಣಾಙ್ಕ nsets = NSETS(s);
            ಪೂರ್ಣಾಙ್ಕ w;

            ಪ್ರತಿ (w = 0; w < assoc; w++) {
                ಪೂರ್ಣಾಙ್ಕ v;

                ಪ್ರತಿ (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanInvalidate_D_by_level(ಪೂರ್ಣಾಙ್ಕ l)
{
    word_t s = readCacheSize(l, 0);
    ಪೂರ್ಣಾಙ್ಕ lbits = LINEBITS(s);
    ಪೂರ್ಣಾಙ್ಕ assoc = ASSOC(s);
    ಪೂರ್ಣಾಙ್ಕ assoc_bits = wordBits - clzl(assoc - 1);
    ಪೂರ್ಣಾಙ್ಕ nsets = NSETS(s);
    ಪೂರ್ಣಾಙ್ಕ w;

    ಪ್ರತಿ (w = 0; w < assoc; w++) {
        ಪೂರ್ಣಾಙ್ಕ v;

        ಪ್ರತಿ (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_D_PoC(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ clid = readCLID();
    ಪೂರ್ಣಾಙ್ಕ loc = LOC(clid);
    ಪೂರ್ಣಾಙ್ಕ l;

    ಪ್ರತಿ (l = 0; l < loc; l++) {
        ಯದಿ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_L1D(ಶೂನ್ಯ)
{
    cleanInvalidate_D_by_level(0);
}
