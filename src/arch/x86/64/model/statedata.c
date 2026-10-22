/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_PML4_ಬಿಟ್ಟುಗಳು)) VISIBLE;
pdpte_t x64KSKernelPDPT[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(seL4_PDPTBits));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HUGE_ಪುಟ
pde_t x64KSKernelPD[ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));
#ಇಲ್ಲದಿದ್ದರೆ
pde_t x64KSKernelPDs[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)][ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ
pte_t x64KSKernelPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
pml4e_t x64KSSKIMPML4[ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_PML4_ಬಿಟ್ಟುಗಳು));
pdpte_t x64KSSKIMPDPT[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(seL4_PDPTBits));
pde_t x64KSSKIMPD[ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, x64KSCurrentUserCR3);
#ಇಲ್ಲದಿದ್ದರೆ
ಮೇಲೆ_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#ಆದರೆ_ಅಂತ್ಯ

ಶಬ್ದ_t x64KSIRQStack[CONFIG_MAX_ಅಂಕೆ_NODES][IRQ_ರಾಶಿ_ಗಾತ್ರ + 2] ALIGN(64) VISIBLE SKIM_BSS;
