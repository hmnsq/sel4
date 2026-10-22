/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
ऊर्ध्वम्_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_Dir_द्वयङ्काः));
pte_t ia32KSGlobalPT[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_सारणी_द्वयङ्काः));

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
pte_t ia32KSGlobalLogPT[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_सारणी_द्वयङ्काः));
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
