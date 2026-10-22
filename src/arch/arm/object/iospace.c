/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_TK1_SMMU

#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/smmu.h>


రకనిర్వచనం నిర్మాణం lookupIOPDSlot_ret {
    exception_t స్థితి_2;
    iopde_t     *iopd_స్థానం;
} lookupIOPDSlot_ret_t;

రకనిర్వచనం నిర్మాణం lookupIOPTSlot_ret {
    exception_t స్థితి_2;
    iopte_t     *iopt_స్థానం;
} lookupIOPTSlot_ret_t;


#నిర్వచించు IOPDE_VALID_ముసుగు    0xe0000000
#నిర్వచించు IOPTE_ఖాళీలు_ముసుగు    0xe0000000

స్థిర bool_t isIOPDEValid(iopde_t *iopde)
{
    ప్రతిజ్ఞా(iopde != 0);
    తిరిగి (iopde->పదాలు[0] & IOPDE_VALID_ముసుగు) != 0;
}

స్థిర bool_t isIOPTEEmpty(iopte_t *iopte)
{
    ప్రతిజ్ఞా(iopte != 0);
    తిరిగి (iopte->పదాలు[0] & IOPTE_ఖాళీలు_ముసుగు) == 0;
}


స్థిర lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, మాట_t io_చిరునామా)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t సూచీ = plat_smmu_iopd_సూచీ(io_చిరునామా);
    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.iopd_స్థానం = iopd + సూచీ;
    తిరిగి ret;
}

స్థిర lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, మాట_t io_చిరునామా)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t సూచీ;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_చిరునామా);
    ఒకవేళ (pd_ret.స్థితి_2 != EXCEPTION_NONE) {
        pt_ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_స్థానం = 0;
        తిరిగి pt_ret;
    }

    ఒకవేళ (!isIOPDEValid(pd_ret.iopd_స్థానం) ||
        iopde_ptr_get_పుట_కొలత(pd_ret.iopd_స్థానం) != iopde_iopde_pt) {
        pt_ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_స్థానం = 0;
        తిరిగి pt_ret;
    }

    సూచీ = plat_smmu_iopt_సూచీ(io_చిరునామా);
    pt = (iopte_t *)paddr_గమ్యం_pptr(iopde_iopde_pt_ptr_get_చిరునామా(pd_ret.iopd_స్థానం));

    ఒకవేళ (pt == 0) {
        pt_ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_స్థానం = 0;
        తిరిగి pt_ret;
    }

    pt_ret.స్థితి_2 = EXCEPTION_NONE;
    pt_ret.iopt_స్థానం = pt + సూచీ;
    తిరిగి pt_ret;
}

BOOT_సంకేతం se_L4_స్థానం_Region create_iospace_caps(cap_t మూలం_cnode_cap)
{
    se_L4_స్థానం_Pos ప్రారంభం = ndks_boot.స్థానం_pos_cur;
    se_L4_స్థానం_Pos చివర = 0;
    cap_t        io_space_cap;
    పూర్ణాంకం i = 0;
    పూర్ణాంకం అంకె_smmu = plat_smmu_init();

    ఒకవేళ (అంకె_smmu == 0) {
        రాయి("SMMU init failure\n");
        తిరిగి S_REG_ఖాళీలు;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    కొరకు (i = 1; i <= అంకె_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, io_space_cap)) {
            తిరిగి S_REG_ఖాళీలు;
        }
    }
    చివర = ndks_boot.స్థానం_pos_cur;
    రాయి("Region [%x to %x) for SMMU caps\n", (గుర్తులేని పూర్ణాంకం)ప్రారంభం, (గుర్తులేని పూర్ణాంకం)చివర);
    తిరిగి (se_L4_స్థానం_Region) {
        ప్రారంభం, చివర
    };
}

స్థిర exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *స్థానం, iopde_t *iopd_స్థానం,
                                               iopde_t iopde)
{


    *iopd_స్థానం = iopde;
    cleanCacheRange_RAM((మాట_t)iopd_స్థానం,
                        ((మాట_t)iopd_స్థానం) + పరిమాణం(iopde_t),
                        addrFromPPtr(iopd_స్థానం));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    స్థానం->cap = cap;
    సమితి_Thread_State(ksCurThread, ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    మాట_t       invLabel,
    uint32_t     నిడివి,
    cte_t       *స్థానం,
    cap_t        cap,
    మాట_t      *బఫరు
)
{
    cap_t      io_space;
    మాట_t     io_చిరునామా;
    మాట_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    ఒకవేళ (invLabel == ARMIOPage_పట్టిక_Unmap) {
        ఒకవేళ (!isFinalCapability(స్థానం)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_పట్టిక(స్థానం->cap);
        స్థానం->cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(స్థానం->cap, 0);

        సమితి_Thread_State(ksCurThread, ThreadState_Restart);
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ || నిడివి < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (invLabel != ARMIOPage_పట్టిక_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_చిరునామా   = getSyscallArg(0, బఫరు) & ~ముసుగు(SMMU_IOPD_సూచీ_జరుపు);

    ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ప్రతిజ్ఞా(asid != asidInvalid);

    paddr = pptr_గమ్యం_paddr((శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_చిరునామా);

    ఒకవేళ (isIOPDEValid(lu_ret.iopd_స్థానం)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTASID(cap, asid);
    cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTMapped_చిరునామా(cap, io_చిరునామా);

    తిరిగి performARMIOPTInvocationMap(cap, స్థానం, lu_ret.iopd_స్థానం, iopde);
}

స్థిర exception_t performARMIOMapInvocation(cap_t cap, cte_t *స్థానం, iopte_t *iopt_స్థానం,
                                             iopte_t iopte)
{
    *iopt_స్థానం = iopte;
    cleanCacheRange_RAM((మాట_t)iopt_స్థానం,
                        ((మాట_t)iopt_స్థానం) + పరిమాణం(iopte_t),
                        addrFromPPtr(iopt_స్థానం));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    స్థానం->cap = cap;

    సమితి_Thread_State(ksCurThread, ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    మాట_t       invLabel,
    uint32_t     నిడివి,
    cte_t       *స్థానం,
    cap_t        cap,
    మాట_t      *బఫరు
)
{
    cap_t      io_space;
    paddr_t    io_చిరునామా;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     చట్రం_cap_rights;
    seL4_CapRights_t    dma_cap_rights_ముసుగు;
    lookupIOPTSlot_ret_t lu_ret;

    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ || నిడివి < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (generic_చట్రం_cap_get_cap_FSize(cap) != ARMSmall_పుట) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_small_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_చిరునామా  = getSyscallArg(1, బఫరు) & ~ముసుగు(పుట_బిట్లు);
    paddr       = pptr_గమ్యం_paddr((శూన్య *)cap_small_చట్రం_cap_get_cap_FBase_Ptr(cap));

    ఒకవేళ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ప్రతిజ్ఞా(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_చిరునామా);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (!isIOPTEEmpty(lu_ret.iopt_స్థానం)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    చట్రం_cap_rights = cap_small_చట్రం_cap_get_cap_FVMRights(cap);
    dma_cap_rights_ముసుగు = rights_From_మాట(getSyscallArg(0, బఫరు));

    ఒకవేళ ((చట్రం_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_ముసుగు)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } లేకపోతే ఒకవేళ (చట్రం_cap_rights == VMReadWrite) {
        ఒకవేళ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ముసుగు) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_ముసుగు)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } లేకపోతే ఒకవేళ (!seL4_CapRights_get_capAllowRead(dma_cap_rights_ముసుగు) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ముసుగు)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } లేకపోతే ఒకవేళ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ముసుగు) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ముసుగు)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } లేకపోతే {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

    } లేకపోతే {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_చట్రం_cap_సమితి_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asid);
    cap = cap_small_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, io_చిరునామా);

    తిరిగి performARMIOMapInvocation(cap, స్థానం, lu_ret.iopt_స్థానం, iopte);
}


శూన్య delete_IOPage_పట్టిక(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    మాట_t io_చిరునామా;
    ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_పుట_పట్టిక_cap_సమితి_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_పుట_పట్టిక_cap_get_cap_IOPTASID(io_pt_cap);
        ప్రతిజ్ఞా(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_చిరునామా = cap_io_పుట_పట్టిక_cap_get_cap_IOPTMapped_చిరునామా(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_చిరునామా);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి;
        }

        ఒకవేళ (!isIOPDEValid(lu_ret.iopd_స్థానం) ||
            iopde_ptr_get_పుట_కొలత(lu_ret.iopd_స్థానం) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_చిరునామా(lu_ret.iopd_స్థానం) != (pptr_గమ్యం_paddr((శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            తిరిగి;
        }

        *lu_ret.iopd_స్థానం = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((మాట_t)lu_ret.iopd_స్థానం,
                            ((మాట_t)lu_ret.iopd_స్థానం) + పరిమాణం(iopde_t),
                            addrFromPPtr(lu_ret.iopd_స్థానం));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

శూన్య unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    మాట_t  io_చిరునామా;
    uint32_t asid;

    io_చిరునామా = cap_small_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
    asid = cap_small_చట్రం_cap_get_cap_FMapped_ASID(cap);
    ప్రతిజ్ఞా(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_చిరునామా);

    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }
    ఒకవేళ (iopte_ptr_get_చిరునామా(lu_ret.iopt_స్థానం) != pptr_గమ్యం_paddr((శూన్య *)cap_small_చట్రం_cap_get_cap_FBase_Ptr(cap))) {
        తిరిగి;
    }

    *lu_ret.iopt_స్థానం = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((మాట_t)lu_ret.iopt_స్థానం,
                        ((మాట_t)lu_ret.iopt_స్థానం) + పరిమాణం(iopte_t),
                        addrFromPPtr(lu_ret.iopt_స్థానం));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    తిరిగి;
}

శూన్య clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    మాట_t   కొలత = బిట్టు((SMMU_PD_సూచీ_బిట్లు));
    ప్రతిజ్ఞా(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((శూన్య *)pd, 0, కొలత);
    cleanCacheRange_RAM((మాట_t)pd, (మాట_t)pd + కొలత, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    తిరిగి;
}

exception_t perform_పుట_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *స్థానం
)
{
    unmapIOPage(స్థానం->cap);
    స్థానం->cap = cap_small_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(స్థానం->cap, 0);
    స్థానం->cap = cap_small_చట్రం_cap_సమితి_cap_FIs_IOSpace(స్థానం->cap, 0);
    స్థానం->cap = cap_small_చట్రం_cap_సమితి_cap_FMapped_ASID(స్థానం->cap, asidInvalid);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(మాట_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    తిరిగి EXCEPTION_SYSCALL_ERROR;
}
#అయితే_ముగింపు /* end of CONFIG_TK1_SMMU */
