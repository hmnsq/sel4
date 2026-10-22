/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/machine/hardware.h>

स्थिर अन्तःस्थ शून्य cleanByWSL(word_t wsl)
{
    asm चञ्चल("dc csw, %0" : : "r"(wsl));
}

स्थिर अन्तःस्थ शून्य cleanInvalidateByWSL(word_t wsl)
{
    asm चञ्चल("dc cisw, %0" : : "r"(wsl));
}

स्थिर अन्तःस्थ word_t readCLID(शून्य)
{
    word_t CLID;
    MRS("clidr_el1", CLID);
    प्रत्यागम CLID;
}

#परिभाषय LOUU(x)    (((x) >> 27)        & आवरणम्(3))
#परिभाषय LOC(x)     (((x) >> 24)        & आवरणम्(3))
#परिभाषय LOUIS(x)   (((x) >> 21)        & आवरणम्(3))
#परिभाषय CTYPE(x,n) (((x) >> (n*3))     & आवरणम्(3))

गणना arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

स्थिर अन्तःस्थ word_t read_Cache_आकार(पूर्णाङ्क level, bool_t instruction)
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
    प्रत्यागम आकार;
}

#परिभाषय LINEBITS(s)     (((s) & आवरणम्(3)) + 4)
#परिभाषय ASSOC(s)        ((((s) >> 3) & आवरणम्(10)) + 1)
#परिभाषय NSETS(s)        ((((s) >> 13) & आवरणम्(15)) + 1)

शून्य clean_D_PoU(शून्य)
{
    पूर्णाङ्क clid = readCLID();
    पूर्णाङ्क lou = LOUU(clid);

    प्रति (पूर्णाङ्क l = 0; l < lou; l++) {
        यदि (CTYPE(clid, l) > ARMCacheI) {
            word_t lsize = read_Cache_आकार(l, 0);
            पूर्णाङ्क lbits = LINEBITS(lsize);
            पूर्णाङ्क assoc = ASSOC(lsize);
            पूर्णाङ्क assoc_द्वयङ्काः = word_द्वयङ्काः - clzl(assoc - 1);
            पूर्णाङ्क nsets = NSETS(lsize);
            प्रति (पूर्णाङ्क w = 0; w < assoc; w++) {
                प्रति (पूर्णाङ्क s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_द्वयङ्काः)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

स्थिर अन्तःस्थ शून्य cleanInvalidate_D_by_level(पूर्णाङ्क l)
{
    word_t lsize = read_Cache_आकार(l, 0);
    पूर्णाङ्क lbits = LINEBITS(lsize);
    पूर्णाङ्क assoc = ASSOC(lsize);
    पूर्णाङ्क assoc_द्वयङ्काः = word_द्वयङ्काः - clzl(assoc - 1);
    पूर्णाङ्क nsets = NSETS(lsize);

    प्रति (पूर्णाङ्क w = 0; w < assoc; w++) {
        प्रति (पूर्णाङ्क s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_द्वयङ्काः)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

शून्य cleanInvalidate_D_PoC(शून्य)
{
    पूर्णाङ्क clid = readCLID();
    पूर्णाङ्क loc = LOC(clid);

    प्रति (पूर्णाङ्क l = 0; l < loc; l++) {
        यदि (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

शून्य cleanInvalidate_L1D(शून्य)
{
    cleanInvalidate_D_by_level(0);
}
