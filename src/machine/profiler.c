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

#جيڪڏهن_وصف_ٿيل PROFILER

#ضمن <util.h>
#ضمن <machine.h>
#ضمن <machine/profiler.h>

#جيڪڏهن_وصف_ٿيل CHECKPOINT_PROFILER
/* The current checkpoint value */
متطاير بنانشان صحيح checkpoint VISIBLE;
بنانشان صحيح max_checkpoint;

/* Event count for each checkpoint value */
profiler_داخلا_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#وإلا
/* Number of entries the profiler currently keeps track of */
صحيح profiler_انگ_entries;

/* Number of instructions the profiler could not record */
ڊگهو ڊگهو profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_داخلا_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#جيڪڏهن_پورو

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#جيڪڏهن_وصف_ٿيل CHECKPOINT_PROFILER
فراغ profiler_reset(فراغ)
{
    لفظ_t i;

    لكل (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ڳڻپ = 0;
    }
    checkpoint = 0;
}

فراغ profiler_list(فراغ)
{
    بنانشان صحيح samples, i, ڳڻپ;

    لکو("checkpoint count\n");

    samples = 0;
    ڳڻپ = 0;
    لكل (i = 0; i <= max_checkpoint; i++) {
        إذا (profiler_entries[i].pc != 0) {
            لکو("%u %u\n", i, (بنانشان صحيح)profiler_entries[i].ڳڻپ);
            samples += profiler_entries[i].ڳڻپ;
            ڳڻپ++;
        }
    }

    لکو("%u checkpoints, %u sample(s)\n", ڳڻپ, samples);
}

فراغ profiler_رڪارڊ_sample(لفظ_t pc)
{
    إذا (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    إذا (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ڳڻپ++;
}
#وإلا
/*
 * Reset all counters
 */
فراغ profiler_reset(فراغ)
{
    لكل (لفظ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ڳڻپ = 0;
    }
    profiler_انگ_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
فراغ profiler_list(فراغ)
{
    ڊگهو ڊگهو samples;

    /* Print header */
    لکو("addr     count\n");

    /* Print out each address */
    samples = 0;
    لكل (لفظ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        إذا (profiler_entries[i].pc != 0) {
            لکو("%x %d\n", (بنانشان صحيح)profiler_entries[i].pc,
                   (صحيح)profiler_entries[i].ڳڻپ);
            samples += profiler_entries[i].ڳڻپ;
        }
    }

    /* Print statistics */
    لکو("\n%d unique address(es), %d sample(s)\n",
           (صحيح)profiler_انگ_entries, (صحيح)samples);
    إذا (profiler_dropped_instructions > 0) {
        لکو("*** WARNING : %d instructions dropped\n",
               (صحيح)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
فراغ profiler_رڪارڊ_sample(لفظ_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ثابت صحيح hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    لفظ_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    لفظ_t hash2 = ((pc >> 2) % hashVal) + 1;

    إذا (!profiler_enabled) {
        أرجع;
    }

    طالما (true) {

        إذا (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ڳڻپ++;
            اكسر;

        } وإلا إذا (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            إذا (profiler_انگ_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ڳڻپ = 1;
                profiler_انگ_entries++;
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
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CONFIG_KDB_PROFILER */
