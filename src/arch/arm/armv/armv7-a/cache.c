/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <arch/machine/hardware.h>

স্থির অন্তঃরেখ শূন্য cleanByWSL(word_t wsl)
{
    asm অস্থির("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

স্থির অন্তঃরেখ শূন্য cleanInvalidateByWSL(word_t wsl)
{
    asm অস্থির("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


স্থির অন্তঃরেখ word_t readCLID(শূন্য)
{
    word_t CLID;
    asm অস্থির("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    ফেরত CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

গণনা arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


স্থির অন্তঃরেখ word_t read_Cache_আকার(পূর্ণসংখ্যা level, bool_t instruction)
{
    word_t আকার_unique_name, csselr_old;
    /* Save CSSELR */
    asm অস্থির("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm অস্থির("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm অস্থির("mrc p15, 1, %0, c0, c0, 0" : "=r"(আকার_unique_name));
    /* Restore CSSELR */
    asm অস্থির("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    ফেরত আকার_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


শূন্য clean_D_PoU(শূন্য)
{
    পূর্ণসংখ্যা clid = readCLID();
    পূর্ণসংখ্যা lou = LOUU(clid);
    পূর্ণসংখ্যা l;

    জন্য (l = 0; l < lou; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            word_t s = read_Cache_আকার(l, 0);
            পূর্ণসংখ্যা lbits = LINEBITS(s);
            পূর্ণসংখ্যা assoc = ASSOC(s);
            পূর্ণসংখ্যা assoc_bits = wordBits - clzl(assoc - 1);
            পূর্ণসংখ্যা nsets = NSETS(s);
            পূর্ণসংখ্যা w;

            জন্য (w = 0; w < assoc; w++) {
                পূর্ণসংখ্যা v;

                জন্য (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

স্থির অন্তঃরেখ শূন্য cleanInvalidate_D_by_level(পূর্ণসংখ্যা l)
{
    word_t s = read_Cache_আকার(l, 0);
    পূর্ণসংখ্যা lbits = LINEBITS(s);
    পূর্ণসংখ্যা assoc = ASSOC(s);
    পূর্ণসংখ্যা assoc_bits = wordBits - clzl(assoc - 1);
    পূর্ণসংখ্যা nsets = NSETS(s);
    পূর্ণসংখ্যা w;

    জন্য (w = 0; w < assoc; w++) {
        পূর্ণসংখ্যা v;

        জন্য (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

শূন্য cleanInvalidate_D_PoC(শূন্য)
{
    পূর্ণসংখ্যা clid = readCLID();
    পূর্ণসংখ্যা loc = LOC(clid);
    পূর্ণসংখ্যা l;

    জন্য (l = 0; l < loc; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

শূন্য cleanInvalidate_L1D(শূন্য)
{
    cleanInvalidate_D_by_level(0);
}
