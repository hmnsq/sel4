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


bool_t Arch_is_قاب_Type(کلمه_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_صفحه_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t نما_derive_Cap(cte_t *جایگاه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_قاب_cap:
        cap = cap_قاب_cap_مجموعه_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asidInvalid);
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t نما_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_قاب_cap:
        إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_قاب_cap_get_cap_FMap_Type(cap)) {
#اگر_تعریف_شده CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_قاب_cap_get_cap_FSize(cap),
                    cap_قاب_cap_get_cap_FMapped_ASID(cap),
                    cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                    (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#پایان_اگر
            حالة X86_MappingVSpace:

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                إذا (unlikely(cap_قاب_cap_get_cap_FSize(cap) == X86_Large_صفحه)) {
                    إذا (pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_میانگیر) {
                        ks_User_Log_میانگیر = 0;

                        /* Invalidate log page table entries */
                        clear_حافظه(ia32KSGlobalLogPT, se_L4_صفحه_جدول_بیتها);

                        لكل (صحيح idx = 0; idx < بیت(PT_شاخص_بیتها); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_صفحه_بیتها), ماسک(ks_رقم_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_صفحه(
                    cap_قاب_cap_get_cap_FSize(cap),
                    cap_قاب_cap_get_cap_FMapped_ASID(cap),
                    cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                    (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#اگر_تعریف_شده CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#پایان_اگر
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

bool_t CONST نما_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    أرجع false;
}

کلمه_t نما_get_Object_اندازه(کلمه_t t)
{
    fail("Invalid object type");
    أرجع 0;
}

cap_t نما_create_Object(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع cap_قاب_cap_new(
                   X86_Small_صفحه,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظه,           /* capFIsDevice         */
                   ASID_بالا(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (کلمه_t)region_پایه      /* capFBasePtr          */
               );

    حالة se_L4_X86_Large_صفحه_Object:
        أرجع cap_قاب_cap_new(
                   X86_Large_صفحه,          /* capFSize             */
                   ASID_کم(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_حافظه,           /* capFIsDevice         */
                   ASID_بالا(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (کلمه_t)region_پایه      /* capFBasePtr          */
               );

    حالة se_L4_X86_صفحه_جدول_Object:
        أرجع cap_صفحه_جدول_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (کلمه_t)region_پایه  /* capPTBasePtr         */
               );

    حالة se_L4_X86_صفحه_Directory_Object:
        copyGlobalMappings(region_پایه);
        أرجع cap_صفحه_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (کلمه_t)region_پایه  /* capPDBasePtr       */
               );

#اگر_تعریف_شده CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_صفحه_جدول_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (کلمه_t)region_پایه  /* capIOPTBasePtr */
               );
#پایان_اگر

    افتراضي:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t نما_decode_Invocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_directory_cap:
    حالة cap_صفحه_جدول_cap:
    حالة cap_قاب_cap:
        أرجع decodeX86MMUInvocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
