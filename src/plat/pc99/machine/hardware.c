/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/kernel/apic.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/pic.h>
#அந்தர்பாவ <plat/machine/ioapic.h>
#அந்தர்பாவ <plat/machine.h>

#அந்தர்பாவ <plat/machine/intel-vtd.h>

BOOT_CODE bool_t platAddDevices(வெறுமை)
{
    /* remove the MSI region as poking at this is undefined and may allow for
     * the user to generate arbitrary MSI interrupts. Only need to consider
     * this if it would actually be in the user device region */
    எனில் (CONFIG_PADDR_USER_DEVICE_உச்சி > 0xFFFFFFF8) {
        எனில் (!reserve_region((p_region_t) {
        (word_t)0xFFFFFFF8, (word_t)0xFFFFFFF8 + 8
        })) {
            திரும்பு false;
        }
    }
    திரும்பு true;
}

/* ============================== timer ============================== */

#define TSC_FREQ_RETRIES 10

BOOT_CODE நிலையான உள்வரி uint32_t measure_tsc_khz(வெறுமை)
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
    க்கு (முழு i = 0; i < TSC_FREQ_RETRIES; i++) {

        /* read tsc */
        uint64_t old_ticks = x86_rdtsc();

        /* measure how many tsc cycles pass while PIT wraps around */
        pit_wait_wraparound();

        uint64_t new_ticks = x86_rdtsc();

        uint64_t diff = new_ticks - old_ticks;

        எனில் ((uint32_t)diff == diff && new_ticks > old_ticks) {
            திரும்பு (uint32_t)diff / PIT_WRAPAROUND_MS;
        }

        printf("warning: TSC frequency too high (%d retries remaining)\n",
               TSC_FREQ_RETRIES - i - 1);
    }

    fail("TSC frequency too high");

    /* should never get here */
    திரும்பு 0;
}

BOOT_CODE uint32_t tsc_init(வெறுமை)
{

#எனில் CONFIG_PC99_TSC_FREQUENCY > 0
    திரும்பு CONFIG_PC99_TSC_FREQUENCY / 1000000;
#endif

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
    எனில் (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID) {
        க்கு (முழு i = 0; i < ARRAY_SIZE(valid_models); i++) {
            எனில் (model_info->model == valid_models[i]) {

                /* read tsc freq from the platform info msr. Under some environments such
                 * as KVM this MSR will cause a GP fault even though it should be there.
                 * As such we perform a 'safe' rdmsr, which will catch a GP fault and
                 * tells through .success whether or not one happened */
                rdmsr_safe_result_t info = x86_rdmsr_safe(IA32_PLATFORM_INFO_MSR);
                எனில் (info.success) {
                    uint32_t ratio = (((uint32_t) info.மதிப்பு) & 0xFF00) >> 8u;
                    /* Ignore hardware that claims a tsc frequency of zero */
                    எனில் (ratio != 0) {
                        /* Convert to MHz */
                        எனில் (model_info->model == NEHALEM_1_MODEL_ID ||
                            model_info->model == NEHALEM_2_MODEL_ID ||
                            model_info->model == NEHALEM_3_MODEL_ID) {
                            திரும்பு ratio * 13333u / 100u;
                        } இல்லையெனில் {
                            திரும்பு ratio * 100u;
                        }
                    }
                }
                /* We just found the matching model, so no point continuing the search */
                நிறுத்து;
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
    திரும்பு tsc_khz / 1000u;
}
