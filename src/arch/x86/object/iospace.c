/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_IOMMU

#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <kernel/thread.h>
#شامل <arch/api/invocation.h>
#شامل <arch/object/iospace.h>
#شامل <arch/model/statedata.h>
#شامل <linker.h>
#شامل <plat/machine/intel-vtd.h>


قسمتعریف ساخت lookup_VTDContext_خانہ_ret {
    vtd_cte_t   *cte;
    لفظ_t      اشاریہ;
} lookup_VTDContext_خانہ_ret_t;


BOOT_رمز cap_t master_iospace_cap(خالی)
{
    اگر (x86KSnumDrhu == 0) {
        واپس cap_null_cap_new();
    }

    واپس
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

ساکن vtd_cte_t *lookup_vtd_context_خانہ(cap_t cap)
{
    uint32_t   vtd_جڑ_اشاریہ;
    uint32_t   vtd_context_اشاریہ;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_جڑ_خانہ;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_خانہ;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        توڑو;

    حالت cap_io_صفحہ_جدول_cap:
        pci_request_id = cap_io_صفحہ_جدول_cap_get_cap_IOPTIOASID(cap);
        توڑو;

    حالت cap_چوکھٹا_cap:
        pci_request_id = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
        توڑو;

    طےشدہ:
        fail("Invalid cap type");
    }

    vtd_جڑ_اشاریہ = get_pci_bus(pci_request_id);
    vtd_جڑ_خانہ = x86_KSvtd_جڑ_جدول + vtd_جڑ_اشاریہ;

    vtd_context = (vtd_cte_t *)paddr_مقصد_pptr(vtd_rte_ptr_get_ctp(vtd_جڑ_خانہ));
    vtd_context_اشاریہ = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_خانہ = &vtd_context[vtd_context_اشاریہ];

    واپس vtd_context_خانہ;
}

ساکن lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_سطحیں(vtd_pte_t *iopt, لفظ_t translation,
                                                          لفظ_t سطحیں_مقصد_resolve, لفظ_t سطحیں_بقایا)
{
    lookupIOPTSlot_ret_t ret;

    لفظ_t      iopt_اشاریہ = 0;
    vtd_pte_t   *iopt_خانہ_2 = 0;
    vtd_pte_t   *اگلا_iopt_خانہ = 0;

    اگر (iopt == 0) {
        ret.iopt_خانہ = 0;
        ret.level = سطحیں_بقایا;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    iopt_اشاریہ = (translation  >> (VTD_PT_اشاریہ_بٹ * (x86KSnumIOPTLevels - 1 - (سطحیں_مقصد_resolve - سطحیں_بقایا)))) &
                 نقاب(VTD_PT_اشاریہ_بٹ);
    iopt_خانہ_2 = iopt + iopt_اشاریہ;

    اگر (!vtd_pte_ptr_get_write(iopt_خانہ_2) || سطحیں_بقایا == 0) {
        ret.iopt_خانہ = iopt_خانہ_2;
        ret.level = سطحیں_بقایا;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    }
    اگلا_iopt_خانہ = (vtd_pte_t *)paddr_مقصد_pptr(vtd_pte_ptr_get_addr(iopt_خانہ_2));
    واپس lookup_IOPTSlot_resolve_سطحیں(اگلا_iopt_خانہ, translation, سطحیں_مقصد_resolve, سطحیں_بقایا - 1);
}


ساکن اندرونی lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, لفظ_t io_پتہ)
{
    lookupIOPTSlot_ret_t ret;

    اگر (iopt == 0) {
        ret.iopt_خانہ    = 0;
        ret.level       = 0;
        ret.حالت_2      = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    } ورنہ {
        واپس lookup_IOPTSlot_resolve_سطحیں(iopt, io_پتہ >> صفحہ_بٹ,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

خالی unmap_VTDContext_اندراج(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_خانہ(cap);
    أكد(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_سائز_بٹ);
    invalidate_iotlb();
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس;
}

ساکن exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_خانہ)
{
    delete_IOPage_جدول(cap);
    cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 0);
    ct_خانہ->cap = cap;

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_X86_IOPTInvocation_Map_Context_جڑ(cap_t cap, cte_t *ct_خانہ, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_خانہ)
{
    *vtd_context_خانہ = vtd_cte;
    flushCacheRange(vtd_context_خانہ, VTD_CTE_سائز_بٹ);
    ct_خانہ->cap = cap;

    واپس EXCEPTION_NONE;
}

ساکن exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_خانہ, vtd_pte_t iopte, vtd_pte_t *iopt_خانہ)
{
    *iopt_خانہ = iopte;
    flushCacheRange(iopt_خانہ, VTD_PTE_سائز_بٹ);
    ct_خانہ->cap = cap;

    واپس EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    لفظ_t       invLabel,
    لفظ_t       لمبائی,
    cte_t       *خانہ,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    لفظ_t   io_پتہ;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_خانہ;
    vtd_pte_t *vtd_pte;

    اگر (invLabel == X86_IOPage_جدول_Unmap) {
        اگر (!isFinalCapability(خانہ)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performX86IOPTInvocationUnmap(cap, خانہ);
    }

    اگر (invLabel != X86_IOPage_جدول_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (current_extra_caps.excaprefs[0] == تہی || لمبائی < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتہ   = getSyscallArg(0, بفر) & ~نقاب(VTD_PT_اشاریہ_بٹ + se_L4_صفحہ_بٹ);

    اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    اگر (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(VTD_PTE_PTR(cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_خانہ = lookup_vtd_context_خانہ(io_space);

    اگر (!vtd_cte_ptr_get_present(vtd_context_خانہ)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTLevel(cap, 0);
        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIOASID(cap, pci_request_id);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_X86_IOPTInvocation_Map_Context_جڑ(cap, خانہ, vtd_cte, vtd_context_خانہ);
    } ورنہ {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_خانہ));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);

        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        اگر (vtd_pte_ptr_get_addr(lu_ret.iopt_خانہ) != 0) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTMapped_پتہ(cap, io_پتہ);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performX86IOPTInvocationMapPT(cap, خانہ, iopte, lu_ret.iopt_خانہ);
    }
}

ساکن exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_خانہ, vtd_pte_t iopte, vtd_pte_t *iopt_خانہ)
{
    ct_خانہ->cap = cap;
    *iopt_خانہ = iopte;
    flushCacheRange(iopt_خانہ, VTD_PTE_سائز_بٹ);

    واپس EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    لفظ_t       لمبائی,
    cte_t       *خانہ,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    لفظ_t     io_پتہ;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_خانہ;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t چوکھٹا_cap_rights;
    seL4_CapRights_t dma_cap_rights_نقاب;

    اگر (current_extra_caps.excaprefs[0] == تہی || لمبائی < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_چوکھٹا_cap_get_cap_FSize(cap) != X86_Small_صفحہ) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتہ  = getSyscallArg(1, بفر) & ~نقاب(صفحہ_بٹ);
    paddr       = pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

    اگر (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    اگر (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_خانہ = lookup_vtd_context_خانہ(io_space);

    اگر (!vtd_cte_ptr_get_present(vtd_context_خانہ)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_خانہ));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (vtd_pte_ptr_get_addr(lu_ret.iopt_خانہ) != 0) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));
    چوکھٹا_cap_rights    = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

    bool_t اكتب = seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب) && (چوکھٹا_cap_rights == VMReadWrite);
    bool_t اقرأ = seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) && (چوکھٹا_cap_rights != VMKernelOnly);
    اگر (اكتب || اقرأ) {
        iopte = vtd_pte_new(paddr, !!اكتب, !!اقرأ);
    } ورنہ {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, io_پتہ);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس performX86IOInvocationMap(cap, خانہ, iopte, lu_ret.iopt_خانہ);
}

خالی delete_IOPage_جدول(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    لفظ_t               io_پتہ;
    vtd_cte_t           *vtd_context_خانہ;
    vtd_pte_t           *vtd_pte;

    اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_صفحہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_صفحہ_جدول_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_خانہ = lookup_vtd_context_خانہ(io_pt_cap);

        اگر (!vtd_cte_ptr_get_present(vtd_context_خانہ)) {
            واپس;
        }

        vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_خانہ));

        اگر (level == 0) {
            /* if we have been overmapped or something */
            اگر (pptr_مقصد_paddr(vtd_pte) != pptr_مقصد_paddr((خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                واپس;
            }
            *vtd_context_خانہ = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_خانہ, VTD_CTE_سائز_بٹ);
        } ورنہ {
            io_پتہ = cap_io_صفحہ_جدول_cap_get_cap_IOPTMapped_پتہ(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_سطحیں(vtd_pte, io_پتہ >> صفحہ_بٹ, level - 1, level - 1);

            /* if we have been overmapped or something */
            اگر (lu_ret.حالت_2 != EXCEPTION_NONE || lu_ret.level != 0) {
                واپس;
            }
            اگر (vtd_pte_ptr_get_addr(lu_ret.iopt_خانہ) != pptr_مقصد_paddr((خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                واپس;
            }
            *lu_ret.iopt_خانہ = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_خانہ, VTD_PTE_سائز_بٹ);
        }
        invalidate_iotlb();
    }
}

خالی unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    لفظ_t               io_پتہ;
    vtd_cte_t           *vtd_context_خانہ;
    vtd_pte_t           *vtd_pte;

    io_پتہ  = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
    vtd_context_خانہ = lookup_vtd_context_خانہ(cap);


    اگر (!vtd_cte_ptr_get_present(vtd_context_خانہ)) {
        واپس;
    }

    vtd_pte = (vtd_pte_t *)paddr_مقصد_pptr(vtd_cte_ptr_get_asr(vtd_context_خانہ));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        واپس;
    }

    اگر (vtd_pte_ptr_get_addr(lu_ret.iopt_خانہ) != pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap))) {
        واپس;
    }

    *lu_ret.iopt_خانہ = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_خانہ, VTD_PTE_سائز_بٹ);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_خانہ)
{
    unmapIOPage(ct_خانہ->cap);

    ct_خانہ->cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(ct_خانہ->cap, 0);
    ct_خانہ->cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(ct_خانہ->cap, X86_MappingNone);
    ct_خانہ->cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(ct_خانہ->cap, asidInvalid);

    واپس EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    واپس EXCEPTION_SYSCALL_ERROR;
}

#اگر_ختم /* CONFIG_IOMMU */
