/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU

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
    iopde_t     *iopd_جڳهه;
} lookupIOPDSlot_ret_t;

تعريفنوع بنية lookupIOPTSlot_ret {
    exception_t حالت;
    iopte_t     *iopt_جڳهه;
} lookupIOPTSlot_ret_t;


#وصف_ڪريو IOPDE_VALID_نقاب    0xe0000000
#وصف_ڪريو IOPTE_خالي_جايون_نقاب    0xe0000000

ساكن bool_t isIOPDEValid(iopde_t *iopde)
{
    أكد(iopde != 0);
    أرجع (iopde->لفظ[0] & IOPDE_VALID_نقاب) != 0;
}

ساكن bool_t isIOPTEEmpty(iopte_t *iopte)
{
    أكد(iopte != 0);
    أرجع (iopte->لفظ[0] & IOPTE_خالي_جايون_نقاب) == 0;
}


ساكن lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, لفظ_t io_پتو)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t اشاريو = plat_smmu_iopd_اشاريو(io_پتو);
    ret.حالت = EXCEPTION_NONE;
    ret.iopd_جڳهه = iopd + اشاريو;
    أرجع ret;
}

ساكن lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, لفظ_t io_پتو)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t اشاريو;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_پتو);
    إذا (pd_ret.حالت != EXCEPTION_NONE) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جڳهه = 0;
        أرجع pt_ret;
    }

    إذا (!isIOPDEValid(pd_ret.iopd_جڳهه) ||
        iopde_ptr_get_صفحو_سائيز(pd_ret.iopd_جڳهه) != iopde_iopde_pt) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جڳهه = 0;
        أرجع pt_ret;
    }

    اشاريو = plat_smmu_iopt_اشاريو(io_پتو);
    pt = (iopte_t *)paddr_منزل_pptr(iopde_iopde_pt_ptr_get_پتو(pd_ret.iopd_جڳهه));

    إذا (pt == 0) {
        pt_ret.حالت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جڳهه = 0;
        أرجع pt_ret;
    }

    pt_ret.حالت = EXCEPTION_NONE;
    pt_ret.iopt_جڳهه = pt + اشاريو;
    أرجع pt_ret;
}

BOOT_ڪوڊ se_L4_جڳهه_Region create_iospace_caps(cap_t پاڙ_cnode_cap)
{
    se_L4_جڳهه_Pos شروعات = ndks_boot.جڳهه_pos_cur;
    se_L4_جڳهه_Pos پڇاڙي = 0;
    cap_t        io_space_cap;
    صحيح i = 0;
    صحيح انگ_smmu = plat_smmu_init();

    إذا (انگ_smmu == 0) {
        لکو("SMMU init failure\n");
        أرجع S_REG_خالي_جايون;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    لكل (i = 1; i <= انگ_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        إذا (!provide_cap(پاڙ_cnode_cap, io_space_cap)) {
            أرجع S_REG_خالي_جايون;
        }
    }
    پڇاڙي = ndks_boot.جڳهه_pos_cur;
    لکو("Region [%x to %x) for SMMU caps\n", (بنانشان صحيح)شروعات, (بنانشان صحيح)پڇاڙي);
    أرجع (se_L4_جڳهه_Region) {
        شروعات, پڇاڙي
    };
}

ساكن exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *جڳهه, iopde_t *iopd_جڳهه,
                                               iopde_t iopde)
{


    *iopd_جڳهه = iopde;
    cleanCacheRange_RAM((لفظ_t)iopd_جڳهه,
                        ((لفظ_t)iopd_جڳهه) + حجم(iopde_t),
                        addrFromPPtr(iopd_جڳهه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    جڳهه->cap = cap;
    سيٽ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    لفظ_t       invLabel,
    uint32_t     ڊيگهائي,
    cte_t       *جڳهه,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    لفظ_t     io_پتو;
    لفظ_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    إذا (invLabel == ARMIOPage_جدول_Unmap) {
        إذا (!isFinalCapability(جڳهه)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_جدول(جڳهه->cap);
        جڳهه->cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(جڳهه->cap, 0);

        سيٽ_Thread_State(ksCurThread, ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || ڊيگهائي < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (invLabel != ARMIOPage_جدول_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتو   = getSyscallArg(0, بفر) & ~نقاب(SMMU_IOPD_اشاريو_سرڪايو);

    إذا (cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    paddr = pptr_منزل_paddr((فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_پتو);

    إذا (isIOPDEValid(lu_ret.iopd_جڳهه)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTASID(cap, asid);
    cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTMapped_پتو(cap, io_پتو);

    أرجع performARMIOPTInvocationMap(cap, جڳهه, lu_ret.iopd_جڳهه, iopde);
}

ساكن exception_t performARMIOMapInvocation(cap_t cap, cte_t *جڳهه, iopte_t *iopt_جڳهه,
                                             iopte_t iopte)
{
    *iopt_جڳهه = iopte;
    cleanCacheRange_RAM((لفظ_t)iopt_جڳهه,
                        ((لفظ_t)iopt_جڳهه) + حجم(iopte_t),
                        addrFromPPtr(iopt_جڳهه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    جڳهه->cap = cap;

    سيٽ_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    لفظ_t       invLabel,
    uint32_t     ڊيگهائي,
    cte_t       *جڳهه,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    io_پتو;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     چوکٺو_cap_rights;
    seL4_CapRights_t    dma_cap_rights_نقاب;
    lookupIOPTSlot_ret_t lu_ret;

    إذا (current_extra_caps.excaprefs[0] == عدم || ڊيگهائي < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (generic_چوکٺو_cap_get_cap_FSize(cap) != ARMSmall_صفحو) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_small_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتو  = getSyscallArg(1, بفر) & ~نقاب(صفحو_بٽ);
    paddr       = pptr_منزل_paddr((فراغ *)cap_small_چوکٺو_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    أكد(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پتو);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPTEEmpty(lu_ret.iopt_جڳهه)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    چوکٺو_cap_rights = cap_small_چوکٺو_cap_get_cap_FVMRights(cap);
    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));

    إذا ((چوکٺو_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } وإلا إذا (چوکٺو_cap_rights == VMReadWrite) {
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
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

    } وإلا {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_چوکٺو_cap_سيٽ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap, io_پتو);

    أرجع performARMIOMapInvocation(cap, جڳهه, lu_ret.iopt_جڳهه, iopte);
}


فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    لفظ_t io_پتو;
    إذا (cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_صفحو_جدول_cap_get_cap_IOPTASID(io_pt_cap);
        أكد(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_پتو = cap_io_صفحو_جدول_cap_get_cap_IOPTMapped_پتو(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_پتو);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }

        إذا (!isIOPDEValid(lu_ret.iopd_جڳهه) ||
            iopde_ptr_get_صفحو_سائيز(lu_ret.iopd_جڳهه) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_پتو(lu_ret.iopd_جڳهه) != (pptr_منزل_paddr((فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            أرجع;
        }

        *lu_ret.iopd_جڳهه = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((لفظ_t)lu_ret.iopd_جڳهه,
                            ((لفظ_t)lu_ret.iopd_جڳهه) + حجم(iopde_t),
                            addrFromPPtr(lu_ret.iopd_جڳهه));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    لفظ_t  io_پتو;
    uint32_t asid;

    io_پتو = cap_small_چوکٺو_cap_get_cap_FMapped_پتو(cap);
    asid = cap_small_چوکٺو_cap_get_cap_FMapped_ASID(cap);
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_پتو);

    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }
    إذا (iopte_ptr_get_پتو(lu_ret.iopt_جڳهه) != pptr_منزل_paddr((فراغ *)cap_small_چوکٺو_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_جڳهه = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((لفظ_t)lu_ret.iopt_جڳهه,
                        ((لفظ_t)lu_ret.iopt_جڳهه) + حجم(iopte_t),
                        addrFromPPtr(lu_ret.iopt_جڳهه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

فراغ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    لفظ_t   سائيز = ٻٽو_انگ((SMMU_PD_اشاريو_بٽ));
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((فراغ *)pd, 0, سائيز);
    cleanCacheRange_RAM((لفظ_t)pd, (لفظ_t)pd + سائيز, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

exception_t perform_صفحو_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *جڳهه
)
{
    unmapIOPage(جڳهه->cap);
    جڳهه->cap = cap_small_چوکٺو_cap_سيٽ_cap_FMapped_پتو(جڳهه->cap, 0);
    جڳهه->cap = cap_small_چوکٺو_cap_سيٽ_cap_FIs_IOSpace(جڳهه->cap, 0);
    جڳهه->cap = cap_small_چوکٺو_cap_سيٽ_cap_FMapped_ASID(جڳهه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}
#جيڪڏهن_پورو /* end of CONFIG_TK1_SMMU */
