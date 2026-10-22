/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(لفظ_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(لفظ_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ساكن مضمن لفظ_t readCLID(فراغ)
{
    لفظ_t CLID;
    asm متطاير("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    أرجع CLID;
}

#وصف_ڪريو LOUU(x)    (((x) >> 27)        & نقاب(3))
#وصف_ڪريو LOC(x)     (((x) >> 24)        & نقاب(3))
#وصف_ڪريو LOUIS(x)   (((x) >> 21)        & نقاب(3))
#وصف_ڪريو CTYPE(x,n) (((x) >> (n*3))     & نقاب(3))

تعداد arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ساكن مضمن لفظ_t read_Cache_سائيز(صحيح level, bool_t instruction)
{
    لفظ_t سائيز_unique_name, csselr_old;
    /* Save CSSELR */
    asm متطاير("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm متطاير("mrc p15, 1, %0, c0, c0, 0" : "=r"(سائيز_unique_name));
    /* Restore CSSELR */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    أرجع سائيز_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#وصف_ڪريو LINEBITS(s) (( (s)        & نقاب(3))  + 4)
/* Associativity, field is assoc - 1. */
#وصف_ڪريو ASSOC(s)    ((((s) >> 3)  & نقاب(10)) + 1)
/* Number of sets, field is nsets - 1. */
#وصف_ڪريو NSETS(s)    ((((s) >> 13) & نقاب(15)) + 1)


فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);
    صحيح l;

    لكل (l = 0; l < lou; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            لفظ_t s = read_Cache_سائيز(l, 0);
            صحيح lbits = LINEBITS(s);
            صحيح assoc = ASSOC(s);
            صحيح assoc_بٽ = لفظ_بٽ - clzl(assoc - 1);
            صحيح nsets = NSETS(s);
            صحيح w;

            لكل (w = 0; w < assoc; w++) {
                صحيح v;

                لكل (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_بٽ)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    لفظ_t s = read_Cache_سائيز(l, 0);
    صحيح lbits = LINEBITS(s);
    صحيح assoc = ASSOC(s);
    صحيح assoc_بٽ = لفظ_بٽ - clzl(assoc - 1);
    صحيح nsets = NSETS(s);
    صحيح w;

    لكل (w = 0; w < assoc; w++) {
        صحيح v;

        لكل (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_بٽ)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

فراغ cleanInvalidate_D_PoC(فراغ)
{
    صحيح clid = readCLID();
    صحيح loc = LOC(clid);
    صحيح l;

    لكل (l = 0; l < loc; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

فراغ cleanInvalidate_L1D(فراغ)
{
    cleanInvalidate_D_by_level(0);
}
