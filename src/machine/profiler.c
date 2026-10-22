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

#شامل <util.h>
#شامل <machine.h>
#شامل <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
فرار unsigned صحیح checkpoint VISIBLE;
unsigned صحیح max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#وگرنه
/* Number of entries the profiler currently keeps track of */
صحیح profiler_رقم_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
تهی profiler_reset(تهی)
{
    word_t i;

    برای (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمار = 0;
    }
    checkpoint = 0;
}

تهی profiler_list(تهی)
{
    unsigned صحیح samples, i, شمار;

    printf("checkpoint count\n");

    samples = 0;
    شمار = 0;
    برای (i = 0; i <= max_checkpoint; i++) {
        اگر (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned صحیح)profiler_entries[i].شمار);
            samples += profiler_entries[i].شمار;
            شمار++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", شمار, samples);
}

تهی profiler_record_sample(word_t pc)
{
    اگر (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    اگر (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].شمار++;
}
#وگرنه
/*
 * Reset all counters
 */
تهی profiler_reset(تهی)
{
    برای (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].شمار = 0;
    }
    profiler_رقم_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
تهی profiler_list(تهی)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    برای (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        اگر (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned صحیح)profiler_entries[i].pc,
                   (صحیح)profiler_entries[i].شمار);
            samples += profiler_entries[i].شمار;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (صحیح)profiler_رقم_entries, (صحیح)samples);
    اگر (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (صحیح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
تهی profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ثابت صحیح hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    اگر (!profiler_enabled) {
        بازگشت;
    }

    وقتی (true) {

        اگر (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].شمار++;
            بشکن;

        } وگرنه اگر (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            اگر (profiler_رقم_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].شمار = 1;
                profiler_رقم_entries++;
                بشکن;
            } وگرنه {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                بشکن;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
