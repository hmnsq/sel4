/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_TK1_SMMU

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
    iopde_t     *iopd_خونه;
} lookupIOPDSlot_ret_t;

تعريفنوع بنية lookupIOPTSlot_ret {
    exception_t حالت;
    iopte_t     *iopt_خونه;
} lookupIOPTSlot_ret_t;


#تعریف_کړه IOPDE_VALID_ماسک    0xe0000000
#تعریف_کړه IOPTE_تشه_ماسک    0xe0000000

ساكن bool_t isIOPDEValid(iopde_t *iopde)
{
    أكد(iopde != 0);
    أرجع (iopde->ټکي[0] & IOPDE_VALID_ماسک) != 0;
}

ساكن bool_t isIOPTEEmpty(iopte_t *iopte)
{
    أكد(iopte != 0);
    أرجع (iopte->ټکي[0] & IOPTE_تشه_ماسک) == 0;
}


ساكن lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, کلمه_t io_پته)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t شاخص = plat_smmu_iopd_شاخص(io_پته);
    ret.حالت = EXCEPTION_NONE;
    ret.iopd_خونه = iopd + شاخص;
    أرجع ret;
}

ساكن lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, کلمه_t io_پته)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t شاخص;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_پته);
    إذا (pd_ret.حالت != EXCEPTION_NONE) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خونه = 0;
        أرجع pt_ret;
    }

    إذا (!isIOPDEValid(pd_ret.iopd_خونه) ||
        iopde_ptr_get_پاڼه_اندازه(pd_ret.iopd_خونه) != iopde_iopde_pt) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خونه = 0;
        أرجع pt_ret;
    }

    شاخص = plat_smmu_iopt_شاخص(io_پته);
    pt = (iopte_t *)paddr_منزل_pptr(iopde_iopde_pt_ptr_get_پته(pd_ret.iopd_خونه));

    إذا (pt == 0) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_خونه = 0;
        أرجع pt_ret;
    }

    pt_ret.حالت = EXCEPTION_NONE;
    pt_ret.iopt_خونه = pt + شاخص;
    أرجع pt_ret;
}

BOOT_کوډ se_L4_خونه_Region create_iospace_caps(cap_t ریښه_cnode_cap)
{
    se_L4_خونه_Pos پیل = ndks_boot.خونه_pos_cur;
    se_L4_خونه_Pos پای_ټکی = 0;
    cap_t        io_space_cap;
    صحيح i = 0;
    صحيح عدد_smmu = plat_smmu_init();

    إذا (عدد_smmu == 0) {
        چاپ_کړه("SMMU init failure\n");
        أرجع S_REG_تشه;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    لكل (i = 1; i <= عدد_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        إذا (!provide_cap(ریښه_cnode_cap, io_space_cap)) {
            أرجع S_REG_تشه;
        }
    }
    پای_ټکی = ndks_boot.خونه_pos_cur;
    چاپ_کړه("Region [%x to %x) for SMMU caps\n", (بینښه صحيح)پیل, (بینښه صحيح)پای_ټکی);
    أرجع (se_L4_خونه_Region) {
        پیل, پای_ټکی
    };
}

ساكن exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *خونه, iopde_t *iopd_خونه,
                                               iopde_t iopde)
{


    *iopd_خونه = iopde;
    cleanCacheRange_RAM((کلمه_t)iopd_خونه,
                        ((کلمه_t)iopd_خونه) + حجم(iopde_t),
                        addrFromPPtr(iopd_خونه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    خونه->cap = cap;
    سېټ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    کلمه_t       invLabel,
    uint32_t     اوږدتیا,
    cte_t       *خونه,
    cap_t        cap,
    کلمه_t      *بفر
)
{
    cap_t      io_space;
    کلمه_t     io_پته;
    کلمه_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    إذا (invLabel == ARMIOPage_جدول_Unmap) {
        إذا (!isFinalCapability(خونه)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_جدول(خونه->cap);
        خونه->cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(خونه->cap, 0);

        سېټ_Thread_State(ksCurThread, ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || اوږدتیا < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (invLabel != ARMIOPage_جدول_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پته   = getSyscallArg(0, بفر) & ~ماسک(SMMU_IOPD_شاخص_ښويدنه);

    إذا (cap_io_پاڼه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    paddr = pptr_منزل_paddr((فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_پته);

    إذا (isIOPDEValid(lu_ret.iopd_خونه)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTASID(cap, asid);
    cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTMapped_پته(cap, io_پته);

    أرجع performARMIOPTInvocationMap(cap, خونه, lu_ret.iopd_خونه, iopde);
}

ساكن exception_t performARMIOMapInvocation(cap_t cap, cte_t *خونه, iopte_t *iopt_خونه,
                                             iopte_t iopte)
{
    *iopt_خونه = iopte;
    cleanCacheRange_RAM((کلمه_t)iopt_خونه,
                        ((کلمه_t)iopt_خونه) + حجم(iopte_t),
                        addrFromPPtr(iopt_خونه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    خونه->cap = cap;

    سېټ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    کلمه_t       invLabel,
    uint32_t     اوږدتیا,
    cte_t       *خونه,
    cap_t        cap,
    کلمه_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    io_پته;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     چوکاټ_cap_rights;
    seL4_CapRights_t    dma_cap_rights_ماسک;
    lookupIOPTSlot_ret_t lu_ret;

    إذا (current_extra_caps.excaprefs[0] == عدم || اوږدتیا < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (generic_چوکاټ_cap_get_cap_FSize(cap) != ARMSmall_پاڼه) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_small_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پته  = getSyscallArg(1, بفر) & ~ماسک(پاڼه_بټونه);
    paddr       = pptr_منزل_paddr((فراغ *)cap_small_چوکاټ_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پته);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPTEEmpty(lu_ret.iopt_خونه)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    چوکاټ_cap_rights = cap_small_چوکاټ_cap_get_cap_FVMRights(cap);
    dma_cap_rights_ماسک = rights_From_کلمه(getSyscallArg(0, بفر));

    إذا ((چوکاټ_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } وإلا إذا (چوکاټ_cap_rights == VMReadWrite) {
        إذا (seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_ماسک)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } وإلا إذا (!seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ماسک)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } وإلا إذا (seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ماسک)) {
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
            current_syscall_error.invalid_دلیل_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

    } وإلا {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_چوکاټ_cap_سېټ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, io_پته);

    أرجع performARMIOMapInvocation(cap, خونه, lu_ret.iopt_خونه, iopte);
}


فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    کلمه_t io_پته;
    إذا (cap_io_پاڼه_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_پاڼه_جدول_cap_get_cap_IOPTASID(io_pt_cap);
        أكد(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_پته = cap_io_پاڼه_جدول_cap_get_cap_IOPTMapped_پته(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_پته);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }

        إذا (!isIOPDEValid(lu_ret.iopd_خونه) ||
            iopde_ptr_get_پاڼه_اندازه(lu_ret.iopd_خونه) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_پته(lu_ret.iopd_خونه) != (pptr_منزل_paddr((فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            أرجع;
        }

        *lu_ret.iopd_خونه = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((کلمه_t)lu_ret.iopd_خونه,
                            ((کلمه_t)lu_ret.iopd_خونه) + حجم(iopde_t),
                            addrFromPPtr(lu_ret.iopd_خونه));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    کلمه_t  io_پته;
    uint32_t asid;

    io_پته = cap_small_چوکاټ_cap_get_cap_FMapped_پته(cap);
    asid = cap_small_چوکاټ_cap_get_cap_FMapped_ASID(cap);
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پته);

    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }
    إذا (iopte_ptr_get_پته(lu_ret.iopt_خونه) != pptr_منزل_paddr((فراغ *)cap_small_چوکاټ_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_خونه = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((کلمه_t)lu_ret.iopt_خونه,
                        ((کلمه_t)lu_ret.iopt_خونه) + حجم(iopte_t),
                        addrFromPPtr(lu_ret.iopt_خونه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

فراغ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    کلمه_t   اندازه = بټ((SMMU_PD_شاخص_بټونه));
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((فراغ *)pd, 0, اندازه);
    cleanCacheRange_RAM((کلمه_t)pd, (کلمه_t)pd + اندازه, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

exception_t perform_پاڼه_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *خونه
)
{
    unmapIOPage(خونه->cap);
    خونه->cap = cap_small_چوکاټ_cap_سېټ_cap_FMapped_پته(خونه->cap, 0);
    خونه->cap = cap_small_چوکاټ_cap_سېټ_cap_FIs_IOSpace(خونه->cap, 0);
    خونه->cap = cap_small_چوکاټ_cap_سېټ_cap_FMapped_ASID(خونه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(کلمه_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}
#د_که_پای /* end of CONFIG_TK1_SMMU */
