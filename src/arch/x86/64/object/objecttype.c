/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/objecttype.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <arch/kernel/ept.h>

#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_X86_4K:
        प्रत्यागम true;
    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम true;
    पक्ष seL4_X64_HugePageObject:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

deriveCap_ret_t बहुलकम्_derive_Cap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_pml4_cap:
        यदि (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_pdpt_cap:
        यदि (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_frame_cap:
        cap = cap_frame_cap_समुच्चयः_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asidInvalid);
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t बहुलकम्_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_pml4_cap:
        यदि (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_आधारः_Ptr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_pdpt_cap:
        यदि (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
            विकल्प (cap_frame_cap_get_capFMapType(cap)) {
#यदि_परिभाषितम् CONFIG_VTX
            पक्ष X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_IOMMU
            पक्ष X86_MappingIOSpace:
                unmapIOPage(cap);
                भङ्ग;
#यदि_अन्तः
            पक्ष X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
            स्वभाव:
                fail("Invalid map type");
            }
        }
        भङ्ग;

    स्वभाव:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST बहुलकम्_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {

    पक्ष cap_pdpt_cap:
        यदि (cap_get_capType(cap_b) == cap_pdpt_cap) {
            प्रत्यागम cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        प्रत्यागम false;

    पक्ष cap_pml4_cap:
        यदि (cap_get_capType(cap_b) == cap_pml4_cap) {
            प्रत्यागम cap_pml4_cap_get_cap_PML4_आधारः_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_आधारः_Ptr(cap_b);
        }
        प्रत्यागम false;
    स्वभाव:
        प्रत्यागम false;
    }
}

word_t बहुलकम्_get_Object_आकार(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_X64_PML4Object:
        प्रत्यागम se_L4_PML4_द्वयङ्काः;

    पक्ष seL4_X64_HugePageObject:
        प्रत्यागम page_द्वयङ्काः_For_आकार(X64_HugePage);

    स्वभाव:
        fail("Invalid object type");
        प्रत्यागम 0;
    }
}

cap_t बहुलकम्_create_Object(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
    विकल्प (t) {

    पक्ष seL4_X86_4K:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)region_आधारः, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_स्मृतिः        /* capFIsDevice             */
               );

    पक्ष seL4_X86_LargePageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)region_आधारः, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_स्मृतिः        /* capFIsDevice             */
               );

    पक्ष seL4_X64_HugePageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)region_आधारः, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_स्मृतिः        /* capFIsDevice             */
               );

    पक्ष se_L4_X86_Page_सारणी_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        प्रत्यागम cap_page_सारणी_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)region_आधारः,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    पक्ष seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        प्रत्यागम cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)region_आधारः,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    पक्ष seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        प्रत्यागम cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)region_आधारः,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    पक्ष seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_आधारः));
        प्रत्यागम cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)region_आधारः,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष se_L4_X86_IOPage_सारणी_Object:
        प्रत्यागम cap_io_page_सारणी_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)region_आधारः
               );
#यदि_अन्तः

    स्वभाव:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t बहुलकम्_decode_Invocation(
    word_t label,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_pml4_cap:
    पक्ष cap_pdpt_cap:
    पक्ष cap_page_directory_cap:
    पक्ष cap_page_सारणी_cap:
    पक्ष cap_frame_cap:
        प्रत्यागम decodeX86MMUInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);

    स्वभाव:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}
