/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_IOMMU

#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/thread.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/intel-vtd.h>


تعريفنوع بنية lookup_VTDContext_خونه_ret {
    vtd_cte_t   *cte;
    کلمه_t      شاخص;
} lookup_VTDContext_خونه_ret_t;


BOOT_کوډ cap_t master_iospace_cap(فراغ)
{
    إذا (x86KSnumDrhu == 0) {
        أرجع cap_null_cap_new();
    }

    أرجع
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

ساكن vtd_cte_t *lookup_vtd_context_خونه(cap_t cap)
{
    uint32_t   vtd_ریښه_شاخص;
    uint32_t   vtd_context_شاخص;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_ریښه_خونه;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_خونه;

    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        اكسر;

    حالة cap_io_پاڼه_جدول_cap:
        pci_request_id = cap_io_پاڼه_جدول_cap_get_cap_IOPTIOASID(cap);
        اكسر;

    حالة cap_چوکاټ_cap:
        pci_request_id = cap_چوکاټ_cap_get_cap_FMapped_ASID(cap);
        اكسر;

    افتراضي:
        fail("Invalid cap type");
    }

    vtd_ریښه_شاخص = get_pci_bus(pci_request_id);
    vtd_ریښه_خونه = x86_KSvtd_ریښه_جدول + vtd_ریښه_شاخص;

    vtd_context = (vtd_cte_t *)paddr_منزل_pptr(vtd_rte_ptr_get_ctp(vtd_ریښه_خونه));
    vtd_context_شاخص = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_خونه = &vtd_context[vtd_context_شاخص];

    أرجع vtd_context_خونه;
}

ساكن lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_پوړونه(vtd_pte_t *iopt, کلمه_t translation,
                                                          کلمه_t پوړونه_منزل_resolve, کلمه_t پوړونه_پاتې_شوی)
{
    lookupIOPTSlot_ret_t ret;

    کلمه_t      iopt_شاخص = 0;
    vtd_pte_t   *iopt_خونه_2 = 0;
    vtd_pte_t   *بل_iopt_خونه = 0;

    إذا (iopt == 0) {
        ret.iopt_خونه = 0;
        ret.level = پوړونه_پاتې_شوی;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    iopt_شاخص = (translation  >> (VTD_PT_شاخص_بټونه * (x86KSnumIOPTLevels - 1 - (پوړونه_منزل_resolve - پوړونه_پاتې_شوی)))) &
                 ماسک(VTD_PT_شاخص_بټونه);
    iopt_خونه_2 = iopt + iopt_شاخص;

    إذا (!vtd_pte_ptr_get_write(iopt_خونه_2) || پوړونه_پاتې_شوی == 0) {
        ret.iopt_خونه = iopt_خونه_2;
        ret.level = پوړونه_پاتې_شوی;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
    بل_iopt_خونه = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(iopt_خونه_2));
    أرجع lookup_IOPTSlot_resolve_پوړونه(بل_iopt_خونه, translation, پوړونه_منزل_resolve, پوړونه_پاتې_شوی - 1);
}


ساكن مضمن lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, کلمه_t io_پته)
{
    lookupIOPTSlot_ret_t ret;

    إذا (iopt == 0) {
        ret.iopt_خونه    = 0;
        ret.level       = 0;
        ret.حالت      = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        أرجع lookup_IOPTSlot_resolve_پوړونه(iopt, io_پته >> پاڼه_بټونه,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

فراغ unmap_VTDContext_ثبت(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_خونه(cap);
    أكد(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_اندازه_بټونه);
    invalidate_iotlb();
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع;
}

ساكن exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_خونه)
{
    delete_IOPage_جدول(cap);
    cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(cap, 0);
    ct_خونه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_IOPTInvocation_Map_Context_ریښه(cap_t cap, cte_t *ct_خونه, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_خونه)
{
    *vtd_context_خونه = vtd_cte;
    flushCacheRange(vtd_context_خونه, VTD_CTE_اندازه_بټونه);
    ct_خونه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_خونه, vtd_pte_t iopte, vtd_pte_t *iopt_خونه)
{
    *iopt_خونه = iopte;
    flushCacheRange(iopt_خونه, VTD_PTE_اندازه_بټونه);
    ct_خونه->cap = cap;

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    کلمه_t       invLabel,
    کلمه_t       اوږدتیا,
    cte_t       *خونه,
    cap_t        cap,
    کلمه_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    کلمه_t   io_پته;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_خونه;
    vtd_pte_t *vtd_pte;

    إذا (invLabel == X86_IOPage_جدول_Unmap) {
        إذا (!isFinalCapability(خونه)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationUnmap(cap, خونه);
    }

    إذا (invLabel != X86_IOPage_جدول_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || اوږدتیا < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پته   = getSyscallArg(0, بفر) & ~ماسک(VTD_PT_شاخص_بټونه + se_L4_پاڼه_بټونه);

    إذا (cap_io_پاڼه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    إذا (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(VTD_PTE_PTR(cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_خونه = lookup_vtd_context_خونه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_خونه)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTLevel(cap, 0);
        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIOASID(cap, pci_request_id);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_IOPTInvocation_Map_Context_ریښه(cap, خونه, vtd_cte, vtd_context_خونه);
    } وإلا {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_خونه));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_پته);

        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_خونه) != 0) {
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTMapped_پته(cap, io_پته);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationMapPT(cap, خونه, iopte, lu_ret.iopt_خونه);
    }
}

ساكن exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_خونه, vtd_pte_t iopte, vtd_pte_t *iopt_خونه)
{
    ct_خونه->cap = cap;
    *iopt_خونه = iopte;
    flushCacheRange(iopt_خونه, VTD_PTE_اندازه_بټونه);

    أرجع EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    کلمه_t       اوږدتیا,
    cte_t       *خونه,
    cap_t        cap,
    کلمه_t      *بفر
)
{
    cap_t      io_space;
    کلمه_t     io_پته;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_خونه;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t چوکاټ_cap_rights;
    seL4_CapRights_t dma_cap_rights_ماسک;

    إذا (current_extra_caps.excaprefs[0] == عدم || اوږدتیا < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکاټ_cap_get_cap_FSize(cap) != X86_Small_پاڼه) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پته  = getSyscallArg(1, بفر) & ~ماسک(پاڼه_بټونه);
    paddr       = pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    إذا (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_خونه = lookup_vtd_context_خونه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_خونه)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_خونه));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_پته);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_خونه) != 0) {
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_ماسک = rights_From_کلمه(getSyscallArg(0, بفر));
    چوکاټ_cap_rights    = cap_چوکاټ_cap_get_cap_FVMRights(cap);

    bool_t اكتب = seL4_CapRights_get_capAllowWrite(dma_cap_rights_ماسک) && (چوکاټ_cap_rights == VMReadWrite);
    bool_t اقرأ = seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک) && (چوکاټ_cap_rights != VMKernelOnly);
    إذا (اكتب || اقرأ) {
        iopte = vtd_pte_new(paddr, !!اكتب, !!اقرأ);
    } وإلا {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_چوکاټ_cap_سېټ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, io_پته);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX86IOInvocationMap(cap, خونه, iopte, lu_ret.iopt_خونه);
}

فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    کلمه_t               io_پته;
    vtd_cte_t           *vtd_context_خونه;
    vtd_pte_t           *vtd_pte;

    إذا (cap_io_پاڼه_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_پاڼه_جدول_cap_سېټ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_پاڼه_جدول_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_خونه = lookup_vtd_context_خونه(io_pt_cap);

        إذا (!vtd_cte_ptr_get_present(vtd_context_خونه)) {
            أرجع;
        }

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_خونه));

        إذا (level == 0) {
            /* if we have been overmapped or something */
            إذا (pptr_منزل_paddr(vtd_pte) != pptr_منزل_paddr((فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                أرجع;
            }
            *vtd_context_خونه = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_خونه, VTD_CTE_اندازه_بټونه);
        } وإلا {
            io_پته = cap_io_پاڼه_جدول_cap_get_cap_IOPTMapped_پته(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_پوړونه(vtd_pte, io_پته >> پاڼه_بټونه, level - 1, level - 1);

            /* if we have been overmapped or something */
            إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
                أرجع;
            }
            إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_خونه) != pptr_منزل_paddr((فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                أرجع;
            }
            *lu_ret.iopt_خونه = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_خونه, VTD_PTE_اندازه_بټونه);
        }
        invalidate_iotlb();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    کلمه_t               io_پته;
    vtd_cte_t           *vtd_context_خونه;
    vtd_pte_t           *vtd_pte;

    io_پته  = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
    vtd_context_خونه = lookup_vtd_context_خونه(cap);


    إذا (!vtd_cte_ptr_get_present(vtd_context_خونه)) {
        أرجع;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_خونه));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_پته);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        أرجع;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_خونه) != pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_خونه = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_خونه, VTD_PTE_اندازه_بټونه);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_خونه)
{
    unmapIOPage(ct_خونه->cap);

    ct_خونه->cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(ct_خونه->cap, 0);
    ct_خونه->cap = cap_چوکاټ_cap_سېټ_cap_FMap_Type(ct_خونه->cap, X86_MappingNone);
    ct_خونه->cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(ct_خونه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(کلمه_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}

#د_که_پای /* CONFIG_IOMMU */
