/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <api/types.h>
#ضمن <arch/machine.h>
#ضمن <arch/machine/hardware.h>
#ضمن <arch/machine/l2c_310.h>

#define سطر_START(a) ROUND_DOWN(a, L1_CACHE_سطر_SIZE_BITS)
#define سطر_INDEX(a) (سطر_START(a)>>L1_CACHE_سطر_SIZE_BITS)

ساكن فراغ cleanCacheRange_PoC(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t سطر;
    word_t index;

    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        cleanByVA(سطر, pstart + (سطر - start));
    }
}

فراغ cleanInvalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t سطر;
    word_t index;
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */

    /* First clean the L1 range */
    cleanCacheRange_PoC(start, end, pstart);

    /* ensure operation completes and visible in L2 */
    dsb();

    /* Now clean and invalidate the L2 range */
    plat_cleanInvalidateL2Range(pstart, pstart + (end - start));

    /* Finally clean and invalidate the L1 range. The extra clean is only strictly necessary
     * in a multiprocessor environment to prevent a write being lost if another core is
     * attempting a store at the same time. As the range should already be clean asking
     * it to clean again should not affect performance */
    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        cleanInvalByVA(سطر, pstart + (سطر - start));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

فراغ cleanCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* clean l1 to l2 */
    cleanCacheRange_PoC(start, end, pstart);

    /* ensure cache operation completes before cleaning l2 */
    dsb();

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* now clean l2 to RAM */
    plat_cleanL2Range(pstart, pstart + (end - start));
}

فراغ cleanCacheRange_PoU(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t سطر;
    word_t index;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        cleanByVA_PoU(سطر, pstart + (سطر - start));
    }
}

فراغ invalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t سطر;
    word_t index;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    إذا (start != سطر_START(start)) {
        cleanCacheRange_RAM(start, start, pstart);
    }
    إذا (end + 1 != سطر_START(end + 1)) {
        سطر = سطر_START(end);
        cleanCacheRange_RAM(سطر, سطر, pstart + (سطر - start));
    }

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Invalidate L2 range. Invalidating the L2 before the L1 is the order
     * given in the l2c_310 manual, as an L1 line might be allocated from the L2
     * before the L2 can be invalidated. */
    plat_invalidateL2Range(pstart, pstart + (end - start));

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Now invalidate L1 range */
    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        invalidateByVA(سطر, pstart + (سطر - start));
    }
    /* Ensure invalidate completes */
    dsb();
}

فراغ invalidateCacheRange_I(vptr_t start, vptr_t end, paddr_t pstart)
{
#إذا defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#وإلا
    vptr_t سطر;
    word_t index;

    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        invalidateByVA_I(سطر, pstart + (سطر - start));
    }
#endif
}

فراغ branchFlushRange(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t سطر;
    word_t index;

    لكل (index = سطر_INDEX(start); index < سطر_INDEX(end) + 1; index++) {
        سطر = index << L1_CACHE_سطر_SIZE_BITS;
        branchFlush(سطر, pstart + (سطر - start));
    }
}

فراغ cleanCaches_PoU(فراغ)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

فراغ cleanInvalidateL1Caches(فراغ)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

فراغ arch_clean_invalidate_caches(فراغ)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

فراغ arch_clean_invalidate_L1_caches(word_t type)
{
    dsb();
    إذا (type & BIT(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    إذا (type & BIT(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
