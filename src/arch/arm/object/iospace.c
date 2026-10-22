/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_TK1_SMMU

#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <arch/api/invocation.h>
#અન્તર્ભાવ <arch/object/iospace.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/smmu.h>


પ્રકારવ્યાખ્યા સંરચના lookupIOPDSlot_ret {
    exception_t status;
    iopde_t     *iopdSlot;
} lookupIOPDSlot_ret_t;

પ્રકારવ્યાખ્યા સંરચના lookupIOPTSlot_ret {
    exception_t status;
    iopte_t     *ioptSlot;
} lookupIOPTSlot_ret_t;


#define IOPDE_VALID_MASK    0xe0000000
#define IOPTE_EMPTY_MASK    0xe0000000

સ્થિર bool_t isIOPDEValid(iopde_t *iopde)
{
    પ્રતિજ્ઞા(iopde != 0);
    પાછા (iopde->શબ્દો[0] & IOPDE_VALID_MASK) != 0;
}

સ્થિર bool_t isIOPTEEmpty(iopte_t *iopte)
{
    પ્રતિજ્ઞા(iopte != 0);
    પાછા (iopte->શબ્દો[0] & IOPTE_EMPTY_MASK) == 0;
}


સ્થિર lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t index = plat_smmu_iopd_index(io_address);
    ret.status = EXCEPTION_NONE;
    ret.iopdSlot = iopd + index;
    પાછા ret;
}

સ્થિર lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t index;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_address);
    જો (pd_ret.status != EXCEPTION_NONE) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        પાછા pt_ret;
    }

    જો (!isIOPDEValid(pd_ret.iopdSlot) ||
        iopde_ptr_get_page_size(pd_ret.iopdSlot) != iopde_iopde_pt) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        પાછા pt_ret;
    }

    index = plat_smmu_iopt_index(io_address);
    pt = (iopte_t *)paddr_to_pptr(iopde_iopde_pt_ptr_get_address(pd_ret.iopdSlot));

    જો (pt == 0) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        પાછા pt_ret;
    }

    pt_ret.status = EXCEPTION_NONE;
    pt_ret.ioptSlot = pt + index;
    પાછા pt_ret;
}

BOOT_CODE seL4_SlotRegion create_iospace_caps(cap_t મૂળ_cnode_cap)
{
    seL4_SlotPos start = ndks_boot.slot_pos_cur;
    seL4_SlotPos end = 0;
    cap_t        io_space_cap;
    પૂર્ણાંક i = 0;
    પૂર્ણાંક અંક_smmu = plat_smmu_init();

    જો (અંક_smmu == 0) {
        printf("SMMU init failure\n");
        પાછા S_REG_EMPTY;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    માટે (i = 1; i <= અંક_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        જો (!provide_cap(મૂળ_cnode_cap, io_space_cap)) {
            પાછા S_REG_EMPTY;
        }
    }
    end = ndks_boot.slot_pos_cur;
    printf("Region [%x to %x) for SMMU caps\n", (unsigned પૂર્ણાંક)start, (unsigned પૂર્ણાંક)end);
    પાછા (seL4_SlotRegion) {
        start, end
    };
}

સ્થિર exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *slot, iopde_t *iopdSlot,
                                               iopde_t iopde)
{


    *iopdSlot = iopde;
    cleanCacheRange_RAM((word_t)iopdSlot,
                        ((word_t)iopdSlot) + કદ(iopde_t),
                        addrFromPPtr(iopdSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;
    setThreadState(ksCurThread, ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    word_t       invLabel,
    uint32_t     length,
    cte_t       *slot,
    cap_t        cap,
    word_t      *buffer
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

    જો (invLabel == ARMIOPageTableUnmap) {
        જો (!isFinalCapability(slot)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        deleteIOPageTable(slot->cap);
        slot->cap = cap_io_page_table_cap_set_capIOPTIsMapped(slot->cap, 0);

        setThreadState(ksCurThread, ThreadState_Restart);
        પાછા EXCEPTION_NONE;
    }

    જો (current_extra_caps.excaprefs[0] == ખાલી || length < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (invLabel != ARMIOPageTableMap) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_address   = getSyscallArg(0, buffer) & ~MASK(SMMU_IOPD_INDEX_SHIFT);

    જો (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    પ્રતિજ્ઞા(asid != asidInvalid);

    paddr = pptr_to_paddr((શૂન્ય *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_address);

    જો (isIOPDEValid(lu_ret.iopdSlot)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
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

    પાછા performARMIOPTInvocationMap(cap, slot, lu_ret.iopdSlot, iopde);
}

સ્થિર exception_t performARMIOMapInvocation(cap_t cap, cte_t *slot, iopte_t *ioptSlot,
                                             iopte_t iopte)
{
    *ioptSlot = iopte;
    cleanCacheRange_RAM((word_t)ioptSlot,
                        ((word_t)ioptSlot) + કદ(iopte_t),
                        addrFromPPtr(ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;

    setThreadState(ksCurThread, ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    word_t       invLabel,
    uint32_t     length,
    cte_t       *slot,
    cap_t        cap,
    word_t      *buffer
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

    જો (current_extra_caps.excaprefs[0] == ખાલી || length < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (generic_frame_cap_get_capFSize(cap) != ARMSmallPage) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_small_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_address  = getSyscallArg(1, buffer) & ~MASK(PAGE_BITS);
    paddr       = pptr_to_paddr((શૂન્ય *)cap_small_frame_cap_get_capFBasePtr(cap));

    જો (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    પ્રતિજ્ઞા(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);
    જો (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (!isIOPTEEmpty(lu_ret.ioptSlot)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    frame_cap_rights = cap_small_frame_cap_get_capFVMRights(cap);
    dma_cap_rights_mask = rightsFromWord(getSyscallArg(0, buffer));

    જો ((frame_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_mask)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } નહીંતર જો (frame_cap_rights == VMReadWrite) {
        જો (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } નહીંતર જો (!seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } નહીંતર જો (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } નહીંતર {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

    } નહીંતર {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_frame_cap_set_capFIsIOSpace(cap, 1);
    cap = cap_small_frame_cap_set_capFMappedASID(cap, asid);
    cap = cap_small_frame_cap_set_capFMappedAddress(cap, io_address);

    પાછા performARMIOMapInvocation(cap, slot, lu_ret.ioptSlot, iopte);
}


શૂન્ય deleteIOPageTable(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    word_t io_address;
    જો (cap_io_page_table_cap_get_capIOPTIsMapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_table_cap_set_capIOPTIsMapped(io_pt_cap, 0);
        asid = cap_io_page_table_cap_get_capIOPTASID(io_pt_cap);
        પ્રતિજ્ઞા(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_address = cap_io_page_table_cap_get_capIOPTMappedAddress(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_address);
        જો (lu_ret.status != EXCEPTION_NONE) {
            પાછા;
        }

        જો (!isIOPDEValid(lu_ret.iopdSlot) ||
            iopde_ptr_get_page_size(lu_ret.iopdSlot) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_address(lu_ret.iopdSlot) != (pptr_to_paddr((શૂન્ય *)cap_io_page_table_cap_get_capIOPTBasePtr(
                                                                                  io_pt_cap)))) {
            પાછા;
        }

        *lu_ret.iopdSlot = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((word_t)lu_ret.iopdSlot,
                            ((word_t)lu_ret.iopdSlot) + કદ(iopde_t),
                            addrFromPPtr(lu_ret.iopdSlot));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

શૂન્ય unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    word_t  io_address;
    uint32_t asid;

    io_address = cap_small_frame_cap_get_capFMappedAddress(cap);
    asid = cap_small_frame_cap_get_capFMappedASID(cap);
    પ્રતિજ્ઞા(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);

    જો (lu_ret.status != EXCEPTION_NONE) {
        પાછા;
    }
    જો (iopte_ptr_get_address(lu_ret.ioptSlot) != pptr_to_paddr((શૂન્ય *)cap_small_frame_cap_get_capFBasePtr(cap))) {
        પાછા;
    }

    *lu_ret.ioptSlot = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((word_t)lu_ret.ioptSlot,
                        ((word_t)lu_ret.ioptSlot) + કદ(iopte_t),
                        addrFromPPtr(lu_ret.ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    પાછા;
}

શૂન્ય clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    word_t   size = BIT((SMMU_PD_INDEX_BITS));
    પ્રતિજ્ઞા(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((શૂન્ય *)pd, 0, size);
    cleanCacheRange_RAM((word_t)pd, (word_t)pd + size, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    પાછા;
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

    પાછા EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    પાછા EXCEPTION_SYSCALL_ERROR;
}
#endif /* end of CONFIG_TK1_SMMU */
