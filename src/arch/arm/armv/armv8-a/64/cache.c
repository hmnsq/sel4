/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <arch/machine/hardware.h>

স্থির অন্তঃস্থ শূন্য cleanByWSL(word_t wsl)
{
    asm চঞ্চল("dc csw, %0" : : "r"(wsl));
}

স্থির অন্তঃস্থ শূন্য cleanInvalidateByWSL(word_t wsl)
{
    asm চঞ্চল("dc cisw, %0" : : "r"(wsl));
}

স্থির অন্তঃস্থ word_t readCLID(শূন্য)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    ঘূৰাই CLID;
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

স্থির অন্তঃস্থ word_t readCacheSize(পূর্ণাঙ্ক level, bool_t instruction)
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
    ঘূৰাই size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

শূন্য clean_D_PoU(শূন্য)
{
    পূর্ণাঙ্ক clid = readCLID();
    পূর্ণাঙ্ক lou = LOUU(clid);

    বাবে (পূর্ণাঙ্ক l = 0; l < lou; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            পূর্ণাঙ্ক lbits = LINEBITS(lsize);
            পূর্ণাঙ্ক assoc = ASSOC(lsize);
            পূর্ণাঙ্ক assoc_bits = wordBits - clzl(assoc - 1);
            পূর্ণাঙ্ক nsets = NSETS(lsize);
            বাবে (পূর্ণাঙ্ক w = 0; w < assoc; w++) {
                বাবে (পূর্ণাঙ্ক s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

স্থির অন্তঃস্থ শূন্য cleanInvalidate_D_by_level(পূর্ণাঙ্ক l)
{
    word_t lsize = readCacheSize(l, 0);
    পূর্ণাঙ্ক lbits = LINEBITS(lsize);
    পূর্ণাঙ্ক assoc = ASSOC(lsize);
    পূর্ণাঙ্ক assoc_bits = wordBits - clzl(assoc - 1);
    পূর্ণাঙ্ক nsets = NSETS(lsize);

    বাবে (পূর্ণাঙ্ক w = 0; w < assoc; w++) {
        বাবে (পূর্ণাঙ্ক s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

শূন্য cleanInvalidate_D_PoC(শূন্য)
{
    পূর্ণাঙ্ক clid = readCLID();
    পূর্ণাঙ্ক loc = LOC(clid);

    বাবে (পূর্ণাঙ্ক l = 0; l < loc; l++) {
        যদি (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

শূন্য cleanInvalidate_L1D(শূন্য)
{
    cleanInvalidate_D_by_level(0);
}
