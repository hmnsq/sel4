/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, 0);
        ret.cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_asid_control_cap:
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಪೂರ್ವನಿಯೋಜಿತ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_ದತ್ತಾಂಶ(bool_t preserve, ಶಬ್ದ_t ದತ್ತಾಂಶ, cap_t cap)
{
    ಹಿಂತಿರುಗಿ cap;
}

cap_t CONST Arch_ಮುಖವಾಡ_Cap_Rights(seL4_CapRights_t cap_rights_ಮುಖವಾಡ, cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) == cap_ಚೌಕಟ್ಟು_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ಶಬ್ದ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap));
        vm_rights = ಮುಖವಾಡ_VMRights(vm_rights, cap_rights_ಮುಖವಾಡ);
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FVMRights(cap, ಶಬ್ದ_From_VMRights(vm_rights));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:

        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
            unmap_ಪುಟ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
            ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
            ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು == pte) {
                deleteASID(asid, pte);
            } ಇಲ್ಲದಿದ್ದರೆ {
                unmap_ಪುಟ_ಕೋಷ್ಟಕ(asid, cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap), pte);
            }
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಆದರೆ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_asid_control_cap:
        ನಿಲ್ಲಿಸು;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ಆಯ್ಕೆ (cap_get_capType(cap_a)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {
            ಶಬ್ದ_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b);
            ತುದಿ_A = botA + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a)));
            ತುದಿ_B = botB + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b))) ;
            ಹಿಂತಿರುಗಿ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
            ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_asid_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ಹಿಂತಿರುಗಿ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಆದರೆ ((cap_get_capType(cap_a) == cap_ಚೌಕಟ್ಟು_cap) &&
        (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap)) {
        ಹಿಂತಿರುಗಿ ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a) ==
                 cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b)) &&
                ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    ಹಿಂತಿರುಗಿ Arch_sameRegionAs(cap_a, cap_b);
}

ಶಬ್ದ_t Arch_get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ se_L4_RISCV_4_K_ಪುಟ:
    ಸ್ಥಿತಿ se_L4_RISCV_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_RISCV_Mega_ಪುಟ:
        ಹಿಂತಿರುಗಿ se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ CONFIG_PT_ಮಟ್ಟಗಳು > 2
    ಸ್ಥಿತಿ se_L4_RISCV_Giga_ಪುಟ:
        ಹಿಂತಿರುಗಿ se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ CONFIG_PT_ಮಟ್ಟಗಳು > 3
    ಸ್ಥಿತಿ se_L4_RISCV_Tera_ಪುಟ:
        ಹಿಂತಿರುಗಿ se_L4_Tera_ಪುಟ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid object type");
        ಹಿಂತಿರುಗಿ 0;
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t
                        device_ಸ್ಮರಣೆ)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ se_L4_RISCV_4_K_ಪುಟ:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ಶಬ್ದ_t) region_ತಳ,            /* capFBasePtr          */
                   RISCV_4_K_ಪುಟ,                  /* capFSize             */
                   ಶಬ್ದ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ಸ್ಮರಣೆ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    ಸ್ಥಿತಿ se_L4_RISCV_Mega_ಪುಟ: {
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ಶಬ್ದ_t) region_ತಳ,            /* capFBasePtr          */
                   RISCV_Mega_ಪುಟ,                  /* capFSize             */
                   ಶಬ್ದ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ಸ್ಮರಣೆ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ಆದರೆ CONFIG_PT_ಮಟ್ಟಗಳು > 2
    ಸ್ಥಿತಿ se_L4_RISCV_Giga_ಪುಟ: {
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (ಶಬ್ದ_t) region_ತಳ,            /* capFBasePtr          */
                   RISCV_Giga_ಪುಟ,                  /* capFSize             */
                   ಶಬ್ದ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_ಸ್ಮರಣೆ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ se_L4_RISCV_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ಪೂರ್ವನಿಯೋಜಿತ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    ಶಬ್ದ_t label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಹಿಂತಿರುಗಿ decodeRISCVMMUInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
}

ಶೂನ್ಯ Arch_prepareThreadDelete(tcb_t *thread)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    fpuRelease(thread);
#ಆದರೆ_ಅಂತ್ಯ
}

bool_t Arch_is_ಚೌಕಟ್ಟು_Type(ಶಬ್ದ_t type)
{
    ಆಯ್ಕೆ (type) {
#ಆದರೆ CONFIG_PT_ಮಟ್ಟಗಳು == 4
    ಸ್ಥಿತಿ se_L4_RISCV_Tera_ಪುಟ:
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ CONFIG_PT_ಮಟ್ಟಗಳು > 2
    ಸ್ಥಿತಿ se_L4_RISCV_Giga_ಪುಟ:
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ se_L4_RISCV_Mega_ಪುಟ:
    ಸ್ಥಿತಿ se_L4_RISCV_4_K_ಪುಟ:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}
