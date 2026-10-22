/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <kernel/vspace.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_page_table_cap:
        إذا (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        إرجاع ret;

    حالة cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        إرجاع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        إرجاع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    إرجاع cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        إرجاع cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } وإلا {
        إرجاع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_frame_cap:

        إذا (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        اكسر;
    حالة cap_page_table_cap:
        إذا (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            إذا (find_ret.status == EXCEPTION_NONE && find_ret.vspace_جذر == pte) {
                deleteASID(asid, pte);
            } وإلا {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        اكسر;
    حالة cap_asid_pool_cap:
        إذا (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        اكسر;
    حالة cap_asid_control_cap:
        اكسر;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    إرجاع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_frame_cap:
        إذا (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, قمة_A, قمة_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            قمة_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            قمة_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            إرجاع ((botA <= botB) && (قمة_A >= قمة_B) && (botB <= قمة_B));
        }
        اكسر;

    حالة cap_page_table_cap:
        إذا (cap_get_capType(cap_b) == cap_page_table_cap) {
            إرجاع cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        اكسر;
    حالة cap_asid_control_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_control_cap) {
            إرجاع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            إرجاع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;
    }

    إرجاع false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        إرجاع ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    إرجاع Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    بدل (t) {
    حالة seL4_RISCV_4K_Page:
    حالة seL4_RISCV_PageTableObject:
        إرجاع seL4_PageBits;
    حالة seL4_RISCV_Mega_Page:
        إرجاع seL4_LargePageBits;
#إذا CONFIG_PT_LEVELS > 2
    حالة seL4_RISCV_Giga_Page:
        إرجاع seL4_HugePageBits;
#endif
#إذا CONFIG_PT_LEVELS > 3
    حالة seL4_RISCV_Tera_Page:
        إرجاع seL4_TeraPageBits;
#endif
    افتراضي:
        fail("Invalid object type");
        إرجاع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    بدل (t) {
    حالة seL4_RISCV_4K_Page:
        إذا (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        إرجاع cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    حالة seL4_RISCV_Mega_Page: {
        إذا (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        إرجاع cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#إذا CONFIG_PT_LEVELS > 2
    حالة seL4_RISCV_Giga_Page: {
        إذا (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        إرجاع cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    حالة seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        إرجاع cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    افتراضي:
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
    إرجاع decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    بدل (type) {
#إذا CONFIG_PT_LEVELS == 4
    حالة seL4_RISCV_Tera_Page:
#endif
#إذا CONFIG_PT_LEVELS > 2
    حالة seL4_RISCV_Giga_Page:
#endif
    حالة seL4_RISCV_Mega_Page:
    حالة seL4_RISCV_4K_Page:
        إرجاع true;
    افتراضي:
        إرجاع false;
    }
}
