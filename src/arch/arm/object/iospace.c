/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_TK1_SMMU

#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/thread.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/model/statedata.h>
#ضمن <object/structures.h>
#ضمن <linker.h>
#ضمن <plat/machine/smmu.h>


تعريفنوع بنية lookupIOPDSlot_ret {
    exception_t حالت;
    iopde_t     *iopd_تھاں;
} lookupIOPDSlot_ret_t;

تعريفنوع بنية lookupIOPTSlot_ret {
    exception_t حالت;
    iopte_t     *iopt_تھاں;
} lookupIOPTSlot_ret_t;


#تعریف_کرو IOPDE_VALID_نقاب    0xe0000000
#تعریف_کرو IOPTE_خالی_تھاواں_نقاب    0xe0000000

ساكن bool_t isIOPDEValid(iopde_t *iopde)
{
    أكد(iopde != 0);
    أرجع (iopde->الفاظ[0] & IOPDE_VALID_نقاب) != 0;
}

ساكن bool_t isIOPTEEmpty(iopte_t *iopte)
{
    أكد(iopte != 0);
    أرجع (iopte->الفاظ[0] & IOPTE_خالی_تھاواں_نقاب) == 0;
}


ساكن lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, لفظ_t io_پتہ)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t اشاریہ = plat_smmu_iopd_اشاریہ(io_پتہ);
    ret.حالت = EXCEPTION_NONE;
    ret.iopd_تھاں = iopd + اشاریہ;
    أرجع ret;
}

ساكن lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, لفظ_t io_پتہ)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t اشاریہ;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_پتہ);
    إذا (pd_ret.حالت != EXCEPTION_NONE) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_تھاں = 0;
        أرجع pt_ret;
    }

    إذا (!isIOPDEValid(pd_ret.iopd_تھاں) ||
        iopde_ptr_get_ورقہ_ناپ(pd_ret.iopd_تھاں) != iopde_iopde_pt) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_تھاں = 0;
        أرجع pt_ret;
    }

    اشاریہ = plat_smmu_iopt_اشاریہ(io_پتہ);
    pt = (iopte_t *)paddr_منزل_pptr(iopde_iopde_pt_ptr_get_پتہ(pd_ret.iopd_تھاں));

    إذا (pt == 0) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_تھاں = 0;
        أرجع pt_ret;
    }

    pt_ret.حالت = EXCEPTION_NONE;
    pt_ret.iopt_تھاں = pt + اشاریہ;
    أرجع pt_ret;
}

BOOT_کوڈ se_L4_تھاں_Region create_iospace_caps(cap_t جڑ_cnode_cap)
{
    se_L4_تھاں_Pos شروع = ndks_boot.تھاں_pos_cur;
    se_L4_تھاں_Pos سرا = 0;
    cap_t        io_space_cap;
    صحيح i = 0;
    صحيح عدد_smmu = plat_smmu_init();

    إذا (عدد_smmu == 0) {
        لکھو("SMMU init failure\n");
        أرجع S_REG_خالی_تھاواں;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    لكل (i = 1; i <= عدد_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        إذا (!provide_cap(جڑ_cnode_cap, io_space_cap)) {
            أرجع S_REG_خالی_تھاواں;
        }
    }
    سرا = ndks_boot.تھاں_pos_cur;
    لکھو("Region [%x to %x) for SMMU caps\n", (بنانشانی صحيح)شروع, (بنانشانی صحيح)سرا);
    أرجع (se_L4_تھاں_Region) {
        شروع, سرا
    };
}

ساكن exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *تھاں, iopde_t *iopd_تھاں,
                                               iopde_t iopde)
{


    *iopd_تھاں = iopde;
    cleanCacheRange_RAM((لفظ_t)iopd_تھاں,
                        ((لفظ_t)iopd_تھاں) + حجم(iopde_t),
                        addrFromPPtr(iopd_تھاں));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    تھاں->cap = cap;
    سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    لفظ_t       invLabel,
    uint32_t     لمائی,
    cte_t       *تھاں,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    لفظ_t     io_پتہ;
    لفظ_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    إذا (invLabel == ARMIOPage_جدول_Unmap) {
        إذا (!isFinalCapability(تھاں)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_جدول(تھاں->cap);
        تھاں->cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(تھاں->cap, 0);

        سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || لمائی < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (invLabel != ARMIOPage_جدول_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتہ   = getSyscallArg(0, بفر) & ~نقاب(SMMU_IOPD_اشاریہ_کھسکاؤ);

    إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    paddr = pptr_منزل_paddr((فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_پتہ);

    إذا (isIOPDEValid(lu_ret.iopd_تھاں)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTASID(cap, asid);
    cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTMapped_پتہ(cap, io_پتہ);

    أرجع performARMIOPTInvocationMap(cap, تھاں, lu_ret.iopd_تھاں, iopde);
}

ساكن exception_t performARMIOMapInvocation(cap_t cap, cte_t *تھاں, iopte_t *iopt_تھاں,
                                             iopte_t iopte)
{
    *iopt_تھاں = iopte;
    cleanCacheRange_RAM((لفظ_t)iopt_تھاں,
                        ((لفظ_t)iopt_تھاں) + حجم(iopte_t),
                        addrFromPPtr(iopt_تھاں));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    تھاں->cap = cap;

    سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    لفظ_t       invLabel,
    uint32_t     لمائی,
    cte_t       *تھاں,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    io_پتہ;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     چوکھٹا_cap_rights;
    seL4_CapRights_t    dma_cap_rights_نقاب;
    lookupIOPTSlot_ret_t lu_ret;

    إذا (current_extra_caps.excaprefs[0] == عدم || لمائی < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (generic_چوکھٹا_cap_get_cap_FSize(cap) != ARMSmall_ورقہ) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتہ  = getSyscallArg(1, بفر) & ~نقاب(ورقہ_بٹ);
    paddr       = pptr_منزل_paddr((فراغ *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پتہ);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPTEEmpty(lu_ret.iopt_تھاں)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    چوکھٹا_cap_rights = cap_small_چوکھٹا_cap_get_cap_FVMRights(cap);
    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));

    إذا ((چوکھٹا_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } وإلا إذا (چوکھٹا_cap_rights == VMReadWrite) {
        إذا (seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } وإلا إذا (!seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } وإلا إذا (seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } وإلا {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

    } وإلا {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, io_پتہ);

    أرجع performARMIOMapInvocation(cap, تھاں, lu_ret.iopt_تھاں, iopte);
}


فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    لفظ_t io_پتہ;
    إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_ورقہ_جدول_cap_get_cap_IOPTASID(io_pt_cap);
        أكد(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_پتہ = cap_io_ورقہ_جدول_cap_get_cap_IOPTMapped_پتہ(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_پتہ);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }

        إذا (!isIOPDEValid(lu_ret.iopd_تھاں) ||
            iopde_ptr_get_ورقہ_ناپ(lu_ret.iopd_تھاں) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_پتہ(lu_ret.iopd_تھاں) != (pptr_منزل_paddr((فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            أرجع;
        }

        *lu_ret.iopd_تھاں = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((لفظ_t)lu_ret.iopd_تھاں,
                            ((لفظ_t)lu_ret.iopd_تھاں) + حجم(iopde_t),
                            addrFromPPtr(lu_ret.iopd_تھاں));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    لفظ_t  io_پتہ;
    uint32_t asid;

    io_پتہ = cap_small_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
    asid = cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پتہ);

    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }
    إذا (iopte_ptr_get_پتہ(lu_ret.iopt_تھاں) != pptr_منزل_paddr((فراغ *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_تھاں = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((لفظ_t)lu_ret.iopt_تھاں,
                        ((لفظ_t)lu_ret.iopt_تھاں) + حجم(iopte_t),
                        addrFromPPtr(lu_ret.iopt_تھاں));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

فراغ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    لفظ_t   ناپ = ثنائی_ہندسہ((SMMU_PD_اشاریہ_بٹ));
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((فراغ *)pd, 0, ناپ);
    cleanCacheRange_RAM((لفظ_t)pd, (لفظ_t)pd + ناپ, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

exception_t perform_ورقہ_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *تھاں
)
{
    unmapIOPage(تھاں->cap);
    تھاں->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(تھاں->cap, 0);
    تھاں->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FIs_IOSpace(تھاں->cap, 0);
    تھاں->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(تھاں->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}
#جے_انت /* end of CONFIG_TK1_SMMU */
