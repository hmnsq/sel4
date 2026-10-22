/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_TK1_SMMU

#समावेश <api/syscall.h>
#समावेश <machine/io.h>
#समावेश <kernel/thread.h>
#समावेश <arch/api/invocation.h>
#समावेश <arch/object/iospace.h>
#समावेश <arch/model/statedata.h>
#समावेश <object/structures.h>
#समावेश <linker.h>
#समावेश <plat/machine/smmu.h>


प्रकार काष्ठा lookupIOPDSlot_ret {
    exception_t status;
    iopde_t     *iopdSlot;
} lookupIOPDSlot_ret_t;

प्रकार काष्ठा lookupIOPTSlot_ret {
    exception_t status;
    iopte_t     *ioptSlot;
} lookupIOPTSlot_ret_t;


#घोषणा IOPDE_VALID_MASK    0xe0000000
#घोषणा IOPTE_EMPTY_MASK    0xe0000000

अटल bool_t isIOPDEValid(iopde_t *iopde)
{
    निश्चित(iopde != 0);
    वापस (iopde->शब्दगण[0] & IOPDE_VALID_MASK) != 0;
}

अटल bool_t isIOPTEEmpty(iopte_t *iopte)
{
    निश्चित(iopte != 0);
    वापस (iopte->शब्दगण[0] & IOPTE_EMPTY_MASK) == 0;
}


अटल lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t सूचकांक = plat_smmu_iopd_सूचकांक(io_address);
    ret.status = EXCEPTION_NONE;
    ret.iopdSlot = iopd + सूचकांक;
    वापस ret;
}

अटल lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t सूचकांक;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_address);
    अगर (pd_ret.status != EXCEPTION_NONE) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        वापस pt_ret;
    }

    अगर (!isIOPDEValid(pd_ret.iopdSlot) ||
        iopde_ptr_get_page_आकार(pd_ret.iopdSlot) != iopde_iopde_pt) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        वापस pt_ret;
    }

    सूचकांक = plat_smmu_iopt_सूचकांक(io_address);
    pt = (iopte_t *)paddr_to_pptr(iopde_iopde_pt_ptr_get_address(pd_ret.iopdSlot));

    अगर (pt == 0) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        वापस pt_ret;
    }

    pt_ret.status = EXCEPTION_NONE;
    pt_ret.ioptSlot = pt + सूचकांक;
    वापस pt_ret;
}

BOOT_CODE seL4_SlotRegion create_iospace_caps(cap_t मूल_cnode_cap)
{
    seL4_SlotPos start = ndks_boot.slot_pos_cur;
    seL4_SlotPos end = 0;
    cap_t        io_space_cap;
    पूर्णांक i = 0;
    पूर्णांक अंक_smmu = plat_smmu_init();

    अगर (अंक_smmu == 0) {
        म_लिखो("SMMU init failure\n");
        वापस S_REG_EMPTY;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    क्रम (i = 1; i <= अंक_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        अगर (!provide_cap(मूल_cnode_cap, io_space_cap)) {
            वापस S_REG_EMPTY;
        }
    }
    end = ndks_boot.slot_pos_cur;
    म_लिखो("Region [%x to %x) for SMMU caps\n", (अचिन्हित पूर्णांक)start, (अचिन्हित पूर्णांक)end);
    वापस (seL4_SlotRegion) {
        start, end
    };
}

अटल exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *slot, iopde_t *iopdSlot,
                                               iopde_t iopde)
{


    *iopdSlot = iopde;
    cleanCacheRange_RAM((word_t)iopdSlot,
                        ((word_t)iopdSlot) + माप(iopde_t),
                        addrFromPPtr(iopdSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;
    setThreadState(ksCurThread, ThreadState_Restart);
    वापस EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    word_t       invLabel,
    uint32_t     लंबाई,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    word_t     io_address;
    word_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    अगर (invLabel == ARMIOPageTableUnmap) {
        अगर (!isFinalCapability(slot)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        deleteIOPageTable(slot->cap);
        slot->cap = cap_io_page_table_cap_set_capIOPTIsMapped(slot->cap, 0);

        setThreadState(ksCurThread, ThreadState_Restart);
        वापस EXCEPTION_NONE;
    }

    अगर (current_extra_caps.excaprefs[0] == शून्य || लंबाई < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (invLabel != ARMIOPageTableMap) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_address   = getSyscallArg(0, बफर) & ~MASK(SMMU_IOPD_सूचकांक_SHIFT);

    अगर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    निश्चित(asid != asidInvalid);

    paddr = pptr_to_paddr((व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_address);

    अगर (isIOPDEValid(lu_ret.iopdSlot)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_page_table_cap_set_capIOPTIsMapped(cap, 1);
    cap = cap_io_page_table_cap_set_capIOPTASID(cap, asid);
    cap = cap_io_page_table_cap_set_capIOPTMappedAddress(cap, io_address);

    वापस performARMIOPTInvocationMap(cap, slot, lu_ret.iopdSlot, iopde);
}

अटल exception_t performARMIOMapInvocation(cap_t cap, cte_t *slot, iopte_t *ioptSlot,
                                             iopte_t iopte)
{
    *ioptSlot = iopte;
    cleanCacheRange_RAM((word_t)ioptSlot,
                        ((word_t)ioptSlot) + माप(iopte_t),
                        addrFromPPtr(ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;

    setThreadState(ksCurThread, ThreadState_Restart);
    वापस EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    word_t       invLabel,
    uint32_t     लंबाई,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    paddr_t    io_address;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     frame_cap_rights;
    seL4_CapRights_t    dma_cap_rights_mask;
    lookupIOPTSlot_ret_t lu_ret;

    अगर (current_extra_caps.excaprefs[0] == शून्य || लंबाई < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (generic_frame_cap_get_capFSize(cap) != ARMSmallPage) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_small_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_address  = getSyscallArg(1, बफर) & ~MASK(PAGE_BITS);
    paddr       = pptr_to_paddr((व्योम *)cap_small_frame_cap_get_capFBasePtr(cap));

    अगर (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    निश्चित(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);
    अगर (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (!isIOPTEEmpty(lu_ret.ioptSlot)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    frame_cap_rights = cap_small_frame_cap_get_capFVMRights(cap);
    dma_cap_rights_mask = rightsFromWord(getSyscallArg(0, बफर));

    अगर ((frame_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_mask)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } अन्यथा अगर (frame_cap_rights == VMReadWrite) {
        अगर (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा अगर (!seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा अगर (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

    } अन्यथा {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_frame_cap_set_capFIsIOSpace(cap, 1);
    cap = cap_small_frame_cap_set_capFMappedASID(cap, asid);
    cap = cap_small_frame_cap_set_capFMappedAddress(cap, io_address);

    वापस performARMIOMapInvocation(cap, slot, lu_ret.ioptSlot, iopte);
}


व्योम deleteIOPageTable(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    word_t io_address;
    अगर (cap_io_page_table_cap_get_capIOPTIsMapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_table_cap_set_capIOPTIsMapped(io_pt_cap, 0);
        asid = cap_io_page_table_cap_get_capIOPTASID(io_pt_cap);
        निश्चित(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_address = cap_io_page_table_cap_get_capIOPTMappedAddress(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_address);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            वापस;
        }

        अगर (!isIOPDEValid(lu_ret.iopdSlot) ||
            iopde_ptr_get_page_आकार(lu_ret.iopdSlot) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_address(lu_ret.iopdSlot) != (pptr_to_paddr((व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(
                                                                                  io_pt_cap)))) {
            वापस;
        }

        *lu_ret.iopdSlot = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((word_t)lu_ret.iopdSlot,
                            ((word_t)lu_ret.iopdSlot) + माप(iopde_t),
                            addrFromPPtr(lu_ret.iopdSlot));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

व्योम unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    word_t  io_address;
    uint32_t asid;

    io_address = cap_small_frame_cap_get_capFMappedAddress(cap);
    asid = cap_small_frame_cap_get_capFMappedASID(cap);
    निश्चित(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);

    अगर (lu_ret.status != EXCEPTION_NONE) {
        वापस;
    }
    अगर (iopte_ptr_get_address(lu_ret.ioptSlot) != pptr_to_paddr((व्योम *)cap_small_frame_cap_get_capFBasePtr(cap))) {
        वापस;
    }

    *lu_ret.ioptSlot = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((word_t)lu_ret.ioptSlot,
                        ((word_t)lu_ret.ioptSlot) + माप(iopte_t),
                        addrFromPPtr(lu_ret.ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    वापस;
}

व्योम clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    word_t   आकार = BIT((SMMU_PD_सूचकांक_BITS));
    निश्चित(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    स_रखो((व्योम *)pd, 0, आकार);
    cleanCacheRange_RAM((word_t)pd, (word_t)pd + आकार, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    वापस;
}

exception_t performPageInvocationUnmapIO(
    cap_t        cap,
    cte_t       *slot
)
{
    unmapIOPage(slot->cap);
    slot->cap = cap_small_frame_cap_set_capFMappedAddress(slot->cap, 0);
    slot->cap = cap_small_frame_cap_set_capFIsIOSpace(slot->cap, 0);
    slot->cap = cap_small_frame_cap_set_capFMappedASID(slot->cap, asidInvalid);

    वापस EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    वापस EXCEPTION_SYSCALL_ERROR;
}
#पूर्ण_अगर /* end of CONFIG_TK1_SMMU */
