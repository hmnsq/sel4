/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
పైకి_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[బిట్టు(PD_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_Dir_బిట్లు));
pte_t ia32KSGlobalPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
pte_t ia32KSGlobalLogPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
