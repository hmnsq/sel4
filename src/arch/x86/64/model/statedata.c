/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[BIT(PML4_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_PML4_द्वयङ्काः)) VISIBLE;
pdpte_t x64KSKernelPDPT[BIT(PDPT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(seL4_PDPTBits));
#यदि_परिभाषितम् CONFIG_HUGE_PAGE
pde_t x64KSKernelPD[BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_Dir_द्वयङ्काः));
#अन्यथा
pde_t x64KSKernelPDs[BIT(PDPT_सूचकांक_द्वयङ्काः)][BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_Dir_द्वयङ्काः));
#यदि_अन्तः
pte_t x64KSKernelPT[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_सारणी_द्वयङ्काः));

#यदि_परिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
pml4e_t x64KSSKIMPML4[BIT(PML4_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_PML4_द्वयङ्काः));
pdpte_t x64KSSKIMPDPT[BIT(PDPT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(seL4_PDPTBits));
pde_t x64KSSKIMPD[BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_Dir_द्वयङ्काः));
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
ऊर्ध्वम्_STATE_DEFINE(word_t, x64KSCurrentUserCR3);
#अन्यथा
ऊर्ध्वम्_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#यदि_अन्तः

word_t x64KSIRQStack[CONFIG_MAX_अङ्कः_NODES][IRQ_राशिः_आकार + 2] ALIGN(64) VISIBLE SKIM_BSS;
