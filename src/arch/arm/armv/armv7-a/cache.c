/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/machine/hardware.h>

స్థిర అంతఃవరుస శూన్య cleanByWSL(మాట_t wsl)
{
    asm అస్థిర("mcr p15, 0, %0, c7, c10, 2" : : "r"(wsl));
}

స్థిర అంతఃవరుస శూన్య cleanInvalidateByWSL(మాట_t wsl)
{
    asm అస్థిర("mcr p15, 0, %0, c7, c14, 2" : : "r"(wsl));
}


స్థిర అంతఃవరుస మాట_t readCLID(శూన్య)
{
    మాట_t CLID;
    asm అస్థిర("mrc p15, 1, %0, c0, c0, 1" : "=r"(CLID));
    తిరిగి CLID;
}

#నిర్వచించు LOUU(x)    (((x) >> 27)        & ముసుగు(3))
#నిర్వచించు LOC(x)     (((x) >> 24)        & ముసుగు(3))
#నిర్వచించు LOUIS(x)   (((x) >> 21)        & ముసుగు(3))
#నిర్వచించు CTYPE(x,n) (((x) >> (n*3))     & ముసుగు(3))

గణన arm_cache_type {
    ARMCacheNone = 0,
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
    ARMCacheU =    4,
};


స్థిర అంతఃవరుస మాట_t read_Cache_కొలత(పూర్ణాంకం level, bool_t instruction)
{
    మాట_t కొలత_unique_name, csselr_old;
    /* Save CSSELR */
    asm అస్థిర("mrc p15, 2, %0, c0, c0, 0" : "=r"(csselr_old));
    /* Select cache level */
    asm అస్థిర("mcr p15, 2, %0, c0, c0, 0" : : "r"((level << 1) | instruction));
    /* Read 'size' */
    asm అస్థిర("mrc p15, 1, %0, c0, c0, 0" : "=r"(కొలత_unique_name));
    /* Restore CSSELR */
    asm అస్థిర("mcr p15, 2, %0, c0, c0, 0" : : "r"(csselr_old));
    తిరిగి కొలత_unique_name;
}

/* Number of bits to index within a cache line.  The field is log2(nwords) - 2
 * , and thus by adding 4 we get log2(nbytes). */
#నిర్వచించు LINEBITS(s) (( (s)        & ముసుగు(3))  + 4)
/* Associativity, field is assoc - 1. */
#నిర్వచించు ASSOC(s)    ((((s) >> 3)  & ముసుగు(10)) + 1)
/* Number of sets, field is nsets - 1. */
#నిర్వచించు NSETS(s)    ((((s) >> 13) & ముసుగు(15)) + 1)


శూన్య clean_D_PoU(శూన్య)
{
    పూర్ణాంకం clid = readCLID();
    పూర్ణాంకం lou = LOUU(clid);
    పూర్ణాంకం l;

    కొరకు (l = 0; l < lou; l++) {
        ఒకవేళ (CTYPE(clid, l) > ARMCacheI) {
            మాట_t s = read_Cache_కొలత(l, 0);
            పూర్ణాంకం lbits = LINEBITS(s);
            పూర్ణాంకం assoc = ASSOC(s);
            పూర్ణాంకం assoc_బిట్లు = మాట_బిట్లు - clzl(assoc - 1);
            పూర్ణాంకం nsets = NSETS(s);
            పూర్ణాంకం w;

            కొరకు (w = 0; w < assoc; w++) {
                పూర్ణాంకం v;

                కొరకు (v = 0; v < nsets; v++) {
                    cleanByWSL((w << (32 - assoc_బిట్లు)) |
                               (v << lbits) | (l << 1));
                }
            }
        }
    }
}

స్థిర అంతఃవరుస శూన్య cleanInvalidate_D_by_level(పూర్ణాంకం l)
{
    మాట_t s = read_Cache_కొలత(l, 0);
    పూర్ణాంకం lbits = LINEBITS(s);
    పూర్ణాంకం assoc = ASSOC(s);
    పూర్ణాంకం assoc_బిట్లు = మాట_బిట్లు - clzl(assoc - 1);
    పూర్ణాంకం nsets = NSETS(s);
    పూర్ణాంకం w;

    కొరకు (w = 0; w < assoc; w++) {
        పూర్ణాంకం v;

        కొరకు (v = 0; v < nsets; v++) {
            cleanInvalidateByWSL((w << (32 - assoc_బిట్లు)) |
                                 (v << lbits) | (l << 1));
        }
    }
}

శూన్య cleanInvalidate_D_PoC(శూన్య)
{
    పూర్ణాంకం clid = readCLID();
    పూర్ణాంకం loc = LOC(clid);
    పూర్ణాంకం l;

    కొరకు (l = 0; l < loc; l++) {
        ఒకవేళ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

శూన్య cleanInvalidate_L1D(శూన్య)
{
    cleanInvalidate_D_by_level(0);
}
