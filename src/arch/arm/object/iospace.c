/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_TK1_SMMU

#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/smmu.h>


प्रकारनिर्देश संरचना lookupIOPDSlot_ret {
    exception_t स्थितिः;
    iopde_t     *iopdSlot;
} lookupIOPDSlot_ret_t;

प्रकारनिर्देश संरचना lookupIOPTSlot_ret {
    exception_t स्थितिः;
    iopte_t     *ioptSlot;
} lookupIOPTSlot_ret_t;


#परिभाषय IOPDE_VALID_आवरणम्    0xe0000000
#परिभाषय IOPTE_रिक्तानि_आवरणम्    0xe0000000

स्थिर bool_t isIOPDEValid(iopde_t *iopde)
{
    प्रतिज्ञा(iopde != 0);
    प्रत्यागम (iopde->शब्दाः[0] & IOPDE_VALID_आवरणम्) != 0;
}

स्थिर bool_t isIOPTEEmpty(iopte_t *iopte)
{
    प्रतिज्ञा(iopte != 0);
    प्रत्यागम (iopte->शब्दाः[0] & IOPTE_रिक्तानि_आवरणम्) == 0;
}


स्थिर lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, word_t io_स्थानाङ्कः)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t सूचकांक = plat_smmu_iopd_सूचकांक(io_स्थानाङ्कः);
    ret.स्थितिः = EXCEPTION_NONE;
    ret.iopdSlot = iopd + सूचकांक;
    प्रत्यागम ret;
}

स्थिर lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, word_t io_स्थानाङ्कः)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t सूचकांक;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_स्थानाङ्कः);
    यदि (pd_ret.स्थितिः != EXCEPTION_NONE) {
        pt_ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        प्रत्यागम pt_ret;
    }

    यदि (!isIOPDEValid(pd_ret.iopdSlot) ||
        iopde_ptr_get_page_आकार(pd_ret.iopdSlot) != iopde_iopde_pt) {
        pt_ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        प्रत्यागम pt_ret;
    }

    सूचकांक = plat_smmu_iopt_सूचकांक(io_स्थानाङ्कः);
    pt = (iopte_t *)paddr_गन्तव्यम्_pptr(iopde_iopde_pt_ptr_get_स्थानाङ्कः(pd_ret.iopdSlot));

    यदि (pt == 0) {
        pt_ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        pt_ret.ioptSlot = 0;
        प्रत्यागम pt_ret;
    }

    pt_ret.स्थितिः = EXCEPTION_NONE;
    pt_ret.ioptSlot = pt + सूचकांक;
    प्रत्यागम pt_ret;
}

BOOT_संहिता seL4_SlotRegion create_iospace_caps(cap_t मूलम्_cnode_cap)
{
    seL4_SlotPos start = ndks_boot.slot_pos_cur;
    seL4_SlotPos अन्तः_2 = 0;
    cap_t        io_space_cap;
    पूर्णाङ्क i = 0;
    पूर्णाङ्क अङ्कः_smmu = plat_smmu_init();

    यदि (अङ्कः_smmu == 0) {
        प्रारूप_लिख("SMMU init failure\n");
        प्रत्यागम S_REG_रिक्तानि;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    प्रति (i = 1; i <= अङ्कः_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        यदि (!provide_cap(मूलम्_cnode_cap, io_space_cap)) {
            प्रत्यागम S_REG_रिक्तानि;
        }
    }
    अन्तः_2 = ndks_boot.slot_pos_cur;
    प्रारूप_लिख("Region [%x to %x) for SMMU caps\n", (अचिह्नित पूर्णाङ्क)start, (अचिह्नित पूर्णाङ्क)अन्तः_2);
    प्रत्यागम (seL4_SlotRegion) {
        start, अन्तः_2
    };
}

स्थिर exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *slot, iopde_t *iopdSlot,
                                               iopde_t iopde)
{


    *iopdSlot = iopde;
    cleanCacheRange_RAM((word_t)iopdSlot,
                        ((word_t)iopdSlot) + परिमाण(iopde_t),
                        addrFromPPtr(iopdSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;
    समुच्चयः_Thread_State(ksCurThread, ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    word_t       invLabel,
    uint32_t     दीर्घता,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    word_t     io_स्थानाङ्कः;
    word_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    यदि (invLabel == ARMIOPage_सारणी_Unmap) {
        यदि (!isFinalCapability(slot)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_सारणी(slot->cap);
        slot->cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(slot->cap, 0);

        समुच्चयः_Thread_State(ksCurThread, ThreadState_Restart);
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (current_extra_caps.excaprefs[0] == रिक्त || दीर्घता < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (invLabel != ARMIOPage_सारणी_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_स्थानाङ्कः   = getSyscallArg(0, बफर) & ~आवरणम्(SMMU_IOPD_सूचकांक_सरणम्);

    यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    प्रतिज्ञा(asid != asidInvalid);

    paddr = pptr_गन्तव्यम्_paddr((शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_स्थानाङ्कः);

    यदि (isIOPDEValid(lu_ret.iopdSlot)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTASID(cap, asid);
    cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTMapped_स्थानाङ्कः(cap, io_स्थानाङ्कः);

    प्रत्यागम performARMIOPTInvocationMap(cap, slot, lu_ret.iopdSlot, iopde);
}

स्थिर exception_t performARMIOMapInvocation(cap_t cap, cte_t *slot, iopte_t *ioptSlot,
                                             iopte_t iopte)
{
    *ioptSlot = iopte;
    cleanCacheRange_RAM((word_t)ioptSlot,
                        ((word_t)ioptSlot) + परिमाण(iopte_t),
                        addrFromPPtr(ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    slot->cap = cap;

    समुच्चयः_Thread_State(ksCurThread, ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    word_t       invLabel,
    uint32_t     दीर्घता,
    cte_t       *slot,
    cap_t        cap,
    word_t      *बफर
)
{
    cap_t      io_space;
    paddr_t    io_स्थानाङ्कः;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     frame_cap_rights;
    seL4_CapRights_t    dma_cap_rights_आवरणम्;
    lookupIOPTSlot_ret_t lu_ret;

    यदि (current_extra_caps.excaprefs[0] == रिक्त || दीर्घता < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (generic_frame_cap_get_capFSize(cap) != ARMSmallPage) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_small_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_स्थानाङ्कः  = getSyscallArg(1, बफर) & ~आवरणम्(PAGE_द्वयङ्काः);
    paddr       = pptr_गन्तव्यम्_paddr((शून्य *)cap_small_frame_cap_get_capFBasePtr(cap));

    यदि (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    प्रतिज्ञा(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_स्थानाङ्कः);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (!isIOPTEEmpty(lu_ret.ioptSlot)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    frame_cap_rights = cap_small_frame_cap_get_capFVMRights(cap);
    dma_cap_rights_आवरणम् = rightsFromWord(getSyscallArg(0, बफर));

    यदि ((frame_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_आवरणम्)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } अन्यथा यदि (frame_cap_rights == VMReadWrite) {
        यदि (seL4_CapRights_get_capAllowRead(dma_cap_rights_आवरणम्) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_आवरणम्)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा यदि (!seL4_CapRights_get_capAllowRead(dma_cap_rights_आवरणम्) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_आवरणम्)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा यदि (seL4_CapRights_get_capAllowRead(dma_cap_rights_आवरणम्) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_आवरणम्)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } अन्यथा {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

    } अन्यथा {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_frame_cap_समुच्चयः_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asid);
    cap = cap_small_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, io_स्थानाङ्कः);

    प्रत्यागम performARMIOMapInvocation(cap, slot, lu_ret.ioptSlot, iopte);
}


शून्य delete_IOPage_सारणी(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    word_t io_स्थानाङ्कः;
    यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_page_सारणी_cap_समुच्चयः_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_page_सारणी_cap_get_cap_IOPTASID(io_pt_cap);
        प्रतिज्ञा(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_स्थानाङ्कः = cap_io_page_सारणी_cap_get_cap_IOPTMapped_स्थानाङ्कः(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_स्थानाङ्कः);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम;
        }

        यदि (!isIOPDEValid(lu_ret.iopdSlot) ||
            iopde_ptr_get_page_आकार(lu_ret.iopdSlot) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_स्थानाङ्कः(lu_ret.iopdSlot) != (pptr_गन्तव्यम्_paddr((शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            प्रत्यागम;
        }

        *lu_ret.iopdSlot = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((word_t)lu_ret.iopdSlot,
                            ((word_t)lu_ret.iopdSlot) + परिमाण(iopde_t),
                            addrFromPPtr(lu_ret.iopdSlot));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

शून्य unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    word_t  io_स्थानाङ्कः;
    uint32_t asid;

    io_स्थानाङ्कः = cap_small_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
    asid = cap_small_frame_cap_get_capFMappedASID(cap);
    प्रतिज्ञा(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_स्थानाङ्कः);

    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }
    यदि (iopte_ptr_get_स्थानाङ्कः(lu_ret.ioptSlot) != pptr_गन्तव्यम्_paddr((शून्य *)cap_small_frame_cap_get_capFBasePtr(cap))) {
        प्रत्यागम;
    }

    *lu_ret.ioptSlot = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((word_t)lu_ret.ioptSlot,
                        ((word_t)lu_ret.ioptSlot) + परिमाण(iopte_t),
                        addrFromPPtr(lu_ret.ioptSlot));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    प्रत्यागम;
}

शून्य clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    word_t   आकार = BIT((SMMU_PD_सूचकांक_द्वयङ्काः));
    प्रतिज्ञा(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    स्मृतिं_पूरय((शून्य *)pd, 0, आकार);
    cleanCacheRange_RAM((word_t)pd, (word_t)pd + आकार, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    प्रत्यागम;
}

exception_t performPageInvocationUnmapIO(
    cap_t        cap,
    cte_t       *slot
)
{
    unmapIOPage(slot->cap);
    slot->cap = cap_small_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(slot->cap, 0);
    slot->cap = cap_small_frame_cap_समुच्चयः_cap_FIs_IOSpace(slot->cap, 0);
    slot->cap = cap_small_frame_cap_समुच्चयः_cap_FMapped_ASID(slot->cap, asidInvalid);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(word_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    प्रत्यागम EXCEPTION_SYSCALL_ERROR;
}
#यदि_अन्तः /* end of CONFIG_TK1_SMMU */
