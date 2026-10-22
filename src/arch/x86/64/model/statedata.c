/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[بیت(PML4_شاخص_بیتها)] ALIGN(بیت(se_L4_PML4_بیتها)) VISIBLE;
pdpte_t x64KSKernelPDPT[بیت(PDPT_شاخص_بیتها)] ALIGN(بیت(seL4_PDPTBits));
#اگر_تعریف_شده CONFIG_HUGE_صفحه
pde_t x64KSKernelPD[بیت(PD_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_Dir_بیتها));
#وإلا
pde_t x64KSKernelPDs[بیت(PDPT_شاخص_بیتها)][بیت(PD_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_Dir_بیتها));
#پایان_اگر
pte_t x64KSKernelPT[بیت(PT_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_جدول_بیتها));

#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
pml4e_t x64KSSKIMPML4[بیت(PML4_شاخص_بیتها)] ALIGN(بیت(se_L4_PML4_بیتها));
pdpte_t x64KSSKIMPDPT[بیت(PDPT_شاخص_بیتها)] ALIGN(بیت(seL4_PDPTBits));
pde_t x64KSSKIMPD[بیت(PD_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_Dir_بیتها));
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
به_بالا_STATE_DEFINE(کلمه_t, x64KSCurrentUserCR3);
#وإلا
به_بالا_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#پایان_اگر

کلمه_t x64KSIRQStack[CONFIG_MAX_رقم_NODES][IRQ_پشته_اندازه + 2] ALIGN(64) VISIBLE SKIM_BSS;
