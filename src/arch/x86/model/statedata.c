/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <arch/types.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
ಮೇಲೆ_STATE_DEFINE(ಅಡಚಣೆ_t, x86_KScur_ಅಡಚಣೆ VISIBLE);

ಮೇಲೆ_STATE_DEFINE(ಅಡಚಣೆ_t, x86_KSPending_ಅಡಚಣೆ);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_ಅಂಕೆ_NODES] ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, x86KSCurrentFSBase);
ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, x86KSCurrentGSBase);

ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, x86_KSGPExcept_Return_ಗಮ್ಯ);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_ಬೇರು_ಕೋಷ್ಟಕ;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
ಮೇಲೆ_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
ಮೇಲೆ_STATE_DEFINE(ರಚನೆ vmxon_region, x86KSVMXOnRegion);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
uint16_t x86_KSconsole_ಪೋರ್ಟ್;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_ಪೋರ್ಟ್;
#ಆದರೆ_ಅಂತ್ಯ

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

ಶಬ್ದ_t x86KSAllocatedIOPorts[ಅಂಕೆ_IO_PORTS / CONFIG_ಶಬ್ದ_ಗಾತ್ರ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_ಅನುಪಾತ;
#ಆದರೆ_ಅಂತ್ಯ
