/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2013
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <jailhouse/types.h>

/*
 * __attribute__ GNU C属性值
 * 格式：
 * __attribute__((attribute-list))
 * 1. function attribute
 * 1) format(archetype, string-index, first-to-check)
 * archetype:风格，包括printf, scanf, strftime, strfmon，编译器会对参数进行检查
 * string-index:指定传入函数的第几个参数是格式字符串
 * first-to-check:指定从函数第几个参数开始按格式检查
 * 2) noreturn
 * 当函数定义有返回值，但不可能运行到返回值处就已退出
 * 通知编译器函数不会返回值
 * 3) const
 * 只用于带有树脂类型参数的函数，且此函数返回值恒为常数时
 * 编译器只需做一遍
 * 2. aligned
 * 1) aligned (num)
 * 为变量对齐放置，用于数值类型或结构体
 * 2) packed
 * 使用该属性可以使得变量或者结构体成员使用最小的对齐方式
 * 即对变量是一字节对齐，对域（field）是位对齐
 * 若使用typedef，则需要在类型名称之前使用__attribute__
 * 3. at
 * at(location)
 * 绝对定位到RAM
 * 4. section
 * section("section_name")
 * 将函数放到指定的段
 */
void __attribute__((format(printf, 1, 2))) printk(const char *fmt, ...);

void __attribute__((format(printf, 1, 2))) panic_printk(const char *fmt, ...);

#ifdef CONFIG_TRACE_ERROR
#define trace_error(code) ({						  \
	printk("%s:%d: returning error %s\n", __FILE__, __LINE__, #code); \
	code;								  \
})
#else /* !CONFIG_TRACE_ERROR */
#define trace_error(code)	code
#endif /* !CONFIG_TRACE_ERROR */

void arch_dbg_write_init(void);
extern void (*arch_dbg_write)(const char *msg);

extern bool virtual_console;
extern volatile struct jailhouse_virt_console console;
