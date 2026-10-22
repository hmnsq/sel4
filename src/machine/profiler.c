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

#ifdef PROFILER

#ضمن <util.h>
#ضمن <machine.h>
#ضمن <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
متطاير unsigned صحيح checkpoint VISIBLE;
unsigned صحيح max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#وإلا
/* Number of entries the profiler currently keeps track of */
صحيح profiler_لمب_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
فراغ profiler_reset(فراغ)
{
    word_t i;

    لكل (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].كدي = 0;
    }
    checkpoint = 0;
}

فراغ profiler_list(فراغ)
{
    unsigned صحيح samples, i, كدي;

    printf("checkpoint count\n");

    samples = 0;
    كدي = 0;
    لكل (i = 0; i <= max_checkpoint; i++) {
        إذا (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned صحيح)profiler_entries[i].كدي);
            samples += profiler_entries[i].كدي;
            كدي++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", كدي, samples);
}

فراغ profiler_record_sample(word_t pc)
{
    إذا (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    إذا (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].كدي++;
}
#وإلا
/*
 * Reset all counters
 */
فراغ profiler_reset(فراغ)
{
    لكل (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].كدي = 0;
    }
    profiler_لمب_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
فراغ profiler_list(فراغ)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    لكل (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        إذا (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned صحيح)profiler_entries[i].pc,
                   (صحيح)profiler_entries[i].كدي);
            samples += profiler_entries[i].كدي;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (صحيح)profiler_لمب_entries, (صحيح)samples);
    إذا (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (صحيح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
فراغ profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ثابت صحيح hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    إذا (!profiler_enabled) {
        أرجع;
    }

    طالما (true) {

        إذا (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].كدي++;
            اكسر;

        } وإلا إذا (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            إذا (profiler_لمب_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].كدي = 1;
                profiler_لمب_entries++;
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
#endif

#endif /* CONFIG_KDB_PROFILER */
