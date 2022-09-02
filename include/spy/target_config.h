#ifndef _SPY_TARGET_CONFIG_H
#define _SPY_TARGET_CONFIG_H

#include "../../lib/types.h"

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
	u16 revision;

	char name[SPY_TARGET_NAME_MAXLEN+1];
	u32 id; /* set by the driver */
	u32 flags;

	u32 cpu_set_size;
	u32 num_memory_regions;
	u32 num_cache_regions;

	u64 cpu_reset_address;
	u64 msg_reply_timeout;
} __attribute__((packed));

#endif /* !_SPY_TARGET_CONFIG_H */
