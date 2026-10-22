/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU

#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookup_VTDContext_ಸ್ಥಾನ_ret {
    vtd_cte_t   *cte;
    ಶಬ್ದ_t      ಸೂಚ್ಯಂಕ;
} lookup_VTDContext_ಸ್ಥಾನ_ret_t;


BOOT_ಸಂಕೇತ cap_t master_iospace_cap(ಶೂನ್ಯ)
{
    ಆದರೆ (x86KSnumDrhu == 0) {
        ಹಿಂತಿರುಗಿ cap_null_cap_new();
    }

    ಹಿಂತಿರುಗಿ
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

ಸ್ಥಿರ vtd_cte_t *lookup_vtd_context_ಸ್ಥಾನ(cap_t cap)
{
    uint32_t   vtd_ಬೇರು_ಸೂಚ್ಯಂಕ;
    uint32_t   vtd_context_ಸೂಚ್ಯಂಕ;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_ಬೇರು_ಸ್ಥಾನ;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_ಸ್ಥಾನ;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        pci_request_id = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIOASID(cap);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        pci_request_id = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid cap type");
    }

    vtd_ಬೇರು_ಸೂಚ್ಯಂಕ = get_pci_bus(pci_request_id);
    vtd_ಬೇರು_ಸ್ಥಾನ = x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ + vtd_ಬೇರು_ಸೂಚ್ಯಂಕ;

    vtd_context = (vtd_cte_t *)paddr_ಗಮ್ಯ_pptr(vtd_rte_ptr_get_ctp(vtd_ಬೇರು_ಸ್ಥಾನ));
    vtd_context_ಸೂಚ್ಯಂಕ = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_ಸ್ಥಾನ = &vtd_context[vtd_context_ಸೂಚ್ಯಂಕ];

    ಹಿಂತಿರುಗಿ vtd_context_ಸ್ಥಾನ;
}

ಸ್ಥಿರ lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_ಮಟ್ಟಗಳು(vtd_pte_t *iopt, ಶಬ್ದ_t translation,
                                                          ಶಬ್ದ_t ಮಟ್ಟಗಳು_ಗಮ್ಯ_resolve, ಶಬ್ದ_t ಮಟ್ಟಗಳು_ಉಳಿದ)
{
    lookupIOPTSlot_ret_t ret;

    ಶಬ್ದ_t      iopt_ಸೂಚ್ಯಂಕ = 0;
    vtd_pte_t   *iopt_ಸ್ಥಾನ_2 = 0;
    vtd_pte_t   *ಮುಂದಿನ_iopt_ಸ್ಥಾನ = 0;

    ಆದರೆ (iopt == 0) {
        ret.iopt_ಸ್ಥಾನ = 0;
        ret.level = ಮಟ್ಟಗಳು_ಉಳಿದ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    iopt_ಸೂಚ್ಯಂಕ = (translation  >> (VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * (x86KSnumIOPTLevels - 1 - (ಮಟ್ಟಗಳು_ಗಮ್ಯ_resolve - ಮಟ್ಟಗಳು_ಉಳಿದ)))) &
                 ಮುಖವಾಡ(VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    iopt_ಸ್ಥಾನ_2 = iopt + iopt_ಸೂಚ್ಯಂಕ;

    ಆದರೆ (!vtd_pte_ptr_get_write(iopt_ಸ್ಥಾನ_2) || ಮಟ್ಟಗಳು_ಉಳಿದ == 0) {
        ret.iopt_ಸ್ಥಾನ = iopt_ಸ್ಥಾನ_2;
        ret.level = ಮಟ್ಟಗಳು_ಉಳಿದ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    }
    ಮುಂದಿನ_iopt_ಸ್ಥಾನ = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_pte_ptr_get_addr(iopt_ಸ್ಥಾನ_2));
    ಹಿಂತಿರುಗಿ lookup_IOPTSlot_resolve_ಮಟ್ಟಗಳು(ಮುಂದಿನ_iopt_ಸ್ಥಾನ, translation, ಮಟ್ಟಗಳು_ಗಮ್ಯ_resolve, ಮಟ್ಟಗಳು_ಉಳಿದ - 1);
}


ಸ್ಥಿರ ಸಾಲೊಳಗೆ lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, ಶಬ್ದ_t io_ವಿಳಾಸ)
{
    lookupIOPTSlot_ret_t ret;

    ಆದರೆ (iopt == 0) {
        ret.iopt_ಸ್ಥಾನ    = 0;
        ret.level       = 0;
        ret.ಸ್ಥಿತಿ_2      = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ lookup_IOPTSlot_resolve_ಮಟ್ಟಗಳು(iopt, io_ವಿಳಾಸ >> ಪುಟ_ಬಿಟ್ಟುಗಳು,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

ಶೂನ್ಯ unmap_VTDContext_ನಮೂದು(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_ಸ್ಥಾನ(cap);
    ಪ್ರತಿಜ್ಞಾ(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    invalidate_iotlb();
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    delete_IOPage_ಕೋಷ್ಟಕ(cap);
    cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(cap, 0);
    ct_ಸ್ಥಾನ->cap = cap;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X86_IOPTInvocation_Map_Context_ಬೇರು(cap_t cap, cte_t *ct_ಸ್ಥಾನ, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_ಸ್ಥಾನ)
{
    *vtd_context_ಸ್ಥಾನ = vtd_cte;
    flushCacheRange(vtd_context_ಸ್ಥಾನ, VTD_CTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    ct_ಸ್ಥಾನ->cap = cap;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_ಸ್ಥಾನ, vtd_pte_t iopte, vtd_pte_t *iopt_ಸ್ಥಾನ)
{
    *iopt_ಸ್ಥಾನ = iopte;
    flushCacheRange(iopt_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    ct_ಸ್ಥಾನ->cap = cap;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    ಶಬ್ದ_t       invLabel,
    ಶಬ್ದ_t       ಉದ್ದಳತೆ,
    cte_t       *ಸ್ಥಾನ,
    cap_t        cap,
    ಶಬ್ದ_t      *ಬಫರ್
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    ಶಬ್ದ_t   io_ವಿಳಾಸ;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_ಸ್ಥಾನ;
    vtd_pte_t *vtd_pte;

    ಆದರೆ (invLabel == X86_IOPage_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (!isFinalCapability(ಸ್ಥಾನ)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performX86IOPTInvocationUnmap(cap, ಸ್ಥಾನ);
    }

    ಆದರೆ (invLabel != X86_IOPage_ಕೋಷ್ಟಕ_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ || ಉದ್ದಳತೆ < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_ವಿಳಾಸ   = getSyscallArg(0, ಬಫರ್) & ~ಮುಖವಾಡ(VTD_PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);

    ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    ಆದರೆ (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr(VTD_PTE_PTR(cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_ಸ್ಥಾನ = lookup_vtd_context_ಸ್ಥಾನ(io_space);

    ಆದರೆ (!vtd_cte_ptr_get_present(vtd_context_ಸ್ಥಾನ)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTLevel(cap, 0);
        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIOASID(cap, pci_request_id);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_X86_IOPTInvocation_Map_Context_ಬೇರು(cap, ಸ್ಥಾನ, vtd_cte, vtd_context_ಸ್ಥಾನ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_cte_ptr_get_asr(vtd_context_ಸ್ಥಾನ));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_ವಿಳಾಸ);

        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        ಆದರೆ (vtd_pte_ptr_get_addr(lu_ret.iopt_ಸ್ಥಾನ) != 0) {
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTMapped_ವಿಳಾಸ(cap, io_ವಿಳಾಸ);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performX86IOPTInvocationMapPT(cap, ಸ್ಥಾನ, iopte, lu_ret.iopt_ಸ್ಥಾನ);
    }
}

ಸ್ಥಿರ exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_ಸ್ಥಾನ, vtd_pte_t iopte, vtd_pte_t *iopt_ಸ್ಥಾನ)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *iopt_ಸ್ಥಾನ = iopte;
    flushCacheRange(iopt_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    ಶಬ್ದ_t       ಉದ್ದಳತೆ,
    cte_t       *ಸ್ಥಾನ,
    cap_t        cap,
    ಶಬ್ದ_t      *ಬಫರ್
)
{
    cap_t      io_space;
    ಶಬ್ದ_t     io_ವಿಳಾಸ;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_ಸ್ಥಾನ;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t ಚೌಕಟ್ಟು_cap_rights;
    seL4_CapRights_t dma_cap_rights_ಮುಖವಾಡ;

    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ || ಉದ್ದಳತೆ < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap) != X86_Small_ಪುಟ) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_ವಿಳಾಸ  = getSyscallArg(1, ಬಫರ್) & ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    paddr       = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

    ಆದರೆ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    ಆದರೆ (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_ಸ್ಥಾನ = lookup_vtd_context_ಸ್ಥಾನ(io_space);

    ಆದರೆ (!vtd_cte_ptr_get_present(vtd_context_ಸ್ಥಾನ)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_cte_ptr_get_asr(vtd_context_ಸ್ಥಾನ));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_ವಿಳಾಸ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (vtd_pte_ptr_get_addr(lu_ret.iopt_ಸ್ಥಾನ) != 0) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_ಮುಖವಾಡ = rights_From_ಶಬ್ದ(getSyscallArg(0, ಬಫರ್));
    ಚೌಕಟ್ಟು_cap_rights    = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);

    bool_t ಲಿಖ = seL4_CapRights_get_capAllowWrite(dma_cap_rights_ಮುಖವಾಡ) && (ಚೌಕಟ್ಟು_cap_rights == VMReadWrite);
    bool_t ಪಠ = seL4_CapRights_get_capAllowRead(dma_cap_rights_ಮುಖವಾಡ) && (ಚೌಕಟ್ಟು_cap_rights != VMKernelOnly);
    ಆದರೆ (ಲಿಖ || ಪಠ) {
        iopte = vtd_pte_new(paddr, !!ಲಿಖ, !!ಪಠ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, io_ವಿಳಾಸ);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ performX86IOInvocationMap(cap, ಸ್ಥಾನ, iopte, lu_ret.iopt_ಸ್ಥಾನ);
}

ಶೂನ್ಯ delete_IOPage_ಕೋಷ್ಟಕ(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    ಶಬ್ದ_t               io_ವಿಳಾಸ;
    vtd_cte_t           *vtd_context_ಸ್ಥಾನ;
    vtd_pte_t           *vtd_pte;

    ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_ಸ್ಥಾನ = lookup_vtd_context_ಸ್ಥಾನ(io_pt_cap);

        ಆದರೆ (!vtd_cte_ptr_get_present(vtd_context_ಸ್ಥಾನ)) {
            ಹಿಂತಿರುಗಿ;
        }

        vtd_pte = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_cte_ptr_get_asr(vtd_context_ಸ್ಥಾನ));

        ಆದರೆ (level == 0) {
            /* if we have been overmapped or something */
            ಆದರೆ (pptr_ಗಮ್ಯ_paddr(vtd_pte) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                ಹಿಂತಿರುಗಿ;
            }
            *vtd_context_ಸ್ಥಾನ = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_ಸ್ಥಾನ, VTD_CTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        } ಇಲ್ಲದಿದ್ದರೆ {
            io_ವಿಳಾಸ = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTMapped_ವಿಳಾಸ(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_ಮಟ್ಟಗಳು(vtd_pte, io_ವಿಳಾಸ >> ಪುಟ_ಬಿಟ್ಟುಗಳು, level - 1, level - 1);

            /* if we have been overmapped or something */
            ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || lu_ret.level != 0) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ (vtd_pte_ptr_get_addr(lu_ret.iopt_ಸ್ಥಾನ) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                ಹಿಂತಿರುಗಿ;
            }
            *lu_ret.iopt_ಸ್ಥಾನ = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        }
        invalidate_iotlb();
    }
}

ಶೂನ್ಯ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    ಶಬ್ದ_t               io_ವಿಳಾಸ;
    vtd_cte_t           *vtd_context_ಸ್ಥಾನ;
    vtd_pte_t           *vtd_pte;

    io_ವಿಳಾಸ  = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
    vtd_context_ಸ್ಥಾನ = lookup_vtd_context_ಸ್ಥಾನ(cap);


    ಆದರೆ (!vtd_cte_ptr_get_present(vtd_context_ಸ್ಥಾನ)) {
        ಹಿಂತಿರುಗಿ;
    }

    vtd_pte = (vtd_pte_t *)paddr_ಗಮ್ಯ_pptr(vtd_cte_ptr_get_asr(vtd_context_ಸ್ಥಾನ));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_ವಿಳಾಸ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (vtd_pte_ptr_get_addr(lu_ret.iopt_ಸ್ಥಾನ) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap))) {
        ಹಿಂತಿರುಗಿ;
    }

    *lu_ret.iopt_ಸ್ಥಾನ = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_ಸ್ಥಾನ, VTD_PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    unmapIOPage(ct_ಸ್ಥಾನ->cap);

    ct_ಸ್ಥಾನ->cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(ct_ಸ್ಥಾನ->cap, 0);
    ct_ಸ್ಥಾನ->cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(ct_ಸ್ಥಾನ->cap, X86_MappingNone);
    ct_ಸ್ಥಾನ->cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(ct_ಸ್ಥಾನ->cap, asidInvalid);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(ಶಬ್ದ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_IOMMU */
