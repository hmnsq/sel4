/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>

/* functions to manage the circular buffer of
 * sporadic budget replenishments (refills for short).
 *
 * The circular buffer always has at least one item in it.
 *
 * Items are appended at the tail (the back) and
 * removed from the head (the front). Below is
 * an example of a queue with 4 items (h = head, t = tail, x = item, [] = slot)
 * and max size 8.
 *
 * [][h][x][x][t][][][]
 *
 * and another example of a queue with 5 items
 *
 * [x][t][][][][h][x][x]
 *
 * The queue has a minimum size of 1, so it is possible that h == t.
 *
 * The queue is implemented as head + tail rather than head + size as
 * we cannot use the mod operator on all architectures without accessing
 * the fpu or implementing divide.
 */

/* return the index of the next item in the refill queue */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t refill_ಮುಂದಿನ(sched_context_t *sc, ಶಬ್ದ_t ಸೂಚ್ಯಂಕ)
{
    ಹಿಂತಿರುಗಿ (ಸೂಚ್ಯಂಕ == sc->scRefillMax - 1u) ? (0) : ಸೂಚ್ಯಂಕ + 1u;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
/* for debugging */
ಬಳಸದ ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ print_ಸೂಚ್ಯಂಕ(sched_context_t *sc, ಶಬ್ದ_t ಸೂಚ್ಯಂಕ)
{

    ಬರೆ("index %lu, Amount: %llx, time %llx\n", ಸೂಚ್ಯಂಕ, refill_ಸೂಚ್ಯಂಕ(sc, ಸೂಚ್ಯಂಕ)->rAmount,
           refill_ಸೂಚ್ಯಂಕ(sc, ಸೂಚ್ಯಂಕ)->r_ಸಮಯ);
}

ಬಳಸದ ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ refill_print(sched_context_t *sc)
{
    ಬರೆ("Head %lu tail %lu\n", sc->sc_Refill_ತಲೆ, sc->sc_Refill_ಬಾಲ);
    ಶಬ್ದ_t current = sc->sc_Refill_ತಲೆ;
    /* always print the head */
    print_ಸೂಚ್ಯಂಕ(sc, current);

    ಆಗ (current != sc->sc_Refill_ಬಾಲ) {
        current = refill_ಮುಂದಿನ(sc, current);
        print_ಸೂಚ್ಯಂಕ(sc, current);
    }

}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ಸ್ಥಿರ ಬಳಸದ bool_t refill_ordered(sched_context_t *sc)
{
    ಆದರೆ (is_ಸುತ್ತು_Robin(sc)) {
        ಹಿಂತಿರುಗಿ true;
    }

    ಶಬ್ದ_t current = sc->sc_Refill_ತಲೆ;
    ಶಬ್ದ_t ಮುಂದಿನ = refill_ಮುಂದಿನ(sc, sc->sc_Refill_ತಲೆ);

    ಆಗ (current != sc->sc_Refill_ಬಾಲ) {
        ಆದರೆ (!(refill_ಸೂಚ್ಯಂಕ(sc, current)->r_ಸಮಯ + refill_ಸೂಚ್ಯಂಕ(sc, current)->rAmount <= refill_ಸೂಚ್ಯಂಕ(sc, ಮುಂದಿನ)->r_ಸಮಯ)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
            refill_print(sc);
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ false;
        }
        current = ಮುಂದಿನ;
        ಮುಂದಿನ = refill_ಮುಂದಿನ(sc, current);
    }

    ಹಿಂತಿರುಗಿ true;
}

#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_ಆರಂಭ(sc) ticks_t ಮೊತ್ತ = refill_ಮೊತ್ತ(sc); ಪ್ರತಿಜ್ಞಾ(is_ಸುತ್ತು_Robin(sc) || refill_ordered(sc));
#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_CHECK(sc, budget) \
    ಮಾಡು { \
        ಪ್ರತಿಜ್ಞಾ(refill_ಮೊತ್ತ(sc) == budget); ಪ್ರತಿಜ್ಞಾ(is_ಸುತ್ತು_Robin(sc) || refill_ordered(sc)); \
    } ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_ಕೊನೆ(sc) \
    ಮಾಡು {\
        REFILL_SANITY_CHECK(sc, ಮೊತ್ತ);\
    } ಆಗ (0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_ಆರಂಭ(sc)
#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_CHECK(sc, budget)
#ವ್ಯಾಖ್ಯಾನಿಸು REFILL_SANITY_ಕೊನೆ(sc)
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ಸ್ಥಿರ ಬಳಸದ ticks_t refill_ಮೊತ್ತ(sched_context_t *sc)
{
    ticks_t ಮೊತ್ತ_2 = refill_ತಲೆ(sc)->rAmount;
    ಶಬ್ದ_t current = sc->sc_Refill_ತಲೆ;

    ಆಗ (current != sc->sc_Refill_ಬಾಲ) {
        current = refill_ಮುಂದಿನ(sc, current);
        ಮೊತ್ತ_2 += refill_ಸೂಚ್ಯಂಕ(sc, current)->rAmount;
    }

    ಹಿಂತಿರುಗಿ ಮೊತ್ತ_2;
}

/* pop head of refill queue */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ refill_t refill_ತೆಗೆ_ತಲೆ(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    ಪ್ರತಿಜ್ಞಾ(!refill_single(sc));

    ಬಳಸದ ಶಬ್ದ_t prev_ಗಾತ್ರ = refill_ಗಾತ್ರ(sc);
    refill_t refill = *refill_ತಲೆ(sc);
    sc->sc_Refill_ತಲೆ = refill_ಮುಂದಿನ(sc, sc->sc_Refill_ತಲೆ);

    /* sanity */
    ಪ್ರತಿಜ್ಞಾ(prev_ಗಾತ್ರ == (refill_ಗಾತ್ರ(sc) + 1));
    ಪ್ರತಿಜ್ಞಾ(sc->sc_Refill_ತಲೆ < sc->scRefillMax);
    ಹಿಂತಿರುಗಿ refill;
}

/* add item to tail of refill queue */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ refill_ಕೂಡಿಸು_ಬಾಲ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    ಪ್ರತಿಜ್ಞಾ(refill_ಗಾತ್ರ(sc) < sc->scRefillMax);

    ಶಬ್ದ_t new_ಬಾಲ = refill_ಮುಂದಿನ(sc, sc->sc_Refill_ಬಾಲ);
    sc->sc_Refill_ಬಾಲ = new_ಬಾಲ;
    *refill_ಬಾಲ(sc) = refill;

    /* sanity */
    ಪ್ರತಿಜ್ಞಾ(new_ಬಾಲ < sc->scRefillMax);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ maybe_ಕೂಡಿಸು_ಖಾಲಿಗಳು_ಬಾಲ(sched_context_t *sc)
{
    ಆದರೆ (is_ಸುತ್ತು_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t ಖಾಲಿಗಳು_ಬಾಲ = { .r_ಸಮಯ = refill_ತಲೆ(sc)->r_ಸಮಯ };
        refill_ಕೂಡಿಸು_ಬಾಲ(sc, ಖಾಲಿಗಳು_ಬಾಲ);
        ಪ್ರತಿಜ್ಞಾ(refill_ಗಾತ್ರ(sc) == MIN_REFILLS);
    }
}

ಶೂನ್ಯ refill_new(sched_context_t *sc, ಶಬ್ದ_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_ತಲೆ = 0;
    sc->sc_Refill_ಬಾಲ = 0;
    sc->scRefillMax = max_refills;
    ಪ್ರತಿಜ್ಞಾ(budget >= MIN_BUDGET);
    /* full budget available */
    refill_ತಲೆ(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_ತಲೆ(sc)->r_ಸಮಯ = ಗಂಟು_STATE(ks_Cur_ಸಮಯ);
    maybe_ಕೂಡಿಸು_ಖಾಲಿಗಳು_ಬಾಲ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

ಶೂನ್ಯ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, ಶಬ್ದ_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    ಪ್ರತಿಜ್ಞಾ(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_ಸೂಚ್ಯಂಕ(sc, 0) = *refill_ತಲೆ(sc);
    sc->sc_Refill_ತಲೆ = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_ಬಾಲ = sc->sc_Refill_ತಲೆ;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    ಆದರೆ (refill_ready(sc)) {
        refill_ತಲೆ(sc)->r_ಸಮಯ = ಗಂಟು_STATE(ks_Cur_ಸಮಯ);
    }

    ಆದರೆ (refill_ತಲೆ(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_ತಲೆ(sc)->rAmount = new_budget;
        maybe_ಕೂಡಿಸು_ಖಾಲಿಗಳು_ಬಾಲ(sc);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_ತಲೆ(sc)->rAmount),
                         .r_ಸಮಯ = refill_ತಲೆ(sc)->r_ಸಮಯ + new_period
                       };
        refill_ಕೂಡಿಸು_ಬಾಲ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಬಳಸಿದ(sched_context_t *sc, refill_t new)
{
    refill_t ಬಾಲ = *refill_ಬಾಲ(sc);
    ಆದರೆ (unlikely(ಬಾಲ.r_ಸಮಯ + ಬಾಲ.rAmount >= new.r_ಸಮಯ)) {
        /* Merge overlapping or adjacent refill.
         *
         * refill_update can produce a tail refill that will overlap
         * with new refills when time is charged to the head refill.
         *
         * Preemption will cause the head refill to be partially
         * charged. When the head refill is again later charged the
         * additionally charged amount will be added where the new
         * refill ended such that they are merged here. This ensures
         * that (beyond a refill being split as it is charged
         * incrementally) a refill split is only caused by a thread
         * blocking. */
        refill_ಬಾಲ(sc)->rAmount = ಬಾಲ.rAmount + new.rAmount;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (likely(!refill_ತುಂಬಿದ(sc))) {
        /* Add tail normally */
        refill_ಕೂಡಿಸು_ಬಾಲ(sc, new);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Delay existing tail to merge */
        refill_ಬಾಲ(sc)->r_ಸಮಯ = new.r_ಸಮಯ - ಬಾಲ.rAmount;
        refill_ಬಾಲ(sc)->rAmount = ಬಾಲ.rAmount + new.rAmount;
    }
}

ಸ್ಥಿರ bool_t refill_ತಲೆ_overlapping(sched_context_t *sc)
{
    ಆದರೆ (!refill_single(sc)) {
        refill_t ತಲೆ = *refill_ತಲೆ(sc);
        ticks_t ಬಾಲ = ತಲೆ.r_ಸಮಯ + ತಲೆ.rAmount;
        ಹಿಂತಿರುಗಿ refill_ಸೂಚ್ಯಂಕ(sc, refill_ಮುಂದಿನ(sc, sc->sc_Refill_ತಲೆ))->r_ಸಮಯ <= ಬಾಲ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ false;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ತಲೆ_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t ತಲೆ = *refill_ತಲೆ(sc);
    ಹಿಂತಿರುಗಿ ತಲೆ.rAmount <= usage && ತಲೆ.r_ಸಮಯ < MAX_RELEASE_ಸಮಯ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ticks_t charge_entire_ತಲೆ_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t ತಲೆ = *refill_ತಲೆ(sc);

    ಆದರೆ (refill_single(sc)) {
        refill_ತಲೆ(sc)->r_ಸಮಯ = ತಲೆ.r_ಸಮಯ + sc->scPeriod;
    } ಇಲ್ಲದಿದ್ದರೆ {
        refill_t old_ತಲೆ = refill_ತೆಗೆ_ತಲೆ(sc);
        old_ತಲೆ.r_ಸಮಯ += sc->scPeriod;
        ವೇಳಾಪಟ್ಟಿ_ಬಳಸಿದ(sc, old_ತಲೆ);
    }

    ಹಿಂತಿರುಗಿ usage - ತಲೆ.rAmount;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
{
    ticks_t current_usage = usage;

    /*
     * We repeatedly charge the head refill in its entirety until we end up
     * with a partial refill, or at a point where we can't place refills
     * into the future without integer overflow.
     *
     * Verification actually requires that the current time is at least
     * 3 * MAX_PERIOD from the INT64_MAX value, so to ease relation to
     * that assertion we ensure that we never delay a refill past this
     * point in the future.
     */
    ಆಗ (ತಲೆ_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_ತಲೆ_refill(sc, current_usage);
    }

    ಹಿಂತಿರುಗಿ current_usage;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ತಲೆ_refill_insufficient(sched_context_t *sc)
{
    ಹಿಂತಿರುಗಿ refill_ತಲೆ(sc)->rAmount < MIN_BUDGET;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ವಿಲೀನ_nonoverlapping_ತಲೆ_refill(sched_context_t *sc)
{
    refill_t ತಲೆ = refill_ತೆಗೆ_ತಲೆ(sc);
    refill_ತಲೆ(sc)->rAmount += ತಲೆ.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_ತಲೆ(sc)->r_ಸಮಯ -= ತಲೆ.rAmount;
}

ಶೂನ್ಯ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ಗಂಟು_STATE(ksCurSC);
    ಪ್ರತಿಜ್ಞಾ(!is_ಸುತ್ತು_Robin(sc));
    REFILL_SANITY_ಆರಂಭ(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t ತಲೆ = *refill_ತಲೆ(sc);
    ಆದರೆ (usage > 0 && ತಲೆ.r_ಸಮಯ < MAX_RELEASE_ಸಮಯ) {
        ಪ್ರತಿಜ್ಞಾ(ತಲೆ.rAmount > usage);
        refill_t ಬಳಸಿದ_2 = (refill_t) {
            .rAmount = usage,
            .r_ಸಮಯ = ತಲೆ.r_ಸಮಯ + sc->scPeriod,
        };

        refill_ತಲೆ(sc)->rAmount = ತಲೆ.rAmount - usage;
        /* We need to keep the head refill no more than a period before
         * the start of the tail refill. This ensures that new refills
         * are never added before the tail refill (breaking the ordered
         * invariant). This code actually keeps the head refill no more
         * than a period before the end of the tail refill (which is
         * stronger than necessary) but is what is used in the current
         * proofs. In combination with the merging behaviour of
         * schedule_used, the following will still ensure that
         * incremental charging of a refill across preemptions only
         * produces a single new refill one period in the future. */
        refill_ತಲೆ(sc)->r_ಸಮಯ = ತಲೆ.r_ಸಮಯ + usage;
        ವೇಳಾಪಟ್ಟಿ_ಬಳಸಿದ(sc, ಬಳಸಿದ_2);
    }

    /* Ensure the head refill has the minimum budget */
    ಆಗ (ತಲೆ_refill_insufficient(sc)) {
        ವಿಲೀನ_nonoverlapping_ತಲೆ_refill(sc);
    }

    REFILL_SANITY_ಕೊನೆ(sc);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ವಿಲೀನ_overlapping_ತಲೆ_refill(sched_context_t *sc)
{
    refill_t old_ತಲೆ = refill_ತೆಗೆ_ತಲೆ(sc);
    refill_ತಲೆ(sc)->r_ಸಮಯ = old_ತಲೆ.r_ಸಮಯ;
    refill_ತಲೆ(sc)->rAmount += old_ತಲೆ.rAmount;
}

ಶೂನ್ಯ refill_unblock_check(sched_context_t *sc)
{

    ಆದರೆ (is_ಸುತ್ತು_Robin(sc)) {
        /* nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_ಆರಂಭ(sc);
    ಆದರೆ (refill_ready(sc)) {
        refill_ತಲೆ(sc)->r_ಸಮಯ = ಗಂಟು_STATE(ks_Cur_ಸಮಯ);
        ಗಂಟು_STATE(ksReprogram) = true;

        /* merge available replenishments */
        ಆಗ (refill_ತಲೆ_overlapping(sc)) {
            ವಿಲೀನ_overlapping_ತಲೆ_refill(sc);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_ಕೊನೆ(sc);
}
