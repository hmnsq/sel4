/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_UL          0xffffffff
#ವ್ಯಾಖ್ಯಾನಿಸು PT_ಸೂಚ್ಯಂಕ(vptr)  ((vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು))
#ವ್ಯಾಖ್ಯಾನಿಸು PD_ಸೂಚ್ಯಂಕ(vptr)  (vptr >> (ಪುಟ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು))

ಶಬ್ದ_t get_tcb_sp(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ tcb->tcbArch.tcbContext.registers[SP];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(resolve_ret_t ret);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pte);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde_t *pde);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t AP, ಶಬ್ದ_t XN, ಶಬ್ದ_t TEX);
ಸ್ಥಿರ ಶೂನ್ಯ arm32_obj_pt_print_slots(pte_t *pt);
ಸ್ಥಿರ ಶೂನ್ಯ arm32_cap_pt_print_slots(pte_t *pt);

/*
 * Caps
 */

/*
 * AP   S   R   Privileged permissions  User permissions
 * 00   0   0   No access               No access
 * 00   1   0   Read-only               No access
 * 00   0   1   Read-only               Read-only
 * 00   1   1   Unpredictable           Unpredictable
 * 01   x   x   Read/write              No access
 * 10   x   x   Read/write              Read-only
 * 11   x   x   Read/write              Read/write
 */
/* use when only have access to pte of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pte)
{
    ಶಬ್ದ_t AP, XN, TEX;
    ಆಯ್ಕೆ (pte_ptr_get_pteType(pte)) {
    ಸ್ಥಿತಿ pte_pte_small:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#ಇಲ್ಲದಿದ್ದರೆ
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#ಆದರೆ_ಅಂತ್ಯ
        XN = pte_pte_small_ptr_get_XN(pte);
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(!"should not happend");
    }
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(AP, XN, TEX);
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde_t *pde)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#ಇಲ್ಲದಿದ್ದರೆ
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t AP, ಶಬ್ದ_t XN, ಶಬ್ದ_t TEX)
{
    ಬರೆ("(");

    /* rights */
    ಆಯ್ಕೆ (AP) {
    ಸ್ಥಿತಿ 0b00:
    ಸ್ಥಿತಿ 0b01:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b10:
        ಬರೆ("R");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b11:
        ಬರೆ("RW");
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!XN) {
        ಬರೆ("X");
    }

    ಆದರೆ (!TEX) {
        ಬರೆ(", uncached");
    }

    ಬರೆ(")\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ;
    ಶಬ್ದ_t i = 0;
    ಆಗ (i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ಆಯ್ಕೆ (pte_ptr_get_pteType(pte)) {
        ಸ್ಥಿತಿ pte_pte_small: {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            ಆದರೆ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                ಪುಟ_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಪುಟ_ಗಾತ್ರ = ARMSmall_ಪುಟ;
            }
#ಇಲ್ಲದಿದ್ದರೆ
            ಪುಟ_ಗಾತ್ರ = ARMSmall_ಪುಟ;
#ಆದರೆ_ಅಂತ್ಯ
            ಬರೆ("0x%lx: frame_%p_%04lu ", PT_ಸೂಚ್ಯಂಕ(i), pte, PT_ಸೂಚ್ಯಂಕ(i));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte);
            ನಿಲ್ಲಿಸು;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಸ್ಥಿತಿ pte_pte_large: {
            ಪುಟ_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            ಬರೆ("0x%lx: frame_%p_%04lu ", PT_ಸೂಚ್ಯಂಕ(i), pte, PT_ಸೂಚ್ಯಂಕ(i));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte);
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಪುಟ_ಗಾತ್ರ = ARMSmall_ಪುಟ;
        }
        i += (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ));
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_ದ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ;
        ಬರೆ("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ಶಬ್ದ_t i = 0;
        ಆಗ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ಆಯ್ಕೆ (pde_ptr_get_pdeType(pde)) {
            ಸ್ಥಿತಿ pde_pde_section: {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                ಆದರೆ (pde_pde_section_ptr_get_ಗಾತ್ರ(pde)) {
                    ಪುಟ_ಗಾತ್ರ = ARMSuperSection;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಪುಟ_ಗಾತ್ರ = ARMSection;
                }
#ಇಲ್ಲದಿದ್ದರೆ
                ಆದರೆ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    ಪುಟ_ಗಾತ್ರ = ARMSuperSection;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಪುಟ_ಗಾತ್ರ = ARMSection;
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಬರೆ("0x%lx: frame_%p_%04lu ", PD_ಸೂಚ್ಯಂಕ(i), pde, PD_ಸೂಚ್ಯಂಕ(i));
                cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde);
                ನಿಲ್ಲಿಸು;
            }

            ಸ್ಥಿತಿ pde_pde_coarse: {
                ಬರೆ("0x%lx: pt_%p_%04lu\n", PD_ಸೂಚ್ಯಂಕ(i), pde, PD_ಸೂಚ್ಯಂಕ(i));
                ಪುಟ_ಗಾತ್ರ = ARMSection;
                ನಿಲ್ಲಿಸು;
            }
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಪುಟ_ಗಾತ್ರ = ARMSection;
                ನಿಲ್ಲಿಸು;
            }
            i += (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ));
            ಆದರೆ (i < (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ))) {
                ನಿಲ್ಲಿಸು; /* overflowed */
            }
        }
        ಬರೆ("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ಆಗ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ಆದರೆ (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pde));
                ಬರೆ("pt_%p_%04lu {\n", pde, PD_ಸೂಚ್ಯಂಕ(i));
                arm32_cap_pt_print_slots(pt);
                ಬರೆ("}\n"); /* pt */
            }
            i += (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection));
            ಆದರೆ (i < (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection))) {
                ನಿಲ್ಲಿಸು; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    ಆಯ್ಕೆ (pde_ptr_get_pdeType(pde)) {
    ಸ್ಥಿತಿ pde_pde_section: {
        ಬರೆ("frame_%p_%04lu ", pde, PD_ಸೂಚ್ಯಂಕ(vptr));
        cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        ಆಯ್ಕೆ (pte_ptr_get_pteType(pte)) {
        ಸ್ಥಿತಿ pte_pte_small: {
            ಬರೆ("frame_%p_%04lu ", pte, PT_ಸೂಚ್ಯಂಕ(vptr));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte);
            ನಿಲ್ಲಿಸು;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಸ್ಥಿತಿ pte_pte_large: {
            ಬರೆ("frame_%p_%04lu ", pte, PT_ಸೂಚ್ಯಂಕ(vptr));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte);
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಪ್ರತಿಜ್ಞಾ(0);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(0);
    }
}

ಶೂನ್ಯ print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb_t *tcb)
{
    ಶಬ್ದ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
    ಬರೆ("ipc_buffer_slot: ");
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, ಕಂಡುಹಿಡಿ_ret.pd);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
        vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap);
        ಆದರೆ (asid) {
            ಬರೆ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr), PD_ಸೂಚ್ಯಂಕ(vptr), (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pt_%p_%04lu\n", lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr), PD_ಸೂಚ್ಯಂಕ(vptr));
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap: {
        asid_t asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap);
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
        ಆದರೆ (asid) {
            ಬರೆ("%p_pd (asid: %lu)\n",
                   ಕಂಡುಹಿಡಿ_ret.pd, (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("%p_pd\n", ಕಂಡುಹಿಡಿ_ret.pd);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_control_cap: {
        /* only one in the system */
        ಬರೆ("asid_control\n");
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap: {
        vptr_t vptr = cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap));
        ಪ್ರತಿಜ್ಞಾ(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE);
        cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, ಕಂಡುಹಿಡಿ_ret.pd);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap: {
        vptr_t vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap));
        ಪ್ರತಿಜ್ಞಾ(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE);
        cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, ಕಂಡುಹಿಡಿ_ret.pd);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap: {
        ಬರೆ("%p_vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ

        /* ARM specific caps */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap: {
        ಬರೆ("%p_io_space\n", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown cap %u]\n", cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap: {
        ಬರೆ("%p_vcpu = vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
        /* ARM specific objects */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap: {
        ಬರೆ("%p_io_space = io_space ", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown object %u]\n", cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(resolve_ret_t ret)
{
    ಬರೆ("(");

    /* VM size */
    ಆಯ್ಕೆ (ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ ARMSection:
        ಬರೆ("2M");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMSuperSection:
        ಬರೆ("32M");
        ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿತಿ ARMSection:
        ಬರೆ("1M");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMSuperSection:
        ಬರೆ("16M");
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ ARMLarge_ಪುಟ:
        ಬರೆ("64k");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ:
        ಬರೆ("4k");
        ನಿಲ್ಲಿಸು;
    }

    ಬರೆ(", paddr: %p)\n", (ಶೂನ್ಯ *)ret.ಚೌಕಟ್ಟು_ತಳ);
}

ಸ್ಥಿರ ಶೂನ್ಯ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    ಶಬ್ದ_t i = 0;
    ಆಗ (i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ಆಯ್ಕೆ (pte_ptr_get_pteType(pte)) {
        ಸ್ಥಿತಿ pte_pte_small: {
            ret.ಚೌಕಟ್ಟು_ತಳ = pte_pte_small_ptr_get_ವಿಳಾಸ(pte);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            ಆದರೆ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.ಚೌಕಟ್ಟು_ತಳ &= ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMLarge_ಪುಟ));
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSmall_ಪುಟ;
            }
#ಇಲ್ಲದಿದ್ದರೆ
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSmall_ಪುಟ;
#ಆದರೆ_ಅಂತ್ಯ
            ಬರೆ("frame_%p_%04lu = frame ", pte, PT_ಸೂಚ್ಯಂಕ(i));
            obj_ಚೌಕಟ್ಟು_print_attrs(ret);
            ನಿಲ್ಲಿಸು;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಸ್ಥಿತಿ pte_pte_large: {
            ret.ಚೌಕಟ್ಟು_ತಳ = pte_pte_large_ptr_get_ವಿಳಾಸ(pte);
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            ಬರೆ("frame_%p_%04lu = frame ", pte, PT_ಸೂಚ್ಯಂಕ(i));
            obj_ಚೌಕಟ್ಟು_print_attrs(ret);
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSmall_ಪುಟ;
        }
        i += (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ));
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_ದ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        ಬರೆ("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ಶಬ್ದ_t i = 0;
        ಆಗ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ಆಯ್ಕೆ (pde_ptr_get_pdeType(pde)) {
            ಸ್ಥಿತಿ pde_pde_section: {
                ret.ಚೌಕಟ್ಟು_ತಳ = pde_pde_section_ptr_get_ವಿಳಾಸ(pde);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                ಆದರೆ (pde_pde_section_ptr_get_ಗಾತ್ರ(pde)) {
                    ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSuperSection;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
                }
#ಇಲ್ಲದಿದ್ದರೆ
                ಆದರೆ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.ಚೌಕಟ್ಟು_ತಳ &= ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSuperSection));
                    ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSuperSection;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಬರೆ("frame_%p_%04lu = frame ", pde, PD_ಸೂಚ್ಯಂಕ(i));
                obj_ಚೌಕಟ್ಟು_print_attrs(ret);
                ನಿಲ್ಲಿಸು;
            }

            ಸ್ಥಿತಿ pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pde));
                ಬರೆ("pt_%p_%04lu = pt\n", pde, PD_ಸೂಚ್ಯಂಕ(i));
                arm32_obj_pt_print_slots(pt);
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
                ನಿಲ್ಲಿಸು;
            }
            ಪೂರ್ವನಿಯೋಜಿತ:
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
                ನಿಲ್ಲಿಸು;
            }
            i += (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ));
            ಆದರೆ (i < (1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ))) {
                ನಿಲ್ಲಿಸು; /* overflowed */
            }
        }
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
{
    ಬರೆ("arch aarch32\n");
    ಬರೆ("objects {\n");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    print_objects();
#ಆದರೆ_ಅಂತ್ಯ
    ಬರೆ("}\n");

    ಬರೆ("caps {\n");

    /* reset the seen list */
    reset_ನೋಡಿದ_list();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    print_caps();
    ಬರೆ("}\n");

    obj_irq_print_maps();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */
}


#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
