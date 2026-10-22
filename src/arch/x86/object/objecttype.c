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
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/object/objecttype.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <plat/machine/devices.h>

#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_asid_control_cap:
    స్థితి cap_asid_pool_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    స్థితి cap_io_పోర్టు_control_cap:
        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        తిరిగి ret;
    స్థితి cap_io_పోర్టు_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    స్థితి cap_ept_pml4_cap:
        ఒకవేళ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;
    స్థితి cap_ept_pdpt_cap:
        ఒకవేళ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_ept_pd_cap:
        ఒకవేళ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_ept_pt_cap:
        ఒకవేళ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;
#అయితే_ముగింపు

    అప్రమేయ:
        తిరిగి బహుళకం_derive_Cap(స్థానం, cap);
    }
}

cap_t CONST Arch_update_Cap_దత్తాంశం(bool_t preserve, మాట_t దత్తాంశం, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#నిర్వచితమైతే CONFIG_IOMMU
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_దత్తాంశం_t w = { { దత్తాంశం } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_దత్తాంశం_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_దత్తాంశం_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_దత్తాంశం_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_దత్తాంశం_get_domain_ID(w);
        ఒకవేళ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= ముసుగు(x86KSnumIODomainIDBits)) {
            తిరిగి cap_io_space_cap_new(domainID, PCIDevice);
        } లేకపోతే {
            తిరిగి cap_null_cap_new();
        }
    }

    అప్రమేయ:
        తిరిగి cap;
    }
#అయితే_ముగింపు
    తిరిగి cap;
}

cap_t CONST Arch_ముసుగు_Cap_Rights(seL4_CapRights_t cap_rights_ముసుగు, cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) == cap_చట్రం_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_మాట(cap_చట్రం_cap_get_cap_FVMRights(cap));
        vm_rights = ముసుగు_VMRights(vm_rights, cap_rights_ముసుగు);
        తిరిగి cap_చట్రం_cap_సమితి_cap_FVMRights(cap, మాట_From_VMRights(vm_rights));
    } లేకపోతే {
        తిరిగి cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (final && cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_పుట_Directory(
                cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_పుట_directory_cap_get_cap_PDMapped_చిరునామా(cap),
                PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_పుట_పట్టిక(
                cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
                cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
                PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        ఆపు;

    స్థితి cap_asid_pool_cap:
        ఒకవేళ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ఆపు;
    స్థితి cap_asid_control_cap:
    స్థితి cap_io_పోర్టు_control_cap:
        ఆపు;
    స్థితి cap_io_పోర్టు_cap:
#నిర్వచితమైతే CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_పోర్టు_cap_get_cap_IOPort_VPID(cap),
                                cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap),
                                cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap));
#అయితే_ముగింపు
        ఒకవేళ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            తిరిగి fc_ret;
        }
        ఆపు;
#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap:
        ఒకవేళ (final) {
            unmap_VTDContext_నమోదు(cap);
        }
        ఆపు;

    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_పట్టిక(cap);
        }
        ఆపు;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap:
        ఒకవేళ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ఆపు;
    స్థితి cap_ept_pml4_cap:
        ఒకవేళ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(cap));
        }
        ఆపు;

    స్థితి cap_ept_pdpt_cap:
        ఒకవేళ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_చిరునామా(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ఆపు;

    స్థితి cap_ept_pd_cap:
        ఒకవేళ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_చిరునామా(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ఆపు;

    స్థితి cap_ept_pt_cap:
        ఒకవేళ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_పట్టిక(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_చిరునామా(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ఆపు;
#అయితే_ముగింపు

    అప్రమేయ:
        తిరిగి బహుళకం_finalise_Cap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ఎంపిక (cap_get_capType(cap_a)) {
    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {
            మాట_t botA, botB, శిఖరం_A, శిఖరం_B;
            botA = cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b);
            శిఖరం_A = botA + ముసుగు(పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap_a)));
            శిఖరం_B = botB + ముసుగు(పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap_b)));
            తిరిగి ((botA <= botB) && (శిఖరం_A >= శిఖరం_B) && (botB <= శిఖరం_B));
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_పుట_పట్టిక_cap) {
            తిరిగి cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_పుట_directory_cap) {
            తిరిగి cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_asid_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            తిరిగి true;
        }
        ఆపు;

    స్థితి cap_asid_pool_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            తిరిగి cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ఆపు;

    స్థితి cap_io_పోర్టు_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_పోర్టు_control_cap ||
            cap_get_capType(cap_b) == cap_io_పోర్టు_cap) {
            తిరిగి true;
        }
        ఆపు;

    స్థితి cap_io_పోర్టు_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_పోర్టు_cap) {
            తిరిగి  cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap_a) ==
                    cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap_b) &&
                    cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap_a) ==
                    cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap_b);
        }
        ఆపు;

#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_space_cap) {
            తిరిగి cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ఆపు;

    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_పుట_పట్టిక_cap) {
            తిరిగి cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ఆపు;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            తిరిగి cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ఆపు;

    స్థితి cap_ept_pml4_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            తిరిగి cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_ept_pdpt_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            తిరిగి cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ఆపు;

    స్థితి cap_ept_pd_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            తిరిగి cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ఆపు;

    స్థితి cap_ept_pt_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            తిరిగి cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ఆపు;

#అయితే_ముగింపు

    }

    తిరిగి బహుళకం_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ఒకవేళ (cap_get_capType(cap_a) == cap_io_పోర్టు_control_cap &&
        cap_get_capType(cap_b) == cap_io_పోర్టు_cap) {
        తిరిగి false;
    }
    ఒకవేళ (cap_get_capType(cap_a) == cap_చట్రం_cap) {
        ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {
            తిరిగి ((cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_చట్రం_cap_get_cap_FSize(cap_a) ==
                     cap_చట్రం_cap_get_cap_FSize(cap_b)) &&
                    ((cap_చట్రం_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_చట్రం_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
    తిరిగి Arch_sameRegionAs(cap_a, cap_b);
}

మాట_t Arch_get_Object_కొలత(మాట_t t)
{
    ఎంపిక (t) {
    స్థితి seL4_X86_4K:
        తిరిగి పుట_బిట్లు_For_కొలత(X86_Small_పుట);
    స్థితి se_L4_X86_Large_పుట_Object:
        తిరిగి పుట_బిట్లు_For_కొలత(X86_Large_పుట);
    స్థితి se_L4_X86_పుట_పట్టిక_Object:
        తిరిగి se_L4_పుట_పట్టిక_బిట్లు;
    స్థితి se_L4_X86_పుట_Directory_Object:
        తిరిగి se_L4_పుట_Dir_బిట్లు;
    స్థితి seL4_IA32_PDPTObject:
        తిరిగి seL4_PDPTBits;
    స్థితి se_L4_X86_IOPage_పట్టిక_Object:
        తిరిగి se_L4_IOPage_పట్టిక_బిట్లు;
#నిర్వచితమైతే CONFIG_VTX
    స్థితి seL4_X86_VCPUObject:
        తిరిగి seL4_X86_VCPUBits;
    స్థితి seL4_X86_EPTPML4Object:
        తిరిగి se_L4_X86_EPTPML4_బిట్లు;
    స్థితి seL4_X86_EPTPDPTObject:
        తిరిగి seL4_X86_EPTPDPTBits;
    స్థితి seL4_X86_EPTPDObject:
        తిరిగి seL4_X86_EPTPDBits;
    స్థితి seL4_X86_EPTPTObject:
        తిరిగి seL4_X86_EPTPTBits;
#అయితే_ముగింపు
    అప్రమేయ:
        తిరిగి బహుళకం_get_Object_కొలత(t);
    }
}

cap_t Arch_createObject(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
#నిర్వచితమైతే CONFIG_VTX
    ఎంపిక (t) {
    స్థితి seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((మాట_t)region_పునాది);
        vcpu_init(vcpu);
        తిరిగి cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    స్థితి seL4_X86_EPTPML4Object:
        తిరిగి cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (మాట_t)region_పునాది  /* capPML4BasePtr       */
               );
    స్థితి seL4_X86_EPTPDPTObject:
        తిరిగి cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (మాట_t)region_పునాది   /* capPDPTBasePtr      */
               );
    స్థితి seL4_X86_EPTPDObject:
        తిరిగి cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (మాట_t)region_పునాది  /* capPDBasePtr         */
               );
    స్థితి seL4_X86_EPTPTObject:
        తిరిగి cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (మాట_t)region_పునాది  /* capPTBasePtr         */
               );
    అప్రమేయ:
#అయితే_ముగింపు
        తిరిగి బహుళకం_create_Object(t, region_పునాది, user_కొలత, device_జ్ఞాపకం);
#నిర్వచితమైతే CONFIG_VTX
    }
#అయితే_ముగింపు
}

exception_t Arch_decodeInvocation(
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
    స్థితి cap_asid_control_cap:
    స్థితి cap_asid_pool_cap:
        తిరిగి decodeX86MMUInvocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    స్థితి cap_io_పోర్టు_control_cap:
        తిరిగి decode_X86_పోర్టు_Control_Invocation(invLabel, నిడివి, cptr, స్థానం, cap, బఫరు);
    స్థితి cap_io_పోర్టు_cap:
        తిరిగి decode_X86_పోర్టు_Invocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap:
        తిరిగి decodeX86IOSpaceInvocation(invLabel, cap);
    స్థితి cap_io_పుట_పట్టిక_cap:
        తిరిగి decodeX86IOPTInvocation(invLabel, నిడివి, స్థానం, cap, బఫరు);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap:
        తిరిగి decodeX86VCPUInvocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    స్థితి cap_ept_pml4_cap:
    స్థితి cap_ept_pdpt_cap:
    స్థితి cap_ept_pd_cap:
    స్థితి cap_ept_pt_cap:
        తిరిగి decodeX86EPTInvocation(invLabel, నిడివి, cptr, స్థానం, cap, బఫరు);
#అయితే_ముగింపు
    అప్రమేయ:
        తిరిగి బహుళకం_decode_Invocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    }
}

శూన్య Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

శూన్య Arch_postCapDeletion(cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) == cap_io_పోర్టు_cap) {
        uint16_t మొదటి_పోర్టు = cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap);
        uint16_t చివరి_పోర్టు = cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap);

        freeIOPortRange(మొదటి_పోర్టు, చివరి_పోర్టు);
    }
}
