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


bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    انتخاب (type) {
    حالت seL4_X86_4K:
        واپس true;
    حالت se_L4_X86_Large_صفحہ_Object:
        واپس true;
    طےشدہ:
        واپس false;
    }
}

deriveCap_ret_t منوال_derive_Cap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_چوکھٹا_cap:
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    طےشدہ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t منوال_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {

    حالت cap_چوکھٹا_cap:
        اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            انتخاب (cap_چوکھٹا_cap_get_cap_FMap_Type(cap)) {
#اگر_تعریف_شدہ CONFIG_VTX
            حالت X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                توڑو;
#اگر_ختم
            حالت X86_MappingVSpace:

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                اگر (unlikely(cap_چوکھٹا_cap_get_cap_FSize(cap) == X86_Large_صفحہ)) {
                    اگر (pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                        ks_User_Log_بفر = 0;

                        /* Invalidate log page table entries */
                        clear_حافظہ(ia32KSGlobalLogPT, se_L4_صفحہ_جدول_بٹ);

                        برائے (صحیح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_صفحہ_بٹ), نقاب(ks_عدد_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_صفحہ(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                توڑو;
#اگر_تعریف_شدہ CONFIG_IOMMU
            حالت X86_MappingIOSpace:
                unmapIOPage(cap);
                توڑو;
#اگر_ختم
            طےشدہ:
                fail("No mapping type for mapped cap");
                توڑو;
            }
        }
        توڑو;

    طےشدہ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST منوال_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    واپس false;
}

لفظ_t منوال_get_Object_سائز(لفظ_t t)
{
    fail("Invalid object type");
    واپس 0;
}

cap_t منوال_create_Object(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    انتخاب (t) {
    حالت seL4_X86_4K:
        واپس cap_چوکھٹا_cap_new(
                   X86_Small_صفحہ,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظہ,           /* capFIsDevice         */
                   ASID_بلند(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_بنیاد      /* capFBasePtr          */
               );

    حالت se_L4_X86_Large_صفحہ_Object:
        واپس cap_چوکھٹا_cap_new(
                   X86_Large_صفحہ,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظہ,           /* capFIsDevice         */
                   ASID_بلند(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_بنیاد      /* capFBasePtr          */
               );

    حالت se_L4_X86_صفحہ_جدول_Object:
        واپس cap_صفحہ_جدول_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (لفظ_t)region_بنیاد  /* capPTBasePtr         */
               );

    حالت se_L4_X86_صفحہ_Directory_Object:
        copyGlobalMappings(region_بنیاد);
        واپس cap_صفحہ_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (لفظ_t)region_بنیاد  /* capPDBasePtr       */
               );

#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت se_L4_X86_IOPage_جدول_Object:
        واپس cap_io_صفحہ_جدول_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (لفظ_t)region_بنیاد  /* capIOPTBasePtr */
               );
#اگر_ختم

    طےشدہ:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t منوال_decode_Invocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_directory_cap:
    حالت cap_صفحہ_جدول_cap:
    حالت cap_چوکھٹا_cap:
        واپس decodeX86MMUInvocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
    طےشدہ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}
