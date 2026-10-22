/*
 * Copyright 2025, UNSW
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <machine/assembler.h>

.खण्डः .text, "ax"
.code32

BEGIN_FUNC(idle_thread)
    1: hlt
    कूर्द 1b
END_FUNC(idle_thread)
