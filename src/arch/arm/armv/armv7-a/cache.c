/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine/hardware.h>

ساکن اندرونی خالی cleanByWSL(لفظ_t wsl)
{
    asm متلون("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ساکن اندرونی خالی cleanInvalidateByWSL(لفظ_t wsl)
{
    asm متلون("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ساکن اندرونی لفظ_t readCLID(خالی)
{
    لفظ_t CLID;
    asm متلون("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    واپس CLID;
}

#تعریف_کرو LOUU(x)    (((x) >> 27)        & نقاب(3))
#تعریف_کرو LOC(x)     (((x) >> 24)        & نقاب(3))
#تعریف_کرو LOUIS(x)   (((x) >> 21)        & نقاب(3))
#تعریف_کرو CTYPE(x,n) (((x) >> (n*3))     & نقاب(3))

شمار arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ساکن اندرونی لفظ_t read_Cache_سائز(صحیح level, bool_t instruction)
{
    لفظ_t سائز_unique_name, csselr_old;
    /* Save CSSELR */
    asm متلون("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm متلون("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm متلون("mrc p15, 1, %0, c0, c0, 0" : "=r"(سائز_unique_name));
    /* Restore CSSELR */
    asm متلون("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    واپس سائز_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#تعریف_کرو LINEBITS(s) (( (s)        & نقاب(3))  + 4)
/* Associativity, field is assoc - 1. */
#تعریف_کرو ASSOC(s)    ((((s) >> 3)  & نقاب(10)) + 1)
/* Number of sets, field is nsets - 1. */
#تعریف_کرو NSETS(s)    ((((s) >> 13) & نقاب(15)) + 1)


خالی clean_D_PoU(خالی)
{
    صحیح clid = readCLID();
    صحیح lou = LOUU(clid);
    صحیح l;

    برائے (l = 0; l < lou; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            لفظ_t s = read_Cache_سائز(l, 0);
            صحیح lbits = LINEBITS(s);
            صحیح assoc = ASSOC(s);
            صحیح assoc_بٹ = لفظ_بٹ - clzl(assoc - 1);
            صحیح nsets = NSETS(s);
            صحیح w;

            برائے (w = 0; w < assoc; w++) {
                صحیح v;

                برائے (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_بٹ)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ساکن اندرونی خالی cleanInvalidate_D_by_level(صحیح l)
{
    لفظ_t s = read_Cache_سائز(l, 0);
    صحیح lbits = LINEBITS(s);
    صحیح assoc = ASSOC(s);
    صحیح assoc_بٹ = لفظ_بٹ - clzl(assoc - 1);
    صحیح nsets = NSETS(s);
    صحیح w;

    برائے (w = 0; w < assoc; w++) {
        صحیح v;

        برائے (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_بٹ)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

خالی cleanInvalidate_D_PoC(خالی)
{
    صحیح clid = readCLID();
    صحیح loc = LOC(clid);
    صحیح l;

    برائے (l = 0; l < loc; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

خالی cleanInvalidate_L1D(خالی)
{
    cleanInvalidate_D_by_level(0);
}
