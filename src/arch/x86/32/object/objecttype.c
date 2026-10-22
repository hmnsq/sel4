/*
 * Copyright 2014, General Dynamics C4 Systems
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
#અન્તર્ભાવ <arch/machine/fpu.h>
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
    મૂળભૂત:
        પાછા false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    મૂળભૂત:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    પસંદગી (cap_get_capType(cap)) {

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
            સ્થિતિ X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                જો (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    જો (pptr_to_paddr((શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        માટે (પૂર્ણાંક idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_અંક_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)
                );
                તોડો;
#ifdef CONFIG_IOMMU
            સ્થિતિ X86_MappingIOSpace:
                unmapIOPage(cap);
                તોડો;
#endif
            મૂળભૂત:
                fail("No mapping type for mapped cap");
                તોડો;
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
    પાછા false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    પાછા 0;
}

cap_t Mode_createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
    પસંદગી (t) {
    સ્થિતિ seL4_X86_4K:
        પાછા cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_નીચલું(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ઉપલું(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    સ્થિતિ seL4_X86_LargePageObject:
        પાછા cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_નીચલું(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ઉપલું(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    સ્થિતિ seL4_X86_PageTableObject:
        પાછા cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    સ્થિતિ seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        પાછા cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    સ્થિતિ seL4_X86_IOPageTableObject:
        પાછા cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    મૂળભૂત:
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
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_directory_cap:
    સ્થિતિ cap_page_table_cap:
    સ્થિતિ cap_frame_cap:
        પાછા decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    મૂળભૂત:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}
