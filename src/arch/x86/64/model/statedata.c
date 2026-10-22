/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[ثنائی_ہندسہ(PML4_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_PML4_بٹ)) VISIBLE;
pdpte_t x64KSKernelPDPT[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(seL4_PDPTBits));
#جے_تعریف_شدہ CONFIG_HUGE_ورقہ
pde_t x64KSKernelPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_Dir_بٹ));
#وإلا
pde_t x64KSKernelPDs[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)][ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_Dir_بٹ));
#جے_انت
pte_t x64KSKernelPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ));

#جے_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
pml4e_t x64KSSKIMPML4[ثنائی_ہندسہ(PML4_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_PML4_بٹ));
pdpte_t x64KSSKIMPDPT[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(seL4_PDPTBits));
pde_t x64KSSKIMPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_Dir_بٹ));
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
اتے_STATE_DEFINE(لفظ_t, x64KSCurrentUserCR3);
#وإلا
اتے_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#جے_انت

لفظ_t x64KSIRQStack[CONFIG_MAX_عدد_NODES][IRQ_ڈھیر_ناپ + 2] ALIGN(64) VISIBLE SKIM_BSS;
