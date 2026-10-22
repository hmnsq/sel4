/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    അവസ്ഥ cap_asid_control_cap:
    അവസ്ഥ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    സ്ഥിരസ്ഥിതി:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    തിരികെ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        തിരികെ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } അല്ലെങ്കിൽ {
        തിരികെ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_frame_cap:

        എങ്കിൽ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        നിർത്തുക;
    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            എങ്കിൽ (find_ret.status == EXCEPTION_NONE && find_ret.vspace_വേര് == pte) {
                deleteASID(asid, pte);
            } അല്ലെങ്കിൽ {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        നിർത്തുക;
    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        നിർത്തുക;
    അവസ്ഥ cap_asid_control_cap:
        നിർത്തുക;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap_a)) {
    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, മുകൾ_A, മുകൾ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            മുകൾ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            മുകൾ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            തിരികെ ((botA <= botB) && (മുകൾ_A >= മുകൾ_B) && (botB <= മുകൾ_B));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_table_cap) {
            തിരികെ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        നിർത്തുക;
    അവസ്ഥ cap_asid_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            തിരികെ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        നിർത്തുക;
    }

    തിരികെ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    എങ്കിൽ ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        തിരികെ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    തിരികെ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_RISCV_4K_Page:
    അവസ്ഥ seL4_RISCV_PageTableObject:
        തിരികെ seL4_PageBits;
    അവസ്ഥ seL4_RISCV_Mega_Page:
        തിരികെ seL4_LargePageBits;
#എങ്കിൽ CONFIG_PT_LEVELS > 2
    അവസ്ഥ seL4_RISCV_Giga_Page:
        തിരികെ seL4_HugePageBits;
#endif
#എങ്കിൽ CONFIG_PT_LEVELS > 3
    അവസ്ഥ seL4_RISCV_Tera_Page:
        തിരികെ seL4_TeraPageBits;
#endif
    സ്ഥിരസ്ഥിതി:
        fail("Invalid object type");
        തിരികെ 0;
    }
}

cap_t Arch_createObject(object_t t, ശൂന്യം *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_RISCV_4K_Page:
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        തിരികെ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    അവസ്ഥ seL4_RISCV_Mega_Page: {
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        തിരികെ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#എങ്കിൽ CONFIG_PT_LEVELS > 2
    അവസ്ഥ seL4_RISCV_Giga_Page: {
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        തിരികെ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    അവസ്ഥ seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        തിരികെ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    സ്ഥിരസ്ഥിതി:
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
    തിരികെ decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

ശൂന്യം Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    തിരഞ്ഞെടുപ്പ് (type) {
#എങ്കിൽ CONFIG_PT_LEVELS == 4
    അവസ്ഥ seL4_RISCV_Tera_Page:
#endif
#എങ്കിൽ CONFIG_PT_LEVELS > 2
    അവസ്ഥ seL4_RISCV_Giga_Page:
#endif
    അവസ്ഥ seL4_RISCV_Mega_Page:
    അവസ്ഥ seL4_RISCV_4K_Page:
        തിരികെ true;
    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}
