/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <kernel/vspace.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/machine/fpu.h>
#ضمن <arch/object/objecttype.h>
#ضمن <arch/object/ioport.h>
#ضمن <arch/kernel/ept.h>

#ضمن <arch/object/iospace.h>
#ضمن <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة seL4_X86_LargePageObject:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_frame_cap:
        𐤀𐤌 (cap_frame_cap_get_capFMappedASID(cap)) {
            بدل (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (فراغ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                اكسر;
#endif
            حالة X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                𐤀𐤌 (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    𐤀𐤌 (pptr_to_paddr((فراغ *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        لكل (صحيح idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
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
                    (فراغ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                اكسر;
#ifdef CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#endif
            افتراضي:
                fail("No mapping type for mapped cap");
                اكسر;
            }
        }
        اكسر;

    افتراضي:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    أرجع false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    أرجع 0;
}

cap_t Mode_createObject(object_t t, فراغ *regionBase, word_t userSize, bool_t deviceMemory)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_LOW(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_HIGH(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    حالة seL4_X86_LargePageObject:
        أرجع cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_LOW(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_HIGH(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    حالة seL4_X86_PageTableObject:
        أرجع cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    حالة seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        أرجع cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    حالة seL4_X86_IOPageTableObject:
        أرجع cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    افتراضي:
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
    بدل (cap_get_capType(cap)) {
    حالة cap_page_directory_cap:
    حالة cap_page_table_cap:
    حالة cap_frame_cap:
        أرجع decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
