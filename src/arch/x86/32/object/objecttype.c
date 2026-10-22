/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/ept.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>


bool_t Arch_is_ਚੌਖਟਾ_Type(ਸ਼ਬਦ_t type)
{
    ਚੋਣ (type) {
    ਹਾਲਤ seL4_X86_4K:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_X86_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

deriveCap_ret_t ਬਹੁਲਕ_derive_Cap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਮੂਲ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t ਬਹੁਲਕ_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
            ਚੋਣ (cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
            ਹਾਲਤ X86_MappingEPT:
                unmapEPTPage(
                    cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                    (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
                );
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ
            ਹਾਲਤ X86_MappingVSpace:

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap) == X86_Large_ਪੰਨਾ)) {
                    ਜੇ (pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_ਬਫਰ) {
                        ks_User_Log_ਬਫਰ = 0;

                        /* Invalidate log page table entries */
                        clear_ਸਮ੍ਰਿਤੀ(ia32KSGlobalLogPT, se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);

                        ਲਈ (ਪੂਰਨ idx = 0; idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_ਪੰਨਾ_ਬਿੱਟ), ਨਕਾਬ(ks_ਅੰਕ_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_ਪੰਨਾ(
                    cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                    (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
                );
                ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
            ਹਾਲਤ X86_MappingIOSpace:
                unmapIOPage(cap);
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ
            ਮੂਲ:
                fail("No mapping type for mapped cap");
                ਰੋਕੋ;
            }
        }
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST ਬਹੁਲਕ_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    ਵਾਪਸ false;
}

ਸ਼ਬਦ_t ਬਹੁਲਕ_get_Object_ਮਾਪ(ਸ਼ਬਦ_t t)
{
    fail("Invalid object type");
    ਵਾਪਸ 0;
}

cap_t ਬਹੁਲਕ_create_Object(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
    ਚੋਣ (t) {
    ਹਾਲਤ seL4_X86_4K:
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   X86_Small_ਪੰਨਾ,          /* capFSize             */
                   ASID_ਨੀਵਾਂ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_ਸਮ੍ਰਿਤੀ,           /* capFIsDevice         */
                   ASID_ਉੱਚਾ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ      /* capFBasePtr          */
               );

    ਹਾਲਤ se_L4_X86_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   X86_Large_ਪੰਨਾ,          /* capFSize             */
                   ASID_ਨੀਵਾਂ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_ਸਮ੍ਰਿਤੀ,           /* capFIsDevice         */
                   ASID_ਉੱਚਾ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ      /* capFBasePtr          */
               );

    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capPTBasePtr         */
               );

    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_Directory_Object:
        copyGlobalMappings(region_ਅਧਾਰ);
        ਵਾਪਸ cap_ਪੰਨਾ_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capPDBasePtr       */
               );

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ se_L4_X86_IOPage_ਸਾਰਣੀ_Object:
        ਵਾਪਸ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capIOPTBasePtr */
               );
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t ਬਹੁਲਕ_decode_Invocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decodeX86MMUInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਮੂਲ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}
