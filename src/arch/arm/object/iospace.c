/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU

#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/smmu.h>


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupIOPDSlot_ret {
    exception_t ಸ್ಥಿತಿ_2;
    iopde_t     *iopd_ಸ್ಥಾನ;
} lookupIOPDSlot_ret_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupIOPTSlot_ret {
    exception_t ಸ್ಥಿತಿ_2;
    iopte_t     *iopt_ಸ್ಥಾನ;
} lookupIOPTSlot_ret_t;


#ವ್ಯಾಖ್ಯಾನಿಸು IOPDE_VALID_ಮುಖವಾಡ    0xe0000000
#ವ್ಯಾಖ್ಯಾನಿಸು IOPTE_ಖಾಲಿಗಳು_ಮುಖವಾಡ    0xe0000000

ಸ್ಥಿರ bool_t isIOPDEValid(iopde_t *iopde)
{
    ಪ್ರತಿಜ್ಞಾ(iopde != 0);
    ಹಿಂತಿರುಗಿ (iopde->ಪದಗಳು[0] & IOPDE_VALID_ಮುಖವಾಡ) != 0;
}

ಸ್ಥಿರ bool_t isIOPTEEmpty(iopte_t *iopte)
{
    ಪ್ರತಿಜ್ಞಾ(iopte != 0);
    ಹಿಂತಿರುಗಿ (iopte->ಪದಗಳು[0] & IOPTE_ಖಾಲಿಗಳು_ಮುಖವಾಡ) == 0;
}


ಸ್ಥಿರ lookupIOPDSlot_ret_t lookupIOPDSlot(iopde_t *iopd, ಶಬ್ದ_t io_ವಿಳಾಸ)
{
    lookupIOPDSlot_ret_t ret;
    uint32_t ಸೂಚ್ಯಂಕ = plat_smmu_iopd_ಸೂಚ್ಯಂಕ(io_ವಿಳಾಸ);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.iopd_ಸ್ಥಾನ = iopd + ಸೂಚ್ಯಂಕ;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ lookupIOPTSlot_ret_t lookupIOPTSlot(iopde_t *iopd, ಶಬ್ದ_t io_ವಿಳಾಸ)
{
    lookupIOPTSlot_ret_t pt_ret;
    uint32_t ಸೂಚ್ಯಂಕ;
    iopte_t *pt;

    lookupIOPDSlot_ret_t pd_ret = lookupIOPDSlot(iopd, io_ವಿಳಾಸ);
    ಆದರೆ (pd_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        pt_ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ಸ್ಥಾನ = 0;
        ಹಿಂತಿರುಗಿ pt_ret;
    }

    ಆದರೆ (!isIOPDEValid(pd_ret.iopd_ಸ್ಥಾನ) ||
        iopde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ret.iopd_ಸ್ಥಾನ) != iopde_iopde_pt) {
        pt_ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ಸ್ಥಾನ = 0;
        ಹಿಂತಿರುಗಿ pt_ret;
    }

    ಸೂಚ್ಯಂಕ = plat_smmu_iopt_ಸೂಚ್ಯಂಕ(io_ವಿಳಾಸ);
    pt = (iopte_t *)paddr_ಗಮ್ಯ_pptr(iopde_iopde_pt_ptr_get_ವಿಳಾಸ(pd_ret.iopd_ಸ್ಥಾನ));

    ಆದರೆ (pt == 0) {
        pt_ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        pt_ret.iopt_ಸ್ಥಾನ = 0;
        ಹಿಂತಿರುಗಿ pt_ret;
    }

    pt_ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    pt_ret.iopt_ಸ್ಥಾನ = pt + ಸೂಚ್ಯಂಕ;
    ಹಿಂತಿರುಗಿ pt_ret;
}

BOOT_ಸಂಕೇತ se_L4_ಸ್ಥಾನ_Region create_iospace_caps(cap_t ಬೇರು_cnode_cap)
{
    se_L4_ಸ್ಥಾನ_Pos ಆರಂಭ = ndks_boot.ಸ್ಥಾನ_pos_cur;
    se_L4_ಸ್ಥಾನ_Pos ಕೊನೆ = 0;
    cap_t        io_space_cap;
    ಪೂರ್ಣಾಂಕ i = 0;
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_smmu = plat_smmu_init();

    ಆದರೆ (ಅಂಕೆ_smmu == 0) {
        ಬರೆ("SMMU init failure\n");
        ಹಿಂತಿರುಗಿ S_REG_ಖಾಲಿಗಳು;
    }

    /* the 0 is reserved as an invalidASID,
     * assuming each module is assigned an unique ASID
     * and the ASIDs are contiguous
     * */
    ಗಾಗಿ (i = 1; i <= ಅಂಕೆ_smmu; i++) {
        io_space_cap = cap_io_space_cap_new(i, i);
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, io_space_cap)) {
            ಹಿಂತಿರುಗಿ S_REG_ಖಾಲಿಗಳು;
        }
    }
    ಕೊನೆ = ndks_boot.ಸ್ಥಾನ_pos_cur;
    ಬರೆ("Region [%x to %x) for SMMU caps\n", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಆರಂಭ, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಕೊನೆ);
    ಹಿಂತಿರುಗಿ (se_L4_ಸ್ಥಾನ_Region) {
        ಆರಂಭ, ಕೊನೆ
    };
}

ಸ್ಥಿರ exception_t performARMIOPTInvocationMap(cap_t cap, cte_t *ಸ್ಥಾನ, iopde_t *iopd_ಸ್ಥಾನ,
                                               iopde_t iopde)
{


    *iopd_ಸ್ಥಾನ = iopde;
    cleanCacheRange_RAM((ಶಬ್ದ_t)iopd_ಸ್ಥಾನ,
                        ((ಶಬ್ದ_t)iopd_ಸ್ಥಾನ) + ಗಾತ್ರ(iopde_t),
                        addrFromPPtr(iopd_ಸ್ಥಾನ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    ಸ್ಥಾನ->cap = cap;
    ಗಣ_Thread_State(ksCurThread, ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


exception_t decodeARMIOPTInvocation(
    ಶಬ್ದ_t       invLabel,
    uint32_t     ಉದ್ದಳತೆ,
    cte_t       *ಸ್ಥಾನ,
    cap_t        cap,
    ಶಬ್ದ_t      *ಬಫರ್
)
{
    cap_t      io_space;
    ಶಬ್ದ_t     io_ವಿಳಾಸ;
    ಶಬ್ದ_t     paddr;
    uint16_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopde_t    iopde;
    lookupIOPDSlot_ret_t    lu_ret;

    ಆದರೆ (invLabel == ARMIOPage_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (!isFinalCapability(ಸ್ಥಾನ)) {
            userError("IOPTUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        delete_IOPage_ಕೋಷ್ಟಕ(ಸ್ಥಾನ->cap);
        ಸ್ಥಾನ->cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(ಸ್ಥಾನ->cap, 0);

        ಗಣ_Thread_State(ksCurThread, ThreadState_Restart);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ || ಉದ್ದಳತೆ < 1) {
        userError("IOPTInvocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (invLabel != ARMIOPage_ಕೋಷ್ಟಕ_Map) {
        userError("IOPTInvocation: Invalid operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    io_space     = current_extra_caps.excaprefs[0]->cap;
    io_ವಿಳಾಸ   = getSyscallArg(0, ಬಫರ್) & ~ಮುಖವಾಡ(SMMU_IOPD_ಸೂಚ್ಯಂಕ_ಸರಿಸು);

    ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
        userError("IOPTMap: Cap already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOPTMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ಪ್ರತಿಜ್ಞಾ(asid != asidInvalid);

    paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap));

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPDSlot(pd, io_ವಿಳಾಸ);

    ಆದರೆ (isIOPDEValid(lu_ret.iopd_ಸ್ಥಾನ)) {
        userError("IOPTMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    iopde = iopde_iopde_pt_new(
                1,      /* read         */
                1,      /* write        */
                1,      /* nonsecure    */
                paddr
            );

    cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(cap, 1);
    cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTASID(cap, asid);
    cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTMapped_ವಿಳಾಸ(cap, io_ವಿಳಾಸ);

    ಹಿಂತಿರುಗಿ performARMIOPTInvocationMap(cap, ಸ್ಥಾನ, lu_ret.iopd_ಸ್ಥಾನ, iopde);
}

ಸ್ಥಿರ exception_t performARMIOMapInvocation(cap_t cap, cte_t *ಸ್ಥಾನ, iopte_t *iopt_ಸ್ಥಾನ,
                                             iopte_t iopte)
{
    *iopt_ಸ್ಥಾನ = iopte;
    cleanCacheRange_RAM((ಶಬ್ದ_t)iopt_ಸ್ಥಾನ,
                        ((ಶಬ್ದ_t)iopt_ಸ್ಥಾನ) + ಗಾತ್ರ(iopte_t),
                        addrFromPPtr(iopt_ಸ್ಥಾನ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();

    ಸ್ಥಾನ->cap = cap;

    ಗಣ_Thread_State(ksCurThread, ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMIOMapInvocation(
    ಶಬ್ದ_t       invLabel,
    uint32_t     ಉದ್ದಳತೆ,
    cte_t       *ಸ್ಥಾನ,
    cap_t        cap,
    ಶಬ್ದ_t      *ಬಫರ್
)
{
    cap_t      io_space;
    paddr_t    io_ವಿಳಾಸ;
    paddr_t    paddr;
    uint32_t   module_id;
    uint32_t   asid;
    iopde_t    *pd;
    iopte_t    iopte;
    vm_rights_t     ಚೌಕಟ್ಟು_cap_rights;
    seL4_CapRights_t    dma_cap_rights_ಮುಖವಾಡ;
    lookupIOPTSlot_ret_t lu_ret;

    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ || ಉದ್ದಳತೆ < 2) {
        userError("IOMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap) != ARMSmall_ಪುಟ) {
        userError("IOMap: Invalid cap type.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        userError("IOMap: Frame all ready mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    io_space    = current_extra_caps.excaprefs[0]->cap;
    io_ವಿಳಾಸ  = getSyscallArg(1, ಬಫರ್) & ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    paddr       = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

    ಆದರೆ (cap_get_capType(io_space) != cap_io_space_cap) {
        userError("IOMap: Invalid IOSpace cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    module_id = cap_io_space_cap_get_capModuleID(io_space);
    asid = plat_smmu_get_asid_by_module_id(module_id);
    ಪ್ರತಿಜ್ಞಾ(asid != asidInvalid);

    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_ವಿಳಾಸ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (!isIOPTEEmpty(lu_ret.iopt_ಸ್ಥಾನ)) {
        userError("IOMap: Delete first.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಚೌಕಟ್ಟು_cap_rights = cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);
    dma_cap_rights_ಮುಖವಾಡ = rights_From_ಶಬ್ದ(getSyscallArg(0, ಬಫರ್));

    ಆದರೆ ((ಚೌಕಟ್ಟು_cap_rights == VMReadOnly) && seL4_CapRights_get_capAllowRead(dma_cap_rights_ಮುಖವಾಡ)) {
        /* read only */
        iopte = iopte_new(
                    1,      /* read         */
                    0,      /* write        */
                    1,      /* nonsecure    */
                    paddr
                );
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಚೌಕಟ್ಟು_cap_rights == VMReadWrite) {
        ಆದರೆ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ಮುಖವಾಡ) &&
            !seL4_CapRights_get_capAllowWrite(dma_cap_rights_ಮುಖವಾಡ)) {
            /* read only */
            iopte = iopte_new(
                        1,      /* read         */
                        0,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!seL4_CapRights_get_capAllowRead(dma_cap_rights_ಮುಖವಾಡ) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ಮುಖವಾಡ)) {
            /* write only */
            iopte = iopte_new(
                        0,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (seL4_CapRights_get_capAllowRead(dma_cap_rights_ಮುಖವಾಡ) &&
                   seL4_CapRights_get_capAllowWrite(dma_cap_rights_ಮುಖವಾಡ)) {
            /* read write */
            iopte = iopte_new(
                        1,      /* read         */
                        1,      /* write        */
                        1,      /* nonsecure    */
                        paddr
                    );
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("IOMap: Invalid argument.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

    } ಇಲ್ಲದಿದ್ದರೆ {
        /* VMKernelOnly */
        userError("IOMap: Invalid argument.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FIs_IOSpace(cap, 1);
    cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asid);
    cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, io_ವಿಳಾಸ);

    ಹಿಂತಿರುಗಿ performARMIOMapInvocation(cap, ಸ್ಥಾನ, lu_ret.iopt_ಸ್ಥಾನ, iopte);
}


ಶೂನ್ಯ delete_IOPage_ಕೋಷ್ಟಕ(cap_t io_pt_cap)
{

    uint32_t asid;
    iopde_t *pd;
    lookupIOPDSlot_ret_t lu_ret;
    ಶಬ್ದ_t io_ವಿಳಾಸ;
    ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(io_pt_cap)) {
        io_pt_cap = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_IOPTIs_Mapped(io_pt_cap, 0);
        asid = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTASID(io_pt_cap);
        ಪ್ರತಿಜ್ಞಾ(asid != asidInvalid);
        pd = plat_smmu_lookup_iopd_by_asid(asid);
        io_ವಿಳಾಸ = cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTMapped_ವಿಳಾಸ(io_pt_cap);

        lu_ret = lookupIOPDSlot(pd, io_ವಿಳಾಸ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ (!isIOPDEValid(lu_ret.iopd_ಸ್ಥಾನ) ||
            iopde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.iopd_ಸ್ಥಾನ) != iopde_iopde_pt ||
            iopde_iopde_pt_ptr_get_ವಿಳಾಸ(lu_ret.iopd_ಸ್ಥಾನ) != (pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(
                                                                                  io_pt_cap)))) {
            ಹಿಂತಿರುಗಿ;
        }

        *lu_ret.iopd_ಸ್ಥಾನ = iopde_iopde_pt_new(0, 0, 0, 0);
        cleanCacheRange_RAM((ಶಬ್ದ_t)lu_ret.iopd_ಸ್ಥಾನ,
                            ((ಶಬ್ದ_t)lu_ret.iopd_ಸ್ಥಾನ) + ಗಾತ್ರ(iopde_t),
                            addrFromPPtr(lu_ret.iopd_ಸ್ಥಾನ));


        /* nice to have: flush by address and asid */
        plat_smmu_tlb_flush_all();
        plat_smmu_ptc_flush_all();
    }
}

ಶೂನ್ಯ unmapIOPage(cap_t cap)
{
    lookupIOPTSlot_ret_t lu_ret;
    iopde_t *pd;
    ಶಬ್ದ_t  io_ವಿಳಾಸ;
    uint32_t asid;

    io_ವಿಳಾಸ = cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
    asid = cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);
    ಪ್ರತಿಜ್ಞಾ(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    lu_ret = lookupIOPTSlot(pd, io_ವಿಳಾಸ);

    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (iopte_ptr_get_ವಿಳಾಸ(lu_ret.iopt_ಸ್ಥಾನ) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap))) {
        ಹಿಂತಿರುಗಿ;
    }

    *lu_ret.iopt_ಸ್ಥಾನ = iopte_new(0, 0, 0, 0);
    cleanCacheRange_RAM((ಶಬ್ದ_t)lu_ret.iopt_ಸ್ಥಾನ,
                        ((ಶಬ್ದ_t)lu_ret.iopt_ಸ್ಥಾನ) + ಗಾತ್ರ(iopte_t),
                        addrFromPPtr(lu_ret.iopt_ಸ್ಥಾನ));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    ಹಿಂತಿರುಗಿ;
}

ಶೂನ್ಯ clearIOPageDirectory(cap_t cap)
{
    iopde_t  *pd;
    uint32_t asid = cap_io_space_cap_get_capModuleID(cap);
    ಶಬ್ದ_t   ಗಾತ್ರ_2 = ಬಿಟ್((SMMU_PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
    ಪ್ರತಿಜ್ಞಾ(asid != asidInvalid);
    pd = plat_smmu_lookup_iopd_by_asid(asid);

    memset((ಶೂನ್ಯ *)pd, 0, ಗಾತ್ರ_2);
    cleanCacheRange_RAM((ಶಬ್ದ_t)pd, (ಶಬ್ದ_t)pd + ಗಾತ್ರ_2, addrFromPPtr(pd));

    plat_smmu_tlb_flush_all();
    plat_smmu_ptc_flush_all();
    ಹಿಂತಿರುಗಿ;
}

exception_t perform_ಪುಟ_Invocation_Unmap_IO(
    cap_t        cap,
    cte_t       *ಸ್ಥಾನ
)
{
    unmapIOPage(ಸ್ಥಾನ->cap);
    ಸ್ಥಾನ->cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(ಸ್ಥಾನ->cap, 0);
    ಸ್ಥಾನ->cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FIs_IOSpace(ಸ್ಥಾನ->cap, 0);
    ಸ್ಥಾನ->cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(ಸ್ಥಾನ->cap, asidInvalid);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMIOSpaceInvocation(ಶಬ್ದ_t invLabel, cap_t cap)
{
    userError("IOSpace capability has no invocations");
    current_syscall_error.type = seL4_IllegalOperation;
    ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
}
#ಆದರೆ_ಅಂತ್ಯ /* end of CONFIG_TK1_SMMU */
