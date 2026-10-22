/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <util.h>
#समावेश <api/types.h>
#समावेश <arch/types.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
UP_STATE_DEFINE(interrupt_t, x86KScurInterrupt VISIBLE);

UP_STATE_DEFINE(interrupt_t, x86KSPendingInterrupt);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_अंक_NODES] ALIGN(L1_CACHE_पंक्ति_आकार) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
UP_STATE_DEFINE(word_t, x86KSCurrentFSBase);
UP_STATE_DEFINE(word_t, x86KSCurrentGSBase);

UP_STATE_DEFINE(word_t, x86KSGPExceptReturnTo);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_पंक्ति_आकार_Bits;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#अगर_घोषित CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_मूल_Table;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#पूर्ण_अगर

#अगर_घोषित CONFIG_VTX
UP_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
UP_STATE_DEFINE(काष्ठा vmxon_region, x86KSVMXOnRegion);
#पूर्ण_अगर

#अगर_घोषित CONFIG_PRINTING
uint16_t x86KSconsolePort;
#पूर्ण_अगर
#अगर defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86KSdebugPort;
#पूर्ण_अगर

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

word_t x86KSAllocatedIOPorts[अंक_IO_PORTS / CONFIG_WORD_आकार];
#अगर_घोषित CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86KSapicRatio;
#पूर्ण_अगर
