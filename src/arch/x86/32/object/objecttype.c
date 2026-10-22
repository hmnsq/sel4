/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/machine/fpu.h>
#அந்தர்பாவ <arch/object/objecttype.h>
#அந்தர்பாவ <arch/object/ioport.h>
#அந்தர்பாவ <arch/kernel/ept.h>

#அந்தர்பாவ <arch/object/iospace.h>
#அந்தர்பாவ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    தேர்வு (type) {
    நிலை seL4_X86_4K:
        திரும்பு true;
    நிலை seL4_X86_LargePageObject:
        திரும்பு true;
    இயல்பு:
        திரும்பு false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

    இயல்பு:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    தேர்வு (cap_get_capType(cap)) {

    நிலை cap_frame_cap:
        எனில் (cap_frame_cap_get_capFMappedASID(cap)) {
            தேர்வு (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            நிலை X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (வெறுமை *)cap_frame_cap_get_capFBasePtr(cap)
                );
                நிறுத்து;
#endif
            நிலை X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                எனில் (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    எனில் (pptr_to_paddr((வெறுமை *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        க்கு (முழு idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_இலக்கம்_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (வெறுமை *)cap_frame_cap_get_capFBasePtr(cap)
                );
                நிறுத்து;
#ifdef CONFIG_IOMMU
            நிலை X86_MappingIOSpace:
                unmapIOPage(cap);
                நிறுத்து;
#endif
            இயல்பு:
                fail("No mapping type for mapped cap");
                நிறுத்து;
            }
        }
        நிறுத்து;

    இயல்பு:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    திரும்பு fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    திரும்பு false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    திரும்பு 0;
}

cap_t Mode_createObject(object_t t, வெறுமை *regionBase, word_t userSize, bool_t deviceMemory)
{
    தேர்வு (t) {
    நிலை seL4_X86_4K:
        திரும்பு cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_கீழ்(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_உயர்வு(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    நிலை seL4_X86_LargePageObject:
        திரும்பு cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_கீழ்(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_உயர்வு(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    நிலை seL4_X86_PageTableObject:
        திரும்பு cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    நிலை seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        திரும்பு cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    நிலை seL4_X86_IOPageTableObject:
        திரும்பு cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    இயல்பு:
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
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_directory_cap:
    நிலை cap_page_table_cap:
    நிலை cap_frame_cap:
        திரும்பு decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    இயல்பு:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}
