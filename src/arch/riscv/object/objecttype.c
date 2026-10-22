/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <kernel/vspace.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    चयन (cap_get_capType(cap)) {

    हाल cap_page_table_cap:
        अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल cap_asid_control_cap:
    हाल cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;

    शेष:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_तथ्य(bool_t preserve, word_t तथ्य, cap_t cap)
{
    वापस cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    अगर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        वापस cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        वापस cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_frame_cap:

        अगर (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        अवरोध;
    हाल cap_page_table_cap:
        अगर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
            अगर (find_ret.status == EXCEPTION_NONE && find_ret.vspace_मूल == pte) {
                deleteASID(asid, pte);
            } अन्यथा {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        अवरोध;
    हाल cap_asid_pool_cap:
        अगर (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        अवरोध;
    हाल cap_asid_control_cap:
        अवरोध;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    चयन (cap_get_capType(cap_a)) {
    हाल cap_frame_cap:
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_b))) ;
            वापस ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_page_table_cap) {
            वापस cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        अवरोध;
    हाल cap_asid_control_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_asid_pool_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            वापस cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        अवरोध;
    }

    वापस false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    अगर ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        वापस ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    वापस Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    चयन (t) {
    हाल seL4_RISCV_4K_Page:
    हाल seL4_RISCV_PageTableObject:
        वापस seL4_PageBits;
    हाल seL4_RISCV_Mega_Page:
        वापस seL4_LargePageBits;
#अगर CONFIG_PT_LEVELS > 2
    हाल seL4_RISCV_Giga_Page:
        वापस seL4_HugePageBits;
#पूर्ण_अगर
#अगर CONFIG_PT_LEVELS > 3
    हाल seL4_RISCV_Tera_Page:
        वापस seL4_TeraPageBits;
#पूर्ण_अगर
    शेष:
        fail("Invalid object type");
        वापस 0;
    }
}

cap_t Arch_createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t
                        deviceMemory)
{
    चयन (t) {
    हाल seL4_RISCV_4K_Page:
        अगर (deviceMemory) {
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
        वापस cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    हाल seL4_RISCV_Mega_Page: {
        अगर (deviceMemory) {
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
        वापस cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#अगर CONFIG_PT_LEVELS > 2
    हाल seL4_RISCV_Giga_Page: {
        अगर (deviceMemory) {
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
        वापस cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#पूर्ण_अगर

    हाल seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        वापस cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    शेष:
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
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    वापस decodeRISCVMMUInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
}

व्योम Arch_prepareThreadDelete(tcb_t *thread)
{
#अगर_घोषित CONFIG_HAVE_FPU
    fpuRelease(thread);
#पूर्ण_अगर
}

bool_t Arch_isFrameType(word_t type)
{
    चयन (type) {
#अगर CONFIG_PT_LEVELS == 4
    हाल seL4_RISCV_Tera_Page:
#पूर्ण_अगर
#अगर CONFIG_PT_LEVELS > 2
    हाल seL4_RISCV_Giga_Page:
#पूर्ण_अगर
    हाल seL4_RISCV_Mega_Page:
    हाल seL4_RISCV_4K_Page:
        वापस true;
    शेष:
        वापस false;
    }
}
