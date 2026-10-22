/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/machine/fpu.h>
#അന്തര്ഭാവ <arch/object/objecttype.h>
#അന്തര്ഭാവ <arch/object/ioport.h>
#അന്തര്ഭാവ <arch/kernel/ept.h>

#അന്തര്ഭാവ <arch/object/iospace.h>
#അന്തര്ഭാവ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    തിരഞ്ഞെടുപ്പ് (type) {
    അവസ്ഥ seL4_X86_4K:
        തിരികെ true;
    അവസ്ഥ seL4_X86_LargePageObject:
        തിരികെ true;
    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    സ്ഥിരസ്ഥിതി:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {

    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_frame_cap_get_capFMappedASID(cap)) {
            തിരഞ്ഞെടുപ്പ് (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            അവസ്ഥ X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap)
                );
                നിർത്തുക;
#endif
            അവസ്ഥ X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                എങ്കിൽ (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    എങ്കിൽ (pptr_to_paddr((ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_അക്കം_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap)
                );
                നിർത്തുക;
#ifdef CONFIG_IOMMU
            അവസ്ഥ X86_MappingIOSpace:
                unmapIOPage(cap);
                നിർത്തുക;
#endif
            സ്ഥിരസ്ഥിതി:
                fail("No mapping type for mapped cap");
                നിർത്തുക;
            }
        }
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരികെ false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    തിരികെ 0;
}

cap_t Mode_createObject(object_t t, ശൂന്യം *regionBase, word_t userSize, bool_t deviceMemory)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_X86_4K:
        തിരികെ cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_താഴെയുള്ള(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_മുകളിലെ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    അവസ്ഥ seL4_X86_LargePageObject:
        തിരികെ cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_താഴെയുള്ള(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_മുകളിലെ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    അവസ്ഥ seL4_X86_PageTableObject:
        തിരികെ cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    അവസ്ഥ seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        തിരികെ cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    അവസ്ഥ seL4_X86_IOPageTableObject:
        തിരികെ cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    സ്ഥിരസ്ഥിതി:
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
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_directory_cap:
    അവസ്ഥ cap_page_table_cap:
    അവസ്ഥ cap_frame_cap:
        തിരികെ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    സ്ഥിരസ്ഥിതി:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}
