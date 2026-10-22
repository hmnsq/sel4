/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/machine/hardware.h>

स्थिर ओळीत शून्य cleanByWSL(word_t wsl)
{
    यंत्र अस्थिर("dc csw, %0" : : "r"(wsl));
}

स्थिर ओळीत शून्य cleanInvalidateByWSL(word_t wsl)
{
    यंत्र अस्थिर("dc cisw, %0" : : "r"(wsl));
}

स्थिर ओळीत word_t readCLID(शून्य)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    परत CLID;
}

#define LOUU(x)    (((x) >> 27)        & MASK(3))
#define LOC(x)     (((x) >> 24)        & MASK(3))
#define LOUIS(x)   (((x) >> 21)        & MASK(3))
#define CTYPE(x,n) (((x) >> (n*3))     & MASK(3))

गणना arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

स्थिर ओळीत word_t readCacheSize(पूर्णांक level, bool_t instruction)
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
    परत size;
}

#define LINEBITS(s)     (((s) & MASK(3)) + 4)
#define ASSOC(s)        ((((s) >> 3) & MASK(10)) + 1)
#define NSETS(s)        ((((s) >> 13) & MASK(15)) + 1)

शून्य clean_D_PoU(शून्य)
{
    पूर्णांक clid = readCLID();
    पूर्णांक lou = LOUU(clid);

    साठी (पूर्णांक l = 0; l < lou; l++) {
        जर (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = readCacheSize(l, 0);
            पूर्णांक lbits = LINEBITS(lsize);
            पूर्णांक assoc = ASSOC(lsize);
            पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
            पूर्णांक nsets = NSETS(lsize);
            साठी (पूर्णांक w = 0; w < assoc; w++) {
                साठी (पूर्णांक s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_bits)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

स्थिर ओळीत शून्य cleanInvalidate_D_by_level(पूर्णांक l)
{
    word_t lsize = readCacheSize(l, 0);
    पूर्णांक lbits = LINEBITS(lsize);
    पूर्णांक assoc = ASSOC(lsize);
    पूर्णांक assoc_bits = wordBits - clzl(assoc - 1);
    पूर्णांक nsets = NSETS(lsize);

    साठी (पूर्णांक w = 0; w < assoc; w++) {
        साठी (पूर्णांक s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_bits)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

शून्य cleanInvalidate_D_PoC(शून्य)
{
    पूर्णांक clid = readCLID();
    पूर्णांक loc = LOC(clid);

    साठी (पूर्णांक l = 0; l < loc; l++) {
        जर (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

शून्य cleanInvalidate_L1D(शून्य)
{
    cleanInvalidate_D_by_level(0);
}
