/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_TK1_SMMU

#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <arch/api/invocation.h>
#அந்தர்பாவ <arch/object/iospace.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/smmu.h>


வகைவரை கட்டமைப்பு lookupIOPDSlot_ret {
    exception_t status;
    iopde_t     *iopdSlot;
} lookupIOPDSlot_ret_t;

வகைவரை கட்டமைப்பு lookupIOPTSlot_ret {
    exception_t status;
    iopte_t     *ioptSlot;
} lookupIOPTSlot_ret_t;


#define IOPDE_VALID_MASK    0xe0000000
#define IOPTE_EMPTY_MASK    0xe0000000

நிலையான bool_t isIOPDEValid(iopde_t *iopde)
{
    ப்ரதிஜ்ஞா(iopde != 0);
    திரும்பு (iopde->சொற்கள்[0] & IOPDE_VALID_MASK) != 0;
}

நிலையான bool_t isIOPTEEmpty(iopte_t *iopte)
{
    ப்ரதிஜ்ஞா(iopte != 0);
    திரும்பு (iopte->சொற்கள்[0] & IOPTE_EMPTY_MASK) == 0;
}


நிலையான lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t index = plat_smmu_iopd_index(io_address);
    ret.status = EXCEPTION_NONE;
    ret.iopdSlot = iopd + index;
    திரும்பு ret;
}

நிலையான lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, word_t io_address)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t index;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_address);
    எனில் (pd_ret.status != EXCEPTION_NONE) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        திரும்பு pt_ret;
    }

    எனில் (!isIOPDEValid(pd_ret.iopdSlot) ||
        iopde_ptr_get_page_size(pd_ret.iopdSlot) != iopde_iopde_pt) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        திரும்பு pt_ret;
    }

    index = plat_smmu_iopt_index(io_address);
    pt = (iopte_t *)paddr_to_pptr(iopde_iopde_pt_ptr_get_address(pd_ret.iopdSlot));

    எனில் (pt == 0) {
        pt_ret.status = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        திரும்பு pt_ret;
    }

    pt_ret.status = EXCEPTION_NONE;
    pt_ret.ioptSlot = pt + index;
    திரும்பு pt_ret;
}

BOOT_CODE seL4_SlotRegion create_iospace_caps(cap_t வேர்_cnode_cap)
{
    seL4_SlotPos start = ndks_boot.slot_pos_cur;
    seL4_SlotPos end = 0;
    cap_t        io_space_cap;
    முழு i = 0;
    முழு இலக்கம்_smmu = plat_smmu_init();

    எனில் (இலக்கம்_smmu == 0) {
        printf("SMMU init failure\n");
        திரும்பு S_REG_EMPTY;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    க்கு (i = 1; i <= இலக்கம்_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        எனில் (!provide_cap(வேர்_cnode_cap, io_space_cap)) {
            திரும்பு S_REG_EMPTY;
        }
    }
    end = ndks_boot.slot_pos_cur;
    printf("Region [%x to %x) for SMMU caps\n", (unsigned முழு)start, (unsigned முழு)end);
    திரும்பு (seL4_SlotRegion) {
        start, end
    };
}

நிலையான exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *slot, iopde_t *iopdSlot,
                                               iopde_t iopde)
{


    *iopdSlot = iopde;
    cleanCacheRange_RAM((word_t)iopdSlot,
                        ((word_t)iopdSlot) + அளவு(iopde_t),
                        addrFromPPtr(iopdSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;
    setThreadState(ksCurThread, ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
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

    எனில் (invLabel == ARMIOPageTableUnmap) {
        எனில் (!isFinalCapability(slot)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        deleteIOPageTable(slot->cap);
        slot->cap = cap_io_page_table_cap_set_capIOPTIsMapped(slot->cap, 0);

        setThreadState(ksCurThread, ThreadState_Restart);
        திரும்பு EXCEPTION_NONE;
    }

    எனில் (current_extra_caps.excaprefs[0] == வெற்று || length < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (invLabel != ARMIOPageTableMap) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_address   = getSyscallArg(0, buffer) & ~MASK(SMMU_IOPD_INDEX_SHIFT);

    எனில் (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ப்ரதிஜ்ஞா(asid != asidInvalid);

    paddr = pptr_to_paddr((வெறுமை *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_address);

    எனில் (isIOPDEValid(lu_ret.iopdSlot)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
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

    திரும்பு performARMIOPTInvocationMap(cap, slot, lu_ret.iopdSlot, iopde);
}

நிலையான exception_t performARMIOMapInvocation(cap_t cap, cte_t *slot, iopte_t *ioptSlot,
                                             iopte_t iopte)
{
    *ioptSlot = iopte;
    cleanCacheRange_RAM((word_t)ioptSlot,
                        ((word_t)ioptSlot) + அளவு(iopte_t),
                        addrFromPPtr(ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;

    setThreadState(ksCurThread, ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
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

    எனில் (current_extra_caps.excaprefs[0] == வெற்று || length < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (generic_frame_cap_get_capFSize(cap) != ARMSmallPage) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (cap_small_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_address  = getSyscallArg(1, buffer) & ~MASK(PAGE_BITS);
    paddr       = pptr_to_paddr((வெறுமை *)cap_small_frame_cap_get_capFBasePtr(cap));

    எனில் (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ப்ரதிஜ்ஞா(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (!isIOPTEEmpty(lu_ret.ioptSlot)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    frame_cap_rights = cap_small_frame_cap_get_capFVMRights(cap);
    dma_cap_rights_mask = rightsFromWord(getSyscallArg(0, buffer));

    எனில் ((frame_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_mask)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } இல்லையெனில் எனில் (frame_cap_rights == VMReadWrite) {
        எனில் (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } இல்லையெனில் எனில் (!seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } இல்லையெனில் எனில் (seL4_CapRights_get_capAllowRead(dma_cap_rights_mask) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_mask)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } இல்லையெனில் {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

    } இல்லையெனில் {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_frame_cap_set_capFIsIOSpace(cap, 1);
    cap = cap_small_frame_cap_set_capFMappedASID(cap, asid);
    cap = cap_small_frame_cap_set_capFMappedAddress(cap, io_address);

    திரும்பு performARMIOMapInvocation(cap, slot, lu_ret.ioptSlot, iopte);
}


வெறுமை deleteIOPageTable(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    word_t io_address;
    எனில் (cap_io_page_table_cap_get_capIOPTIsMapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_table_cap_set_capIOPTIsMapped(io_pt_cap, 0);
        asid = cap_io_page_table_cap_get_capIOPTASID(io_pt_cap);
        ப்ரதிஜ்ஞா(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_address = cap_io_page_table_cap_get_capIOPTMappedAddress(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_address);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            திரும்பு;
        }

        எனில் (!isIOPDEValid(lu_ret.iopdSlot) ||
            iopde_ptr_get_page_size(lu_ret.iopdSlot) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_address(lu_ret.iopdSlot) != (pptr_to_paddr((வெறுமை *)cap_io_page_table_cap_get_capIOPTBasePtr(
                                                                                  io_pt_cap)))) {
            திரும்பு;
        }

        *lu_ret.iopdSlot = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((word_t)lu_ret.iopdSlot,
                            ((word_t)lu_ret.iopdSlot) + அளவு(iopde_t),
                            addrFromPPtr(lu_ret.iopdSlot));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

வெறுமை unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    word_t  io_address;
    uint32_t asid;

    io_address = cap_small_frame_cap_get_capFMappedAddress(cap);
    asid = cap_small_frame_cap_get_capFMappedASID(cap);
    ப்ரதிஜ்ஞா(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_address);

    எனில் (lu_ret.status != EXCEPTION_NONE) {
        திரும்பு;
    }
    எனில் (iopte_ptr_get_address(lu_ret.ioptSlot) != pptr_to_paddr((வெறுமை *)cap_small_frame_cap_get_capFBasePtr(cap))) {
        திரும்பு;
    }

    *lu_ret.ioptSlot = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((word_t)lu_ret.ioptSlot,
                        ((word_t)lu_ret.ioptSlot) + அளவு(iopte_t),
                        addrFromPPtr(lu_ret.ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    திரும்பு;
}

வெறுமை clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    word_t   size = BIT((SMMU_PD_INDEX_BITS));
    ப்ரதிஜ்ஞா(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((வெறுமை *)pd, 0, size);
    cleanCacheRange_RAM((word_t)pd, (word_t)pd + size, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    திரும்பு;
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

    திரும்பு EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    திரும்பு EXCEPTION_SYSCALL_ERROR;
}
#endif /* end of CONFIG_TK1_SMMU */
