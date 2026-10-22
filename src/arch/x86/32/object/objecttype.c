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


bool_t Arch_is_ಚೌಕಟ್ಟು_Type(ಶಬ್ದ_t type)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ seL4_X86_4K:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_X86_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

deriveCap_ret_t ಬಹುಲಕ_derive_Cap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಪೂರ್ವನಿಯೋಜಿತ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t ಬಹುಲಕ_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
            ಆಯ್ಕೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
            ಸ್ಥಿತಿ X86_MappingEPT:
                unmapEPTPage(
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                    (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
                );
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
            ಸ್ಥಿತಿ X86_MappingVSpace:

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap) == X86_Large_ಪುಟ)) {
                    ಆದರೆ (pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_ಬಫರ್) {
                        ks_User_Log_ಬಫರ್ = 0;

                        /* Invalidate log page table entries */
                        clear_ಸ್ಮರಣೆ(ia32KSGlobalLogPT, se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

                        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು), ಮುಖವಾಡ(ks_ಅಂಕೆ_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_ಪುಟ(
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                    cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                    (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
                );
                ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
            ಸ್ಥಿತಿ X86_MappingIOSpace:
                unmapIOPage(cap);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
            ಪೂರ್ವನಿಯೋಜಿತ:
                fail("No mapping type for mapped cap");
                ನಿಲ್ಲಿಸು;
            }
        }
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST ಬಹುಲಕ_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    ಹಿಂತಿರುಗಿ false;
}

ಶಬ್ದ_t ಬಹುಲಕ_get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t)
{
    fail("Invalid object type");
    ಹಿಂತಿರುಗಿ 0;
}

cap_t ಬಹುಲಕ_create_Object(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ seL4_X86_4K:
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   X86_Small_ಪುಟ,          /* capFSize             */
                   ASID_ಕೆಳಗಿನ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_ಸ್ಮರಣೆ,           /* capFIsDevice         */
                   ASID_ಮೇಲಿನ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (ಶಬ್ದ_t)region_ತಳ      /* capFBasePtr          */
               );

    ಸ್ಥಿತಿ se_L4_X86_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   X86_Large_ಪುಟ,          /* capFSize             */
                   ASID_ಕೆಳಗಿನ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_ಸ್ಮರಣೆ,           /* capFIsDevice         */
                   ASID_ಮೇಲಿನ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (ಶಬ್ದ_t)region_ತಳ      /* capFBasePtr          */
               );

    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (ಶಬ್ದ_t)region_ತಳ  /* capPTBasePtr         */
               );

    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_Directory_Object:
        copyGlobalMappings(region_ತಳ);
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (ಶಬ್ದ_t)region_ತಳ  /* capPDBasePtr       */
               );

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ se_L4_X86_IOPage_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (ಶಬ್ದ_t)region_ತಳ  /* capIOPTBasePtr */
               );
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t ಬಹುಲಕ_decode_Invocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decodeX86MMUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}
