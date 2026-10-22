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

#నిర్వచితమైతే PROFILER

#అన్తర్భావ <util.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <machine/profiler.h>

#నిర్వచితమైతే CHECKPOINT_PROFILER
/* The current checkpoint value */
అస్థిర గుర్తులేని పూర్ణాంకం checkpoint VISIBLE;
గుర్తులేని పూర్ణాంకం max_checkpoint;

/* Event count for each checkpoint value */
profiler_నమోదు_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#లేకపోతే
/* Number of entries the profiler currently keeps track of */
పూర్ణాంకం profiler_అంకె_entries;

/* Number of instructions the profiler could not record */
దీర్ఘ దీర్ఘ profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_నమోదు_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#అయితే_ముగింపు

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#నిర్వచితమైతే CHECKPOINT_PROFILER
శూన్య profiler_reset(శూన్య)
{
    మాట_t i;

    కొరకు (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].లెక్క = 0;
    }
    checkpoint = 0;
}

శూన్య profiler_list(శూన్య)
{
    గుర్తులేని పూర్ణాంకం samples, i, లెక్క;

    రాయి("checkpoint count\n");

    samples = 0;
    లెక్క = 0;
    కొరకు (i = 0; i <= max_checkpoint; i++) {
        ఒకవేళ (profiler_entries[i].pc != 0) {
            రాయి("%u %u\n", i, (గుర్తులేని పూర్ణాంకం)profiler_entries[i].లెక్క);
            samples += profiler_entries[i].లెక్క;
            లెక్క++;
        }
    }

    రాయి("%u checkpoints, %u sample(s)\n", లెక్క, samples);
}

శూన్య profiler_రికార్డు_sample(మాట_t pc)
{
    ఒకవేళ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ఒకవేళ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].లెక్క++;
}
#లేకపోతే
/*
 * Reset all counters
 */
శూన్య profiler_reset(శూన్య)
{
    కొరకు (మాట_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].లెక్క = 0;
    }
    profiler_అంకె_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
శూన్య profiler_list(శూన్య)
{
    దీర్ఘ దీర్ఘ samples;

    /* Print header */
    రాయి("addr     count\n");

    /* Print out each address */
    samples = 0;
    కొరకు (మాట_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ఒకవేళ (profiler_entries[i].pc != 0) {
            రాయి("%x %d\n", (గుర్తులేని పూర్ణాంకం)profiler_entries[i].pc,
                   (పూర్ణాంకం)profiler_entries[i].లెక్క);
            samples += profiler_entries[i].లెక్క;
        }
    }

    /* Print statistics */
    రాయి("\n%d unique address(es), %d sample(s)\n",
           (పూర్ణాంకం)profiler_అంకె_entries, (పూర్ణాంకం)samples);
    ఒకవేళ (profiler_dropped_instructions > 0) {
        రాయి("*** WARNING : %d instructions dropped\n",
               (పూర్ణాంకం)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
శూన్య profiler_రికార్డు_sample(మాట_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    స్థిరరాశి పూర్ణాంకం hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    మాట_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    మాట_t hash2 = ((pc >> 2) % hashVal) + 1;

    ఒకవేళ (!profiler_enabled) {
        తిరిగి;
    }

    అయితే (true) {

        ఒకవేళ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].లెక్క++;
            ఆపు;

        } లేకపోతే ఒకవేళ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ఒకవేళ (profiler_అంకె_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].లెక్క = 1;
                profiler_అంకె_entries++;
                ఆపు;
            } లేకపోతే {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ఆపు;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* CONFIG_KDB_PROFILER */
