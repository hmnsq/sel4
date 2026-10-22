/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU

#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/smmu.h>


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupIOPDSlot_ret {
    exception_t ਸਥਿਤੀ;
    iopde_t     *iopd_ਥਾਂ;
} lookupIOPDSlot_ret_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupIOPTSlot_ret {
    exception_t ਸਥਿਤੀ;
    iopte_t     *iopt_ਥਾਂ;
} lookupIOPTSlot_ret_t;


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOPDE_VALID_ਨਕਾਬ    0xe0000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOPTE_ਖਾਲੀ_ਥਾਵਾਂ_ਨਕਾਬ    0xe0000000

ਸਥਿਰ bool_t isIOPDEValid(iopde_t *iopde)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(iopde != 0);
    ਵਾਪਸ (iopde->ਸ਼ਬਦ[0] & IOPDE_VALID_ਨਕਾਬ) != 0;
}

ਸਥਿਰ bool_t isIOPTEEmpty(iopte_t *iopte)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(iopte != 0);
    ਵਾਪਸ (iopte->ਸ਼ਬਦ[0] & IOPTE_ਖਾਲੀ_ਥਾਵਾਂ_ਨਕਾਬ) == 0;
}


ਸਥਿਰ lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, ਸ਼ਬਦ_t io_ਪਤਾ)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t ਸੂਚਕਾਂਕ = plat_smmu_iopd_ਸੂਚਕਾਂਕ(io_ਪਤਾ);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.iopd_ਥਾਂ = iopd + ਸੂਚਕਾਂਕ;
    ਵਾਪਸ ret;
}

ਸਥਿਰ lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, ਸ਼ਬਦ_t io_ਪਤਾ)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t ਸੂਚਕਾਂਕ;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_ਪਤਾ);
    ਜੇ (pd_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        pt_ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ਥਾਂ = 0;
        ਵਾਪਸ pt_ret;
    }

    ਜੇ (!isIOPDEValid(pd_ret.iopd_ਥਾਂ) ||
        iopde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ret.iopd_ਥਾਂ) != iopde_iopde_pt) {
        pt_ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ਥਾਂ = 0;
        ਵਾਪਸ pt_ret;
    }

    ਸੂਚਕਾਂਕ = plat_smmu_iopt_ਸੂਚਕਾਂਕ(io_ਪਤਾ);
    pt = (iopte_t *)paddr_ਮੰਜ਼ਿਲ_pptr(iopde_iopde_pt_ptr_get_ਪਤਾ(pd_ret.iopd_ਥਾਂ));

    ਜੇ (pt == 0) {
        pt_ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ਥਾਂ = 0;
        ਵਾਪਸ pt_ret;
    }

    pt_ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    pt_ret.iopt_ਥਾਂ = pt + ਸੂਚਕਾਂਕ;
    ਵਾਪਸ pt_ret;
}

BOOT_ਕੋਡ se_L4_ਥਾਂ_Region create_iospace_caps(cap_t ਜੜ੍ਹ_cnode_cap)
{
    se_L4_ਥਾਂ_Pos ਸ਼ੁਰੂ = ndks_boot.ਥਾਂ_pos_cur;
    se_L4_ਥਾਂ_Pos ਸਿਰਾ = 0;
    cap_t        io_space_cap;
    ਪੂਰਨ i = 0;
    ਪੂਰਨ ਅੰਕ_smmu = plat_smmu_init();

    ਜੇ (ਅੰਕ_smmu == 0) {
        ਲਿਖੋ("SMMU init failure\n");
        ਵਾਪਸ S_REG_ਖਾਲੀ_ਥਾਵਾਂ;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    ਲਈ (i = 1; i <= ਅੰਕ_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, io_space_cap)) {
            ਵਾਪਸ S_REG_ਖਾਲੀ_ਥਾਵਾਂ;
        }
    }
    ਸਿਰਾ = ndks_boot.ਥਾਂ_pos_cur;
    ਲਿਖੋ("Region [%x to %x) for SMMU caps\n", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸ਼ੁਰੂ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸਿਰਾ);
    ਵਾਪਸ (se_L4_ਥਾਂ_Region) {
        ਸ਼ੁਰੂ, ਸਿਰਾ
    };
}

ਸਥਿਰ exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *ਥਾਂ, iopde_t *iopd_ਥਾਂ,
                                               iopde_t iopde)
{


    *iopd_ਥਾਂ = iopde;
    cleanCacheRange_RAM((ਸ਼ਬਦ_t)iopd_ਥਾਂ,
                        ((ਸ਼ਬਦ_t)iopd_ਥਾਂ) + ਆਕਾਰ(iopde_t),
                        addrFromPPtr(iopd_ਥਾਂ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    ਥਾਂ->cap = cap;
    ਸੈੱਟ_Thread_State(ksCurThread, ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    ਸ਼ਬਦ_t       invLabel,
    uint32_t     ਲੰਮਾਈ,
    cte_t       *ਥਾਂ,
    cap_t        cap,
    ਸ਼ਬਦ_t      *ਬਫਰ
)
{
    cap_t      io_space;
    ਸ਼ਬਦ_t     io_ਪਤਾ;
    ਸ਼ਬਦ_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    ਜੇ (invLabel == ARMIOPage_ਸਾਰਣੀ_Unmap) {
        ਜੇ (!isFinalCapability(ਥਾਂ)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_ਸਾਰਣੀ(ਥਾਂ->cap);
        ਥਾਂ->cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(ਥਾਂ->cap, 0);

        ਸੈੱਟ_Thread_State(ksCurThread, ThreadState_Restart);
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ || ਲੰਮਾਈ < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (invLabel != ARMIOPage_ਸਾਰਣੀ_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_ਪਤਾ   = getSyscallArg(0, ਬਫਰ) & ~ਨਕਾਬ(SMMU_IOPD_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);

    ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ਪ੍ਰਤਿਜ੍ਞਾ(asid != asidInvalid);

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_ਪਤਾ);

    ਜੇ (isIOPDEValid(lu_ret.iopd_ਥਾਂ)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTASID(cap, asid);
    cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTMapped_ਪਤਾ(cap, io_ਪਤਾ);

    ਵਾਪਸ performARMIOPTInvocationMap(cap, ਥਾਂ, lu_ret.iopd_ਥਾਂ, iopde);
}

ਸਥਿਰ exception_t performARMIOMapInvocation(cap_t cap, cte_t *ਥਾਂ, iopte_t *iopt_ਥਾਂ,
                                             iopte_t iopte)
{
    *iopt_ਥਾਂ = iopte;
    cleanCacheRange_RAM((ਸ਼ਬਦ_t)iopt_ਥਾਂ,
                        ((ਸ਼ਬਦ_t)iopt_ਥਾਂ) + ਆਕਾਰ(iopte_t),
                        addrFromPPtr(iopt_ਥਾਂ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    ਥਾਂ->cap = cap;

    ਸੈੱਟ_Thread_State(ksCurThread, ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    ਸ਼ਬਦ_t       invLabel,
    uint32_t     ਲੰਮਾਈ,
    cte_t       *ਥਾਂ,
    cap_t        cap,
    ਸ਼ਬਦ_t      *ਬਫਰ
)
{
    cap_t      io_space;
    paddr_t    io_ਪਤਾ;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     ਚੌਖਟਾ_cap_rights;
    seL4_CapRights_t    dma_cap_rights_ਨਕਾਬ;
    lookupIOPTSlot_ret_t lu_ret;

    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ || ਲੰਮਾਈ < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap) != ARMSmall_ਪੰਨਾ) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_ਪਤਾ  = getSyscallArg(1, ਬਫਰ) & ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ);
    paddr       = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_small_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

    ਜੇ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ਪ੍ਰਤਿਜ੍ਞਾ(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_ਪਤਾ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (!isIOPTEEmpty(lu_ret.iopt_ਥਾਂ)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਚੌਖਟਾ_cap_rights = cap_small_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);
    dma_cap_rights_ਨਕਾਬ = rights_From_ਸ਼ਬਦ(getSyscallArg(0, ਬਫਰ));

    ਜੇ ((ਚੌਖਟਾ_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_ਨਕਾਬ)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } ਨਹੀਂਤਾਂ ਜੇ (ਚੌਖਟਾ_cap_rights == VMReadWrite) {
        ਜੇ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ਨਕਾਬ) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_ਨਕਾਬ)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ਨਹੀਂਤਾਂ ਜੇ (!seL4_CapRights_get_capAllowRead(dma_cap_rights_ਨਕਾਬ) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ਨਕਾਬ)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ਨਹੀਂਤਾਂ ਜੇ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ਨਕਾਬ) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ਨਕਾਬ)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ਨਹੀਂਤਾਂ {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

    } ਨਹੀਂਤਾਂ {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, io_ਪਤਾ);

    ਵਾਪਸ performARMIOMapInvocation(cap, ਥਾਂ, lu_ret.iopt_ਥਾਂ, iopte);
}


ਖਾਲੀ delete_IOPage_ਸਾਰਣੀ(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    ਸ਼ਬਦ_t io_ਪਤਾ;
    ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTASID(io_pt_cap);
        ਪ੍ਰਤਿਜ੍ਞਾ(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_ਪਤਾ = cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTMapped_ਪਤਾ(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_ਪਤਾ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ;
        }

        ਜੇ (!isIOPDEValid(lu_ret.iopd_ਥਾਂ) ||
            iopde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.iopd_ਥਾਂ) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_ਪਤਾ(lu_ret.iopd_ਥਾਂ) != (pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            ਵਾਪਸ;
        }

        *lu_ret.iopd_ਥਾਂ = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((ਸ਼ਬਦ_t)lu_ret.iopd_ਥਾਂ,
                            ((ਸ਼ਬਦ_t)lu_ret.iopd_ਥਾਂ) + ਆਕਾਰ(iopde_t),
                            addrFromPPtr(lu_ret.iopd_ਥਾਂ));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

ਖਾਲੀ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    ਸ਼ਬਦ_t  io_ਪਤਾ;
    uint32_t asid;

    io_ਪਤਾ = cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
    asid = cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);
    ਪ੍ਰਤਿਜ੍ਞਾ(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_ਪਤਾ);

    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }
    ਜੇ (iopte_ptr_get_ਪਤਾ(lu_ret.iopt_ਥਾਂ) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_small_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap))) {
        ਵਾਪਸ;
    }

    *lu_ret.iopt_ਥਾਂ = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((ਸ਼ਬਦ_t)lu_ret.iopt_ਥਾਂ,
                        ((ਸ਼ਬਦ_t)lu_ret.iopt_ਥਾਂ) + ਆਕਾਰ(iopte_t),
                        addrFromPPtr(lu_ret.iopt_ਥਾਂ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    ਵਾਪਸ;
}

ਖਾਲੀ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    ਸ਼ਬਦ_t   ਮਾਪ = ਦੋਆਧਾਰੀ_ਅੰਕ((SMMU_PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
    ਪ੍ਰਤਿਜ੍ਞਾ(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((ਖਾਲੀ *)pd, 0, ਮਾਪ);
    cleanCacheRange_RAM((ਸ਼ਬਦ_t)pd, (ਸ਼ਬਦ_t)pd + ਮਾਪ, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    ਵਾਪਸ;
}

exception_t perform_ਪੰਨਾ_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *ਥਾਂ
)
{
    unmapIOPage(ਥਾਂ->cap);
    ਥਾਂ->cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(ਥਾਂ->cap, 0);
    ਥਾਂ->cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FIs_IOSpace(ਥਾਂ->cap, 0);
    ਥਾਂ->cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(ਥਾਂ->cap, asidInvalid);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(ਸ਼ਬਦ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
}
#ਜੇ_ਅੰਤ /* end of CONFIG_TK1_SMMU */
