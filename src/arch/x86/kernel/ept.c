/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_VTX

#ضمن <model/statedata.h>
#ضمن <arch/kernel/ept.h>
#ضمن <arch/api/invocation.h>

بنية lookupEPTPDPTSlot_ret {
    exception_t status;
    ept_pdpte_t *pdptSlot;
};
تعريفنوع بنية lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

بنية lookupEPTPDSlot_ret {
    exception_t status;
    ept_pde_t  *pdSlot;
};
تعريفنوع بنية lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

بنية lookupEPTPTSlot_ret {
    exception_t status;
    ept_pte_t  *ptSlot;
};
تعريفنوع بنية lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

تعداد ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWriteBack = 6
};
تعريفنوع تعداد ept_cache_options ept_cache_options_t;

فراغ deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_HIGH(asid)];
    𐤀𐤌 (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_LOW(asid)];
        𐤀𐤌 (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_root(asid_map) == ept) {
            poolPtr->array[ASID_LOW(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    unmapEPTPage(
        cap_frame_cap_get_capFSize(cap),
        cap_frame_cap_get_capFMappedASID(cap),
        cap_frame_cap_get_capFMappedAddress(cap),
        (فراغ *)cap_frame_cap_get_capFBasePtr(cap)
    );

    cap_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, 0);
    cap_frame_cap_ptr_set_capFMappedASID(&ctSlot->cap, asidInvalid);
    cap_frame_cap_ptr_set_capFMapType(&ctSlot->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

findEPTForASID_ret_t findEPTForASID(asid_t asid)
{
    findEPTForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    𐤀𐤌 (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_root_new();

        ret.ept = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_root(asid_map);
    ret.status = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_pml4e_t *CONST lookupEPTPML4Slot(ept_pml4e_t *pml4, vptr_t vptr)
{
    أرجع pml4 + GET_EPT_PML4_INDEX(vptr);
}

ساكن lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4Slot;

    pml4Slot = lookupEPTPML4Slot(pml4, vptr);

    𐤀𐤌 (!ept_pml4e_ptr_get_read(pml4Slot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_INDEX_OFFSET);

        ret.pdptSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pdpte_t *pdpt = paddr_to_pptr(ept_pml4e_ptr_get_pdpt_base_address(pml4Slot));
    uint32_t index = GET_EPT_PDPT_INDEX(vptr);
    ret.pdptSlot = pdpt + index;
    ret.status = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pdSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    𐤀𐤌 (!ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_INDEX_OFFSET);

        ret.pdSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pde_t *pd = paddr_to_pptr(ept_pdpte_ptr_get_pd_base_address(lu_ret.pdptSlot));
    uint32_t index = GET_EPT_PD_INDEX(vptr);
    ret.pdSlot = pd + index;
    ret.status = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.ptSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    𐤀𐤌 ((ept_pde_ptr_get_page_size(lu_ret.pdSlot) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_INDEX_OFFSET);

        ret.ptSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pte_t *pt = paddr_to_pptr(ept_pde_ept_pde_pt_ptr_get_pt_base_address(lu_ret.pdSlot));
    uint32_t index = GET_EPT_PT_INDEX(vptr);

    ret.ptSlot = pt + index;
    ret.status = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_cache_options_t eptCacheFromVmAttr(vm_attributes_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.words[0];
    𐤀𐤌 (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWriteBack) {
        option = EPTWriteBack;
    }
    أرجع option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    findEPTForASID_ret_t asid_ret;
    ept_pml4e_t *pml4Slot;

    asid_ret = findEPTForASID(asid);
    𐤀𐤌 (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pml4Slot = عدم;
        ret.status = asid_ret.status;
        أرجع ret;
    }

    pml4Slot = lookupEPTPML4Slot(asid_ret.ept, vptr);

    𐤀𐤌 (ept_pml4e_ptr_get_read(pml4Slot)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_base_address(pml4Slot)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4Slot = pml4Slot;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pml4Slot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    𐤀𐤌 (lu_ret.status == EXCEPTION_NONE) {
        *lu_ret.pml4Slot = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ساكن exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    𐤀𐤌 (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
            pdpt);
        clearMemory((فراغ *)pdpt, cap_get_capSizeBits(cap));
    }
    cap_ept_pdpt_cap_ptr_set_capPDPTIsMapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4Slot,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4Slot = pml4e;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX86EPTPDPTInvocation(
    word_t invLabel,
    word_t length,
    cte_t *cte,
    cap_t cap,
    word_t *buffer
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

    𐤀𐤌 (invLabel == X86EPTPDPTUnmap) {
        𐤀𐤌 (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDPTInvocationUnmap(cap, cte);
    }

    𐤀𐤌 (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (length < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_INDEX_OFFSET) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    𐤀𐤌 (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4Slot = lookupEPTPML4Slot(pml4, vaddr);

    𐤀𐤌 (ept_pml4e_ptr_get_read(pml4Slot)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((فراغ *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_set_capPDPTIsMapped(cap, 1);
    cap = cap_ept_pdpt_cap_set_capPDPTMappedASID(cap, asid);
    cap = cap_ept_pdpt_cap_set_capPDPTMappedAddress(cap, vaddr);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDPTInvocationMap(cap, cte, pml4e, pml4Slot, pml4);
}

exception_t decodeX86EPTInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    word_t *buffer
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ept_pdpt_cap:
        أرجع decodeX86EPTPDPTInvocation(invLabel, length, cte, cap, buffer);
    حالة cap_ept_pd_cap:
        أرجع decodeX86EPTPDInvocation(invLabel, length, cte, cap, buffer);
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTPTInvocation(invLabel, length, cte, cap, buffer);
    افتراضي:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    𐤀𐤌 (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdptSlot = عدم;
        ret.status = asid_ret.status;
        أرجع ret;
    }

    find_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdptSlot = عدم;
        ret.status = find_ret.status;
        أرجع ret;
    }

    𐤀𐤌 (ept_pdpte_ptr_get_read(find_ret.pdptSlot)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_base_address(find_ret.pdptSlot)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdptSlot = find_ret.pdptSlot;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pdptSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    𐤀𐤌 (lu_ret.status == EXCEPTION_NONE) {
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

ساكن exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    𐤀𐤌 (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_capPDMappedAddress(cap),
            pd);
        clearMemory((فراغ *)pd, cap_get_capSizeBits(cap));
    }
    cap_ept_pd_cap_ptr_set_capPDIsMapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdptSlot,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdptSlot = pdpte;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    word_t invLabel,
    word_t length,
    cte_t *cte,
    cap_t cap,
    word_t *buffer
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

    𐤀𐤌 (invLabel == X86EPTPDUnmap) {
        𐤀𐤌 (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDInvocationUnmap(cap, cte);
    }

    𐤀𐤌 (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (length < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    vaddr = vaddr & ~MASK(EPT_PDPT_INDEX_OFFSET);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    𐤀𐤌 (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (find_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((فراغ *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_set_capPDIsMapped(cap, 1);
    cap = cap_ept_pd_cap_set_capPDMappedASID(cap, asid);
    cap = cap_ept_pd_cap_set_capPDMappedAddress(cap, vaddr);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdptSlot, pml4);
}

EPTPageTableMapped_ret_t EPTPageTableMapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPageTableMapped_ret_t ret;
    lookupEPTPDSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    𐤀𐤌 (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdSlot = عدم;
        ret.status = asid_ret.status;
        أرجع ret;
    }

    find_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdSlot = عدم;
        ret.status = find_ret.status;
        أرجع ret;
    }

    𐤀𐤌 (ept_pde_ptr_get_page_size(find_ret.pdSlot) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_base_address(find_ret.pdSlot)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pdSlot = find_ret.pdSlot;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pdSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPageTable(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPageTableMapped_ret_t lu_ret;

    lu_ret = EPTPageTableMapped(asid, vaddr, pt);

    𐤀𐤌 (lu_ret.status == EXCEPTION_NONE) {
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

ساكن exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    𐤀𐤌 (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmapEPTPageTable(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory((فراغ *)pt, cap_get_capSizeBits(cap));
    }
    cap_ept_pt_cap_ptr_set_capPTIsMapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pdSlot, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdSlot = pde;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    word_t invLabel,
    word_t length,
    cte_t *cte,
    cap_t cap,
    word_t *buffer
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

    𐤀𐤌 (invLabel == X86EPTPTUnmap) {
        𐤀𐤌 (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPTInvocationUnmap(cap, cte);
    }

    𐤀𐤌 (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (length < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    vaddr = vaddr & ~MASK(EPT_PD_INDEX_OFFSET);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    𐤀𐤌 (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (((ept_pde_ptr_get_page_size(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) ||
        ((ept_pde_ptr_get_page_size(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = seL4_DeleteFirst;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((فراغ *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_set_capPTIsMapped(cap, 1);
    cap = cap_ept_pt_cap_set_capPTMappedASID(cap, asid);
    cap = cap_ept_pt_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPTInvocationMap(cap, cte, pde, lu_ret.pdSlot, pml4);
}

ساكن exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *ptSlot, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *ptSlot = pte;
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pdSlot, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pdSlot[0] = pde1;
    𐤀𐤌 (LARGE_PAGE_BITS == 22) {
        pdSlot[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    word_t invLabel,
    word_t length,
    cte_t *cte,
    cap_t cap,
    word_t *buffer)
{
    word_t          vaddr;
    word_t          w_rightsMask;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_attributes_t vmAttr;
    vm_page_size_t  frameSize;
    asid_t          asid;

    frameSize = cap_frame_cap_get_capFSize(cap);
    vaddr = getSyscallArg(0, buffer);
    vaddr = vaddr & ~MASK(EPT_PT_INDEX_OFFSET);
    w_rightsMask = getSyscallArg(1, buffer);
    vmAttr = vmAttributesFromWord(getSyscallArg(2, buffer));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_frame_cap_get_capFVMRights(cap);

    𐤀𐤌 (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أكد(cap_frame_cap_get_capFMapType(cap) == X86_MappingNone);

    𐤀𐤌 (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    findEPTForASID_ret_t find_ret = findEPTForASID(asid);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

    𐤀𐤌 (!checkVPAlignment(frameSize, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((فراغ *)cap_frame_cap_get_capFBasePtr(cap));

    cap = cap_frame_cap_set_capFMappedASID(cap, asid);
    cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);
    cap = cap_frame_cap_set_capFMapType(cap, X86_MappingEPT);

    بدل (frameSize) {
    /* PTE mappings */
    حالة X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        𐤀𐤌 (ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPTE(cap, cte, lu_ret.ptSlot, pte, pml4);
    }

    /* PDE mappings */
    حالة X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        𐤀𐤌 ((ept_pde_ptr_get_page_size(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        𐤀𐤌 (LARGE_PAGE_BITS != EPT_PD_INDEX_OFFSET &&
            (ept_pde_ptr_get_page_size(lu_ret.pdSlot + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        𐤀𐤌 ((ept_pde_ptr_get_page_size(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
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
                             paddr + BIT(EPT_PD_INDEX_OFFSET),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPDE(cap, cte, lu_ret.pdSlot, pde1, pde2, pml4);
    }

    افتراضي:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ unmapEPTPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    findEPTForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findEPTForASID(asid);
    𐤀𐤌 (find_ret.status != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (page_size) {
    حالة X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(find_ret.ept, vptr);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            أرجع;
        }
        𐤀𐤌 (!ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            أرجع;
        }
        𐤀𐤌 (ept_pte_ptr_get_page_base_address(lu_ret.ptSlot) != addr) {
            أرجع;
        }

        *lu_ret.ptSlot = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        اكسر;
    }
    حالة X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(find_ret.ept, vptr);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            أرجع;
        }
        𐤀𐤌 (ept_pde_ptr_get_page_size(lu_ret.pdSlot) != ept_pde_ept_pde_2m) {
            أرجع;
        }
        𐤀𐤌 (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            أرجع;
        }
        𐤀𐤌 (ept_pde_ept_pde_2m_ptr_get_page_base_address(lu_ret.pdSlot) != addr) {
            أرجع;
        }

        lu_ret.pdSlot[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        𐤀𐤌 (LARGE_PAGE_BITS != EPT_PD_INDEX_OFFSET) {
            أكد(ept_pde_ptr_get_page_size(lu_ret.pdSlot + 1) == ept_pde_ept_pde_2m);
            أكد(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot + 1));
            أكد(ept_pde_ept_pde_2m_ptr_get_page_base_address(lu_ret.pdSlot + 1) == addr + BIT(21));

            lu_ret.pdSlot[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        اكسر;
    }
    افتراضي:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        أكد(!"Invalid page size for unmap");
    }
}

#endif /* CONFIG_VTX */
