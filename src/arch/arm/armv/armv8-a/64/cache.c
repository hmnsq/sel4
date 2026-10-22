/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine/hardware.h>

ایستا درونخطی تهی cleanByWSL(word_t wsl)
{
    asm فرار("dc csw, %0" : : "r"(wsl));
}

ایستا درونخطی تهی cleanInvalidateByWSL(word_t wsl)
{
    asm فرار("dc cisw, %0" : : "r"(wsl));
}

ایستا درونخطی word_t readCLID(تهی)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    بازگشت CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

شمارش arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ایستا درونخطی word_t readCacheSize(صحیح level, bool_t instruction)
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
    بازگشت size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

تهی clean_D_PoU(تهی)
{
    صحیح clid = readCLID();
    صحیح lou = LOUU(clid);

    برای (صحیح l = 0; l < lou; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            صحیح lbits = LINEBITS(lsize);
            صحیح assoc = ASSOC(lsize);
            صحیح assoc_bits = wordBits - clzl(assoc - 1);
            صحیح nsets = NSETS(lsize);
            برای (صحیح w = 0; w < assoc; w++) {
                برای (صحیح s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ایستا درونخطی تهی cleanInvalidate_D_by_level(صحیح l)
{
    word_t lsize = readCacheSize(l, 0);
    صحیح lbits = LINEBITS(lsize);
    صحیح assoc = ASSOC(lsize);
    صحیح assoc_bits = wordBits - clzl(assoc - 1);
    صحیح nsets = NSETS(lsize);

    برای (صحیح w = 0; w < assoc; w++) {
        برای (صحیح s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

تهی cleanInvalidate_D_PoC(تهی)
{
    صحیح clid = readCLID();
    صحیح loc = LOC(clid);

    برای (صحیح l = 0; l < loc; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

تهی cleanInvalidate_L1D(تهی)
{
    cleanInvalidate_D_by_level(0);
}
