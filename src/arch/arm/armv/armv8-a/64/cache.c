/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(کلمه_t wsl)
{
    asm متطاير("dc csw, %0" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(کلمه_t wsl)
{
    asm متطاير("dc cisw, %0" : : "r"(wsl));
}

ساكن مضمن کلمه_t readCLID(فراغ)
{
    کلمه_t CLID;
    MRS("clidr_el1", CLID);
    أرجع CLID;
}

#تعریف_کړه LOUU(x)    (((x) >> 27)        & ماسک(3))
#تعریف_کړه LOC(x)     (((x) >> 24)        & ماسک(3))
#تعریف_کړه LOUIS(x)   (((x) >> 21)        & ماسک(3))
#تعریف_کړه CTYPE(x,n) (((x) >> (n*3))     & ماسک(3))

تعداد arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ساكن مضمن کلمه_t read_Cache_اندازه(صحيح level, bool_t instruction)
{
    کلمه_t اندازه, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", اندازه);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    أرجع اندازه;
}

#تعریف_کړه LINEBITS(s)     (((s) & ماسک(3)) + 4)
#تعریف_کړه ASSOC(s)        ((((s) >> 3) & ماسک(10)) + 1)
#تعریف_کړه NSETS(s)        ((((s) >> 13) & ماسک(15)) + 1)

فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);

    لكل (صحيح l = 0; l < lou; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            کلمه_t lsize = read_Cache_اندازه(l, 0);
            صحيح lbits = LINEBITS(lsize);
            صحيح assoc = ASSOC(lsize);
            صحيح assoc_بټونه = کلمه_بټونه - clzl(assoc - 1);
            صحيح nsets = NSETS(lsize);
            لكل (صحيح w = 0; w < assoc; w++) {
                لكل (صحيح s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_بټونه)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    کلمه_t lsize = read_Cache_اندازه(l, 0);
    صحيح lbits = LINEBITS(lsize);
    صحيح assoc = ASSOC(lsize);
    صحيح assoc_بټونه = کلمه_بټونه - clzl(assoc - 1);
    صحيح nsets = NSETS(lsize);

    لكل (صحيح w = 0; w < assoc; w++) {
        لكل (صحيح s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_بټونه)) |
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
