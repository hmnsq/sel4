/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU

#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookup_VTDContext_ਥਾਂ_ret {
    vtd_cte_t   *cte;
    ਸ਼ਬਦ_t      ਸੂਚਕਾਂਕ;
} lookup_VTDContext_ਥਾਂ_ret_t;


BOOT_ਕੋਡ cap_t master_iospace_cap(ਖਾਲੀ)
{
    ਜੇ (x86KSnumDrhu == 0) {
        ਵਾਪਸ cap_null_cap_new();
    }

    ਵਾਪਸ
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

ਸਥਿਰ vtd_cte_t *lookup_vtd_context_ਥਾਂ(cap_t cap)
{
    uint32_t   vtd_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ;
    uint32_t   vtd_context_ਸੂਚਕਾਂਕ;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_ਜੜ੍ਹ_ਥਾਂ;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_ਥਾਂ;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        pci_request_id = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIOASID(cap);
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        pci_request_id = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid cap type");
    }

    vtd_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ = get_pci_bus(pci_request_id);
    vtd_ਜੜ੍ਹ_ਥਾਂ = x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ + vtd_ਜੜ੍ਹ_ਸੂਚਕਾਂਕ;

    vtd_context = (vtd_cte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_rte_ptr_get_ctp(vtd_ਜੜ੍ਹ_ਥਾਂ));
    vtd_context_ਸੂਚਕਾਂਕ = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_ਥਾਂ = &vtd_context[vtd_context_ਸੂਚਕਾਂਕ];

    ਵਾਪਸ vtd_context_ਥਾਂ;
}

ਸਥਿਰ lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_ਪੱਧਰ(vtd_pte_t *iopt, ਸ਼ਬਦ_t translation,
                                                          ਸ਼ਬਦ_t ਪੱਧਰ_ਮੰਜ਼ਿਲ_resolve, ਸ਼ਬਦ_t ਪੱਧਰ_ਬਚਿਆ)
{
    lookupIOPTSlot_ret_t ret;

    ਸ਼ਬਦ_t      iopt_ਸੂਚਕਾਂਕ = 0;
    vtd_pte_t   *iopt_ਥਾਂ_2 = 0;
    vtd_pte_t   *ਅਗਲਾ_iopt_ਥਾਂ = 0;

    ਜੇ (iopt == 0) {
        ret.iopt_ਥਾਂ = 0;
        ret.level = ਪੱਧਰ_ਬਚਿਆ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    iopt_ਸੂਚਕਾਂਕ = (translation  >> (VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * (x86KSnumIOPTLevels - 1 - (ਪੱਧਰ_ਮੰਜ਼ਿਲ_resolve - ਪੱਧਰ_ਬਚਿਆ)))) &
                 ਨਕਾਬ(VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    iopt_ਥਾਂ_2 = iopt + iopt_ਸੂਚਕਾਂਕ;

    ਜੇ (!vtd_pte_ptr_get_write(iopt_ਥਾਂ_2) || ਪੱਧਰ_ਬਚਿਆ == 0) {
        ret.iopt_ਥਾਂ = iopt_ਥਾਂ_2;
        ret.level = ਪੱਧਰ_ਬਚਿਆ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    }
    ਅਗਲਾ_iopt_ਥਾਂ = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_pte_ptr_get_addr(iopt_ਥਾਂ_2));
    ਵਾਪਸ lookup_IOPTSlot_resolve_ਪੱਧਰ(ਅਗਲਾ_iopt_ਥਾਂ, translation, ਪੱਧਰ_ਮੰਜ਼ਿਲ_resolve, ਪੱਧਰ_ਬਚਿਆ - 1);
}


ਸਥਿਰ ਸਤਰਅੰਦਰ lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, ਸ਼ਬਦ_t io_ਪਤਾ)
{
    lookupIOPTSlot_ret_t ret;

    ਜੇ (iopt == 0) {
        ret.iopt_ਥਾਂ    = 0;
        ret.level       = 0;
        ret.ਸਥਿਤੀ      = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ lookup_IOPTSlot_resolve_ਪੱਧਰ(iopt, io_ਪਤਾ >> ਪੰਨਾ_ਬਿੱਟ,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

ਖਾਲੀ unmap_VTDContext_ਇੰਦਰਾਜ(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_ਥਾਂ(cap);
    ਪ੍ਰਤਿਜ੍ਞਾ(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_ਮਾਪ_ਬਿੱਟ);
    invalidate_iotlb();
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ;
}

ਸਥਿਰ exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    delete_IOPage_ਸਾਰਣੀ(cap);
    cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(cap, 0);
    ct_ਥਾਂ->cap = cap;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X86_IOPTInvocation_Map_Context_ਜੜ੍ਹ(cap_t cap, cte_t *ct_ਥਾਂ, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_ਥਾਂ)
{
    *vtd_context_ਥਾਂ = vtd_cte;
    flushCacheRange(vtd_context_ਥਾਂ, VTD_CTE_ਮਾਪ_ਬਿੱਟ);
    ct_ਥਾਂ->cap = cap;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_ਥਾਂ, vtd_pte_t iopte, vtd_pte_t *iopt_ਥਾਂ)
{
    *iopt_ਥਾਂ = iopte;
    flushCacheRange(iopt_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);
    ct_ਥਾਂ->cap = cap;

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    ਸ਼ਬਦ_t       invLabel,
    ਸ਼ਬਦ_t       ਲੰਮਾਈ,
    cte_t       *ਥਾਂ,
    cap_t        cap,
    ਸ਼ਬਦ_t      *ਬਫਰ
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    ਸ਼ਬਦ_t   io_ਪਤਾ;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_ਥਾਂ;
    vtd_pte_t *vtd_pte;

    ਜੇ (invLabel == X86_IOPage_ਸਾਰਣੀ_Unmap) {
        ਜੇ (!isFinalCapability(ਥਾਂ)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performX86IOPTInvocationUnmap(cap, ਥਾਂ);
    }

    ਜੇ (invLabel != X86_IOPage_ਸਾਰਣੀ_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ || ਲੰਮਾਈ < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_ਪਤਾ   = getSyscallArg(0, ਬਫਰ) & ~ਨਕਾਬ(VTD_PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + se_L4_ਪੰਨਾ_ਬਿੱਟ);

    ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    ਜੇ (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr(VTD_PTE_PTR(cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_ਥਾਂ = lookup_vtd_context_ਥਾਂ(io_space);

    ਜੇ (!vtd_cte_ptr_get_present(vtd_context_ਥਾਂ)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTLevel(cap, 0);
        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIOASID(cap, pci_request_id);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_X86_IOPTInvocation_Map_Context_ਜੜ੍ਹ(cap, ਥਾਂ, vtd_cte, vtd_context_ਥਾਂ);
    } ਨਹੀਂਤਾਂ {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_cte_ptr_get_asr(vtd_context_ਥਾਂ));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_ਪਤਾ);

        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        ਜੇ (vtd_pte_ptr_get_addr(lu_ret.iopt_ਥਾਂ) != 0) {
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTMapped_ਪਤਾ(cap, io_ਪਤਾ);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performX86IOPTInvocationMapPT(cap, ਥਾਂ, iopte, lu_ret.iopt_ਥਾਂ);
    }
}

ਸਥਿਰ exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_ਥਾਂ, vtd_pte_t iopte, vtd_pte_t *iopt_ਥਾਂ)
{
    ct_ਥਾਂ->cap = cap;
    *iopt_ਥਾਂ = iopte;
    flushCacheRange(iopt_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);

    ਵਾਪਸ EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    ਸ਼ਬਦ_t       ਲੰਮਾਈ,
    cte_t       *ਥਾਂ,
    cap_t        cap,
    ਸ਼ਬਦ_t      *ਬਫਰ
)
{
    cap_t      io_space;
    ਸ਼ਬਦ_t     io_ਪਤਾ;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_ਥਾਂ;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t ਚੌਖਟਾ_cap_rights;
    seL4_CapRights_t dma_cap_rights_ਨਕਾਬ;

    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ || ਲੰਮਾਈ < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap) != X86_Small_ਪੰਨਾ) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_ਪਤਾ  = getSyscallArg(1, ਬਫਰ) & ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ);
    paddr       = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

    ਜੇ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    ਜੇ (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_ਥਾਂ = lookup_vtd_context_ਥਾਂ(io_space);

    ਜੇ (!vtd_cte_ptr_get_present(vtd_context_ਥਾਂ)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_cte_ptr_get_asr(vtd_context_ਥਾਂ));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_ਪਤਾ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (vtd_pte_ptr_get_addr(lu_ret.iopt_ਥਾਂ) != 0) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_ਨਕਾਬ = rights_From_ਸ਼ਬਦ(getSyscallArg(0, ਬਫਰ));
    ਚੌਖਟਾ_cap_rights    = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);

    bool_t ਲਿਖ = seL4_CapRights_get_capAllowWrite(dma_cap_rights_ਨਕਾਬ) && (ਚੌਖਟਾ_cap_rights == VMReadWrite);
    bool_t ਪਠ = seL4_CapRights_get_capAllowRead(dma_cap_rights_ਨਕਾਬ) && (ਚੌਖਟਾ_cap_rights != VMKernelOnly);
    ਜੇ (ਲਿਖ || ਪਠ) {
        iopte = vtd_pte_new(paddr, !!ਲਿਖ, !!ਪਠ);
    } ਨਹੀਂਤਾਂ {
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, io_ਪਤਾ);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ performX86IOInvocationMap(cap, ਥਾਂ, iopte, lu_ret.iopt_ਥਾਂ);
}

ਖਾਲੀ delete_IOPage_ਸਾਰਣੀ(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    ਸ਼ਬਦ_t               io_ਪਤਾ;
    vtd_cte_t           *vtd_context_ਥਾਂ;
    vtd_pte_t           *vtd_pte;

    ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_ਥਾਂ = lookup_vtd_context_ਥਾਂ(io_pt_cap);

        ਜੇ (!vtd_cte_ptr_get_present(vtd_context_ਥਾਂ)) {
            ਵਾਪਸ;
        }

        vtd_pte = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_cte_ptr_get_asr(vtd_context_ਥਾਂ));

        ਜੇ (level == 0) {
            /* if we have been overmapped or something */
            ਜੇ (pptr_ਮੰਜ਼ਿਲ_paddr(vtd_pte) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                ਵਾਪਸ;
            }
            *vtd_context_ਥਾਂ = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_ਥਾਂ, VTD_CTE_ਮਾਪ_ਬਿੱਟ);
        } ਨਹੀਂਤਾਂ {
            io_ਪਤਾ = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTMapped_ਪਤਾ(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_ਪੱਧਰ(vtd_pte, io_ਪਤਾ >> ਪੰਨਾ_ਬਿੱਟ, level - 1, level - 1);

            /* if we have been overmapped or something */
            ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE || lu_ret.level != 0) {
                ਵਾਪਸ;
            }
            ਜੇ (vtd_pte_ptr_get_addr(lu_ret.iopt_ਥਾਂ) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                ਵਾਪਸ;
            }
            *lu_ret.iopt_ਥਾਂ = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);
        }
        invalidate_iotlb();
    }
}

ਖਾਲੀ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    ਸ਼ਬਦ_t               io_ਪਤਾ;
    vtd_cte_t           *vtd_context_ਥਾਂ;
    vtd_pte_t           *vtd_pte;

    io_ਪਤਾ  = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
    vtd_context_ਥਾਂ = lookup_vtd_context_ਥਾਂ(cap);


    ਜੇ (!vtd_cte_ptr_get_present(vtd_context_ਥਾਂ)) {
        ਵਾਪਸ;
    }

    vtd_pte = (vtd_pte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(vtd_cte_ptr_get_asr(vtd_context_ਥਾਂ));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_ਪਤਾ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE || lu_ret.level != 0) {
        ਵਾਪਸ;
    }

    ਜੇ (vtd_pte_ptr_get_addr(lu_ret.iopt_ਥਾਂ) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap))) {
        ਵਾਪਸ;
    }

    *lu_ret.iopt_ਥਾਂ = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_ਥਾਂ, VTD_PTE_ਮਾਪ_ਬਿੱਟ);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_ਥਾਂ)
{
    unmapIOPage(ct_ਥਾਂ->cap);

    ct_ਥਾਂ->cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(ct_ਥਾਂ->cap, 0);
    ct_ਥਾਂ->cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(ct_ਥਾਂ->cap, X86_MappingNone);
    ct_ਥਾਂ->cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(ct_ਥਾਂ->cap, asidInvalid);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(ਸ਼ਬਦ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
}

#ਜੇ_ਅੰਤ /* CONFIG_IOMMU */
