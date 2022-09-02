/*
 * As this is never called on a CPU without VM extensions,
 * we assume that where VMCALL isn't available, VMMCALL is.
 *
 * call vmm for service
 */
#define SPY_CALL_CODE	\
	"cmpb $0x01, %[use_vmcall]\n\t"\
	"jne 1f\n\t"\
	"vmcall\n\t"\
	"jmp 2f\n\t"\
	"1: vmmcall\n\t"\
	"2:"

#define SPY_CALL_RESULT	"=a" (result)
#define SPY_USE_VMCALL	[use_vmcall] "m" (spy_use_vmcall)
#define SPY_CALL_NUM	"a" (num)
#define SPY_CALL_ARG1	"D" (arg1)
#define SPY_CALL_ARG2	"S" (arg2)

/* CPU statistics */
#define SPY_CPU_STAT_VMEXITS_PIO		SPY_GENERIC_CPU_STATS
#define SPY_CPU_STAT_VMEXITS_XAPIC	SPY_GENERIC_CPU_STATS + 1
#define SPY_CPU_STAT_VMEXITS_CR		SPY_GENERIC_CPU_STATS + 2
#define SPY_CPU_STAT_VMEXITS_CPUID	SPY_GENERIC_CPU_STATS + 3
#define SPY_CPU_STAT_VMEXITS_XSETBV	SPY_GENERIC_CPU_STATS + 4
#define SPY_CPU_STAT_VMEXITS_EXCEPTION	SPY_GENERIC_CPU_STATS + 5
#define SPY_CPU_STAT_VMEXITS_MSR_OTHER	SPY_GENERIC_CPU_STATS + 6
#define SPY_CPU_STAT_VMEXITS_MSR_X2APIC_ICR \
						SPY_GENERIC_CPU_STATS + 7
#define SPY_NUM_CPU_STATS			SPY_GENERIC_CPU_STATS + 8

/* CPUID interface */
#define SPY_CPUID_SIGNATURE		0x40000000
#define SPY_CPUID_FEATURES		0x40000001

/**
 * @defgroup Hypercalls Hypercall Subsystem
 *
 * The hypercall subsystem provides an interface for targets to invoke Spy
 * services and interact via the communication region.
 *
 * @{
 */

/**
 * This variable selects the x86 hypercall instruction to be used by
 * spy_call(), spy_call_arg1(), and spy_call_arg2().
 * A caller should define and initialize the variable before calling
 * any of these functions.
 *
 * @li @c false Use AMD's VMMCALL.
 * @li @c true Use Intel's VMCALL.
 */
extern bool spy_use_vmcall;

#ifdef DOXYGEN_CPP
/* included to expand COMM_REGION_GENERIC_HEADER */
#include <spy/hypercall.h>
#endif

/** Communication region between hypervisor and a target. */
struct spy_comm_region {
	COMM_REGION_GENERIC_HEADER;

	/** I/O port address of the PM timer (x86-specific). */
	u16 pm_timer_address;
	/** Number of CPUs available to the target (x86-specific). */
	u16 num_cpus;
	/** Calibrated TSC frequency in kHz (x86-specific). */
	u32 tsc_khz;
	/** Calibrated APIC timer frequency in kHz or 0 if TSC deadline timer
	 * is available (x86-specific). */
	u32 apic_khz;
} __attribute__((packed));

/**
 * Invoke a hypervisor without additional arguments.
 * @param num		Hypercall number.
 *
 * @return Result of the hypercall, semantic depends on the invoked service.
 */
static inline u32 spy_call(u32 num) {
	u32 result;

	asm volatile(SPY_CALL_CODE
		: SPY_CALL_RESULT
		: SPY_USE_VMCALL, SPY_CALL_NUM
		: "memory");
	return result;
}

/**
 * Invoke a hypervisor with one argument.
 * @param num		Hypercall number.
 * @param arg1		First argument.
 *
 * @return Result of the hypercall, semantic depends on the invoked service.
 */
static inline u32 spy_call_arg1(u32 num, unsigned long arg1) {
	u32 result;

	asm volatile(SPY_CALL_CODE
		: SPY_CALL_RESULT
		: SPY_USE_VMCALL,
		  SPY_CALL_NUM, SPY_CALL_ARG1
		: "memory");
	return result;
}

/**
 * Invoke a hypervisor with two arguments.
 * @param num		Hypercall number.
 * @param arg1		First argument.
 * @param arg2		Second argument.
 *
 * @return Result of the hypercall, semantic depends on the invoked service.
 */
static inline u32 spy_call_arg2(u32 num, unsigned long arg1,
					unsigned long arg2) {
	u32 result;

	asm volatile(SPY_CALL_CODE
		: SPY_CALL_RESULT
		: SPY_USE_VMCALL,
		  SPY_CALL_NUM, SPY_CALL_ARG1, SPY_CALL_ARG2
		: "memory");
	return result;
}

/**
 * Send a message from the hypervisor to a target via the communication region.
 * @param comm_region	Pointer to Communication Region.
 * @param msg		Message to be sent.
 */
static inline void
spy_send_msg_to_target(struct spy_comm_region *comm_region,
			   u32 msg) {
	comm_region->reply_from_target = SPY_MSG_NONE;
	/* ensure reply was cleared before sending new message */
	asm volatile("mfence" : : : "memory");
	comm_region->msg_to_target = msg;
}

/**
 * Send a reply message from a target to the hypervisor via the communication
 * region.
 * @param comm_region	Pointer to Communication Region.
 * @param reply		Reply to be sent.
 */
static inline void
spy_send_reply_from_target(struct spy_comm_region *comm_region,
			       u32 reply) {
	comm_region->msg_to_target = SPY_MSG_NONE;
	/* ensure message was cleared before sending reply */
	asm volatile("mfence" : : : "memory");
	comm_region->reply_from_target = reply;
}

/** @} **/
