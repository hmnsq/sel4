/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <object/structures.h>

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
સ્થિર અંતઃરેખ word_t refill_next(sched_context_t *sc, word_t index)
{
    પાછા (index == sc->scRefillMax - 1u) ? (0) : index + 1u;
}

#ifdef CONFIG_PRINTING
/* for debugging */
UNUSED સ્થિર અંતઃરેખ શૂન્ય print_index(sched_context_t *sc, word_t index)
{

    printf("index %lu, Amount: %llx, time %llx\n", index, refill_index(sc, index)->rAmount,
           refill_index(sc, index)->rTime);
}

UNUSED સ્થિર અંતઃરેખ શૂન્ય refill_print(sched_context_t *sc)
{
    printf("Head %lu tail %lu\n", sc->scRefillHead, sc->scRefillTail);
    word_t current = sc->scRefillHead;
    /* always print the head */
    print_index(sc, current);

    જ્યારે (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        print_index(sc, current);
    }

}
#endif /* CONFIG_PRINTING */
#ifdef CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
સ્થિર UNUSED bool_t refill_ordered(sched_context_t *sc)
{
    જો (isRoundRobin(sc)) {
        પાછા true;
    }

    word_t current = sc->scRefillHead;
    word_t next = refill_next(sc, sc->scRefillHead);

    જ્યારે (current != sc->scRefillTail) {
        જો (!(refill_index(sc, current)->rTime + refill_index(sc, current)->rAmount <= refill_index(sc, next)->rTime)) {
#ifdef CONFIG_PRINTING
            refill_print(sc);
#endif
            પાછા false;
        }
        current = next;
        next = refill_next(sc, current);
    }

    પાછા true;
}

#define REFILL_SANITY_START(sc) ticks_t _sum = refill_sum(sc); પ્રતિજ્ઞા(isRoundRobin(sc) || refill_ordered(sc));
#define REFILL_SANITY_CHECK(sc, budget) \
    કરો { \
        પ્રતિજ્ઞા(refill_sum(sc) == budget); પ્રતિજ્ઞા(isRoundRobin(sc) || refill_ordered(sc)); \
    } જ્યારે (0)

#define REFILL_SANITY_END(sc) \
    કરો {\
        REFILL_SANITY_CHECK(sc, _sum);\
    } જ્યારે (0)
#નહીંતર
#define REFILL_SANITY_START(sc)
#define REFILL_SANITY_CHECK(sc, budget)
#define REFILL_SANITY_END(sc)
#endif /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
સ્થિર UNUSED ticks_t refill_sum(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    જ્યારે (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        sum += refill_index(sc, current)->rAmount;
    }

    પાછા sum;
}

/* pop head of refill queue */
સ્થિર અંતઃરેખ refill_t refill_કાઢો_head(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    પ્રતિજ્ઞા(!refill_single(sc));

    UNUSED word_t prev_size = refill_size(sc);
    refill_t refill = *refill_head(sc);
    sc->scRefillHead = refill_next(sc, sc->scRefillHead);

    /* sanity */
    પ્રતિજ્ઞા(prev_size == (refill_size(sc) + 1));
    પ્રતિજ્ઞા(sc->scRefillHead < sc->scRefillMax);
    પાછા refill;
}

/* add item to tail of refill queue */
સ્થિર અંતઃરેખ શૂન્ય refill_add_tail(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    પ્રતિજ્ઞા(refill_size(sc) < sc->scRefillMax);

    word_t new_tail = refill_next(sc, sc->scRefillTail);
    sc->scRefillTail = new_tail;
    *refill_tail(sc) = refill;

    /* sanity */
    પ્રતિજ્ઞા(new_tail < sc->scRefillMax);
}

સ્થિર અંતઃરેખ શૂન્ય maybe_add_empty_tail(sched_context_t *sc)
{
    જો (isRoundRobin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t empty_tail = { .rTime = refill_head(sc)->rTime };
        refill_add_tail(sc, empty_tail);
        પ્રતિજ્ઞા(refill_size(sc) == MIN_REFILLS);
    }
}

શૂન્ય refill_new(sched_context_t *sc, word_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->scRefillHead = 0;
    sc->scRefillTail = 0;
    sc->scRefillMax = max_refills;
    પ્રતિજ્ઞા(budget >= MIN_BUDGET);
    /* full budget available */
    refill_head(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_head(sc)->rTime = ગાંઠ_STATE(ksCurTime);
    maybe_add_empty_tail(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

શૂન્ય refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, word_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    પ્રતિજ્ઞા(sc->scRefillMax > 0);

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

    જો (refill_ready(sc)) {
        refill_head(sc)->rTime = ગાંઠ_STATE(ksCurTime);
    }

    જો (refill_head(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_head(sc)->rAmount = new_budget;
        maybe_add_empty_tail(sc);
    } નહીંતર {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_head(sc)->rAmount),
                         .rTime = refill_head(sc)->rTime + new_period
                       };
        refill_add_tail(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

સ્થિર અંતઃરેખ શૂન્ય schedule_used(sched_context_t *sc, refill_t new)
{
    refill_t tail = *refill_tail(sc);
    જો (unlikely(tail.rTime + tail.rAmount >= new.rTime)) {
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
    } નહીંતર જો (likely(!refill_full(sc))) {
        /* Add tail normally */
        refill_add_tail(sc, new);
    } નહીંતર {
        /* Delay existing tail to merge */
        refill_tail(sc)->rTime = new.rTime - tail.rAmount;
        refill_tail(sc)->rAmount = tail.rAmount + new.rAmount;
    }
}

સ્થિર bool_t refill_head_overlapping(sched_context_t *sc)
{
    જો (!refill_single(sc)) {
        refill_t head = *refill_head(sc);
        ticks_t tail = head.rTime + head.rAmount;
        પાછા refill_index(sc, refill_next(sc, sc->scRefillHead))->rTime <= tail;
    } નહીંતર {
        પાછા false;
    }
}

સ્થિર અંતઃરેખ bool_t head_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);
    પાછા head.rAmount <= usage && head.rTime < MAX_RELEASE_TIME;
}

સ્થિર અંતઃરેખ ticks_t charge_entire_head_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);

    જો (refill_single(sc)) {
        refill_head(sc)->rTime = head.rTime + sc->scPeriod;
    } નહીંતર {
        refill_t old_head = refill_કાઢો_head(sc);
        old_head.rTime += sc->scPeriod;
        schedule_used(sc, old_head);
    }

    પાછા usage - head.rAmount;
}

સ્થિર અંતઃરેખ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    જ્યારે (head_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_head_refill(sc, current_usage);
    }

    પાછા current_usage;
}

સ્થિર અંતઃરેખ bool_t head_refill_insufficient(sched_context_t *sc)
{
    પાછા refill_head(sc)->rAmount < MIN_BUDGET;
}

સ્થિર અંતઃરેખ શૂન્ય merge_nonoverlapping_head_refill(sched_context_t *sc)
{
    refill_t head = refill_કાઢો_head(sc);
    refill_head(sc)->rAmount += head.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_head(sc)->rTime -= head.rAmount;
}

શૂન્ય refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ગાંઠ_STATE(ksCurSC);
    પ્રતિજ્ઞા(!isRoundRobin(sc));
    REFILL_SANITY_START(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t head = *refill_head(sc);
    જો (usage > 0 && head.rTime < MAX_RELEASE_TIME) {
        પ્રતિજ્ઞા(head.rAmount > usage);
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
    જ્યારે (head_refill_insufficient(sc)) {
        merge_nonoverlapping_head_refill(sc);
    }

    REFILL_SANITY_END(sc);
}

સ્થિર અંતઃરેખ શૂન્ય merge_overlapping_head_refill(sched_context_t *sc)
{
    refill_t old_head = refill_કાઢો_head(sc);
    refill_head(sc)->rTime = old_head.rTime;
    refill_head(sc)->rAmount += old_head.rAmount;
}

શૂન્ય refill_unblock_check(sched_context_t *sc)
{

    જો (isRoundRobin(sc)) {
        /* nothing to do */
        પાછા;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_START(sc);
    જો (refill_ready(sc)) {
        refill_head(sc)->rTime = ગાંઠ_STATE(ksCurTime);
        ગાંઠ_STATE(ksReprogram) = true;

        /* merge available replenishments */
        જ્યારે (refill_head_overlapping(sc)) {
            merge_overlapping_head_refill(sc);
        }

        પ્રતિજ્ઞા(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_END(sc);
}
