/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_TK1_SMMU

#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <kernel/thread.h>
#شامل <arch/api/invocation.h>
#شامل <arch/object/iospace.h>
#شامل <arch/model/statedata.h>
#شامل <object/structures.h>
#شامل <linker.h>
#شامل <plat/machine/smmu.h>


قسمتعریف ساخت lookupIOPDSlot_ret {
    exception_t حالت_2;
    iopde_t     *iopd_خانہ;
} lookupIOPDSlot_ret_t;

قسمتعریف ساخت lookupIOPTSlot_ret {
    exception_t حالت_2;
    iopte_t     *iopt_خانہ;
} lookupIOPTSlot_ret_t;


#تعریف_کرو IOPDE_VALID_نقاب    0xe0000000
#تعریف_کرو IOPTE_خالی_خانے_نقاب    0xe0000000

ساکن bool_t isIOPDEValid(iopde_t *iopde)
{
    أكد(iopde != 0);
    واپس (iopde->الفاظ[0] & IOPDE_VALID_نقاب) != 0;
}

ساکن bool_t isIOPTEEmpty(iopte_t *iopte)
{
    أكد(iopte != 0);
    واپس (iopte->الفاظ[0] & IOPTE_خالی_خانے_نقاب) == 0;
}


ساکن lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, لفظ_t io_پتہ)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t اشاریہ = plat_smmu_iopd_اشاریہ(io_پتہ);
    ret.حالت_2 = EXCEPTION_NONE;
    ret.iopd_خانہ = iopd + اشاریہ;
    واپس ret;
}

ساکن lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, لفظ_t io_پتہ)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t اشاریہ;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_پتہ);
    اگر (pd_ret.حالت_2 != EXCEPTION_NONE) {
        pt_ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خانہ = 0;
        واپس pt_ret;
    }

    اگر (!isIOPDEValid(pd_ret.iopd_خانہ) ||
        iopde_ptr_get_صفحہ_سائز(pd_ret.iopd_خانہ) != iopde_iopde_pt) {
        pt_ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خانہ = 0;
        واپس pt_ret;
    }

    اشاریہ = plat_smmu_iopt_اشاریہ(io_پتہ);
    pt = (iopte_t *)paddr_مقصد_pptr(iopde_iopde_pt_ptr_get_پتہ(pd_ret.iopd_خانہ));

    اگر (pt == 0) {
        pt_ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خانہ = 0;
        واپس pt_ret;
    }

    pt_ret.حالت_2 = EXCEPTION_NONE;
    pt_ret.iopt_خانہ = pt + اشاریہ;
    واپس pt_ret;
}

BOOT_رمز se_L4_خانہ_Region create_iospace_caps(cap_t جڑ_cnode_cap)
{
    se_L4_خانہ_Pos آغاز = ndks_boot.خانہ_pos_cur;
    se_L4_خانہ_Pos انتہا = 0;
    cap_t        io_space_cap;
    صحیح i = 0;
    صحیح عدد_smmu = plat_smmu_init();

    اگر (عدد_smmu == 0) {
        لکھو("SMMU init failure\n");
        واپس S_REG_خالی_خانے;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    برائے (i = 1; i <= عدد_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        اگر (!provide_cap(جڑ_cnode_cap, io_space_cap)) {
            واپس S_REG_خالی_خانے;
        }
    }
    انتہا = ndks_boot.خانہ_pos_cur;
    لکھو("Region [%x to %x) for SMMU caps\n", (غیرعلامتی صحیح)آغاز, (غیرعلامتی صحیح)انتہا);
    واپس (se_L4_خانہ_Region) {
        آغاز, انتہا
    };
}

ساکن exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *خانہ, iopde_t *iopd_خانہ,
                                               iopde_t iopde)
{


    *iopd_خانہ = iopde;
    cleanCacheRange_RAM((لفظ_t)iopd_خانہ,
                        ((لفظ_t)iopd_خانہ) + حجم(iopde_t),
                        addrFromPPtr(iopd_خانہ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    خانہ->cap = cap;
    سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
    واپس EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    لفظ_t       invLabel,
    uint32_t     لمبائی,
    cte_t       *خانہ,
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

    اگر (invLabel == ARMIOPage_جدول_Unmap) {
        اگر (!isFinalCapability(خانہ)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_جدول(خانہ->cap);
        خانہ->cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(خانہ->cap, 0);

        سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
        واپس EXCEPTION_NONE;
    }

    اگر (current_extra_caps.excaprefs[0] == تہی || لمبائی < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (invLabel != ARMIOPage_جدول_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتہ   = getSyscallArg(0, بفر) & ~نقاب(SMMU_IOPD_اشاریہ_سرکاؤ);

    اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    paddr = pptr_مقصد_paddr((خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_پتہ);

    اگر (isIOPDEValid(lu_ret.iopd_خانہ)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTASID(cap, asid);
    cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTMapped_پتہ(cap, io_پتہ);

    واپس performARMIOPTInvocationMap(cap, خانہ, lu_ret.iopd_خانہ, iopde);
}

ساکن exception_t performARMIOMapInvocation(cap_t cap, cte_t *خانہ, iopte_t *iopt_خانہ,
                                             iopte_t iopte)
{
    *iopt_خانہ = iopte;
    cleanCacheRange_RAM((لفظ_t)iopt_خانہ,
                        ((لفظ_t)iopt_خانہ) + حجم(iopte_t),
                        addrFromPPtr(iopt_خانہ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    خانہ->cap = cap;

    سیٹ_Thread_State(ksCurThread, ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    لفظ_t       invLabel,
    uint32_t     لمبائی,
    cte_t       *خانہ,
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

    اگر (current_extra_caps.excaprefs[0] == تہی || لمبائی < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (generic_چوکھٹا_cap_get_cap_FSize(cap) != ARMSmall_صفحہ) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتہ  = getSyscallArg(1, بفر) & ~نقاب(صفحہ_بٹ);
    paddr       = pptr_مقصد_paddr((خالی *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

    اگر (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پتہ);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!isIOPTEEmpty(lu_ret.iopt_خانہ)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    چوکھٹا_cap_rights = cap_small_چوکھٹا_cap_get_cap_FVMRights(cap);
    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));

    اگر ((چوکھٹا_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } ورنہ اگر (چوکھٹا_cap_rights == VMReadWrite) {
        اگر (seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ورنہ اگر (!seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ورنہ اگر (seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ورنہ {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

    } ورنہ {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, io_پتہ);

    واپس performARMIOMapInvocation(cap, خانہ, lu_ret.iopt_خانہ, iopte);
}


خالی delete_IOPage_جدول(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    لفظ_t io_پتہ;
    اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_صفحہ_جدول_cap_get_cap_IOPTASID(io_pt_cap);
        أكد(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_پتہ = cap_io_صفحہ_جدول_cap_get_cap_IOPTMapped_پتہ(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_پتہ);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس;
        }

        اگر (!isIOPDEValid(lu_ret.iopd_خانہ) ||
            iopde_ptr_get_صفحہ_سائز(lu_ret.iopd_خانہ) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_پتہ(lu_ret.iopd_خانہ) != (pptr_مقصد_paddr((خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            واپس;
        }

        *lu_ret.iopd_خانہ = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((لفظ_t)lu_ret.iopd_خانہ,
                            ((لفظ_t)lu_ret.iopd_خانہ) + حجم(iopde_t),
                            addrFromPPtr(lu_ret.iopd_خانہ));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

خالی unmapIOPage(cap_t cap)
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

    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }
    اگر (iopte_ptr_get_پتہ(lu_ret.iopt_خانہ) != pptr_مقصد_paddr((خالی *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap))) {
        واپس;
    }

    *lu_ret.iopt_خانہ = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((لفظ_t)lu_ret.iopt_خانہ,
                        ((لفظ_t)lu_ret.iopt_خانہ) + حجم(iopte_t),
                        addrFromPPtr(lu_ret.iopt_خانہ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    واپس;
}

خالی clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    لفظ_t   سائز = ثنائی_ہندسہ((SMMU_PD_اشاریہ_بٹ));
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((خالی *)pd, 0, سائز);
    cleanCacheRange_RAM((لفظ_t)pd, (لفظ_t)pd + سائز, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    واپس;
}

exception_t perform_صفحہ_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *خانہ
)
{
    unmapIOPage(خانہ->cap);
    خانہ->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(خانہ->cap, 0);
    خانہ->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FIs_IOSpace(خانہ->cap, 0);
    خانہ->cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(خانہ->cap, asidInvalid);

    واپس EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    واپس EXCEPTION_SYSCALL_ERROR;
}
#اگر_ختم /* end of CONFIG_TK1_SMMU */
