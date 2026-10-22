/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_asid_control_cap:
    ସ୍ଥିତି cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ଫେରନ୍ତୁ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ଯଦି (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ଫେରନ୍ତୁ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_frame_cap:

        ଯଦି (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            ଯଦି (find_ret.status == EXCEPTION_NONE && find_ret.vspace_ମୂଳ == pte) {
                deleteASID(asid, pte);
            } ଅନ୍ୟଥା {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_asid_control_cap:
        ଭାଙ୍ଗ;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {
    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ଶୀର୍ଷ_A, ଶୀର୍ଷ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ଶୀର୍ଷ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ଶୀର୍ଷ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ଫେରନ୍ତୁ ((botA <= botB) && (ଶୀର୍ଷ_A >= ଶୀର୍ଷ_B) && (botB <= ଶୀର୍ଷ_B));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_table_cap) {
            ଫେରନ୍ତୁ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_asid_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ଫେରନ୍ତୁ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ଯଦି ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        ଫେରନ୍ତୁ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    ଫେରନ୍ତୁ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_RISCV_4K_Page:
    ସ୍ଥିତି seL4_RISCV_PageTableObject:
        ଫେରନ୍ତୁ seL4_PageBits;
    ସ୍ଥିତି seL4_RISCV_Mega_Page:
        ଫେରନ୍ତୁ seL4_LargePageBits;
#ଯଦି CONFIG_PT_LEVELS > 2
    ସ୍ଥିତି seL4_RISCV_Giga_Page:
        ଫେରନ୍ତୁ seL4_HugePageBits;
#endif
#ଯଦି CONFIG_PT_LEVELS > 3
    ସ୍ଥିତି seL4_RISCV_Tera_Page:
        ଫେରନ୍ତୁ seL4_TeraPageBits;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid object type");
        ଫେରନ୍ତୁ 0;
    }
}

cap_t Arch_createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_RISCV_4K_Page:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    ସ୍ଥିତି seL4_RISCV_Mega_Page: {
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ଯଦି CONFIG_PT_LEVELS > 2
    ସ୍ଥିତି seL4_RISCV_Giga_Page: {
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    ସ୍ଥିତି seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ଫେରନ୍ତୁ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    ଫେରନ୍ତୁ decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

ଶୂନ୍ୟ Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    ବାଛ (type) {
#ଯଦି CONFIG_PT_LEVELS == 4
    ସ୍ଥିତି seL4_RISCV_Tera_Page:
#endif
#ଯଦି CONFIG_PT_LEVELS > 2
    ସ୍ଥିତି seL4_RISCV_Giga_Page:
#endif
    ସ୍ଥିତି seL4_RISCV_Mega_Page:
    ସ୍ଥିତି seL4_RISCV_4K_Page:
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}
