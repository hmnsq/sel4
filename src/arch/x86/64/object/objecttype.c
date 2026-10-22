/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/ept.h>

#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>


bool_t Arch_is_ಚೌಕಟ್ಟು_Type(ಶಬ್ದ_t type)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ seL4_X86_4K:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_X86_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_X64_Huge_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

deriveCap_ret_t ಬಹುಲಕ_derive_Cap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_pml4_cap:
        ಆದರೆ (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_pdpt_cap:
        ಆದರೆ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t ಬಹುಲಕ_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_pml4_cap:
        ಆದರೆ (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap))
            );
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_pdpt_cap:
        ಆದರೆ (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
            ಆಯ್ಕೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
            ಸ್ಥಿತಿ X86_MappingEPT:
                unmapEPTPage(
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                    (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
                );
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
            ಸ್ಥಿತಿ X86_MappingIOSpace:
                unmapIOPage(cap);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
            ಸ್ಥಿತಿ X86_MappingVSpace:
                unmap_ಪುಟ(
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                    (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
                );
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                fail("Invalid map type");
            }
        }
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST ಬಹುಲಕ_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    ಆಯ್ಕೆ (cap_get_capType(cap_a)) {

    ಸ್ಥಿತಿ cap_pdpt_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_pdpt_cap) {
            ಹಿಂತಿರುಗಿ cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ಹಿಂತಿರುಗಿ false;

    ಸ್ಥಿತಿ cap_pml4_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_pml4_cap) {
            ಹಿಂತಿರುಗಿ cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap_b);
        }
        ಹಿಂತಿರುಗಿ false;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

ಶಬ್ದ_t ಬಹುಲಕ_get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ seL4_X64_PML4Object:
        ಹಿಂತಿರುಗಿ se_L4_PML4_ಬಿಟ್ಟುಗಳು;

    ಸ್ಥಿತಿ se_L4_X64_Huge_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(X64_Huge_ಪುಟ);

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid object type");
        ಹಿಂತಿರುಗಿ 0;
    }
}

cap_t ಬಹುಲಕ_create_Object(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
    ಆಯ್ಕೆ (t) {

    ಸ್ಥಿತಿ seL4_X86_4K:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ಶಬ್ದ_t)region_ತಳ, /* capFBasePtr              */
                   X86_Small_ಪುಟ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ಸ್ಮರಣೆ        /* capFIsDevice             */
               );

    ಸ್ಥಿತಿ se_L4_X86_Large_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ಶಬ್ದ_t)region_ತಳ, /* capFBasePtr              */
                   X86_Large_ಪುಟ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ಸ್ಮರಣೆ        /* capFIsDevice             */
               );

    ಸ್ಥಿತಿ se_L4_X64_Huge_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ಶಬ್ದ_t)region_ತಳ, /* capFBasePtr              */
                   X64_Huge_ಪುಟ,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ಸ್ಮರಣೆ        /* capFIsDevice             */
               );

    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (ಶಬ್ದ_t)region_ತಳ,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    ಸ್ಥಿತಿ seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        ಹಿಂತಿರುಗಿ cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    ಸ್ಥಿತಿ seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (ಶಬ್ದ_t)region_ತಳ,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ se_L4_X86_IOPage_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (ಶಬ್ದ_t)region_ತಳ
               );
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t ಬಹುಲಕ_decode_Invocation(
    ಶಬ್ದ_t label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_pml4_cap:
    ಸ್ಥಿತಿ cap_pdpt_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decodeX86MMUInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);

    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}
