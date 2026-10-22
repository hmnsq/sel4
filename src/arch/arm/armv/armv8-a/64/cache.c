/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <arch/machine/hardware.h>

நிலையான உள்வரி வெறுமை cleanByWSL(word_t wsl)
{
    asm நிலையற்ற("dc csw, %0" : : "r"(wsl));
}

நிலையான உள்வரி வெறுமை cleanInvalidateByWSL(word_t wsl)
{
    asm நிலையற்ற("dc cisw, %0" : : "r"(wsl));
}

நிலையான உள்வரி word_t readCLID(வெறுமை)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    திரும்பு CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

எண்ணிக்கை arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

நிலையான உள்வரி word_t readCacheSize(முழு level, bool_t instruction)
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
    திரும்பு size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

வெறுமை clean_D_PoU(வெறுமை)
{
    முழு clid = readCLID();
    முழு lou = LOUU(clid);

    க்கு (முழு l = 0; l < lou; l++) {
        எனில் (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            முழு lbits = LINEBITS(lsize);
            முழு assoc = ASSOC(lsize);
            முழு assoc_bits = wordBits - clzl(assoc - 1);
            முழு nsets = NSETS(lsize);
            க்கு (முழு w = 0; w < assoc; w++) {
                க்கு (முழு s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

நிலையான உள்வரி வெறுமை cleanInvalidate_D_by_level(முழு l)
{
    word_t lsize = readCacheSize(l, 0);
    முழு lbits = LINEBITS(lsize);
    முழு assoc = ASSOC(lsize);
    முழு assoc_bits = wordBits - clzl(assoc - 1);
    முழு nsets = NSETS(lsize);

    க்கு (முழு w = 0; w < assoc; w++) {
        க்கு (முழு s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

வெறுமை cleanInvalidate_D_PoC(வெறுமை)
{
    முழு clid = readCLID();
    முழு loc = LOC(clid);

    க்கு (முழு l = 0; l < loc; l++) {
        எனில் (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

வெறுமை cleanInvalidate_L1D(வெறுமை)
{
    cleanInvalidate_D_by_level(0);
}
