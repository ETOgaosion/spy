#ifndef __SPY_CONFIG__
#define __SPY_CONFIG__

#include "target_config.h"
#include "../../lib/types.h"

#define SPY_MEM_READ		0x0001
#define SPY_MEM_WRITE		0x0002
#define SPY_MEM_EXECUTE		0x0004
#define SPY_MEM_DMA		0x0008
#define SPY_MEM_IO		0x0010
#define SPY_MEM_COMM_REGION	0x0020
#define SPY_MEM_LOADABLE		0x0040
#define SPY_MEM_ROOTSHARED	0x0080
#define SPY_MEM_NO_HUGEPAGES	0x0100
#define SPY_MEM_IO_UNALIGNED	0x8000
#define SPY_MEM_IO_WIDTH_SHIFT	16 /* uses bits 16..19 */
#define SPY_MEM_IO_8		(1 << SPY_MEM_IO_WIDTH_SHIFT)
#define SPY_MEM_IO_16		(2 << SPY_MEM_IO_WIDTH_SHIFT)
#define SPY_MEM_IO_32		(4 << SPY_MEM_IO_WIDTH_SHIFT)
#define SPY_MEM_IO_64		(8 << SPY_MEM_IO_WIDTH_SHIFT)

#define SPY_MEMORY_IS_SUBPAGE(mem)	\
	((mem)->virt_start & PAGE_OFFS_MASK || (mem)->size & PAGE_OFFS_MASK)

#define SPY_CACHE_L3_CODE		0x01
#define SPY_CACHE_L3_DATA		0x02
#define SPY_CACHE_L3		(SPY_CACHE_L3_CODE | \
					 SPY_CACHE_L3_DATA)

#define SPY_CACHE_ROOTSHARED	0x0001

struct spy_cache {
	u32 start;
	u32 size;
	u8 type;
	u8 padding;
	u16 flags;
} __attribute__((packed));

struct spy_irqchip {
	u64 address;
	u32 id;
	u32 pin_base;
	u32 pin_bitmap[4];
} __attribute__((packed));

struct spy_memory {
	u64 phys_start;
	u64 virt_start;
	u64 size;
	u64 flags;
} __attribute__((packed));

/**
 * General descriptor of the system.
 */
struct spy_system {
	char signature[6];
	u16 revision;
    u32 flags;

	/** Spy's location in memory */
	struct spy_memory hypervisor_memory;
	struct spy_target_desc root_target;
} __attribute__((packed));

#endif