/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanByWSL(ಶಬ್ದ_t wsl)
{
    asm ಅಸ್ಥಿರ("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanInvalidateByWSL(ಶಬ್ದ_t wsl)
{
    asm ಅಸ್ಥಿರ("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t readCLID(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t CLID;
    asm ಅಸ್ಥಿರ("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    ಹಿಂತಿರುಗಿ CLID;
}

#ವ್ಯಾಖ್ಯಾನಿಸು LOUU(x)    (((x) >> 27)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು LOC(x)     (((x) >> 24)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು LOUIS(x)   (((x) >> 21)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು CTYPE(x,n) (((x) >> (n*3))     & ಮುಖವಾಡ(3))

ಎಣಿಕೆ arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t read_Cache_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ level, bool_t instruction)
{
    ಶಬ್ದ_t ಗಾತ್ರ_unique_name, csselr_old;
    /* Save CSSELR */
    asm ಅಸ್ಥಿರ("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm ಅಸ್ಥಿರ("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm ಅಸ್ಥಿರ("mrc p15, 1, %0, c0, c0, 0" : "=r"(ಗಾತ್ರ_unique_name));
    /* Restore CSSELR */
    asm ಅಸ್ಥಿರ("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#ವ್ಯಾಖ್ಯಾನಿಸು LINEBITS(s) (( (s)        & ಮುಖವಾಡ(3))  + 4)
/* Associativity, field is assoc - 1. */
#ವ್ಯಾಖ್ಯಾನಿಸು ASSOC(s)    ((((s) >> 3)  & ಮುಖವಾಡ(10)) + 1)
/* Number of sets, field is nsets - 1. */
#ವ್ಯಾಖ್ಯಾನಿಸು NSETS(s)    ((((s) >> 13) & ಮುಖವಾಡ(15)) + 1)


ಶೂನ್ಯ clean_D_PoU(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ clid = readCLID();
    ಪೂರ್ಣಾಂಕ lou = LOUU(clid);
    ಪೂರ್ಣಾಂಕ l;

    ಗಾಗಿ (l = 0; l < lou; l++) {
        ಆದರೆ (CTYPE(clid, l) > ARMCacheI) {
            ಶಬ್ದ_t s = read_Cache_ಗಾತ್ರ(l, 0);
            ಪೂರ್ಣಾಂಕ lbits = LINEBITS(s);
            ಪೂರ್ಣಾಂಕ assoc = ASSOC(s);
            ಪೂರ್ಣಾಂಕ assoc_ಬಿಟ್ಟುಗಳು = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - clzl(assoc - 1);
            ಪೂರ್ಣಾಂಕ nsets = NSETS(s);
            ಪೂರ್ಣಾಂಕ w;

            ಗಾಗಿ (w = 0; w < assoc; w++) {
                ಪೂರ್ಣಾಂಕ v;

                ಗಾಗಿ (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_ಬಿಟ್ಟುಗಳು)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanInvalidate_D_by_level(ಪೂರ್ಣಾಂಕ l)
{
    ಶಬ್ದ_t s = read_Cache_ಗಾತ್ರ(l, 0);
    ಪೂರ್ಣಾಂಕ lbits = LINEBITS(s);
    ಪೂರ್ಣಾಂಕ assoc = ASSOC(s);
    ಪೂರ್ಣಾಂಕ assoc_ಬಿಟ್ಟುಗಳು = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - clzl(assoc - 1);
    ಪೂರ್ಣಾಂಕ nsets = NSETS(s);
    ಪೂರ್ಣಾಂಕ w;

    ಗಾಗಿ (w = 0; w < assoc; w++) {
        ಪೂರ್ಣಾಂಕ v;

        ಗಾಗಿ (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_ಬಿಟ್ಟುಗಳು)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_D_PoC(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ clid = readCLID();
    ಪೂರ್ಣಾಂಕ loc = LOC(clid);
    ಪೂರ್ಣಾಂಕ l;

    ಗಾಗಿ (l = 0; l < loc; l++) {
        ಆದರೆ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_L1D(ಶೂನ್ಯ)
{
    cleanInvalidate_D_by_level(0);
}
