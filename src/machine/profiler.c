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

#اگر_تعریف_شدہ PROFILER

#شامل <util.h>
#شامل <machine.h>
#شامل <machine/profiler.h>

#اگر_تعریف_شدہ CHECKPOINT_PROFILER
/* The current checkpoint value */
متلون غیرعلامتی صحیح checkpoint VISIBLE;
غیرعلامتی صحیح max_checkpoint;

/* Event count for each checkpoint value */
profiler_اندراج_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#ورنہ
/* Number of entries the profiler currently keeps track of */
صحیح profiler_عدد_entries;

/* Number of instructions the profiler could not record */
طویل طویل profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_اندراج_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#اگر_ختم

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#اگر_تعریف_شدہ CHECKPOINT_PROFILER
خالی profiler_reset(خالی)
{
    لفظ_t i;

    برائے (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].گنتی = 0;
    }
    checkpoint = 0;
}

خالی profiler_list(خالی)
{
    غیرعلامتی صحیح samples, i, گنتی;

    لکھو("checkpoint count\n");

    samples = 0;
    گنتی = 0;
    برائے (i = 0; i <= max_checkpoint; i++) {
        اگر (profiler_entries[i].pc != 0) {
            لکھو("%u %u\n", i, (غیرعلامتی صحیح)profiler_entries[i].گنتی);
            samples += profiler_entries[i].گنتی;
            گنتی++;
        }
    }

    لکھو("%u checkpoints, %u sample(s)\n", گنتی, samples);
}

خالی profiler_ریکارڈ_sample(لفظ_t pc)
{
    اگر (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    اگر (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].گنتی++;
}
#ورنہ
/*
 * Reset all counters
 */
خالی profiler_reset(خالی)
{
    برائے (لفظ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].گنتی = 0;
    }
    profiler_عدد_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
خالی profiler_list(خالی)
{
    طویل طویل samples;

    /* Print header */
    لکھو("addr     count\n");

    /* Print out each address */
    samples = 0;
    برائے (لفظ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        اگر (profiler_entries[i].pc != 0) {
            لکھو("%x %d\n", (غیرعلامتی صحیح)profiler_entries[i].pc,
                   (صحیح)profiler_entries[i].گنتی);
            samples += profiler_entries[i].گنتی;
        }
    }

    /* Print statistics */
    لکھو("\n%d unique address(es), %d sample(s)\n",
           (صحیح)profiler_عدد_entries, (صحیح)samples);
    اگر (profiler_dropped_instructions > 0) {
        لکھو("*** WARNING : %d instructions dropped\n",
               (صحیح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
خالی profiler_ریکارڈ_sample(لفظ_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ثابت صحیح hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    لفظ_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    لفظ_t hash2 = ((pc >> 2) % hashVal) + 1;

    اگر (!profiler_enabled) {
        واپس;
    }

    جب (true) {

        اگر (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].گنتی++;
            توڑو;

        } ورنہ اگر (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            اگر (profiler_عدد_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].گنتی = 1;
                profiler_عدد_entries++;
                توڑو;
            } ورنہ {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                توڑو;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#اگر_ختم

#اگر_ختم /* CONFIG_KDB_PROFILER */
