/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_IOMMU

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
    word_t      index;
} lookupVTDContextSlot_ret_t;


BOOT_CODE cap_t master_iospace_cap(शून्य)
{
    यदि (x86KSnumDrhu == 0) {
        फर्काऊ cap_null_cap_new();
    }

    फर्काऊ
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

स्थिर vtd_cte_t *lookup_vtd_context_slot(cap_t cap)
{
    uint32_t   vtd_जरा_index;
    uint32_t   vtd_context_index;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_जरा_slot;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_slot;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        रोक;

    पक्ष cap_io_page_table_cap:
        pci_request_id = cap_io_page_table_cap_get_capIOPTIOASID(cap);
        रोक;

    पक्ष cap_frame_cap:
        pci_request_id = cap_frame_cap_get_capFMappedASID(cap);
        रोक;

    स्वभाव:
        fail("Invalid cap type");
    }

    vtd_जरा_index = get_pci_bus(pci_request_id);
    vtd_जरा_slot = x86_KSvtd_जरा_Table + vtd_जरा_index;

    vtd_context = (vtd_cte_t *)paddr_to_pptr(vtd_rte_ptr_get_ctp(vtd_जरा_slot));
    vtd_context_index = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_slot = &vtd_context[vtd_context_index];

    फर्काऊ vtd_context_slot;
}

स्थिर lookupIOPTSlot_ret_t lookupIOPTSlot_resolve_levels(vtd_pte_t *iopt, word_t translation,
                                                          word_t levels_to_resolve, word_t levels_remaining)
{
    lookupIOPTSlot_ret_t ret;

    word_t      iopt_index = 0;
    vtd_pte_t   *iopt_slot = 0;
    vtd_pte_t   *next_iopt_slot = 0;

    यदि (iopt == 0) {
        ret.ioptSlot = 0;
        ret.level = levels_remaining;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        फर्काऊ ret;
    }

    iopt_index = (translation  >> (VTD_PT_INDEX_BITS * (x86KSnumIOPTLevels - 1 - (levels_to_resolve - levels_remaining)))) &
                 MASK(VTD_PT_INDEX_BITS);
    iopt_slot = iopt + iopt_index;

    यदि (!vtd_pte_ptr_get_write(iopt_slot) || levels_remaining == 0) {
        ret.ioptSlot = iopt_slot;
        ret.level = levels_remaining;
        ret.status = EXCEPTION_NONE;
        फर्काऊ ret;
    }
    next_iopt_slot = (vtd_pte_t *)paddr_to_pptr(vtd_pte_ptr_get_addr(iopt_slot));
    फर्काऊ lookupIOPTSlot_resolve_levels(next_iopt_slot, translation, levels_to_resolve, levels_remaining - 1);
}


स्थिर अन्तःस्थ lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, word_t io_address)
{
    lookupIOPTSlot_ret_t ret;

    यदि (iopt == 0) {
        ret.ioptSlot    = 0;
        ret.level       = 0;
        ret.status      = EXCEPTION_LOOKUP_FAULT;
        फर्काऊ ret;
    } अन्यथा {
        फर्काऊ lookupIOPTSlot_resolve_levels(iopt, io_address >> PAGE_BITS,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

शून्य unmapVTDContextEntry(cap_t cap)
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

    flushCacheRange(cte, VTD_CTE_SIZE_BITS);
    invalidate_iotlb();
    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ;
}

स्थिर exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    deleteIOPageTable(cap);
    cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 0);
    ctSlot->cap = cap;

    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t perform_X86_IOPTInvocation_Map_Context_जरा(cap_t cap, cte_t *ctSlot, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_slot)
{
    *vtd_context_slot = vtd_cte;
    flushCacheRange(vtd_context_slot, VTD_CTE_SIZE_BITS);
    ctSlot->cap = cap;

    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_SIZE_BITS);
    ctSlot->cap = cap;

    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    word_t       invLabel,
    word_t       length,
    cte_t       *slot,
    cap_t        cap,
    word_t      *buffer
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    word_t   io_address;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_slot;
    vtd_pte_t *vtd_pte;

    यदि (invLabel == X86IOPageTableUnmap) {
        यदि (!isFinalCapability(slot)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performX86IOPTInvocationUnmap(cap, slot);
    }

    यदि (invLabel != X86IOPageTableMap) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (current_extra_caps.excaprefs[0] == रिक्त || length < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_address   = getSyscallArg(0, buffer) & ~MASK(VTD_PT_INDEX_BITS + seL4_PageBits);

    यदि (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    यदि (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr(VTD_PTE_PTR(cap_io_page_table_cap_get_capIOPTBasePtr(cap)));
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

        cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 1);
        cap = cap_io_page_table_cap_set_capIOPTLevel(cap, 0);
        cap = cap_io_page_table_cap_set_capIOPTIOASID(cap, pci_request_id);

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ perform_X86_IOPTInvocation_Map_Context_जरा(cap, slot, vtd_cte, vtd_context_slot);
    } अन्यथा {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_address);

        यदि (lu_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
            current_syscall_error.type = seL4_DeleteFirst;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
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

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performX86IOPTInvocationMapPT(cap, slot, iopte, lu_ret.ioptSlot);
    }
}

स्थिर exception_t performX86IOInvocationMap(cap_t cap, cte_t *ctSlot, vtd_pte_t iopte, vtd_pte_t *ioptSlot)
{
    ctSlot->cap = cap;
    *ioptSlot = iopte;
    flushCacheRange(ioptSlot, VTD_PTE_SIZE_BITS);

    फर्काऊ EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    word_t       length,
    cte_t       *slot,
    cap_t        cap,
    word_t      *buffer
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

    यदि (current_extra_caps.excaprefs[0] == रिक्त || length < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_frame_cap_get_capFSize(cap) != X86_SmallPage) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_address  = getSyscallArg(1, buffer) & ~MASK(PAGE_BITS);
    paddr       = pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap));

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    यदि (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_slot = lookup_vtd_context_slot(io_space);

    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_address);
    यदि (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != 0) {
        current_syscall_error.type = seL4_DeleteFirst;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_mask = rightsFromWord(getSyscallArg(0, buffer));
    frame_cap_rights    = cap_frame_cap_get_capFVMRights(cap);

    bool_t लिख = seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask) && (frame_cap_rights == VMReadWrite);
    bool_t पठ = seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) && (frame_cap_rights != VMKernelOnly);
    यदि (लिख || पठ) {
        iopte = vtd_pte_new(paddr, !!लिख, !!पठ);
    } अन्यथा {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_frame_cap_set_capFMapType(cap, X86_MappingIOSpace);
    cap = cap_frame_cap_set_capFMappedASID(cap, pci_request_id);
    cap = cap_frame_cap_set_capFMappedAddress(cap, io_address);

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ performX86IOInvocationMap(cap, slot, iopte, lu_ret.ioptSlot);
}

शून्य deleteIOPageTable(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    word_t               io_address;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    यदि (cap_io_page_table_cap_get_capIOPTIsMapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_table_cap_set_capIOPTIsMapped(io_pt_cap, 0);
        level = cap_io_page_table_cap_get_capIOPTLevel(io_pt_cap);
        vtd_context_slot = lookup_vtd_context_slot(io_pt_cap);

        यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
            फर्काऊ;
        }

        vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

        यदि (level == 0) {
            /* if we have been overmapped or something */
            यदि (pptr_to_paddr(vtd_pte) != pptr_to_paddr((शून्य *)cap_io_page_table_cap_get_capIOPTBasePtr(io_pt_cap))) {
                फर्काऊ;
            }
            *vtd_context_slot = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_slot, VTD_CTE_SIZE_BITS);
        } अन्यथा {
            io_address = cap_io_page_table_cap_get_capIOPTMappedAddress(io_pt_cap);
            lu_ret = lookupIOPTSlot_resolve_levels(vtd_pte, io_address >> PAGE_BITS, level - 1, level - 1);

            /* if we have been overmapped or something */
            यदि (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
                फर्काऊ;
            }
            यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_to_paddr((शून्य *)cap_io_page_table_cap_get_capIOPTBasePtr(
                                                                           io_pt_cap))) {
                फर्काऊ;
            }
            *lu_ret.ioptSlot = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.ioptSlot, VTD_PTE_SIZE_BITS);
        }
        invalidate_iotlb();
    }
}

शून्य unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    word_t               io_address;
    vtd_cte_t           *vtd_context_slot;
    vtd_pte_t           *vtd_pte;

    io_address  = cap_frame_cap_get_capFMappedAddress(cap);
    vtd_context_slot = lookup_vtd_context_slot(cap);


    यदि (!vtd_cte_ptr_get_present(vtd_context_slot)) {
        फर्काऊ;
    }

    vtd_pte = (vtd_pte_t *)paddr_to_pptr(vtd_cte_ptr_get_asr(vtd_context_slot));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_address);
    यदि (lu_ret.status != EXCEPTION_NONE || lu_ret.level != 0) {
        फर्काऊ;
    }

    यदि (vtd_pte_ptr_get_addr(lu_ret.ioptSlot) != pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap))) {
        फर्काऊ;
    }

    *lu_ret.ioptSlot = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.ioptSlot, VTD_PTE_SIZE_BITS);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ctSlot)
{
    unmapIOPage(ctSlot->cap);

    ctSlot->cap = cap_frame_cap_set_capFMappedAddress(ctSlot->cap, 0);
    ctSlot->cap = cap_frame_cap_set_capFMapType(ctSlot->cap, X86_MappingNone);
    ctSlot->cap = cap_frame_cap_set_capFMappedASID(ctSlot->cap, asidInvalid);

    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    फर्काऊ EXCEPTION_SYSCALL_ERROR;
}

#endif /* CONFIG_IOMMU */
