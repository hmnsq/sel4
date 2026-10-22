/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine/hardware.h>

ساكن مضمن فراغ cleanByWSL(word_t wsl)
{
    asm متطاير("dc csw, %0" : : "r"(wsl));
}

ساكن مضمن فراغ cleanInvalidateByWSL(word_t wsl)
{
    asm متطاير("dc cisw, %0" : : "r"(wsl));
}

ساكن مضمن word_t readCLID(فراغ)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    أرجع CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

تعداد arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ساكن مضمن word_t readCacheSize(صحيح level, bool_t instruction)
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
    أرجع size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

فراغ clean_D_PoU(فراغ)
{
    صحيح clid = readCLID();
    صحيح lou = LOUU(clid);

    لكل (صحيح l = 0; l < lou; l++) {
        𐤀𐤌 (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            صحيح lbits = LINEBITS(lsize);
            صحيح assoc = ASSOC(lsize);
            صحيح assoc_bits = wordBits - clzl(assoc - 1);
            صحيح nsets = NSETS(lsize);
            لكل (صحيح w = 0; w < assoc; w++) {
                لكل (صحيح s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ساكن مضمن فراغ cleanInvalidate_D_by_level(صحيح l)
{
    word_t lsize = readCacheSize(l, 0);
    صحيح lbits = LINEBITS(lsize);
    صحيح assoc = ASSOC(lsize);
    صحيح assoc_bits = wordBits - clzl(assoc - 1);
    صحيح nsets = NSETS(lsize);

    لكل (صحيح w = 0; w < assoc; w++) {
        لكل (صحيح s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

فراغ cleanInvalidate_D_PoC(فراغ)
{
    صحيح clid = readCLID();
    صحيح loc = LOC(clid);

    لكل (صحيح l = 0; l < loc; l++) {
        𐤀𐤌 (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

فراغ cleanInvalidate_L1D(فراغ)
{
    cleanInvalidate_D_by_level(0);
}
