/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <arch/machine/hardware.h>

நிலையான உள்வரி வெறுமை cleanByWSL(word_t wsl)
{
    asm நிலையற்ற("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

நிலையான உள்வரி வெறுமை cleanInvalidateByWSL(word_t wsl)
{
    asm நிலையற்ற("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


நிலையான உள்வரி word_t readCLID(வெறுமை)
{
    word_t CLID;
    asm நிலையற்ற("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    திரும்பு CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

எண்ணிக்கை arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


நிலையான உள்வரி word_t readCacheSize(முழு level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm நிலையற்ற("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm நிலையற்ற("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm நிலையற்ற("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm நிலையற்ற("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    திரும்பு size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


வெறுமை clean_D_PoU(வெறுமை)
{
    முழு clid = readCLID();
    முழு lou = LOUU(clid);
    முழு l;

    க்கு (l = 0; l < lou; l++) {
        எனில் (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            முழு lbits = LINEBITS(s);
            முழு assoc = ASSOC(s);
            முழு assoc_bits = wordBits - clzl(assoc - 1);
            முழு nsets = NSETS(s);
            முழு w;

            க்கு (w = 0; w < assoc; w++) {
                முழு v;

                க்கு (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

நிலையான உள்வரி வெறுமை cleanInvalidate_D_by_level(முழு l)
{
    word_t s = readCacheSize(l, 0);
    முழு lbits = LINEBITS(s);
    முழு assoc = ASSOC(s);
    முழு assoc_bits = wordBits - clzl(assoc - 1);
    முழு nsets = NSETS(s);
    முழு w;

    க்கு (w = 0; w < assoc; w++) {
        முழு v;

        க்கு (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

வெறுமை cleanInvalidate_D_PoC(வெறுமை)
{
    முழு clid = readCLID();
    முழு loc = LOC(clid);
    முழு l;

    க்கு (l = 0; l < loc; l++) {
        எனில் (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

வெறுமை cleanInvalidate_L1D(வெறுமை)
{
    cleanInvalidate_D_by_level(0);
}
