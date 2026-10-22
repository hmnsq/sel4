/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <arch/machine/hardware.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanByWSL(word_t wsl)
{
    asm ଅସ୍ଥିର("dc csw, %0" : : "r"(wsl));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanInvalidateByWSL(word_t wsl)
{
    asm ଅସ୍ଥିର("dc cisw, %0" : : "r"(wsl));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readCLID(ଶୂନ୍ୟ)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    ଫେରନ୍ତୁ CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ଗଣନା arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readCacheSize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, bool_t instruction)
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
    ଫେରନ୍ତୁ size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

ଶୂନ୍ୟ clean_D_PoU(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clid = readCLID();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lou = LOUU(clid);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = 0; l < lou; l++) {
        ଯଦି (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା lbits = LINEBITS(lsize);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc = ASSOC(lsize);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc_bits = wordBits - clzl(assoc - 1);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsets = NSETS(lsize);
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = 0; w < assoc; w++) {
                ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanInvalidate_D_by_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା l)
{
    word_t lsize = readCacheSize(l, 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lbits = LINEBITS(lsize);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc = ASSOC(lsize);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc_bits = wordBits - clzl(assoc - 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsets = NSETS(lsize);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = 0; w < assoc; w++) {
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

ଶୂନ୍ୟ cleanInvalidate_D_PoC(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clid = readCLID();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା loc = LOC(clid);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = 0; l < loc; l++) {
        ଯଦି (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ଶୂନ୍ୟ cleanInvalidate_L1D(ଶୂନ୍ୟ)
{
    cleanInvalidate_D_by_level(0);
}
