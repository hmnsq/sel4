/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/l2c_310.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_ಆರಂಭ(a) ಸುತ್ತು_ಕೆಳಕ್ಕೆ(a, L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_ಸೂಚ್ಯಂಕ(a) (ಸಾಲು_ಆರಂಭ(a)>>L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)

ಸ್ಥಿರ ಶೂನ್ಯ cleanCacheRange_PoC(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        cleanByVA(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
}

ಶೂನ್ಯ cleanInvalidateCacheRange_RAM(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */

    /* First clean the L1 range */
    cleanCacheRange_PoC(ಆರಂಭ, ಕೊನೆ, pstart);

    /* ensure operation completes and visible in L2 */
    dsb();

    /* Now clean and invalidate the L2 range */
    plat_cleanInvalidateL2Range(pstart, pstart + (ಕೊನೆ - ಆರಂಭ));

    /* Finally clean and invalidate the L1 range. The extra clean is only strictly necessary
     * in a multiprocessor environment to prevent a write being lost if another core is
     * attempting a store at the same time. As the range should already be clean asking
     * it to clean again should not affect performance */
    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        cleanInvalByVA(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

ಶೂನ್ಯ cleanCacheRange_RAM(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* clean l1 to l2 */
    cleanCacheRange_PoC(ಆರಂಭ, ಕೊನೆ, pstart);

    /* ensure cache operation completes before cleaning l2 */
    dsb();

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* now clean l2 to RAM */
    plat_cleanL2Range(pstart, pstart + (ಕೊನೆ - ಆರಂಭ));
}

ಶೂನ್ಯ cleanCacheRange_PoU(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        cleanByVA_PoU(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
}

ಶೂನ್ಯ invalidateCacheRange_RAM(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    ಆದರೆ (ಆರಂಭ != ಸಾಲು_ಆರಂಭ(ಆರಂಭ)) {
        cleanCacheRange_RAM(ಆರಂಭ, ಆರಂಭ, pstart);
    }
    ಆದರೆ (ಕೊನೆ + 1 != ಸಾಲು_ಆರಂಭ(ಕೊನೆ + 1)) {
        ಸಾಲು = ಸಾಲು_ಆರಂಭ(ಕೊನೆ);
        cleanCacheRange_RAM(ಸಾಲು, ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Invalidate L2 range. Invalidating the L2 before the L1 is the order
     * given in the l2c_310 manual, as an L1 line might be allocated from the L2
     * before the L2 can be invalidated. */
    plat_invalidateL2Range(pstart, pstart + (ಕೊನೆ - ಆರಂಭ));

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Now invalidate L1 range */
    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        invalidateByVA(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
    /* Ensure invalidate completes */
    dsb();
}

ಶೂನ್ಯ invalidateCacheRange_I(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
#ಆದರೆ defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
    /* In cases where the hypervisor is supported, the virtual address passed
     * to this function are kernel aliases for the underlying physical memory
     * rather than the virtual address in the actual vspace. This works fine
     * when the cache is PIPT, as the cache-line is indexed by physical address,
     * and the alias maps to the same physical address. On VIPT this is not the
     * case, and it is not possible to correctly index using an aliased address.
     * As the only possible fallback the entire cache is invalidated in this
     * case
     */
    invalidate_I_PoU();
#ಇಲ್ಲದಿದ್ದರೆ
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        invalidateByVA_I(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ branchFlushRange(vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    vptr_t ಸಾಲು;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = ಸಾಲು_ಸೂಚ್ಯಂಕ(ಆರಂಭ); ಸೂಚ್ಯಂಕ < ಸಾಲು_ಸೂಚ್ಯಂಕ(ಕೊನೆ) + 1; ಸೂಚ್ಯಂಕ++) {
        ಸಾಲು = ಸೂಚ್ಯಂಕ << L1_CACHE_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
        branchFlush(ಸಾಲು, pstart + (ಸಾಲು - ಆರಂಭ));
    }
}

ಶೂನ್ಯ cleanCaches_PoU(ಶೂನ್ಯ)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

ಶೂನ್ಯ cleanInvalidateL1Caches(ಶೂನ್ಯ)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

ಶೂನ್ಯ arch_clean_invalidate_caches(ಶೂನ್ಯ)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

ಶೂನ್ಯ arch_clean_invalidate_L1_caches(ಶಬ್ದ_t type)
{
    dsb();
    ಆದರೆ (type & ಬಿಟ್(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    ಆದರೆ (type & ಬಿಟ್(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
