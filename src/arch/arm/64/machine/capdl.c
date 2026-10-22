/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>

ಶಬ್ದ_t get_tcb_sp(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ, paddr_t ಚೌಕಟ್ಟು_ತಳ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t SH, ಶಬ್ದ_t AP, ಶಬ್ದ_t NXN);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, cap_t vspace);

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(ಶಬ್ದ_t vptr, vspace_ಬೇರು_t *vspace_ಬೇರು_2);

ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pt_print_slots(pte_t *pd_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pd_print_slots(pte_t *pud_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pud_print_slots(ಶೂನ್ಯ *pgd_ಸ್ಥಾನ_or_vspace);

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pt_print_slots(pte_t *pd_ಸ್ಥಾನ, vptr_t vptr);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pd_print_slots(pte_t *pud_ಸ್ಥಾನ, vptr_t vptr);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pud_print_slots(ಶೂನ್ಯ *pgd_ಸ್ಥಾನ_or_vspace, vptr_t vptr);

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */
/* use when only have access to pte of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ)
{
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pte_pte_ಪುಟ_ptr_get_SH(pt_ಸ್ಥಾನ),
                               pte_pte_ಪುಟ_ptr_get_AP(pt_ಸ್ಥಾನ),
                               pte_pte_ಪುಟ_ptr_get_UXN(pt_ಸ್ಥಾನ));
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t SH, ಶಬ್ದ_t AP, ಶಬ್ದ_t NXN)
{
    ಬರೆ("(");

    /* rights */
    ಆಯ್ಕೆ (AP) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ 0b00:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b01:
        ಬರೆ("R");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b10:
        ಬರೆ("W");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b11:
        ಬರೆ("RW");
        ನಿಲ್ಲಿಸು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿತಿ 0b00:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b01:
        ಬರೆ("RW");
    ಸ್ಥಿತಿ 0b10:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0b11:
        ಬರೆ("R");
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!NXN) {
        ಬರೆ("X");
    }

    /* Only has effect if SMP enabled */
    ಆದರೆ (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        ಬರೆ(", uncached");
    }

    ಬರೆ(")\n");
}

/* use when only have access to vptr of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(ಶಬ್ದ_t vptr, vspace_ಬೇರು_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    ಆದರೆ (!pte_ptr_get_valid(ret.pt_ಸ್ಥಾನ) ||
        (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(ret.pt_ಸ್ಥಾನ) && ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ > ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ಪ್ರತಿಜ್ಞಾ(0);
    }

    ಶಬ್ದ_t ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ;
    ಆಯ್ಕೆ (ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) {

    ಸ್ಥಿತಿ ARMHuge_ಪುಟ:
        ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ = GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMLarge_ಪುಟ:
        ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ = GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(2));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ:
        ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ = GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(3));
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(0);

    }
    ಬರೆ("frame_%p_%04lu ", ret.pt_ಸ್ಥಾನ, ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ);
    cap_ಚೌಕಟ್ಟು_print_attrs_pt(ret.pt_ಸ್ಥಾನ);
}

ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, cap_t vspace)
{
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_ದ_cap(vspace)));
}

/*
 * print object slots
 */
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pt_print_slots(pte_t *pd_ಸ್ಥಾನ, vptr_t vptr)
{
    pte_t *pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ));
    ಬರೆ("pt_%p_%04lu {\n", pd_ಸ್ಥಾನ, GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i ++) {
        pte_t *pt_ಸ್ಥಾನ = pt + i;

        ಆದರೆ (pte_4k_ಪುಟ_ptr_get_present(pt_ಸ್ಥಾನ)) {
            // print pte entries
            ಬರೆ("0x%lx: frame_%p_%04lu", i, pt_ಸ್ಥಾನ, i);
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pt_ಸ್ಥಾನ);
        }
    }
    ಬರೆ("}\n"); /* pt */
}

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pd_print_slots(pte_t *pud_ಸ್ಥಾನ, vptr_t vptr)
{
    ಬರೆ("pd_%p_%04lu {\n", pud_ಸ್ಥಾನ, GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pud_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pd_ಸ್ಥಾನ = pd + i;

        ಆಯ್ಕೆ (pte_ptr_get_pte_type(pd_ಸ್ಥಾನ)) {

        ಸ್ಥಿತಿ pte_pte_ಪುಟ:
            ಬರೆ("0x%lx: frame_%p_%04lu", i, pd_ಸ್ಥಾನ, i);
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pd_ಸ್ಥಾನ);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ pte_pte_ಕೋಷ್ಟಕ:
            ಬರೆ("0x%lx: pt_%p_%04lu\n", i, pd_ಸ್ಥಾನ, i);
            ನಿಲ್ಲಿಸು;
        }
    }

    ಬರೆ("}\n"); /* pd */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pd_ಸ್ಥಾನ = pd + i;
        ಆದರೆ (pte_ptr_get_pte_type(pd_ಸ್ಥಾನ) == pte_pte_ಕೋಷ್ಟಕ) {
            arm64_cap_pt_print_slots(pd_ಸ್ಥಾನ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pud_print_slots(ಶೂನ್ಯ *pgd_ಸ್ಥಾನ_or_vspace, vptr_t vptr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    pte_t *pud = pgd_ಸ್ಥಾನ_or_vspace;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = se_L4_VSpace_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
    ಬರೆ("%p_pd {\n", pgd_ಸ್ಥಾನ_or_vspace);
#ಇಲ್ಲದಿದ್ದರೆ
    pte_t *pud = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pgd_ಸ್ಥಾನ_or_vspace));
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
    ಬರೆ("pud_%p_%04lu {\n", pgd_ಸ್ಥಾನ_or_vspace, GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pud_ಸ್ಥಾನ = pud + i;
        ಆದರೆ (pte_ptr_get_pte_type(pud_ಸ್ಥಾನ) == pte_pte_ಕೋಷ್ಟಕ) {
            ಬರೆ("0x%lx: pd_%p_%04lu\n", i, pud_ಸ್ಥಾನ, i);
        }
    }

    ಬರೆ("}\n"); /* pgd/pud */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pud_ಸ್ಥಾನ = pud + GET_UPT_ಸೂಚ್ಯಂಕ(i, ULVL_FRM_ARM_PT_LVL(1));
        ಆದರೆ (pte_ptr_get_pte_type(pud_ಸ್ಥಾನ) == pte_pte_ಕೋಷ್ಟಕ) {
            arm64_cap_pd_print_slots(pud_ಸ್ಥಾನ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಆದರೆ (is_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ಬೇರು_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
        arm64_cap_pud_print_slots(vspace, 0);
#ಇಲ್ಲದಿದ್ದರೆ
        ಬರೆ("%p_pd {\n", vspace);
        ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು; i++) {
            pte_t *pt_ಸ್ಥಾನ = vspace + i;
            ಆದರೆ (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pt_ಸ್ಥಾನ)) {
                ಬರೆ("0x%lx: pud_%p_%04lu\n", i, pt_ಸ್ಥಾನ, i);
            }
        }
        ಬರೆ("}\n"); /* pd */

        ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು; i++) {
            pte_t *pt_ಸ್ಥಾನ = vspace + i;
            ಆದರೆ (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pt_ಸ್ಥಾನ)) {
                arm64_cap_pud_print_slots(pt_ಸ್ಥಾನ, i * GET_ULVL_PGSIZE(0));
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
    }
}

ಶೂನ್ಯ print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb_t *tcb)
{
    ಶಬ್ದ_t vptr = tcb->tcbIPCBuffer;
    ಬರೆ("ipc_buffer_slot: ");
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap);
        pte_t *ಗುರಿ_pt = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));

        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        pte_t *pt_ಸ್ಥಾನ = ಖಾಲಿ;
        pte_t *pt = (pte_t *)ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು;
        ಶಬ್ದ_t level;
        ಗಾಗಿ (level = 0; level < UPT_ಮಟ್ಟಗಳು - 1 && pt != ಗುರಿ_pt; level++) {
            pt_ಸ್ಥಾನ = pt + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, level);
            ಆದರೆ (unlikely(!pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pt_ಸ್ಥಾನ))) {
                /* couldn't find it */
                ನಿಲ್ಲಿಸು;
            }
            pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pt_ಸ್ಥಾನ));
        }
        ಆದರೆ (pt != ಗುರಿ_pt) {
            /* didn't find it */
            ನಿಲ್ಲಿಸು;
        }


        ಆದರೆ (asid) {
            ಬರೆ("pt_%p_%04lu (asid: %lu)\n",
                   ಗುರಿ_pt, GET_UPT_ಸೂಚ್ಯಂಕ(vptr, level), (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pt_%p_%04lu\n", ಗುರಿ_pt, GET_UPT_ಸೂಚ್ಯಂಕ(vptr, level));
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (asid) {
            ಬರೆ("%p_pd (asid: %lu)\n",
                   ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("%p_pd\n", ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_control_cap: {
        /* only one in the system */
        ಬರೆ("asid_control\n");
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap: {
        vptr_t vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap));
        ಪ್ರತಿಜ್ಞಾ(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE);
        cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(vptr, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
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
        ಬರೆ("[unknown cap %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
    ಸ್ಥಿತಿ cap_vspace_cap:
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
        ಬರೆ("[unknown object %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ, paddr_t ಚೌಕಟ್ಟು_ತಳ)
{
    ಬರೆ("(");

    /* VM size */
    ಆಯ್ಕೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ ARMHuge_ಪುಟ:
        ಬರೆ("1G");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMLarge_ಪುಟ:
        ಬರೆ("2M");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ:
        ಬರೆ("4k");
        ನಿಲ್ಲಿಸು;
    }

    ಬರೆ(", paddr: 0x%p)\n", (ಶೂನ್ಯ *)ಚೌಕಟ್ಟು_ತಳ);
}

ಶೂನ್ಯ arm64_obj_pt_print_slots(pte_t *pd_ಸ್ಥಾನ)
{
    pte_t *pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pt_ಸ್ಥಾನ = pt + i;

        ಆದರೆ (pte_4k_ಪುಟ_ptr_get_present(pt_ಸ್ಥಾನ)) {
            ಬರೆ("frame_%p_%04lu = frame ", pt_ಸ್ಥಾನ, i);
            obj_ಚೌಕಟ್ಟು_print_attrs(ARMSmall_ಪುಟ, pte_ಪುಟ_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pt_ಸ್ಥಾನ));
        }
    }
}

ಶೂನ್ಯ arm64_obj_pd_print_slots(pte_t *pud_ಸ್ಥಾನ)
{
    pte_t *pd = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pud_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pd_ಸ್ಥಾನ = pd + i;

        ಆದರೆ (pte_ptr_get_pte_type(pd_ಸ್ಥಾನ) == pte_pte_ಪುಟ) {
            ಬರೆ("frame_%p_%04lu = frame ", pd_ಸ್ಥಾನ, i);
            obj_ಚೌಕಟ್ಟು_print_attrs(ARMLarge_ಪುಟ, pte_ಪುಟ_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ));
        }

        ಆದರೆ (pte_ptr_get_pte_type(pd_ಸ್ಥಾನ) == pte_pte_ಕೋಷ್ಟಕ) {
            ಬರೆ("pt_%p_%04lu = pt\n", pd_ಸ್ಥಾನ, i);
            arm64_obj_pt_print_slots(pd_ಸ್ಥಾನ);
        }
    }
}

ಶೂನ್ಯ arm64_obj_pud_print_slots(ಶೂನ್ಯ *pgd_ಸ್ಥಾನ_or_vspace)
{
    pte_t *pud = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pgd_ಸ್ಥಾನ_or_vspace));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = se_L4_VSpace_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
#ಇಲ್ಲದಿದ್ದರೆ
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        pte_t *pud_ಸ್ಥಾನ = pud + i;

        ಆಯ್ಕೆ (pte_ptr_get_pte_type(pud_ಸ್ಥಾನ)) {
        ಸ್ಥಿತಿ pte_pte_ಪುಟ:
            ಬರೆ("frame_%p_%04lu = frame ", pud_ಸ್ಥಾನ, i);
            obj_ಚೌಕಟ್ಟು_print_attrs(ARMHuge_ಪುಟ, pte_ಪುಟ_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pud_ಸ್ಥಾನ));
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ pte_pte_ಕೋಷ್ಟಕ: {
            ಬರೆ("pd_%p_%04lu = pd\n", pud_ಸ್ಥಾನ, i);
            arm64_obj_pd_print_slots(pud_ಸ್ಥಾನ);

        }
        }
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಆದರೆ (is_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ಬೇರು_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
        ಬರೆ("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#ಇಲ್ಲದಿದ್ದರೆ
        ಬರೆ("%p_pd = pgd\n", vspace);
        ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು; i++) {
            pte_t *pt_ಸ್ಥಾನ = vspace + i;
            ಆದರೆ (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pt_ಸ್ಥಾನ)) {
                ಬರೆ("pud_%p_%04lu = pud\n", pt_ಸ್ಥಾನ, i);
                arm64_obj_pud_print_slots(pt_ಸ್ಥಾನ);
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
{
    ಬರೆ("arch aarch64\n");
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
