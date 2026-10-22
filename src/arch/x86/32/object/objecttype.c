/*
 * Copyright 2014, General Dynamics C4 Systems
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
#include <arch/machine/fpu.h>
#include <arch/object/objecttype.h>
#include <arch/object/ioport.h>
#include <arch/kernel/ept.h>

#include <arch/object/iospace.h>
#include <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
    pak_sa seL4_X86_4K:
        ཕྱིར true;
    pak_sa seL4_X86_LargePageObject:
        ཕྱིར true;
    default:
        ཕྱིར false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;

    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {

    pak_sa cap_frame_cap:
        གལ་ཏེ (cap_frame_cap_get_capFMappedASID(cap)) {
            switch (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            pak_sa X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (void *)cap_frame_cap_get_capFBasePtr(cap)
                );
                bha_nga;
#endif
            pak_sa X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                གལ་ཏེ (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    གལ་ཏེ (pptr_to_paddr((void *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        ཆེད་དུ (ཧྲིལ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ksNumCPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (void *)cap_frame_cap_get_capFBasePtr(cap)
                );
                bha_nga;
#ifdef CONFIG_IOMMU
            pak_sa X86_MappingIOSpace:
                unmapIOPage(cap);
                bha_nga;
#endif
            default:
                fail("No mapping type for mapped cap");
                bha_nga;
            }
        }
        bha_nga;

    default:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ཕྱིར fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ཕྱིར false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    ཕྱིར 0;
}

cap_t Mode_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {
    pak_sa seL4_X86_4K:
        ཕྱིར cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_LOW(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_HIGH(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    pak_sa seL4_X86_LargePageObject:
        ཕྱིར cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_LOW(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_HIGH(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    pak_sa seL4_X86_PageTableObject:
        ཕྱིར cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    pak_sa seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        ཕྱིར cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    pak_sa seL4_X86_IOPageTableObject:
        ཕྱིར cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    default:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t Mode_decodeInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_page_directory_cap:
    pak_sa cap_page_table_cap:
    pak_sa cap_frame_cap:
        ཕྱིར decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    default:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        ཕྱིར EXCEPTION_SYSCALL_ERROR;
    }
}
