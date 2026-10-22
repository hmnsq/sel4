/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, 0);
        ret.cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_asid_control_cap:
    ਹਾਲਤ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਮੂਲ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_ਅੰਕੜੇ(bool_t preserve, ਸ਼ਬਦ_t ਅੰਕੜੇ, cap_t cap)
{
    ਵਾਪਸ cap;
}

cap_t CONST Arch_ਨਕਾਬ_Cap_Rights(seL4_CapRights_t cap_rights_ਨਕਾਬ, cap_t cap)
{
    ਜੇ (cap_get_capType(cap) == cap_ਚੌਖਟਾ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ਸ਼ਬਦ(cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap));
        vm_rights = ਨਕਾਬ_VMRights(vm_rights, cap_rights_ਨਕਾਬ);
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FVMRights(cap, ਸ਼ਬਦ_From_VMRights(vm_rights));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:

        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
            unmap_ਪੰਨਾ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
            ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
            ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE && ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ == pte) {
                deleteASID(asid, pte);
            } ਨਹੀਂਤਾਂ {
                unmap_ਪੰਨਾ_ਸਾਰਣੀ(asid, cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap), pte);
            }
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_asid_pool_cap:
        ਜੇ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_asid_control_cap:
        ਰੋਕੋ;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ਚੋਣ (cap_get_capType(cap_a)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {
            ਸ਼ਬਦ_t botA, botB, ਸਿਖਰ_A, ਸਿਖਰ_B;
            botA = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b);
            ਸਿਖਰ_A = botA + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a)));
            ਸਿਖਰ_B = botB + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b))) ;
            ਵਾਪਸ ((botA <= botB) && (ਸਿਖਰ_A >= ਸਿਖਰ_B) && (botB <= ਸਿਖਰ_B));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
            ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_asid_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ਵਾਪਸ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ਰੋਕੋ;
    }

    ਵਾਪਸ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ਜੇ ((cap_get_capType(cap_a) == cap_ਚੌਖਟਾ_cap) &&
        (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap)) {
        ਵਾਪਸ ((cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a) ==
                 cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b)) &&
                ((cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    ਵਾਪਸ Arch_sameRegionAs(cap_a, cap_b);
}

ਸ਼ਬਦ_t Arch_get_Object_ਮਾਪ(ਸ਼ਬਦ_t t)
{
    ਚੋਣ (t) {
    ਹਾਲਤ se_L4_RISCV_4_K_ਪੰਨਾ:
    ਹਾਲਤ se_L4_RISCV_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        ਵਾਪਸ se_L4_ਪੰਨਾ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_RISCV_Mega_ਪੰਨਾ:
        ਵਾਪਸ se_L4_Large_ਪੰਨਾ_ਬਿੱਟ;
#ਜੇ CONFIG_PT_ਪੱਧਰ > 2
    ਹਾਲਤ se_L4_RISCV_Giga_ਪੰਨਾ:
        ਵਾਪਸ se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
#ਜੇ CONFIG_PT_ਪੱਧਰ > 3
    ਹਾਲਤ se_L4_RISCV_Tera_ਪੰਨਾ:
        ਵਾਪਸ se_L4_Tera_ਪੰਨਾ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        fail("Invalid object type");
        ਵਾਪਸ 0;
    }
}

cap_t Arch_createObject(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t
                        device_ਸਮ੍ਰਿਤੀ)
{
    ਚੋਣ (t) {
    ਹਾਲਤ se_L4_RISCV_4_K_ਪੰਨਾ:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ਸ਼ਬਦ_t) region_ਅਧਾਰ,            /* capFBasePtr          */
                   RISCV_4_K_ਪੰਨਾ,                  /* capFSize             */
                   ਸ਼ਬਦ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ਸਮ੍ਰਿਤੀ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    ਹਾਲਤ se_L4_RISCV_Mega_ਪੰਨਾ: {
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ਸ਼ਬਦ_t) region_ਅਧਾਰ,            /* capFBasePtr          */
                   RISCV_Mega_ਪੰਨਾ,                  /* capFSize             */
                   ਸ਼ਬਦ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ਸਮ੍ਰਿਤੀ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ਜੇ CONFIG_PT_ਪੱਧਰ > 2
    ਹਾਲਤ se_L4_RISCV_Giga_ਪੰਨਾ: {
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ਸ਼ਬਦ_t) region_ਅਧਾਰ,            /* capFBasePtr          */
                   RISCV_Giga_ਪੰਨਾ,                  /* capFSize             */
                   ਸ਼ਬਦ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ਸਮ੍ਰਿਤੀ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#ਜੇ_ਅੰਤ

    ਹਾਲਤ se_L4_RISCV_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ਮੂਲ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    ਸ਼ਬਦ_t label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਵਾਪਸ decodeRISCVMMUInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
}

ਖਾਲੀ Arch_prepareThreadDelete(tcb_t *thread)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    fpuRelease(thread);
#ਜੇ_ਅੰਤ
}

bool_t Arch_is_ਚੌਖਟਾ_Type(ਸ਼ਬਦ_t type)
{
    ਚੋਣ (type) {
#ਜੇ CONFIG_PT_ਪੱਧਰ == 4
    ਹਾਲਤ se_L4_RISCV_Tera_ਪੰਨਾ:
#ਜੇ_ਅੰਤ
#ਜੇ CONFIG_PT_ਪੱਧਰ > 2
    ਹਾਲਤ se_L4_RISCV_Giga_ਪੰਨਾ:
#ਜੇ_ਅੰਤ
    ਹਾਲਤ se_L4_RISCV_Mega_ਪੰਨਾ:
    ਹਾਲਤ se_L4_RISCV_4_K_ਪੰਨਾ:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}
