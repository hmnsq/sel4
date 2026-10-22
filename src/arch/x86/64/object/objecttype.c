/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/ioport.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/ept.h>

#ଅନ୍ତର୍ଭାଵ <arch/object/iospace.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    ବାଛ (type) {
    ସ୍ଥିତି seL4_X86_4K:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_X86_LargePageObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_X64_HugePageObject:
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_pml4_cap:
        ଯଦି (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_pdpt_cap:
        ଯଦି (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ବାଛ (cap_get_capType(cap)) {

    ସ୍ଥିତି cap_pml4_cap:
        ଯଦି (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_pdpt_cap:
        ଯଦି (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_frame_cap_get_capFMappedASID(cap)) {
            ବାଛ (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            ସ୍ଥିତି X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ଭାଙ୍ଗ;
#endif
#ifdef CONFIG_IOMMU
            ସ୍ଥିତି X86_MappingIOSpace:
                unmapIOPage(cap);
                ଭାଙ୍ଗ;
#endif
            ସ୍ଥିତି X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                fail("Invalid map type");
            }
        }
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {

    ସ୍ଥିତି cap_pdpt_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_pdpt_cap) {
            ଫେରନ୍ତୁ cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ଫେରନ୍ତୁ false;

    ସ୍ଥିତି cap_pml4_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_pml4_cap) {
            ଫେରନ୍ତୁ cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        ଫେରନ୍ତୁ false;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_X64_PML4Object:
        ଫେରନ୍ତୁ seL4_PML4Bits;

    ସ୍ଥିତି seL4_X64_HugePageObject:
        ଫେରନ୍ତୁ pageBitsForSize(X64_HugePage);

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid object type");
        ଫେରନ୍ତୁ 0;
    }
}

cap_t Mode_createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ବାଛ (t) {

    ସ୍ଥିତି seL4_X86_4K:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ସ୍ଥିତି seL4_X86_LargePageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ସ୍ଥିତି seL4_X64_HugePageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    ସ୍ଥିତି seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ଫେରନ୍ତୁ cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ସ୍ଥିତି seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        ଫେରନ୍ତୁ cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    ସ୍ଥିତି seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        ଫେରନ୍ତୁ cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    ସ୍ଥିତି seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        ଫେରନ୍ତୁ cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    ସ୍ଥିତି seL4_X86_IOPageTableObject:
        ଫେରନ୍ତୁ cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_pml4_cap:
    ସ୍ଥିତି cap_pdpt_cap:
    ସ୍ଥିତି cap_page_directory_cap:
    ସ୍ଥିତି cap_page_table_cap:
    ସ୍ଥିତି cap_frame_cap:
        ଫେରନ୍ତୁ decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}
