/*
 * Spy, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2014-2018
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * Partly derived from Linux kernel code.
 */

/*
 * We need guards around ARRAY_SIZE as there is a duplicate definition in
 * spy/target-config.h due to header license incompatibility. Once
 * ARRAY_SIZE is replaced in target-config.h, this guard can be removed.
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))
#endif

#define BUG()			*(int *)0 = 0xdead

/* sizeof() for a structure/union field */
#define FIELD_SIZEOF(type, fld)	(sizeof(((type *)0)->fld))

/* extract the field value at [last:first] from an input of up to 64 bits */
#define GET_FIELD(value, last, first) \
	(((value) & BIT_MASK((last), (first))) >> (first))

/* set the field value at [last:first] from an input of up to 64 bits*/
#define SET_FIELD(value, last, first) \
	((value) << (first) & BIT_MASK((last), (first)))

#define MAX(a, b)		((a) >= (b) ? (a) : (b))
#define MIN(a, b)		((a) <= (b) ? (a) : (b))
