/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/ept.h>

#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ seL4_X86_4K:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_X86_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಸ್ವಭಾವ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {

    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_frame_cap_get_capFMappedASID(cap)) {
            ವಿಕಲ್ಪ (cap_frame_cap_get_capFMapType(cap)) {
#ifdef CONFIG_VTX
            ಪಕ್ಷ X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ಭಙ್ಗ;
#endif
            ಪಕ್ಷ X86_MappingVSpace:

#ifdef CONFIG_KERNEL_LOG_BUFFER
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                ಯದಿ (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    ಯದಿ (pptr_to_paddr((ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                        ksUserLogBuffer = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_ಅಂಕೆ_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#endif /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)
                );
                ಭಙ್ಗ;
#ifdef CONFIG_IOMMU
            ಪಕ್ಷ X86_MappingIOSpace:
                unmapIOPage(cap);
                ಭಙ್ಗ;
#endif
            ಸ್ವಭಾವ:
                fail("No mapping type for mapped cap");
                ಭಙ್ಗ;
            }
        }
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ಪ್ರತ್ಯಾಗಮ false;
}

word_t Mode_getObjectSize(word_t t)
{
    fail("Invalid object type");
    ಪ್ರತ್ಯಾಗಮ 0;
}

cap_t Mode_createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_X86_4K:
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_ಕೆಳಗಿನ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ಮೇಲಿನ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ಪಕ್ಷ seL4_X86_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_ಕೆಳಗಿನ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ಮೇಲಿನ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    ಪಕ್ಷ seL4_X86_PageTableObject:
        ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    ಪಕ್ಷ seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        ಪ್ರತ್ಯಾಗಮ cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#ifdef CONFIG_IOMMU
    ಪಕ್ಷ seL4_X86_IOPageTableObject:
        ಪ್ರತ್ಯಾಗಮ cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#endif

    ಸ್ವಭಾವ:
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
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_directory_cap:
    ಪಕ್ಷ cap_page_table_cap:
    ಪಕ್ಷ cap_frame_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ಸ್ವಭಾವ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}
