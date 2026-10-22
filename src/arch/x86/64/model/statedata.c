/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[బిట్టు(PML4_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_PML4_బిట్లు)) VISIBLE;
pdpte_t x64KSKernelPDPT[బిట్టు(PDPT_సూచీ_బిట్లు)] ALIGN(బిట్టు(seL4_PDPTBits));
#నిర్వచితమైతే CONFIG_HUGE_పుట
pde_t x64KSKernelPD[బిట్టు(PD_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_Dir_బిట్లు));
#లేకపోతే
pde_t x64KSKernelPDs[బిట్టు(PDPT_సూచీ_బిట్లు)][బిట్టు(PD_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_Dir_బిట్లు));
#అయితే_ముగింపు
pte_t x64KSKernelPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
pml4e_t x64KSSKIMPML4[బిట్టు(PML4_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_PML4_బిట్లు));
pdpte_t x64KSSKIMPDPT[బిట్టు(PDPT_సూచీ_బిట్లు)] ALIGN(బిట్టు(seL4_PDPTBits));
pde_t x64KSSKIMPD[బిట్టు(PD_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_Dir_బిట్లు));
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
పైకి_STATE_DEFINE(మాట_t, x64KSCurrentUserCR3);
#లేకపోతే
పైకి_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#అయితే_ముగింపు

మాట_t x64KSIRQStack[CONFIG_MAX_అంకె_NODES][IRQ_దొంతర_కొలత + 2] ALIGN(64) VISIBLE SKIM_BSS;
