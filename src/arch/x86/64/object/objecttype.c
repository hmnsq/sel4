/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <arch/object/objecttype.h>
#অন্তর্ভাব <arch/object/ioport.h>
#অন্তর্ভাব <arch/kernel/ept.h>

#অন্তর্ভাব <arch/object/iospace.h>
#অন্তর্ভাব <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    বিকল্প (type) {
    পক্ষ seL4_X86_4K:
        প্রত্যাগম true;
    পক্ষ seL4_X86_LargePageObject:
        প্রত্যাগম true;
    পক্ষ seL4_X64_HugePageObject:
        প্রত্যাগম true;
    স্বভাব:
        প্রত্যাগম false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_pml4_cap:
        যদি (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_pdpt_cap:
        যদি (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        প্রত্যাগম ret;

    স্বভাব:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    বিকল্প (cap_get_capType(cap)) {

    পক্ষ cap_pml4_cap:
        যদি (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        ভঙ্গ;

    পক্ষ cap_pdpt_cap:
        যদি (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ভঙ্গ;

    পক্ষ cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
            বিকল্প (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            পক্ষ X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (শূন্য *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ভঙ্গ;
#endif
#ifdef CONFIG_IOMMU
            পক্ষ X86_MappingIOSpace:
                unmapIOPage(cap);
                ভঙ্গ;
#endif
            পক্ষ X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (শূন্য *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ভঙ্গ;
            স্বভাব:
                fail("Invalid map type");
            }
        }
        ভঙ্গ;

    স্বভাব:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    প্রত্যাগম fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    বিকল্প (cap_get_capType(cap_a)) {

    পক্ষ cap_pdpt_cap:
        যদি (cap_get_capType(cap_b) == cap_pdpt_cap) {
            প্রত্যাগম cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        প্রত্যাগম false;

    পক্ষ cap_pml4_cap:
        যদি (cap_get_capType(cap_b) == cap_pml4_cap) {
            প্রত্যাগম cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        প্রত্যাগম false;
    স্বভাব:
        প্রত্যাগম false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    বিকল্প (t) {
    পক্ষ seL4_X64_PML4Object:
        প্রত্যাগম seL4_PML4Bits;

    পক্ষ seL4_X64_HugePageObject:
        প্রত্যাগম pageBitsForSize(X64_HugePage);

    স্বভাব:
        fail("Invalid object type");
        প্রত্যাগম 0;
    }
}

cap_t Mode_createObject(object_t t, শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
    বিকল্প (t) {

    পক্ষ seL4_X86_4K:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        প্রত্যাগম cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    পক্ষ seL4_X86_LargePageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        প্রত্যাগম cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    পক্ষ seL4_X64_HugePageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        প্রত্যাগম cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    পক্ষ seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        প্রত্যাগম cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    পক্ষ seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        প্রত্যাগম cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    পক্ষ seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        প্রত্যাগম cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    পক্ষ seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        প্রত্যাগম cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    পক্ষ seL4_X86_IOPageTableObject:
        প্রত্যাগম cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    স্বভাব:
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
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_pml4_cap:
    পক্ষ cap_pdpt_cap:
    পক্ষ cap_page_directory_cap:
    পক্ষ cap_page_table_cap:
    পক্ষ cap_frame_cap:
        প্রত্যাগম decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    স্বভাব:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
}
