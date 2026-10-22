/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/ept.h>

#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ seL4_X86_4K:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_X86_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_X64_HugePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_pml4_cap:
        ಯದಿ (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_pdpt_cap:
        ಯದಿ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಸ್ವಭಾವ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {

    ಪಕ್ಷ cap_pml4_cap:
        ಯದಿ (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_pdpt_cap:
        ಯದಿ (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_frame_cap_get_capFMappedASID(cap)) {
            ವಿಕಲ್ಪ (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            ಪಕ್ಷ X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ಭಙ್ಗ;
#endif
#ifdef CONFIG_IOMMU
            ಪಕ್ಷ X86_MappingIOSpace:
                unmapIOPage(cap);
                ಭಙ್ಗ;
#endif
            ಪಕ್ಷ X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                fail("Invalid map type");
            }
        }
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {

    ಪಕ್ಷ cap_pdpt_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_pdpt_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ಪ್ರತ್ಯಾಗಮ false;

    ಪಕ್ಷ cap_pml4_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_pml4_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        ಪ್ರತ್ಯಾಗಮ false;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_X64_PML4Object:
        ಪ್ರತ್ಯಾಗಮ seL4_PML4Bits;

    ಪಕ್ಷ seL4_X64_HugePageObject:
        ಪ್ರತ್ಯಾಗಮ pageBitsForSize(X64_HugePage);

    ಸ್ವಭಾವ:
        fail("Invalid object type");
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

cap_t Mode_createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ವಿಕಲ್ಪ (t) {

    ಪಕ್ಷ seL4_X86_4K:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ಪಕ್ಷ seL4_X86_LargePageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ಪಕ್ಷ seL4_X64_HugePageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ಪಕ್ಷ seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ಪಕ್ಷ seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    ಪಕ್ಷ seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    ಪಕ್ಷ seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    ಪಕ್ಷ seL4_X86_IOPageTableObject:
        ಪ್ರತ್ಯಾಗಮ cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    ಸ್ವಭಾವ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Mode_decodeInvocation(
    word_t label,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_pml4_cap:
    ಪಕ್ಷ cap_pdpt_cap:
    ಪಕ್ಷ cap_page_directory_cap:
    ಪಕ್ಷ cap_page_table_cap:
    ಪಕ್ಷ cap_frame_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    ಸ್ವಭಾವ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}
