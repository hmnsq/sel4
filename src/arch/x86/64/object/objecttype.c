/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <machine/fpu.h>
#include <arch/object/objecttype.h>
#include <arch/object/ioport.h>
#include <arch/kernel/ept.h>

#include <arch/object/iospace.h>
#include <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
    case seL4_X86_4K:
        trả_về true;
    case seL4_X86_LargePageObject:
        trả_về true;
    case seL4_X64_HugePageObject:
        trả_về true;
    default:
        trả_về false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    case cap_pml4_cap:
        nếu (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } không_thì {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        trả_về ret;

    case cap_pdpt_cap:
        nếu (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } không_thì {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        trả_về ret;

    case cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        trả_về ret;

    default:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {

    case cap_pml4_cap:
        nếu (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(cap))
            );
        }
        break;

    case cap_pdpt_cap:
        nếu (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        break;

    case cap_frame_cap:
        nếu (cap_frame_cap_get_capFMappedASID(cap)) {
            switch (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            case X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (void *)cap_frame_cap_get_capFBasePtr(cap)
                );
                break;
#endif
#ifdef CONFIG_IOMMU
            case X86_MappingIOSpace:
                unmapIOPage(cap);
                break;
#endif
            case X86_MappingVSpace:
                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (void *)cap_frame_cap_get_capFBasePtr(cap)
                );
                break;
            default:
                fail("Invalid map type");
            }
        }
        break;

    default:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    trả_về fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {

    case cap_pdpt_cap:
        nếu (cap_get_capType(cap_b) == cap_pdpt_cap) {
            trả_về cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        trả_về false;

    case cap_pml4_cap:
        nếu (cap_get_capType(cap_b) == cap_pml4_cap) {
            trả_về cap_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        trả_về false;
    default:
        trả_về false;
    }
}

word_t Mode_getObjectSize(word_t t)
{
    switch (t) {
    case seL4_X64_PML4Object:
        trả_về seL4_PML4Bits;

    case seL4_X64_HugePageObject:
        trả_về pageBitsForSize(X64_HugePage);

    default:
        fail("Invalid object type");
        trả_về 0;
    }
}

cap_t Mode_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {

    case seL4_X86_4K:
        nếu (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } không_thì {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        trả_về cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_SmallPage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    case seL4_X86_LargePageObject:
        nếu (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } không_thì {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        trả_về cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X86_LargePage,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    case seL4_X64_HugePageObject:
        nếu (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } không_thì {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        trả_về cap_frame_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (word_t)regionBase, /* capFBasePtr              */
                   X64_HugePage,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   deviceMemory        /* capFIsDevice             */
               );

    case seL4_X86_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        trả_về cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    case seL4_X86_PageDirectoryObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        trả_về cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    case seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        trả_về cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (word_t)regionBase,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    case seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(regionBase));
        trả_về cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (word_t)regionBase,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ifdef CONFIG_IOMMU
    case seL4_X86_IOPageTableObject:
        trả_về cap_io_page_table_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (word_t)regionBase
               );
#endif

    default:
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
    switch (cap_get_capType(cap)) {
    case cap_pml4_cap:
    case cap_pdpt_cap:
    case cap_page_directory_cap:
    case cap_page_table_cap:
    case cap_frame_cap:
        trả_về decodeX86MMUInvocation(label, length, cptr, slot, cap, call, buffer);

    default:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        trả_về EXCEPTION_SYSCALL_ERROR;
    }
}
