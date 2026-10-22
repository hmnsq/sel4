/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_IOMMU

#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/thread.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/model/statedata.h>
#ضمن <linker.h>
#ضمن <plat/machine/intel-vtd.h>


تعريفنوع بنية lookup_VTDContext_تھاں_ret {
    vtd_cte_t   *cte;
    لفظ_t      اشاریہ;
} lookup_VTDContext_تھاں_ret_t;


BOOT_کوڈ cap_t master_iospace_cap(فراغ)
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

ساكن vtd_cte_t *lookup_vtd_context_تھاں(cap_t cap)
{
    uint32_t   vtd_جڑ_اشاریہ;
    uint32_t   vtd_context_اشاریہ;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_جڑ_تھاں;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_تھاں;

    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        اكسر;

    حالة cap_io_ورقہ_جدول_cap:
        pci_request_id = cap_io_ورقہ_جدول_cap_get_cap_IOPTIOASID(cap);
        اكسر;

    حالة cap_چوکھٹا_cap:
        pci_request_id = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
        اكسر;

    افتراضي:
        fail("Invalid cap type");
    }

    vtd_جڑ_اشاریہ = get_pci_bus(pci_request_id);
    vtd_جڑ_تھاں = x86_KSvtd_جڑ_جدول + vtd_جڑ_اشاریہ;

    vtd_context = (vtd_cte_t *)paddr_منزل_pptr(vtd_rte_ptr_get_ctp(vtd_جڑ_تھاں));
    vtd_context_اشاریہ = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_تھاں = &vtd_context[vtd_context_اشاریہ];

    أرجع vtd_context_تھاں;
}

ساكن lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_پدھراں(vtd_pte_t *iopt, لفظ_t translation,
                                                          لفظ_t پدھراں_منزل_resolve, لفظ_t پدھراں_بچیا)
{
    lookupIOPTSlot_ret_t ret;

    لفظ_t      iopt_اشاریہ = 0;
    vtd_pte_t   *iopt_تھاں_2 = 0;
    vtd_pte_t   *اگلا_iopt_تھاں = 0;

    إذا (iopt == 0) {
        ret.iopt_تھاں = 0;
        ret.level = پدھراں_بچیا;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    iopt_اشاریہ = (translation  >> (VTD_PT_اشاریہ_بٹ * (x86KSnumIOPTLevels - 1 - (پدھراں_منزل_resolve - پدھراں_بچیا)))) &
                 نقاب(VTD_PT_اشاریہ_بٹ);
    iopt_تھاں_2 = iopt + iopt_اشاریہ;

    إذا (!vtd_pte_ptr_get_write(iopt_تھاں_2) || پدھراں_بچیا == 0) {
        ret.iopt_تھاں = iopt_تھاں_2;
        ret.level = پدھراں_بچیا;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
    اگلا_iopt_تھاں = (vtd_pte_t *)paddr_منزل_pptr(vtd_pte_ptr_get_addr(iopt_تھاں_2));
    أرجع lookup_IOPTSlot_resolve_پدھراں(اگلا_iopt_تھاں, translation, پدھراں_منزل_resolve, پدھراں_بچیا - 1);
}


ساكن مضمن lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, لفظ_t io_پتہ)
{
    lookupIOPTSlot_ret_t ret;

    إذا (iopt == 0) {
        ret.iopt_تھاں    = 0;
        ret.level       = 0;
        ret.حالت      = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        أرجع lookup_IOPTSlot_resolve_پدھراں(iopt, io_پتہ >> ورقہ_بٹ,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

فراغ unmap_VTDContext_اندراج(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_تھاں(cap);
    أكد(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_ناپ_بٹ);
    invalidate_iotlb();
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع;
}

ساكن exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_تھاں)
{
    delete_IOPage_جدول(cap);
    cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 0);
    ct_تھاں->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_IOPTInvocation_Map_Context_جڑ(cap_t cap, cte_t *ct_تھاں, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_تھاں)
{
    *vtd_context_تھاں = vtd_cte;
    flushCacheRange(vtd_context_تھاں, VTD_CTE_ناپ_بٹ);
    ct_تھاں->cap = cap;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_تھاں, vtd_pte_t iopte, vtd_pte_t *iopt_تھاں)
{
    *iopt_تھاں = iopte;
    flushCacheRange(iopt_تھاں, VTD_PTE_ناپ_بٹ);
    ct_تھاں->cap = cap;

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    لفظ_t       invLabel,
    لفظ_t       لمائی,
    cte_t       *تھاں,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    لفظ_t   io_پتہ;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_تھاں;
    vtd_pte_t *vtd_pte;

    إذا (invLabel == X86_IOPage_جدول_Unmap) {
        إذا (!isFinalCapability(تھاں)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationUnmap(cap, تھاں);
    }

    إذا (invLabel != X86_IOPage_جدول_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (current_extra_caps.excaprefs[0] == عدم || لمائی < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_پتہ   = getSyscallArg(0, بفر) & ~نقاب(VTD_PT_اشاریہ_بٹ + se_L4_ورقہ_بٹ);

    إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    إذا (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(VTD_PTE_PTR(cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_تھاں = lookup_vtd_context_تھاں(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_تھاں)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTLevel(cap, 0);
        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIOASID(cap, pci_request_id);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_IOPTInvocation_Map_Context_جڑ(cap, تھاں, vtd_cte, vtd_context_تھاں);
    } وإلا {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_تھاں));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);

        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_تھاں) != 0) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTMapped_پتہ(cap, io_پتہ);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performX86IOPTInvocationMapPT(cap, تھاں, iopte, lu_ret.iopt_تھاں);
    }
}

ساكن exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_تھاں, vtd_pte_t iopte, vtd_pte_t *iopt_تھاں)
{
    ct_تھاں->cap = cap;
    *iopt_تھاں = iopte;
    flushCacheRange(iopt_تھاں, VTD_PTE_ناپ_بٹ);

    أرجع EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    لفظ_t       لمائی,
    cte_t       *تھاں,
    cap_t        cap,
    لفظ_t      *بفر
)
{
    cap_t      io_space;
    لفظ_t     io_پتہ;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_تھاں;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t چوکھٹا_cap_rights;
    seL4_CapRights_t dma_cap_rights_نقاب;

    إذا (current_extra_caps.excaprefs[0] == عدم || لمائی < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکھٹا_cap_get_cap_FSize(cap) != X86_Small_ورقہ) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_پتہ  = getSyscallArg(1, بفر) & ~نقاب(ورقہ_بٹ);
    paddr       = pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

    إذا (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    إذا (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_تھاں = lookup_vtd_context_تھاں(io_space);

    إذا (!vtd_cte_ptr_get_present(vtd_context_تھاں)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_تھاں));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_تھاں) != 0) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_نقاب = rights_From_لفظ(getSyscallArg(0, بفر));
    چوکھٹا_cap_rights    = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

    bool_t اكتب = seL4_CapRights_get_capAllowWrite(dma_cap_rights_نقاب) && (چوکھٹا_cap_rights == VMReadWrite);
    bool_t اقرأ = seL4_CapRights_get_capAllowRead(dma_cap_rights_نقاب) && (چوکھٹا_cap_rights != VMKernelOnly);
    إذا (اكتب || اقرأ) {
        iopte = vtd_pte_new(paddr, !!اكتب, !!اقرأ);
    } وإلا {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, io_پتہ);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX86IOInvocationMap(cap, تھاں, iopte, lu_ret.iopt_تھاں);
}

فراغ delete_IOPage_جدول(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    لفظ_t               io_پتہ;
    vtd_cte_t           *vtd_context_تھاں;
    vtd_pte_t           *vtd_pte;

    إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ورقہ_جدول_cap_سیٹ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_ورقہ_جدول_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_تھاں = lookup_vtd_context_تھاں(io_pt_cap);

        إذا (!vtd_cte_ptr_get_present(vtd_context_تھاں)) {
            أرجع;
        }

        vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_تھاں));

        إذا (level == 0) {
            /* if we have been overmapped or something */
            إذا (pptr_منزل_paddr(vtd_pte) != pptr_منزل_paddr((فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                أرجع;
            }
            *vtd_context_تھاں = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_تھاں, VTD_CTE_ناپ_بٹ);
        } وإلا {
            io_پتہ = cap_io_ورقہ_جدول_cap_get_cap_IOPTMapped_پتہ(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_پدھراں(vtd_pte, io_پتہ >> ورقہ_بٹ, level - 1, level - 1);

            /* if we have been overmapped or something */
            إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
                أرجع;
            }
            إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_تھاں) != pptr_منزل_paddr((فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                أرجع;
            }
            *lu_ret.iopt_تھاں = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_تھاں, VTD_PTE_ناپ_بٹ);
        }
        invalidate_iotlb();
    }
}

فراغ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    لفظ_t               io_پتہ;
    vtd_cte_t           *vtd_context_تھاں;
    vtd_pte_t           *vtd_pte;

    io_پتہ  = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
    vtd_context_تھاں = lookup_vtd_context_تھاں(cap);


    إذا (!vtd_cte_ptr_get_present(vtd_context_تھاں)) {
        أرجع;
    }

    vtd_pte = (vtd_pte_t *)paddr_منزل_pptr(vtd_cte_ptr_get_asr(vtd_context_تھاں));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_پتہ);
    إذا (lu_ret.حالت != EXCEPTION_NONE || lu_ret.level != 0) {
        أرجع;
    }

    إذا (vtd_pte_ptr_get_addr(lu_ret.iopt_تھاں) != pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap))) {
        أرجع;
    }

    *lu_ret.iopt_تھاں = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_تھاں, VTD_PTE_ناپ_بٹ);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_تھاں)
{
    unmapIOPage(ct_تھاں->cap);

    ct_تھاں->cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(ct_تھاں->cap, 0);
    ct_تھاں->cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(ct_تھاں->cap, X86_MappingNone);
    ct_تھاں->cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(ct_تھاں->cap, asidInvalid);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(لفظ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    أرجع EXCEPTION_SYSCALL_ERROR;
}

#جے_انت /* CONFIG_IOMMU */
