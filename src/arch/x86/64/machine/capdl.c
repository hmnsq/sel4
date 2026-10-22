/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[RSP];
}

#جے_تعریف_شدہ CONFIG_PRINTING

ساكن فراغ obj_چوکھٹا_print_attrs(paddr_t paddr, لفظ_t ورقہ_ناپ);
ساكن فراغ cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace);
ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace);
ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ cap_چوکھٹا_print_attrs_pdpt(pdpte_t *pdpt_تھاں);
ساكن فراغ cap_چوکھٹا_print_attrs_pd(pde_t *pd_تھاں);
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں);
ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd);

/* use when only have access to pte of frames */
ساكن فراغ cap_چوکھٹا_print_attrs_pdpt(pdpte_t *pdpt_تھاں)
{
    cap_چوکھٹا_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_تھاں),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_تھاں),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_تھاں),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_تھاں));
}
ساكن فراغ cap_چوکھٹا_print_attrs_pd(pde_t *pd_تھاں)
{
    cap_چوکھٹا_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_تھاں),
                               pde_pde_large_ptr_get_read_write(pd_تھاں),
                               pde_pde_large_ptr_get_cache_disabled(pd_تھاں),
                               pde_pde_large_ptr_get_xd(pd_تھاں));
}

ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں)
{
    cap_چوکھٹا_print_attrs_impl(pte_ptr_get_super_user(pt_تھاں),
                               pte_ptr_get_read_write(pt_تھاں),
                               pte_ptr_get_cache_disabled(pt_تھاں),
                               pte_ptr_get_xd(pt_تھاں));
}

ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd)
{
    لکھو("(");

    /* rights */
    إذا (read_write) {
        لکھو("RW");
    } وإلا إذا (super_user) {
        لکھو("R");
    }

    إذا (!xd) {
        لکھو("X");
    }

    /* asid, mapping */

    إذا (cache_disabled) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

ساكن فراغ obj_چوکھٹا_print_attrs(paddr_t paddr, لفظ_t ورقہ_ناپ)
{
    لکھو("(");

    /* VM size */
    بدل (ورقہ_ناپ) {
    حالة se_L4_Huge_ورقہ_بٹ:
        لکھو("1G");
        اكسر;
    حالة se_L4_Large_ورقہ_بٹ:
        لکھو("2M");
        اكسر;
    حالة se_L4_ورقہ_بٹ:
        لکھو("4k");
        اكسر;
    }

    لکھو(", paddr: 0x%p)\n", (فراغ *)paddr);
}

ساكن فراغ x86_64_obj_pt_print_slots(pde_t *pd_تھاں)
{
    paddr_t paddr;
    لفظ_t ورقہ_ناپ;
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_ادھار_پتہ(pd_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_وتھ + PT_اشاریہ_بٹ); i += (1UL << PT_اشاریہ_وتھ)) {
        pte_t *pt_تھاں = pt + GET_PT_اشاریہ(i);

        إذا (pte_ptr_get_present(pt_تھاں)) {
            paddr = pte_ptr_get_ورقہ_ادھار_پتہ(pt_تھاں);
            ورقہ_ناپ = se_L4_ورقہ_بٹ;
            لکھو("frame_%p_%04lu = frame ", pt_تھاں, GET_PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, ورقہ_ناپ);
        }
    }
}

ساكن فراغ x86_64_obj_pd_print_slots(pdpte_t *pdpt_تھاں)
{
    paddr_t paddr;
    لفظ_t ورقہ_ناپ;
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_ادھار_پتہ(pdpt_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_وتھ + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_وتھ)) {
        pde_t *pd_تھاں = pd + GET_PD_اشاریہ(i);

        إذا ((pde_ptr_get_ورقہ_ناپ(pd_تھاں) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_تھاں)) {
            paddr = pde_pde_large_ptr_get_ورقہ_ادھار_پتہ(pd_تھاں);
            ورقہ_ناپ = se_L4_Large_ورقہ_بٹ;

            لکھو("frame_%p_%04lu = frame ", pd_تھاں, GET_PD_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, ورقہ_ناپ);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_تھاں)) {
            لکھو("pt_%p_%04lu = pt\n", pd_تھاں, GET_PD_اشاریہ(i));
            x86_64_obj_pt_print_slots(pd_تھاں);
        }
    }
}

ساكن فراغ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_تھاں)
{
    paddr_t paddr;
    لفظ_t ورقہ_ناپ;
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_ادھار_پتہ(pml4_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_وتھ + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_وتھ)) {
        pdpte_t *pdpt_تھاں = pdpt + GET_PDPT_اشاریہ(i);

        إذا (pdpte_ptr_get_ورقہ_ناپ(pdpt_تھاں) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_تھاں)) {
            paddr = pdpte_pdpte_1g_ptr_get_ورقہ_ادھار_پتہ(pdpt_تھاں);
            ورقہ_ناپ = se_L4_Huge_ورقہ_بٹ;

            لکھو("frame_%p_%04lu = frame ", pdpt_تھاں, GET_PDPT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, ورقہ_ناپ);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_تھاں)) {
            لکھو("pd_%p_%04lu = pd\n", pdpt_تھاں, GET_PDPT_اشاریہ(i));
            x86_64_obj_pd_print_slots(pdpt_تھاں);
        }
    }
}

ساكن فراغ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_وتھ + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_وتھ)) {
        pml4e_t *pml4_تھاں = lookup_PML4_تھاں(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_تھاں)) {
            لکھو("pdpt_%p_%04lu = pdpt\n", pml4_تھاں, GET_PML4_اشاریہ(i));
            x86_64_obj_pdpt_print_slots(pml4_تھاں);
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ادھار_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        لکھو("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_تھاں = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_تھاں;
    lookupPDSlot_ret_t pd_تھاں;

    إذا (pdpt_تھاں.حالت == EXCEPTION_NONE &&
        pdpte_ptr_get_ورقہ_ناپ(pdpt_تھاں.pdpt_تھاں) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_تھاں.pdpt_تھاں)) {
        لکھو("frame_%p_%04lu ", pdpt_تھاں.pdpt_تھاں, GET_PDPT_اشاریہ(vptr));
        cap_چوکھٹا_print_attrs_pdpt(pdpt_تھاں.pdpt_تھاں);
    } وإلا {
        pd_تھاں = lookupPDSlot(vspace, vptr);
        إذا (pd_تھاں.حالت == EXCEPTION_NONE &&
            ((pde_ptr_get_ورقہ_ناپ(pd_تھاں.pd_تھاں) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_تھاں.pd_تھاں))) {
            لکھو("frame_%p_%04lu ", pd_تھاں.pd_تھاں, GET_PD_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pd(pd_تھاں.pd_تھاں);
        } وإلا {
            pt_تھاں = lookupPTSlot(vspace, vptr);
            أكد(pt_تھاں.حالت == EXCEPTION_NONE && pte_ptr_get_present(pt_تھاں.pt_تھاں));
            لکھو("frame_%p_%04lu ", pt_تھاں.pt_تھاں, GET_PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pt_تھاں.pt_تھاں);
        }
    }
}

ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
    cap_چوکھٹا_print_attrs_vptr_2(vptr, لبھو_ret.vspace_جڑ);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific caps */
    حالة cap_ورقہ_جدول_cap: {
        asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        إذا (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(لبھو_ret.vspace_جڑ, vptr).pd_تھاں, GET_PD_اشاریہ(vptr), (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pt_%p_%04lu\n", lookupPDSlot(لبھو_ret.vspace_جڑ, vptr).pd_تھاں, GET_PD_اشاریہ(vptr));
        }
        اكسر;
    }
    حالة cap_ورقہ_directory_cap: {
        asid_t asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ورقہ_directory_cap_get_cap_PDMapped_پتہ(cap);
        إذا (asid) {
            لکھو("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(لبھو_ret.vspace_جڑ, vptr).pdpt_تھاں, GET_PDPT_اشاریہ(vptr), (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pd_%p_%04lu\n", lookupPDPTSlot(لبھو_ret.vspace_جڑ, vptr).pdpt_تھاں, GET_PDPT_اشاریہ(vptr));
        }
        اكسر;
    }
    حالة cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پتہ(cap);
        إذا (asid) {
            لکھو("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_تھاں(لبھو_ret.vspace_جڑ, vptr), GET_PML4_اشاریہ(vptr), (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pdpt_%p_%04lu\n", lookup_PML4_تھاں(لبھو_ret.vspace_جڑ, vptr), GET_PML4_اشاریہ(vptr));
        }
        اكسر;
    }
    حالة cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (asid) {
            لکھو("%p_pd (asid: %lu)\n",
                   لبھو_ret.vspace_جڑ, (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("%p_pd\n", لبھو_ret.vspace_جڑ);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        اكسر;
    }
    حالة cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(لبھو_ret.حالت == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr_2(vptr, لبھو_ret.vspace_جڑ);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت
    /* X86 specific caps */
    حالة cap_io_پورٹ_cap: {
        لکھو("%p%p_io_port\n", (فراغ *)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
               (فراغ *)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        اكسر;
    }
    حالة cap_io_ورقہ_جدول_cap: {
        لکھو("%p_iopt\n", (فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown cap %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکھو("(asid_high: 0x%lx)\n", ASID_بلند(asid));
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific objects */
    حالة cap_چوکھٹا_cap:
    حالة cap_ورقہ_جدول_cap:
    حالة cap_ورقہ_directory_cap:
    حالة cap_pdpt_cap:
    حالة cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت
    /* X86 specific caps */
    حالة cap_io_پورٹ_cap: {
        لکھو("%p%p_io_port = io_port ",
               (فراغ *)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
               (فراغ *)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
        x86_obj_ioports_print_attrs(cap);
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        اكسر;
    }
    حالة cap_io_ورقہ_جدول_cap: {
        لکھو("%p_iopt = iopt ", (فراغ *)cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown object %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_ipc_بفر_تھاں(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ساكن فراغ x86_64_cap_pt_print_slots(pde_t *pd_تھاں, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_ادھار_پتہ(pd_تھاں));
    لکھو("pt_%p_%04lu {\n", pd_تھاں, GET_PD_اشاریہ(vptr));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_وتھ + PT_اشاریہ_بٹ); i += (1UL << PT_اشاریہ_وتھ)) {
        pte_t *pt_تھاں = pt + GET_PT_اشاریہ(i);

        إذا (pte_ptr_get_present(pt_تھاں)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PT_اشاریہ(i), pt_تھاں, GET_PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pt_تھاں);
        }
    }
    لکھو("}\n"); /* pt */
}

ساكن فراغ x86_64_cap_pd_print_slots(pdpte_t *pdpt_تھاں, vptr_t vptr)
{
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_ادھار_پتہ(pdpt_تھاں));
    لکھو("pd_%p_%04lu {\n", pdpt_تھاں, GET_PDPT_اشاریہ(vptr));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_وتھ + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_وتھ)) {
        pde_t *pd_تھاں = pd + GET_PD_اشاریہ(i);

        إذا ((pde_ptr_get_ورقہ_ناپ(pd_تھاں) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_تھاں)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PD_اشاریہ(i), pd_تھاں, GET_PD_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pd(pd_تھاں);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_تھاں)) {
            لکھو("0x%lx: pt_%p_%04lu\n", GET_PD_اشاریہ(i), pd_تھاں, GET_PD_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pd */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_وتھ + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_وتھ)) {
        pde_t *pd_تھاں = pd + GET_PD_اشاریہ(i);
        إذا ((pde_ptr_get_ورقہ_ناپ(pd_تھاں) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_تھاں)) {
            x86_64_cap_pt_print_slots(pd_تھاں, i);
        }
    }
}

ساكن فراغ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_تھاں, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_ادھار_پتہ(pml4_تھاں));

    لکھو("pdpt_%p_%04lu {\n", pml4_تھاں, GET_PML4_اشاریہ(vptr));
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_وتھ + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_وتھ)) {
        pdpte_t *pdpt_تھاں = pdpt + GET_PDPT_اشاریہ(i);

        إذا (pdpte_ptr_get_ورقہ_ناپ(pdpt_تھاں) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_تھاں)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PDPT_اشاریہ(i), pdpt_تھاں, GET_PDPT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pdpt(pdpt_تھاں);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_تھاں)) {
            لکھو("0x%lx: pd_%p_%04lu\n", GET_PDPT_اشاریہ(i), pdpt_تھاں, GET_PDPT_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pdpt */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_وتھ + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_وتھ)) {
        pdpte_t *pdpt_تھاں = pdpt + GET_PDPT_اشاریہ(i);

        إذا (pdpte_ptr_get_ورقہ_ناپ(pdpt_تھاں) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_تھاں)) {
            x86_64_cap_pd_print_slots(pdpt_تھاں, i);
        }
    }
}

ساكن فراغ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    لکھو("%p_pd {\n", pml4);
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_وتھ + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_وتھ)) {
        pml4e_t *pml4_تھاں = lookup_PML4_تھاں(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_تھاں)) {
            لکھو("0x%lx: pdpt_%p_%04lu\n", GET_PML4_اشاریہ(i), pml4_تھاں, GET_PML4_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pml4 */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_وتھ + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_وتھ)) {
        pml4e_t *pml4_تھاں = lookup_PML4_تھاں(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_تھاں)) {
            x86_64_cap_pdpt_print_slots(pml4_تھاں, i);
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ادھار_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#جے_انت /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکھو("arch x86_64\n");
    لکھو("objects {\n");
#جے_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#جے_انت
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_تکیا_list();

#جے_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#جے_انت
}

#جے_انت /* CONFIG_DEBUG_BUILD */
