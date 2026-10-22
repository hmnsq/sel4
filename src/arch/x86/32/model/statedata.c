/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
اوپر_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));
pte_t ia32KSGlobalPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
pte_t ia32KSGlobalLogPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
