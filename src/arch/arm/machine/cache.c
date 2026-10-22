/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <api/types.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/machine/hardware.h>
#অন্তর্ভাব <arch/machine/l2c_310.h>

#define সারি_START(a) ROUND_DOWN(a, L1_CACHE_সারি_আকার_BITS)
#define সারি_সূচক(a) (সারি_START(a)>>L1_CACHE_সারি_আকার_BITS)

স্থির শূন্য cleanCacheRange_PoC(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t সারি;
    word_t সূচক;

    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        cleanByVA(সারি, pstart + (সারি - start));
    }
}

শূন্য cleanInvalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t সারি;
    word_t সূচক;
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
    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        cleanInvalByVA(সারি, pstart + (সারি - start));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

শূন্য cleanCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
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

শূন্য cleanCacheRange_PoU(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t সারি;
    word_t সূচক;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        cleanByVA_PoU(সারি, pstart + (সারি - start));
    }
}

শূন্য invalidateCacheRange_RAM(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t সারি;
    word_t সূচক;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    যদি (start != সারি_START(start)) {
        cleanCacheRange_RAM(start, start, pstart);
    }
    যদি (end + 1 != সারি_START(end + 1)) {
        সারি = সারি_START(end);
        cleanCacheRange_RAM(সারি, সারি, pstart + (সারি - start));
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
    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        invalidateByVA(সারি, pstart + (সারি - start));
    }
    /* Ensure invalidate completes */
    dsb();
}

শূন্য invalidateCacheRange_I(vptr_t start, vptr_t end, paddr_t pstart)
{
#যদি defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#অন্যথা
    vptr_t সারি;
    word_t সূচক;

    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        invalidateByVA_I(সারি, pstart + (সারি - start));
    }
#endif
}

শূন্য branchFlushRange(vptr_t start, vptr_t end, paddr_t pstart)
{
    vptr_t সারি;
    word_t সূচক;

    জন্য (সূচক = সারি_সূচক(start); সূচক < সারি_সূচক(end) + 1; সূচক++) {
        সারি = সূচক << L1_CACHE_সারি_আকার_BITS;
        branchFlush(সারি, pstart + (সারি - start));
    }
}

শূন্য cleanCaches_PoU(শূন্য)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

শূন্য cleanInvalidateL1Caches(শূন্য)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

শূন্য arch_clean_invalidate_caches(শূন্য)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

শূন্য arch_clean_invalidate_L1_caches(word_t type)
{
    dsb();
    যদি (type & BIT(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    যদি (type & BIT(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
