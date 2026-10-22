/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_IOMMU

#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>


प्रकारनिर्देश संरचना lookupVTDContextSlot_ret {
    vtd_cte_t   *cte;
    word_t      सूचकांक;
} lookupVTDContextSlot_ret_t;


BOOT_संहिता cap_t master_iospace_cap(शून्य)
{
    यदि (x86KSnumDrhu == 0) {
        प्रत्यागम cap_null_cap_new();
    }

    प्रत्यागम
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

स्थिर vtd_cte_t *lookup_vtd_context_slot(cap_t cap)
{
    uint32_t   vtd_मूलम्_सूचकांक;
    uint32_t   vtd_context_सूचकांक;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_मूलम्_slot;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_slot;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        भङ्ग;

    पक्ष cap_io_page_सारणी_cap:
        pci_request_id = cap_io_page_सारणी_cap_get_cap_IOPTIOASID(cap);
        भङ्ग;

    पक्ष cap_frame_cap:
        pci_request_id = cap_frame_cap_get_capFMappedASID(cap);
        भङ्ग;

    स्वभाव:
        fail("Invalid cap type");
    }

    vtd_मूलम्_सूचकांक = get_pci_bus(pci_request_id);
    vtd_मूलम्_slot = x86_KSvtd_मूलम्_सारणी + vtd_मूलम्_सूचकांक;

    vtd_context = (vtd_cte_t *)paddr_गन्तव्यम्_pptr(vtd_rte_ptr_get_ctp(vtd_मूलम्_slot));
    vtd_context_सूचकांक = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_slot = &vtd_context[vtd_context_सूचकांक];

    प्रत्यागम vtd_context_slot;
}

स्थिर lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_स्तराः(vtd_pte_t *iopt, word_t translation,
                                                          word_t स्तराः_गन्तव्यम्_resolve, word_t स्तराः_remaining)
{
    lookupIOPTSlot_ret_t ret;

    word_t      iopt_सूचकांक = 0;
    vtd_pte_t   *iopt_slot = 0;
    vtd_pte_t   *अग्र_iopt_slot = 0;

    यदि (iopt == 0) {
        ret.ioptSlot = 0;
        ret.level = स्तराः_remaining;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    iopt_सूचकांक = (translation  >> (VTD_PT_सूचकांक_द्वयङ्काः * (x86KSnumIOPTLevels - 1 - (स्तराः_गन्तव्यम्_resolve - स्तराः_remaining)))) &
                 आवरणम्(VTD_PT_सूचकांक_द्वयङ्काः);
    iopt_slot = iopt + iopt_सूचकांक;

    यदि (!vtd_pte_ptr_get_write(iopt_slot) || स्तराः_remaining == 0) {
        ret.ioptSlot = iopt_slot;
        ret.level = स्तराः_remaining;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    }
    अग्र_iopt_slot = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_pte_ptr_get_addr(iopt_slot));
    प्रत्यागम lookup_IOPTSlot_resolve_स्तराः(अग्र_iopt_slot, translation, स्तराः_गन्तव्यम्_resolve, स्तराः_remaining - 1);
}


स्थिर अन्तःस्थ lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, word_t io_स्थानाङ्कः)
{
    lookupIOPTSlot_ret_t ret;

    यदि (iopt == 0) {
        ret.ioptSlot    = 0;
        ret.level       = 0;
        ret.स्थितिः      = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    } अन्यथा {
        प्रत्यागम lookup_IOPTSlot_resolve_स्तराः(iopt, io_स्थानाङ्कः >> PAGE_द्वयङ्काः,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

शून्य unmap_VTDContext_प्रविष्टिः(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_slot(cap);
    प्रतिज्ञा(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_आकार_द्वयङ्काः);
    invalidate_iotlb();
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम;
}

स्थिर exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    delete_IOPage_सारणी(cap);
    cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(cap, 0);
    ctSlot->cap = cap;

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_X86_IOPTInvocation_Map_Context_मूलम्(cap_t cap, cte_t *ctSlot, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_slot)
{
    *vtd_context_slot = vtd_cte;
    flushCacheRange(vtd_context_slot, VTD_CTE_आकार_द्वयङ्काः);
    ctSlot->cap = cap;

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_आकार_द्वयङ्काः);
    ctSlot->cap = cap;

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    word_t       invLabel,
    word_t       दीर्घता,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    word_t   io_स्थानाङ्कः;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_slot;
    vtd_pte_t *vtd_pte;

    यदि (invLabel == X86_IOPage_सारणी_Unmap) {
        यदि (!isFinalCapability(slot)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performX86IOPTInvocationUnmap(cap, slot);
    }

    यदि (invLabel != X86_IOPage_सारणी_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (current_extra_caps.excaprefs[0] == रिक्त || दीर्घता < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_स्थानाङ्कः   = getSyscallArg(0, बफर) & ~आवरणम्(VTD_PT_सूचकांक_द्वयङ्काः + se_L4_Page_द्वयङ्काः);

    यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    यदि (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr(VTD_PTE_PTR(cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_slot = lookup_vtd_context_slot(io_space);

    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTLevel(cap, 0);
        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIOASID(cap, pci_request_id);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_X86_IOPTInvocation_Map_Context_मूलम्(cap, slot, vtd_cte, vtd_context_slot);
    } अन्यथा {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_स्थानाङ्कः);

        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTMapped_स्थानाङ्कः(cap, io_स्थानाङ्कः);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performX86IOPTInvocationMapPT(cap, slot, iopte, lu_ret.ioptSlot);
    }
}

स्थिर exception_t performX86IOInvocationMap(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    ctSlot->cap = cap;
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_आकार_द्वयङ्काः);

    प्रत्यागम EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    word_t       दीर्घता,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    word_t     io_स्थानाङ्कः;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_slot;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t frame_cap_rights;
    seL4_CapRights_t dma_cap_rights_आवरणम्;

    यदि (current_extra_caps.excaprefs[0] == रिक्त || दीर्घता < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_frame_cap_get_capFSize(cap) != X86_SmallPage) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_स्थानाङ्कः  = getSyscallArg(1, बफर) & ~आवरणम्(PAGE_द्वयङ्काः);
    paddr       = pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap));

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    यदि (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_slot = lookup_vtd_context_slot(io_space);

    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_स्थानाङ्कः);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_आवरणम् = rightsFromWord(getSyscallArg(0, बफर));
    frame_cap_rights    = cap_frame_cap_get_capFVMRights(cap);

    bool_t लिख = seL4_CapRights_get_capAllowWrite(dma_cap_rights_आवरणम्) && (frame_cap_rights == VMReadWrite);
    bool_t पठ = seL4_CapRights_get_capAllowRead(dma_cap_rights_आवरणम्) && (frame_cap_rights != VMKernelOnly);
    यदि (लिख || पठ) {
        iopte = vtd_pte_new(paddr, !!लिख, !!पठ);
    } अन्यथा {
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_frame_cap_समुच्चयः_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, io_स्थानाङ्कः);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performX86IOInvocationMap(cap, slot, iopte, lu_ret.ioptSlot);
}

शून्य delete_IOPage_सारणी(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    word_t               io_स्थानाङ्कः;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_page_सारणी_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_slot = lookup_vtd_context_slot(io_pt_cap);

        यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
            प्रत्यागम;
        }

        vtd_pte = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

        यदि (level == 0) {
            /* if we have been overmapped or something */
            यदि (pptr_गन्तव्यम्_paddr(vtd_pte) != pptr_गन्तव्यम्_paddr((शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                प्रत्यागम;
            }
            *vtd_context_slot = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_slot, VTD_CTE_आकार_द्वयङ्काः);
        } अन्यथा {
            io_स्थानाङ्कः = cap_io_page_सारणी_cap_get_cap_IOPTMapped_स्थानाङ्कः(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_स्तराः(vtd_pte, io_स्थानाङ्कः >> PAGE_द्वयङ्काः, level - 1, level - 1);

            /* if we have been overmapped or something */
            यदि (lu_ret.स्थितिः != EXCEPTION_NONE || lu_ret.level != 0) {
                प्रत्यागम;
            }
            यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_गन्तव्यम्_paddr((शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                प्रत्यागम;
            }
            *lu_ret.ioptSlot = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.ioptSlot, VTD_PTE_आकार_द्वयङ्काः);
        }
        invalidate_iotlb();
    }
}

शून्य unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    word_t               io_स्थानाङ्कः;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    io_स्थानाङ्कः  = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
    vtd_context_slot = lookup_vtd_context_slot(cap);


    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        प्रत्यागम;
    }

    vtd_pte = (vtd_pte_t *)paddr_गन्तव्यम्_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_स्थानाङ्कः);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE || lu_ret.level != 0) {
        प्रत्यागम;
    }

    यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap))) {
        प्रत्यागम;
    }

    *lu_ret.ioptSlot = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.ioptSlot, VTD_PTE_आकार_द्वयङ्काः);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ctSlot)
{
    unmapIOPage(ctSlot->cap);

    ctSlot->cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(ctSlot->cap, 0);
    ctSlot->cap = cap_frame_cap_समुच्चयः_cap_FMap_Type(ctSlot->cap, X86_MappingNone);
    ctSlot->cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(ctSlot->cap, asidInvalid);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    प्रत्यागम EXCEPTION_SYSCALL_ERROR;
}

#यदि_अन्तः /* CONFIG_IOMMU */
