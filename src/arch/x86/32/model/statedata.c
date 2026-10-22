/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
اتے_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_Dir_بٹ));
pte_t ia32KSGlobalPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ));

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
pte_t ia32KSGlobalLogPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ));
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */
