/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <util.h>
#شامل <api/types.h>
#شامل <arch/types.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
اوپر_STATE_DEFINE(مداخلت_t, x86_KScur_مداخلت VISIBLE);

اوپر_STATE_DEFINE(مداخلت_t, x86_KSPending_مداخلت);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_عدد_NODES] ALIGN(L1_CACHE_سطر_سائز) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
اوپر_STATE_DEFINE(لفظ_t, x86KSCurrentFSBase);
اوپر_STATE_DEFINE(لفظ_t, x86KSCurrentGSBase);

اوپر_STATE_DEFINE(لفظ_t, x86_KSGPExcept_Return_مقصد);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_سطر_سائز_بٹ;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#اگر_تعریف_شدہ CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_جڑ_جدول;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_VTX
اوپر_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
اوپر_STATE_DEFINE(ساخت vmxon_region, x86KSVMXOnRegion);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_PRINTING
uint16_t x86_KSconsole_پورٹ;
#اگر_ختم
#اگر defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_پورٹ;
#اگر_ختم

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

لفظ_t x86KSAllocatedIOPorts[عدد_IO_PORTS / CONFIG_لفظ_سائز];
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_تناسب;
#اگر_ختم
