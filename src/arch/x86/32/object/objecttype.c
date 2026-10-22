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


bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_ورقہ_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t منوال_derive_Cap(cte_t *تھاں, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکھٹا_cap:
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t منوال_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_چوکھٹا_cap:
        إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_چوکھٹا_cap_get_cap_FMap_Type(cap)) {
#جے_تعریف_شدہ CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#جے_انت
            حالة X86_MappingVSpace:

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                إذا (unlikely(cap_چوکھٹا_cap_get_cap_FSize(cap) == X86_Large_ورقہ)) {
                    إذا (pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                        ks_User_Log_بفر = 0;

                        /* Invalidate log page table entries */
                        clear_حافظہ(ia32KSGlobalLogPT, se_L4_ورقہ_جدول_بٹ);

                        لكل (صحيح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_ورقہ_بٹ), نقاب(ks_عدد_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_ورقہ(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#جے_تعریف_شدہ CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#جے_انت
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

bool_t CONST منوال_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    أرجع false;
}

لفظ_t منوال_get_Object_ناپ(لفظ_t t)
{
    fail("Invalid object type");
    أرجع 0;
}

cap_t منوال_create_Object(object_t t, فراغ *region_ادھار, لفظ_t user_ناپ, bool_t device_حافظہ)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع cap_چوکھٹا_cap_new(
                   X86_Small_ورقہ,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظہ,           /* capFIsDevice         */
                   ASID_بلند(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_ادھار      /* capFBasePtr          */
               );

    حالة se_L4_X86_Large_ورقہ_Object:
        أرجع cap_چوکھٹا_cap_new(
                   X86_Large_ورقہ,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظہ,           /* capFIsDevice         */
                   ASID_بلند(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_ادھار      /* capFBasePtr          */
               );

    حالة se_L4_X86_ورقہ_جدول_Object:
        أرجع cap_ورقہ_جدول_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (لفظ_t)region_ادھار  /* capPTBasePtr         */
               );

    حالة se_L4_X86_ورقہ_Directory_Object:
        copyGlobalMappings(region_ادھار);
        أرجع cap_ورقہ_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (لفظ_t)region_ادھار  /* capPDBasePtr       */
               );

#جے_تعریف_شدہ CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_ورقہ_جدول_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (لفظ_t)region_ادھار  /* capIOPTBasePtr */
               );
#جے_انت

    افتراضي:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t منوال_decode_Invocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *تھاں,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_directory_cap:
    حالة cap_ورقہ_جدول_cap:
    حالة cap_چوکھٹا_cap:
        أرجع decodeX86MMUInvocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
