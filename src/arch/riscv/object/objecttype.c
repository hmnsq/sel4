/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    પસંદગી (cap_get_capType(cap)) {

    સ્થિતિ cap_page_table_cap:
        જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ cap_asid_control_cap:
    સ્થિતિ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    મૂળભૂત:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    પાછા cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    જો (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        પાછા cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } નહીંતર {
        પાછા cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_frame_cap:

        જો (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        તોડો;
    સ્થિતિ cap_page_table_cap:
        જો (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            જો (find_ret.status == EXCEPTION_NONE && find_ret.vspace_મૂળ == pte) {
                deleteASID(asid, pte);
            } નહીંતર {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        તોડો;
    સ્થિતિ cap_asid_pool_cap:
        જો (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        તોડો;
    સ્થિતિ cap_asid_control_cap:
        તોડો;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    પાછા fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    પસંદગી (cap_get_capType(cap_a)) {
    સ્થિતિ cap_frame_cap:
        જો (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ટોચ_A, ટોચ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ટોચ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ટોચ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            પાછા ((botA <= botB) && (ટોચ_A >= ટોચ_B) && (botB <= ટોચ_B));
        }
        તોડો;

    સ્થિતિ cap_page_table_cap:
        જો (cap_get_capType(cap_b) == cap_page_table_cap) {
            પાછા cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        તોડો;
    સ્થિતિ cap_asid_control_cap:
        જો (cap_get_capType(cap_b) == cap_asid_control_cap) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_asid_pool_cap:
        જો (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            પાછા cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        તોડો;
    }

    પાછા false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    જો ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        પાછા ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    પાછા Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    પસંદગી (t) {
    સ્થિતિ seL4_RISCV_4K_Page:
    સ્થિતિ seL4_RISCV_PageTableObject:
        પાછા seL4_PageBits;
    સ્થિતિ seL4_RISCV_Mega_Page:
        પાછા seL4_LargePageBits;
#જો CONFIG_PT_LEVELS > 2
    સ્થિતિ seL4_RISCV_Giga_Page:
        પાછા seL4_HugePageBits;
#endif
#જો CONFIG_PT_LEVELS > 3
    સ્થિતિ seL4_RISCV_Tera_Page:
        પાછા seL4_TeraPageBits;
#endif
    મૂળભૂત:
        fail("Invalid object type");
        પાછા 0;
    }
}

cap_t Arch_createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    પસંદગી (t) {
    સ્થિતિ seL4_RISCV_4K_Page:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    સ્થિતિ seL4_RISCV_Mega_Page: {
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#જો CONFIG_PT_LEVELS > 2
    સ્થિતિ seL4_RISCV_Giga_Page: {
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    સ્થિતિ seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        પાછા cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    મૂળભૂત:
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
    પાછા decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

શૂન્ય Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    પસંદગી (type) {
#જો CONFIG_PT_LEVELS == 4
    સ્થિતિ seL4_RISCV_Tera_Page:
#endif
#જો CONFIG_PT_LEVELS > 2
    સ્થિતિ seL4_RISCV_Giga_Page:
#endif
    સ્થિતિ seL4_RISCV_Mega_Page:
    સ્થિતિ seL4_RISCV_4K_Page:
        પાછા true;
    મૂળભૂત:
        પાછા false;
    }
}
