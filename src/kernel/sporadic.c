/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <object/structures.h>

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
ساكن مضمن لفظ_t refill_ايندڙ(sched_context_t *sc, لفظ_t اشاريو)
{
    أرجع (اشاريو == sc->scRefillMax - 1u) ? (0) : اشاريو + 1u;
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
/* for debugging */
اڻ_استعمال_ٿيل ساكن مضمن فراغ print_اشاريو(sched_context_t *sc, لفظ_t اشاريو)
{

    لکو("index %lu, Amount: %llx, time %llx\n", اشاريو, refill_اشاريو(sc, اشاريو)->rAmount,
           refill_اشاريو(sc, اشاريو)->r_وقت);
}

اڻ_استعمال_ٿيل ساكن مضمن فراغ refill_print(sched_context_t *sc)
{
    لکو("Head %lu tail %lu\n", sc->sc_Refill_سر, sc->sc_Refill_پڇ);
    لفظ_t current = sc->sc_Refill_سر;
    /* always print the head */
    print_اشاريو(sc, current);

    طالما (current != sc->sc_Refill_پڇ) {
        current = refill_ايندڙ(sc, current);
        print_اشاريو(sc, current);
    }

}
#جيڪڏهن_پورو /* CONFIG_PRINTING */
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ساكن اڻ_استعمال_ٿيل bool_t refill_ordered(sched_context_t *sc)
{
    إذا (is_ڦيرو_Robin(sc)) {
        أرجع true;
    }

    لفظ_t current = sc->sc_Refill_سر;
    لفظ_t ايندڙ = refill_ايندڙ(sc, sc->sc_Refill_سر);

    طالما (current != sc->sc_Refill_پڇ) {
        إذا (!(refill_اشاريو(sc, current)->r_وقت + refill_اشاريو(sc, current)->rAmount <= refill_اشاريو(sc, ايندڙ)->r_وقت)) {
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
            refill_print(sc);
#جيڪڏهن_پورو
            أرجع false;
        }
        current = ايندڙ;
        ايندڙ = refill_ايندڙ(sc, current);
    }

    أرجع true;
}

#وصف_ڪريو REFILL_SANITY_شروعات(sc) ticks_t جوڙ = refill_جوڙ(sc); أكد(is_ڦيرو_Robin(sc) || refill_ordered(sc));
#وصف_ڪريو REFILL_SANITY_CHECK(sc, budget) \
    افعل { \
        أكد(refill_جوڙ(sc) == budget); أكد(is_ڦيرو_Robin(sc) || refill_ordered(sc)); \
    } طالما (0)

#وصف_ڪريو REFILL_SANITY_پڇاڙي(sc) \
    افعل {\
        REFILL_SANITY_CHECK(sc, جوڙ);\
    } طالما (0)
#وإلا
#وصف_ڪريو REFILL_SANITY_شروعات(sc)
#وصف_ڪريو REFILL_SANITY_CHECK(sc, budget)
#وصف_ڪريو REFILL_SANITY_پڇاڙي(sc)
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ساكن اڻ_استعمال_ٿيل ticks_t refill_جوڙ(sched_context_t *sc)
{
    ticks_t جوڙ_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_پڇ) {
        current = refill_ايندڙ(sc, current);
        جوڙ_2 += refill_اشاريو(sc, current)->rAmount;
    }

    أرجع جوڙ_2;
}

/* pop head of refill queue */
ساكن مضمن refill_t refill_ڪڍو_سر(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    أكد(!refill_single(sc));

    اڻ_استعمال_ٿيل لفظ_t prev_سائيز = refill_سائيز(sc);
    refill_t refill = *refill_سر(sc);
    sc->sc_Refill_سر = refill_ايندڙ(sc, sc->sc_Refill_سر);

    /* sanity */
    أكد(prev_سائيز == (refill_سائيز(sc) + 1));
    أكد(sc->sc_Refill_سر < sc->scRefillMax);
    أرجع refill;
}

/* add item to tail of refill queue */
ساكن مضمن فراغ refill_جوڙيو_پڇ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    أكد(refill_سائيز(sc) < sc->scRefillMax);

    لفظ_t new_پڇ = refill_ايندڙ(sc, sc->sc_Refill_پڇ);
    sc->sc_Refill_پڇ = new_پڇ;
    *refill_پڇ(sc) = refill;

    /* sanity */
    أكد(new_پڇ < sc->scRefillMax);
}

ساكن مضمن فراغ maybe_جوڙيو_خالي_جايون_پڇ(sched_context_t *sc)
{
    إذا (is_ڦيرو_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t خالي_جايون_پڇ = { .r_وقت = refill_سر(sc)->r_وقت };
        refill_جوڙيو_پڇ(sc, خالي_جايون_پڇ);
        أكد(refill_سائيز(sc) == MIN_REFILLS);
    }
}

فراغ refill_new(sched_context_t *sc, لفظ_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_سر = 0;
    sc->sc_Refill_پڇ = 0;
    sc->scRefillMax = max_refills;
    أكد(budget >= MIN_BUDGET);
    /* full budget available */
    refill_سر(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_سر(sc)->r_وقت = ڳنڍ_STATE(ks_Cur_وقت);
    maybe_جوڙيو_خالي_جايون_پڇ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

فراغ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, لفظ_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    أكد(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_اشاريو(sc, 0) = *refill_سر(sc);
    sc->sc_Refill_سر = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_پڇ = sc->sc_Refill_سر;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_وقت = ڳنڍ_STATE(ks_Cur_وقت);
    }

    إذا (refill_سر(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_سر(sc)->rAmount = new_budget;
        maybe_جوڙيو_خالي_جايون_پڇ(sc);
    } وإلا {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_سر(sc)->rAmount),
                         .r_وقت = refill_سر(sc)->r_وقت + new_period
                       };
        refill_جوڙيو_پڇ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ساكن مضمن فراغ وقت_جدول_استعمال_ٿيل(sched_context_t *sc, refill_t new)
{
    refill_t پڇ = *refill_پڇ(sc);
    إذا (unlikely(پڇ.r_وقت + پڇ.rAmount >= new.r_وقت)) {
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
        refill_پڇ(sc)->rAmount = پڇ.rAmount + new.rAmount;
    } وإلا إذا (likely(!refill_ڀريل(sc))) {
        /* Add tail normally */
        refill_جوڙيو_پڇ(sc, new);
    } وإلا {
        /* Delay existing tail to merge */
        refill_پڇ(sc)->r_وقت = new.r_وقت - پڇ.rAmount;
        refill_پڇ(sc)->rAmount = پڇ.rAmount + new.rAmount;
    }
}

ساكن bool_t refill_سر_overlapping(sched_context_t *sc)
{
    إذا (!refill_single(sc)) {
        refill_t سر = *refill_سر(sc);
        ticks_t پڇ = سر.r_وقت + سر.rAmount;
        أرجع refill_اشاريو(sc, refill_ايندڙ(sc, sc->sc_Refill_سر))->r_وقت <= پڇ;
    } وإلا {
        أرجع false;
    }
}

ساكن مضمن bool_t سر_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);
    أرجع سر.rAmount <= usage && سر.r_وقت < MAX_RELEASE_وقت;
}

ساكن مضمن ticks_t charge_entire_سر_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);

    إذا (refill_single(sc)) {
        refill_سر(sc)->r_وقت = سر.r_وقت + sc->scPeriod;
    } وإلا {
        refill_t old_سر = refill_ڪڍو_سر(sc);
        old_سر.r_وقت += sc->scPeriod;
        وقت_جدول_استعمال_ٿيل(sc, old_سر);
    }

    أرجع usage - سر.rAmount;
}

ساكن مضمن ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    طالما (سر_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_سر_refill(sc, current_usage);
    }

    أرجع current_usage;
}

ساكن مضمن bool_t سر_refill_insufficient(sched_context_t *sc)
{
    أرجع refill_سر(sc)->rAmount < MIN_BUDGET;
}

ساكن مضمن فراغ ملايو_nonoverlapping_سر_refill(sched_context_t *sc)
{
    refill_t سر = refill_ڪڍو_سر(sc);
    refill_سر(sc)->rAmount += سر.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_سر(sc)->r_وقت -= سر.rAmount;
}

فراغ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ڳنڍ_STATE(ksCurSC);
    أكد(!is_ڦيرو_Robin(sc));
    REFILL_SANITY_شروعات(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t سر = *refill_سر(sc);
    إذا (usage > 0 && سر.r_وقت < MAX_RELEASE_وقت) {
        أكد(سر.rAmount > usage);
        refill_t استعمال_ٿيل_2 = (refill_t) {
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
        وقت_جدول_استعمال_ٿيل(sc, استعمال_ٿيل_2);
    }

    /* Ensure the head refill has the minimum budget */
    طالما (سر_refill_insufficient(sc)) {
        ملايو_nonoverlapping_سر_refill(sc);
    }

    REFILL_SANITY_پڇاڙي(sc);
}

ساكن مضمن فراغ ملايو_overlapping_سر_refill(sched_context_t *sc)
{
    refill_t old_سر = refill_ڪڍو_سر(sc);
    refill_سر(sc)->r_وقت = old_سر.r_وقت;
    refill_سر(sc)->rAmount += old_سر.rAmount;
}

فراغ refill_unblock_check(sched_context_t *sc)
{

    إذا (is_ڦيرو_Robin(sc)) {
        /* nothing to do */
        أرجع;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_شروعات(sc);
    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_وقت = ڳنڍ_STATE(ks_Cur_وقت);
        ڳنڍ_STATE(ksReprogram) = true;

        /* merge available replenishments */
        طالما (refill_سر_overlapping(sc)) {
            ملايو_overlapping_سر_refill(sc);
        }

        أكد(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_پڇاڙي(sc);
}
