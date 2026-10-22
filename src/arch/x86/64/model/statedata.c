/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[بټ(PML4_شاخص_بټونه)] ALIGN(بټ(se_L4_PML4_بټونه)) VISIBLE;
pdpte_t x64KSKernelPDPT[بټ(PDPT_شاخص_بټونه)] ALIGN(بټ(seL4_PDPTBits));
#که_تعریف_شوی CONFIG_HUGE_پاڼه
pde_t x64KSKernelPD[بټ(PD_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_Dir_بټونه));
#وإلا
pde_t x64KSKernelPDs[بټ(PDPT_شاخص_بټونه)][بټ(PD_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_Dir_بټونه));
#د_که_پای
pte_t x64KSKernelPT[بټ(PT_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_جدول_بټونه));

#که_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
pml4e_t x64KSSKIMPML4[بټ(PML4_شاخص_بټونه)] ALIGN(بټ(se_L4_PML4_بټونه));
pdpte_t x64KSSKIMPDPT[بټ(PDPT_شاخص_بټونه)] ALIGN(بټ(seL4_PDPTBits));
pde_t x64KSSKIMPD[بټ(PD_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_Dir_بټونه));
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
پورته_STATE_DEFINE(کلمه_t, x64KSCurrentUserCR3);
#وإلا
پورته_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#د_که_پای

کلمه_t x64KSIRQStack[CONFIG_MAX_عدد_NODES][IRQ_ډېری_اندازه + 2] ALIGN(64) VISIBLE SKIM_BSS;
