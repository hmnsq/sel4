/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    নির্বাচন (cap_get_capType(cap)) {

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    ক্ষেত্র cap_asid_control_cap:
    ক্ষেত্র cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    পূর্বনির্ধারিত:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_তথ্য(bool_t preserve, word_t তথ্য, cap_t cap)
{
    ফেরত cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ফেরত cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } অন্যথা {
        ফেরত cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_frame_cap:

        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        বিরতি;
    ক্ষেত্র cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            যদি (find_ret.status == EXCEPTION_NONE && find_ret.vspace_মূল == pte) {
                deleteASID(asid, pte);
            } অন্যথা {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        বিরতি;
    ক্ষেত্র cap_asid_pool_cap:
        যদি (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        বিরতি;
    ক্ষেত্র cap_asid_control_cap:
        বিরতি;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    নির্বাচন (cap_get_capType(cap_a)) {
    ক্ষেত্র cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, শীর্ষ_A, শীর্ষ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            শীর্ষ_A = botA + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_a)));
            শীর্ষ_B = botB + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_b))) ;
            ফেরত ((botA <= botB) && (শীর্ষ_A >= শীর্ষ_B) && (botB <= শীর্ষ_B));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            ফেরত cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        বিরতি;
    ক্ষেত্র cap_asid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ফেরত cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        বিরতি;
    }

    ফেরত false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        ফেরত ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    ফেরত Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_আকার(word_t t)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_RISCV_4K_Page:
    ক্ষেত্র seL4_RISCV_PageTableObject:
        ফেরত seL4_PageBits;
    ক্ষেত্র seL4_RISCV_Mega_Page:
        ফেরত seL4_LargePageBits;
#যদি CONFIG_PT_LEVELS > 2
    ক্ষেত্র seL4_RISCV_Giga_Page:
        ফেরত seL4_HugePageBits;
#endif
#যদি CONFIG_PT_LEVELS > 3
    ক্ষেত্র seL4_RISCV_Tera_Page:
        ফেরত seL4_TeraPageBits;
#endif
    পূর্বনির্ধারিত:
        fail("Invalid object type");
        ফেরত 0;
    }
}

cap_t Arch_createObject(object_t t, শূন্য *regionBase, word_t user_আকার, bool_t
                        deviceMemory)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_RISCV_4K_Page:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    ক্ষেত্র seL4_RISCV_Mega_Page: {
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#যদি CONFIG_PT_LEVELS > 2
    ক্ষেত্র seL4_RISCV_Giga_Page: {
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    ক্ষেত্র seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ফেরত cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    পূর্বনির্ধারিত:
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
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    ফেরত decodeRISCVMMUInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
}

শূন্য Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    নির্বাচন (type) {
#যদি CONFIG_PT_LEVELS == 4
    ক্ষেত্র seL4_RISCV_Tera_Page:
#endif
#যদি CONFIG_PT_LEVELS > 2
    ক্ষেত্র seL4_RISCV_Giga_Page:
#endif
    ক্ষেত্র seL4_RISCV_Mega_Page:
    ক্ষেত্র seL4_RISCV_4K_Page:
        ফেরত true;
    পূর্বনির্ধারিত:
        ফেরত false;
    }
}
