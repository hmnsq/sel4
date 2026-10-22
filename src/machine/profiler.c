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

#که_تعریف_شوی PROFILER

#ضمن <util.h>
#ضمن <machine.h>
#ضمن <machine/profiler.h>

#که_تعریف_شوی CHECKPOINT_PROFILER
/* The current checkpoint value */
متطاير بینښه صحيح checkpoint VISIBLE;
بینښه صحيح max_checkpoint;

/* Event count for each checkpoint value */
profiler_ثبت_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#وإلا
/* Number of entries the profiler currently keeps track of */
صحيح profiler_عدد_entries;

/* Number of instructions the profiler could not record */
اوږد اوږد profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_ثبت_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#د_که_پای

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#که_تعریف_شوی CHECKPOINT_PROFILER
فراغ profiler_reset(فراغ)
{
    کلمه_t i;

    لكل (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمېر = 0;
    }
    checkpoint = 0;
}

فراغ profiler_list(فراغ)
{
    بینښه صحيح samples, i, شمېر;

    چاپ_کړه("checkpoint count\n");

    samples = 0;
    شمېر = 0;
    لكل (i = 0; i <= max_checkpoint; i++) {
        إذا (profiler_entries[i].pc != 0) {
            چاپ_کړه("%u %u\n", i, (بینښه صحيح)profiler_entries[i].شمېر);
            samples += profiler_entries[i].شمېر;
            شمېر++;
        }
    }

    چاپ_کړه("%u checkpoints, %u sample(s)\n", شمېر, samples);
}

فراغ profiler_ریکارډ_sample(کلمه_t pc)
{
    إذا (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    إذا (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].شمېر++;
}
#وإلا
/*
 * Reset all counters
 */
فراغ profiler_reset(فراغ)
{
    لكل (کلمه_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمېر = 0;
    }
    profiler_عدد_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
فراغ profiler_list(فراغ)
{
    اوږد اوږد samples;

    /* Print header */
    چاپ_کړه("addr     count\n");

    /* Print out each address */
    samples = 0;
    لكل (کلمه_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        إذا (profiler_entries[i].pc != 0) {
            چاپ_کړه("%x %d\n", (بینښه صحيح)profiler_entries[i].pc,
                   (صحيح)profiler_entries[i].شمېر);
            samples += profiler_entries[i].شمېر;
        }
    }

    /* Print statistics */
    چاپ_کړه("\n%d unique address(es), %d sample(s)\n",
           (صحيح)profiler_عدد_entries, (صحيح)samples);
    إذا (profiler_dropped_instructions > 0) {
        چاپ_کړه("*** WARNING : %d instructions dropped\n",
               (صحيح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
فراغ profiler_ریکارډ_sample(کلمه_t pc)
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
            profiler_entries[hash].شمېر++;
            اكسر;

        } وإلا إذا (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            إذا (profiler_عدد_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].شمېر = 1;
                profiler_عدد_entries++;
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
#د_که_پای

#د_که_پای /* CONFIG_KDB_PROFILER */
