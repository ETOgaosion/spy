#ifndef _SPY_ASM_VCPU_H
#define _SPY_ASM_VCPU_H

#include <spy/target.h>
#include <spy/target-config.h>
#include <spy/entry.h>
#include <spy/paging.h>
#include <spy/percpu.h>
#include <asm/processor.h>

#define X86_CR0_HOST_STATE \
	(X86_CR0_PG | X86_CR0_WP | X86_CR0_NE | X86_CR0_ET | X86_CR0_TS | \
	 X86_CR0_MP | X86_CR0_PE)
#define X86_CR4_HOST_STATE	X86_CR4_PAE

struct vcpu_io_intercept {
	u16 port;
	unsigned int size;
	bool in;
	unsigned int inst_len;
	bool rep_or_str;
};

struct vcpu_mmio_intercept {
	u64 phys_addr;
	bool is_write;
};

int vcpu_early_init(void);

int vcpu_vendor_early_init(void);

int vcpu_target_init(struct target *target);
int vcpu_vendor_target_init(struct target *target);

int vcpu_map_memory_region(struct target *target,
			   const struct spy_memory *mem);
int vcpu_unmap_memory_region(struct target *target,
			     const struct spy_memory *mem);
void vcpu_target_exit(struct target *target);
void vcpu_vendor_target_exit(struct target *target);

int vcpu_init(struct per_cpu *cpu_data);
void vcpu_exit(struct per_cpu *cpu_data);

void __attribute__((noreturn)) vcpu_activate_vmm(void);
void __attribute__((noreturn)) vcpu_deactivate_vmm(void);

void vcpu_handle_exit(struct per_cpu *cpu_data);

void vcpu_park(void);

void vcpu_nmi_handler(void);

void vcpu_tlb_flush(void);

/*
 * vcpu_map_inst() and vcpu_get_inst_bytes() contract:
 *
 * On input, *size gives the number of bytes to get.
 * On output, *size is the number of bytes available.
 *
 * If the function fails (returns NULL), *size is undefined.
 */

const u8 *vcpu_map_inst(const struct guest_paging_structures *pg_structs,
			unsigned long pc, unsigned int *size);

const u8 *vcpu_get_inst_bytes(const struct guest_paging_structures *pg_structs,
			      unsigned long pc, unsigned int *size);

void vcpu_skip_emulated_instruction(unsigned int inst_len);

unsigned int vcpu_vendor_get_io_bitmap_pages(void);

#define VCPU_CS_DPL_MASK	BIT_MASK(6, 5)
#define VCPU_CS_L		(1 << 13)
#define VCPU_CS_DB		(1 << 14)

u64 vcpu_vendor_get_efer(void);
u64 vcpu_vendor_get_rflags(void);
u64 vcpu_vendor_get_rip(void);
u16 vcpu_vendor_get_cs_attr(void);

void vcpu_vendor_get_io_intercept(struct vcpu_io_intercept *io);
void vcpu_vendor_get_mmio_intercept(struct vcpu_mmio_intercept *mmio);

unsigned long vcpu_vendor_get_guest_cr4(void);

void vcpu_get_guest_paging_structs(struct guest_paging_structures *pg_structs);
pt_entry_t vcpu_pae_get_pdpte(page_table_t page_table, unsigned long virt);

void vcpu_vendor_set_guest_pat(unsigned long val);

void vcpu_handle_hypercall(void);

bool vcpu_handle_io_access(void);
bool vcpu_handle_mmio_access(void);

bool vcpu_handle_msr_read(void);
bool vcpu_handle_msr_write(void);

void vcpu_handle_cpuid(void);

void vcpu_reset(unsigned int sipi_vector);
void vcpu_vendor_reset(unsigned int sipi_vector);

#endif
