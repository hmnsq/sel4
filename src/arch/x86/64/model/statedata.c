/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[BIT(PML4_सूचकांक_BITS)] ALIGN(BIT(seL4_PML4Bits)) VISIBLE;
pdpte_t x64KSKernelPDPT[BIT(PDPT_सूचकांक_BITS)] ALIGN(BIT(seL4_PDPTBits));
#अगर_घोषित CONFIG_HUGE_PAGE
pde_t x64KSKernelPD[BIT(PD_सूचकांक_BITS)] ALIGN(BIT(seL4_PageDirBits));
#अन्यथा
pde_t x64KSKernelPDs[BIT(PDPT_सूचकांक_BITS)][BIT(PD_सूचकांक_BITS)] ALIGN(BIT(seL4_PageDirBits));
#पूर्ण_अगर
pte_t x64KSKernelPT[BIT(PT_सूचकांक_BITS)] ALIGN(BIT(seL4_PageTableBits));

#अगर_घोषित CONFIG_KERNEL_SKIM_WINDOW
pml4e_t x64KSSKIMPML4[BIT(PML4_सूचकांक_BITS)] ALIGN(BIT(seL4_PML4Bits));
pdpte_t x64KSSKIMPDPT[BIT(PDPT_सूचकांक_BITS)] ALIGN(BIT(seL4_PDPTBits));
pde_t x64KSSKIMPD[BIT(PD_सूचकांक_BITS)] ALIGN(BIT(seL4_PageDirBits));
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_SKIM_WINDOW
UP_STATE_DEFINE(word_t, x64KSCurrentUserCR3);
#अन्यथा
UP_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#पूर्ण_अगर

word_t x64KSIRQStack[CONFIG_MAX_अंक_NODES][IRQ_ढेर_आकार + 2] ALIGN(64) VISIBLE SKIM_BSS;
