/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_VTX

#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <arch/kernel/ept.h>
#অন্তর্ভাব <arch/api/invocation.h>

কাঠামো lookupEPTPDPTSlot_ret {
    exception_t status;
    ept_pdpte_t *pdptSlot;
};
প্রকারলেখ কাঠামো lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

কাঠামো lookupEPTPDSlot_ret {
    exception_t status;
    ept_pde_t  *pdSlot;
};
প্রকারলেখ কাঠামো lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

কাঠামো lookupEPTPTSlot_ret {
    exception_t status;
    ept_pte_t  *ptSlot;
};
প্রকারলেখ কাঠামো lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

গণনা ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWriteBack = 6
};
প্রকারলেখ গণনা ept_cache_options ept_cache_options_t;

শূন্য deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_উপরের(asid)];
    যদি (poolPtr != ফাঁকা) {
        asid_map_t asid_map = poolPtr->array[ASID_নিচের(asid)];
        যদি (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_মূল(asid_map) == ept) {
            poolPtr->array[ASID_নিচের(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    unmapEPTPage(
        cap_frame_cap_get_capFSize(cap),
        cap_frame_cap_get_capFMappedASID(cap),
        cap_frame_cap_get_capFMappedAddress(cap),
        (শূন্য *)cap_frame_cap_get_capFBasePtr(cap)
    );

    cap_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, 0);
    cap_frame_cap_ptr_set_capFMappedASID(&ctSlot->cap, asidInvalid);
    cap_frame_cap_ptr_set_capFMapType(&ctSlot->cap, X86_MappingNone);

    ফেরত EXCEPTION_NONE;
}

findEPTForASID_ret_t findEPTForASID(asid_t asid)
{
    findEPTForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    যদি (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();

        ret.ept = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_মূল(asid_map);
    ret.status = EXCEPTION_NONE;
    ফেরত ret;
}

স্থির ept_pml4e_t *CONST lookupEPTPML4Slot(ept_pml4e_t *pml4, vptr_t vptr)
{
    ফেরত pml4 + GET_EPT_PML4_সূচক(vptr);
}

স্থির lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4Slot;

    pml4Slot = lookupEPTPML4Slot(pml4, vptr);

    যদি (!ept_pml4e_ptr_get_read(pml4Slot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_সূচক_OFFSET);

        ret.pdptSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    ept_pdpte_t *pdpt = paddr_to_pptr(ept_pml4e_ptr_get_pdpt_base_address(pml4Slot));
    uint32_t সূচক = GET_EPT_PDPT_সূচক(vptr);
    ret.pdptSlot = pdpt + সূচক;
    ret.status = EXCEPTION_NONE;
    ফেরত ret;
}

স্থির lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pdSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    যদি (!ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_সূচক_OFFSET);

        ret.pdSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    ept_pde_t *pd = paddr_to_pptr(ept_pdpte_ptr_get_pd_base_address(lu_ret.pdptSlot));
    uint32_t সূচক = GET_EPT_PD_সূচক(vptr);
    ret.pdSlot = pd + সূচক;
    ret.status = EXCEPTION_NONE;
    ফেরত ret;
}

স্থির lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.ptSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    যদি ((ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_সূচক_OFFSET);

        ret.ptSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    ept_pte_t *pt = paddr_to_pptr(ept_pde_ept_pde_pt_ptr_get_pt_base_address(lu_ret.pdSlot));
    uint32_t সূচক = GET_EPT_PT_সূচক(vptr);

    ret.ptSlot = pt + সূচক;
    ret.status = EXCEPTION_NONE;
    ফেরত ret;
}

স্থির ept_cache_options_t eptCacheFromVmAttr(vm_attributes_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.শব্দগুলি[0];
    যদি (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWriteBack) {
        option = EPTWriteBack;
    }
    ফেরত option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    findEPTForASID_ret_t asid_ret;
    ept_pml4e_t *pml4Slot;

    asid_ret = findEPTForASID(asid);
    যদি (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = ফাঁকা;
        ret.pml4Slot = ফাঁকা;
        ret.status = asid_ret.status;
        ফেরত ret;
    }

    pml4Slot = lookupEPTPML4Slot(asid_ret.ept, vptr);

    যদি (ept_pml4e_ptr_get_read(pml4Slot)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_base_address(pml4Slot)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4Slot = pml4Slot;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    } অন্যথা {
        ret.pml4 = ফাঁকা;
        ret.pml4Slot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }
}

শূন্য unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    যদি (lu_ret.status == EXCEPTION_NONE) {
        *lu_ret.pml4Slot = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

স্থির exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    যদি (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
            pdpt);
        clearMemory((শূন্য *)pdpt, cap_get_cap_আকার_Bits(cap));
    }
    cap_ept_pdpt_cap_ptr_set_capPDPTIsMapped(&(cte->cap), 0);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4Slot,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4Slot = pml4e;
    invept(pml4);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeX86EPTPDPTInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cte_t *cte,
    cap_t cap,
    word_t *বাফার
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

    যদি (invLabel == X86EPTPDPTUnmap) {
        যদি (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performEPTPDPTInvocationUnmap(cap, cte);
    }

    যদি (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < 2 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, বাফার);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_সূচক_OFFSET) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pml4Slot = lookupEPTPML4Slot(pml4, vaddr);

    যদি (ept_pml4e_ptr_get_read(pml4Slot)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((শূন্য *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_set_capPDPTIsMapped(cap, 1);
    cap = cap_ept_pdpt_cap_set_capPDPTMappedASID(cap, asid);
    cap = cap_ept_pdpt_cap_set_capPDPTMappedAddress(cap, vaddr);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত performEPTPDPTInvocationMap(cap, cte, pml4e, pml4Slot, pml4);
}

exception_t decodeX86EPTInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    word_t *বাফার
)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_ept_pdpt_cap:
        ফেরত decodeX86EPTPDPTInvocation(invLabel, দৈর্ঘ্য, cte, cap, বাফার);
    ক্ষেত্র cap_ept_pd_cap:
        ফেরত decodeX86EPTPDInvocation(invLabel, দৈর্ঘ্য, cte, cap, বাফার);
    ক্ষেত্র cap_ept_pt_cap:
        ফেরত decodeX86EPTPTInvocation(invLabel, দৈর্ঘ্য, cte, cap, বাফার);
    পূর্বনির্ধারিত:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    যদি (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = ফাঁকা;
        ret.pdptSlot = ফাঁকা;
        ret.status = asid_ret.status;
        ফেরত ret;
    }

    find_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    যদি (find_ret.status != EXCEPTION_NONE) {
        ret.pml4 = ফাঁকা;
        ret.pdptSlot = ফাঁকা;
        ret.status = find_ret.status;
        ফেরত ret;
    }

    যদি (ept_pdpte_ptr_get_read(find_ret.pdptSlot)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_base_address(find_ret.pdptSlot)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdptSlot = find_ret.pdptSlot;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    } অন্যথা {
        ret.pml4 = ফাঁকা;
        ret.pdptSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }
}

শূন্য unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    যদি (lu_ret.status == EXCEPTION_NONE) {
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

স্থির exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    যদি (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_capPDMappedAddress(cap),
            pd);
        clearMemory((শূন্য *)pd, cap_get_cap_আকার_Bits(cap));
    }
    cap_ept_pd_cap_ptr_set_capPDIsMapped(&(cte->cap), 0);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdptSlot,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdptSlot = pdpte;
    invept(pml4);

    ফেরত EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cte_t *cte,
    cap_t cap,
    word_t *বাফার
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

    যদি (invLabel == X86EPTPDUnmap) {
        যদি (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performEPTPDInvocationUnmap(cap, cte);
    }

    যদি (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < 2 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, বাফার);
    vaddr = vaddr & ~MASK(EPT_PDPT_সূচক_OFFSET);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (find_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (ept_pdpte_ptr_get_read(lu_ret.pdptSlot)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = seL4_DeleteFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((শূন্য *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
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

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdptSlot, pml4);
}

EPTPageTableMapped_ret_t EPTPageTableMapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPageTableMapped_ret_t ret;
    lookupEPTPDSlot_ret_t find_ret;
    findEPTForASID_ret_t asid_ret;

    asid_ret = findEPTForASID(asid);
    যদি (asid_ret.status != EXCEPTION_NONE) {
        ret.pml4 = ফাঁকা;
        ret.pdSlot = ফাঁকা;
        ret.status = asid_ret.status;
        ফেরত ret;
    }

    find_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    যদি (find_ret.status != EXCEPTION_NONE) {
        ret.pml4 = ফাঁকা;
        ret.pdSlot = ফাঁকা;
        ret.status = find_ret.status;
        ফেরত ret;
    }

    যদি (ept_pde_ptr_get_page_আকার(find_ret.pdSlot) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_base_address(find_ret.pdSlot)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pdSlot = find_ret.pdSlot;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    } অন্যথা {
        ret.pml4 = ফাঁকা;
        ret.pdSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }
}

শূন্য unmapEPTPageTable(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPageTableMapped_ret_t lu_ret;

    lu_ret = EPTPageTableMapped(asid, vaddr, pt);

    যদি (lu_ret.status == EXCEPTION_NONE) {
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

স্থির exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    যদি (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmapEPTPageTable(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory((শূন্য *)pt, cap_get_cap_আকার_Bits(cap));
    }
    cap_ept_pt_cap_ptr_set_capPTIsMapped(&(cte->cap), 0);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pdSlot, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdSlot = pde;
    invept(pml4);

    ফেরত EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cte_t *cte,
    cap_t cap,
    word_t *বাফার
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

    যদি (invLabel == X86EPTPTUnmap) {
        যদি (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performEPTPTInvocationUnmap(cap, cte);
    }

    যদি (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < 2 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, বাফার);
    vaddr = vaddr & ~MASK(EPT_PD_সূচক_OFFSET);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    find_ret = findEPTForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (((ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) ||
        ((ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = seL4_DeleteFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((শূন্য *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
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

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত performEPTPTInvocationMap(cap, cte, pde, lu_ret.pdSlot, pml4);
}

স্থির exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *ptSlot, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *ptSlot = pte;
    cte->cap = cap;
    invept(pml4);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pdSlot, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pdSlot[0] = pde1;
    যদি (LARGE_PAGE_BITS == 22) {
        pdSlot[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    ফেরত EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cte_t *cte,
    cap_t cap,
    word_t *বাফার)
{
    word_t          vaddr;
    word_t          w_rightsMask;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_attributes_t vmAttr;
    vm_page_আকার_t  frame_আকার;
    asid_t          asid;

    frame_আকার = cap_frame_cap_get_capFSize(cap);
    vaddr = getSyscallArg(0, বাফার);
    vaddr = vaddr & ~MASK(EPT_PT_সূচক_OFFSET);
    w_rightsMask = getSyscallArg(1, বাফার);
    vmAttr = vmAttributesFromWord(getSyscallArg(2, বাফার));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_frame_cap_get_capFVMRights(cap);

    যদি (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    প্রতিজ্ঞা(cap_frame_cap_get_capFMapType(cap) == X86_MappingNone);

    যদি (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_capPML4BasePtr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    findEPTForASID_ret_t find_ret = findEPTForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (find_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

    যদি (!checkVPAlignment(frame_আকার, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr((শূন্য *)cap_frame_cap_get_capFBasePtr(cap));

    cap = cap_frame_cap_set_capFMappedASID(cap, asid);
    cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);
    cap = cap_frame_cap_set_capFMapType(cap, X86_MappingEPT);

    নির্বাচন (frame_আকার) {
    /* PTE mappings */
    ক্ষেত্র X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performEPTPageMapPTE(cap, cte, lu_ret.ptSlot, pte, pml4);
    }

    /* PDE mappings */
    ক্ষেত্র X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }


        যদি ((ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        যদি (LARGE_PAGE_BITS != EPT_PD_সূচক_OFFSET &&
            (ept_pde_ptr_get_page_আকার(lu_ret.pdSlot + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pdSlot + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        যদি ((ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = seL4_DeleteFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
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
                             paddr + BIT(EPT_PD_সূচক_OFFSET),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performEPTPageMapPDE(cap, cte, lu_ret.pdSlot, pde1, pde2, pml4);
    }

    পূর্বনির্ধারিত:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

শূন্য unmapEPTPage(vm_page_আকার_t page_আকার, asid_t asid, vptr_t vptr, শূন্য *pptr)
{
    findEPTForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findEPTForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        ফেরত;
    }

    নির্বাচন (page_আকার) {
    ক্ষেত্র X86_SmallPage: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(find_ret.ept, vptr);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            ফেরত;
        }
        যদি (!ept_pte_ptr_get_read(lu_ret.ptSlot)) {
            ফেরত;
        }
        যদি (ept_pte_ptr_get_page_base_address(lu_ret.ptSlot) != addr) {
            ফেরত;
        }

        *lu_ret.ptSlot = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        বিরতি;
    }
    ক্ষেত্র X86_LargePage: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(find_ret.ept, vptr);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            ফেরত;
        }
        যদি (ept_pde_ptr_get_page_আকার(lu_ret.pdSlot) != ept_pde_ept_pde_2m) {
            ফেরত;
        }
        যদি (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot)) {
            ফেরত;
        }
        যদি (ept_pde_ept_pde_2m_ptr_get_page_base_address(lu_ret.pdSlot) != addr) {
            ফেরত;
        }

        lu_ret.pdSlot[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        যদি (LARGE_PAGE_BITS != EPT_PD_সূচক_OFFSET) {
            প্রতিজ্ঞা(ept_pde_ptr_get_page_আকার(lu_ret.pdSlot + 1) == ept_pde_ept_pde_2m);
            প্রতিজ্ঞা(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pdSlot + 1));
            প্রতিজ্ঞা(ept_pde_ept_pde_2m_ptr_get_page_base_address(lu_ret.pdSlot + 1) == addr + BIT(21));

            lu_ret.pdSlot[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        প্রতিজ্ঞা(!"Invalid page size for unmap");
    }
}

#endif /* CONFIG_VTX */
