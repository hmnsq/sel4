/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_IOMMU

#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/thread.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/intel-vtd.h>


تعريفنوع بنية lookup_VTDContext_جایگاه_ret {
    vtd_cte_t   *cte;
    کلمه_t      شاخص;
} lookup_VTDContext_جایگاه_ret_t;


BOOT_کد cap_t master_iospace_cap(فراغ)
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

ساكن vtd_cte_t *lookup_vtd_context_جایگاه(cap_t cap)
{
    uint32_t   vtd_ریشه_شاخص;
    uint32_t   vtd_context_شاخص;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_ریشه_جایگاه;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_جایگاه;

    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        اكسر;

    حالة cap_io_صفحه_جدول_cap:
        pci_request_id = cap_io_صفحه_جدول_cap_get_cap_IOPTIOASID(cap);
        اكسر;

    حالة cap_قاب_cap:
        pci_request_id = cap_قاب_cap_get_cap_FMapped_ASID(cap);
        اكسر;

    افتراضي:
        fail("Invalid cap type");
    }

    vtd_ریشه_شاخص = get_pci_bus(pci_request_id);
    vtd_ریشه_جایگاه = x86_KSvtd_ریشه_جدول + vtd_ریشه_شاخص;

    vtd_context = (vtd_cte_t *)paddr_مقصد_pptr(vtd_rte_ptr_get_ctp(vtd_ریشه_جایگاه));
    vtd_context_شاخص = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_جایگاه = &vtd_context[vtd_context_شاخص];

    أرجع vtd_context_جایگاه;
}

ساكن lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_سطوح(vtd_pte_t *iopt, کلمه_t translation,
                                                          کلمه_t سطوح_مقصد_resolve, کلمه_t سطوح_مانده)
{
    lookupIOPTSlot_ret_t ret;

    کلمه_t      iopt_شاخص = 0;
    vtd_pte_t   *iopt_جایگاه_2 = 0;
    vtd_pte_t   *بعدی_iopt_جایگاه = 0;

    إذا (iopt == 0) {
        ret.iopt_جایگاه = 0;
        ret.level = سطوح_مانده;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    iopt_شاخص = (translation  >> (VTD_PT_شاخص_بیتها * (x86KSnumIOPTLevels - 1 - (سطوح_مقصد_resolve - سطوح_مانده)))) &
                 ماسک(VTD_PT_شاخص_بیتها);
    iopt_جایگاه_2 = iopt + iopt_شاخص;

    إذا (!vtd_pte_ptr_get_write(iopt_جایگاه_2) || سطوح_مانده == 0) {
        ret.iopt_جایگاه = iopt_جایگاه_2;
        ret.level = سطوح_مانده;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    }
    بعدی_iopt_جایگاه = (vtd_pte_t *)paddr_مقصد_pptr(vtd_pte_ptr_get_addr(iopt_جایگاه_2));
    أرجع lookup_IOPTSlot_resolve_سطوح(بعدی_iopt_جایگاه, translation, سطوح_مقصد_resolve, سطوح_مانده - 1);
}


ساكن مضمن lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, کلمه_t io_آدرس)
{
    lookupIOPTSlot_ret_t ret;

    إذا (iopt == 0) {
        ret.iopt_جایگاه    = 0;
        ret.level       = 0;
        ret.وضعیت      = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        أرجع lookup_IOPTSlot_resolve_سطوح(iopt, io_آدرس >> صفحه_بیتها,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

فراغ unmap_VTDContext_مدخل(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_جایگاه(cap);
    أكد(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_اندازه_بیتها);
    invalidate_iotlb();
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع;
}

ساكن exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_جایگاه)
{
    delete_IOPage_جدول(cap);
    cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(cap, 0);
    ct_جایگاه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_IOPTInvocation_Map_Context_ریشه(cap_t cap, cte_t *ct_جایگاه, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_جایگاه)
{
    *vtd_context_جایگاه = vtd_cte;
    flushCacheRange(vtd_context_جایگاه, VTD_CTE_اندازه_بیتها);
    ct_جایگاه->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_جایگاه, vtd_pte_t iopte, vtd_pte_t *iopt_جایگاه)
{
    *iopt_جایگاه = iopte;
    flushCacheRange(iopt_جایگاه, VTD_PTE_اندازه_بیتها);
    ct_جایگاه->cap = cap;

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    کلمه_t       invLabel,
    کلمه_t       درازی,
    cte_t       *جایگاه,
    cap_t        cap,
    کلمه_t      *میانگیر
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    کلمه_t   io_آدرس;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_جایگاه;
    vtd_pte_t *vtd_pte;

    إذا (invLabel == X86_IOPage_جدول_Unmap) {
        إذا (!isFinalCapability(جایگاه)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationUnmap(cap, جایگاه);
    }

    إذا (invLabel != X86_IOPage_جدول_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || درازی < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_آدرس   = getSyscallArg(0, میانگیر) & ~ماسک(VTD_PT_شاخص_بیتها + se_L4_صفحه_بیتها);

    إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
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

    paddr = pptr_مقصد_paddr(VTD_PTE_PTR(cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_جایگاه = lookup_vtd_context_جایگاه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_جایگاه)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTLevel(cap, 0);
        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIOASID(cap, pci_request_id);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_IOPTInvocation_Map_Context_ریشه(cap, جایگاه, vtd_cte, vtd_context_جایگاه);
    } وإلا {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_جایگاه));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_آدرس);

        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جایگاه) != 0) {
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTMapped_آدرس(cap, io_آدرس);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationMapPT(cap, جایگاه, iopte, lu_ret.iopt_جایگاه);
    }
}

ساكن exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_جایگاه, vtd_pte_t iopte, vtd_pte_t *iopt_جایگاه)
{
    ct_جایگاه->cap = cap;
    *iopt_جایگاه = iopte;
    flushCacheRange(iopt_جایگاه, VTD_PTE_اندازه_بیتها);

    أرجع EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    کلمه_t       درازی,
    cte_t       *جایگاه,
    cap_t        cap,
    کلمه_t      *میانگیر
)
{
    cap_t      io_space;
    کلمه_t     io_آدرس;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_جایگاه;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t قاب_cap_rights;
    seL4_CapRights_t dma_cap_rights_ماسک;

    إذا (current_extra_caps.excaprefs[0] == عدم || درازی < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_قاب_cap_get_cap_FSize(cap) != X86_Small_صفحه) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_آدرس  = getSyscallArg(1, میانگیر) & ~ماسک(صفحه_بیتها);
    paddr       = pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap));

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

    vtd_context_جایگاه = lookup_vtd_context_جایگاه(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_جایگاه)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_جایگاه));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_آدرس);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جایگاه) != 0) {
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_ماسک = rights_From_کلمه(getSyscallArg(0, میانگیر));
    قاب_cap_rights    = cap_قاب_cap_get_cap_FVMRights(cap);

    bool_t اكتب = seL4_CapRights_get_capAllowWrite(dma_cap_rights_ماسک) && (قاب_cap_rights == VMReadWrite);
    bool_t اقرأ = seL4_CapRights_get_capAllowRead(dma_cap_rights_ماسک) && (قاب_cap_rights != VMKernelOnly);
    إذا (اكتب || اقرأ) {
        iopte = vtd_pte_new(paddr, !!اكتب, !!اقرأ);
    } وإلا {
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_قاب_cap_مجموعه_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, io_آدرس);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX86IOInvocationMap(cap, جایگاه, iopte, lu_ret.iopt_جایگاه);
}

فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    کلمه_t               io_آدرس;
    vtd_cte_t           *vtd_context_جایگاه;
    vtd_pte_t           *vtd_pte;

    إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحه_جدول_cap_مجموعه_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_صفحه_جدول_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_جایگاه = lookup_vtd_context_جایگاه(io_pt_cap);

        إذا (!vtd_cte_ptr_get_present(vtd_context_جایگاه)) {
            أرجع;
        }

        vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_جایگاه));

        إذا (level == 0) {
            /* if we have been overmapped or something */
            إذا (pptr_مقصد_paddr(vtd_pte) != pptr_مقصد_paddr((فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                أرجع;
            }
            *vtd_context_جایگاه = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_جایگاه, VTD_CTE_اندازه_بیتها);
        } وإلا {
            io_آدرس = cap_io_صفحه_جدول_cap_get_cap_IOPTMapped_آدرس(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_سطوح(vtd_pte, io_آدرس >> صفحه_بیتها, level - 1, level - 1);

            /* if we have been overmapped or something */
            إذا (lu_ret.وضعیت != EXCEPTION_NONE || lu_ret.level != 0) {
                أرجع;
            }
            إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جایگاه) != pptr_مقصد_paddr((فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                أرجع;
            }
            *lu_ret.iopt_جایگاه = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_جایگاه, VTD_PTE_اندازه_بیتها);
        }
        invalidate_iotlb();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    کلمه_t               io_آدرس;
    vtd_cte_t           *vtd_context_جایگاه;
    vtd_pte_t           *vtd_pte;

    io_آدرس  = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
    vtd_context_جایگاه = lookup_vtd_context_جایگاه(cap);


    إذا (!vtd_cte_ptr_get_present(vtd_context_جایگاه)) {
        أرجع;
    }

    vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_جایگاه));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_آدرس);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE || lu_ret.level != 0) {
        أرجع;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_جایگاه) != pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_جایگاه = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_جایگاه, VTD_PTE_اندازه_بیتها);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_جایگاه)
{
    unmapIOPage(ct_جایگاه->cap);

    ct_جایگاه->cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(ct_جایگاه->cap, 0);
    ct_جایگاه->cap = cap_قاب_cap_مجموعه_cap_FMap_Type(ct_جایگاه->cap, X86_MappingNone);
    ct_جایگاه->cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(ct_جایگاه->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(کلمه_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}

#پایان_اگر /* CONFIG_IOMMU */
