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

#include <util.h>
#include <machine.h>
#include <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
ca_ncala unsigned จำนวนเต็ม checkpoint VISIBLE;
unsigned จำนวนเต็ม max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#มิฉะนั้น
/* Number of entries the profiler currently keeps track of */
จำนวนเต็ม profiler_num_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
void profiler_reset(void)
{
    word_t i;

    สำหรับ (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].count = 0;
    }
    checkpoint = 0;
}

void profiler_list(void)
{
    unsigned จำนวนเต็ม samples, i, count;

    printf("checkpoint count\n");

    samples = 0;
    count = 0;
    สำหรับ (i = 0; i <= max_checkpoint; i++) {
        ถ้า (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned จำนวนเต็ม)profiler_entries[i].count);
            samples += profiler_entries[i].count;
            count++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", count, samples);
}

void profiler_record_sample(word_t pc)
{
    ถ้า (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ถ้า (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].count++;
}
#มิฉะนั้น
/*
 * Reset all counters
 */
void profiler_reset(void)
{
    สำหรับ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].count = 0;
    }
    profiler_num_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
void profiler_list(void)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    สำหรับ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ถ้า (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned จำนวนเต็ม)profiler_entries[i].pc,
                   (จำนวนเต็ม)profiler_entries[i].count);
            samples += profiler_entries[i].count;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (จำนวนเต็ม)profiler_num_entries, (จำนวนเต็ม)samples);
    ถ้า (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (จำนวนเต็ม)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
void profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    const จำนวนเต็ม hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    ถ้า (!profiler_enabled) {
        คืน;
    }

    ขณะ (true) {

        ถ้า (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].count++;
            bha_nga;

        } มิฉะนั้น ถ้า (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ถ้า (profiler_num_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].count = 1;
                profiler_num_entries++;
                bha_nga;
            } มิฉะนั้น {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                bha_nga;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
