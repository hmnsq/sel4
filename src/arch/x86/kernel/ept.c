/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_VTX

#شامل <model/statedata.h>
#شامل <arch/kernel/ept.h>
#شامل <arch/api/invocation.h>

ساخت lookupEPTPDPTSlot_ret {
    exception_t حالت_2;
    ept_pdpte_t *pdpt_خانہ;
};
قسمتعریف ساخت lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

ساخت lookupEPTPDSlot_ret {
    exception_t حالت_2;
    ept_pde_t  *pd_خانہ;
};
قسمتعریف ساخت lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

ساخت lookupEPTPTSlot_ret {
    exception_t حالت_2;
    ept_pte_t  *pt_خانہ;
};
قسمتعریف ساخت lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

شمار ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_عقب = 6
};
قسمتعریف شمار ept_cache_options ept_cache_options_t;

خالی deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بلند(asid)];
    اگر (poolPtr != تہی) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        اگر (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_جڑ(asid_map) == ept) {
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_خانہ)
{
    unmapEPTPage(
        cap_چوکھٹا_cap_get_cap_FSize(cap),
        cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
        cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
        (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
    );

    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_پتہ(&ct_خانہ->cap, 0);
    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_ASID(&ct_خانہ->cap, asidInvalid);
    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMap_Type(&ct_خانہ->cap, X86_MappingNone);

    واپس EXCEPTION_NONE;
}

ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_EPTFor_ASID(asid_t asid)
{
    ڈھونڈو_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.ept = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_جڑ(asid_map);
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

ساکن ept_pml4e_t *CONST lookup_EPTPML4_خانہ(ept_pml4e_t *pml4, vptr_t vptr)
{
    واپس pml4 + GET_EPT_PML4_اشاریہ(vptr);
}

ساکن lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_خانہ;

    pml4_خانہ = lookup_EPTPML4_خانہ(pml4, vptr);

    اگر (!ept_pml4e_ptr_get_read(pml4_خانہ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_اشاریہ_انحراف);

        ret.pdpt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ept_pdpte_t *pdpt = paddr_مقصد_pptr(ept_pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ));
    uint32_t اشاریہ = GET_EPT_PDPT_اشاریہ(vptr);
    ret.pdpt_خانہ = pdpt + اشاریہ;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

ساکن lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    اگر (!ept_pdpte_ptr_get_read(lu_ret.pdpt_خانہ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_اشاریہ_انحراف);

        ret.pd_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ept_pde_t *pd = paddr_مقصد_pptr(ept_pdpte_ptr_get_pd_بنیاد_پتہ(lu_ret.pdpt_خانہ));
    uint32_t اشاریہ = GET_EPT_PD_اشاریہ(vptr);
    ret.pd_خانہ = pd + اشاریہ;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

ساکن lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    اگر ((ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خانہ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_اشاریہ_انحراف);

        ret.pt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ept_pte_t *pt = paddr_مقصد_pptr(ept_pde_ept_pde_pt_ptr_get_pt_بنیاد_پتہ(lu_ret.pd_خانہ));
    uint32_t اشاریہ = GET_EPT_PT_اشاریہ(vptr);

    ret.pt_خانہ = pt + اشاریہ;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

ساکن ept_cache_options_t eptCacheFromVmAttr(vm_اوصاف_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.الفاظ[0];
    اگر (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_عقب) {
        option = EPTWrite_عقب;
    }
    واپس option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    ڈھونڈو_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_خانہ;

    asid_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (asid_ret.حالت_2 != EXCEPTION_NONE) {
        ret.pml4 = تہی;
        ret.pml4_خانہ = تہی;
        ret.حالت_2 = asid_ret.حالت_2;
        واپس ret;
    }

    pml4_خانہ = lookup_EPTPML4_خانہ(asid_ret.ept, vptr);

    اگر (ept_pml4e_ptr_get_read(pml4_خانہ)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_خانہ = pml4_خانہ;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    } ورنہ {
        ret.pml4 = تہی;
        ret.pml4_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }
}

خالی unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    اگر (lu_ret.حالت_2 == EXCEPTION_NONE) {
        *lu_ret.pml4_خانہ = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ساکن exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    اگر (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
            pdpt);
        clear_حافظہ((خالی *)pdpt, cap_get_cap_سائز_بٹ(cap));
    }
    cap_ept_pdpt_cap_ptr_سیٹ_cap_PDPTIs_Mapped(&(cte->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_خانہ,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_خانہ = pml4e;
    invept(pml4);

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeX86EPTPDPTInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret;
    ept_pml4e_t    *pml4_خانہ;

    اگر (invLabel == X86EPTPDPTUnmap) {
        اگر (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performEPTPDPTInvocationUnmap(cap, cte);
    }

    اگر (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_اشاریہ_انحراف) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4_خانہ = lookup_EPTPML4_خانہ(pml4, vaddr);

    اگر (ept_pml4e_ptr_get_read(pml4_خانہ)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((خالی *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_سیٹ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_سیٹ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_سیٹ_cap_PDPTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_خانہ, pml4);
}

exception_t decodeX86EPTInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_ept_pdpt_cap:
        واپس decodeX86EPTPDPTInvocation(invLabel, لمبائی, cte, cap, بفر);
    حالت cap_ept_pd_cap:
        واپس decodeX86EPTPDInvocation(invLabel, لمبائی, cte, cap, بفر);
    حالت cap_ept_pt_cap:
        واپس decodeX86EPTPTInvocation(invLabel, لمبائی, cte, cap, بفر);
    طےشدہ:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t ڈھونڈو_ret;
    ڈھونڈو_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (asid_ret.حالت_2 != EXCEPTION_NONE) {
        ret.pml4 = تہی;
        ret.pdpt_خانہ = تہی;
        ret.حالت_2 = asid_ret.حالت_2;
        واپس ret;
    }

    ڈھونڈو_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        ret.pml4 = تہی;
        ret.pdpt_خانہ = تہی;
        ret.حالت_2 = ڈھونڈو_ret.حالت_2;
        واپس ret;
    }

    اگر (ept_pdpte_ptr_get_read(ڈھونڈو_ret.pdpt_خانہ)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_بنیاد_پتہ(ڈھونڈو_ret.pdpt_خانہ)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_خانہ = ڈھونڈو_ret.pdpt_خانہ;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    } ورنہ {
        ret.pml4 = تہی;
        ret.pdpt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }
}

خالی unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    اگر (lu_ret.حالت_2 == EXCEPTION_NONE) {
        *lu_ret.pdpt_خانہ = ept_pdpte_new(
                               0,  /* pd_base_address  */
                               0,  /* avl_cte_depth    */
                               0,  /* execute          */
                               0,  /* write            */
                               0   /* read             */
                           );
        invept(lu_ret.pml4);
    }
}

ساکن exception_t performEPTPDInvocationUnmap(cap_t cap, cte_t *cte)
{
    اگر (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        ept_pde_t *pd = (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap);
        unmapEPTPageDirectory(
            cap_ept_pd_cap_get_capPDMappedASID(cap),
            cap_ept_pd_cap_get_cap_PDMapped_پتہ(cap),
            pd);
        clear_حافظہ((خالی *)pd, cap_get_cap_سائز_بٹ(cap));
    }
    cap_ept_pd_cap_ptr_سیٹ_cap_PDIs_Mapped(&(cte->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_خانہ,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_خانہ = pdpte;
    invept(pml4);

    واپس EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    اگر (invLabel == X86EPTPDUnmap) {
        اگر (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performEPTPDInvocationUnmap(cap, cte);
    }

    اگر (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~نقاب(EPT_PDPT_اشاریہ_انحراف);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ept_pdpte_ptr_get_read(lu_ret.pdpt_خانہ)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((خالی *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_سیٹ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_سیٹ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_سیٹ_cap_PDMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_خانہ, pml4);
}

EPTPage_جدول_Mapped_ret_t EPTPage_جدول_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t ڈھونڈو_ret;
    ڈھونڈو_EPTFor_ASID_ret_t asid_ret;

    asid_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (asid_ret.حالت_2 != EXCEPTION_NONE) {
        ret.pml4 = تہی;
        ret.pd_خانہ = تہی;
        ret.حالت_2 = asid_ret.حالت_2;
        واپس ret;
    }

    ڈھونڈو_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        ret.pml4 = تہی;
        ret.pd_خانہ = تہی;
        ret.حالت_2 = ڈھونڈو_ret.حالت_2;
        واپس ret;
    }

    اگر (ept_pde_ptr_get_صفحہ_سائز(ڈھونڈو_ret.pd_خانہ) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_بنیاد_پتہ(ڈھونڈو_ret.pd_خانہ)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_خانہ = ڈھونڈو_ret.pd_خانہ;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    } ورنہ {
        ret.pml4 = تہی;
        ret.pd_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }
}

خالی unmap_EPTPage_جدول(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_جدول_Mapped(asid, vaddr, pt);

    اگر (lu_ret.حالت_2 == EXCEPTION_NONE) {
        *lu_ret.pd_خانہ = ept_pde_ept_pde_pt_new(
                             0,  /* pt_base_address  */
                             0,  /* avl_cte_depth    */
                             0,  /* execute          */
                             0,  /* write            */
                             0   /* read             */
                         );
        invept(lu_ret.pml4);
    }
}

ساکن exception_t performEPTPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    اگر (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        ept_pte_t *pt = (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap);
        unmap_EPTPage_جدول(
            cap_ept_pt_cap_get_capPTMappedASID(cap),
            cap_ept_pt_cap_get_cap_PTMapped_پتہ(cap),
            pt);
        clear_حافظہ((خالی *)pt, cap_get_cap_سائز_بٹ(cap));
    }
    cap_ept_pt_cap_ptr_سیٹ_cap_PTIs_Mapped(&(cte->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_خانہ, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_خانہ = pde;
    invept(pml4);

    واپس EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    اگر (invLabel == X86EPTPTUnmap) {
        اگر (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performEPTPTInvocationUnmap(cap, cte);
    }

    اگر (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~نقاب(EPT_PD_اشاریہ_انحراف);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (((ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خانہ)) ||
        ((ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خانہ))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((خالی *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_سیٹ_cap_PTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_خانہ, pml4);
}

ساکن exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_خانہ, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_خانہ = pte;
    cte->cap = cap;
    invept(pml4);

    واپس EXCEPTION_NONE;
}

ساکن exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_خانہ, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_خانہ[0] = pde1;
    اگر (LARGE_صفحہ_بٹ == 22) {
        pd_خانہ[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    واپس EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر)
{
    لفظ_t          vaddr;
    لفظ_t          w_rights_نقاب;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_اوصاف_t vmAttr;
    vm_صفحہ_سائز_t  چوکھٹا_سائز;
    asid_t          asid;

    چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, بفر);
    vaddr = vaddr & ~نقاب(EPT_PT_اشاریہ_انحراف);
    w_rights_نقاب = getSyscallArg(1, بفر);
    vmAttr = vm_اوصاف_From_لفظ(getSyscallArg(2, بفر));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

    اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    أكد(cap_چوکھٹا_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    اگر (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

    اگر (!checkVPAlignment(چوکھٹا_سائز, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, vaddr);
    cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingEPT);

    انتخاب (چوکھٹا_سائز) {
    /* PTE mappings */
    حالت X86_Small_صفحہ: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (ept_pte_ptr_get_read(lu_ret.pt_خانہ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pte = ept_pte_new(
                  paddr,
                  0,
                  0,
                  eptCacheFromVmAttr(vmAttr),
                  1,
                  WritableFromVMRights(vmRights),
                  1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performEPTPageMapPTE(cap, cte, lu_ret.pt_خانہ, pte, pml4);
    }

    /* PDE mappings */
    حالت X86_Large_صفحہ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            واپس EXCEPTION_SYSCALL_ERROR;
        }


        اگر ((ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خانہ)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (LARGE_صفحہ_بٹ != EPT_PD_اشاریہ_انحراف &&
            (ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_خانہ + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر ((ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خانہ)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
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
                             paddr + ثنائی_ہندسہ(EPT_PD_اشاریہ_انحراف),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performEPTPageMapPDE(cap, cte, lu_ret.pd_خانہ, pde1, pde2, pml4);
    }

    طےشدہ:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

خالی unmapEPTPage(vm_صفحہ_سائز_t صفحہ_سائز, asid_t asid, vptr_t vptr, خالی *pptr)
{
    ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    انتخاب (صفحہ_سائز) {
    حالت X86_Small_صفحہ: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(ڈھونڈو_ret.ept, vptr);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس;
        }
        اگر (!ept_pte_ptr_get_read(lu_ret.pt_خانہ)) {
            واپس;
        }
        اگر (ept_pte_ptr_get_صفحہ_بنیاد_پتہ(lu_ret.pt_خانہ) != addr) {
            واپس;
        }

        *lu_ret.pt_خانہ = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        توڑو;
    }
    حالت X86_Large_صفحہ: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(ڈھونڈو_ret.ept, vptr);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس;
        }
        اگر (ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ) != ept_pde_ept_pde_2m) {
            واپس;
        }
        اگر (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خانہ)) {
            واپس;
        }
        اگر (ept_pde_ept_pde_2m_ptr_get_صفحہ_بنیاد_پتہ(lu_ret.pd_خانہ) != addr) {
            واپس;
        }

        lu_ret.pd_خانہ[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        اگر (LARGE_صفحہ_بٹ != EPT_PD_اشاریہ_انحراف) {
            أكد(ept_pde_ptr_get_صفحہ_سائز(lu_ret.pd_خانہ + 1) == ept_pde_ept_pde_2m);
            أكد(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_خانہ + 1));
            أكد(ept_pde_ept_pde_2m_ptr_get_صفحہ_بنیاد_پتہ(lu_ret.pd_خانہ + 1) == addr + ثنائی_ہندسہ(21));

            lu_ret.pd_خانہ[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
        }
        توڑو;
    }
    طےشدہ:
        /* we did not allow mapping additional page sizes into EPT objects,
         * so this should not happen. As we have no way to return an error
         * all we can do is assert */
        أكد(!"Invalid page size for unmap");
    }
}

#اگر_ختم /* CONFIG_VTX */
