/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <arch/object/objecttype.h>
#અન્તર્ભાવ <arch/object/ioport.h>
#અન્તર્ભાવ <arch/kernel/ept.h>

#અન્તર્ભાવ <arch/object/iospace.h>
#અન્તર્ભાવ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    પસંદગી (type) {
    સ્થિતિ seL4_X86_4K:
        પાછા true;
    સ્થિતિ seL4_X86_LargePageObject:
        પાછા true;
    સ્થિતિ seL4_X64_HugePageObject:
        પાછા true;
    મૂળભૂત:
        પાછા false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_pml4_cap:
        જો (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_pdpt_cap:
        જો (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    મૂળભૂત:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    પસંદગી (cap_get_capType(cap)) {

    સ્થિતિ cap_pml4_cap:
        જો (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        તોડો;

    સ્થિતિ cap_pdpt_cap:
        જો (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        તોડો;

    સ્થિતિ cap_frame_cap:
        જો (cap_frame_cap_get_capFMappedASID(cap)) {
            પસંદગી (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            સ્થિતિ X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)
                );
                તોડો;
#endif
#ifdef CONFIG_IOMMU
            સ્થિતિ X86_MappingIOSpace:
                unmapIOPage(cap);
                તોડો;
#endif
            સ્થિતિ X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)
                );
                તોડો;
            મૂળભૂત:
                fail("Invalid map type");
            }
        }
        તોડો;

    મૂળભૂત:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    પાછા fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    પસંદગી (cap_get_capType(cap_a)) {

    સ્થિતિ cap_pdpt_cap:
        જો (cap_get_capType(cap_b) == cap_pdpt_cap) {
            પાછા cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        પાછા false;

    સ્થિતિ cap_pml4_cap:
        જો (cap_get_capType(cap_b) == cap_pml4_cap) {
            પાછા cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        પાછા false;
    મૂળભૂત:
        પાછા false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    પસંદગી (t) {
    સ્થિતિ seL4_X64_PML4Object:
        પાછા seL4_PML4Bits;

    સ્થિતિ seL4_X64_HugePageObject:
        પાછા pageBitsForSize(X64_HugePage);

    મૂળભૂત:
        fail("Invalid object type");
        પાછા 0;
    }
}

cap_t Mode_createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
    પસંદગી (t) {

    સ્થિતિ seL4_X86_4K:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    સ્થિતિ seL4_X86_LargePageObject:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    સ્થિતિ seL4_X64_HugePageObject:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        પાછા cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    સ્થિતિ seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        પાછા cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    સ્થિતિ seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        પાછા cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    સ્થિતિ seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        પાછા cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    સ્થિતિ seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        પાછા cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    સ્થિતિ seL4_X86_IOPageTableObject:
        પાછા cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    મૂળભૂત:
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
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_pml4_cap:
    સ્થિતિ cap_pdpt_cap:
    સ્થિતિ cap_page_directory_cap:
    સ્થિતિ cap_page_table_cap:
    સ્થિતિ cap_frame_cap:
        પાછા decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    મૂળભૂત:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}
