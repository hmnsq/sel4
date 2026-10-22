/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <arch/machine/hardware.h>

अटल अंतरभूत व्योम cleanByWSL(word_t wsl)
{
    यंत्र अस्थिर("dc csw, %0" : : "r"(wsl));
}

अटल अंतरभूत व्योम cleanInvalidateByWSL(word_t wsl)
{
    यंत्र अस्थिर("dc cisw, %0" : : "r"(wsl));
}

अटल अंतरभूत word_t readCLID(व्योम)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    वापस CLID;
}

#घोषणा LOUU(x)    (((x) >> 27)        & MASK(3))
#घोषणा LOC(x)     (((x) >> 24)        & MASK(3))
#घोषणा LOUIS(x)   (((x) >> 21)        & MASK(3))
#घोषणा CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

क्रमागत arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

अटल अंतरभूत word_t read_Cache_आकार(पूर्णांक level, bool_t instruction)
{
    word_t आकार, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", आकार);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    वापस आकार;
}

#घोषणा LINEBITS(s)     (((s) & MASK(3)) + 4)
#घोषणा ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#घोषणा NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

व्योम clean_D_PoU(व्योम)
{
    पूर्णांक clid = readCLID();
    पूर्णांक lou = LOUU(clid);

    क्रम (पूर्णांक l = 0; l < lou; l++) {
        अगर (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = read_Cache_आकार(l, 0);
            पूर्णांक lbits = LINEBITS(lsize);
            पूर्णांक assoc = ASSOC(lsize);
            पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
            पूर्णांक nsets = NSETS(lsize);
            क्रम (पूर्णांक w = 0; w < assoc; w++) {
                क्रम (पूर्णांक s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

अटल अंतरभूत व्योम cleanInvalidate_D_by_level(पूर्णांक l)
{
    word_t lsize = read_Cache_आकार(l, 0);
    पूर्णांक lbits = LINEBITS(lsize);
    पूर्णांक assoc = ASSOC(lsize);
    पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
    पूर्णांक nsets = NSETS(lsize);

    क्रम (पूर्णांक w = 0; w < assoc; w++) {
        क्रम (पूर्णांक s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

व्योम cleanInvalidate_D_PoC(व्योम)
{
    पूर्णांक clid = readCLID();
    पूर्णांक loc = LOC(clid);

    क्रम (पूर्णांक l = 0; l < loc; l++) {
        अगर (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

व्योम cleanInvalidate_L1D(व्योम)
{
    cleanInvalidate_D_by_level(0);
}
