/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(کلمه_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(کلمه_t wsl)
{
    asm متطاير("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ساكن مضمن کلمه_t readCLID(فراغ)
{
    کلمه_t CLID;
    asm متطاير("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    أرجع CLID;
}

#تعریف_کړه LOUU(x)    (((x) >> 27)        & ماسک(3))
#تعریف_کړه LOC(x)     (((x) >> 24)        & ماسک(3))
#تعریف_کړه LOUIS(x)   (((x) >> 21)        & ماسک(3))
#تعریف_کړه CTYPE(x,n) (((x) >> (n*3))     & ماسک(3))

تعداد arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ساكن مضمن کلمه_t read_Cache_اندازه(صحيح level, bool_t instruction)
{
    کلمه_t اندازه_unique_name, csselr_old;
    /* Save CSSELR */
    asm متطاير("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm متطاير("mrc p15, 1, %0, c0, c0, 0" : "=r"(اندازه_unique_name));
    /* Restore CSSELR */
    asm متطاير("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    أرجع اندازه_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#تعریف_کړه LINEBITS(s) (( (s)        & ماسک(3))  + 4)
/* Associativity, field is assoc - 1. */
#تعریف_کړه ASSOC(s)    ((((s) >> 3)  & ماسک(10)) + 1)
/* Number of sets, field is nsets - 1. */
#تعریف_کړه NSETS(s)    ((((s) >> 13) & ماسک(15)) + 1)


فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);
    صحيح l;

    لكل (l = 0; l < lou; l++) {
        إذا (CTYPE(clid, l) > ARMCacheI) {
            کلمه_t s = read_Cache_اندازه(l, 0);
            صحيح lbits = LINEBITS(s);
            صحيح assoc = ASSOC(s);
            صحيح assoc_بټونه = کلمه_بټونه - clzl(assoc - 1);
            صحيح nsets = NSETS(s);
            صحيح w;

            لكل (w = 0; w < assoc; w++) {
                صحيح v;

                لكل (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_بټونه)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    کلمه_t s = read_Cache_اندازه(l, 0);
    صحيح lbits = LINEBITS(s);
    صحيح assoc = ASSOC(s);
    صحيح assoc_بټونه = کلمه_بټونه - clzl(assoc - 1);
    صحيح nsets = NSETS(s);
    صحيح w;

    لكل (w = 0; w < assoc; w++) {
        صحيح v;

        لكل (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_بټونه)) |
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
