/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <api/types.h>
#شامل <arch/machine.h>
#شامل <arch/machine/hardware.h>
#شامل <arch/machine/l2c_310.h>

#تعریف_کرو سطر_آغاز(a) باری_نیچے_کو(a, L1_CACHE_سطر_سائز_بٹ)
#تعریف_کرو سطر_اشاریہ(a) (سطر_آغاز(a)>>L1_CACHE_سطر_سائز_بٹ)

ساکن خالی cleanCacheRange_PoC(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    vptr_t سطر;
    لفظ_t اشاریہ;

    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        cleanByVA(سطر, pstart + (سطر - آغاز));
    }
}

خالی cleanInvalidateCacheRange_RAM(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    vptr_t سطر;
    لفظ_t اشاریہ;
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */

    /* First clean the L1 range */
    cleanCacheRange_PoC(آغاز, انتہا, pstart);

    /* ensure operation completes and visible in L2 */
    dsb();

    /* Now clean and invalidate the L2 range */
    plat_cleanInvalidateL2Range(pstart, pstart + (انتہا - آغاز));

    /* Finally clean and invalidate the L1 range. The extra clean is only strictly necessary
     * in a multiprocessor environment to prevent a write being lost if another core is
     * attempting a store at the same time. As the range should already be clean asking
     * it to clean again should not affect performance */
    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        cleanInvalByVA(سطر, pstart + (سطر - آغاز));
    }
    /* ensure clean and invalidate complete */
    dsb();
}

خالی cleanCacheRange_RAM(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* clean l1 to l2 */
    cleanCacheRange_PoC(آغاز, انتہا, pstart);

    /* ensure cache operation completes before cleaning l2 */
    dsb();

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* now clean l2 to RAM */
    plat_cleanL2Range(pstart, pstart + (انتہا - آغاز));
}

خالی cleanCacheRange_PoU(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    vptr_t سطر;
    لفظ_t اشاریہ;

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        cleanByVA_PoU(سطر, pstart + (سطر - آغاز));
    }
}

خالی invalidateCacheRange_RAM(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    vptr_t سطر;
    لفظ_t اشاریہ;

    /* If the start and end are not aligned to a cache line boundary
     * then we need to clean the line first to prevent invalidating
     * bytes we didn't mean to. Calling the functions in this way is
     * not the most efficient method, but we assume the user will
     * rarely be this silly */
    اگر (آغاز != سطر_آغاز(آغاز)) {
        cleanCacheRange_RAM(آغاز, آغاز, pstart);
    }
    اگر (انتہا + 1 != سطر_آغاز(انتہا + 1)) {
        سطر = سطر_آغاز(انتہا);
        cleanCacheRange_RAM(سطر, سطر, pstart + (سطر - آغاز));
    }

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Invalidate L2 range. Invalidating the L2 before the L1 is the order
     * given in the l2c_310 manual, as an L1 line might be allocated from the L2
     * before the L2 can be invalidated. */
    plat_invalidateL2Range(pstart, pstart + (انتہا - آغاز));

    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end
        \<and> \<acute>pstart <= \<acute>pstart + (\<acute>end - \<acute>start), id)" */

    /* Now invalidate L1 range */
    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        invalidateByVA(سطر, pstart + (سطر - آغاز));
    }
    /* Ensure invalidate completes */
    dsb();
}

خالی invalidateCacheRange_I(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
#اگر defined(CONFIG_ARM_ICACHE_VIPT) && defined(CONFIG_ARM_HYPERVISOR_SUPPORT)
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
#ورنہ
    vptr_t سطر;
    لفظ_t اشاریہ;

    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        invalidateByVA_I(سطر, pstart + (سطر - آغاز));
    }
#اگر_ختم
}

خالی branchFlushRange(vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    vptr_t سطر;
    لفظ_t اشاریہ;

    برائے (اشاریہ = سطر_اشاریہ(آغاز); اشاریہ < سطر_اشاریہ(انتہا) + 1; اشاریہ++) {
        سطر = اشاریہ << L1_CACHE_سطر_سائز_بٹ;
        branchFlush(سطر, pstart + (سطر - آغاز));
    }
}

خالی cleanCaches_PoU(خالی)
{
    dsb();
    clean_D_PoU();
    dsb();
    invalidate_I_PoU();
    dsb();
}

خالی cleanInvalidateL1Caches(خالی)
{
    dsb();
    cleanInvalidate_D_PoC();
    dsb();
    invalidate_I_PoU();
    dsb();
}

خالی arch_clean_invalidate_caches(خالی)
{
    cleanCaches_PoU();
    plat_cleanInvalidateL2Cache();
    cleanInvalidateL1Caches();
    isb();
}

خالی arch_clean_invalidate_L1_caches(لفظ_t type)
{
    dsb();
    اگر (type & ثنائی_ہندسہ(1)) {
        cleanInvalidate_L1D();
        dsb();
    }
    اگر (type & ثنائی_ہندسہ(0)) {
        invalidate_I_PoU();
        dsb();
        isb();
    }
}
