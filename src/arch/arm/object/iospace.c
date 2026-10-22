/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_TK1_SMMU

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
    exception_t وضعیت;
    iopde_t     *iopd_جایگاه;
} lookupIOPDSlot_ret_t;

تعريفنوع بنية lookupIOPTSlot_ret {
    exception_t وضعیت;
    iopte_t     *iopt_جایگاه;
} lookupIOPTSlot_ret_t;


#تعریف_کن IOPDE_VALID_ماسک    0xe0000000
#تعریف_کن IOPTE_خالی_ماسک    0xe0000000

ساكن bool_t isIOPDEValid(iopde_t *iopde)
{
    أكد(iopde != 0);
    أرجع (iopde->واژگان[0] & IOPDE_VALID_ماسک) != 0;
}

ساكن bool_t isIOPTEEmpty(iopte_t *iopte)
{
    أكد(iopte != 0);
    أرجع (iopte->واژگان[0] & IOPTE_خالی_ماسک) == 0;
}


ساكن lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, کلمه_t io_آدرس)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t شاخص = plat_smmu_iopd_شاخص(io_آدرس);
    ret.وضعیت = EXCEPTION_NONE;
    ret.iopd_جایگاه = iopd + شاخص;
    أرجع ret;
}

ساكن lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, کلمه_t io_آدرس)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t شاخص;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_آدرس);
    إذا (pd_ret.وضعیت != EXCEPTION_NONE) {
        pt_ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جایگاه = 0;
        أرجع pt_ret;
    }

    إذا (!isIOPDEValid(pd_ret.iopd_جایگاه) ||
        iopde_ptr_get_صفحه_اندازه(pd_ret.iopd_جایگاه) != iopde_iopde_pt) {
        pt_ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جایگاه = 0;
        أرجع pt_ret;
    }

    شاخص = plat_smmu_iopt_شاخص(io_آدرس);
    pt = (iopte_t *)paddr_مقصد_pptr(iopde_iopde_pt_ptr_get_آدرس(pd_ret.iopd_جایگاه));

    إذا (pt == 0) {
        pt_ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_جایگاه = 0;
        أرجع pt_ret;
    }

    pt_ret.وضعیت = EXCEPTION_NONE;
    pt_ret.iopt_جایگاه = pt + شاخص;
    أرجع pt_ret;
}

BOOT_کد se_L4_جایگاه_Region create_iospace_caps(cap_t ریشه_cnode_cap)
{
    se_L4_جایگاه_Pos شروع = ndks_boot.جایگاه_pos_cur;
    se_L4_جایگاه_Pos انتها = 0;
    cap_t        io_space_cap;
    صحيح i = 0;
    صحيح رقم_smmu = plat_smmu_init();

    إذا (رقم_smmu == 0) {
        چاپ_کن("SMMU init failure\n");
        أرجع S_REG_خالی;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    لكل (i = 1; i <= رقم_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        إذا (!provide_cap(ریشه_cnode_cap, io_space_cap)) {
            أرجع S_REG_خالی;
        }
    }
    انتها = ndks_boot.جایگاه_pos_cur;
    چاپ_کن("Region [%x to %x) for SMMU caps\n", (بیعلامت صحيح)شروع, (بیعلامت صحيح)انتها);
    أرجع (se_L4_جایگاه_Region) {
        شروع, انتها
    };
}

ساكن exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *جایگاه, iopde_t *iopd_جایگاه,
                                               iopde_t iopde)
{


    *iopd_جایگاه = iopde;
    cleanCacheRange_RAM((کلمه_t)iopd_جایگاه,
                        ((کلمه_t)iopd_جایگاه) + حجم(iopde_t),
                        addrFromPPtr(iopd_جایگاه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    جایگاه->cap = cap;
    مجموعه_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    کلمه_t       invLabel,
    uint32_t     درازی,
    cte_t       *جایگاه,
    cap_t        cap,
    کلمه_t      *میانگیر
)
{
    cap_t      io_space;
    کلمه_t     io_آدرس;
    کلمه_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    إذا (invLabel == ARMIOPage_جدول_Unmap) {
        إذا (!isFinalCapability(جایگاه)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_جدول(جایگاه->cap);
        جایگاه->cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(جایگاه->cap, 0);

        مجموعه_Thread_State(ksCurThread, ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || درازی < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (invLabel != ARMIOPage_جدول_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_آدرس   = getSyscallArg(0, میانگیر) & ~ماسک(SMMU_IOPD_شاخص_لغزش);

    إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
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

    paddr = pptr_مقصد_paddr((فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_آدرس);

    إذا (isIOPDEValid(lu_ret.iopd_جایگاه)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTASID(cap, asid);
    cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTMapped_آدرس(cap, io_آدرس);

    أرجع performARMIOPTInvocationMap(cap, جایگاه, lu_ret.iopd_جایگاه, iopde);
}

ساكن exception_t performARMIOMapInvocation(cap_t cap, cte_t *جایگاه, iopte_t *iopt_جایگاه,
                                             iopte_t iopte)
{
    *iopt_جایگاه = iopte;
    cleanCacheRange_RAM((کلمه_t)iopt_جایگاه,
                        ((کلمه_t)iopt_جایگاه) + حجم(iopte_t),
                        addrFromPPtr(iopt_جایگاه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    جایگاه->cap = cap;

    مجموعه_Thread_State(ksCurThread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    کلمه_t       invLabel,
    uint32_t     درازی,
    cte_t       *جایگاه,
    cap_t        cap,
    کلمه_t      *میانگیر
)
{
    cap_t      io_space;
    paddr_t    io_آدرس;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     قاب_cap_rights;
    seL4_CapRights_t    dma_cap_rights_ماسک;
    lookupIOPTSlot_ret_t lu_ret;

    إذا (current_extra_caps.excaprefs[0] == عدم || درازی < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (generic_قاب_cap_get_cap_FSize(cap) != ARMSmall_صفحه) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_small_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_آدرس  = getSyscallArg(1, میانگیر) & ~ماسک(صفحه_بیتها);
    paddr       = pptr_مقصد_paddr((فراغ *)cap_small_قاب_cap_get_cap_FBase_Ptr(cap));

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

    lu_ret = lookupIOPTSlot(pd, io_آدرس);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPTEEmpty(lu_ret.iopt_جایگاه)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    قاب_cap_rights = cap_small_قاب_cap_get_cap_FVMRights(cap);
    dma_cap_rights_ماسک = rights_From_کلمه(getSyscallArg(0, میانگیر));

    إذا ((قاب_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } وإلا إذا (قاب_cap_rights == VMReadWrite) {
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
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

    } وإلا {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_قاب_cap_مجموعه_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asid);
    cap = cap_small_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, io_آدرس);

    أرجع performARMIOMapInvocation(cap, جایگاه, lu_ret.iopt_جایگاه, iopte);
}


فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    کلمه_t io_آدرس;
    إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_صفحه_جدول_cap_get_cap_IOPTASID(io_pt_cap);
        أكد(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_آدرس = cap_io_صفحه_جدول_cap_get_cap_IOPTMapped_آدرس(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_آدرس);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع;
        }

        إذا (!isIOPDEValid(lu_ret.iopd_جایگاه) ||
            iopde_ptr_get_صفحه_اندازه(lu_ret.iopd_جایگاه) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_آدرس(lu_ret.iopd_جایگاه) != (pptr_مقصد_paddr((فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            أرجع;
        }

        *lu_ret.iopd_جایگاه = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((کلمه_t)lu_ret.iopd_جایگاه,
                            ((کلمه_t)lu_ret.iopd_جایگاه) + حجم(iopde_t),
                            addrFromPPtr(lu_ret.iopd_جایگاه));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    کلمه_t  io_آدرس;
    uint32_t asid;

    io_آدرس = cap_small_قاب_cap_get_cap_FMapped_آدرس(cap);
    asid = cap_small_قاب_cap_get_cap_FMapped_ASID(cap);
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_آدرس);

    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }
    إذا (iopte_ptr_get_آدرس(lu_ret.iopt_جایگاه) != pptr_مقصد_paddr((فراغ *)cap_small_قاب_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_جایگاه = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((کلمه_t)lu_ret.iopt_جایگاه,
                        ((کلمه_t)lu_ret.iopt_جایگاه) + حجم(iopte_t),
                        addrFromPPtr(lu_ret.iopt_جایگاه));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

فراغ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    کلمه_t   اندازه = بیت((SMMU_PD_شاخص_بیتها));
    أكد(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((فراغ *)pd, 0, اندازه);
    cleanCacheRange_RAM((کلمه_t)pd, (کلمه_t)pd + اندازه, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    أرجع;
}

exception_t perform_صفحه_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *جایگاه
)
{
    unmapIOPage(جایگاه->cap);
    جایگاه->cap = cap_small_قاب_cap_مجموعه_cap_FMapped_آدرس(جایگاه->cap, 0);
    جایگاه->cap = cap_small_قاب_cap_مجموعه_cap_FIs_IOSpace(جایگاه->cap, 0);
    جایگاه->cap = cap_small_قاب_cap_مجموعه_cap_FMapped_ASID(جایگاه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(کلمه_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}
#پایان_اگر /* end of CONFIG_TK1_SMMU */
