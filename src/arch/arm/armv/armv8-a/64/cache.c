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
    반환 CLID;
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

static inline word_t readCacheSize(정수 level, bool_t instruction)
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
    반환 size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

void clean_D_PoU(void)
{
    정수 clid = readCLID();
    정수 lou = LOUU(clid);

    위해 (정수 l = 0; l < lou; l++) {
        만약 (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            정수 lbits = LINEBITS(lsize);
            정수 assoc = ASSOC(lsize);
            정수 assoc_bits = wordBits - clzl(assoc - 1);
            정수 nsets = NSETS(lsize);
            위해 (정수 w = 0; w < assoc; w++) {
                위해 (정수 s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

static inline void cleanInvalidate_D_by_level(정수 l)
{
    word_t lsize = readCacheSize(l, 0);
    정수 lbits = LINEBITS(lsize);
    정수 assoc = ASSOC(lsize);
    정수 assoc_bits = wordBits - clzl(assoc - 1);
    정수 nsets = NSETS(lsize);

    위해 (정수 w = 0; w < assoc; w++) {
        위해 (정수 s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    정수 clid = readCLID();
    정수 loc = LOC(clid);

    위해 (정수 l = 0; l < loc; l++) {
        만약 (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
