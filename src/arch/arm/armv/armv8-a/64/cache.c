/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanByWSL(ಶಬ್ದ_t wsl)
{
    asm ಅಸ್ಥಿರ("dc csw, %0" : : "r"(wsl));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanInvalidateByWSL(ಶಬ್ದ_t wsl)
{
    asm ಅಸ್ಥಿರ("dc cisw, %0" : : "r"(wsl));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t readCLID(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t CLID;
    MRS("clidr_el1", CLID);
    ಹಿಂತಿರುಗಿ CLID;
}

#ವ್ಯಾಖ್ಯಾನಿಸು LOUU(x)    (((x) >> 27)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು LOC(x)     (((x) >> 24)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು LOUIS(x)   (((x) >> 21)        & ಮುಖವಾಡ(3))
#ವ್ಯಾಖ್ಯಾನಿಸು CTYPE(x,n) (((x) >> (n*3))     & ಮುಖವಾಡ(3))

ಎಣಿಕೆ arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t read_Cache_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ level, bool_t instruction)
{
    ಶಬ್ದ_t ಗಾತ್ರ_2, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", ಗಾತ್ರ_2);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

#ವ್ಯಾಖ್ಯಾನಿಸು LINEBITS(s)     (((s) & ಮುಖವಾಡ(3)) + 4)
#ವ್ಯಾಖ್ಯಾನಿಸು ASSOC(s)        ((((s) >> 3) & ಮುಖವಾಡ(10)) + 1)
#ವ್ಯಾಖ್ಯಾನಿಸು NSETS(s)        ((((s) >> 13) & ಮುಖವಾಡ(15)) + 1)

ಶೂನ್ಯ clean_D_PoU(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ clid = readCLID();
    ಪೂರ್ಣಾಂಕ lou = LOUU(clid);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < lou; l++) {
        ಆದರೆ (CTYPE(clid, l) > ARMCacheI) {
            ಶಬ್ದ_t lsize = read_Cache_ಗಾತ್ರ(l, 0);
            ಪೂರ್ಣಾಂಕ lbits = LINEBITS(lsize);
            ಪೂರ್ಣಾಂಕ assoc = ASSOC(lsize);
            ಪೂರ್ಣಾಂಕ assoc_ಬಿಟ್ಟುಗಳು = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - clzl(assoc - 1);
            ಪೂರ್ಣಾಂಕ nsets = NSETS(lsize);
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ w = 0; w < assoc; w++) {
                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_ಬಿಟ್ಟುಗಳು)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ cleanInvalidate_D_by_level(ಪೂರ್ಣಾಂಕ l)
{
    ಶಬ್ದ_t lsize = read_Cache_ಗಾತ್ರ(l, 0);
    ಪೂರ್ಣಾಂಕ lbits = LINEBITS(lsize);
    ಪೂರ್ಣಾಂಕ assoc = ASSOC(lsize);
    ಪೂರ್ಣಾಂಕ assoc_ಬಿಟ್ಟುಗಳು = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - clzl(assoc - 1);
    ಪೂರ್ಣಾಂಕ nsets = NSETS(lsize);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ w = 0; w < assoc; w++) {
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_ಬಿಟ್ಟುಗಳು)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_D_PoC(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ clid = readCLID();
    ಪೂರ್ಣಾಂಕ loc = LOC(clid);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ l = 0; l < loc; l++) {
        ಆದರೆ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ಶೂನ್ಯ cleanInvalidate_L1D(ಶೂನ್ಯ)
{
    cleanInvalidate_D_by_level(0);
}
