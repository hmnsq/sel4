/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <arch/machine/hardware.h>

স্থির অন্তঃরেখ শূন্য cleanByWSL(word_t wsl)
{
    asm অস্থির("dc csw, %0" : : "r"(wsl));
}

স্থির অন্তঃরেখ শূন্য cleanInvalidateByWSL(word_t wsl)
{
    asm অস্থির("dc cisw, %0" : : "r"(wsl));
}

স্থির অন্তঃরেখ word_t readCLID(শূন্য)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    ফেরত CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

গণনা arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

স্থির অন্তঃরেখ word_t read_Cache_আকার(পূর্ণসংখ্যা level, bool_t instruction)
{
    word_t আকার_2, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", আকার_2);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    ফেরত আকার_2;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

শূন্য clean_D_PoU(শূন্য)
{
    পূর্ণসংখ্যা clid = readCLID();
    পূর্ণসংখ্যা lou = LOUU(clid);

    জন্য (পূর্ণসংখ্যা l = 0; l < lou; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = read_Cache_আকার(l, 0);
            পূর্ণসংখ্যা lbits = LINEBITS(lsize);
            পূর্ণসংখ্যা assoc = ASSOC(lsize);
            পূর্ণসংখ্যা assoc_bits = wordBits - clzl(assoc - 1);
            পূর্ণসংখ্যা nsets = NSETS(lsize);
            জন্য (পূর্ণসংখ্যা w = 0; w < assoc; w++) {
                জন্য (পূর্ণসংখ্যা s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

স্থির অন্তঃরেখ শূন্য cleanInvalidate_D_by_level(পূর্ণসংখ্যা l)
{
    word_t lsize = read_Cache_আকার(l, 0);
    পূর্ণসংখ্যা lbits = LINEBITS(lsize);
    পূর্ণসংখ্যা assoc = ASSOC(lsize);
    পূর্ণসংখ্যা assoc_bits = wordBits - clzl(assoc - 1);
    পূর্ণসংখ্যা nsets = NSETS(lsize);

    জন্য (পূর্ণসংখ্যা w = 0; w < assoc; w++) {
        জন্য (পূর্ণসংখ্যা s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

শূন্য cleanInvalidate_D_PoC(শূন্য)
{
    পূর্ণসংখ্যা clid = readCLID();
    পূর্ণসংখ্যা loc = LOC(clid);

    জন্য (পূর্ণসংখ্যা l = 0; l < loc; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

শূন্য cleanInvalidate_L1D(শূন্য)
{
    cleanInvalidate_D_by_level(0);
}
