/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[ٻٽو_انگ(PML4_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_PML4_بٽ)) VISIBLE;
pdpte_t x64KSKernelPDPT[ٻٽو_انگ(PDPT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(seL4_PDPTBits));
#جيڪڏهن_وصف_ٿيل CONFIG_HUGE_صفحو
pde_t x64KSKernelPD[ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));
#وإلا
pde_t x64KSKernelPDs[ٻٽو_انگ(PDPT_اشاريو_بٽ)][ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));
#جيڪڏهن_پورو
pte_t x64KSKernelPT[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_SKIM_دري
pml4e_t x64KSSKIMPML4[ٻٽو_انگ(PML4_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_PML4_بٽ));
pdpte_t x64KSSKIMPDPT[ٻٽو_انگ(PDPT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(seL4_PDPTBits));
pde_t x64KSSKIMPD[ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_SKIM_دري
مٿي_STATE_DEFINE(لفظ_t, x64KSCurrentUserCR3);
#وإلا
مٿي_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#جيڪڏهن_پورو

لفظ_t x64KSIRQStack[CONFIG_MAX_انگ_NODES][IRQ_ڍير_سائيز + 2] ALIGN(64) VISIBLE SKIM_BSS;
