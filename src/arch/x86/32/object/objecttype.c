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


bool_t Arch_is_چوکٺو_Type(لفظ_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_صفحو_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t گهڻائي_derive_Cap(cte_t *جڳهه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکٺو_cap:
        cap = cap_چوکٺو_cap_سيٽ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t گهڻائي_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_چوکٺو_cap:
        إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_چوکٺو_cap_get_cap_FMap_Type(cap)) {
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکٺو_cap_get_cap_FSize(cap),
                    cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                    (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#جيڪڏهن_پورو
            حالة X86_MappingVSpace:

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                إذا (unlikely(cap_چوکٺو_cap_get_cap_FSize(cap) == X86_Large_صفحو)) {
                    إذا (pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                        ks_User_Log_بفر = 0;

                        /* Invalidate log page table entries */
                        clear_يادگيري(ia32KSGlobalLogPT, se_L4_صفحو_جدول_بٽ);

                        لكل (صحيح idx = 0; idx < ٻٽو_انگ(PT_اشاريو_بٽ); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_صفحو_بٽ), نقاب(ks_انگ_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_صفحو(
                    cap_چوکٺو_cap_get_cap_FSize(cap),
                    cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                    (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#جيڪڏهن_پورو
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

bool_t CONST گهڻائي_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    أرجع false;
}

لفظ_t گهڻائي_get_Object_سائيز(لفظ_t t)
{
    fail("Invalid object type");
    أرجع 0;
}

cap_t گهڻائي_create_Object(object_t t, فراغ *region_بنياد, لفظ_t user_سائيز, bool_t device_يادگيري)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع cap_چوکٺو_cap_new(
                   X86_Small_صفحو,          /* capFSize             */
                   ASID_هيٺيون(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_يادگيري,           /* capFIsDevice         */
                   ASID_مٿاهون(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_بنياد      /* capFBasePtr          */
               );

    حالة se_L4_X86_Large_صفحو_Object:
        أرجع cap_چوکٺو_cap_new(
                   X86_Large_صفحو,          /* capFSize             */
                   ASID_هيٺيون(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_يادگيري,           /* capFIsDevice         */
                   ASID_مٿاهون(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (لفظ_t)region_بنياد      /* capFBasePtr          */
               );

    حالة se_L4_X86_صفحو_جدول_Object:
        أرجع cap_صفحو_جدول_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (لفظ_t)region_بنياد  /* capPTBasePtr         */
               );

    حالة se_L4_X86_صفحو_Directory_Object:
        copyGlobalMappings(region_بنياد);
        أرجع cap_صفحو_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (لفظ_t)region_بنياد  /* capPDBasePtr       */
               );

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_صفحو_جدول_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (لفظ_t)region_بنياد  /* capIOPTBasePtr */
               );
#جيڪڏهن_پورو

    افتراضي:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t گهڻائي_decode_Invocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *جڳهه,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_directory_cap:
    حالة cap_صفحو_جدول_cap:
    حالة cap_چوکٺو_cap:
        أرجع decodeX86MMUInvocation(invLabel, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
