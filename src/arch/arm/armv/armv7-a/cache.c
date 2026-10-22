/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/machine/hardware.h>

स्थिर अन्तःस्थ शून्य cleanByWSL(word_t wsl)
{
    asm चञ्चल("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

स्थिर अन्तःस्थ शून्य cleanInvalidateByWSL(word_t wsl)
{
    asm चञ्चल("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


स्थिर अन्तःस्थ word_t readCLID(शून्य)
{
    word_t CLID;
    asm चञ्चल("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    प्रत्यागम CLID;
}

#परिभाषय LOUU(x)    (((x) >> 27)        & आवरणम्(3))
#परिभाषय LOC(x)     (((x) >> 24)        & आवरणम्(3))
#परिभाषय LOUIS(x)   (((x) >> 21)        & आवरणम्(3))
#परिभाषय CTYPE(x,n) (((x) >> (n*3))     & आवरणम्(3))

गणना arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


स्थिर अन्तःस्थ word_t read_Cache_आकार(पूर्णाङ्क level, bool_t instruction)
{
    word_t आकार_unique_name, csselr_old;
    /* Save CSSELR */
    asm चञ्चल("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm चञ्चल("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm चञ्चल("mrc p15, 1, %0, c0, c0, 0" : "=r"(आकार_unique_name));
    /* Restore CSSELR */
    asm चञ्चल("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    प्रत्यागम आकार_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#परिभाषय LINEBITS(s) (( (s)        & आवरणम्(3))  + 4)
/* Associativity, field is assoc - 1. */
#परिभाषय ASSOC(s)    ((((s) >> 3)  & आवरणम्(10)) + 1)
/* Number of sets, field is nsets - 1. */
#परिभाषय NSETS(s)    ((((s) >> 13) & आवरणम्(15)) + 1)


शून्य clean_D_PoU(शून्य)
{
    पूर्णाङ्क clid = readCLID();
    पूर्णाङ्क lou = LOUU(clid);
    पूर्णाङ्क l;

    प्रति (l = 0; l < lou; l++) {
        यदि (CTYPE(clid, l) > ARMCacheI) {
            word_t s = read_Cache_आकार(l, 0);
            पूर्णाङ्क lbits = LINEBITS(s);
            पूर्णाङ्क assoc = ASSOC(s);
            पूर्णाङ्क assoc_द्वयङ्काः = word_द्वयङ्काः - clzl(assoc - 1);
            पूर्णाङ्क nsets = NSETS(s);
            पूर्णाङ्क w;

            प्रति (w = 0; w < assoc; w++) {
                पूर्णाङ्क v;

                प्रति (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_द्वयङ्काः)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

स्थिर अन्तःस्थ शून्य cleanInvalidate_D_by_level(पूर्णाङ्क l)
{
    word_t s = read_Cache_आकार(l, 0);
    पूर्णाङ्क lbits = LINEBITS(s);
    पूर्णाङ्क assoc = ASSOC(s);
    पूर्णाङ्क assoc_द्वयङ्काः = word_द्वयङ्काः - clzl(assoc - 1);
    पूर्णाङ्क nsets = NSETS(s);
    पूर्णाङ्क w;

    प्रति (w = 0; w < assoc; w++) {
        पूर्णाङ्क v;

        प्रति (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_द्वयङ्काः)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

शून्य cleanInvalidate_D_PoC(शून्य)
{
    पूर्णाङ्क clid = readCLID();
    पूर्णाङ्क loc = LOC(clid);
    पूर्णाङ्क l;

    प्रति (l = 0; l < loc; l++) {
        यदि (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

शून्य cleanInvalidate_L1D(शून्य)
{
    cleanInvalidate_D_by_level(0);
}
