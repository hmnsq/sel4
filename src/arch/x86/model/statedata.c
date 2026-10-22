/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <arch/types.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
ऊर्ध्वम्_STATE_DEFINE(व्यवधानम्_t, x86_KScur_व्यवधानम् VISIBLE);

ऊर्ध्वम्_STATE_DEFINE(व्यवधानम्_t, x86_KSPending_व्यवधानम्);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_अङ्कः_NODES] ALIGN(L1_CACHE_पङ्क्तिः_आकार) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
ऊर्ध्वम्_STATE_DEFINE(word_t, x86KSCurrentFSBase);
ऊर्ध्वम्_STATE_DEFINE(word_t, x86KSCurrentGSBase);

ऊर्ध्वम्_STATE_DEFINE(word_t, x86_KSGPExcept_Return_गन्तव्यम्);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_पङ्क्तिः_आकार_द्वयङ्काः;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#यदि_परिभाषितम् CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_मूलम्_सारणी;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_VTX
ऊर्ध्वम्_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
ऊर्ध्वम्_STATE_DEFINE(संरचना vmxon_region, x86KSVMXOnRegion);
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_PRINTING
uint16_t x86_KSconsole_पत्तनम्;
#यदि_अन्तः
#यदि defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_पत्तनम्;
#यदि_अन्तः

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

word_t x86KSAllocatedIOPorts[अङ्कः_IO_PORTS / CONFIG_WORD_आकार];
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_अनुपातः;
#यदि_अन्तः
