/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(لفظ_t wsl)
{
    asm متطاير("dc csw, %0" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(لفظ_t wsl)
{
    asm متطاير("dc cisw, %0" : : "r"(wsl));
}

ساكن مضمن لفظ_t readCLID(فراغ)
{
    لفظ_t CLID;
    MRS("clidr_el1", CLID);
    أرجع CLID;
}

#وصف_ڪريو LOUU(x)    (((x) >> 27)        & نقاب(3))
#وصف_ڪريو LOC(x)     (((x) >> 24)        & نقاب(3))
#وصف_ڪريو LOUIS(x)   (((x) >> 21)        & نقاب(3))
#وصف_ڪريو CTYPE(x,n) (((x) >> (n*3))     & نقاب(3))

تعداد arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ساكن مضمن لفظ_t read_Cache_سائيز(صحيح level, bool_t instruction)
{
    لفظ_t سائيز, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", سائيز);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    أرجع سائيز;
}

#وصف_ڪريو LINEBITS(s)     (((s) & نقاب(3)) + 4)
#وصف_ڪريو ASSOC(s)        ((((s) >> 3) & نقاب(10)) + 1)
#وصف_ڪريو NSETS(s)        ((((s) >> 13) & نقاب(15)) + 1)

فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);

    لكل (صحيح l = 0; l < lou; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            لفظ_t lsize = read_Cache_سائيز(l, 0);
            صحيح lbits = LINEBITS(lsize);
            صحيح assoc = ASSOC(lsize);
            صحيح assoc_بٽ = لفظ_بٽ - clzl(assoc - 1);
            صحيح nsets = NSETS(lsize);
            لكل (صحيح w = 0; w < assoc; w++) {
                لكل (صحيح s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_بٽ)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    لفظ_t lsize = read_Cache_سائيز(l, 0);
    صحيح lbits = LINEBITS(lsize);
    صحيح assoc = ASSOC(lsize);
    صحيح assoc_بٽ = لفظ_بٽ - clzl(assoc - 1);
    صحيح nsets = NSETS(lsize);

    لكل (صحيح w = 0; w < assoc; w++) {
        لكل (صحيح s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_بٽ)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

فراغ cleanInvalidate_D_PoC(فراغ)
{
    صحيح clid = readCLID();
    صحيح loc = LOC(clid);

    لكل (صحيح l = 0; l < loc; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

فراغ cleanInvalidate_L1D(فراغ)
{
    cleanInvalidate_D_by_level(0);
}
