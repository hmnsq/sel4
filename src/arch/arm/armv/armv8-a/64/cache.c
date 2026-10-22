/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanByWSL(word_t wsl)
{
    asm ಚಞ್ಚಲ("dc csw, %0" : : "r"(wsl));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanInvalidateByWSL(word_t wsl)
{
    asm ಚಞ್ಚಲ("dc cisw, %0" : : "r"(wsl));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t readCLID(ಶೂನ್ಯ)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    ಪ್ರತ್ಯಾಗಮ CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ಗಣನಾ arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t readCacheSize(ಪೂರ್ಣಾಙ್ಕ level, bool_t instruction)
{
    word_t size, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", size);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    ಪ್ರತ್ಯಾಗಮ size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

ಶೂನ್ಯ clean_D_PoU(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ clid = readCLID();
    ಪೂರ್ಣಾಙ್ಕ lou = LOUU(clid);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ l = 0; l < lou; l++) {
        ಯದಿ (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            ಪೂರ್ಣಾಙ್ಕ lbits = LINEBITS(lsize);
            ಪೂರ್ಣಾಙ್ಕ assoc = ASSOC(lsize);
            ಪೂರ್ಣಾಙ್ಕ assoc_bits = wordBits - clzl(assoc - 1);
            ಪೂರ್ಣಾಙ್ಕ nsets = NSETS(lsize);
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ w = 0; w < assoc; w++) {
                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ cleanInvalidate_D_by_level(ಪೂರ್ಣಾಙ್ಕ l)
{
    word_t lsize = readCacheSize(l, 0);
    ಪೂರ್ಣಾಙ್ಕ lbits = LINEBITS(lsize);
    ಪೂರ್ಣಾಙ್ಕ assoc = ASSOC(lsize);
    ಪೂರ್ಣಾಙ್ಕ assoc_bits = wordBits - clzl(assoc - 1);
    ಪೂರ್ಣಾಙ್ಕ nsets = NSETS(lsize);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ w = 0; w < assoc; w++) {
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_D_PoC(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ clid = readCLID();
    ಪೂರ್ಣಾಙ್ಕ loc = LOC(clid);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ l = 0; l < loc; l++) {
        ಯದಿ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_L1D(ಶೂನ್ಯ)
{
    cleanInvalidate_D_by_level(0);
}
