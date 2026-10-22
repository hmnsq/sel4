/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <arch/machine/hardware.h>

अटल अंतरभूत व्योम cleanByWSL(word_t wsl)
{
    यंत्र अस्थिर("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

अटल अंतरभूत व्योम cleanInvalidateByWSL(word_t wsl)
{
    यंत्र अस्थिर("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


अटल अंतरभूत word_t readCLID(व्योम)
{
    word_t CLID;
    यंत्र अस्थिर("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    वापस CLID;
}

#घोषणा LOUU(x)    (((x) >> 27)        & MASK(3))
#घोषणा LOC(x)     (((x) >> 24)        & MASK(3))
#घोषणा LOUIS(x)   (((x) >> 21)        & MASK(3))
#घोषणा CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

क्रमागत arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


अटल अंतरभूत word_t read_Cache_आकार(पूर्णांक level, bool_t instruction)
{
    word_t आकार_unique_name, csselr_old;
    /* Save CSSELR */
    यंत्र अस्थिर("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    यंत्र अस्थिर("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    यंत्र अस्थिर("mrc p15, 1, %0, c0, c0, 0" : "=r"(आकार_unique_name));
    /* Restore CSSELR */
    यंत्र अस्थिर("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    वापस आकार_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#घोषणा LINEBITS(s) (( (s)        & MASK(3))  + 4)
/* Associativity, field is assoc - 1. */
#घोषणा ASSOC(s)    ((((s) >> 3)  & MASK(10)) + 1)
/* Number of sets, field is nsets - 1. */
#घोषणा NSETS(s)    ((((s) >> 13) & MASK(15)) + 1)


व्योम clean_D_PoU(व्योम)
{
    पूर्णांक clid = readCLID();
    पूर्णांक lou = LOUU(clid);
    पूर्णांक l;

    क्रम (l = 0; l < lou; l++) {
        अगर (CTYPE(clid, l) > ARMCacheI) {
            word_t s = read_Cache_आकार(l, 0);
            पूर्णांक lbits = LINEBITS(s);
            पूर्णांक assoc = ASSOC(s);
            पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
            पूर्णांक nsets = NSETS(s);
            पूर्णांक w;

            क्रम (w = 0; w < assoc; w++) {
                पूर्णांक v;

                क्रम (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

अटल अंतरभूत व्योम cleanInvalidate_D_by_level(पूर्णांक l)
{
    word_t s = read_Cache_आकार(l, 0);
    पूर्णांक lbits = LINEBITS(s);
    पूर्णांक assoc = ASSOC(s);
    पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
    पूर्णांक nsets = NSETS(s);
    पूर्णांक w;

    क्रम (w = 0; w < assoc; w++) {
        पूर्णांक v;

        क्रम (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

व्योम cleanInvalidate_D_PoC(व्योम)
{
    पूर्णांक clid = readCLID();
    पूर्णांक loc = LOC(clid);
    पूर्णांक l;

    क्रम (l = 0; l < loc; l++) {
        अगर (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

व्योम cleanInvalidate_L1D(व्योम)
{
    cleanInvalidate_D_by_level(0);
}
