/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <machine/io.h>
#شامل <arch/kernel/apic.h>
#شامل <arch/model/statedata.h>
#شامل <linker.h>
#شامل <plat/machine/pic.h>
#شامل <plat/machine/ioapic.h>
#شامل <plat/machine.h>

#شامل <plat/machine/intel-vtd.h>

BOOT_رمز bool_t plat_جوڑو_Devices(خالی)
{
    /* remove the MSI region as poking at this is undefined and may allow for
     * the user to generate arbitrary MSI interrupts. Only need to consider
     * this if it would actually be in the user device region */
    اگر (CONFIG_PADDR_USER_DEVICE_چوٹی > 0xFFFFFFF8) {
        اگر (!reserve_region((p_region_t) {
        (لفظ_t)0xFFFFFFF8, (لفظ_t)0xFFFFFFF8 + 8
        })) {
            واپس false;
        }
    }
    واپس true;
}

/* ============================== timer ============================== */

#تعریف_کرو TSC_FREQ_RETRIES 10

BOOT_رمز ساکن اندرونی uint32_t measure_tsc_khz(خالی)
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
    برائے (صحیح i = 0; i < TSC_FREQ_RETRIES; i++) {

        /* read tsc */
        uint64_t old_ticks = x86_rdtsc();

        /* measure how many tsc cycles pass while PIT wraps around */
        pit_wait_wraparound();

        uint64_t new_ticks = x86_rdtsc();

        uint64_t diff = new_ticks - old_ticks;

        اگر ((uint32_t)diff == diff && new_ticks > old_ticks) {
            واپس (uint32_t)diff / PIT_WRAPAROUND_MS;
        }

        لکھو("warning: TSC frequency too high (%d retries remaining)\n",
               TSC_FREQ_RETRIES - i - 1);
    }

    fail("TSC frequency too high");

    /* should never get here */
    واپس 0;
}

BOOT_رمز uint32_t tsc_init(خالی)
{

#اگر CONFIG_PC99_TSC_FREQUENCY > 0
    واپس CONFIG_PC99_TSC_FREQUENCY / 1000000;
#اگر_ختم

    x86_cpu_شناختی_عنصر_t *model_info = x86_cpuid_get_model_info();
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
    اگر (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID) {
        برائے (صحیح i = 0; i < ARRAY_سائز(valid_models); i++) {
            اگر (model_info->model == valid_models[i]) {

                /* read tsc freq from the platform info msr. Under some environments such
                 * as KVM this MSR will cause a GP fault even though it should be there.
                 * As such we perform a 'safe' rdmsr, which will catch a GP fault and
                 * tells through .success whether or not one happened */
                rdmsr_محفوظ_نتیجہ_t info = x86_rdmsr_محفوظ(IA32_PLATFORM_INFO_MSR);
                اگر (info.success) {
                    uint32_t تناسب = (((uint32_t) info.قدر) & 0xFF00) >> 8u;
                    /* Ignore hardware that claims a tsc frequency of zero */
                    اگر (تناسب != 0) {
                        /* Convert to MHz */
                        اگر (model_info->model == NEHALEM_1_MODEL_ID ||
                            model_info->model == NEHALEM_2_MODEL_ID ||
                            model_info->model == NEHALEM_3_MODEL_ID) {
                            واپس تناسب * 13333u / 100u;
                        } ورنہ {
                            واپس تناسب * 100u;
                        }
                    }
                }
                /* We just found the matching model, so no point continuing the search */
                توڑو;
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
    واپس tsc_khz / 1000u;
}
