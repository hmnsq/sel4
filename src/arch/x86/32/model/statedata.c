/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
ਉੱਪਰ_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));
pte_t ia32KSGlobalPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
pte_t ia32KSGlobalLogPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
