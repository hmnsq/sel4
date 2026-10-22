/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_page_सारणी_cap:
        यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_frame_cap:
        cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, 0);
        ret.cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asidInvalid);
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_दत्त(bool_t preserve, word_t दत्त, cap_t cap)
{
    प्रत्यागम cap;
}

cap_t CONST Arch_आवरणम्_Cap_Rights(seL4_CapRights_t cap_rights_आवरणम्, cap_t cap)
{
    यदि (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = आवरणम्_VMRights(vm_rights, cap_rights_आवरणम्);
        प्रत्यागम cap_frame_cap_समुच्चयः_cap_FVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        प्रत्यागम cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:

        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;
    पक्ष cap_page_सारणी_cap:
        यदि (final && cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
            यदि (find_ret.स्थितिः == EXCEPTION_NONE && find_ret.vspace_मूलम् == pte) {
                deleteASID(asid, pte);
            } अन्यथा {
                unmap_Page_सारणी(asid, cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap), pte);
            }
        }
        भङ्ग;
    पक्ष cap_asid_pool_cap:
        यदि (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        भङ्ग;
    पक्ष cap_asid_control_cap:
        भङ्ग;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_frame_cap:
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_b))) ;
            प्रत्यागम ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_page_सारणी_cap) {
            प्रत्यागम cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_b);
        }
        भङ्ग;
    पक्ष cap_asid_control_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            प्रत्यागम cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;
    }

    प्रत्यागम false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    यदि ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        प्रत्यागम ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    प्रत्यागम Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_RISCV_4K_Page:
    पक्ष se_L4_RISCV_Page_सारणी_Object:
        प्रत्यागम se_L4_Page_द्वयङ्काः;
    पक्ष seL4_RISCV_Mega_Page:
        प्रत्यागम se_L4_Large_Page_द्वयङ्काः;
#यदि CONFIG_PT_स्तराः > 2
    पक्ष seL4_RISCV_Giga_Page:
        प्रत्यागम se_L4_Huge_Page_द्वयङ्काः;
#यदि_अन्तः
#यदि CONFIG_PT_स्तराः > 3
    पक्ष seL4_RISCV_Tera_Page:
        प्रत्यागम se_L4_Tera_Page_द्वयङ्काः;
#यदि_अन्तः
    स्वभाव:
        fail("Invalid object type");
        प्रत्यागम 0;
    }
}

cap_t Arch_createObject(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t
                        device_स्मृतिः)
{
    विकल्प (t) {
    पक्ष seL4_RISCV_4K_Page:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) region_आधारः,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   device_स्मृतिः,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    पक्ष seL4_RISCV_Mega_Page: {
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) region_आधारः,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   device_स्मृतिः,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#यदि CONFIG_PT_स्तराः > 2
    पक्ष seL4_RISCV_Giga_Page: {
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) region_आधारः,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   device_स्मृतिः,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#यदि_अन्तः

    पक्ष se_L4_RISCV_Page_सारणी_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        प्रत्यागम cap_page_सारणी_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)region_आधारः,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    स्वभाव:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    word_t label,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    प्रत्यागम decodeRISCVMMUInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
}

शून्य Arch_prepareThreadDelete(tcb_t *thread)
{
#यदि_परिभाषितम् CONFIG_HAVE_FPU
    fpuRelease(thread);
#यदि_अन्तः
}

bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
#यदि CONFIG_PT_स्तराः == 4
    पक्ष seL4_RISCV_Tera_Page:
#यदि_अन्तः
#यदि CONFIG_PT_स्तराः > 2
    पक्ष seL4_RISCV_Giga_Page:
#यदि_अन्तः
    पक्ष seL4_RISCV_Mega_Page:
    पक्ष seL4_RISCV_4K_Page:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}
