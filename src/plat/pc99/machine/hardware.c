/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <machine/io.h>
#समावेश <arch/kernel/apic.h>
#समावेश <arch/model/statedata.h>
#समावेश <linker.h>
#समावेश <plat/machine/pic.h>
#समावेश <plat/machine/ioapic.h>
#समावेश <plat/machine.h>

#समावेश <plat/machine/intel-vtd.h>

BOOT_CODE bool_t platAddDevices(व्योम)
{
    /* remove the MSI region as poking at this is undefined and may allow for
     * the user to generate arbitrary MSI interrupts. Only need to consider
     * this if it would actually be in the user device region */
    अगर (CONFIG_PADDR_USER_DEVICE_शीर्ष > 0xFFFFFFF8) {
        अगर (!reserve_region((p_region_t) {
        (word_t)0xFFFFFFF8, (word_t)0xFFFFFFF8 + 8
        })) {
            वापस false;
        }
    }
    वापस true;
}

/* ============================== timer ============================== */

#घोषणा TSC_FREQ_RETRIES 10

BOOT_CODE अटल अंतरभूत uint32_t measure_tsc_khz(व्योम)
{
    /* The frequency is repeatedly measured until the number of TSC
     * ticks in the pit wraparound interval (~50ms) fits in 32 bits.
     * On bare metal, this should succeed immediately, since x86
     * guarantees the number of TSC ticks in a second can be stored
     * in 32 bits. When running in a simulator, it's possible for
     * the emulation (or not) of the PIT and TSC to occasionally
     * allow too many TSC ticks per PIT wraparound. This loop
     * repeatedly measures the TSC ticks per PIT wraparound under
     * the expectation that it will eventually yield a sensible
     * result.
     */
    क्रम (पूर्णांक i = 0; i < TSC_FREQ_RETRIES; i++) {

        /* read tsc */
        uint64_t old_ticks = x86_rdtsc();

        /* measure how many tsc cycles pass while PIT wraps around */
        pit_wait_wraparound();

        uint64_t new_ticks = x86_rdtsc();

        uint64_t diff = new_ticks - old_ticks;

        अगर ((uint32_t)diff == diff && new_ticks > old_ticks) {
            वापस (uint32_t)diff / PIT_WRAPAROUND_MS;
        }

        म_लिखो("warning: TSC frequency too high (%d retries remaining)\n",
               TSC_FREQ_RETRIES - i - 1);
    }

    fail("TSC frequency too high");

    /* should never get here */
    वापस 0;
}

BOOT_CODE uint32_t tsc_init(व्योम)
{

#अगर CONFIG_PC99_TSC_FREQUENCY > 0
    वापस CONFIG_PC99_TSC_FREQUENCY / 1000000;
#पूर्ण_अगर

    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();
    uint32_t valid_models[] = {
        NEHALEM_1_MODEL_ID, NEHALEM_2_MODEL_ID, NEHALEM_3_MODEL_ID,
        SANDY_BRIDGE_1_MODEL_ID, SANDY_BRIDGE_2_MODEL_ID,
        HASWELL_1_MODEL_ID, HASWELL_2_MODEL_ID, HASWELL_3_MODEL_ID, HASWELL_4_MODEL_ID,
        IVY_BRIDGE_1_MODEL_ID, IVY_BRIDGE_2_MODEL_ID, IVY_BRIDGE_3_MODEL_ID,
        /* BROADWELL_1_MODEL_ID is an Atom that doesn't support the MSR */
        BROADWELL_2_MODEL_ID, BROADWELL_3_MODEL_ID, BROADWELL_4_MODEL_ID, BROADWELL_5_MODEL_ID,
        SKYLAKE_1_MODEL_ID, SKYLAKE_2_MODEL_ID
    };

    /* try to read the frequency from the platform info MSR */
    अगर (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID) {
        क्रम (पूर्णांक i = 0; i < ARRAY_आकार(valid_models); i++) {
            अगर (model_info->model == valid_models[i]) {

                /* read tsc freq from the platform info msr. Under some environments such
                 * as KVM this MSR will cause a GP fault even though it should be there.
                 * As such we perform a 'safe' rdmsr, which will catch a GP fault and
                 * tells through .success whether or not one happened */
                rdmsr_safe_परिणाम_t info = x86_rdmsr_safe(IA32_PLATFORM_INFO_MSR);
                अगर (info.success) {
                    uint32_t ratio = (((uint32_t) info.मूल्य) & 0xFF00) >> 8u;
                    /* Ignore hardware that claims a tsc frequency of zero */
                    अगर (ratio != 0) {
                        /* Convert to MHz */
                        अगर (model_info->model == NEHALEM_1_MODEL_ID ||
                            model_info->model == NEHALEM_2_MODEL_ID ||
                            model_info->model == NEHALEM_3_MODEL_ID) {
                            वापस ratio * 13333u / 100u;
                        } अन्यथा {
                            वापस ratio * 100u;
                        }
                    }
                }
                /* We just found the matching model, so no point continuing the search */
                अवरोध;
            }
        }
    }

    /* otherwise use the pit to find out the tsc freq */
    pit_init();

    /* wait for pit to wraparound */
    pit_wait_wraparound();

    /* count tsc ticks per ms */
    uint32_t tsc_khz = measure_tsc_khz();

    /* finally, return mhz */
    वापस tsc_khz / 1000u;
}
