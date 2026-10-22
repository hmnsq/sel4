/*
 * Copyright 2014, General Dynamics C4 Systems
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
#ଅନ୍ତର୍ଭାଵ <arch/machine/fpu.h>
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
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ବାଛ (cap_get_capType(cap)) {

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
            ସ୍ଥିତି X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                ଯଦି (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    ଯଦି (pptr_to_paddr((ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_ଅଙ୍କ_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ଭାଙ୍ଗ;
#ifdef CONFIG_IOMMU
            ସ୍ଥିତି X86_MappingIOSpace:
                unmapIOPage(cap);
                ଭାଙ୍ଗ;
#endif
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                fail("No mapping type for mapped cap");
                ଭାଙ୍ଗ;
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
    ଫେରନ୍ତୁ false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    ଫେରନ୍ତୁ 0;
}

cap_t Mode_createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_X86_4K:
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_ତଳ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ଉପର(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ସ୍ଥିତି seL4_X86_LargePageObject:
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_ତଳ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ଉପର(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ସ୍ଥିତି seL4_X86_PageTableObject:
        ଫେରନ୍ତୁ cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    ସ୍ଥିତି seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        ଫେରନ୍ତୁ cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    ସ୍ଥିତି seL4_X86_IOPageTableObject:
        ଫେରନ୍ତୁ cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_directory_cap:
    ସ୍ଥିତି cap_page_table_cap:
    ସ୍ଥିତି cap_frame_cap:
        ଫେରନ୍ତୁ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}
