/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
پورته_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[بټ(PD_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_Dir_بټونه));
pte_t ia32KSGlobalPT[بټ(PT_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_جدول_بټونه));

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
pte_t ia32KSGlobalLogPT[بټ(PT_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_جدول_بټونه));
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
