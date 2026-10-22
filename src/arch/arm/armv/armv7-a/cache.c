/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/machine/hardware.h>

static inline void cleanByWSL(word_t wsl)
{
    asm volatile("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

static inline void cleanInvalidateByWSL(word_t wsl)
{
    asm volatile("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


static inline word_t readCLID(void)
{
    word_t CLID;
    asm volatile("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    返回 CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

enum arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


static inline word_t readCacheSize(整数 level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm volatile("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm volatile("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm volatile("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm volatile("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    返回 size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


void clean_D_PoU(void)
{
    整数 clid = readCLID();
    整数 lou = LOUU(clid);
    整数 l;

    为 (l = 0; l < lou; l++) {
        如果 (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            整数 lbits = LINEBITS(s);
            整数 assoc = ASSOC(s);
            整数 assoc_bits = wordBits - clzl(assoc - 1);
            整数 nsets = NSETS(s);
            整数 w;

            为 (w = 0; w < assoc; w++) {
                整数 v;

                为 (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

static inline void cleanInvalidate_D_by_level(整数 l)
{
    word_t s = readCacheSize(l, 0);
    整数 lbits = LINEBITS(s);
    整数 assoc = ASSOC(s);
    整数 assoc_bits = wordBits - clzl(assoc - 1);
    整数 nsets = NSETS(s);
    整数 w;

    为 (w = 0; w < assoc; w++) {
        整数 v;

        为 (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    整数 clid = readCLID();
    整数 loc = LOC(clid);
    整数 l;

    为 (l = 0; l < loc; l++) {
        如果 (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
