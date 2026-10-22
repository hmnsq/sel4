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
    निवड (type) {
    स्थिती seL4_X86_4K:
        परत true;
    स्थिती seL4_X86_LargePageObject:
        परत true;
    पूर्वनिर्धारित:
        परत false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        परत ret;

    पूर्वनिर्धारित:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    निवड (cap_get_capType(cap)) {

    स्थिती cap_frame_cap:
        जर (cap_frame_cap_get_capFMappedASID(cap)) {
            निवड (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            स्थिती X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                थांबा;
#endif
            स्थिती X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                जर (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    जर (pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        साठी (पूर्णांक idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_अंक_CPUs));
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
                थांबा;
#ifdef CONFIG_IOMMU
            स्थिती X86_MappingIOSpace:
                unmapIOPage(cap);
                थांबा;
#endif
            पूर्वनिर्धारित:
                fail("No mapping type for mapped cap");
                थांबा;
            }
        }
        थांबा;

    पूर्वनिर्धारित:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    परत fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    परत false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    परत 0;
}

cap_t Mode_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    निवड (t) {
    स्थिती seL4_X86_4K:
        परत cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_खालचा(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_वरचा(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    स्थिती seL4_X86_LargePageObject:
        परत cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_खालचा(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_वरचा(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    स्थिती seL4_X86_PageTableObject:
        परत cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    स्थिती seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        परत cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    स्थिती seL4_X86_IOPageTableObject:
        परत cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    पूर्वनिर्धारित:
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
    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_directory_cap:
    स्थिती cap_page_table_cap:
    स्थिती cap_frame_cap:
        परत decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    पूर्वनिर्धारित:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        परत EXCEPTION_SYSCALL_ERROR;
    }
}
