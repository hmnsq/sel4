/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <types.h>
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {

    pak_sa cap_page_table_cap:
        ᬬᬾᬦ᭄ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ᬮᬶᬬ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ᬯᬮᬶ ret;

    pak_sa cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ᬯᬮᬶ ret;

    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᬯᬮᬶ ret;

    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ᬯᬮᬶ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ᬬᬾᬦ᭄ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ᬯᬮᬶ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ᬮᬶᬬ {
        ᬯᬮᬶ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_frame_cap:

        ᬬᬾᬦ᭄ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        bha_nga;
    pak_sa cap_page_table_cap:
        ᬬᬾᬦ᭄ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
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
            ᬬᬾᬦ᭄ (find_ret.status == EXCEPTION_NONE && find_ret.vspace_root == pte) {
                deleteASID(asid, pte);
            } ᬮᬶᬬ {
                unmapPageTable(asid, cap_page_table_cap_get_capPTMappedAddress(cap), pte);
            }
        }
        bha_nga;
    pak_sa cap_asid_pool_cap:
        ᬬᬾᬦ᭄ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        bha_nga;
    pak_sa cap_asid_control_cap:
        bha_nga;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ᬯᬮᬶ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    pak_sa cap_frame_cap:
        ᬬᬾᬦ᭄ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ᬯᬮᬶ ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        ᬬᬾᬦ᭄ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ᬯᬮᬶ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;
    pak_sa cap_asid_control_cap:
        ᬬᬾᬦ᭄ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ᬯᬮᬶ true;
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        ᬬᬾᬦ᭄ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ᬯᬮᬶ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        bha_nga;
    }

    ᬯᬮᬶ false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ᬬᬾᬦ᭄ ((cap_get_capType(cap_a) == cap_frame_cap) &&
        (cap_get_capType(cap_b) == cap_frame_cap)) {
        ᬯᬮᬶ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)) &&
                ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                 (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
    }
    ᬯᬮᬶ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    pak_sa seL4_RISCV_4K_Page:
    pak_sa seL4_RISCV_PageTableObject:
        ᬯᬮᬶ seL4_PageBits;
    pak_sa seL4_RISCV_Mega_Page:
        ᬯᬮᬶ seL4_LargePageBits;
#ᬬᬾᬦ᭄ CONFIG_PT_LEVELS > 2
    pak_sa seL4_RISCV_Giga_Page:
        ᬯᬮᬶ seL4_HugePageBits;
#endif
#ᬬᬾᬦ᭄ CONFIG_PT_LEVELS > 3
    pak_sa seL4_RISCV_Tera_Page:
        ᬯᬮᬶ seL4_TeraPageBits;
#endif
    default:
        fail("Invalid object type");
        ᬯᬮᬶ 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t
                        deviceMemory)
{
    switch (t) {
    pak_sa seL4_RISCV_4K_Page:
        ᬬᬾᬦ᭄ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ᬮᬶᬬ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        ᬯᬮᬶ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    pak_sa seL4_RISCV_Mega_Page: {
        ᬬᬾᬦ᭄ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ᬮᬶᬬ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        ᬯᬮᬶ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Mega_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ᬬᬾᬦ᭄ CONFIG_PT_LEVELS > 2
    pak_sa seL4_RISCV_Giga_Page: {
        ᬬᬾᬦ᭄ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ᬮᬶᬬ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        ᬯᬮᬶ cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_Giga_Page,                  /* capFSize             */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   deviceMemory,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#endif

    pak_sa seL4_RISCV_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ᬯᬮᬶ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    default:
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
    ᬯᬮᬶ decodeRISCVMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}

void Arch_prepareThreadDelete(tcb_t *thread)
{
#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
#ᬬᬾᬦ᭄ CONFIG_PT_LEVELS == 4
    pak_sa seL4_RISCV_Tera_Page:
#endif
#ᬬᬾᬦ᭄ CONFIG_PT_LEVELS > 2
    pak_sa seL4_RISCV_Giga_Page:
#endif
    pak_sa seL4_RISCV_Mega_Page:
    pak_sa seL4_RISCV_4K_Page:
        ᬯᬮᬶ true;
    default:
        ᬯᬮᬶ false;
    }
}
