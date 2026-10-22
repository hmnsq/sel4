/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/machine/hardware.h>

static anta_hstha void cleanByWSL(word_t wsl)
{
    asm ca_ncala("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

static anta_hstha void cleanInvalidateByWSL(word_t wsl)
{
    asm ca_ncala("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


static anta_hstha word_t readCLID(void)
{
    word_t CLID;
    asm ca_ncala("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    คืน CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ga_nanaa arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


static anta_hstha word_t readCacheSize(จำนวนเต็ม level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm ca_ncala("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm ca_ncala("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm ca_ncala("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm ca_ncala("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    คืน size_unique_name;
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
    จำนวนเต็ม clid = readCLID();
    จำนวนเต็ม lou = LOUU(clid);
    จำนวนเต็ม l;

    สำหรับ (l = 0; l < lou; l++) {
        ถ้า (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            จำนวนเต็ม lbits = LINEBITS(s);
            จำนวนเต็ม assoc = ASSOC(s);
            จำนวนเต็ม assoc_bits = wordBits - clzl(assoc - 1);
            จำนวนเต็ม nsets = NSETS(s);
            จำนวนเต็ม w;

            สำหรับ (w = 0; w < assoc; w++) {
                จำนวนเต็ม v;

                สำหรับ (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

static anta_hstha void cleanInvalidate_D_by_level(จำนวนเต็ม l)
{
    word_t s = readCacheSize(l, 0);
    จำนวนเต็ม lbits = LINEBITS(s);
    จำนวนเต็ม assoc = ASSOC(s);
    จำนวนเต็ม assoc_bits = wordBits - clzl(assoc - 1);
    จำนวนเต็ม nsets = NSETS(s);
    จำนวนเต็ม w;

    สำหรับ (w = 0; w < assoc; w++) {
        จำนวนเต็ม v;

        สำหรับ (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    จำนวนเต็ม clid = readCLID();
    จำนวนเต็ม loc = LOC(clid);
    จำนวนเต็ม l;

    สำหรับ (l = 0; l < loc; l++) {
        ถ้า (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
