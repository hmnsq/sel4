/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/l2c_310.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_ਸ਼ੁਰੂ(a) ਗੇੜਾ_ਥੱਲੇ(a, L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_ਸੂਚਕਾਂਕ(a) (ਸਤਰ_ਸ਼ੁਰੂ(a)>>L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ)

ਸਥਿਰ ਖਾਲੀ cleanCacheRange_PoC(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        cleanByVA(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
}

ਖਾਲੀ cleanInvalidateCacheRange_RAM(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */

    /* First clean the L1 range */
    cleanCacheRange_PoC(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);

    /* ensure operation completes and visible in L2 */
    dsb();

    /* Now clean and invalidate the L2 range */
    plat_cleanInvalidateL2Range(pstart, pstart + (ਸਿਰਾ - ਸ਼ੁਰੂ));

    /* Finally clean and invalidate the L1 range. The extra clean is only strictly necessary
     * in a multiprocessor environment to prevent a write being lost if another core is
     * attempting a store at the same time. As the range should already be clean asking
     * it to clean again should not affect performance */
    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        cleanInvalByVA(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

ਖਾਲੀ cleanCacheRange_RAM(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* clean l1 to l2 */
    cleanCacheRange_PoC(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);

    /* ensure cache operation completes before cleaning l2 */
    dsb();

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* now clean l2 to RAM */
    plat_cleanL2Range(pstart, pstart + (ਸਿਰਾ - ਸ਼ੁਰੂ));
}

ਖਾਲੀ cleanCacheRange_PoU(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        cleanByVA_PoU(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
}

ਖਾਲੀ invalidateCacheRange_RAM(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    ਜੇ (ਸ਼ੁਰੂ != ਸਤਰ_ਸ਼ੁਰੂ(ਸ਼ੁਰੂ)) {
        cleanCacheRange_RAM(ਸ਼ੁਰੂ, ਸ਼ੁਰੂ, pstart);
    }
    ਜੇ (ਸਿਰਾ + 1 != ਸਤਰ_ਸ਼ੁਰੂ(ਸਿਰਾ + 1)) {
        ਸਤਰ = ਸਤਰ_ਸ਼ੁਰੂ(ਸਿਰਾ);
        cleanCacheRange_RAM(ਸਤਰ, ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Invalidate L2 range. Invalidating the L2 before the L1 is the order
     * given in the l2c_310 manual, as an L1 line might be allocated from the L2
     * before the L2 can be invalidated. */
    plat_invalidateL2Range(pstart, pstart + (ਸਿਰਾ - ਸ਼ੁਰੂ));

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Now invalidate L1 range */
    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        invalidateByVA(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
    /* Ensure invalidate completes */
    dsb();
}

ਖਾਲੀ invalidateCacheRange_I(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
#ਜੇ defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#ਨਹੀਂਤਾਂ
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        invalidateByVA_I(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ branchFlushRange(vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    vptr_t ਸਤਰ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    ਲਈ (ਸੂਚਕਾਂਕ = ਸਤਰ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ); ਸੂਚਕਾਂਕ < ਸਤਰ_ਸੂਚਕਾਂਕ(ਸਿਰਾ) + 1; ਸੂਚਕਾਂਕ++) {
        ਸਤਰ = ਸੂਚਕਾਂਕ << L1_CACHE_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;
        branchFlush(ਸਤਰ, pstart + (ਸਤਰ - ਸ਼ੁਰੂ));
    }
}

ਖਾਲੀ cleanCaches_PoU(ਖਾਲੀ)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

ਖਾਲੀ cleanInvalidateL1Caches(ਖਾਲੀ)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

ਖਾਲੀ arch_clean_invalidate_caches(ਖਾਲੀ)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

ਖਾਲੀ arch_clean_invalidate_L1_caches(ਸ਼ਬਦ_t type)
{
    dsb();
    ਜੇ (type & ਦੋਆਧਾਰੀ_ਅੰਕ(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    ਜੇ (type & ਦੋਆਧਾਰੀ_ਅੰਕ(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
