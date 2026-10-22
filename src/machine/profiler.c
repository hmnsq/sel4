/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/*
 * Kernel Profiler
 *
 * 2007 David Greenaway
 * 2007 Ported to seL4 C kernel by Philip Derrin
 */

#اگر_تعریف_شده PROFILER

#ضمن <util.h>
#ضمن <machine.h>
#ضمن <machine/profiler.h>

#اگر_تعریف_شده CHECKPOINT_PROFILER
/* The current checkpoint value */
متطاير بیعلامت صحيح checkpoint VISIBLE;
بیعلامت صحيح max_checkpoint;

/* Event count for each checkpoint value */
profiler_مدخل_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#وإلا
/* Number of entries the profiler currently keeps track of */
صحيح profiler_رقم_entries;

/* Number of instructions the profiler could not record */
دراز دراز profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_مدخل_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#پایان_اگر

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#اگر_تعریف_شده CHECKPOINT_PROFILER
فراغ profiler_reset(فراغ)
{
    کلمه_t i;

    لكل (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمار = 0;
    }
    checkpoint = 0;
}

فراغ profiler_list(فراغ)
{
    بیعلامت صحيح samples, i, شمار;

    چاپ_کن("checkpoint count\n");

    samples = 0;
    شمار = 0;
    لكل (i = 0; i <= max_checkpoint; i++) {
        إذا (profiler_entries[i].pc != 0) {
            چاپ_کن("%u %u\n", i, (بیعلامت صحيح)profiler_entries[i].شمار);
            samples += profiler_entries[i].شمار;
            شمار++;
        }
    }

    چاپ_کن("%u checkpoints, %u sample(s)\n", شمار, samples);
}

فراغ profiler_رکورد_sample(کلمه_t pc)
{
    إذا (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    إذا (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].شمار++;
}
#وإلا
/*
 * Reset all counters
 */
فراغ profiler_reset(فراغ)
{
    لكل (کلمه_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمار = 0;
    }
    profiler_رقم_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
فراغ profiler_list(فراغ)
{
    دراز دراز samples;

    /* Print header */
    چاپ_کن("addr     count\n");

    /* Print out each address */
    samples = 0;
    لكل (کلمه_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        إذا (profiler_entries[i].pc != 0) {
            چاپ_کن("%x %d\n", (بیعلامت صحيح)profiler_entries[i].pc,
                   (صحيح)profiler_entries[i].شمار);
            samples += profiler_entries[i].شمار;
        }
    }

    /* Print statistics */
    چاپ_کن("\n%d unique address(es), %d sample(s)\n",
           (صحيح)profiler_رقم_entries, (صحيح)samples);
    إذا (profiler_dropped_instructions > 0) {
        چاپ_کن("*** WARNING : %d instructions dropped\n",
               (صحيح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
فراغ profiler_رکورد_sample(کلمه_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ثابت صحيح hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    کلمه_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    کلمه_t hash2 = ((pc >> 2) % hashVal) + 1;

    إذا (!profiler_enabled) {
        أرجع;
    }

    طالما (true) {

        إذا (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].شمار++;
            اكسر;

        } وإلا إذا (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            إذا (profiler_رقم_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].شمار = 1;
                profiler_رقم_entries++;
                اكسر;
            } وإلا {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                اكسر;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#پایان_اگر

#پایان_اگر /* CONFIG_KDB_PROFILER */
