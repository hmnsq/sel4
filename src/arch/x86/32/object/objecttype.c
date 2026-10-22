/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <kernel/vspace.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/machine/fpu.h>
#समावेश <arch/object/objecttype.h>
#समावेश <arch/object/ioport.h>
#समावेश <arch/kernel/ept.h>

#समावेश <arch/object/iospace.h>
#समावेश <plat/machine/intel-vtd.h>


bool_t Arch_isFrameType(word_t type)
{
    चयन (type) {
    हाल seL4_X86_4K:
        वापस true;
    हाल seL4_X86_LargePageObject:
        वापस true;
    शेष:
        वापस false;
    }
}

deriveCap_ret_t Mode_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_frame_cap:
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        वापस ret;

    शेष:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t Mode_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    चयन (cap_get_capType(cap)) {

    हाल cap_frame_cap:
        अगर (cap_frame_cap_get_capFMappedASID(cap)) {
            चयन (cap_frame_cap_get_capFMapType(cap)) {
#अगर_घोषित CONFIG_VTX
            हाल X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (व्योम *)cap_frame_cap_get_capFBasePtr(cap)
                );
                अवरोध;
#पूर्ण_अगर
            हाल X86_MappingVSpace:

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                अगर (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    अगर (pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_बफर) {
                        ks_User_Log_बफर = 0;

                        /* Invalidate log page table entries */
                        clearMemory(ia32KSGlobalLogPT, seL4_PageTableBits);

                        क्रम (पूर्णांक idx = 0; idx < BIT(PT_सूचकांक_BITS); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_अंक_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_capFMappedAddress(cap),
                    (व्योम *)cap_frame_cap_get_capFBasePtr(cap)
                );
                अवरोध;
#अगर_घोषित CONFIG_IOMMU
            हाल X86_MappingIOSpace:
                unmapIOPage(cap);
                अवरोध;
#पूर्ण_अगर
            शेष:
                fail("No mapping type for mapped cap");
                अवरोध;
            }
        }
        अवरोध;

    शेष:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST Mode_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    वापस false;
}

word_t Mode_get_Object_आकार(word_t t)
{
    fail("Invalid object type");
    वापस 0;
}

cap_t Mode_createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
    चयन (t) {
    हाल seL4_X86_4K:
        वापस cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_निचला(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ऊपरी(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    हाल seL4_X86_LargePageObject:
        वापस cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_निचला(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   deviceMemory,           /* capFIsDevice         */
                   ASID_ऊपरी(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)regionBase      /* capFBasePtr          */
               );

    हाल seL4_X86_PageTableObject:
        वापस cap_page_table_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)regionBase  /* capPTBasePtr         */
               );

    हाल seL4_X86_PageDirectoryObject:
        copyGlobalMappings(regionBase);
        वापस cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)regionBase  /* capPDBasePtr       */
               );

#अगर_घोषित CONFIG_IOMMU
    हाल seL4_X86_IOPageTableObject:
        वापस cap_io_page_table_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)regionBase  /* capIOPTBasePtr */
               );
#पूर्ण_अगर

    शेष:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t Mode_decodeInvocation(
    word_t invLabel,
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_page_directory_cap:
    हाल cap_page_table_cap:
    हाल cap_frame_cap:
        वापस decodeX86MMUInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
    शेष:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}
