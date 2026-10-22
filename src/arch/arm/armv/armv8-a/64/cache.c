/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/machine/hardware.h>

static inline void cleanByWSL(word_t wsl)
{
    asm volatile("dc csw, %0" : : "r"(wsl));
}

static inline void cleanInvalidateByWSL(word_t wsl)
{
    asm volatile("dc cisw, %0" : : "r"(wsl));
}

static inline word_t readCLID(void)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    trả_về CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

enum arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

static inline word_t readCacheSize(số_nguyên level, bool_t instruction)
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
    trả_về size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

void clean_D_PoU(void)
{
    số_nguyên clid = readCLID();
    số_nguyên lou = LOUU(clid);

    cho (số_nguyên l = 0; l < lou; l++) {
        nếu (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            số_nguyên lbits = LINEBITS(lsize);
            số_nguyên assoc = ASSOC(lsize);
            số_nguyên assoc_bits = wordBits - clzl(assoc - 1);
            số_nguyên nsets = NSETS(lsize);
            cho (số_nguyên w = 0; w < assoc; w++) {
                cho (số_nguyên s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

static inline void cleanInvalidate_D_by_level(số_nguyên l)
{
    word_t lsize = readCacheSize(l, 0);
    số_nguyên lbits = LINEBITS(lsize);
    số_nguyên assoc = ASSOC(lsize);
    số_nguyên assoc_bits = wordBits - clzl(assoc - 1);
    số_nguyên nsets = NSETS(lsize);

    cho (số_nguyên w = 0; w < assoc; w++) {
        cho (số_nguyên s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    số_nguyên clid = readCLID();
    số_nguyên loc = LOC(clid);

    cho (số_nguyên l = 0; l < loc; l++) {
        nếu (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
