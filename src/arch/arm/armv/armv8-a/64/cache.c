/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine/hardware.h>

ساکن اندرونی خالی cleanByWSL(لفظ_t wsl)
{
    asm متلون("dc csw, %0" : : "r"(wsl));
}

ساکن اندرونی خالی cleanInvalidateByWSL(لفظ_t wsl)
{
    asm متلون("dc cisw, %0" : : "r"(wsl));
}

ساکن اندرونی لفظ_t readCLID(خالی)
{
    لفظ_t CLID;
    MRS("clidr_el1", CLID);
    واپس CLID;
}

#تعریف_کرو LOUU(x)    (((x) >> 27)        & نقاب(3))
#تعریف_کرو LOC(x)     (((x) >> 24)        & نقاب(3))
#تعریف_کرو LOUIS(x)   (((x) >> 21)        & نقاب(3))
#تعریف_کرو CTYPE(x,n) (((x) >> (n*3))     & نقاب(3))

شمار arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ساکن اندرونی لفظ_t read_Cache_سائز(صحیح level, bool_t instruction)
{
    لفظ_t سائز, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", سائز);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    واپس سائز;
}

#تعریف_کرو LINEBITS(s)     (((s) & نقاب(3)) + 4)
#تعریف_کرو ASSOC(s)        ((((s) >> 3) & نقاب(10)) + 1)
#تعریف_کرو NSETS(s)        ((((s) >> 13) & نقاب(15)) + 1)

خالی clean_D_PoU(خالی)
{
    صحیح clid = readCLID();
    صحیح lou = LOUU(clid);

    برائے (صحیح l = 0; l < lou; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            لفظ_t lsize = read_Cache_سائز(l, 0);
            صحیح lbits = LINEBITS(lsize);
            صحیح assoc = ASSOC(lsize);
            صحیح assoc_بٹ = لفظ_بٹ - clzl(assoc - 1);
            صحیح nsets = NSETS(lsize);
            برائے (صحیح w = 0; w < assoc; w++) {
                برائے (صحیح s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_بٹ)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ساکن اندرونی خالی cleanInvalidate_D_by_level(صحیح l)
{
    لفظ_t lsize = read_Cache_سائز(l, 0);
    صحیح lbits = LINEBITS(lsize);
    صحیح assoc = ASSOC(lsize);
    صحیح assoc_بٹ = لفظ_بٹ - clzl(assoc - 1);
    صحیح nsets = NSETS(lsize);

    برائے (صحیح w = 0; w < assoc; w++) {
        برائے (صحیح s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_بٹ)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

خالی cleanInvalidate_D_PoC(خالی)
{
    صحیح clid = readCLID();
    صحیح loc = LOC(clid);

    برائے (صحیح l = 0; l < loc; l++) {
        اگر (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

خالی cleanInvalidate_L1D(خالی)
{
    cleanInvalidate_D_by_level(0);
}
