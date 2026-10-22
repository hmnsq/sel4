/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>

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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t refill_ਅਗਲਾ(sched_context_t *sc, ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ)
{
    ਵਾਪਸ (ਸੂਚਕਾਂਕ == sc->scRefillMax - 1u) ? (0) : ਸੂਚਕਾਂਕ + 1u;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
/* for debugging */
ਅਣਵਰਤਿਆ ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ print_ਸੂਚਕਾਂਕ(sched_context_t *sc, ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ)
{

    ਲਿਖੋ("index %lu, Amount: %llx, time %llx\n", ਸੂਚਕਾਂਕ, refill_ਸੂਚਕਾਂਕ(sc, ਸੂਚਕਾਂਕ)->rAmount,
           refill_ਸੂਚਕਾਂਕ(sc, ਸੂਚਕਾਂਕ)->r_ਸਮਾਂ);
}

ਅਣਵਰਤਿਆ ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ refill_print(sched_context_t *sc)
{
    ਲਿਖੋ("Head %lu tail %lu\n", sc->sc_Refill_ਸਿਰ, sc->sc_Refill_ਪੂਛ);
    ਸ਼ਬਦ_t current = sc->sc_Refill_ਸਿਰ;
    /* always print the head */
    print_ਸੂਚਕਾਂਕ(sc, current);

    ਜਦੋਂ (current != sc->sc_Refill_ਪੂਛ) {
        current = refill_ਅਗਲਾ(sc, current);
        print_ਸੂਚਕਾਂਕ(sc, current);
    }

}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ਸਥਿਰ ਅਣਵਰਤਿਆ bool_t refill_ordered(sched_context_t *sc)
{
    ਜੇ (is_ਗੇੜਾ_Robin(sc)) {
        ਵਾਪਸ true;
    }

    ਸ਼ਬਦ_t current = sc->sc_Refill_ਸਿਰ;
    ਸ਼ਬਦ_t ਅਗਲਾ = refill_ਅਗਲਾ(sc, sc->sc_Refill_ਸਿਰ);

    ਜਦੋਂ (current != sc->sc_Refill_ਪੂਛ) {
        ਜੇ (!(refill_ਸੂਚਕਾਂਕ(sc, current)->r_ਸਮਾਂ + refill_ਸੂਚਕਾਂਕ(sc, current)->rAmount <= refill_ਸੂਚਕਾਂਕ(sc, ਅਗਲਾ)->r_ਸਮਾਂ)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
            refill_print(sc);
#ਜੇ_ਅੰਤ
            ਵਾਪਸ false;
        }
        current = ਅਗਲਾ;
        ਅਗਲਾ = refill_ਅਗਲਾ(sc, current);
    }

    ਵਾਪਸ true;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_ਸ਼ੁਰੂ(sc) ticks_t ਜੋੜ = refill_ਜੋੜ(sc); ਪ੍ਰਤਿਜ੍ਞਾ(is_ਗੇੜਾ_Robin(sc) || refill_ordered(sc));
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_CHECK(sc, budget) \
    ਕਰੋ { \
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ਜੋੜ(sc) == budget); ਪ੍ਰਤਿਜ੍ਞਾ(is_ਗੇੜਾ_Robin(sc) || refill_ordered(sc)); \
    } ਜਦੋਂ (0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_ਸਿਰਾ(sc) \
    ਕਰੋ {\
        REFILL_SANITY_CHECK(sc, ਜੋੜ);\
    } ਜਦੋਂ (0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_ਸ਼ੁਰੂ(sc)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_CHECK(sc, budget)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REFILL_SANITY_ਸਿਰਾ(sc)
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ਸਥਿਰ ਅਣਵਰਤਿਆ ticks_t refill_ਜੋੜ(sched_context_t *sc)
{
    ticks_t ਜੋੜ_2 = refill_ਸਿਰ(sc)->rAmount;
    ਸ਼ਬਦ_t current = sc->sc_Refill_ਸਿਰ;

    ਜਦੋਂ (current != sc->sc_Refill_ਪੂਛ) {
        current = refill_ਅਗਲਾ(sc, current);
        ਜੋੜ_2 += refill_ਸੂਚਕਾਂਕ(sc, current)->rAmount;
    }

    ਵਾਪਸ ਜੋੜ_2;
}

/* pop head of refill queue */
ਸਥਿਰ ਸਤਰਅੰਦਰ refill_t refill_ਕੱਢੋ_ਸਿਰ(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    ਪ੍ਰਤਿਜ੍ਞਾ(!refill_single(sc));

    ਅਣਵਰਤਿਆ ਸ਼ਬਦ_t prev_ਮਾਪ = refill_ਮਾਪ(sc);
    refill_t refill = *refill_ਸਿਰ(sc);
    sc->sc_Refill_ਸਿਰ = refill_ਅਗਲਾ(sc, sc->sc_Refill_ਸਿਰ);

    /* sanity */
    ਪ੍ਰਤਿਜ੍ਞਾ(prev_ਮਾਪ == (refill_ਮਾਪ(sc) + 1));
    ਪ੍ਰਤਿਜ੍ਞਾ(sc->sc_Refill_ਸਿਰ < sc->scRefillMax);
    ਵਾਪਸ refill;
}

/* add item to tail of refill queue */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ refill_ਜੋੜੋ_ਪੂਛ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_ਮਾਪ(sc) < sc->scRefillMax);

    ਸ਼ਬਦ_t new_ਪੂਛ = refill_ਅਗਲਾ(sc, sc->sc_Refill_ਪੂਛ);
    sc->sc_Refill_ਪੂਛ = new_ਪੂਛ;
    *refill_ਪੂਛ(sc) = refill;

    /* sanity */
    ਪ੍ਰਤਿਜ੍ਞਾ(new_ਪੂਛ < sc->scRefillMax);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ maybe_ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੂਛ(sched_context_t *sc)
{
    ਜੇ (is_ਗੇੜਾ_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t ਖਾਲੀ_ਥਾਵਾਂ_ਪੂਛ = { .r_ਸਮਾਂ = refill_ਸਿਰ(sc)->r_ਸਮਾਂ };
        refill_ਜੋੜੋ_ਪੂਛ(sc, ਖਾਲੀ_ਥਾਵਾਂ_ਪੂਛ);
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ਮਾਪ(sc) == MIN_REFILLS);
    }
}

ਖਾਲੀ refill_new(sched_context_t *sc, ਸ਼ਬਦ_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_ਸਿਰ = 0;
    sc->sc_Refill_ਪੂਛ = 0;
    sc->scRefillMax = max_refills;
    ਪ੍ਰਤਿਜ੍ਞਾ(budget >= MIN_BUDGET);
    /* full budget available */
    refill_ਸਿਰ(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_ਸਿਰ(sc)->r_ਸਮਾਂ = ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ);
    maybe_ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੂਛ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

ਖਾਲੀ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, ਸ਼ਬਦ_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    ਪ੍ਰਤਿਜ੍ਞਾ(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_ਸੂਚਕਾਂਕ(sc, 0) = *refill_ਸਿਰ(sc);
    sc->sc_Refill_ਸਿਰ = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_ਪੂਛ = sc->sc_Refill_ਸਿਰ;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    ਜੇ (refill_ready(sc)) {
        refill_ਸਿਰ(sc)->r_ਸਮਾਂ = ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ);
    }

    ਜੇ (refill_ਸਿਰ(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_ਸਿਰ(sc)->rAmount = new_budget;
        maybe_ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_ਪੂਛ(sc);
    } ਨਹੀਂਤਾਂ {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_ਸਿਰ(sc)->rAmount),
                         .r_ਸਮਾਂ = refill_ਸਿਰ(sc)->r_ਸਮਾਂ + new_period
                       };
        refill_ਜੋੜੋ_ਪੂਛ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਵਰਤਿਆ(sched_context_t *sc, refill_t new)
{
    refill_t ਪੂਛ = *refill_ਪੂਛ(sc);
    ਜੇ (unlikely(ਪੂਛ.r_ਸਮਾਂ + ਪੂਛ.rAmount >= new.r_ਸਮਾਂ)) {
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
        refill_ਪੂਛ(sc)->rAmount = ਪੂਛ.rAmount + new.rAmount;
    } ਨਹੀਂਤਾਂ ਜੇ (likely(!refill_ਭਰਿਆ(sc))) {
        /* Add tail normally */
        refill_ਜੋੜੋ_ਪੂਛ(sc, new);
    } ਨਹੀਂਤਾਂ {
        /* Delay existing tail to merge */
        refill_ਪੂਛ(sc)->r_ਸਮਾਂ = new.r_ਸਮਾਂ - ਪੂਛ.rAmount;
        refill_ਪੂਛ(sc)->rAmount = ਪੂਛ.rAmount + new.rAmount;
    }
}

ਸਥਿਰ bool_t refill_ਸਿਰ_overlapping(sched_context_t *sc)
{
    ਜੇ (!refill_single(sc)) {
        refill_t ਸਿਰ = *refill_ਸਿਰ(sc);
        ticks_t ਪੂਛ = ਸਿਰ.r_ਸਮਾਂ + ਸਿਰ.rAmount;
        ਵਾਪਸ refill_ਸੂਚਕਾਂਕ(sc, refill_ਅਗਲਾ(sc, sc->sc_Refill_ਸਿਰ))->r_ਸਮਾਂ <= ਪੂਛ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ false;
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਸਿਰ_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t ਸਿਰ = *refill_ਸਿਰ(sc);
    ਵਾਪਸ ਸਿਰ.rAmount <= usage && ਸਿਰ.r_ਸਮਾਂ < MAX_RELEASE_ਸਮਾਂ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ticks_t charge_entire_ਸਿਰ_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t ਸਿਰ = *refill_ਸਿਰ(sc);

    ਜੇ (refill_single(sc)) {
        refill_ਸਿਰ(sc)->r_ਸਮਾਂ = ਸਿਰ.r_ਸਮਾਂ + sc->scPeriod;
    } ਨਹੀਂਤਾਂ {
        refill_t old_ਸਿਰ = refill_ਕੱਢੋ_ਸਿਰ(sc);
        old_ਸਿਰ.r_ਸਮਾਂ += sc->scPeriod;
        ਸਮਾਂ_ਸਾਰਣੀ_ਵਰਤਿਆ(sc, old_ਸਿਰ);
    }

    ਵਾਪਸ usage - ਸਿਰ.rAmount;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    ਜਦੋਂ (ਸਿਰ_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_ਸਿਰ_refill(sc, current_usage);
    }

    ਵਾਪਸ current_usage;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਸਿਰ_refill_insufficient(sched_context_t *sc)
{
    ਵਾਪਸ refill_ਸਿਰ(sc)->rAmount < MIN_BUDGET;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਮਿਲਾਓ_nonoverlapping_ਸਿਰ_refill(sched_context_t *sc)
{
    refill_t ਸਿਰ = refill_ਕੱਢੋ_ਸਿਰ(sc);
    refill_ਸਿਰ(sc)->rAmount += ਸਿਰ.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_ਸਿਰ(sc)->r_ਸਮਾਂ -= ਸਿਰ.rAmount;
}

ਖਾਲੀ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ਗੰਢ_STATE(ksCurSC);
    ਪ੍ਰਤਿਜ੍ਞਾ(!is_ਗੇੜਾ_Robin(sc));
    REFILL_SANITY_ਸ਼ੁਰੂ(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t ਸਿਰ = *refill_ਸਿਰ(sc);
    ਜੇ (usage > 0 && ਸਿਰ.r_ਸਮਾਂ < MAX_RELEASE_ਸਮਾਂ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਿਰ.rAmount > usage);
        refill_t ਵਰਤਿਆ_2 = (refill_t) {
            .rAmount = usage,
            .r_ਸਮਾਂ = ਸਿਰ.r_ਸਮਾਂ + sc->scPeriod,
        };

        refill_ਸਿਰ(sc)->rAmount = ਸਿਰ.rAmount - usage;
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
        refill_ਸਿਰ(sc)->r_ਸਮਾਂ = ਸਿਰ.r_ਸਮਾਂ + usage;
        ਸਮਾਂ_ਸਾਰਣੀ_ਵਰਤਿਆ(sc, ਵਰਤਿਆ_2);
    }

    /* Ensure the head refill has the minimum budget */
    ਜਦੋਂ (ਸਿਰ_refill_insufficient(sc)) {
        ਮਿਲਾਓ_nonoverlapping_ਸਿਰ_refill(sc);
    }

    REFILL_SANITY_ਸਿਰਾ(sc);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਮਿਲਾਓ_overlapping_ਸਿਰ_refill(sched_context_t *sc)
{
    refill_t old_ਸਿਰ = refill_ਕੱਢੋ_ਸਿਰ(sc);
    refill_ਸਿਰ(sc)->r_ਸਮਾਂ = old_ਸਿਰ.r_ਸਮਾਂ;
    refill_ਸਿਰ(sc)->rAmount += old_ਸਿਰ.rAmount;
}

ਖਾਲੀ refill_unblock_check(sched_context_t *sc)
{

    ਜੇ (is_ਗੇੜਾ_Robin(sc)) {
        /* nothing to do */
        ਵਾਪਸ;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_ਸ਼ੁਰੂ(sc);
    ਜੇ (refill_ready(sc)) {
        refill_ਸਿਰ(sc)->r_ਸਮਾਂ = ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ);
        ਗੰਢ_STATE(ksReprogram) = true;

        /* merge available replenishments */
        ਜਦੋਂ (refill_ਸਿਰ_overlapping(sc)) {
            ਮਿਲਾਓ_overlapping_ਸਿਰ_refill(sc);
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_ਸਿਰਾ(sc);
}
