/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <arch/machine/hardware.h>

സ്ഥിതം വരിയിൽ ശൂന്യം cleanByWSL(word_t wsl)
{
    asm അസ്ഥിര("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

സ്ഥിതം വരിയിൽ ശൂന്യം cleanInvalidateByWSL(word_t wsl)
{
    asm അസ്ഥിര("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


സ്ഥിതം വരിയിൽ word_t readCLID(ശൂന്യം)
{
    word_t CLID;
    asm അസ്ഥിര("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    തിരികെ CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

എണ്ണൽ arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


സ്ഥിതം വരിയിൽ word_t readCacheSize(പൂർണ്ണസംഖ്യ level, bool_t instruction)
{
    word_t size_unique_name, csselr_old;
    /* Save CSSELR */
    asm അസ്ഥിര("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm അസ്ഥിര("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm അസ്ഥിര("mrc p15, 1, %0, c0, c0, 0" : "=r"(size_unique_name));
    /* Restore CSSELR */
    asm അസ്ഥിര("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    തിരികെ size_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#define LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#define ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#define NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


ശൂന്യം clean_D_PoU(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ clid = readCLID();
    പൂർണ്ണസംഖ്യ lou = LOUU(clid);
    പൂർണ്ണസംഖ്യ l;

    വേണ്ടി (l = 0; l < lou; l++) {
        എങ്കിൽ (CTYPE(clid, l) > ARMCacheI) {
            word_t s = readCacheSize(l, 0);
            പൂർണ്ണസംഖ്യ lbits = LINEBITS(s);
            പൂർണ്ണസംഖ്യ assoc = ASSOC(s);
            പൂർണ്ണസംഖ്യ assoc_bits = wordBits - clzl(assoc - 1);
            പൂർണ്ണസംഖ്യ nsets = NSETS(s);
            പൂർണ്ണസംഖ്യ w;

            വേണ്ടി (w = 0; w < assoc; w++) {
                പൂർണ്ണസംഖ്യ v;

                വേണ്ടി (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

സ്ഥിതം വരിയിൽ ശൂന്യം cleanInvalidate_D_by_level(പൂർണ്ണസംഖ്യ l)
{
    word_t s = readCacheSize(l, 0);
    പൂർണ്ണസംഖ്യ lbits = LINEBITS(s);
    പൂർണ്ണസംഖ്യ assoc = ASSOC(s);
    പൂർണ്ണസംഖ്യ assoc_bits = wordBits - clzl(assoc - 1);
    പൂർണ്ണസംഖ്യ nsets = NSETS(s);
    പൂർണ്ണസംഖ്യ w;

    വേണ്ടി (w = 0; w < assoc; w++) {
        പൂർണ്ണസംഖ്യ v;

        വേണ്ടി (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

ശൂന്യം cleanInvalidate_D_PoC(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ clid = readCLID();
    പൂർണ്ണസംഖ്യ loc = LOC(clid);
    പൂർണ്ണസംഖ്യ l;

    വേണ്ടി (l = 0; l < loc; l++) {
        എങ്കിൽ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

ശൂന്യം cleanInvalidate_L1D(ശൂന്യം)
{
    cleanInvalidate_D_by_level(0);
}
