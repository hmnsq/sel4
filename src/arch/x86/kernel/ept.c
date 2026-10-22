/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_VTX

#ضمن <model/statedata.h>
#ضمن <arch/kernel/ept.h>
#ضمن <arch/api/invocation.h>

بنية lookupEPTPDPTSlot_ret {
    exception_t حالت;
    ept_pdpte_t *pdpt_خونه;
};
تعريفنوع بنية lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

بنية lookupEPTPDSlot_ret {
    exception_t حالت;
    ept_pde_t  *pd_خونه;
};
تعريفنوع بنية lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

بنية lookupEPTPTSlot_ret {
    exception_t حالت;
    ept_pte_t  *pt_خونه;
};
تعريفنوع بنية lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

تعداد ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_شا = 6
};
تعريفنوع تعداد ept_cache_options ept_cache_options_t;

فراغ deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_لوړ(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_ټیټ(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ریښه(asid_map) == ept) {
            poolPtr->array[ASID_ټیټ(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_خونه)
{
    unmapEPTPage(
        cap_چوکاټ_cap_get_cap_FSize(cap),
        cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
        cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
        (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)
    );

    cap_چوکاټ_cap_ptr_سېټ_cap_FMapped_پته(&ct_خونه->cap, 0);
    cap_چوکاټ_cap_ptr_سېټ_cap_FMapped_ASID(&ct_خونه->cap, asidInvalid);
    cap_چوکاټ_cap_ptr_سېټ_cap_FMap_Type(&ct_خونه->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_EPTFor_ASID(asid_t asid)
{
    پیدا_کړه_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.ept = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ریښه(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_pml4e_t *CONST lookup_EPTPML4_خونه(ept_pml4e_t *pml4, vptr_t vptr)
{
    أرجع pml4 + GET_EPT_PML4_شاخص(vptr);
}

ساكن lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_خونه;

    pml4_خونه = lookup_EPTPML4_خونه(pml4, vptr);

    إذا (!ept_pml4e_ptr_get_read(pml4_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_شاخص_دننه_واټن);

        ret.pdpt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pdpte_t *pdpt = paddr_منزل_pptr(ept_pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه));
    uint32_t شاخص = GET_EPT_PDPT_شاخص(vptr);
    ret.pdpt_خونه = pdpt + شاخص;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    إذا (!ept_pdpte_ptr_get_read(lu_ret.pdpt_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_شاخص_دننه_واټن);

        ret.pd_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pde_t *pd = paddr_منزل_pptr(ept_pdpte_ptr_get_pd_بنسټ_پته(lu_ret.pdpt_خونه));
    uint32_t شاخص = GET_EPT_PD_شاخص(vptr);
    ret.pd_خونه = pd + شاخص;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    إذا ((ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_شاخص_دننه_واټن);

        ret.pt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pte_t *pt = paddr_منزل_pptr(ept_pde_ept_pde_pt_ptr_get_pt_بنسټ_پته(lu_ret.pd_خونه));
    uint32_t شاخص = GET_EPT_PT_شاخص(vptr);

    ret.pt_خونه = pt + شاخص;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_cache_options_t eptCacheFromVmAttr(vm_ځانګړتیاوې_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.ټکي[0];
    إذا (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_شا) {
        option = EPTWrite_شا;
    }
    أرجع option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    پیدا_کړه_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_خونه;

    asid_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (asid_ret.حالت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pml4_خونه = عدم;
        ret.حالت = asid_ret.حالت;
        أرجع ret;
    }

    pml4_خونه = lookup_EPTPML4_خونه(asid_ret.ept, vptr);

    إذا (ept_pml4e_ptr_get_read(pml4_خونه)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_خونه = pml4_خونه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pml4_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    إذا (lu_ret.حالت == EXCEPTION_NONE) {
        *lu_ret.pml4_خونه = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ساكن exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_پته(cap),
            pdpt);
        clear_حافظه((فراغ *)pdpt, cap_get_cap_اندازه_بټونه(cap));
    }
    cap_ept_pdpt_cap_ptr_سېټ_cap_PDPTIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_خونه,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_خونه = pml4e;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX86EPTPDPTInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret;
    ept_pml4e_t    *pml4_خونه;

    إذا (invLabel == X86EPTPDPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDPTInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_شاخص_دننه_واټن) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4_خونه = lookup_EPTPML4_خونه(pml4, vaddr);

    إذا (ept_pml4e_ptr_get_read(pml4_خونه)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr((فراغ *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_سېټ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_سېټ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_سېټ_cap_PDPTMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_خونه, pml4);
}

exception_t decodeX86EPTInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ept_pdpt_cap:
        أرجع decodeX86EPTPDPTInvocation(invLabel, اوږدتیا, cte, cap, بفر);
    حالة cap_ept_pd_cap:
        أرجع decodeX86EPTPDInvocation(invLabel, اوږدتیا, cte, cap, بفر);
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTPTInvocation(invLabel, اوږدتیا, cte, cap, بفر);
    افتراضي:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t پیدا_کړه_ret;
    پیدا_کړه_EPTFor_ASID_ret_t asid_ret;

    asid_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (asid_ret.حالت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdpt_خونه = عدم;
        ret.حالت = asid_ret.حالت;
        أرجع ret;
    }

    پیدا_کړه_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdpt_خونه = عدم;
        ret.حالت = پیدا_کړه_ret.حالت;
        أرجع ret;
    }

    إذا (ept_pdpte_ptr_get_read(پیدا_کړه_ret.pdpt_خونه)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_بنسټ_پته(پیدا_کړه_ret.pdpt_خونه)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_خونه = پیدا_کړه_ret.pdpt_خونه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pdpt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    إذا (lu_ret.حالت == EXCEPTION_NONE) {
        *lu_ret.pdpt_خونه = ept_pdpte_new(
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
    إذا (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_پته(cap),
            pd);
        clear_حافظه((فراغ *)pd, cap_get_cap_اندازه_بټونه(cap));
    }
    cap_ept_pd_cap_ptr_سېټ_cap_PDIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_خونه,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_خونه = pdpte;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    إذا (invLabel == X86EPTPDUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~ماسک(EPT_PDPT_شاخص_دننه_واټن);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ept_pdpte_ptr_get_read(lu_ret.pdpt_خونه)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr((فراغ *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_سېټ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_سېټ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_سېټ_cap_PDMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_خونه, pml4);
}

EPTPage_جدول_Mapped_ret_t EPTPage_جدول_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t پیدا_کړه_ret;
    پیدا_کړه_EPTFor_ASID_ret_t asid_ret;

    asid_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (asid_ret.حالت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pd_خونه = عدم;
        ret.حالت = asid_ret.حالت;
        أرجع ret;
    }

    پیدا_کړه_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pd_خونه = عدم;
        ret.حالت = پیدا_کړه_ret.حالت;
        أرجع ret;
    }

    إذا (ept_pde_ptr_get_پاڼه_اندازه(پیدا_کړه_ret.pd_خونه) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_بنسټ_پته(پیدا_کړه_ret.pd_خونه)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_خونه = پیدا_کړه_ret.pd_خونه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pd_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmap_EPTPage_جدول(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_جدول_Mapped(asid, vaddr, pt);

    إذا (lu_ret.حالت == EXCEPTION_NONE) {
        *lu_ret.pd_خونه = ept_pde_ept_pde_pt_new(
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
    إذا (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_جدول(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_پته(cap),
            pt);
        clear_حافظه((فراغ *)pt, cap_get_cap_اندازه_بټونه(cap));
    }
    cap_ept_pt_cap_ptr_سېټ_cap_PTIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_خونه, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_خونه = pde;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    إذا (invLabel == X86EPTPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPTInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~ماسک(EPT_PD_شاخص_دننه_واټن);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خونه)) ||
        ((ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خونه))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr((فراغ *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_سېټ_cap_PTMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_خونه, pml4);
}

ساكن exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_خونه, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_خونه = pte;
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_خونه, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_خونه[0] = pde1;
    إذا (LARGE_پاڼه_بټونه == 22) {
        pd_خونه[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر)
{
    کلمه_t          vaddr;
    کلمه_t          w_rights_ماسک;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_ځانګړتیاوې_t vmAttr;
    vm_پاڼه_اندازه_t  چوکاټ_اندازه;
    asid_t          asid;

    چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~ماسک(EPT_PT_شاخص_دننه_واټن);
    w_rights_ماسک = getSyscallArg(1, بفر);
    vmAttr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(2, بفر));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_چوکاټ_cap_get_cap_FVMRights(cap);

    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أكد(cap_چوکاټ_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

    إذا (!checkVPAlignment(چوکاټ_اندازه, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));

    cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asid);
    cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, vaddr);
    cap = cap_چوکاټ_cap_سېټ_cap_FMap_Type(cap, X86_MappingEPT);

    بدل (چوکاټ_اندازه) {
    /* PTE mappings */
    حالة X86_Small_پاڼه: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (ept_pte_ptr_get_read(lu_ret.pt_خونه)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_لومړی;
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

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPTE(cap, cte, lu_ret.pt_خونه, pte, pml4);
    }

    /* PDE mappings */
    حالة X86_Large_پاڼه: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        إذا ((ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خونه)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (LARGE_پاڼه_بټونه != EPT_PD_شاخص_دننه_واټن &&
            (ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خونه + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خونه)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_لومړی;
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
                             paddr + بټ(EPT_PD_شاخص_دننه_واټن),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPDE(cap, cte, lu_ret.pd_خونه, pde1, pde2, pml4);
    }

    افتراضي:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ unmapEPTPage(vm_پاڼه_اندازه_t پاڼه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret;
    paddr_t addr = addrFromPPtr(pptr);

    پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (پاڼه_اندازه) {
    حالة X86_Small_پاڼه: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(پیدا_کړه_ret.ept, vptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!ept_pte_ptr_get_read(lu_ret.pt_خونه)) {
            أرجع;
        }
        إذا (ept_pte_ptr_get_پاڼه_بنسټ_پته(lu_ret.pt_خونه) != addr) {
            أرجع;
        }

        *lu_ret.pt_خونه = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        اكسر;
    }
    حالة X86_Large_پاڼه: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(پیدا_کړه_ret.ept, vptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) != ept_pde_ept_pde_2m) {
            أرجع;
        }
        إذا (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خونه)) {
            أرجع;
        }
        إذا (ept_pde_ept_pde_2m_ptr_get_پاڼه_بنسټ_پته(lu_ret.pd_خونه) != addr) {
            أرجع;
        }

        lu_ret.pd_خونه[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        إذا (LARGE_پاڼه_بټونه != EPT_PD_شاخص_دننه_واټن) {
            أكد(ept_pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه + 1) == ept_pde_ept_pde_2m);
            أكد(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خونه + 1));
            أكد(ept_pde_ept_pde_2m_ptr_get_پاڼه_بنسټ_پته(lu_ret.pd_خونه + 1) == addr + بټ(21));

            lu_ret.pd_خونه[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
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

#د_که_پای /* CONFIG_VTX */
