/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));

#ਜੇ __riscv_xlen != 32
pte_t kernel_image_level1_pt[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
pte_t kernel_image_level1_dev_pt[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
#ਜਾਂਜੇ defined(CONFIG_KERNEL_LOG_ਬਫਰ)
pte_t kernel_image_level1_log_ਬਫਰ_pt[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
#ਜੇ_ਅੰਤ

SMP_STATE_DEFINE(core_map_t, coreMap);
