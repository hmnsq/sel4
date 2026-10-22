/*
 * Copyright 2014, General Dynamics C4 Systems
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
#अन्तर्भाव <arch/machine/fpu.h>
#अन्तर्भाव <arch/object/objecttype.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <arch/kernel/ept.h>

#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_X86_4K:
        फर्काऊ true;
    पक्ष seL4_X86_LargePageObject:
        फर्काऊ true;
    स्वभाव:
        फर्काऊ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        फर्काऊ ret;

    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {

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
                रोक;
#endif
            पक्ष X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                यदि (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    यदि (pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        लागि (पूर्णाङ्क idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_अङ्क_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                रोक;
#ifdef CONFIG_IOMMU
            पक्ष X86_MappingIOSpace:
                unmapIOPage(cap);
                रोक;
#endif
            स्वभाव:
                fail("No mapping type for mapped cap");
                रोक;
            }
        }
        रोक;

    स्वभाव:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    फर्काऊ fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    फर्काऊ false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    फर्काऊ 0;
}

cap_t Mode_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    विकल्प (t) {
    पक्ष seL4_X86_4K:
        फर्काऊ cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_तल्लो(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_माथिल्लो(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    पक्ष seL4_X86_LargePageObject:
        फर्काऊ cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_तल्लो(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_माथिल्लो(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    पक्ष seL4_X86_PageTableObject:
        फर्काऊ cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    पक्ष seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        फर्काऊ cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    पक्ष seL4_X86_IOPageTableObject:
        फर्काऊ cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    स्वभाव:
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
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
    पक्ष cap_page_table_cap:
    पक्ष cap_frame_cap:
        फर्काऊ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    स्वभाव:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }
}
