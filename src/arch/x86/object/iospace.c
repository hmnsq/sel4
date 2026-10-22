/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_IOMMU

#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/intel-vtd.h>


రకనిర్వచనం నిర్మాణం lookup_VTDContext_స్థానం_ret {
    vtd_cte_t   *cte;
    మాట_t      సూచీ;
} lookup_VTDContext_స్థానం_ret_t;


BOOT_సంకేతం cap_t master_iospace_cap(శూన్య)
{
    ఒకవేళ (x86KSnumDrhu == 0) {
        తిరిగి cap_null_cap_new();
    }

    తిరిగి
        cap_io_space_cap_new(
            0,              /* capDomainID  */
            0              /* capPCIDevice */
        );
}

స్థిర vtd_cte_t *lookup_vtd_context_స్థానం(cap_t cap)
{
    uint32_t   vtd_మూలం_సూచీ;
    uint32_t   vtd_context_సూచీ;
    uint32_t   pci_request_id;
    vtd_rte_t *vtd_మూలం_స్థానం;
    vtd_cte_t *vtd_context;
    vtd_cte_t *vtd_context_స్థానం;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_io_space_cap:
        pci_request_id = cap_io_space_cap_get_capPCIDevice(cap);
        ఆపు;

    స్థితి cap_io_పుట_పట్టిక_cap:
        pci_request_id = cap_io_పుట_పట్టిక_cap_get_cap_IOPTIOASID(cap);
        ఆపు;

    స్థితి cap_చట్రం_cap:
        pci_request_id = cap_చట్రం_cap_get_cap_FMapped_ASID(cap);
        ఆపు;

    అప్రమేయ:
        fail("Invalid cap type");
    }

    vtd_మూలం_సూచీ = get_pci_bus(pci_request_id);
    vtd_మూలం_స్థానం = x86_KSvtd_మూలం_పట్టిక + vtd_మూలం_సూచీ;

    vtd_context = (vtd_cte_t *)paddr_గమ్యం_pptr(vtd_rte_ptr_get_ctp(vtd_మూలం_స్థానం));
    vtd_context_సూచీ = (get_pci_dev(pci_request_id) << 3) | get_pci_fun(pci_request_id);
    vtd_context_స్థానం = &vtd_context[vtd_context_సూచీ];

    తిరిగి vtd_context_స్థానం;
}

స్థిర lookupIOPTSlot_ret_t lookup_IOPTSlot_resolve_స్థాయిలు(vtd_pte_t *iopt, మాట_t translation,
                                                          మాట_t స్థాయిలు_గమ్యం_resolve, మాట_t స్థాయిలు_మిగిలిన)
{
    lookupIOPTSlot_ret_t ret;

    మాట_t      iopt_సూచీ = 0;
    vtd_pte_t   *iopt_స్థానం_2 = 0;
    vtd_pte_t   *తదుపరి_iopt_స్థానం = 0;

    ఒకవేళ (iopt == 0) {
        ret.iopt_స్థానం = 0;
        ret.level = స్థాయిలు_మిగిలిన;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    iopt_సూచీ = (translation  >> (VTD_PT_సూచీ_బిట్లు * (x86KSnumIOPTLevels - 1 - (స్థాయిలు_గమ్యం_resolve - స్థాయిలు_మిగిలిన)))) &
                 ముసుగు(VTD_PT_సూచీ_బిట్లు);
    iopt_స్థానం_2 = iopt + iopt_సూచీ;

    ఒకవేళ (!vtd_pte_ptr_get_write(iopt_స్థానం_2) || స్థాయిలు_మిగిలిన == 0) {
        ret.iopt_స్థానం = iopt_స్థానం_2;
        ret.level = స్థాయిలు_మిగిలిన;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    }
    తదుపరి_iopt_స్థానం = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_pte_ptr_get_addr(iopt_స్థానం_2));
    తిరిగి lookup_IOPTSlot_resolve_స్థాయిలు(తదుపరి_iopt_స్థానం, translation, స్థాయిలు_గమ్యం_resolve, స్థాయిలు_మిగిలిన - 1);
}


స్థిర అంతఃవరుస lookupIOPTSlot_ret_t lookupIOPTSlot(vtd_pte_t *iopt, మాట_t io_చిరునామా)
{
    lookupIOPTSlot_ret_t ret;

    ఒకవేళ (iopt == 0) {
        ret.iopt_స్థానం    = 0;
        ret.level       = 0;
        ret.స్థితి_2      = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    } లేకపోతే {
        తిరిగి lookup_IOPTSlot_resolve_స్థాయిలు(iopt, io_చిరునామా >> పుట_బిట్లు,
                                             x86KSnumIOPTLevels - 1, x86KSnumIOPTLevels - 1);
    }
}

శూన్య unmap_VTDContext_నమోదు(cap_t cap)
{
    vtd_cte_t *cte = lookup_vtd_context_స్థానం(cap);
    ప్రతిజ్ఞా(cte != 0);
    *cte = vtd_cte_new(
               0,
               false,
               0,
               0,
               0,
               false
           );

    flushCacheRange(cte, VTD_CTE_కొలత_బిట్లు);
    invalidate_iotlb();
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి;
}

స్థిర exception_t performX86IOPTInvocationUnmap(cap_t cap, cte_t *ct_స్థానం)
{
    delete_IOPage_పట్టిక(cap);
    cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(cap, 0);
    ct_స్థానం->cap = cap;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X86_IOPTInvocation_Map_Context_మూలం(cap_t cap, cte_t *ct_స్థానం, vtd_cte_t vtd_cte,
                                                          vtd_cte_t *vtd_context_స్థానం)
{
    *vtd_context_స్థానం = vtd_cte;
    flushCacheRange(vtd_context_స్థానం, VTD_CTE_కొలత_బిట్లు);
    ct_స్థానం->cap = cap;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performX86IOPTInvocationMapPT(cap_t cap, cte_t *ct_స్థానం, vtd_pte_t iopte, vtd_pte_t *iopt_స్థానం)
{
    *iopt_స్థానం = iopte;
    flushCacheRange(iopt_స్థానం, VTD_PTE_కొలత_బిట్లు);
    ct_స్థానం->cap = cap;

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeX86IOPTInvocation(
    మాట_t       invLabel,
    మాట_t       నిడివి,
    cte_t       *స్థానం,
    cap_t        cap,
    మాట_t      *బఫరు
)
{
    cap_t      io_space;
    paddr_t    paddr;
    uint32_t   pci_request_id;
    మాట_t   io_చిరునామా;
    uint16_t   domain_id;
    vtd_cte_t *vtd_context_స్థానం;
    vtd_pte_t *vtd_pte;

    ఒకవేళ (invLabel == X86_IOPage_పట్టిక_Unmap) {
        ఒకవేళ (!isFinalCapability(స్థానం)) {
            userError("X86IOPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performX86IOPTInvocationUnmap(cap, స్థానం);
    }

    ఒకవేళ (invLabel != X86_IOPage_పట్టిక_Map) {
        userError("X86IOPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ || నిడివి < 1) {
        userError("X86IOPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_చిరునామా   = getSyscallArg(0, బఫరు) & ~ముసుగు(VTD_PT_సూచీ_బిట్లు + se_L4_పుట_బిట్లు);

    ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("X86IOPageTableMap: IO page table is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86IOPageTableMap: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);
    domain_id = cap_io_space_cap_get_capDomainID(io_space);
    ఒకవేళ (pci_request_id == asidInvalid) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr(VTD_PTE_PTR(cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap)));
    vtd_context_స్థానం = lookup_vtd_context_స్థానం(io_space);

    ఒకవేళ (!vtd_cte_ptr_get_present(vtd_context_స్థానం)) {

        /* 1st Level Page Table */
        vtd_cte_t vtd_cte = vtd_cte_new(
                                domain_id,                  /* domain ID                   */
                                false,                      /* RMRR                        */
                                x86KSnumIOPTLevels - 2,     /* addr width (x = levels - 2) */
                                paddr,                      /* address space root          */
                                0,                          /* translation type            */
                                true                        /* present                     */
                            );

        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTLevel(cap, 0);
        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIOASID(cap, pci_request_id);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_X86_IOPTInvocation_Map_Context_మూలం(cap, స్థానం, vtd_cte, vtd_context_స్థానం);
    } లేకపోతే {
        lookupIOPTSlot_ret_t lu_ret;
        vtd_pte_t   iopte;

        vtd_pte = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_cte_ptr_get_asr(vtd_context_స్థానం));
        lu_ret  = lookupIOPTSlot(vtd_pte, io_చిరునామా);

        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret.level = x86KSnumIOPTLevels - lu_ret.level;
        ఒకవేళ (vtd_pte_ptr_get_addr(lu_ret.iopt_స్థానం) != 0) {
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        iopte = vtd_pte_new(
                    paddr,      /* physical addr            */
                    1,          /* write permission flag    */
                    1           /* read  permission flag    */
                );

        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(cap, 1);
        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTLevel(cap, lu_ret.level);
        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIOASID(cap, pci_request_id);
        cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTMapped_చిరునామా(cap, io_చిరునామా);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performX86IOPTInvocationMapPT(cap, స్థానం, iopte, lu_ret.iopt_స్థానం);
    }
}

స్థిర exception_t performX86IOInvocationMap(cap_t cap, cte_t *ct_స్థానం, vtd_pte_t iopte, vtd_pte_t *iopt_స్థానం)
{
    ct_స్థానం->cap = cap;
    *iopt_స్థానం = iopte;
    flushCacheRange(iopt_స్థానం, VTD_PTE_కొలత_బిట్లు);

    తిరిగి EXCEPTION_NONE;
}


exception_t decodeX86IOMapInvocation(
    మాట_t       నిడివి,
    cte_t       *స్థానం,
    cap_t        cap,
    మాట_t      *బఫరు
)
{
    cap_t      io_space;
    మాట_t     io_చిరునామా;
    uint32_t   pci_request_id;
    vtd_cte_t *vtd_context_స్థానం;
    vtd_pte_t *vtd_pte;
    vtd_pte_t  iopte;
    paddr_t    paddr;
    lookupIOPTSlot_ret_t lu_ret;
    vm_rights_t చట్రం_cap_rights;
    seL4_CapRights_t dma_cap_rights_ముసుగు;

    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ || నిడివి < 2) {
        userError("X86PageMapIO: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_చట్రం_cap_get_cap_FSize(cap) != X86_Small_పుట) {
        userError("X86PageMapIO: Invalid page size.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("X86PageMapIO: Page already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_చిరునామా  = getSyscallArg(1, బఫరు) & ~ముసుగు(పుట_బిట్లు);
    paddr       = pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap));

    ఒకవేళ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("X86PageMapIO: Invalid IO space capability.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pci_request_id = cap_io_space_cap_get_capPCIDevice(io_space);

    ఒకవేళ (pci_request_id == asidInvalid) {
        userError("X86PageMapIO: Invalid PCI device.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vtd_context_స్థానం = lookup_vtd_context_స్థానం(io_space);

    ఒకవేళ (!vtd_cte_ptr_get_present(vtd_context_స్థానం)) {
        /* 1st Level Page Table is not installed */
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vtd_pte = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_cte_ptr_get_asr(vtd_context_స్థానం));
    lu_ret  = lookupIOPTSlot(vtd_pte, io_చిరునామా);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (vtd_pte_ptr_get_addr(lu_ret.iopt_స్థానం) != 0) {
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    dma_cap_rights_ముసుగు = rights_From_మాట(getSyscallArg(0, బఫరు));
    చట్రం_cap_rights    = cap_చట్రం_cap_get_cap_FVMRights(cap);

    bool_t లిఖ = seL4_CapRights_get_capAllowWrite(dma_cap_rights_ముసుగు) && (చట్రం_cap_rights == VMReadWrite);
    bool_t పఠ = seL4_CapRights_get_capAllowRead(dma_cap_rights_ముసుగు) && (చట్రం_cap_rights != VMKernelOnly);
    ఒకవేళ (లిఖ || పఠ) {
        iopte = vtd_pte_new(paddr, !!లిఖ, !!పఠ);
    } లేకపోతే {
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(cap, X86_MappingIOSpace);
    cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, pci_request_id);
    cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, io_చిరునామా);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి performX86IOInvocationMap(cap, స్థానం, iopte, lu_ret.iopt_స్థానం);
}

శూన్య delete_IOPage_పట్టిక(cap_t io_pt_cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    uint32_t             level;
    మాట_t               io_చిరునామా;
    vtd_cte_t           *vtd_context_స్థానం;
    vtd_pte_t           *vtd_pte;

    ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(io_pt_cap, 0);
        level = cap_io_పుట_పట్టిక_cap_get_cap_IOPTLevel(io_pt_cap);
        vtd_context_స్థానం = lookup_vtd_context_స్థానం(io_pt_cap);

        ఒకవేళ (!vtd_cte_ptr_get_present(vtd_context_స్థానం)) {
            తిరిగి;
        }

        vtd_pte = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_cte_ptr_get_asr(vtd_context_స్థానం));

        ఒకవేళ (level == 0) {
            /* if we have been overmapped or something */
            ఒకవేళ (pptr_గమ్యం_paddr(vtd_pte) != pptr_గమ్యం_paddr((శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(io_pt_cap))) {
                తిరిగి;
            }
            *vtd_context_స్థానం = vtd_cte_new(
                                    0,      /* Domain ID          */
                                    false,  /* RMRR               */
                                    0,      /* Address Width      */
                                    0,      /* Address Space Root */
                                    0,      /* Translation Type   */
                                    0       /* Present            */
                                );
            flushCacheRange(vtd_context_స్థానం, VTD_CTE_కొలత_బిట్లు);
        } లేకపోతే {
            io_చిరునామా = cap_io_పుట_పట్టిక_cap_get_cap_IOPTMapped_చిరునామా(io_pt_cap);
            lu_ret = lookup_IOPTSlot_resolve_స్థాయిలు(vtd_pte, io_చిరునామా >> పుట_బిట్లు, level - 1, level - 1);

            /* if we have been overmapped or something */
            ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE || lu_ret.level != 0) {
                తిరిగి;
            }
            ఒకవేళ (vtd_pte_ptr_get_addr(lu_ret.iopt_స్థానం) != pptr_గమ్యం_paddr((శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(
                                                                           io_pt_cap))) {
                తిరిగి;
            }
            *lu_ret.iopt_స్థానం = vtd_pte_new(
                                   0,  /* Physical Address */
                                   0,  /* Read Permission  */
                                   0   /* Write Permission */
                               );
            flushCacheRange(lu_ret.iopt_స్థానం, VTD_PTE_కొలత_బిట్లు);
        }
        invalidate_iotlb();
    }
}

శూన్య unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    మాట_t               io_చిరునామా;
    vtd_cte_t           *vtd_context_స్థానం;
    vtd_pte_t           *vtd_pte;

    io_చిరునామా  = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
    vtd_context_స్థానం = lookup_vtd_context_స్థానం(cap);


    ఒకవేళ (!vtd_cte_ptr_get_present(vtd_context_స్థానం)) {
        తిరిగి;
    }

    vtd_pte = (vtd_pte_t *)paddr_గమ్యం_pptr(vtd_cte_ptr_get_asr(vtd_context_స్థానం));

    lu_ret  = lookupIOPTSlot(vtd_pte, io_చిరునామా);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE || lu_ret.level != 0) {
        తిరిగి;
    }

    ఒకవేళ (vtd_pte_ptr_get_addr(lu_ret.iopt_స్థానం) != pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap))) {
        తిరిగి;
    }

    *lu_ret.iopt_స్థానం = vtd_pte_new(
                           0,  /* Physical Address */
                           0,  /* Read Permission  */
                           0   /* Write Permission */
                       );

    flushCacheRange(lu_ret.iopt_స్థానం, VTD_PTE_కొలత_బిట్లు);
    invalidate_iotlb();
}

exception_t performX86IOUnMapInvocation(cap_t cap, cte_t *ct_స్థానం)
{
    unmapIOPage(ct_స్థానం->cap);

    ct_స్థానం->cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(ct_స్థానం->cap, 0);
    ct_స్థానం->cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(ct_స్థానం->cap, X86_MappingNone);
    ct_స్థానం->cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(ct_స్థానం->cap, asidInvalid);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeX86IOSpaceInvocation(మాట_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    తిరిగి EXCEPTION_SYSCALL_ERROR;
}

#అయితే_ముగింపు /* CONFIG_IOMMU */
