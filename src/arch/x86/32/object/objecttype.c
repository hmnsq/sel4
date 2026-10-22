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
        प्रत्यागम true;
    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

deriveCap_ret_t बहुलकम्_derive_Cap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
        cap = cap_frame_cap_समुच्चयः_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asidInvalid);
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t बहुलकम्_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_frame_cap:
        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
            विकल्प (cap_frame_cap_get_capFMapType(cap)) {
#यदि_परिभाषितम् CONFIG_VTX
            पक्ष X86_MappingEPT:
                unmapEPTPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
#यदि_अन्तः
            पक्ष X86_MappingVSpace:

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                यदि (unlikely(cap_frame_cap_get_capFSize(cap) == X86_LargePage)) {
                    यदि (pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_बफर) {
                        ks_User_Log_बफर = 0;

                        /* Invalidate log page table entries */
                        clear_स्मृतिः(ia32KSGlobalLogPT, se_L4_Page_सारणी_द्वयङ्काः);

                        प्रति (पूर्णाङ्क idx = 0; idx < BIT(PT_सूचकांक_द्वयङ्काः); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_Page_द्वयङ्काः), आवरणम्(ks_अङ्कः_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

                unmapPage(
                    cap_frame_cap_get_capFSize(cap),
                    cap_frame_cap_get_capFMappedASID(cap),
                    cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                    (शून्य *)cap_frame_cap_get_capFBasePtr(cap)
                );
                भङ्ग;
#यदि_परिभाषितम् CONFIG_IOMMU
            पक्ष X86_MappingIOSpace:
                unmapIOPage(cap);
                भङ्ग;
#यदि_अन्तः
            स्वभाव:
                fail("No mapping type for mapped cap");
                भङ्ग;
            }
        }
        भङ्ग;

    स्वभाव:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST बहुलकम्_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    प्रत्यागम false;
}

word_t बहुलकम्_get_Object_आकार(word_t t)
{
    fail("Invalid object type");
    प्रत्यागम 0;
}

cap_t बहुलकम्_create_Object(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
    विकल्प (t) {
    पक्ष seL4_X86_4K:
        प्रत्यागम cap_frame_cap_new(
                   X86_SmallPage,          /* capFSize             */
                   ASID_नीचः(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_स्मृतिः,           /* capFIsDevice         */
                   ASID_उच्चः(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)region_आधारः      /* capFBasePtr          */
               );

    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम cap_frame_cap_new(
                   X86_LargePage,          /* capFSize             */
                   ASID_नीचः(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_स्मृतिः,           /* capFIsDevice         */
                   ASID_उच्चः(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (word_t)region_आधारः      /* capFBasePtr          */
               );

    पक्ष se_L4_X86_Page_सारणी_Object:
        प्रत्यागम cap_page_सारणी_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (word_t)region_आधारः  /* capPTBasePtr         */
               );

    पक्ष seL4_X86_PageDirectoryObject:
        copyGlobalMappings(region_आधारः);
        प्रत्यागम cap_page_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (word_t)region_आधारः  /* capPDBasePtr       */
               );

#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष se_L4_X86_IOPage_सारणी_Object:
        प्रत्यागम cap_io_page_सारणी_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (word_t)region_आधारः  /* capIOPTBasePtr */
               );
#यदि_अन्तः

    स्वभाव:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t बहुलकम्_decode_Invocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
    पक्ष cap_page_सारणी_cap:
    पक्ष cap_frame_cap:
        प्रत्यागम decodeX86MMUInvocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
    स्वभाव:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}
