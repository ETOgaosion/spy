/*
 * Spy, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2020
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#ifndef _SPY_BITOPS_H
#define _SPY_BITOPS_H

#include <spy/types.h>
#include <asm/bitops.h>

/* create 64-bit mask with bytes 0 to size-1 set to 0xff */
#define BYTE_MASK(size)		(0xffffffffffffffffULL >> ((8 - (size)) * 8))

/* create 64-bit mask with all bits in [last:first] set */
#define BIT_MASK(last, first) \
	((0xffffffffffffffffULL >> (64 - ((last) + 1 - (first)))) << (first))

static inline __attribute__((always_inline)) void
clear_bit(unsigned int nr, volatile unsigned long *addr) {
	addr[nr / BITS_PER_LONG] &= ~(1UL << (nr % BITS_PER_LONG));
}

static inline __attribute__((always_inline)) void
set_bit(unsigned int nr, volatile unsigned long *addr) {
	addr[nr / BITS_PER_LONG] |= 1UL << (nr % BITS_PER_LONG);
}

#endif /* !_SPY_BITOPS_H */
