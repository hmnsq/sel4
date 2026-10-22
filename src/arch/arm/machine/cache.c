/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <api/types.h>
#समावेश <arch/machine.h>
#समावेश <arch/machine/hardware.h>
#समावेश <arch/machine/l2c_310.h>

#घोषणा पंक्ति_START(a) ROUND_DOWN(a, L1_CACHE_पंक्ति_आकार_BITS)
#घोषणा पंक्ति_सूचकांक(a) (पंक्ति_START(a)>>L1_CACHE_पंक्ति_आकार_BITS)

अटल व्योम cleanCacheRange_PoC(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t पंक्ति_2;
    word_t सूचकांक;

    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        cleanByVA(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
}

व्योम cleanInvalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t पंक्ति_2;
    word_t सूचकांक;
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
    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        cleanInvalByVA(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

व्योम cleanCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
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

व्योम cleanCacheRange_PoU(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t पंक्ति_2;
    word_t सूचकांक;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        cleanByVA_PoU(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
}

व्योम invalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t पंक्ति_2;
    word_t सूचकांक;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    अगर (start != पंक्ति_START(start)) {
        cleanCacheRange_RAM(start, start, pstart);
    }
    अगर (end + 1 != पंक्ति_START(end + 1)) {
        पंक्ति_2 = पंक्ति_START(end);
        cleanCacheRange_RAM(पंक्ति_2, पंक्ति_2, pstart + (पंक्ति_2 - start));
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
    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        invalidateByVA(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
    /* Ensure invalidate completes */
    dsb();
}

व्योम invalidateCacheRange_I(vptr_t start, vptr_t end, paddr_t pstart)
{
#अगर defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#अन्यथा
    vptr_t पंक्ति_2;
    word_t सूचकांक;

    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        invalidateByVA_I(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
#पूर्ण_अगर
}

व्योम branchFlushRange(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t पंक्ति_2;
    word_t सूचकांक;

    क्रम (सूचकांक = पंक्ति_सूचकांक(start); सूचकांक < पंक्ति_सूचकांक(end) + 1; सूचकांक++) {
        पंक्ति_2 = सूचकांक << L1_CACHE_पंक्ति_आकार_BITS;
        branchFlush(पंक्ति_2, pstart + (पंक्ति_2 - start));
    }
}

व्योम cleanCaches_PoU(व्योम)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

व्योम cleanInvalidateL1Caches(व्योम)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

व्योम arch_clean_invalidate_caches(व्योम)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

व्योम arch_clean_invalidate_L1_caches(word_t type)
{
    dsb();
    अगर (type & BIT(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    अगर (type & BIT(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
