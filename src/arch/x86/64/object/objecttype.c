/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/objecttype.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <arch/kernel/ept.h>

#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_X86_4K:
        प्रत्यागम true;
    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम true;
    पक्ष seL4_X64_HugePageObject:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_pml4_cap:
        यदि (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_pdpt_cap:
        यदि (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_pml4_cap:
        यदि (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_pdpt_cap:
        यदि (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
            विकल्प (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            पक्ष X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
#endif
#ifdef CONFIG_IOMMU
            पक्ष X86_MappingIOSpace:
                unmapIOPage(cap);
                भङ्ग;
#endif
            पक्ष X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
            स्वभाव:
                fail("Invalid map type");
            }
        }
        भङ्ग;

    स्वभाव:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {

    पक्ष cap_pdpt_cap:
        यदि (cap_get_capType(cap_b) == cap_pdpt_cap) {
            प्रत्यागम cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        प्रत्यागम false;

    पक्ष cap_pml4_cap:
        यदि (cap_get_capType(cap_b) == cap_pml4_cap) {
            प्रत्यागम cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        प्रत्यागम false;
    स्वभाव:
        प्रत्यागम false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_X64_PML4Object:
        प्रत्यागम seL4_PML4Bits;

    पक्ष seL4_X64_HugePageObject:
        प्रत्यागम pageBitsForSize(X64_HugePage);

    स्वभाव:
        fail("Invalid object type");
        प्रत्यागम 0;
    }
}

cap_t Mode_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    विकल्प (t) {

    पक्ष seL4_X86_4K:
        यदि (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    पक्ष seL4_X86_LargePageObject:
        यदि (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    पक्ष seL4_X64_HugePageObject:
        यदि (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    पक्ष seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        प्रत्यागम cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    पक्ष seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        प्रत्यागम cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    पक्ष seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        प्रत्यागम cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    पक्ष seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        प्रत्यागम cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    पक्ष seL4_X86_IOPageTableObject:
        प्रत्यागम cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    स्वभाव:
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
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_pml4_cap:
    पक्ष cap_pdpt_cap:
    पक्ष cap_page_directory_cap:
    पक्ष cap_page_table_cap:
    पक्ष cap_frame_cap:
        प्रत्यागम decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    स्वभाव:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}
