#ifndef _SPY_TARGET_CONFIG_H
#define _SPY_TARGET_CONFIG_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#endif

/*
 * Incremented on any layout or semantic change of system or target config.
 * Also update formats and HEADER_REVISION in pyspy/config_parser.py.
 */
#define SPY_CONFIG_REVISION	13

#define SPY_TARGET_NAME_MAXLEN	31

#define SPY_TARGET_PASSIVE_COMMREG	0x00000001
#define SPY_TARGET_TEST_DEVICE	0x00000002

#define SPY_TARGET_DESC_SIGNATURE	"JHTARGET"

/**
 * The spy target configuration.
 *
 * @note Keep Config._HEADER_FORMAT in spy-target-linux in sync with this
 * structure.
 */
struct spy_target_desc {
	char signature[6];
	__u16 revision;

	char name[SPY_TARGET_NAME_MAXLEN+1];
	__u32 id; /* set by the driver */
	__u32 flags;

	__u32 cpu_set_size;
	__u32 num_memory_regions;
	__u32 num_cache_regions;
	__u32 num_irqchips;
	__u32 num_pio_regions;
	__u32 num_stream_ids;

	__u64 cpu_reset_address;
	__u64 msg_reply_timeout;

	struct spy_console console;
} __attribute__((packed));

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

struct spy_memory {
	__u64 phys_start;
	__u64 virt_start;
	__u64 size;
	__u64 flags;
} __attribute__((packed));

#define SPY_MEMORY_IS_SUBPAGE(mem)	\
	((mem)->virt_start & PAGE_OFFS_MASK || (mem)->size & PAGE_OFFS_MASK)

#define SPY_CACHE_L3_CODE		0x01
#define SPY_CACHE_L3_DATA		0x02
#define SPY_CACHE_L3		(SPY_CACHE_L3_CODE | \
					 SPY_CACHE_L3_DATA)

#define SPY_CACHE_ROOTSHARED	0x0001

struct spy_cache {
	__u32 start;
	__u32 size;
	__u8 type;
	__u8 padding;
	__u16 flags;
} __attribute__((packed));

struct spy_irqchip {
	__u64 address;
	__u32 id;
	__u32 pin_base;
	__u32 pin_bitmap[4];
} __attribute__((packed));

/**
 * General descriptor of the system.
 */
struct spy_system {
	char signature[6];
	__u16 revision;
	__u32 flags;

	/** Spy's location in memory */
	struct spy_memory hypervisor_memory;
	struct {
        struct {
            __u16 pm_timer_address;
        } __attribute__((packed)) x86;
	} __attribute__((packed)) platform_info;
	struct spy_target_desc root_target;
} __attribute__((packed));

static inline __u32
spy_target_config_size(struct spy_target_desc *target) {
	return sizeof(struct spy_target_desc) +
		target->cpu_set_size +
		target->num_memory_regions * sizeof(struct spy_memory) +
		target->num_cache_regions * sizeof(struct spy_cache) +
		target->num_irqchips * sizeof(struct spy_irqchip);
}

static inline __u32
spy_system_config_size(struct spy_system *system) {
	return sizeof(*system) - sizeof(system->root_target) +
		spy_target_config_size(&system->root_target);
}

static inline const unsigned long *
spy_target_cpu_set(const struct spy_target_desc *target) {
	return (const unsigned long *)((const void *)target +
		sizeof(struct spy_target_desc));
}

static inline const struct spy_memory *
spy_target_mem_regions(const struct spy_target_desc *target) {
	return (const struct spy_memory *)
		((void *)spy_target_cpu_set(target) + target->cpu_set_size);
}

static inline const struct spy_cache *
spy_target_cache_regions(const struct spy_target_desc *target) {
	return (const struct spy_cache *)
		((void *)spy_target_mem_regions(target) +
		 target->num_memory_regions * sizeof(struct spy_memory));
}

static inline const struct spy_irqchip *
spy_target_irqchips(const struct spy_target_desc *target) {
	return (const struct spy_irqchip *)
		((void *)spy_target_cache_regions(target) +
		 target->num_cache_regions * sizeof(struct spy_cache));
}

#endif /* !_SPY_TARGET_CONFIG_H */
