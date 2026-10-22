/*
 * Copyright 2014, General Dynamics C4 Systems
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
#অন্তর্ভাব <arch/machine/fpu.h>
#অন্তর্ভাব <arch/object/objecttype.h>
#অন্তর্ভাব <arch/object/ioport.h>
#অন্তর্ভাব <arch/kernel/ept.h>

#অন্তর্ভাব <arch/object/iospace.h>
#অন্তর্ভাব <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    নির্বাচন (type) {
    ক্ষেত্র seL4_X86_4K:
        ফেরত true;
    ক্ষেত্র seL4_X86_LargePageObject:
        ফেরত true;
    পূর্বনির্ধারিত:
        ফেরত false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    পূর্বনির্ধারিত:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    নির্বাচন (cap_get_capType(cap)) {

    ক্ষেত্র cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
            নির্বাচন (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            ক্ষেত্র X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (শূন্য *)cap_frame_cap_get_capFBasePtr(cap)
                );
                বিরতি;
#endif
            ক্ষেত্র X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_বাফার
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                যদি (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    যদি (pptr_to_paddr((শূন্য *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_বাফার) {
                        ks_User_Log_বাফার = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        জন্য (পূর্ণসংখ্যা idx = 0; idx < BIT(PT_সূচক_BITS); idx++) {
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
                বিরতি;
#ifdef CONFIG_IOMMU
            ক্ষেত্র X86_MappingIOSpace:
                unmapIOPage(cap);
                বিরতি;
#endif
            পূর্বনির্ধারিত:
                fail("No mapping type for mapped cap");
                বিরতি;
            }
        }
        বিরতি;

    পূর্বনির্ধারিত:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ফেরত false;
}

word_t Mode_get_Object_আকার(word_t t)
{
    fail("Invalid object type");
    ফেরত 0;
}

cap_t Mode_createObject(object_t t, শূন্য *regionBase, word_t user_আকার, bool_t deviceMemory)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_X86_4K:
        ফেরত cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_নিচের(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_উপরের(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ক্ষেত্র seL4_X86_LargePageObject:
        ফেরত cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_নিচের(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_উপরের(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ক্ষেত্র seL4_X86_PageTableObject:
        ফেরত cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    ক্ষেত্র seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        ফেরত cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    ক্ষেত্র seL4_X86_IOPageTableObject:
        ফেরত cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    পূর্বনির্ধারিত:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t Mode_decodeInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_directory_cap:
    ক্ষেত্র cap_page_table_cap:
    ক্ষেত্র cap_frame_cap:
        ফেরত decodeX86MMUInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    পূর্বনির্ধারিত:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}
