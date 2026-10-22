/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_VTX

#ضمن <model/statedata.h>
#ضمن <arch/kernel/ept.h>
#ضمن <arch/api/invocation.h>

بنية lookupEPTPDPTSlot_ret {
    exception_t وضعیت;
    ept_pdpte_t *pdpt_جایگاه;
};
تعريفنوع بنية lookupEPTPDPTSlot_ret lookupEPTPDPTSlot_ret_t;

بنية lookupEPTPDSlot_ret {
    exception_t وضعیت;
    ept_pde_t  *pd_جایگاه;
};
تعريفنوع بنية lookupEPTPDSlot_ret lookupEPTPDSlot_ret_t;

بنية lookupEPTPTSlot_ret {
    exception_t وضعیت;
    ept_pte_t  *pt_جایگاه;
};
تعريفنوع بنية lookupEPTPTSlot_ret lookupEPTPTSlot_ret_t;

تعداد ept_cache_options {
    EPTUncacheable = 0,
    EPTWriteCombining = 1,
    EPTWriteThrough = 4,
    EPTWriteProtected = 5,
    EPTWrite_پشت = 6
};
تعريفنوع تعداد ept_cache_options ept_cache_options_t;

فراغ deleteEPTASID(asid_t asid, ept_pml4e_t *ept)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بالا(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_ept &&
            (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ریشه(asid_map) == ept) {
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
        }
    }
}

exception_t performX86EPTPageInvocationUnmap(cap_t cap, cte_t *ct_جایگاه)
{
    unmapEPTPage(
        cap_قاب_cap_get_cap_FSize(cap),
        cap_قاب_cap_get_cap_FMapped_ASID(cap),
        cap_قاب_cap_get_cap_FMapped_آدرس(cap),
        (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
    );

    cap_قاب_cap_ptr_مجموعه_cap_FMapped_آدرس(&ct_جایگاه->cap, 0);
    cap_قاب_cap_ptr_مجموعه_cap_FMapped_ASID(&ct_جایگاه->cap, asidInvalid);
    cap_قاب_cap_ptr_مجموعه_cap_FMap_Type(&ct_جایگاه->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_EPTFor_ASID(asid_t asid)
{
    پیدا_کن_EPTFor_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_ept) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.ept = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.ept = (ept_pml4e_t *)asid_map_asid_map_ept_get_ept_ریشه(asid_map);
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_pml4e_t *CONST lookup_EPTPML4_جایگاه(ept_pml4e_t *pml4, vptr_t vptr)
{
    أرجع pml4 + GET_EPT_PML4_شاخص(vptr);
}

ساكن lookupEPTPDPTSlot_ret_t CONST lookupEPTPDPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDPTSlot_ret_t ret;
    ept_pml4e_t *pml4_جایگاه;

    pml4_جایگاه = lookup_EPTPML4_جایگاه(pml4, vptr);

    إذا (!ept_pml4e_ptr_get_read(pml4_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PML4_شاخص_جابه_جایی);

        ret.pdpt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pdpte_t *pdpt = paddr_مقصد_pptr(ept_pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه));
    uint32_t شاخص = GET_EPT_PDPT_شاخص(vptr);
    ret.pdpt_جایگاه = pdpt + شاخص;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPDSlot_ret_t lookupEPTPDSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPDSlot_ret_t ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDPTSlot(pml4, vptr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        /* current_lookup_fault will have been set by lookupEPTPDPTSlot */
        ret.pd_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    إذا (!ept_pdpte_ptr_get_read(lu_ret.pdpt_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PDPT_شاخص_جابه_جایی);

        ret.pd_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pde_t *pd = paddr_مقصد_pptr(ept_pdpte_ptr_get_pd_پایه_آدرس(lu_ret.pdpt_جایگاه));
    uint32_t شاخص = GET_EPT_PD_شاخص(vptr);
    ret.pd_جایگاه = pd + شاخص;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن lookupEPTPTSlot_ret_t lookupEPTPTSlot(ept_pml4e_t *pml4, vptr_t vptr)
{
    lookupEPTPTSlot_ret_t ret;
    lookupEPTPDSlot_ret_t lu_ret;

    lu_ret = lookupEPTPDSlot(pml4, vptr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        /* current_lookup_fault will have been set by lookupEPTPDSlot */
        ret.pt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    إذا ((ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) != ept_pde_ept_pde_pt) ||
        !ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(EPT_PD_شاخص_جابه_جایی);

        ret.pt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ept_pte_t *pt = paddr_مقصد_pptr(ept_pde_ept_pde_pt_ptr_get_pt_پایه_آدرس(lu_ret.pd_جایگاه));
    uint32_t شاخص = GET_EPT_PT_شاخص(vptr);

    ret.pt_جایگاه = pt + شاخص;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

ساكن ept_cache_options_t eptCacheFromVmAttr(vm_صفات_t vmAttr)
{
    /* Need to sanitise user input, vmAttr will not have been verified at this point. */
    ept_cache_options_t option = vmAttr.واژگان[0];
    إذا (option != EPTUncacheable &&
        option != EPTWriteCombining &&
        option != EPTWriteThrough &&
        option != EPTWrite_پشت) {
        option = EPTWrite_پشت;
    }
    أرجع option;
}

EPTPDPTMapped_ret_t EPTPDPTMapped(asid_t asid, vptr_t vptr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t ret;
    پیدا_کن_EPTFor_ASID_ret_t asid_ret;
    ept_pml4e_t *pml4_جایگاه;

    asid_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (asid_ret.وضعیت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pml4_جایگاه = عدم;
        ret.وضعیت = asid_ret.وضعیت;
        أرجع ret;
    }

    pml4_جایگاه = lookup_EPTPML4_جایگاه(asid_ret.ept, vptr);

    إذا (ept_pml4e_ptr_get_read(pml4_جایگاه)
        && ptrFromPAddr(ept_pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه)) == pdpt) {
        ret.pml4 = asid_ret.ept;
        ret.pml4_جایگاه = pml4_جایگاه;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pml4_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPDPT(asid_t asid, vptr_t vaddr, ept_pdpte_t *pdpt)
{
    EPTPDPTMapped_ret_t lu_ret;

    lu_ret = EPTPDPTMapped(asid, vaddr, pdpt);

    إذا (lu_ret.وضعیت == EXCEPTION_NONE) {
        *lu_ret.pml4_جایگاه = ept_pml4e_new(0, 0, 0, 0);
        invept(lu_ret.pml4);
    }
}

ساكن exception_t performEPTPDPTInvocationUnmap(cap_t cap, cte_t *cte)
{
    إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        ept_pdpte_t *pdpt = (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap);
        unmapEPTPDPT(
            cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
            cap_ept_pdpt_cap_get_cap_PDPTMapped_آدرس(cap),
            pdpt);
        clear_حافظه((فراغ *)pdpt, cap_get_cap_اندازه_بیتها(cap));
    }
    cap_ept_pdpt_cap_ptr_مجموعه_cap_PDPTIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDPTInvocationMap(cap_t cap, cte_t *cte, ept_pml4e_t pml4e, ept_pml4e_t *pml4_جایگاه,
                                               ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pml4_جایگاه = pml4e;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX86EPTPDPTInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pml4e_t     pml4e;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret;
    ept_pml4e_t    *pml4_جایگاه;

    إذا (invLabel == X86EPTPDPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDPTInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPDPTMap) {
        userError("X86EPTPDPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDPTMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X86EPTPDPTMap: EPT PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر);
    /* cannot use ~MASK(EPT_PML4_INDEX_OFFSET) because on 32-bit compilations
     * this results in an error shifting by greater than 31 bits, so we manually
     * force a 64-bit variable to do the shifting with */
    vaddr = vaddr & ~(((uint64_t)1 << EPT_PML4_شاخص_جابه_جایی) - 1);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDPTMap: Not a valid EPT PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کن_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4_جایگاه = lookup_EPTPML4_جایگاه(pml4, vaddr);

    إذا (ept_pml4e_ptr_get_read(pml4_جایگاه)) {
        userError("X86EPTPDPTMap: PDPT already mapped here.");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((فراغ *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
    pml4e = ept_pml4e_new(
                paddr,
                1,
                1,
                1
            );

    cap = cap_ept_pdpt_cap_مجموعه_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_ept_pdpt_cap_مجموعه_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_ept_pdpt_cap_مجموعه_cap_PDPTMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDPTInvocationMap(cap, cte, pml4e, pml4_جایگاه, pml4);
}

exception_t decodeX86EPTInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ept_pdpt_cap:
        أرجع decodeX86EPTPDPTInvocation(invLabel, درازی, cte, cap, میانگیر);
    حالة cap_ept_pd_cap:
        أرجع decodeX86EPTPDInvocation(invLabel, درازی, cte, cap, میانگیر);
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTPTInvocation(invLabel, درازی, cte, cap, میانگیر);
    افتراضي:
        fail("Invalid cap type");
    }
}

EPTPageDirectoryMapped_ret_t EPTPageDirectoryMapped(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t ret;
    lookupEPTPDPTSlot_ret_t پیدا_کن_ret;
    پیدا_کن_EPTFor_ASID_ret_t asid_ret;

    asid_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (asid_ret.وضعیت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdpt_جایگاه = عدم;
        ret.وضعیت = asid_ret.وضعیت;
        أرجع ret;
    }

    پیدا_کن_ret = lookupEPTPDPTSlot(asid_ret.ept, vaddr);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pdpt_جایگاه = عدم;
        ret.وضعیت = پیدا_کن_ret.وضعیت;
        أرجع ret;
    }

    إذا (ept_pdpte_ptr_get_read(پیدا_کن_ret.pdpt_جایگاه)
        && ptrFromPAddr(ept_pdpte_ptr_get_pd_پایه_آدرس(پیدا_کن_ret.pdpt_جایگاه)) == pd) {
        ret.pml4 = asid_ret.ept;
        ret.pdpt_جایگاه = پیدا_کن_ret.pdpt_جایگاه;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pdpt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmapEPTPageDirectory(asid_t asid, vptr_t vaddr, ept_pde_t *pd)
{
    EPTPageDirectoryMapped_ret_t lu_ret;

    lu_ret = EPTPageDirectoryMapped(asid, vaddr, pd);

    إذا (lu_ret.وضعیت == EXCEPTION_NONE) {
        *lu_ret.pdpt_جایگاه = ept_pdpte_new(
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
            cap_ept_pd_cap_get_cap_PDMapped_آدرس(cap),
            pd);
        clear_حافظه((فراغ *)pd, cap_get_cap_اندازه_بیتها(cap));
    }
    cap_ept_pd_cap_ptr_مجموعه_cap_PDIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPDInvocationMap(cap_t cap, cte_t *cte, ept_pdpte_t pdpte, ept_pdpte_t *pdpt_جایگاه,
                                             ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pdpt_جایگاه = pdpte;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPDInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pdpte_t     pdpte;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret;
    lookupEPTPDPTSlot_ret_t lu_ret;

    إذا (invLabel == X86EPTPDUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPDInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPDMap) {
        userError("X86EPTPD Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPDMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
        userError("X86EPTPDMap: EPT Page directory is already mapped to a PDPT.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر);
    vaddr = vaddr & ~ماسک(EPT_PDPT_شاخص_جابه_جایی);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap) {
        userError("X86EPTPDMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(pml4Cap);
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کن_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        userError("X86EPTPDMap: EPT PML4 is not mapped.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.ept != pml4) {
        userError("X86EPTPDMap: EPT PML4 asid is invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDPTSlot(pml4, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ept_pdpte_ptr_get_read(lu_ret.pdpt_جایگاه)) {
        userError("X86EPTPDMap: Page directory already mapped here.");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((فراغ *)(cap_ept_pd_cap_get_capPDBasePtr(cap)));
    pdpte = ept_pdpte_new(
                paddr,  /* pd_base_address  */
                0,      /* avl_cte_depth    */
                1,      /* execute          */
                1,      /* write            */
                1       /* read             */
            );

    cap = cap_ept_pd_cap_مجموعه_cap_PDIs_Mapped(cap, 1);
    cap = cap_ept_pd_cap_مجموعه_cap_PDMapped_ASID(cap, asid);
    cap = cap_ept_pd_cap_مجموعه_cap_PDMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPDInvocationMap(cap, cte, pdpte, lu_ret.pdpt_جایگاه, pml4);
}

EPTPage_جدول_Mapped_ret_t EPTPage_جدول_Mapped(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t ret;
    lookupEPTPDSlot_ret_t پیدا_کن_ret;
    پیدا_کن_EPTFor_ASID_ret_t asid_ret;

    asid_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (asid_ret.وضعیت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pd_جایگاه = عدم;
        ret.وضعیت = asid_ret.وضعیت;
        أرجع ret;
    }

    پیدا_کن_ret = lookupEPTPDSlot(asid_ret.ept, vaddr);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        ret.pml4 = عدم;
        ret.pd_جایگاه = عدم;
        ret.وضعیت = پیدا_کن_ret.وضعیت;
        أرجع ret;
    }

    إذا (ept_pde_ptr_get_صفحه_اندازه(پیدا_کن_ret.pd_جایگاه) == ept_pde_ept_pde_pt
        && ptrFromPAddr(ept_pde_ept_pde_pt_ptr_get_pt_پایه_آدرس(پیدا_کن_ret.pd_جایگاه)) == pt) {
        ret.pml4 = asid_ret.ept;
        ret.pd_جایگاه = پیدا_کن_ret.pd_جایگاه;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    } وإلا {
        ret.pml4 = عدم;
        ret.pd_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }
}

فراغ unmap_EPTPage_جدول(asid_t asid, vptr_t vaddr, ept_pte_t *pt)
{
    EPTPage_جدول_Mapped_ret_t lu_ret;

    lu_ret = EPTPage_جدول_Mapped(asid, vaddr, pt);

    إذا (lu_ret.وضعیت == EXCEPTION_NONE) {
        *lu_ret.pd_جایگاه = ept_pde_ept_pde_pt_new(
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
            cap_ept_pt_cap_get_cap_PTMapped_آدرس(cap),
            pt);
        clear_حافظه((فراغ *)pt, cap_get_cap_اندازه_بیتها(cap));
    }
    cap_ept_pt_cap_ptr_مجموعه_cap_PTIs_Mapped(&(cte->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPTInvocationMap(cap_t cap, cte_t *cte, ept_pde_t pde, ept_pde_t *pd_جایگاه, ept_pml4e_t *pml4)
{
    cte->cap = cap;
    *pd_جایگاه = pde;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPTInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    کلمه_t          vaddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    ept_pde_t       pde;
    paddr_t         paddr;
    asid_t          asid;
    پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret;
    lookupEPTPDSlot_ret_t lu_ret;

    إذا (invLabel == X86EPTPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPTInvocationUnmap(cap, cte);
    }

    إذا (invLabel != X86EPTPTMap) {
        userError("X86EPTPT Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86EPTPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
        userError("X86EPTPT EPT Page table is already mapped to an EPT page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر);
    vaddr = vaddr & ~ماسک(EPT_PD_شاخص_جابه_جایی);
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPTMap: Not a valid EPT pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کن_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupEPTPDSlot(pml4, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) == ept_pde_ept_pde_pt) &&
         ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_جایگاه)) ||
        ((ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) == ept_pde_ept_pde_2m) &&
         ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_جایگاه))) {
        userError("X86EPTPTMap: Page table already mapped here");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((فراغ *)(cap_ept_pt_cap_get_capPTBasePtr(cap)));
    pde = ept_pde_ept_pde_pt_new(
              paddr,/* pt_base_address  */
              0,    /* avl_cte_depth    */
              1,    /* execute          */
              1,    /* write            */
              1     /* read             */
          );

    cap = cap_ept_pt_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    cap = cap_ept_pt_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_ept_pt_cap_مجموعه_cap_PTMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع performEPTPTInvocationMap(cap, cte, pde, lu_ret.pd_جایگاه, pml4);
}

ساكن exception_t performEPTPageMapPTE(cap_t cap, cte_t *cte, ept_pte_t *pt_جایگاه, ept_pte_t pte, ept_pml4e_t *pml4)
{
    *pt_جایگاه = pte;
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performEPTPageMapPDE(cap_t cap, cte_t *cte, ept_pde_t *pd_جایگاه, ept_pde_t pde1, ept_pde_t pde2,
                                        ept_pml4e_t *pml4)
{
    pd_جایگاه[0] = pde1;
    إذا (LARGE_صفحه_بیتها == 22) {
        pd_جایگاه[1] = pde2;
    }
    cte->cap = cap;
    invept(pml4);

    أرجع EXCEPTION_NONE;
}

exception_t decodeX86EPTPageMap(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر)
{
    کلمه_t          vaddr;
    کلمه_t          w_rights_ماسک;
    paddr_t         paddr;
    cap_t           pml4Cap;
    ept_pml4e_t    *pml4;
    vm_rights_t     capVMRights;
    vm_rights_t     vmRights;
    vm_صفات_t vmAttr;
    vm_صفحه_اندازه_t  قاب_اندازه;
    asid_t          asid;

    قاب_اندازه = cap_قاب_cap_get_cap_FSize(cap);
    vaddr = getSyscallArg(0, میانگیر);
    vaddr = vaddr & ~ماسک(EPT_PT_شاخص_جابه_جایی);
    w_rights_ماسک = getSyscallArg(1, میانگیر);
    vmAttr = vm_صفات_From_کلمه(getSyscallArg(2, میانگیر));
    pml4Cap = current_extra_caps.excaprefs[0]->cap;

    capVMRights = cap_قاب_cap_get_cap_FVMRights(cap);

    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86EPTPageMap: Frame already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أكد(cap_قاب_cap_get_cap_FMap_Type(cap) == X86_MappingNone);

    إذا (cap_get_capType(pml4Cap) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(pml4Cap)) {
        userError("X86EPTPageMap: Attempting to map frame into invalid ept pml4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4 = (ept_pml4e_t *)(cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(pml4Cap));
    asid = cap_ept_pml4_cap_get_capPML4MappedASID(pml4Cap);

    پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.ept != pml4) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

    إذا (!checkVPAlignment(قاب_اندازه, vaddr)) {
        current_syscall_error.type = seL4_AlignmentError;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap));

    cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asid);
    cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, vaddr);
    cap = cap_قاب_cap_مجموعه_cap_FMap_Type(cap, X86_MappingEPT);

    بدل (قاب_اندازه) {
    /* PTE mappings */
    حالة X86_Small_صفحه: {
        lookupEPTPTSlot_ret_t lu_ret;
        ept_pte_t pte;

        lu_ret = lookupEPTPTSlot(pml4, vaddr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            /* current_lookup_fault will have been set by lookupEPTPTSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (ept_pte_ptr_get_read(lu_ret.pt_جایگاه)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_اولی;
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

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPTE(cap, cte, lu_ret.pt_جایگاه, pte, pml4);
    }

    /* PDE mappings */
    حالة X86_Large_صفحه: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(pml4, vaddr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("X86EPTPageMap: Need a page directory first.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            /* current_lookup_fault will have been set by lookupEPTPDSlot */
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        إذا ((ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_جایگاه)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (LARGE_صفحه_بیتها != EPT_PD_شاخص_جابه_جایی &&
            (ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه + 1) == ept_pde_ept_pde_pt) &&
            ept_pde_ept_pde_pt_ptr_get_read(lu_ret.pd_جایگاه + 1)) {
            userError("X86EPTPageMap: Page table already present.");
            current_syscall_error.type = se_L4_Delete_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) == ept_pde_ept_pde_2m) &&
            ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_جایگاه)) {
            userError("X86EPTPageMap: Mapping already present.");
            current_syscall_error.type = se_L4_Delete_اولی;
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
                             paddr + بیت(EPT_PD_شاخص_جابه_جایی),
                             0,
                             0,
                             eptCacheFromVmAttr(vmAttr),
                             1,
                             WritableFromVMRights(vmRights),
                             1);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performEPTPageMapPDE(cap, cte, lu_ret.pd_جایگاه, pde1, pde2, pml4);
    }

    افتراضي:
        /* When initializing EPT we only checked for support for 4K and 2M
         * pages, so we must disallow attempting to use any other */
        userError("X86EPTPageMap: Attempted to map unsupported page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ unmapEPTPage(vm_صفحه_اندازه_t صفحه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret;
    paddr_t addr = addrFromPPtr(pptr);

    پیدا_کن_ret = پیدا_کن_EPTFor_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (صفحه_اندازه) {
    حالة X86_Small_صفحه: {
        lookupEPTPTSlot_ret_t lu_ret;

        lu_ret = lookupEPTPTSlot(پیدا_کن_ret.ept, vptr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!ept_pte_ptr_get_read(lu_ret.pt_جایگاه)) {
            أرجع;
        }
        إذا (ept_pte_ptr_get_صفحه_پایه_آدرس(lu_ret.pt_جایگاه) != addr) {
            أرجع;
        }

        *lu_ret.pt_جایگاه = ept_pte_new(0, 0, 0, 0, 0, 0, 0);
        اكسر;
    }
    حالة X86_Large_صفحه: {
        lookupEPTPDSlot_ret_t lu_ret;

        lu_ret = lookupEPTPDSlot(پیدا_کن_ret.ept, vptr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) != ept_pde_ept_pde_2m) {
            أرجع;
        }
        إذا (!ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_جایگاه)) {
            أرجع;
        }
        إذا (ept_pde_ept_pde_2m_ptr_get_صفحه_پایه_آدرس(lu_ret.pd_جایگاه) != addr) {
            أرجع;
        }

        lu_ret.pd_جایگاه[0] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);

        إذا (LARGE_صفحه_بیتها != EPT_PD_شاخص_جابه_جایی) {
            أكد(ept_pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه + 1) == ept_pde_ept_pde_2m);
            أكد(ept_pde_ept_pde_2m_ptr_get_read(lu_ret.pd_جایگاه + 1));
            أكد(ept_pde_ept_pde_2m_ptr_get_صفحه_پایه_آدرس(lu_ret.pd_جایگاه + 1) == addr + بیت(21));

            lu_ret.pd_جایگاه[1] = ept_pde_ept_pde_2m_new(0, 0, 0, 0, 0, 0, 0);
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

#پایان_اگر /* CONFIG_VTX */
