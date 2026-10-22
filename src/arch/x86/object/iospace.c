/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_IOMMU

#समावेश <api/syscall.h>
#समावेश <machine/io.h>
#समावेश <kernel/thread.h>
#समावेश <arch/api/invocation.h>
#समावेश <arch/object/iospace.h>
#समावेश <arch/model/statedata.h>
#समावेश <linker.h>
#समावेश <plat/machine/intel-vtd.h>


प्रकार काष्ठा lookupVTDContextSlot_ret {
    vtd_cte_t   *cte;
    word_t      सूचकांक;
} lookupVTDContextSlot_ret_t;


BOOT_CODE cap_t master_iospace_cap(व्योम)
{
    अगर (x86KSnumDrhu == 0) {
        वापस cap_null_cap_new();
    }

    वापस
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

अटल vtd_cte_t *lookup_vtd_context_slot(cap_t cap)
{
    uint32_t   vtd_मूल_सूचकांक;
    uint32_t   vtd_context_सूचकांक;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_मूल_slot;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_slot;

    चयन (cap_get_capType(cap)) {
    हाल cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        अवरोध;

    हाल cap_io_page_table_cap:
        pci_request_id = cap_io_page_table_cap_get_capIOPTIOASID(cap);
        अवरोध;

    हाल cap_frame_cap:
        pci_request_id = cap_frame_cap_get_capFMappedASID(cap);
        अवरोध;

    शेष:
        fail("Invalid cap type");
    }

    vtd_मूल_सूचकांक = get_pci_bus(pci_request_id);
    vtd_मूल_slot = x86_KSvtd_मूल_Table + vtd_मूल_सूचकांक;

    vtd_context = (vtd_cte_t *)paddr_to_pptr(vtd_rte_ptr_get_ctp(vtd_मूल_slot));
    vtd_context_सूचकांक = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_slot = &vtd_context[vtd_context_सूचकांक];

    वापस vtd_context_slot;
}

अटल lookupIOPTSlot_ret_t lookupIOPTSlot_resolve_levels(vtd_pte_t *iopt, word_t translation,
                                                          word_t levels_to_resolve, word_t levels_remaining)
{
    lookupIOPTSlot_ret_t ret;

    word_t      iopt_सूचकांक = 0;
    vtd_pte_t   *iopt_slot = 0;
    vtd_pte_t   *अग्र_iopt_slot = 0;

    अगर (iopt == 0) {
        ret.ioptSlot = 0;
        ret.level = levels_remaining;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    iopt_सूचकांक = (translation  >> (VTD_PT_सूचकांक_BITS * (x86KSnumIOPTLevels - 1 - (levels_to_resolve - levels_remaining)))) &
                 MASK(VTD_PT_सूचकांक_BITS);
    iopt_slot = iopt + iopt_सूचकांक;

    अगर (!vtd_pte_ptr_get_write(iopt_slot) || levels_remaining == 0) {
        ret.ioptSlot = iopt_slot;
        ret.level = levels_remaining;
        ret.status = EXCEPTION_NONE;
        वापस ret;
    }
    अग्र_iopt_slot = (vtd_pte_t *)paddr_to_pptr(vtd_pte_ptr_get_addr(iopt_slot));
    वापस lookupIOPTSlot_resolve_levels(अग्र_iopt_slot, translation, levels_to_resolve, levels_remaining - 1);
}


अटल अंतरभूत lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, word_t io_address)
{
    lookupIOPTSlot_ret_t ret;

    अगर (iopt == 0) {
        ret.ioptSlot    = 0;
        ret.level       = 0;
        ret.status      = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    } अन्यथा {
        वापस lookupIOPTSlot_resolve_levels(iopt, io_address >> PAGE_BITS,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

व्योम unmapVTDContextEntry(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_slot(cap);
    निश्चित(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_आकार_BITS);
    invalidate_iotlb();
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस;
}

अटल exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    deleteIOPageTable(cap);
    cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 0);
    ctSlot->cap = cap;

    वापस EXCEPTION_NONE;
}

अटल exception_t perform_X86_IOPTInvocation_Map_Context_मूल(cap_t cap, cte_t *ctSlot, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_slot)
{
    *vtd_context_slot = vtd_cte;
    flushCacheRange(vtd_context_slot, VTD_CTE_आकार_BITS);
    ctSlot->cap = cap;

    वापस EXCEPTION_NONE;
}

अटल exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_आकार_BITS);
    ctSlot->cap = cap;

    वापस EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    word_t       invLabel,
    word_t       लंबाई,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    word_t   io_address;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_slot;
    vtd_pte_t *vtd_pte;

    अगर (invLabel == X86IOPageTableUnmap) {
        अगर (!isFinalCapability(slot)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performX86IOPTInvocationUnmap(cap, slot);
    }

    अगर (invLabel != X86IOPageTableMap) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (current_extra_caps.excaprefs[0] == शून्य || लंबाई < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_address   = getSyscallArg(0, बफर) & ~MASK(VTD_PT_सूचकांक_BITS + seL4_PageBits);

    अगर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    अगर (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr(VTD_PTE_PTR(cap_io_page_table_cap_get_capIOPTBasePtr(cap)));
    vtd_context_slot = lookup_vtd_context_slot(io_space);

    अगर (!vtd_cte_ptr_get_present(vtd_context_slot)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 1);
        cap = cap_io_page_table_cap_set_capIOPTLevel(cap, 0);
        cap = cap_io_page_table_cap_set_capIOPTIOASID(cap, pci_request_id);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस perform_X86_IOPTInvocation_Map_Context_मूल(cap, slot, vtd_cte, vtd_context_slot);
    } अन्यथा {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_address);

        अगर (lu_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        अगर (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 1);
        cap = cap_io_page_table_cap_set_capIOPTLevel(cap, lu_ret.level);
        cap = cap_io_page_table_cap_set_capIOPTIOASID(cap, pci_request_id);
        cap = cap_io_page_table_cap_set_capIOPTMappedAddress(cap, io_address);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performX86IOPTInvocationMapPT(cap, slot, iopte, lu_ret.ioptSlot);
    }
}

अटल exception_t performX86IOInvocationMap(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    ctSlot->cap = cap;
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_आकार_BITS);

    वापस EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    word_t       लंबाई,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    word_t     io_address;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_slot;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t frame_cap_rights;
    seL4_CapRights_t dma_cap_rights_mask;

    अगर (current_extra_caps.excaprefs[0] == शून्य || लंबाई < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_frame_cap_get_capFSize(cap) != X86_SmallPage) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_address  = getSyscallArg(1, बफर) & ~MASK(PAGE_BITS);
    paddr       = pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap));

    अगर (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    अगर (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_slot = lookup_vtd_context_slot(io_space);

    अगर (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_address);
    अगर (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_mask = rightsFromWord(getSyscallArg(0, बफर));
    frame_cap_rights    = cap_frame_cap_get_capFVMRights(cap);

    bool_t ग_लिखो = seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask) && (frame_cap_rights == VMReadWrite);
    bool_t पढ़ो = seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) && (frame_cap_rights != VMKernelOnly);
    अगर (ग_लिखो || पढ़ो) {
        iopte = vtd_pte_new(paddr, !!ग_लिखो, !!पढ़ो);
    } अन्यथा {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_frame_cap_set_capFMapType(cap, X86_MappingIOSpace);
    cap = cap_frame_cap_set_capFMappedASID(cap, pci_request_id);
    cap = cap_frame_cap_set_capFMappedAddress(cap, io_address);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस performX86IOInvocationMap(cap, slot, iopte, lu_ret.ioptSlot);
}

व्योम deleteIOPageTable(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    word_t               io_address;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    अगर (cap_io_page_table_cap_get_capIOPTIsMapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_table_cap_set_capIOPTIsMapped(io_pt_cap, 0);
        level = cap_io_page_table_cap_get_capIOPTLevel(io_pt_cap);
        vtd_context_slot = lookup_vtd_context_slot(io_pt_cap);

        अगर (!vtd_cte_ptr_get_present(vtd_context_slot)) {
            वापस;
        }

        vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

        अगर (level == 0) {
            /* if we have been overmapped or something */
            अगर (pptr_to_paddr(vtd_pte) != pptr_to_paddr((व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(io_pt_cap))) {
                वापस;
            }
            *vtd_context_slot = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_slot, VTD_CTE_आकार_BITS);
        } अन्यथा {
            io_address = cap_io_page_table_cap_get_capIOPTMappedAddress(io_pt_cap);
            lu_ret = lookupIOPTSlot_resolve_levels(vtd_pte, io_address >> PAGE_BITS, level - 1, level - 1);

            /* if we have been overmapped or something */
            अगर (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
                वापस;
            }
            अगर (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_to_paddr((व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(
                                                                           io_pt_cap))) {
                वापस;
            }
            *lu_ret.ioptSlot = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.ioptSlot, VTD_PTE_आकार_BITS);
        }
        invalidate_iotlb();
    }
}

व्योम unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    word_t               io_address;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    io_address  = cap_frame_cap_get_capFMappedAddress(cap);
    vtd_context_slot = lookup_vtd_context_slot(cap);


    अगर (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        वापस;
    }

    vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_address);
    अगर (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
        वापस;
    }

    अगर (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap))) {
        वापस;
    }

    *lu_ret.ioptSlot = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.ioptSlot, VTD_PTE_आकार_BITS);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ctSlot)
{
    unmapIOPage(ctSlot->cap);

    ctSlot->cap = cap_frame_cap_set_capFMappedAddress(ctSlot->cap, 0);
    ctSlot->cap = cap_frame_cap_set_capFMapType(ctSlot->cap, X86_MappingNone);
    ctSlot->cap = cap_frame_cap_set_capFMappedASID(ctSlot->cap, asidInvalid);

    वापस EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    वापस EXCEPTION_SYSCALL_ERROR;
}

#पूर्ण_अगर /* CONFIG_IOMMU */
