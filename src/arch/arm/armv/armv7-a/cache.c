/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <arch/machine/hardware.h>

સ્થિર અંતઃરેખ શૂન્ય cleanByWSL(word_t wsl)
{
    asm અસ્થિર("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

સ્થિર અંતઃરેખ શૂન્ય cleanInvalidateByWSL(word_t wsl)
{
    asm અસ્થિર("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


સ્થિર અંતઃરેખ word_t readCLID(શૂન્ય)
{
    word_t CLID;
    asm અસ્થિર("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    પાછા CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

ગણના arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


સ્થિર અંતઃરેખ word_t readCacheSize(પૂર્ણાંક level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm અસ્થિર("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm અસ્થિર("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm અસ્થિર("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm અસ્થિર("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    પાછા size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


શૂન્ય clean_D_PoU(શૂન્ય)
{
    પૂર્ણાંક clid = readCLID();
    પૂર્ણાંક lou = LOUU(clid);
    પૂર્ણાંક l;

    માટે (l = 0; l < lou; l++) {
        જો (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            પૂર્ણાંક lbits = LINEBITS(s);
            પૂર્ણાંક assoc = ASSOC(s);
            પૂર્ણાંક assoc_bits = wordBits - clzl(assoc - 1);
            પૂર્ણાંક nsets = NSETS(s);
            પૂર્ણાંક w;

            માટે (w = 0; w < assoc; w++) {
                પૂર્ણાંક v;

                માટે (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

સ્થિર અંતઃરેખ શૂન્ય cleanInvalidate_D_by_level(પૂર્ણાંક l)
{
    word_t s = readCacheSize(l, 0);
    પૂર્ણાંક lbits = LINEBITS(s);
    પૂર્ણાંક assoc = ASSOC(s);
    પૂર્ણાંક assoc_bits = wordBits - clzl(assoc - 1);
    પૂર્ણાંક nsets = NSETS(s);
    પૂર્ણાંક w;

    માટે (w = 0; w < assoc; w++) {
        પૂર્ણાંક v;

        માટે (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

શૂન્ય cleanInvalidate_D_PoC(શૂન્ય)
{
    પૂર્ણાંક clid = readCLID();
    પૂર્ણાંક loc = LOC(clid);
    પૂર્ણાંક l;

    માટે (l = 0; l < loc; l++) {
        જો (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

શૂન્ય cleanInvalidate_L1D(શૂન્ય)
{
    cleanInvalidate_D_by_level(0);
}
