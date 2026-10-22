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
    ཕྱིར CLID;
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

static anta_hstha word_t readCacheSize(ཧྲིལ level, bool_t instruction)
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
    ཕྱིར size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

void clean_D_PoU(void)
{
    ཧྲིལ clid = readCLID();
    ཧྲིལ lou = LOUU(clid);

    ཆེད་དུ (ཧྲིལ l = 0; l < lou; l++) {
        གལ་ཏེ (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            ཧྲིལ lbits = LINEBITS(lsize);
            ཧྲིལ assoc = ASSOC(lsize);
            ཧྲིལ assoc_bits = wordBits - clzl(assoc - 1);
            ཧྲིལ nsets = NSETS(lsize);
            ཆེད་དུ (ཧྲིལ w = 0; w < assoc; w++) {
                ཆེད་དུ (ཧྲིལ s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

static anta_hstha void cleanInvalidate_D_by_level(ཧྲིལ l)
{
    word_t lsize = readCacheSize(l, 0);
    ཧྲིལ lbits = LINEBITS(lsize);
    ཧྲིལ assoc = ASSOC(lsize);
    ཧྲིལ assoc_bits = wordBits - clzl(assoc - 1);
    ཧྲིལ nsets = NSETS(lsize);

    ཆེད་དུ (ཧྲིལ w = 0; w < assoc; w++) {
        ཆེད་དུ (ཧྲིལ s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

void cleanInvalidate_D_PoC(void)
{
    ཧྲིལ clid = readCLID();
    ཧྲིལ loc = LOC(clid);

    ཆེད་དུ (ཧྲིལ l = 0; l < loc; l++) {
        གལ་ཏེ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

void cleanInvalidate_L1D(void)
{
    cleanInvalidate_D_by_level(0);
}
