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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PROFILER

#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/profiler.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECKPOINT_PROFILER
/* The current checkpoint value */
ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ checkpoint VISIBLE;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_checkpoint;

/* Event count for each checkpoint value */
profiler_ਇੰਦਰਾਜ_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#ਨਹੀਂਤਾਂ
/* Number of entries the profiler currently keeps track of */
ਪੂਰਨ profiler_ਅੰਕ_entries;

/* Number of instructions the profiler could not record */
ਲੰਮਾ ਲੰਮਾ profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_ਇੰਦਰਾਜ_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#ਜੇ_ਅੰਤ

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECKPOINT_PROFILER
ਖਾਲੀ profiler_reset(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t i;

    ਲਈ (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ਗਿਣਤੀ_2 = 0;
    }
    checkpoint = 0;
}

ਖਾਲੀ profiler_list(ਖਾਲੀ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ samples, i, ਗਿਣਤੀ_2;

    ਲਿਖੋ("checkpoint count\n");

    samples = 0;
    ਗਿਣਤੀ_2 = 0;
    ਲਈ (i = 0; i <= max_checkpoint; i++) {
        ਜੇ (profiler_entries[i].pc != 0) {
            ਲਿਖੋ("%u %u\n", i, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)profiler_entries[i].ਗਿਣਤੀ_2);
            samples += profiler_entries[i].ਗਿਣਤੀ_2;
            ਗਿਣਤੀ_2++;
        }
    }

    ਲਿਖੋ("%u checkpoints, %u sample(s)\n", ਗਿਣਤੀ_2, samples);
}

ਖਾਲੀ profiler_ਰਿਕਾਰਡ_sample(ਸ਼ਬਦ_t pc)
{
    ਜੇ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ਜੇ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ਗਿਣਤੀ_2++;
}
#ਨਹੀਂਤਾਂ
/*
 * Reset all counters
 */
ਖਾਲੀ profiler_reset(ਖਾਲੀ)
{
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ਗਿਣਤੀ_2 = 0;
    }
    profiler_ਅੰਕ_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
ਖਾਲੀ profiler_list(ਖਾਲੀ)
{
    ਲੰਮਾ ਲੰਮਾ samples;

    /* Print header */
    ਲਿਖੋ("addr     count\n");

    /* Print out each address */
    samples = 0;
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ਜੇ (profiler_entries[i].pc != 0) {
            ਲਿਖੋ("%x %d\n", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)profiler_entries[i].pc,
                   (ਪੂਰਨ)profiler_entries[i].ਗਿਣਤੀ_2);
            samples += profiler_entries[i].ਗਿਣਤੀ_2;
        }
    }

    /* Print statistics */
    ਲਿਖੋ("\n%d unique address(es), %d sample(s)\n",
           (ਪੂਰਨ)profiler_ਅੰਕ_entries, (ਪੂਰਨ)samples);
    ਜੇ (profiler_dropped_instructions > 0) {
        ਲਿਖੋ("*** WARNING : %d instructions dropped\n",
               (ਪੂਰਨ)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
ਖਾਲੀ profiler_ਰਿਕਾਰਡ_sample(ਸ਼ਬਦ_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ਅਚਲ ਪੂਰਨ hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    ਸ਼ਬਦ_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    ਸ਼ਬਦ_t hash2 = ((pc >> 2) % hashVal) + 1;

    ਜੇ (!profiler_enabled) {
        ਵਾਪਸ;
    }

    ਜਦੋਂ (true) {

        ਜੇ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ਗਿਣਤੀ_2++;
            ਰੋਕੋ;

        } ਨਹੀਂਤਾਂ ਜੇ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ਜੇ (profiler_ਅੰਕ_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ਗਿਣਤੀ_2 = 1;
                profiler_ਅੰਕ_entries++;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ਰੋਕੋ;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CONFIG_KDB_PROFILER */
