/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/machine/hardware.h>

స్థిర అంతఃవరుస శూన్య cleanByWSL(మాట_t wsl)
{
    asm అస్థిర("dc csw, %0" : : "r"(wsl));
}

స్థిర అంతఃవరుస శూన్య cleanInvalidateByWSL(మాట_t wsl)
{
    asm అస్థిర("dc cisw, %0" : : "r"(wsl));
}

స్థిర అంతఃవరుస మాట_t readCLID(శూన్య)
{
    మాట_t CLID;
    MRS("clidr_el1", CLID);
    తిరిగి CLID;
}

#నిర్వచించు LOUU(x)    (((x) >> 27)        & ముసుగు(3))
#నిర్వచించు LOC(x)     (((x) >> 24)        & ముసుగు(3))
#నిర్వచించు LOUIS(x)   (((x) >> 21)        & ముసుగు(3))
#నిర్వచించు CTYPE(x,n) (((x) >> (n*3))     & ముసుగు(3))

గణన arm_cache_type {
    ARMCacheI =    1,
    ARMCacheD =    2,
    ARMCacheID =   3,
};

స్థిర అంతఃవరుస మాట_t read_Cache_కొలత(పూర్ణాంకం level, bool_t instruction)
{
    మాట_t కొలత, csselr_old;
    /* Save CSSELR */
    MRS("csselr_el1", csselr_old);
    /* Select cache level */
    MSR("csselr_el1", ((level << 1) | instruction));
    /* Read 'size' */
    MRS("ccsidr_el1", కొలత);
    /* Restore CSSELR */
    MSR("csselr_el1", csselr_old);
    తిరిగి కొలత;
}

#నిర్వచించు LINEBITS(s)     (((s) & ముసుగు(3)) + 4)
#నిర్వచించు ASSOC(s)        ((((s) >> 3) & ముసుగు(10)) + 1)
#నిర్వచించు NSETS(s)        ((((s) >> 13) & ముసుగు(15)) + 1)

శూన్య clean_D_PoU(శూన్య)
{
    పూర్ణాంకం clid = readCLID();
    పూర్ణాంకం lou = LOUU(clid);

    కొరకు (పూర్ణాంకం l = 0; l < lou; l++) {
        ఒకవేళ (CTYPE(clid, l) > ARMCacheI) {
            మాట_t lsize = read_Cache_కొలత(l, 0);
            పూర్ణాంకం lbits = LINEBITS(lsize);
            పూర్ణాంకం assoc = ASSOC(lsize);
            పూర్ణాంకం assoc_బిట్లు = మాట_బిట్లు - clzl(assoc - 1);
            పూర్ణాంకం nsets = NSETS(lsize);
            కొరకు (పూర్ణాంకం w = 0; w < assoc; w++) {
                కొరకు (పూర్ణాంకం s = 0; s < nsets; s++) {
                    cleanByWSL((w << (32 - assoc_బిట్లు)) |
                               (s << lbits) | (l << 1));
                }
            }
        }
    }
}

స్థిర అంతఃవరుస శూన్య cleanInvalidate_D_by_level(పూర్ణాంకం l)
{
    మాట_t lsize = read_Cache_కొలత(l, 0);
    పూర్ణాంకం lbits = LINEBITS(lsize);
    పూర్ణాంకం assoc = ASSOC(lsize);
    పూర్ణాంకం assoc_బిట్లు = మాట_బిట్లు - clzl(assoc - 1);
    పూర్ణాంకం nsets = NSETS(lsize);

    కొరకు (పూర్ణాంకం w = 0; w < assoc; w++) {
        కొరకు (పూర్ణాంకం s = 0; s < nsets; s++) {
            cleanInvalidateByWSL((w << (32 - assoc_బిట్లు)) |
                                 (s << lbits) | (l << 1));
        }
    }
}

శూన్య cleanInvalidate_D_PoC(శూన్య)
{
    పూర్ణాంకం clid = readCLID();
    పూర్ణాంకం loc = LOC(clid);

    కొరకు (పూర్ణాంకం l = 0; l < loc; l++) {
        ఒకవేళ (CTYPE(clid, l) > ARMCacheI) {
            cleanInvalidate_D_by_level(l);
        }
    }
}

శూన్య cleanInvalidate_L1D(శూన్య)
{
    cleanInvalidate_D_by_level(0);
}
