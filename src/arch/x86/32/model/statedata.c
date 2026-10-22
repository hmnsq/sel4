/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
ಮೇಲೆ_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));
pte_t ia32KSGlobalPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
pte_t ia32KSGlobalLogPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
