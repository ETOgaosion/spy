//
// Created by 蓝色空间 on 2022/8/31.
//

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/apic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Blue Space");
MODULE_DESCRIPTION("Use Linux module to implement lightweight virtualization");
MODULE_VERSION("0.01");
#define DEVICE_NAME "test"

static int __init test_enter(void) {
    pr_info("[test] > can unplug cpu:%d", lapic_can_unplug_cpu());
    return 0;
}

static void __exit test_out(void) {
    return ;
}

module_init(test_enter)
module_exit(test_out)