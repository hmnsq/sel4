/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <arch/machine/hardware.h>

static anta_hstha void cleanByWSL(word_t wsl)
{
    asm ca_ncala("dc csw, %0" : : "r"(wsl));
}

static anta_hstha void cleanInvalidateByWSL(word_t wsl)
{
    asm ca_ncala("dc cisw, %0" : : "r"(wsl));
}

static anta_hstha word_t readCLID(void)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    คืน CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ga_nanaa arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

static anta_hstha word_t readCacheSize(จำนวนเต็ม level, bool_t instruction)
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
    คืน size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

void clean_D_PoU(void)
{
    จำนวนเต็ม clid = readCLID();
    จำนวนเต็ม lou = LOUU(clid);

    สำหรับ (จำนวนเต็ม l = 0; l < lou; l++) {
        ถ้า (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            จำนวนเต็ม lbits = LINEBITS(lsize);
            จำนวนเต็ม assoc = ASSOC(lsize);
            จำนวนเต็ม assoc_bits = wordBits - clzl(assoc - 1);
            จำนวนเต็ม nsets = NSETS(lsize);
            สำหรับ (จำนวนเต็ม w = 0; w < assoc; w++) {
                สำหรับ (จำนวนเต็ม s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

static anta_hstha void cleanInvalidate_D_by_level(จำนวนเต็ม l)
{
    word_t lsize = readCacheSize(l, 0);
    จำนวนเต็ม lbits = LINEBITS(lsize);
    จำนวนเต็ม assoc = ASSOC(lsize);
    จำนวนเต็ม assoc_bits = wordBits - clzl(assoc - 1);
    จำนวนเต็ม nsets = NSETS(lsize);

    สำหรับ (จำนวนเต็ม w = 0; w < assoc; w++) {
        สำหรับ (จำนวนเต็ม s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    จำนวนเต็ม clid = readCLID();
    จำนวนเต็ม loc = LOC(clid);

    สำหรับ (จำนวนเต็ม l = 0; l < loc; l++) {
        ถ้า (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
