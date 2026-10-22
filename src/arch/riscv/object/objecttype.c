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

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_asid_control_cap:
    ಪಕ್ಷ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಸ್ವಭಾವ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_frame_cap:

        ಯದಿ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            ಯದಿ (find_ret.status == EXCEPTION_NONE && find_ret.vspace_ಬೇರು == pte) {
                deleteASID(asid, pte);
            } ಅನ್ಯಥಾ {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_asid_control_cap:
        ಭಙ್ಗ;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {
    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ತುದಿ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ತುದಿ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ಪ್ರತ್ಯಾಗಮ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_asid_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಯದಿ ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        ಪ್ರತ್ಯಾಗಮ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    ಪ್ರತ್ಯಾಗಮ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_RISCV_4K_Page:
    ಪಕ್ಷ seL4_RISCV_PageTableObject:
        ಪ್ರತ್ಯಾಗಮ seL4_PageBits;
    ಪಕ್ಷ seL4_RISCV_Mega_Page:
        ಪ್ರತ್ಯಾಗಮ seL4_LargePageBits;
#ಯದಿ CONFIG_PT_LEVELS > 2
    ಪಕ್ಷ seL4_RISCV_Giga_Page:
        ಪ್ರತ್ಯಾಗಮ seL4_HugePageBits;
#endif
#ಯದಿ CONFIG_PT_LEVELS > 3
    ಪಕ್ಷ seL4_RISCV_Tera_Page:
        ಪ್ರತ್ಯಾಗಮ seL4_TeraPageBits;
#endif
    ಸ್ವಭಾವ:
        fail("Invalid object type");
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_RISCV_4K_Page:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    ಪಕ್ಷ seL4_RISCV_Mega_Page: {
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ಯದಿ CONFIG_PT_LEVELS > 2
    ಪಕ್ಷ seL4_RISCV_Giga_Page: {
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    ಪಕ್ಷ seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ಸ್ವಭಾವ:
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
    ಪ್ರತ್ಯಾಗಮ decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

ಶೂನ್ಯ Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    ವಿಕಲ್ಪ (type) {
#ಯದಿ CONFIG_PT_LEVELS == 4
    ಪಕ್ಷ seL4_RISCV_Tera_Page:
#endif
#ಯದಿ CONFIG_PT_LEVELS > 2
    ಪಕ್ಷ seL4_RISCV_Giga_Page:
#endif
    ಪಕ್ಷ seL4_RISCV_Mega_Page:
    ಪಕ್ಷ seL4_RISCV_4K_Page:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}
