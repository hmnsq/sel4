/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>
#شامل <object/structures.h>

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
ساکن اندرونی لفظ_t refill_اگلا(sched_context_t *sc, لفظ_t اشاریہ)
{
    واپس (اشاریہ == sc->scRefillMax - 1u) ? (0) : اشاریہ + 1u;
}

#اگر_تعریف_شدہ CONFIG_PRINTING
/* for debugging */
غیر_مستعمل ساکن اندرونی خالی print_اشاریہ(sched_context_t *sc, لفظ_t اشاریہ)
{

    لکھو("index %lu, Amount: %llx, time %llx\n", اشاریہ, refill_اشاریہ(sc, اشاریہ)->rAmount,
           refill_اشاریہ(sc, اشاریہ)->r_وقت);
}

غیر_مستعمل ساکن اندرونی خالی refill_print(sched_context_t *sc)
{
    لکھو("Head %lu tail %lu\n", sc->sc_Refill_سر, sc->sc_Refill_دم);
    لفظ_t current = sc->sc_Refill_سر;
    /* always print the head */
    print_اشاریہ(sc, current);

    جب (current != sc->sc_Refill_دم) {
        current = refill_اگلا(sc, current);
        print_اشاریہ(sc, current);
    }

}
#اگر_ختم /* CONFIG_PRINTING */
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ساکن غیر_مستعمل bool_t refill_ordered(sched_context_t *sc)
{
    اگر (is_باری_Robin(sc)) {
        واپس true;
    }

    لفظ_t current = sc->sc_Refill_سر;
    لفظ_t اگلا = refill_اگلا(sc, sc->sc_Refill_سر);

    جب (current != sc->sc_Refill_دم) {
        اگر (!(refill_اشاریہ(sc, current)->r_وقت + refill_اشاریہ(sc, current)->rAmount <= refill_اشاریہ(sc, اگلا)->r_وقت)) {
#اگر_تعریف_شدہ CONFIG_PRINTING
            refill_print(sc);
#اگر_ختم
            واپس false;
        }
        current = اگلا;
        اگلا = refill_اگلا(sc, current);
    }

    واپس true;
}

#تعریف_کرو REFILL_SANITY_آغاز(sc) ticks_t مجموع = refill_مجموع(sc); أكد(is_باری_Robin(sc) || refill_ordered(sc));
#تعریف_کرو REFILL_SANITY_CHECK(sc, budget) \
    کرو { \
        أكد(refill_مجموع(sc) == budget); أكد(is_باری_Robin(sc) || refill_ordered(sc)); \
    } جب (0)

#تعریف_کرو REFILL_SANITY_انتہا(sc) \
    کرو {\
        REFILL_SANITY_CHECK(sc, مجموع);\
    } جب (0)
#ورنہ
#تعریف_کرو REFILL_SANITY_آغاز(sc)
#تعریف_کرو REFILL_SANITY_CHECK(sc, budget)
#تعریف_کرو REFILL_SANITY_انتہا(sc)
#اگر_ختم /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ساکن غیر_مستعمل ticks_t refill_مجموع(sched_context_t *sc)
{
    ticks_t مجموع_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    جب (current != sc->sc_Refill_دم) {
        current = refill_اگلا(sc, current);
        مجموع_2 += refill_اشاریہ(sc, current)->rAmount;
    }

    واپس مجموع_2;
}

/* pop head of refill queue */
ساکن اندرونی refill_t refill_نکالو_سر(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    أكد(!refill_single(sc));

    غیر_مستعمل لفظ_t پچھلا_سائز = refill_سائز(sc);
    refill_t refill = *refill_سر(sc);
    sc->sc_Refill_سر = refill_اگلا(sc, sc->sc_Refill_سر);

    /* sanity */
    أكد(پچھلا_سائز == (refill_سائز(sc) + 1));
    أكد(sc->sc_Refill_سر < sc->scRefillMax);
    واپس refill;
}

/* add item to tail of refill queue */
ساکن اندرونی خالی refill_جوڑو_دم(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    أكد(refill_سائز(sc) < sc->scRefillMax);

    لفظ_t new_دم = refill_اگلا(sc, sc->sc_Refill_دم);
    sc->sc_Refill_دم = new_دم;
    *refill_دم(sc) = refill;

    /* sanity */
    أكد(new_دم < sc->scRefillMax);
}

ساکن اندرونی خالی maybe_جوڑو_خالی_خانے_دم(sched_context_t *sc)
{
    اگر (is_باری_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t خالی_خانے_دم = { .r_وقت = refill_سر(sc)->r_وقت };
        refill_جوڑو_دم(sc, خالی_خانے_دم);
        أكد(refill_سائز(sc) == MIN_REFILLS);
    }
}

خالی refill_new(sched_context_t *sc, لفظ_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_سر = 0;
    sc->sc_Refill_دم = 0;
    sc->scRefillMax = max_refills;
    أكد(budget >= MIN_BUDGET);
    /* full budget available */
    refill_سر(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_سر(sc)->r_وقت = گرہ_STATE(ks_Cur_وقت);
    maybe_جوڑو_خالی_خانے_دم(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

خالی refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, لفظ_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    أكد(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_اشاریہ(sc, 0) = *refill_سر(sc);
    sc->sc_Refill_سر = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_دم = sc->sc_Refill_سر;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    اگر (refill_ready(sc)) {
        refill_سر(sc)->r_وقت = گرہ_STATE(ks_Cur_وقت);
    }

    اگر (refill_سر(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_سر(sc)->rAmount = new_budget;
        maybe_جوڑو_خالی_خانے_دم(sc);
    } ورنہ {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_سر(sc)->rAmount),
                         .r_وقت = refill_سر(sc)->r_وقت + new_period
                       };
        refill_جوڑو_دم(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ساکن اندرونی خالی جدول_وقت_مستعمل(sched_context_t *sc, refill_t new)
{
    refill_t دم = *refill_دم(sc);
    اگر (unlikely(دم.r_وقت + دم.rAmount >= new.r_وقت)) {
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
        refill_دم(sc)->rAmount = دم.rAmount + new.rAmount;
    } ورنہ اگر (likely(!refill_بھرا(sc))) {
        /* Add tail normally */
        refill_جوڑو_دم(sc, new);
    } ورنہ {
        /* Delay existing tail to merge */
        refill_دم(sc)->r_وقت = new.r_وقت - دم.rAmount;
        refill_دم(sc)->rAmount = دم.rAmount + new.rAmount;
    }
}

ساکن bool_t refill_سر_overlapping(sched_context_t *sc)
{
    اگر (!refill_single(sc)) {
        refill_t سر = *refill_سر(sc);
        ticks_t دم = سر.r_وقت + سر.rAmount;
        واپس refill_اشاریہ(sc, refill_اگلا(sc, sc->sc_Refill_سر))->r_وقت <= دم;
    } ورنہ {
        واپس false;
    }
}

ساکن اندرونی bool_t سر_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);
    واپس سر.rAmount <= usage && سر.r_وقت < MAX_RELEASE_وقت;
}

ساکن اندرونی ticks_t charge_entire_سر_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);

    اگر (refill_single(sc)) {
        refill_سر(sc)->r_وقت = سر.r_وقت + sc->scPeriod;
    } ورنہ {
        refill_t old_سر = refill_نکالو_سر(sc);
        old_سر.r_وقت += sc->scPeriod;
        جدول_وقت_مستعمل(sc, old_سر);
    }

    واپس usage - سر.rAmount;
}

ساکن اندرونی ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    جب (سر_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_سر_refill(sc, current_usage);
    }

    واپس current_usage;
}

ساکن اندرونی bool_t سر_refill_insufficient(sched_context_t *sc)
{
    واپس refill_سر(sc)->rAmount < MIN_BUDGET;
}

ساکن اندرونی خالی ملاؤ_nonoverlapping_سر_refill(sched_context_t *sc)
{
    refill_t سر = refill_نکالو_سر(sc);
    refill_سر(sc)->rAmount += سر.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_سر(sc)->r_وقت -= سر.rAmount;
}

خالی refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = گرہ_STATE(ksCurSC);
    أكد(!is_باری_Robin(sc));
    REFILL_SANITY_آغاز(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t سر = *refill_سر(sc);
    اگر (usage > 0 && سر.r_وقت < MAX_RELEASE_وقت) {
        أكد(سر.rAmount > usage);
        refill_t مستعمل_2 = (refill_t) {
            .rAmount = usage,
            .r_وقت = سر.r_وقت + sc->scPeriod,
        };

        refill_سر(sc)->rAmount = سر.rAmount - usage;
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
        refill_سر(sc)->r_وقت = سر.r_وقت + usage;
        جدول_وقت_مستعمل(sc, مستعمل_2);
    }

    /* Ensure the head refill has the minimum budget */
    جب (سر_refill_insufficient(sc)) {
        ملاؤ_nonoverlapping_سر_refill(sc);
    }

    REFILL_SANITY_انتہا(sc);
}

ساکن اندرونی خالی ملاؤ_overlapping_سر_refill(sched_context_t *sc)
{
    refill_t old_سر = refill_نکالو_سر(sc);
    refill_سر(sc)->r_وقت = old_سر.r_وقت;
    refill_سر(sc)->rAmount += old_سر.rAmount;
}

خالی refill_unblock_check(sched_context_t *sc)
{

    اگر (is_باری_Robin(sc)) {
        /* nothing to do */
        واپس;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_آغاز(sc);
    اگر (refill_ready(sc)) {
        refill_سر(sc)->r_وقت = گرہ_STATE(ks_Cur_وقت);
        گرہ_STATE(ksReprogram) = true;

        /* merge available replenishments */
        جب (refill_سر_overlapping(sc)) {
            ملاؤ_overlapping_سر_refill(sc);
        }

        أكد(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_انتہا(sc);
}
