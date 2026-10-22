/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(word_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(word_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ساكن مضمن word_t readCLID(فراغ)
{
    word_t CLID;
    asm متطاير("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    החזר CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

تعداد arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ساكن مضمن word_t readCacheSize(صحيح level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm متطاير("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm متطاير("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    החזר size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);
    صحيح l;

    עבור (l = 0; l < lou; l++) {
        אם (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            صحيح lbits = LINEBITS(s);
            صحيح assoc = ASSOC(s);
            صحيح assoc_bits = wordBits - clzl(assoc - 1);
            صحيح nsets = NSETS(s);
            صحيح w;

            עבור (w = 0; w < assoc; w++) {
                صحيح v;

                עבור (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    word_t s = readCacheSize(l, 0);
    صحيح lbits = LINEBITS(s);
    صحيح assoc = ASSOC(s);
    صحيح assoc_bits = wordBits - clzl(assoc - 1);
    صحيح nsets = NSETS(s);
    صحيح w;

    עבור (w = 0; w < assoc; w++) {
        صحيح v;

        עבור (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

فراغ cleanInvalidate_D_PoC(فراغ)
{
    صحيح clid = readCLID();
    صحيح loc = LOC(clid);
    صحيح l;

    עבור (l = 0; l < loc; l++) {
        אם (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

فراغ cleanInvalidate_L1D(فراغ)
{
    cleanInvalidate_D_by_level(0);
}
