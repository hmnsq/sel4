/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[ثنائی_ہندسہ(PML4_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_PML4_بٹ)) VISIBLE;
pdpte_t x64KSKernelPDPT[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(seL4_PDPTBits));
#اگر_تعریف_شدہ CONFIG_HUGE_صفحہ
pde_t x64KSKernelPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));
#ورنہ
pde_t x64KSKernelPDs[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)][ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));
#اگر_ختم
pte_t x64KSKernelPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));

#اگر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
pml4e_t x64KSSKIMPML4[ثنائی_ہندسہ(PML4_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_PML4_بٹ));
pdpte_t x64KSSKIMPDPT[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(seL4_PDPTBits));
pde_t x64KSSKIMPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
اوپر_STATE_DEFINE(لفظ_t, x64KSCurrentUserCR3);
#ورنہ
اوپر_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#اگر_ختم

لفظ_t x64KSIRQStack[CONFIG_MAX_عدد_NODES][IRQ_ڈھیر_سائز + 2] ALIGN(64) VISIBLE SKIM_BSS;
