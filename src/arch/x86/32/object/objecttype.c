/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/machine/fpu.h>
#అన్తర్భావ <arch/object/objecttype.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <arch/kernel/ept.h>

#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <plat/machine/intel-vtd.h>


bool_t Arch_is_చట్రం_Type(మాట_t type)
{
    ఎంపిక (type) {
    స్థితి seL4_X86_4K:
        తిరిగి true;
    స్థితి se_L4_X86_Large_పుట_Object:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}

deriveCap_ret_t బహుళకం_derive_Cap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_చట్రం_cap:
        cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asidInvalid);
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    అప్రమేయ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t బహుళకం_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ఎంపిక (cap_get_capType(cap)) {

    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
            ఎంపిక (cap_చట్రం_cap_get_cap_FMap_Type(cap)) {
#నిర్వచితమైతే CONFIG_VTX
            స్థితి X86_MappingEPT:
                unmapEPTPage(
                    cap_చట్రం_cap_get_cap_FSize(cap),
                    cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                    cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                    (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
                );
                ఆపు;
#అయితే_ముగింపు
            స్థితి X86_MappingVSpace:

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
                /* If the last cap to the user-level log buffer frame is being revoked,
                 * reset the ksLog so that the kernel doesn't log anymore
                 */
                ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FSize(cap) == X86_Large_పుట)) {
                    ఒకవేళ (pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_బఫరు) {
                        ks_User_Log_బఫరు = 0;

                        /* Invalidate log page table entries */
                        clear_జ్ఞాపకం(ia32KSGlobalLogPT, se_L4_పుట_పట్టిక_బిట్లు);

                        కొరకు (పూర్ణాంకం idx = 0; idx < బిట్టు(PT_సూచీ_బిట్లు); idx++) {
                            invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_పుట_బిట్లు), ముసుగు(ks_అంకె_CPUs));
                        }

                        userError("Log buffer frame is invalidated, kernel can't benchmark anymore\n");
                    }
                }
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

                unmap_పుట(
                    cap_చట్రం_cap_get_cap_FSize(cap),
                    cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                    cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                    (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
                );
                ఆపు;
#నిర్వచితమైతే CONFIG_IOMMU
            స్థితి X86_MappingIOSpace:
                unmapIOPage(cap);
                ఆపు;
#అయితే_ముగింపు
            అప్రమేయ:
                fail("No mapping type for mapped cap");
                ఆపు;
            }
        }
        ఆపు;

    అప్రమేయ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST బహుళకం_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    తిరిగి false;
}

మాట_t బహుళకం_get_Object_కొలత(మాట_t t)
{
    fail("Invalid object type");
    తిరిగి 0;
}

cap_t బహుళకం_create_Object(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
    ఎంపిక (t) {
    స్థితి seL4_X86_4K:
        తిరిగి cap_చట్రం_cap_new(
                   X86_Small_పుట,          /* capFSize             */
                   ASID_దిగువ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_జ్ఞాపకం,           /* capFIsDevice         */
                   ASID_ఎగువ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (మాట_t)region_పునాది      /* capFBasePtr          */
               );

    స్థితి se_L4_X86_Large_పుట_Object:
        తిరిగి cap_చట్రం_cap_new(
                   X86_Large_పుట,          /* capFSize             */
                   ASID_దిగువ(asidInvalid),  /* capFMappedASIDLow    */
                   false,                  /* capFMappedAddress    */
                   X86_MappingNone,        /* capFMapType          */
                   device_జ్ఞాపకం,           /* capFIsDevice         */
                   ASID_ఎగువ(asidInvalid), /* capFMappedASIDHigh   */
                   VMReadWrite,            /* capFVMRights         */
                   (మాట_t)region_పునాది      /* capFBasePtr          */
               );

    స్థితి se_L4_X86_పుట_పట్టిక_Object:
        తిరిగి cap_పుట_పట్టిక_cap_new(
                   0,                  /* capPTIsMapped        */
                   asidInvalid,        /* capPTMappedASID      */
                   0,                  /* capPTMappedAddress   */
                   (మాట_t)region_పునాది  /* capPTBasePtr         */
               );

    స్థితి se_L4_X86_పుట_Directory_Object:
        copyGlobalMappings(region_పునాది);
        తిరిగి cap_పుట_directory_cap_new(
                   0,                  /* capPDIsMapped      */
                   asidInvalid,        /* capPDMappedASID    */
                   0,                  /* capPDMappedAddress */
                   (మాట_t)region_పునాది  /* capPDBasePtr       */
               );

#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి se_L4_X86_IOPage_పట్టిక_Object:
        తిరిగి cap_io_పుట_పట్టిక_cap_new(
                   0,  /* capIOPTIsMapped      */
                   0,  /* capIOPTLevel         */
                   0,  /* capIOPTMappedAddress */
                   0,  /* capIOPTIOASID        */
                   (మాట_t)region_పునాది  /* capIOPTBasePtr */
               );
#అయితే_ముగింపు

    అప్రమేయ:
        fail("Mode_createObject got an API type or invalid object type");
    }
}

exception_t బహుళకం_decode_Invocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_directory_cap:
    స్థితి cap_పుట_పట్టిక_cap:
    స్థితి cap_చట్రం_cap:
        తిరిగి decodeX86MMUInvocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    అప్రమేయ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}
