/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/machine/fpu.h>
#شامل <arch/object/objecttype.h>
#شامل <arch/object/ioport.h>
#شامل <arch/kernel/ept.h>

#شامل <arch/object/iospace.h>
#شامل <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    انتخاب (type) {
    حالت seL4_X86_4K:
        بازگشت true;
    حالت seL4_X86_LargePageObject:
        بازگشت true;
    پیشفرض:
        بازگشت false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

    پیشفرض:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {

    حالت cap_frame_cap:
        اگر (cap_frame_cap_get_capFMappedASID(cap)) {
            انتخاب (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            حالت X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (تهی *)cap_frame_cap_get_capFBasePtr(cap)
                );
                بشکن;
#endif
            حالت X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                اگر (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    اگر (pptr_to_paddr((تهی *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        برای (صحیح idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_رقم_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (تهی *)cap_frame_cap_get_capFBasePtr(cap)
                );
                بشکن;
#ifdef CONFIG_IOMMU
            حالت X86_MappingIOSpace:
                unmapIOPage(cap);
                بشکن;
#endif
            پیشفرض:
                fail("No mapping type for mapped cap");
                بشکن;
            }
        }
        بشکن;

    پیشفرض:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    بازگشت fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بازگشت false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    بازگشت 0;
}

cap_t Mode_createObject(object_t t, تهی *regionBase, word_t userSize, bool_t deviceMemory)
{
    انتخاب (t) {
    حالت seL4_X86_4K:
        بازگشت cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_بالا(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    حالت seL4_X86_LargePageObject:
        بازگشت cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_بالا(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    حالت seL4_X86_PageTableObject:
        بازگشت cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    حالت seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        بازگشت cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    حالت seL4_X86_IOPageTableObject:
        بازگشت cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    پیشفرض:
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
    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_directory_cap:
    حالت cap_page_table_cap:
    حالت cap_frame_cap:
        بازگشت decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    پیشفرض:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}
