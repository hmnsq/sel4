/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_VTX

#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/kernel/ept.h>
#అన్తర్భావ <arch/api/invocation.h>

నిర్మాణం lookupEPTPDPTSlot_ret {
    exception_t స్థితి_2;
    ept_pdpte_t *pdpt_స్థానం;
};
రకనిర్వచనం నిర్మాణం lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

నిర్మాణం lookupEPTPDSlot_ret {
    exception_t స్థితి_2;
    ept_pde_t  *pd_స్థానం;
};
రకనిర్వచనం నిర్మాణం lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

నిర్మాణం lookupEPTPTSlot_ret {
    exception_t స్థితి_2;
    ept_pte_t  *pt_స్థానం;
};
రకనిర్వచనం నిర్మాణం lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

గణన ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_వెనుక = 6
};
రకనిర్వచనం గణన ept_cache_options ept_cache_options_t;

శూన్య deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (poolPtr != ఖాళీ) {
        asid_map_t asid_map = poolPtr->array[ASID_దిగువ(asid)];
        ఒకవేళ (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_మూలం(asid_map) == ept) {
            poolPtr->array[ASID_దిగువ(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_స్థానం)
{
    unmapEPTPage(
        cap_చట్రం_cap_get_cap_FSize(cap),
        cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
        cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
        (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
    );

    cap_చట్రం_cap_ptr_సమితి_cap_FMapped_చిరునామా(&ct_స్థానం->cap, 0);
    cap_చట్రం_cap_ptr_సమితి_cap_FMapped_ASID(&ct_స్థానం->cap, asidInvalid);
    cap_చట్రం_cap_ptr_సమితి_cap_FMap_Type(&ct_స్థానం->cap, X86_MappingNone);

    తిరిగి EXCEPTION_NONE;
}

కనుగొను_EPTFor_ASID_ret_t కనుగొను_EPTFor_ASID(asid_t asid)
{
    కనుగొను_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.ept = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_మూలం(asid_map);
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

స్థిర ept_pml4e_t *CONST lookup_EPTPML4_స్థానం(ept_pml4e_t *pml4, vptr_t vptr)
{
    తిరిగి pml4 + GET_EPT_PML4_సూచీ(vptr);
}

స్థిర lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_స్థానం;

    pml4_స్థానం = lookup_EPTPML4_స్థానం(pml4, vptr);

    ఒకవేళ (!ept_pml4e_ptr_get_read(pml4_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_సూచీ_స్థానభ్రంశం);

        ret.pdpt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ept_pdpte_t *pdpt = paddr_గమ్యం_pptr(ept_pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం));
    uint32_t సూచీ = GET_EPT_PDPT_సూచీ(vptr);
    ret.pdpt_స్థానం = pdpt + సూచీ;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

స్థిర lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ఒకవేళ (!ept_pdpte_ptr_get_read(lu_ret.pdpt_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_సూచీ_స్థానభ్రంశం);

        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ept_pde_t *pd = paddr_గమ్యం_pptr(ept_pdpte_ptr_get_pd_పునాది_చిరునామా(lu_ret.pdpt_స్థానం));
    uint32_t సూచీ = GET_EPT_PD_సూచీ(vptr);
    ret.pd_స్థానం = pd + సూచీ;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

స్థిర lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ఒకవేళ ((ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_సూచీ_స్థానభ్రంశం);

        ret.pt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ept_pte_t *pt = paddr_గమ్యం_pptr(ept_pde_ept_pde_pt_ptr_get_pt_పునాది_చిరునామా(lu_ret.pd_స్థానం));
    uint32_t సూచీ = GET_EPT_PT_సూచీ(vptr);

    ret.pt_స్థానం = pt + సూచీ;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

స్థిర ept_cache_options_t eptCacheFromVmAttr(vm_లక్షణాలు_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.పదాలు[0];
    ఒకవేళ (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_వెనుక) {
        option = EPTWrite_వెనుక;
    }
    తిరిగి option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    కనుగొను_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_స్థానం;

    asid_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (asid_ret.స్థితి_2 != EXCEPTION_NONE) {
        ret.pml4 = ఖాళీ;
        ret.pml4_స్థానం = ఖాళీ;
        ret.స్థితి_2 = asid_ret.స్థితి_2;
        తిరిగి ret;
    }

    pml4_స్థానం = lookup_EPTPML4_స్థానం(asid_ret.ept, vptr);

    ఒకవేళ (ept_pml4e_ptr_get_read(pml4_స్థానం)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_స్థానం = pml4_స్థానం;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    } లేకపోతే {
        ret.pml4 = ఖాళీ;
        ret.pml4_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }
}

శూన్య unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    ఒకవేళ (lu_ret.స్థితి_2 == EXCEPTION_NONE) {
        *lu_ret.pml4_స్థానం = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

స్థిర exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ఒకవేళ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_చిరునామా(cap),
            pdpt);
        clear_జ్ఞాపకం((శూన్య *)pdpt, cap_get_cap_కొలత_బిట్లు(cap));
    }
    cap_ept_pdpt_cap_ptr_సమితి_cap_PDPTIs_Mapped(&(cte->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_స్థానం,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_స్థానం = pml4e;
    invept(pml4);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeX86EPTPDPTInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు
)
{
    మాట_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret;
    ept_pml4e_t    *pml4_స్థానం;

    ఒకవేళ (invLabel == X86EPTPDPTUnmap) {
        ఒకవేళ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performEPTPDPTInvocationUnmap(cap, cte);
    }

    ఒకవేళ (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_సూచీ_స్థానభ్రంశం) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    కనుగొను_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4_స్థానం = lookup_EPTPML4_స్థానం(pml4, vaddr);

    ఒకవేళ (ept_pml4e_ptr_get_read(pml4_స్థానం)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr((శూన్య *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_సమితి_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_సమితి_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_సమితి_cap_PDPTMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_స్థానం, pml4);
}

exception_t decodeX86EPTInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_ept_pdpt_cap:
        తిరిగి decodeX86EPTPDPTInvocation(invLabel, నిడివి, cte, cap, బఫరు);
    స్థితి cap_ept_pd_cap:
        తిరిగి decodeX86EPTPDInvocation(invLabel, నిడివి, cte, cap, బఫరు);
    స్థితి cap_ept_pt_cap:
        తిరిగి decodeX86EPTPTInvocation(invLabel, నిడివి, cte, cap, బఫరు);
    అప్రమేయ:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t కనుగొను_ret;
    కనుగొను_EPTFor_ASID_ret_t asid_ret;

    asid_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (asid_ret.స్థితి_2 != EXCEPTION_NONE) {
        ret.pml4 = ఖాళీ;
        ret.pdpt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = asid_ret.స్థితి_2;
        తిరిగి ret;
    }

    కనుగొను_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        ret.pml4 = ఖాళీ;
        ret.pdpt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = కనుగొను_ret.స్థితి_2;
        తిరిగి ret;
    }

    ఒకవేళ (ept_pdpte_ptr_get_read(కనుగొను_ret.pdpt_స్థానం)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_పునాది_చిరునామా(కనుగొను_ret.pdpt_స్థానం)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_స్థానం = కనుగొను_ret.pdpt_స్థానం;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    } లేకపోతే {
        ret.pml4 = ఖాళీ;
        ret.pdpt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }
}

శూన్య unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    ఒకవేళ (lu_ret.స్థితి_2 == EXCEPTION_NONE) {
        *lu_ret.pdpt_స్థానం = ept_pdpte_new(
                               0,  /* pd_base_address  */
                               0,  /* avl_cte_depth    */
                               0,  /* execute          */
                               0,  /* write            */
                               0   /* read             */
                           );
        invept(lu_ret.pml4);
    }
}

స్థిర exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    ఒకవేళ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_చిరునామా(cap),
            pd);
        clear_జ్ఞాపకం((శూన్య *)pd, cap_get_cap_కొలత_బిట్లు(cap));
    }
    cap_ept_pd_cap_ptr_సమితి_cap_PDIs_Mapped(&(cte->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_స్థానం,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_స్థానం = pdpte;
    invept(pml4);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు
)
{
    మాట_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    ఒకవేళ (invLabel == X86EPTPDUnmap) {
        ఒకవేళ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performEPTPDInvocationUnmap(cap, cte);
    }

    ఒకవేళ (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు);
    vaddr = vaddr & ~ముసుగు(EPT_PDPT_సూచీ_స్థానభ్రంశం);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    కనుగొను_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (ept_pdpte_ptr_get_read(lu_ret.pdpt_స్థానం)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr((శూన్య *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_సమితి_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_సమితి_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_సమితి_cap_PDMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_స్థానం, pml4);
}

EPTPage_పట్టిక_Mapped_ret_t EPTPage_పట్టిక_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_పట్టిక_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t కనుగొను_ret;
    కనుగొను_EPTFor_ASID_ret_t asid_ret;

    asid_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (asid_ret.స్థితి_2 != EXCEPTION_NONE) {
        ret.pml4 = ఖాళీ;
        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = asid_ret.స్థితి_2;
        తిరిగి ret;
    }

    కనుగొను_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        ret.pml4 = ఖాళీ;
        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = కనుగొను_ret.స్థితి_2;
        తిరిగి ret;
    }

    ఒకవేళ (ept_pde_ptr_get_పుట_కొలత(కనుగొను_ret.pd_స్థానం) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_పునాది_చిరునామా(కనుగొను_ret.pd_స్థానం)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_స్థానం = కనుగొను_ret.pd_స్థానం;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    } లేకపోతే {
        ret.pml4 = ఖాళీ;
        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }
}

శూన్య unmap_EPTPage_పట్టిక(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_పట్టిక_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_పట్టిక_Mapped(asid, vaddr, pt);

    ఒకవేళ (lu_ret.స్థితి_2 == EXCEPTION_NONE) {
        *lu_ret.pd_స్థానం = ept_pde_ept_pde_pt_new(
                             0,  /* pt_base_address  */
                             0,  /* avl_cte_depth    */
                             0,  /* execute          */
                             0,  /* write            */
                             0   /* read             */
                         );
        invept(lu_ret.pml4);
    }
}

స్థిర exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    ఒకవేళ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_పట్టిక(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_చిరునామా(cap),
            pt);
        clear_జ్ఞాపకం((శూన్య *)pt, cap_get_cap_కొలత_బిట్లు(cap));
    }
    cap_ept_pt_cap_ptr_సమితి_cap_PTIs_Mapped(&(cte->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_స్థానం, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_స్థానం = pde;
    invept(pml4);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు
)
{
    మాట_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    ఒకవేళ (invLabel == X86EPTPTUnmap) {
        ఒకవేళ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performEPTPTInvocationUnmap(cap, cte);
    }

    ఒకవేళ (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు);
    vaddr = vaddr & ~ముసుగు(EPT_PD_సూచీ_స్థానభ్రంశం);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    కనుగొను_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (((ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_స్థానం)) ||
        ((ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_స్థానం))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr((శూన్య *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_సమితి_cap_PTMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_స్థానం, pml4);
}

స్థిర exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_స్థానం, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_స్థానం = pte;
    cte->cap = cap;
    invept(pml4);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_స్థానం, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_స్థానం[0] = pde1;
    ఒకవేళ (LARGE_పుట_బిట్లు == 22) {
        pd_స్థానం[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు)
{
    మాట_t          vaddr;
    మాట_t          w_rights_ముసుగు;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_లక్షణాలు_t vmAttr;
    vm_పుట_కొలత_t  చట్రం_కొలత;
    asid_t          asid;

    చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, బఫరు);
    vaddr = vaddr & ~ముసుగు(EPT_PT_సూచీ_స్థానభ్రంశం);
    w_rights_ముసుగు = getSyscallArg(1, బఫరు);
    vmAttr = vm_లక్షణాలు_From_మాట(getSyscallArg(2, బఫరు));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_చట్రం_cap_get_cap_FVMRights(cap);

    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    ఒకవేళ (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = ముసుగు_VMRights(capVMRights, rights_From_మాట(w_rights_ముసుగు));

    ఒకవేళ (!checkVPAlignment(చట్రం_కొలత, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap));

    cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asid);
    cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, vaddr);
    cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(cap, X86_MappingEPT);

    ఎంపిక (చట్రం_కొలత) {
    /* PTE mappings */
    స్థితి X86_Small_పుట: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (ept_pte_ptr_get_read(lu_ret.pt_స్థానం)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performEPTPageMapPTE(cap, cte, lu_ret.pt_స్థానం, pte, pml4);
    }

    /* PDE mappings */
    స్థితి X86_Large_పుట: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }


        ఒకవేళ ((ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_స్థానం)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (LARGE_పుట_బిట్లు != EPT_PD_సూచీ_స్థానభ్రంశం &&
            (ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_స్థానం + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ ((ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_స్థానం)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
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
                             paddr + బిట్టు(EPT_PD_సూచీ_స్థానభ్రంశం),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performEPTPageMapPDE(cap, cte, lu_ret.pd_స్థానం, pde1, pde2, pml4);
    }

    అప్రమేయ:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

శూన్య unmapEPTPage(vm_పుట_కొలత_t పుట_కొలత, asid_t asid, vptr_t vptr, శూన్య *pptr)
{
    కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret;
    paddr_t addr = addrFromPPtr(pptr);

    కనుగొను_ret = కనుగొను_EPTFor_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    ఎంపిక (పుట_కొలత) {
    స్థితి X86_Small_పుట: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(కనుగొను_ret.ept, vptr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి;
        }
        ఒకవేళ (!ept_pte_ptr_get_read(lu_ret.pt_స్థానం)) {
            తిరిగి;
        }
        ఒకవేళ (ept_pte_ptr_get_పుట_పునాది_చిరునామా(lu_ret.pt_స్థానం) != addr) {
            తిరిగి;
        }

        *lu_ret.pt_స్థానం = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        ఆపు;
    }
    స్థితి X86_Large_పుట: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(కనుగొను_ret.ept, vptr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి;
        }
        ఒకవేళ (ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) != ept_pde_ept_pde_2m) {
            తిరిగి;
        }
        ఒకవేళ (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_స్థానం)) {
            తిరిగి;
        }
        ఒకవేళ (ept_pde_ept_pde_2m_ptr_get_పుట_పునాది_చిరునామా(lu_ret.pd_స్థానం) != addr) {
            తిరిగి;
        }

        lu_ret.pd_స్థానం[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        ఒకవేళ (LARGE_పుట_బిట్లు != EPT_PD_సూచీ_స్థానభ్రంశం) {
            ప్రతిజ్ఞా(ept_pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం + 1) == ept_pde_ept_pde_2m);
            ప్రతిజ్ఞా(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_స్థానం + 1));
            ప్రతిజ్ఞా(ept_pde_ept_pde_2m_ptr_get_పుట_పునాది_చిరునామా(lu_ret.pd_స్థానం + 1) == addr + బిట్టు(21));

            lu_ret.pd_స్థానం[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        ఆపు;
    }
    అప్రమేయ:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        ప్రతిజ్ఞా(!"Invalid page size for unmap");
    }
}

#అయితే_ముగింపు /* CONFIG_VTX */
