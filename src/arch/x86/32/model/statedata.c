/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
مٿي_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));
pte_t ia32KSGlobalPT[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
pte_t ia32KSGlobalLogPT[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
