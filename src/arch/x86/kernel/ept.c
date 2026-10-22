/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX

#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/ept.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>

ರಚನೆ lookupEPTPDPTSlot_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ept_pdpte_t *pdpt_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

ರಚನೆ lookupEPTPDSlot_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ept_pde_t  *pd_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

ರಚನೆ lookupEPTPTSlot_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ept_pte_t  *pt_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

ಎಣಿಕೆ ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_ಹಿಂಭಾಗ = 6
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ept_cache_options ept_cache_options_t;

ಶೂನ್ಯ deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (poolPtr != ಖಾಲಿ) {
        asid_map_t asid_map = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
        ಆದರೆ (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ಬೇರು(asid_map) == ept) {
            poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    unmapEPTPage(
        cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
        cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
        cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
        (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
    );

    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMapped_ವಿಳಾಸ(&ct_ಸ್ಥಾನ->cap, 0);
    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMapped_ASID(&ct_ಸ್ಥಾನ->cap, asidInvalid);
    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMap_Type(&ct_ಸ್ಥಾನ->cap, X86_MappingNone);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid_t asid)
{
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.ept = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ಬೇರು(asid_map);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ept_pml4e_t *CONST lookup_EPTPML4_ಸ್ಥಾನ(ept_pml4e_t *pml4, vptr_t vptr)
{
    ಹಿಂತಿರುಗಿ pml4 + GET_EPT_PML4_ಸೂಚ್ಯಂಕ(vptr);
}

ಸ್ಥಿರ lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_ಸ್ಥಾನ;

    pml4_ಸ್ಥಾನ = lookup_EPTPML4_ಸ್ಥಾನ(pml4, vptr);

    ಆದರೆ (!ept_pml4e_ptr_get_read(pml4_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);

        ret.pdpt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ept_pdpte_t *pdpt = paddr_ಗಮ್ಯ_pptr(ept_pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ));
    uint32_t ಸೂಚ್ಯಂಕ = GET_EPT_PDPT_ಸೂಚ್ಯಂಕ(vptr);
    ret.pdpt_ಸ್ಥಾನ = pdpt + ಸೂಚ್ಯಂಕ;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (!ept_pdpte_ptr_get_read(lu_ret.pdpt_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);

        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ept_pde_t *pd = paddr_ಗಮ್ಯ_pptr(ept_pdpte_ptr_get_pd_ತಳ_ವಿಳಾಸ(lu_ret.pdpt_ಸ್ಥಾನ));
    uint32_t ಸೂಚ್ಯಂಕ = GET_EPT_PD_ಸೂಚ್ಯಂಕ(vptr);
    ret.pd_ಸ್ಥಾನ = pd + ಸೂಚ್ಯಂಕ;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ ((ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);

        ret.pt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ept_pte_t *pt = paddr_ಗಮ್ಯ_pptr(ept_pde_ept_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(lu_ret.pd_ಸ್ಥಾನ));
    uint32_t ಸೂಚ್ಯಂಕ = GET_EPT_PT_ಸೂಚ್ಯಂಕ(vptr);

    ret.pt_ಸ್ಥಾನ = pt + ಸೂಚ್ಯಂಕ;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ept_cache_options_t eptCacheFromVmAttr(vm_ಗುಣಲಕ್ಷಣಗಳು_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.ಪದಗಳು[0];
    ಆದರೆ (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_ಹಿಂಭಾಗ) {
        option = EPTWrite_ಹಿಂಭಾಗ;
    }
    ಹಿಂತಿರುಗಿ option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_ಸ್ಥಾನ;

    asid_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (asid_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pml4 = ಖಾಲಿ;
        ret.pml4_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = asid_ret.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }

    pml4_ಸ್ಥಾನ = lookup_EPTPML4_ಸ್ಥಾನ(asid_ret.ept, vptr);

    ಆದರೆ (ept_pml4e_ptr_get_read(pml4_ಸ್ಥಾನ)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_ಸ್ಥಾನ = pml4_ಸ್ಥಾನ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ret.pml4 = ಖಾಲಿ;
        ret.pml4_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }
}

ಶೂನ್ಯ unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
        *lu_ret.pml4_ಸ್ಥಾನ = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ಸ್ಥಿರ exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ಆದರೆ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(cap),
            pdpt);
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pdpt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }
    cap_ept_pdpt_cap_ptr_ಗಣ_cap_PDPTIs_Mapped(&(cte->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_ಸ್ಥಾನ,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_ಸ್ಥಾನ = pml4e;
    invept(pml4);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeX86EPTPDPTInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಶಬ್ದ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    ept_pml4e_t    *pml4_ಸ್ಥಾನ;

    ಆದರೆ (invLabel == X86EPTPDPTUnmap) {
        ಆದರೆ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performEPTPDPTInvocationUnmap(cap, cte);
    }

    ಆದರೆ (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4_ಸ್ಥಾನ = lookup_EPTPML4_ಸ್ಥಾನ(pml4, vaddr);

    ಆದರೆ (ept_pml4e_ptr_get_read(pml4_ಸ್ಥಾನ)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_ಗಣ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_ಗಣ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_ಗಣ_cap_PDPTMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_ಸ್ಥಾನ, pml4);
}

exception_t decodeX86EPTInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ept_pdpt_cap:
        ಹಿಂತಿರುಗಿ decodeX86EPTPDPTInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    ಸ್ಥಿತಿ cap_ept_pd_cap:
        ಹಿಂತಿರುಗಿ decodeX86EPTPDInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    ಸ್ಥಿತಿ cap_ept_pt_cap:
        ಹಿಂತಿರುಗಿ decodeX86EPTPTInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t ಕಂಡುಹಿಡಿ_ret;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (asid_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pml4 = ಖಾಲಿ;
        ret.pdpt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = asid_ret.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಕಂಡುಹಿಡಿ_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pml4 = ಖಾಲಿ;
        ret.pdpt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (ept_pdpte_ptr_get_read(ಕಂಡುಹಿಡಿ_ret.pdpt_ಸ್ಥಾನ)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_ತಳ_ವಿಳಾಸ(ಕಂಡುಹಿಡಿ_ret.pdpt_ಸ್ಥಾನ)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_ಸ್ಥಾನ = ಕಂಡುಹಿಡಿ_ret.pdpt_ಸ್ಥಾನ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ret.pml4 = ಖಾಲಿ;
        ret.pdpt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }
}

ಶೂನ್ಯ unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
        *lu_ret.pdpt_ಸ್ಥಾನ = ept_pdpte_new(
                               0,  /* pd_base_address  */
                               0,  /* avl_cte_depth    */
                               0,  /* execute          */
                               0,  /* write            */
                               0   /* read             */
                           );
        invept(lu_ret.pml4);
    }
}

ಸ್ಥಿರ exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    ಆದರೆ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_ವಿಳಾಸ(cap),
            pd);
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pd, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }
    cap_ept_pd_cap_ptr_ಗಣ_cap_PDIs_Mapped(&(cte->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_ಸ್ಥಾನ,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_ಸ್ಥಾನ = pdpte;
    invept(pml4);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಶಬ್ದ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    ಆದರೆ (invLabel == X86EPTPDUnmap) {
        ಆದರೆ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performEPTPDInvocationUnmap(cap, cte);
    }

    ಆದರೆ (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್);
    vaddr = vaddr & ~ಮುಖವಾಡ(EPT_PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ept_pdpte_ptr_get_read(lu_ret.pdpt_ಸ್ಥಾನ)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_ಗಣ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_ಗಣ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_ಗಣ_cap_PDMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_ಸ್ಥಾನ, pml4);
}

EPTPage_ಕೋಷ್ಟಕ_Mapped_ret_t EPTPage_ಕೋಷ್ಟಕ_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_ಕೋಷ್ಟಕ_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t ಕಂಡುಹಿಡಿ_ret;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (asid_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pml4 = ಖಾಲಿ;
        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = asid_ret.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಕಂಡುಹಿಡಿ_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pml4 = ಖಾಲಿ;
        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(ಕಂಡುಹಿಡಿ_ret.pd_ಸ್ಥಾನ) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(ಕಂಡುಹಿಡಿ_ret.pd_ಸ್ಥಾನ)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_ಸ್ಥಾನ = ಕಂಡುಹಿಡಿ_ret.pd_ಸ್ಥಾನ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ret.pml4 = ಖಾಲಿ;
        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }
}

ಶೂನ್ಯ unmap_EPTPage_ಕೋಷ್ಟಕ(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_ಕೋಷ್ಟಕ_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_ಕೋಷ್ಟಕ_Mapped(asid, vaddr, pt);

    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
        *lu_ret.pd_ಸ್ಥಾನ = ept_pde_ept_pde_pt_new(
                             0,  /* pt_base_address  */
                             0,  /* avl_cte_depth    */
                             0,  /* execute          */
                             0,  /* write            */
                             0   /* read             */
                         );
        invept(lu_ret.pml4);
    }
}

ಸ್ಥಿರ exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ಆದರೆ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_ಕೋಷ್ಟಕ(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
            pt);
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }
    cap_ept_pt_cap_ptr_ಗಣ_cap_PTIs_Mapped(&(cte->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_ಸ್ಥಾನ, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_ಸ್ಥಾನ = pde;
    invept(pml4);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಶಬ್ದ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    ಆದರೆ (invLabel == X86EPTPTUnmap) {
        ಆದರೆ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performEPTPTInvocationUnmap(cap, cte);
    }

    ಆದರೆ (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್);
    vaddr = vaddr & ~ಮುಖವಾಡ(EPT_PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (((ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ)) ||
        ((ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_ಸ್ಥಾನ, pml4);
}

ಸ್ಥಿರ exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_ಸ್ಥಾನ, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_ಸ್ಥಾನ = pte;
    cte->cap = cap;
    invept(pml4);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_ಸ್ಥಾನ, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_ಸ್ಥಾನ[0] = pde1;
    ಆದರೆ (LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು == 22) {
        pd_ಸ್ಥಾನ[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t          vaddr;
    ಶಬ್ದ_t          w_rights_ಮುಖವಾಡ;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_ಗುಣಲಕ್ಷಣಗಳು_t vmAttr;
    vm_ಪುಟ_ಗಾತ್ರ_t  ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    asid_t          asid;

    ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, ಬಫರ್);
    vaddr = vaddr & ~ಮುಖವಾಡ(EPT_PT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
    w_rights_ಮುಖವಾಡ = getSyscallArg(1, ಬಫರ್);
    vmAttr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(2, ಬಫರ್));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);

    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    ಆದರೆ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = ಮುಖವಾಡ_VMRights(capVMRights, rights_From_ಶಬ್ದ(w_rights_ಮುಖವಾಡ));

    ಆದರೆ (!checkVPAlignment(ಚೌಕಟ್ಟು_ಗಾತ್ರ, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asid);
    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, vaddr);
    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(cap, X86_MappingEPT);

    ಆಯ್ಕೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ) {
    /* PTE mappings */
    ಸ್ಥಿತಿ X86_Small_ಪುಟ: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (ept_pte_ptr_get_read(lu_ret.pt_ಸ್ಥಾನ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performEPTPageMapPTE(cap, cte, lu_ret.pt_ಸ್ಥಾನ, pte, pml4);
    }

    /* PDE mappings */
    ಸ್ಥಿತಿ X86_Large_ಪುಟ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }


        ಆದರೆ ((ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು != EPT_PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ &&
            (ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ ((ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
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
                             paddr + ಬಿಟ್(EPT_PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performEPTPageMapPDE(cap, cte, lu_ret.pd_ಸ್ಥಾನ, pde1, pde2, pml4);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ unmapEPTPage(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, asid_t asid, vptr_t vptr, ಶೂನ್ಯ *pptr)
{
    ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ X86_Small_ಪುಟ: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(ಕಂಡುಹಿಡಿ_ret.ept, vptr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (!ept_pte_ptr_get_read(lu_ret.pt_ಸ್ಥಾನ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (ept_pte_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(lu_ret.pt_ಸ್ಥಾನ) != addr) {
            ಹಿಂತಿರುಗಿ;
        }

        *lu_ret.pt_ಸ್ಥಾನ = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ X86_Large_ಪುಟ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(ಕಂಡುಹಿಡಿ_ret.ept, vptr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) != ept_pde_ept_pde_2m) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (ept_pde_ept_pde_2m_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(lu_ret.pd_ಸ್ಥಾನ) != addr) {
            ಹಿಂತಿರುಗಿ;
        }

        lu_ret.pd_ಸ್ಥಾನ[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        ಆದರೆ (LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು != EPT_PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ) {
            ಪ್ರತಿಜ್ಞಾ(ept_pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ + 1) == ept_pde_ept_pde_2m);
            ಪ್ರತಿಜ್ಞಾ(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_ಸ್ಥಾನ + 1));
            ಪ್ರತಿಜ್ಞಾ(ept_pde_ept_pde_2m_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(lu_ret.pd_ಸ್ಥಾನ + 1) == addr + ಬಿಟ್(21));

            lu_ret.pd_ಸ್ಥಾನ[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        ಪ್ರತಿಜ್ಞಾ(!"Invalid page size for unmap");
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_VTX */
