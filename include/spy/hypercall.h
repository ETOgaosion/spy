#ifndef _SPY_HYPERCALL_H
#define _SPY_HYPERCALL_H

#include <spy/console.h>

#define SPY_HC_DISABLE			        0
#define SPY_HC_TARGET_CREATE		        1
#define SPY_HC_TARGET_START			    2
#define SPY_HC_TARGET_SET_LOADABLE		    3
#define SPY_HC_TARGET_ELIMINATE		        4
#define SPY_HC_HYPERVISOR_GET_INFO	    5
#define SPY_HC_TARGET_GET_STATE		    6
#define SPY_HC_CPU_GET_INFO		        7
#define SPY_HC_DEBUG_CONSOLE_PUTC		8

/* Hypervisor information type */
#define SPY_INFO_MEM_POOL_SIZE		    0
#define SPY_INFO_MEM_POOL_USED		    1
#define SPY_INFO_REMAP_POOL_SIZE		    2
#define SPY_INFO_REMAP_POOL_USED		    3
#define SPY_INFO_NUM_TARGETS		        4

/* Hypervisor information type */
#define SPY_CPU_INFO_STATE		        0
#define SPY_CPU_INFO_STAT_BASE		    1000

/* CPU state */
#define SPY_CPU_RUNNING			        0
#define SPY_CPU_FAILED			        2 /* terminal state */

/* CPU statistics */
#define SPY_CPU_STAT_VMEXITS_TOTAL	    0
#define SPY_CPU_STAT_VMEXITS_MMIO		1
#define SPY_CPU_STAT_VMEXITS_MANAGEMENT	2
#define SPY_CPU_STAT_VMEXITS_HYPERCALL	3
#define SPY_GENERIC_CPU_STATS		    4

#define SPY_MSG_NONE			            0

/* messages to target */
#define SPY_MSG_SHUTDOWN_REQUEST		    1
#define SPY_MSG_RECONFIG_COMPLETED	    2

/* replies from target */
#define SPY_MSG_UNKNOWN			        1
#define SPY_MSG_REQUEST_DENIED		    2
#define SPY_MSG_REQUEST_APPROVED		    3
#define SPY_MSG_RECEIVED			        4

/* target state, initialized by hypervisor, updated by target */
#define SPY_TARGET_RUNNING			        0
#define SPY_TARGET_RUNNING_LOCKED		    1
#define SPY_TARGET_SHUT_DOWN		        2 /* terminal state */
#define SPY_TARGET_FAILED			        3 /* terminal state */
#define SPY_TARGET_FAILED_COMM_REV		    4 /* terminal state */

/* indicates if inmate may use the Debug Console putc hypercall */
#define SPY_COMM_FLAG_DBG_PUTC_PERMITTED	0x0001
/* indicates if inmate shall use Debug Console putc as output channel */
#define SPY_COMM_FLAG_DBG_PUTC_ACTIVE	0x0002

#define SPY_COMM_HAS_DBG_PUTC_PERMITTED(flags) \
	!!((flags) & SPY_COMM_FLAG_DBG_PUTC_PERMITTED)
#define SPY_COMM_HAS_DBG_PUTC_ACTIVE(flags) \
	!!((flags) & SPY_COMM_FLAG_DBG_PUTC_ACTIVE)

#define COMM_REGION_ABI_REVISION		    2
#define COMM_REGION_MAGIC			    "SPYCOMM"

#define COMM_REGION_GENERIC_HEADER \
	/** Communication region magic JHCOMM */ \
	char signature[6]; \
	/** Communication region ABI revision */ \
	u16 revision; \
	/** Cell state, initialized by hypervisor, updated by target. */ \
	volatile u32 target_state; \
	/** Message code sent from hypervisor to target. */ \
	volatile u32 msg_to_target; \
	/** Reply code sent from target to hypervisor. */ \
	volatile u32 reply_from_target; \
	/** Holds static flags, see SPY_COMM_FLAG_*. */ \
	u32 flags;	 \
	/** Debug console that may be accessed by the inmate. */ \
	struct spy_console console; \
	/** Base address of PCI memory mapped config. */ \
	u64 pci_mmconfig_base;

#include <asm/spy_hypercall.h>

#endif /* !_SPY_HYPERCALL_H */
