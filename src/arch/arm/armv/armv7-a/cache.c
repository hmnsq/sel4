/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanByWSL(ਸ਼ਬਦ_t wsl)
{
    asm ਅਸਥਿਰ("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanInvalidateByWSL(ਸ਼ਬਦ_t wsl)
{
    asm ਅਸਥਿਰ("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t readCLID(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t CLID;
    asm ਅਸਥਿਰ("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    ਵਾਪਸ CLID;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOUU(x)    (((x) >> 27)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOC(x)     (((x) >> 24)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOUIS(x)   (((x) >> 21)        & ਨਕਾਬ(3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTYPE(x,n) (((x) >> (n*3))     & ਨਕਾਬ(3))

ਗਿਣਤੀ arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t read_Cache_ਮਾਪ(ਪੂਰਨ level, bool_t instruction)
{
    ਸ਼ਬਦ_t ਮਾਪ_unique_name, csselr_old;
    /* Save CSSELR */
    asm ਅਸਥਿਰ("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm ਅਸਥਿਰ("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm ਅਸਥਿਰ("mrc p15, 1, %0, c0, c0, 0" : "=r"(ਮਾਪ_unique_name));
    /* Restore CSSELR */
    asm ਅਸਥਿਰ("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    ਵਾਪਸ ਮਾਪ_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINEBITS(s) (( (s)        & ਨਕਾਬ(3))  + 4)
/* Associativity, field is assoc - 1. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSOC(s)    ((((s) >> 3)  & ਨਕਾਬ(10)) + 1)
/* Number of sets, field is nsets - 1. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSETS(s)    ((((s) >> 13) & ਨਕਾਬ(15)) + 1)


ਖਾਲੀ clean_D_PoU(ਖਾਲੀ)
{
    ਪੂਰਨ clid = readCLID();
    ਪੂਰਨ lou = LOUU(clid);
    ਪੂਰਨ l;

    ਲਈ (l = 0; l < lou; l++) {
        ਜੇ (CTYPE(clid, l) > ARMCacheI) {
            ਸ਼ਬਦ_t s = read_Cache_ਮਾਪ(l, 0);
            ਪੂਰਨ lbits = LINEBITS(s);
            ਪੂਰਨ assoc = ASSOC(s);
            ਪੂਰਨ assoc_ਬਿੱਟ = ਸ਼ਬਦ_ਬਿੱਟ - clzl(assoc - 1);
            ਪੂਰਨ nsets = NSETS(s);
            ਪੂਰਨ w;

            ਲਈ (w = 0; w < assoc; w++) {
                ਪੂਰਨ v;

                ਲਈ (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_ਬਿੱਟ)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ cleanInvalidate_D_by_level(ਪੂਰਨ l)
{
    ਸ਼ਬਦ_t s = read_Cache_ਮਾਪ(l, 0);
    ਪੂਰਨ lbits = LINEBITS(s);
    ਪੂਰਨ assoc = ASSOC(s);
    ਪੂਰਨ assoc_ਬਿੱਟ = ਸ਼ਬਦ_ਬਿੱਟ - clzl(assoc - 1);
    ਪੂਰਨ nsets = NSETS(s);
    ਪੂਰਨ w;

    ਲਈ (w = 0; w < assoc; w++) {
        ਪੂਰਨ v;

        ਲਈ (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_ਬਿੱਟ)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

ਖਾਲੀ cleanInvalidate_D_PoC(ਖਾਲੀ)
{
    ਪੂਰਨ clid = readCLID();
    ਪੂਰਨ loc = LOC(clid);
    ਪੂਰਨ l;

    ਲਈ (l = 0; l < loc; l++) {
        ਜੇ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ਖਾਲੀ cleanInvalidate_L1D(ਖਾਲੀ)
{
    cleanInvalidate_D_by_level(0);
}
