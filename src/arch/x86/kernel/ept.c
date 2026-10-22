/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_VTX

#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/kernel/ept.h>
#अन्तर्भाव <arch/api/invocation.h>

संरचना lookupEPTPDPTSlot_ret {
    exception_t स्थितिः;
    ept_pdpte_t *pdptSlot;
};
प्रकारनिर्देश संरचना lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

संरचना lookupEPTPDSlot_ret {
    exception_t स्थितिः;
    ept_pde_t  *pdSlot;
};
प्रकारनिर्देश संरचना lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

संरचना lookupEPTPTSlot_ret {
    exception_t स्थितिः;
    ept_pte_t  *ptSlot;
};
प्रकारनिर्देश संरचना lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

गणना ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_पृष्ठभागः = 6
};
प्रकारनिर्देश गणना ept_cache_options ept_cache_options_t;

शून्य deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_उच्चः(asid)];
    यदि (poolPtr != रिक्त) {
        asid_map_t asid_map = poolPtr->array[ASID_नीचः(asid)];
        यदि (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_मूलम्(asid_map) == ept) {
            poolPtr->array[ASID_नीचः(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    unmapEPTPage(
        cap_frame_cap_get_capFSize(cap),
        cap_frame_cap_get_capFMappedASID(cap),
        cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
        (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
    );

    cap_frame_cap_ptr_समुच्चयः_cap_FMapped_स्थानाङ्कः(&ctSlot->cap, 0);
    cap_frame_cap_ptr_समुच्चयः_cap_FMapped_ASID(&ctSlot->cap, asidInvalid);
    cap_frame_cap_ptr_समुच्चयः_cap_FMap_Type(&ctSlot->cap, X86_MappingNone);

    प्रत्यागम EXCEPTION_NONE;
}

findEPTForASID_ret_t findEPTForASID(asid_t asid)
{
    findEPTForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.ept = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_मूलम्(asid_map);
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

स्थिर ept_pml4e_t *CONST lookupEPTPML4Slot(ept_pml4e_t *pml4, vptr_t vptr)
{
    प्रत्यागम pml4 + GET_EPT_PML4_सूचकांक(vptr);
}

स्थिर lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4Slot;

    pml4Slot = lookupEPTPML4Slot(pml4, vptr);

    यदि (!ept_pml4e_ptr_get_read(pml4Slot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_सूचकांक_विस्थापनम्);

        ret.pdptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ept_pdpte_t *pdpt = paddr_गन्तव्यम्_pptr(ept_pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot));
    uint32_t सूचकांक = GET_EPT_PDPT_सूचकांक(vptr);
    ret.pdptSlot = pdpt + सूचकांक;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

स्थिर lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pdSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    यदि (!ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_सूचकांक_विस्थापनम्);

        ret.pdSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ept_pde_t *pd = paddr_गन्तव्यम्_pptr(ept_pdpte_ptr_get_pd_आधारः_स्थानाङ्कः(lu_ret.pdptSlot));
    uint32_t सूचकांक = GET_EPT_PD_सूचकांक(vptr);
    ret.pdSlot = pd + सूचकांक;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

स्थिर lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.ptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    यदि ((ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_सूचकांक_विस्थापनम्);

        ret.ptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ept_pte_t *pt = paddr_गन्तव्यम्_pptr(ept_pde_ept_pde_pt_ptr_get_pt_आधारः_स्थानाङ्कः(lu_ret.pdSlot));
    uint32_t सूचकांक = GET_EPT_PT_सूचकांक(vptr);

    ret.ptSlot = pt + सूचकांक;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

स्थिर ept_cache_options_t eptCacheFromVmAttr(vm_गुणाः_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.शब्दाः[0];
    यदि (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_पृष्ठभागः) {
        option = EPTWrite_पृष्ठभागः;
    }
    प्रत्यागम option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    findEPTForASID_ret_t asid_ret;
    ept_pml4e_t *pml4Slot;

    asid_ret = findEPTForASID(asid);
    यदि (asid_ret.स्थितिः != EXCEPTION_NONE) {
        ret.pml4 = रिक्त;
        ret.pml4Slot = रिक्त;
        ret.स्थितिः = asid_ret.स्थितिः;
        प्रत्यागम ret;
    }

    pml4Slot = lookupEPTPML4Slot(asid_ret.ept, vptr);

    यदि (ept_pml4e_ptr_get_read(pml4Slot)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4Slot = pml4Slot;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    } अन्यथा {
        ret.pml4 = रिक्त;
        ret.pml4Slot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }
}

शून्य unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    यदि (lu_ret.स्थितिः == EXCEPTION_NONE) {
        *lu_ret.pml4Slot = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

स्थिर exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    यदि (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(cap),
            pdpt);
        clear_स्मृतिः((शून्य *)pdpt, cap_get_cap_आकार_द्वयङ्काः(cap));
    }
    cap_ept_pdpt_cap_ptr_समुच्चयः_cap_PDPTIs_Mapped(&(cte->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4Slot,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4Slot = pml4e;
    invept(pml4);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeX86EPTPDPTInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cte_t *cte,
    cap_t cap,
    word_t *बफर
)
{
    word_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    findEPTForASID_ret_t find_ret;
    ept_pml4e_t    *pml4Slot;

    यदि (invLabel == X86EPTPDPTUnmap) {
        यदि (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performEPTPDPTInvocationUnmap(cap, cte);
    }

    यदि (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_सूचकांक_विस्थापनम्) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4Slot = lookupEPTPML4Slot(pml4, vaddr);

    यदि (ept_pml4e_ptr_get_read(pml4Slot)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr((शून्य *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_समुच्चयः_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_समुच्चयः_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_समुच्चयः_cap_PDPTMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performEPTPDPTInvocationMap(cap, cte, pml4e, pml4Slot, pml4);
}

exception_t decodeX86EPTInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_ept_pdpt_cap:
        प्रत्यागम decodeX86EPTPDPTInvocation(invLabel, दीर्घता, cte, cap, बफर);
    पक्ष cap_ept_pd_cap:
        प्रत्यागम decodeX86EPTPDInvocation(invLabel, दीर्घता, cte, cap, बफर);
    पक्ष cap_ept_pt_cap:
        प्रत्यागम decodeX86EPTPTInvocation(invLabel, दीर्घता, cte, cap, बफर);
    स्वभाव:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    यदि (asid_ret.स्थितिः != EXCEPTION_NONE) {
        ret.pml4 = रिक्त;
        ret.pdptSlot = रिक्त;
        ret.स्थितिः = asid_ret.स्थितिः;
        प्रत्यागम ret;
    }

    find_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        ret.pml4 = रिक्त;
        ret.pdptSlot = रिक्त;
        ret.स्थितिः = find_ret.स्थितिः;
        प्रत्यागम ret;
    }

    यदि (ept_pdpte_ptr_get_read(find_ret.pdptSlot)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_आधारः_स्थानाङ्कः(find_ret.pdptSlot)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdptSlot = find_ret.pdptSlot;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    } अन्यथा {
        ret.pml4 = रिक्त;
        ret.pdptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }
}

शून्य unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    यदि (lu_ret.स्थितिः == EXCEPTION_NONE) {
        *lu_ret.pdptSlot = ept_pdpte_new(
                               0,  /* pd_base_address  */
                               0,  /* avl_cte_depth    */
                               0,  /* execute          */
                               0,  /* write            */
                               0   /* read             */
                           );
        invept(lu_ret.pml4);
    }
}

स्थिर exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    यदि (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_स्थानाङ्कः(cap),
            pd);
        clear_स्मृतिः((शून्य *)pd, cap_get_cap_आकार_द्वयङ्काः(cap));
    }
    cap_ept_pd_cap_ptr_समुच्चयः_cap_PDIs_Mapped(&(cte->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdptSlot,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdptSlot = pdpte;
    invept(pml4);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cte_t *cte,
    cap_t cap,
    word_t *बफर
)
{
    word_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    findEPTForASID_ret_t find_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    यदि (invLabel == X86EPTPDUnmap) {
        यदि (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performEPTPDInvocationUnmap(cap, cte);
    }

    यदि (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
    vaddr = vaddr & ~आवरणम्(EPT_PDPT_सूचकांक_विस्थापनम्);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr((शून्य *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_समुच्चयः_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_समुच्चयः_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_समुच्चयः_cap_PDMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdptSlot, pml4);
}

EPTPage_सारणी_Mapped_ret_t EPTPage_सारणी_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_सारणी_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    यदि (asid_ret.स्थितिः != EXCEPTION_NONE) {
        ret.pml4 = रिक्त;
        ret.pdSlot = रिक्त;
        ret.स्थितिः = asid_ret.स्थितिः;
        प्रत्यागम ret;
    }

    find_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        ret.pml4 = रिक्त;
        ret.pdSlot = रिक्त;
        ret.स्थितिः = find_ret.स्थितिः;
        प्रत्यागम ret;
    }

    यदि (ept_pde_ptr_get_page_आकार(find_ret.pdSlot) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_आधारः_स्थानाङ्कः(find_ret.pdSlot)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pdSlot = find_ret.pdSlot;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    } अन्यथा {
        ret.pml4 = रिक्त;
        ret.pdSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }
}

शून्य unmap_EPTPage_सारणी(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_सारणी_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_सारणी_Mapped(asid, vaddr, pt);

    यदि (lu_ret.स्थितिः == EXCEPTION_NONE) {
        *lu_ret.pdSlot = ept_pde_ept_pde_pt_new(
                             0,  /* pt_base_address  */
                             0,  /* avl_cte_depth    */
                             0,  /* execute          */
                             0,  /* write            */
                             0   /* read             */
                         );
        invept(lu_ret.pml4);
    }
}

स्थिर exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    यदि (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_सारणी(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
            pt);
        clear_स्मृतिः((शून्य *)pt, cap_get_cap_आकार_द्वयङ्काः(cap));
    }
    cap_ept_pt_cap_ptr_समुच्चयः_cap_PTIs_Mapped(&(cte->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pdSlot, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdSlot = pde;
    invept(pml4);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cte_t *cte,
    cap_t cap,
    word_t *बफर
)
{
    word_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    findEPTForASID_ret_t find_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    यदि (invLabel == X86EPTPTUnmap) {
        यदि (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performEPTPTInvocationUnmap(cap, cte);
    }

    यदि (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
    vaddr = vaddr & ~आवरणम्(EPT_PD_सूचकांक_विस्थापनम्);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (((ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) ||
        ((ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr((शून्य *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_समुच्चयः_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_समुच्चयः_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_समुच्चयः_cap_PTMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performEPTPTInvocationMap(cap, cte, pde, lu_ret.pdSlot, pml4);
}

स्थिर exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *ptSlot, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *ptSlot = pte;
    cte->cap = cap;
    invept(pml4);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pdSlot, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pdSlot[0] = pde1;
    यदि (LARGE_PAGE_द्वयङ्काः == 22) {
        pdSlot[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    word_t invLabel,
    word_t दीर्घता,
    cte_t *cte,
    cap_t cap,
    word_t *बफर)
{
    word_t          vaddr;
    word_t          w_rights_आवरणम्;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_गुणाः_t vmAttr;
    vm_page_आकार_t  frame_आकार;
    asid_t          asid;

    frame_आकार = cap_frame_cap_get_capFSize(cap);
    vaddr = getSyscallArg(0, बफर);
    vaddr = vaddr & ~आवरणम्(EPT_PT_सूचकांक_विस्थापनम्);
    w_rights_आवरणम् = getSyscallArg(1, बफर);
    vmAttr = vm_गुणाः_From_Word(getSyscallArg(2, बफर));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_frame_cap_get_capFVMRights(cap);

    यदि (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रतिज्ञा(cap_frame_cap_get_capFMapType(cap) == X86_MappingNone);

    यदि (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    findEPTForASID_ret_t find_ret = findEPTForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = आवरणम्_VMRights(capVMRights, rightsFromWord(w_rights_आवरणम्));

    यदि (!checkVPAlignment(frame_आकार, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap));

    cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asid);
    cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, vaddr);
    cap = cap_frame_cap_समुच्चयः_cap_FMap_Type(cap, X86_MappingEPT);

    विकल्प (frame_आकार) {
    /* PTE mappings */
    पक्ष X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performEPTPageMapPTE(cap, cte, lu_ret.ptSlot, pte, pml4);
    }

    /* PDE mappings */
    पक्ष X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }


        यदि ((ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (LARGE_PAGE_द्वयङ्काः != EPT_PD_सूचकांक_विस्थापनम् &&
            (ept_pde_ptr_get_page_आकार(lu_ret.pdSlot + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि ((ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        ept_pde_t pde1 = ept_pde_ept_pde_2m_new(
                             paddr,
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        ept_pde_t pde2 = ept_pde_ept_pde_2m_new(
                             paddr + BIT(EPT_PD_सूचकांक_विस्थापनम्),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performEPTPageMapPDE(cap, cte, lu_ret.pdSlot, pde1, pde2, pml4);
    }

    स्वभाव:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य unmapEPTPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, शून्य *pptr)
{
    findEPTForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findEPTForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    विकल्प (page_आकार) {
    पक्ष X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(find_ret.ept, vptr);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम;
        }
        यदि (!ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            प्रत्यागम;
        }
        यदि (ept_pte_ptr_get_page_आधारः_स्थानाङ्कः(lu_ret.ptSlot) != addr) {
            प्रत्यागम;
        }

        *lu_ret.ptSlot = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        भङ्ग;
    }
    पक्ष X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(find_ret.ept, vptr);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम;
        }
        यदि (ept_pde_ptr_get_page_आकार(lu_ret.pdSlot) != ept_pde_ept_pde_2m) {
            प्रत्यागम;
        }
        यदि (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            प्रत्यागम;
        }
        यदि (ept_pde_ept_pde_2m_ptr_get_page_आधारः_स्थानाङ्कः(lu_ret.pdSlot) != addr) {
            प्रत्यागम;
        }

        lu_ret.pdSlot[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        यदि (LARGE_PAGE_द्वयङ्काः != EPT_PD_सूचकांक_विस्थापनम्) {
            प्रतिज्ञा(ept_pde_ptr_get_page_आकार(lu_ret.pdSlot + 1) == ept_pde_ept_pde_2m);
            प्रतिज्ञा(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot + 1));
            प्रतिज्ञा(ept_pde_ept_pde_2m_ptr_get_page_आधारः_स्थानाङ्कः(lu_ret.pdSlot + 1) == addr + BIT(21));

            lu_ret.pdSlot[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        भङ्ग;
    }
    स्वभाव:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        प्रतिज्ञा(!"Invalid page size for unmap");
    }
}

#यदि_अन्तः /* CONFIG_VTX */
