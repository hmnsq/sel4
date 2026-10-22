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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t refill_next(sched_context_t *sc, word_t index)
{
    ಪ್ರತ್ಯಾಗಮ (index == sc->scRefillMax - 1u) ? (0) : index + 1u;
}

#ifdef CONFIG_PRINTING
/* for debugging */
UNUSED ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ print_index(sched_context_t *sc, word_t index)
{

    printf("index %lu, Amount: %llx, time %llx\n", index, refill_index(sc, index)->rAmount,
           refill_index(sc, index)->rTime);
}

UNUSED ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ refill_print(sched_context_t *sc)
{
    printf("Head %lu tail %lu\n", sc->scRefillHead, sc->scRefillTail);
    word_t current = sc->scRefillHead;
    /* always print the head */
    print_index(sc, current);

    ಯಾವತ್ (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        print_index(sc, current);
    }

}
#endif /* CONFIG_PRINTING */
#ifdef CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ಸ್ಥಿರ UNUSED bool_t refill_ordered(sched_context_t *sc)
{
    ಯದಿ (isRoundRobin(sc)) {
        ಪ್ರತ್ಯಾಗಮ true;
    }

    word_t current = sc->scRefillHead;
    word_t next = refill_next(sc, sc->scRefillHead);

    ಯಾವತ್ (current != sc->scRefillTail) {
        ಯದಿ (!(refill_index(sc, current)->rTime + refill_index(sc, current)->rAmount <= refill_index(sc, next)->rTime)) {
#ifdef CONFIG_PRINTING
            refill_print(sc);
#endif
            ಪ್ರತ್ಯಾಗಮ false;
        }
        current = next;
        next = refill_next(sc, current);
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

#define REFILL_SANITY_START(sc) ticks_t _sum = refill_sum(sc); ಪ್ರತಿಜ್ಞಾ(isRoundRobin(sc) || refill_ordered(sc));
#define REFILL_SANITY_CHECK(sc, budget) \
    ಕುರು { \
        ಪ್ರತಿಜ್ಞಾ(refill_sum(sc) == budget); ಪ್ರತಿಜ್ಞಾ(isRoundRobin(sc) || refill_ordered(sc)); \
    } ಯಾವತ್ (0)

#define REFILL_SANITY_END(sc) \
    ಕುರು {\
        REFILL_SANITY_CHECK(sc, _sum);\
    } ಯಾವತ್ (0)
#ಅನ್ಯಥಾ
#define REFILL_SANITY_START(sc)
#define REFILL_SANITY_CHECK(sc, budget)
#define REFILL_SANITY_END(sc)
#endif /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ಸ್ಥಿರ UNUSED ticks_t refill_sum(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    ಯಾವತ್ (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        sum += refill_index(sc, current)->rAmount;
    }

    ಪ್ರತ್ಯಾಗಮ sum;
}

/* pop head of refill queue */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ refill_t refill_ತೆಗೆ_head(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    ಪ್ರತಿಜ್ಞಾ(!refill_single(sc));

    UNUSED word_t prev_size = refill_size(sc);
    refill_t refill = *refill_head(sc);
    sc->scRefillHead = refill_next(sc, sc->scRefillHead);

    /* sanity */
    ಪ್ರತಿಜ್ಞಾ(prev_size == (refill_size(sc) + 1));
    ಪ್ರತಿಜ್ಞಾ(sc->scRefillHead < sc->scRefillMax);
    ಪ್ರತ್ಯಾಗಮ refill;
}

/* add item to tail of refill queue */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ refill_add_tail(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    ಪ್ರತಿಜ್ಞಾ(refill_size(sc) < sc->scRefillMax);

    word_t new_tail = refill_next(sc, sc->scRefillTail);
    sc->scRefillTail = new_tail;
    *refill_tail(sc) = refill;

    /* sanity */
    ಪ್ರತಿಜ್ಞಾ(new_tail < sc->scRefillMax);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ maybe_add_empty_tail(sched_context_t *sc)
{
    ಯದಿ (isRoundRobin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t empty_tail = { .rTime = refill_head(sc)->rTime };
        refill_add_tail(sc, empty_tail);
        ಪ್ರತಿಜ್ಞಾ(refill_size(sc) == MIN_REFILLS);
    }
}

ಶೂನ್ಯ refill_new(sched_context_t *sc, word_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->scRefillHead = 0;
    sc->scRefillTail = 0;
    sc->scRefillMax = max_refills;
    ಪ್ರತಿಜ್ಞಾ(budget >= MIN_BUDGET);
    /* full budget available */
    refill_head(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_head(sc)->rTime = ಗಂಟು_STATE(ksCurTime);
    maybe_add_empty_tail(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

ಶೂನ್ಯ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, word_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    ಪ್ರತಿಜ್ಞಾ(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_index(sc, 0) = *refill_head(sc);
    sc->scRefillHead = 0;
    /* truncate refill list to size 1 */
    sc->scRefillTail = sc->scRefillHead;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    ಯದಿ (refill_ready(sc)) {
        refill_head(sc)->rTime = ಗಂಟು_STATE(ksCurTime);
    }

    ಯದಿ (refill_head(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_head(sc)->rAmount = new_budget;
        maybe_add_empty_tail(sc);
    } ಅನ್ಯಥಾ {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_head(sc)->rAmount),
                         .rTime = refill_head(sc)->rTime + new_period
                       };
        refill_add_tail(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ schedule_used(sched_context_t *sc, refill_t new)
{
    refill_t tail = *refill_tail(sc);
    ಯದಿ (unlikely(tail.rTime + tail.rAmount >= new.rTime)) {
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
        refill_tail(sc)->rAmount = tail.rAmount + new.rAmount;
    } ಅನ್ಯಥಾ ಯದಿ (likely(!refill_full(sc))) {
        /* Add tail normally */
        refill_add_tail(sc, new);
    } ಅನ್ಯಥಾ {
        /* Delay existing tail to merge */
        refill_tail(sc)->rTime = new.rTime - tail.rAmount;
        refill_tail(sc)->rAmount = tail.rAmount + new.rAmount;
    }
}

ಸ್ಥಿರ bool_t refill_head_overlapping(sched_context_t *sc)
{
    ಯದಿ (!refill_single(sc)) {
        refill_t head = *refill_head(sc);
        ticks_t tail = head.rTime + head.rAmount;
        ಪ್ರತ್ಯಾಗಮ refill_index(sc, refill_next(sc, sc->scRefillHead))->rTime <= tail;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t head_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);
    ಪ್ರತ್ಯಾಗಮ head.rAmount <= usage && head.rTime < MAX_RELEASE_TIME;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ticks_t charge_entire_head_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);

    ಯದಿ (refill_single(sc)) {
        refill_head(sc)->rTime = head.rTime + sc->scPeriod;
    } ಅನ್ಯಥಾ {
        refill_t old_head = refill_ತೆಗೆ_head(sc);
        old_head.rTime += sc->scPeriod;
        schedule_used(sc, old_head);
    }

    ಪ್ರತ್ಯಾಗಮ usage - head.rAmount;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    ಯಾವತ್ (head_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_head_refill(sc, current_usage);
    }

    ಪ್ರತ್ಯಾಗಮ current_usage;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t head_refill_insufficient(sched_context_t *sc)
{
    ಪ್ರತ್ಯಾಗಮ refill_head(sc)->rAmount < MIN_BUDGET;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ merge_nonoverlapping_head_refill(sched_context_t *sc)
{
    refill_t head = refill_ತೆಗೆ_head(sc);
    refill_head(sc)->rAmount += head.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_head(sc)->rTime -= head.rAmount;
}

ಶೂನ್ಯ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ಗಂಟು_STATE(ksCurSC);
    ಪ್ರತಿಜ್ಞಾ(!isRoundRobin(sc));
    REFILL_SANITY_START(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t head = *refill_head(sc);
    ಯದಿ (usage > 0 && head.rTime < MAX_RELEASE_TIME) {
        ಪ್ರತಿಜ್ಞಾ(head.rAmount > usage);
        refill_t used = (refill_t) {
            .rAmount = usage,
            .rTime = head.rTime + sc->scPeriod,
        };

        refill_head(sc)->rAmount = head.rAmount - usage;
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
        refill_head(sc)->rTime = head.rTime + usage;
        schedule_used(sc, used);
    }

    /* Ensure the head refill has the minimum budget */
    ಯಾವತ್ (head_refill_insufficient(sc)) {
        merge_nonoverlapping_head_refill(sc);
    }

    REFILL_SANITY_END(sc);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ merge_overlapping_head_refill(sched_context_t *sc)
{
    refill_t old_head = refill_ತೆಗೆ_head(sc);
    refill_head(sc)->rTime = old_head.rTime;
    refill_head(sc)->rAmount += old_head.rAmount;
}

ಶೂನ್ಯ refill_unblock_check(sched_context_t *sc)
{

    ಯದಿ (isRoundRobin(sc)) {
        /* nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_START(sc);
    ಯದಿ (refill_ready(sc)) {
        refill_head(sc)->rTime = ಗಂಟು_STATE(ksCurTime);
        ಗಂಟು_STATE(ksReprogram) = true;

        /* merge available replenishments */
        ಯಾವತ್ (refill_head_overlapping(sc)) {
            merge_overlapping_head_refill(sc);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_END(sc);
}
