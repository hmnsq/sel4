/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine/hardware.h>

ایستا درونخطی تهی cleanByWSL(word_t wsl)
{
    asm فرار("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ایستا درونخطی تهی cleanInvalidateByWSL(word_t wsl)
{
    asm فرار("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ایستا درونخطی word_t readCLID(تهی)
{
    word_t CLID;
    asm فرار("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    بازگشت CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

شمارش arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ایستا درونخطی word_t readCacheSize(صحیح level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm فرار("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm فرار("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm فرار("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm فرار("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    بازگشت size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


تهی clean_D_PoU(تهی)
{
    صحیح clid = readCLID();
    صحیح lou = LOUU(clid);
    صحیح l;

    برای (l = 0; l < lou; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            صحیح lbits = LINEBITS(s);
            صحیح assoc = ASSOC(s);
            صحیح assoc_bits = wordBits - clzl(assoc - 1);
            صحیح nsets = NSETS(s);
            صحیح w;

            برای (w = 0; w < assoc; w++) {
                صحیح v;

                برای (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ایستا درونخطی تهی cleanInvalidate_D_by_level(صحیح l)
{
    word_t s = readCacheSize(l, 0);
    صحیح lbits = LINEBITS(s);
    صحیح assoc = ASSOC(s);
    صحیح assoc_bits = wordBits - clzl(assoc - 1);
    صحیح nsets = NSETS(s);
    صحیح w;

    برای (w = 0; w < assoc; w++) {
        صحیح v;

        برای (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

تهی cleanInvalidate_D_PoC(تهی)
{
    صحیح clid = readCLID();
    صحیح loc = LOC(clid);
    صحیح l;

    برای (l = 0; l < loc; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

تهی cleanInvalidate_L1D(تهی)
{
    cleanInvalidate_D_by_level(0);
}
