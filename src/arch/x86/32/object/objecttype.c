/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <object/structures.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/model/statedata.h>
#অন্তর্ভাৱ <arch/machine/fpu.h>
#অন্তর্ভাৱ <arch/object/objecttype.h>
#অন্তর্ভাৱ <arch/object/ioport.h>
#অন্তর্ভাৱ <arch/kernel/ept.h>

#অন্তর্ভাৱ <arch/object/iospace.h>
#অন্তর্ভাৱ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    ৱিকল্প (type) {
    পক্ষ seL4_X86_4K:
        ঘূৰাই true;
    পক্ষ seL4_X86_LargePageObject:
        ঘূৰাই true;
    স্ৱভাৱ:
        ঘূৰাই false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ৱিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;

    স্ৱভাৱ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ৱিকল্প (cap_get_capType(cap)) {

    পক্ষ cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
            ৱিকল্প (cap_frame_cap_get_capFMapType(cap)) {
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
            পক্ষ X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                যদি (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    যদি (pptr_to_paddr((শূন্য *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        বাবে (পূর্ণাঙ্ক idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_অঙ্ক_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (শূন্য *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ভঙ্গ;
#ifdef CONFIG_IOMMU
            পক্ষ X86_MappingIOSpace:
                unmapIOPage(cap);
                ভঙ্গ;
#endif
            স্ৱভাৱ:
                fail("No mapping type for mapped cap");
                ভঙ্গ;
            }
        }
        ভঙ্গ;

    স্ৱভাৱ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ঘূৰাই fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ঘূৰাই false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    ঘূৰাই 0;
}

cap_t Mode_createObject(object_t t, শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
    ৱিকল্প (t) {
    পক্ষ seL4_X86_4K:
        ঘূৰাই cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_নিচেৰ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_উপৰেৰ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    পক্ষ seL4_X86_LargePageObject:
        ঘূৰাই cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_নিচেৰ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_উপৰেৰ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    পক্ষ seL4_X86_PageTableObject:
        ঘূৰাই cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    পক্ষ seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        ঘূৰাই cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    পক্ষ seL4_X86_IOPageTableObject:
        ঘূৰাই cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    স্ৱভাৱ:
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
    ৱিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_page_directory_cap:
    পক্ষ cap_page_table_cap:
    পক্ষ cap_frame_cap:
        ঘূৰাই decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    স্ৱভাৱ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
}
