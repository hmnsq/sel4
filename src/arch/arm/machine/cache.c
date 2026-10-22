/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/machine/hardware.h>
#అన్తర్భావ <arch/machine/l2c_310.h>

#నిర్వచించు పంక్తి_ప్రారంభం(a) విడత_కిందికి(a, L1_CACHE_పంక్తి_కొలత_బిట్లు)
#నిర్వచించు పంక్తి_సూచీ(a) (పంక్తి_ప్రారంభం(a)>>L1_CACHE_పంక్తి_కొలత_బిట్లు)

స్థిర శూన్య cleanCacheRange_PoC(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    vptr_t పంక్తి;
    మాట_t సూచీ;

    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        cleanByVA(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
}

శూన్య cleanInvalidateCacheRange_RAM(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    vptr_t పంక్తి;
    మాట_t సూచీ;
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */

    /* First clean the L1 range */
    cleanCacheRange_PoC(ప్రారంభం, చివర, pstart);

    /* ensure operation completes and visible in L2 */
    dsb();

    /* Now clean and invalidate the L2 range */
    plat_cleanInvalidateL2Range(pstart, pstart + (చివర - ప్రారంభం));

    /* Finally clean and invalidate the L1 range. The extra clean is only strictly necessary
     * in a multiprocessor environment to prevent a write being lost if another core is
     * attempting a store at the same time. As the range should already be clean asking
     * it to clean again should not affect performance */
    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        cleanInvalByVA(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

శూన్య cleanCacheRange_RAM(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* clean l1 to l2 */
    cleanCacheRange_PoC(ప్రారంభం, చివర, pstart);

    /* ensure cache operation completes before cleaning l2 */
    dsb();

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* now clean l2 to RAM */
    plat_cleanL2Range(pstart, pstart + (చివర - ప్రారంభం));
}

శూన్య cleanCacheRange_PoU(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    vptr_t పంక్తి;
    మాట_t సూచీ;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        cleanByVA_PoU(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
}

శూన్య invalidateCacheRange_RAM(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    vptr_t పంక్తి;
    మాట_t సూచీ;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    ఒకవేళ (ప్రారంభం != పంక్తి_ప్రారంభం(ప్రారంభం)) {
        cleanCacheRange_RAM(ప్రారంభం, ప్రారంభం, pstart);
    }
    ఒకవేళ (చివర + 1 != పంక్తి_ప్రారంభం(చివర + 1)) {
        పంక్తి = పంక్తి_ప్రారంభం(చివర);
        cleanCacheRange_RAM(పంక్తి, పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Invalidate L2 range. Invalidating the L2 before the L1 is the order
     * given in the l2c_310 manual, as an L1 line might be allocated from the L2
     * before the L2 can be invalidated. */
    plat_invalidateL2Range(pstart, pstart + (చివర - ప్రారంభం));

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Now invalidate L1 range */
    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        invalidateByVA(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
    /* Ensure invalidate completes */
    dsb();
}

శూన్య invalidateCacheRange_I(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
#ఒకవేళ defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#లేకపోతే
    vptr_t పంక్తి;
    మాట_t సూచీ;

    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        invalidateByVA_I(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
#అయితే_ముగింపు
}

శూన్య branchFlushRange(vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    vptr_t పంక్తి;
    మాట_t సూచీ;

    కొరకు (సూచీ = పంక్తి_సూచీ(ప్రారంభం); సూచీ < పంక్తి_సూచీ(చివర) + 1; సూచీ++) {
        పంక్తి = సూచీ << L1_CACHE_పంక్తి_కొలత_బిట్లు;
        branchFlush(పంక్తి, pstart + (పంక్తి - ప్రారంభం));
    }
}

శూన్య cleanCaches_PoU(శూన్య)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

శూన్య cleanInvalidateL1Caches(శూన్య)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

శూన్య arch_clean_invalidate_caches(శూన్య)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

శూన్య arch_clean_invalidate_L1_caches(మాట_t type)
{
    dsb();
    ఒకవేళ (type & బిట్టు(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    ఒకవేళ (type & బిట్టు(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
