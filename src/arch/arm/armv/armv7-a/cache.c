/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <arch/machine/hardware.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanByWSL(word_t wsl)
{
    asm ଅସ୍ଥିର("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanInvalidateByWSL(word_t wsl)
{
    asm ଅସ୍ଥିର("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readCLID(ଶୂନ୍ୟ)
{
    word_t CLID;
    asm ଅସ୍ଥିର("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    ଫେରନ୍ତୁ CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ଗଣନା arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readCacheSize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm ଅସ୍ଥିର("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm ଅସ୍ଥିର("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm ଅସ୍ଥିର("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm ଅସ୍ଥିର("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    ଫେରନ୍ତୁ size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


ଶୂନ୍ୟ clean_D_PoU(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clid = readCLID();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lou = LOUU(clid);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

    ପାଇଁ (l = 0; l < lou; l++) {
        ଯଦି (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା lbits = LINEBITS(s);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc = ASSOC(s);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc_bits = wordBits - clzl(assoc - 1);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsets = NSETS(s);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;

            ପାଇଁ (w = 0; w < assoc; w++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

                ପାଇଁ (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ cleanInvalidate_D_by_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା l)
{
    word_t s = readCacheSize(l, 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lbits = LINEBITS(s);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc = ASSOC(s);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା assoc_bits = wordBits - clzl(assoc - 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsets = NSETS(s);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;

    ପାଇଁ (w = 0; w < assoc; w++) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;

        ପାଇଁ (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

ଶୂନ୍ୟ cleanInvalidate_D_PoC(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clid = readCLID();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା loc = LOC(clid);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

    ପାଇଁ (l = 0; l < loc; l++) {
        ଯଦି (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ଶୂନ୍ୟ cleanInvalidate_L1D(ଶୂନ୍ୟ)
{
    cleanInvalidate_D_by_level(0);
}
