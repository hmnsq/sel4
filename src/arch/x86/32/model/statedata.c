/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
به_بالا_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[بیت(PD_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_Dir_بیتها));
pte_t ia32KSGlobalPT[بیت(PT_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_جدول_بیتها));

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
pte_t ia32KSGlobalLogPT[بیت(PT_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_جدول_بیتها));
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
