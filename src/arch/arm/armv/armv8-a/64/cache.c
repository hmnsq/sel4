/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanByWSL(ਸ਼ਬਦ_t wsl)
{
    asm ਅਸਥਿਰ("dc csw, %0" : : "r"(wsl));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanInvalidateByWSL(ਸ਼ਬਦ_t wsl)
{
    asm ਅਸਥਿਰ("dc cisw, %0" : : "r"(wsl));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t readCLID(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t CLID;
    MRS("clidr_el1", CLID);
    ਵਾਪਸ CLID;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOUU(x)    (((x) >> 27)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOC(x)     (((x) >> 24)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOUIS(x)   (((x) >> 21)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTYPE(x,n) (((x) >> (n*3))     & ਨਕਾਬ(3))

ਗਿਣਤੀ arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t read_Cache_ਮਾਪ(ਪੂਰਨ level, bool_t instruction)
{
    ਸ਼ਬਦ_t ਮਾਪ, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", ਮਾਪ);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    ਵਾਪਸ ਮਾਪ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINEBITS(s)     (((s) & ਨਕਾਬ(3)) + 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSOC(s)        ((((s) >> 3) & ਨਕਾਬ(10)) + 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSETS(s)        ((((s) >> 13) & ਨਕਾਬ(15)) + 1)

ਖਾਲੀ clean_D_PoU(ਖਾਲੀ)
{
    ਪੂਰਨ clid = readCLID();
    ਪੂਰਨ lou = LOUU(clid);

    ਲਈ (ਪੂਰਨ l = 0; l < lou; l++) {
        ਜੇ (CTYPE(clid, l) > ARMCacheI) {
            ਸ਼ਬਦ_t lsize = read_Cache_ਮਾਪ(l, 0);
            ਪੂਰਨ lbits = LINEBITS(lsize);
            ਪੂਰਨ assoc = ASSOC(lsize);
            ਪੂਰਨ assoc_ਬਿੱਟ = ਸ਼ਬਦ_ਬਿੱਟ - clzl(assoc - 1);
            ਪੂਰਨ nsets = NSETS(lsize);
            ਲਈ (ਪੂਰਨ w = 0; w < assoc; w++) {
                ਲਈ (ਪੂਰਨ s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_ਬਿੱਟ)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanInvalidate_D_by_level(ਪੂਰਨ l)
{
    ਸ਼ਬਦ_t lsize = read_Cache_ਮਾਪ(l, 0);
    ਪੂਰਨ lbits = LINEBITS(lsize);
    ਪੂਰਨ assoc = ASSOC(lsize);
    ਪੂਰਨ assoc_ਬਿੱਟ = ਸ਼ਬਦ_ਬਿੱਟ - clzl(assoc - 1);
    ਪੂਰਨ nsets = NSETS(lsize);

    ਲਈ (ਪੂਰਨ w = 0; w < assoc; w++) {
        ਲਈ (ਪੂਰਨ s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_ਬਿੱਟ)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

ਖਾਲੀ cleanInvalidate_D_PoC(ਖਾਲੀ)
{
    ਪੂਰਨ clid = readCLID();
    ਪੂਰਨ loc = LOC(clid);

    ਲਈ (ਪੂਰਨ l = 0; l < loc; l++) {
        ਜੇ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ਖਾਲੀ cleanInvalidate_L1D(ਖਾਲੀ)
{
    cleanInvalidate_D_by_level(0);
}
