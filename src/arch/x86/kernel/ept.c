/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX

#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/ept.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>

ਬਣਤਰ lookupEPTPDPTSlot_ret {
    exception_t ਸਥਿਤੀ;
    ept_pdpte_t *pdpt_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

ਬਣਤਰ lookupEPTPDSlot_ret {
    exception_t ਸਥਿਤੀ;
    ept_pde_t  *pd_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

ਬਣਤਰ lookupEPTPTSlot_ret {
    exception_t ਸਥਿਤੀ;
    ept_pte_t  *pt_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

ਗਿਣਤੀ ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_ਪਿੱਠ = 6
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ept_cache_options ept_cache_options_t;

ਖਾਲੀ deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (poolPtr != ਨਿਰਾ) {
        asid_map_t asid_map = poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
        ਜੇ (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ਜੜ੍ਹ(asid_map) == ept) {
            poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    unmapEPTPage(
        cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
        cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
        cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
        (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
    );

    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMapped_ਪਤਾ(&ct_ਥਾਂ->cap, 0);
    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMapped_ASID(&ct_ਥਾਂ->cap, asidInvalid);
    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMap_Type(&ct_ਥਾਂ->cap, X86_MappingNone);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_EPTFor_ASID(asid_t asid)
{
    ਲੱਭੋ_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.ept = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ਜੜ੍ਹ(asid_map);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸਥਿਰ ept_pml4e_t *CONST lookup_EPTPML4_ਥਾਂ(ept_pml4e_t *pml4, vptr_t vptr)
{
    ਵਾਪਸ pml4 + GET_EPT_PML4_ਸੂਚਕਾਂਕ(vptr);
}

ਸਥਿਰ lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_ਥਾਂ;

    pml4_ਥਾਂ = lookup_EPTPML4_ਥਾਂ(pml4, vptr);

    ਜੇ (!ept_pml4e_ptr_get_read(pml4_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ);

        ret.pdpt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ept_pdpte_t *pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(ept_pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ));
    uint32_t ਸੂਚਕਾਂਕ = GET_EPT_PDPT_ਸੂਚਕਾਂਕ(vptr);
    ret.pdpt_ਥਾਂ = pdpt + ਸੂਚਕਾਂਕ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸਥਿਰ lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ਜੇ (!ept_pdpte_ptr_get_read(lu_ret.pdpt_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);

        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ept_pde_t *pd = paddr_ਮੰਜ਼ਿਲ_pptr(ept_pdpte_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(lu_ret.pdpt_ਥਾਂ));
    uint32_t ਸੂਚਕਾਂਕ = GET_EPT_PD_ਸੂਚਕਾਂਕ(vptr);
    ret.pd_ਥਾਂ = pd + ਸੂਚਕਾਂਕ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸਥਿਰ lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ਜੇ ((ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ);

        ret.pt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ept_pte_t *pt = paddr_ਮੰਜ਼ਿਲ_pptr(ept_pde_ept_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(lu_ret.pd_ਥਾਂ));
    uint32_t ਸੂਚਕਾਂਕ = GET_EPT_PT_ਸੂਚਕਾਂਕ(vptr);

    ret.pt_ਥਾਂ = pt + ਸੂਚਕਾਂਕ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸਥਿਰ ept_cache_options_t eptCacheFromVmAttr(vm_ਗੁਣ_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.ਸ਼ਬਦ[0];
    ਜੇ (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_ਪਿੱਠ) {
        option = EPTWrite_ਪਿੱਠ;
    }
    ਵਾਪਸ option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    ਲੱਭੋ_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_ਥਾਂ;

    asid_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (asid_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pml4 = ਨਿਰਾ;
        ret.pml4_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = asid_ret.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }

    pml4_ਥਾਂ = lookup_EPTPML4_ਥਾਂ(asid_ret.ept, vptr);

    ਜੇ (ept_pml4e_ptr_get_read(pml4_ਥਾਂ)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_ਥਾਂ = pml4_ਥਾਂ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        ret.pml4 = ਨਿਰਾ;
        ret.pml4_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }
}

ਖਾਲੀ unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    ਜੇ (lu_ret.ਸਥਿਤੀ == EXCEPTION_NONE) {
        *lu_ret.pml4_ਥਾਂ = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ਸਥਿਰ exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ਜੇ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(cap),
            pdpt);
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pdpt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }
    cap_ept_pdpt_cap_ptr_ਸੈੱਟ_cap_PDPTIs_Mapped(&(cte->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_ਥਾਂ,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_ਥਾਂ = pml4e;
    invept(pml4);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeX86EPTPDPTInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਸ਼ਬਦ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret;
    ept_pml4e_t    *pml4_ਥਾਂ;

    ਜੇ (invLabel == X86EPTPDPTUnmap) {
        ਜੇ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performEPTPDPTInvocationUnmap(cap, cte);
    }

    ਜੇ (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4_ਥਾਂ = lookup_EPTPML4_ਥਾਂ(pml4, vaddr);

    ਜੇ (ept_pml4e_ptr_get_read(pml4_ਥਾਂ)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_ਸੈੱਟ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_ਸੈੱਟ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_ਸੈੱਟ_cap_PDPTMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_ਥਾਂ, pml4);
}

exception_t decodeX86EPTInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ept_pdpt_cap:
        ਵਾਪਸ decodeX86EPTPDPTInvocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    ਹਾਲਤ cap_ept_pd_cap:
        ਵਾਪਸ decodeX86EPTPDInvocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    ਹਾਲਤ cap_ept_pt_cap:
        ਵਾਪਸ decodeX86EPTPTInvocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    ਮੂਲ:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t ਲੱਭੋ_ret;
    ਲੱਭੋ_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (asid_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pml4 = ਨਿਰਾ;
        ret.pdpt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = asid_ret.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }

    ਲੱਭੋ_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pml4 = ਨਿਰਾ;
        ret.pdpt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = ਲੱਭੋ_ret.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }

    ਜੇ (ept_pdpte_ptr_get_read(ਲੱਭੋ_ret.pdpt_ਥਾਂ)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(ਲੱਭੋ_ret.pdpt_ਥਾਂ)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_ਥਾਂ = ਲੱਭੋ_ret.pdpt_ਥਾਂ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        ret.pml4 = ਨਿਰਾ;
        ret.pdpt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }
}

ਖਾਲੀ unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    ਜੇ (lu_ret.ਸਥਿਤੀ == EXCEPTION_NONE) {
        *lu_ret.pdpt_ਥਾਂ = ept_pdpte_new(
                               0,  /* pd_base_address  */
                               0,  /* avl_cte_depth    */
                               0,  /* execute          */
                               0,  /* write            */
                               0   /* read             */
                           );
        invept(lu_ret.pml4);
    }
}

ਸਥਿਰ exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    ਜੇ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_ਪਤਾ(cap),
            pd);
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pd, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }
    cap_ept_pd_cap_ptr_ਸੈੱਟ_cap_PDIs_Mapped(&(cte->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_ਥਾਂ,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_ਥਾਂ = pdpte;
    invept(pml4);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਸ਼ਬਦ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    ਜੇ (invLabel == X86EPTPDUnmap) {
        ਜੇ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performEPTPDInvocationUnmap(cap, cte);
    }

    ਜੇ (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ);
    vaddr = vaddr & ~ਨਕਾਬ(EPT_PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ept_pdpte_ptr_get_read(lu_ret.pdpt_ਥਾਂ)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_ਸੈੱਟ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_ਸੈੱਟ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_ਸੈੱਟ_cap_PDMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_ਥਾਂ, pml4);
}

EPTPage_ਸਾਰਣੀ_Mapped_ret_t EPTPage_ਸਾਰਣੀ_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_ਸਾਰਣੀ_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t ਲੱਭੋ_ret;
    ਲੱਭੋ_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (asid_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pml4 = ਨਿਰਾ;
        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = asid_ret.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }

    ਲੱਭੋ_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pml4 = ਨਿਰਾ;
        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = ਲੱਭੋ_ret.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }

    ਜੇ (ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(ਲੱਭੋ_ret.pd_ਥਾਂ) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(ਲੱਭੋ_ret.pd_ਥਾਂ)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_ਥਾਂ = ਲੱਭੋ_ret.pd_ਥਾਂ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        ret.pml4 = ਨਿਰਾ;
        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }
}

ਖਾਲੀ unmap_EPTPage_ਸਾਰਣੀ(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_ਸਾਰਣੀ_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_ਸਾਰਣੀ_Mapped(asid, vaddr, pt);

    ਜੇ (lu_ret.ਸਥਿਤੀ == EXCEPTION_NONE) {
        *lu_ret.pd_ਥਾਂ = ept_pde_ept_pde_pt_new(
                             0,  /* pt_base_address  */
                             0,  /* avl_cte_depth    */
                             0,  /* execute          */
                             0,  /* write            */
                             0   /* read             */
                         );
        invept(lu_ret.pml4);
    }
}

ਸਥਿਰ exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ਜੇ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_ਸਾਰਣੀ(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_ਪਤਾ(cap),
            pt);
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }
    cap_ept_pt_cap_ptr_ਸੈੱਟ_cap_PTIs_Mapped(&(cte->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_ਥਾਂ, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_ਥਾਂ = pde;
    invept(pml4);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਸ਼ਬਦ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    ਜੇ (invLabel == X86EPTPTUnmap) {
        ਜੇ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performEPTPTInvocationUnmap(cap, cte);
    }

    ਜੇ (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ);
    vaddr = vaddr & ~ਨਕਾਬ(EPT_PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (((ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ਥਾਂ)) ||
        ((ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ਥਾਂ))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_ਥਾਂ, pml4);
}

ਸਥਿਰ exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_ਥਾਂ, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_ਥਾਂ = pte;
    cte->cap = cap;
    invept(pml4);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_ਥਾਂ, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_ਥਾਂ[0] = pde1;
    ਜੇ (LARGE_ਪੰਨਾ_ਬਿੱਟ == 22) {
        pd_ਥਾਂ[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t          vaddr;
    ਸ਼ਬਦ_t          w_rights_ਨਕਾਬ;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_ਗੁਣ_t vmAttr;
    vm_ਪੰਨਾ_ਮਾਪ_t  ਚੌਖਟਾ_ਮਾਪ;
    asid_t          asid;

    ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, ਬਫਰ);
    vaddr = vaddr & ~ਨਕਾਬ(EPT_PT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
    w_rights_ਨਕਾਬ = getSyscallArg(1, ਬਫਰ);
    vmAttr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(2, ਬਫਰ));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);

    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    ਜੇ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = ਨਕਾਬ_VMRights(capVMRights, rights_From_ਸ਼ਬਦ(w_rights_ਨਕਾਬ));

    ਜੇ (!checkVPAlignment(ਚੌਖਟਾ_ਮਾਪ, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asid);
    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, vaddr);
    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(cap, X86_MappingEPT);

    ਚੋਣ (ਚੌਖਟਾ_ਮਾਪ) {
    /* PTE mappings */
    ਹਾਲਤ X86_Small_ਪੰਨਾ: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (ept_pte_ptr_get_read(lu_ret.pt_ਥਾਂ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performEPTPageMapPTE(cap, cte, lu_ret.pt_ਥਾਂ, pte, pml4);
    }

    /* PDE mappings */
    ਹਾਲਤ X86_Large_ਪੰਨਾ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }


        ਜੇ ((ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ਥਾਂ)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (LARGE_ਪੰਨਾ_ਬਿੱਟ != EPT_PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ &&
            (ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ਥਾਂ + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ ((ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ਥਾਂ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
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
                             paddr + ਦੋਆਧਾਰੀ_ਅੰਕ(EPT_PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performEPTPageMapPDE(cap, cte, lu_ret.pd_ਥਾਂ, pde1, pde2, pml4);
    }

    ਮੂਲ:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਖਾਲੀ unmapEPTPage(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, asid_t asid, vptr_t vptr, ਖਾਲੀ *pptr)
{
    ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ X86_Small_ਪੰਨਾ: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(ਲੱਭੋ_ret.ept, vptr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ;
        }
        ਜੇ (!ept_pte_ptr_get_read(lu_ret.pt_ਥਾਂ)) {
            ਵਾਪਸ;
        }
        ਜੇ (ept_pte_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(lu_ret.pt_ਥਾਂ) != addr) {
            ਵਾਪਸ;
        }

        *lu_ret.pt_ਥਾਂ = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        ਰੋਕੋ;
    }
    ਹਾਲਤ X86_Large_ਪੰਨਾ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(ਲੱਭੋ_ret.ept, vptr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ;
        }
        ਜੇ (ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) != ept_pde_ept_pde_2m) {
            ਵਾਪਸ;
        }
        ਜੇ (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ਥਾਂ)) {
            ਵਾਪਸ;
        }
        ਜੇ (ept_pde_ept_pde_2m_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(lu_ret.pd_ਥਾਂ) != addr) {
            ਵਾਪਸ;
        }

        lu_ret.pd_ਥਾਂ[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        ਜੇ (LARGE_ਪੰਨਾ_ਬਿੱਟ != EPT_PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ) {
            ਪ੍ਰਤਿਜ੍ਞਾ(ept_pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ + 1) == ept_pde_ept_pde_2m);
            ਪ੍ਰਤਿਜ੍ਞਾ(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ਥਾਂ + 1));
            ਪ੍ਰਤਿਜ੍ਞਾ(ept_pde_ept_pde_2m_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(lu_ret.pd_ਥਾਂ + 1) == addr + ਦੋਆਧਾਰੀ_ਅੰਕ(21));

            lu_ret.pd_ਥਾਂ[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        ਪ੍ਰਤਿਜ੍ਞਾ(!"Invalid page size for unmap");
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_VTX */
