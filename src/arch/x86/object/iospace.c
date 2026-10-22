/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU

#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/thread.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/intel-vtd.h>


تعريفنوع بنية lookup_VTDContext_جڳهه_ret {
    vtd_cte_t   *cte;
    لفظ_t      اشاريو;
} lookup_VTDContext_جڳهه_ret_t;


BOOT_ڪوڊ cap_t master_iospace_cap(فراغ)
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

ساكن vtd_cte_t *lookup_vtd_context_جڳهه(cap_t cap)
{
    uint32_t   vtd_پاڙ_اشاريو;
    uint32_t   vtd_context_اشاريو;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_پاڙ_جڳهه;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_جڳهه;

    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        اكسر;

    حالة cap_io_صفحو_جدول_cap:
        pci_request_id = cap_io_صفحو_جدول_cap_get_cap_IOPTIOASID(cap);
        اكسر;

    حالة cap_چوکٺو_cap:
        pci_request_id = cap_چوکٺو_cap_get_cap_FMapped_ASID(cap);
        اكسر;

    افتراضي:
        fail("Invalid cap type");
    }

    vtd_پاڙ_اشاريو = get_pci_bus(pci_request_id);
    vtd_پاڙ_جڳهه = x86_KSvtd_پاڙ_جدول + vtd_پاڙ_اشاريو;

    vtd_context = (vtd_cte_t *)paddr_منزل_pptr(vtd_rte_ptr_get_ctp(vtd_پاڙ_جڳهه));
    vtd_context_اشاريو = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_جڳهه = &vtd_context[vtd_context_اشاريو];

    أرجع vtd_context_جڳهه;
}

ساكن lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_سطحون(vtd_pte_t *iopt, لفظ_t translation,
                                                          لفظ_t سطحون_منزل_resolve, لفظ_t سطحون_بچيل)
{
    lookupIOPTSlot_ret_t ret;

    لفظ_t      iopt_اشاريو = 0;
    vtd_pte_t   *iopt_جڳهه_2 = 0;
    vtd_pte_t   *ايندڙ_iopt_جڳهه = 0;

    إذا (iopt == 0) {
        ret.iopt_جڳهه = 0;
        ret.level = سطحون_بچيل;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    iopt_اشاريو = (translation  >> (VTD_PT_اشاريو_بٽ * (x86KSnumIOPTLevels - 1 - (سطحون_منزل_resolve - سطحون_بچيل)))) &
                 نقاب(VTD_PT_اشاريو_بٽ);
    iopt_جڳهه_2 = iopt + iopt_اشاريو;

    إذا (!vtd_pte_ptr_get_write(iopt_جڳهه_2) || سطحون_بچيل == 0) {
        ret.iopt_جڳهه = iopt_جڳهه_2;
        ret.level = سطحون_بچيل;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
    ايندڙ_iopt_جڳهه = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(iopt_جڳهه_2));
    أرجع lookup_IOPTSlot_resolve_سطحون(ايندڙ_iopt_جڳهه, translation, سطحون_منزل_resolve, سطحون_بچيل - 1);
}


ساكن مضمن lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, لفظ_t io_پتو)
{
    lookupIOPTSlot_ret_t ret;

    إذا (iopt == 0) {
        ret.iopt_جڳهه    = 0;
        ret.level       = 0;
        ret.حالت      = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        أرجع lookup_IOPTSlot_resolve_سطحون(iopt, io_پتو >> صفحو_بٽ,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

فراغ unmap_VTDContext_داخلا(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_جڳهه(cap);
    أكد(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_سائيز_بٽ);
    invalidate_iotlb();
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع;
}

ساكن exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_جڳهه)
{
    delete_IOPage_جدول(cap);
    cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(cap, 0);
    ct_جڳهه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_IOPTInvocation_Map_Context_پاڙ(cap_t cap, cte_t *ct_جڳهه, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_جڳهه)
{
    *vtd_context_جڳهه = vtd_cte;
    flushCacheRange(vtd_context_جڳهه, VTD_CTE_سائيز_بٽ);
    ct_جڳهه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_جڳهه, vtd_pte_t iopte, vtd_pte_t *iopt_جڳهه)
{
    *iopt_جڳهه = iopte;
    flushCacheRange(iopt_جڳهه, VTD_PTE_سائيز_بٽ);
    ct_جڳهه->cap = cap;

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    لفظ_t       invLabel,
    لفظ_t       ڊيگهائي,
    cte_t       *جڳهه,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    لفظ_t   io_پتو;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_جڳهه;
    vtd_pte_t *vtd_pte;

    إذا (invLabel == X86_IOPage_جدول_Unmap) {
        إذا (!isFinalCapability(جڳهه)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationUnmap(cap, جڳهه);
    }

    إذا (invLabel != X86_IOPage_جدول_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || ڊيگهائي < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتو   = getSyscallArg(0, بفر) & ~نقاب(VTD_PT_اشاريو_بٽ + se_L4_صفحو_بٽ);

    إذا (cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    إذا (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(VTD_PTE_PTR(cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_جڳهه = lookup_vtd_context_جڳهه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_جڳهه)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTLevel(cap, 0);
        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIOASID(cap, pci_request_id);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_IOPTInvocation_Map_Context_پاڙ(cap, جڳهه, vtd_cte, vtd_context_جڳهه);
    } وإلا {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_جڳهه));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_پتو);

        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جڳهه) != 0) {
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTMapped_پتو(cap, io_پتو);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationMapPT(cap, جڳهه, iopte, lu_ret.iopt_جڳهه);
    }
}

ساكن exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_جڳهه, vtd_pte_t iopte, vtd_pte_t *iopt_جڳهه)
{
    ct_جڳهه->cap = cap;
    *iopt_جڳهه = iopte;
    flushCacheRange(iopt_جڳهه, VTD_PTE_سائيز_بٽ);

    أرجع EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    لفظ_t       ڊيگهائي,
    cte_t       *جڳهه,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    لفظ_t     io_پتو;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_جڳهه;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t چوکٺو_cap_rights;
    seL4_CapRights_t dma_cap_rights_نقاب;

    إذا (current_extra_caps.excaprefs[0] == عدم || ڊيگهائي < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکٺو_cap_get_cap_FSize(cap) != X86_Small_صفحو) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتو  = getSyscallArg(1, بفر) & ~نقاب(صفحو_بٽ);
    paddr       = pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    إذا (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_جڳهه = lookup_vtd_context_جڳهه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_جڳهه)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_جڳهه));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتو);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جڳهه) != 0) {
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));
    چوکٺو_cap_rights    = cap_چوکٺو_cap_get_cap_FVMRights(cap);

    bool_t اكتب = seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب) && (چوکٺو_cap_rights == VMReadWrite);
    bool_t اقرأ = seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) && (چوکٺو_cap_rights != VMKernelOnly);
    إذا (اكتب || اقرأ) {
        iopte = vtd_pte_new(paddr, !!اكتب, !!اقرأ);
    } وإلا {
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_چوکٺو_cap_سيٽ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap, io_پتو);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX86IOInvocationMap(cap, جڳهه, iopte, lu_ret.iopt_جڳهه);
}

فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    لفظ_t               io_پتو;
    vtd_cte_t           *vtd_context_جڳهه;
    vtd_pte_t           *vtd_pte;

    إذا (cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحو_جدول_cap_سيٽ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_صفحو_جدول_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_جڳهه = lookup_vtd_context_جڳهه(io_pt_cap);

        إذا (!vtd_cte_ptr_get_present(vtd_context_جڳهه)) {
            أرجع;
        }

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_جڳهه));

        إذا (level == 0) {
            /* if we have been overmapped or something */
            إذا (pptr_منزل_paddr(vtd_pte) != pptr_منزل_paddr((فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                أرجع;
            }
            *vtd_context_جڳهه = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_جڳهه, VTD_CTE_سائيز_بٽ);
        } وإلا {
            io_پتو = cap_io_صفحو_جدول_cap_get_cap_IOPTMapped_پتو(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_سطحون(vtd_pte, io_پتو >> صفحو_بٽ, level - 1, level - 1);

            /* if we have been overmapped or something */
            إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
                أرجع;
            }
            إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جڳهه) != pptr_منزل_paddr((فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                أرجع;
            }
            *lu_ret.iopt_جڳهه = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_جڳهه, VTD_PTE_سائيز_بٽ);
        }
        invalidate_iotlb();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    لفظ_t               io_پتو;
    vtd_cte_t           *vtd_context_جڳهه;
    vtd_pte_t           *vtd_pte;

    io_پتو  = cap_چوکٺو_cap_get_cap_FMapped_پتو(cap);
    vtd_context_جڳهه = lookup_vtd_context_جڳهه(cap);


    إذا (!vtd_cte_ptr_get_present(vtd_context_جڳهه)) {
        أرجع;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_جڳهه));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتو);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        أرجع;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جڳهه) != pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_جڳهه = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_جڳهه, VTD_PTE_سائيز_بٽ);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_جڳهه)
{
    unmapIOPage(ct_جڳهه->cap);

    ct_جڳهه->cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(ct_جڳهه->cap, 0);
    ct_جڳهه->cap = cap_چوکٺو_cap_سيٽ_cap_FMap_Type(ct_جڳهه->cap, X86_MappingNone);
    ct_جڳهه->cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(ct_جڳهه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}

#جيڪڏهن_پورو /* CONFIG_IOMMU */
