//
// Created by 蓝色空间 on 2022/8/31.
//

#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>

#include "include/device.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Blue Space");
MODULE_DESCRIPTION("Use Linux module to implement lightweight virtualization");
MODULE_VERSION("0.01");


static char func_name[NAME_MAX] = "tboot_shutdown";

static struct kprobe kp = {
        .symbol_name    = func_name,
};

int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    return 0;
}

void handler_post(struct kprobe *p, struct pt_regs *regs,
                  unsigned long flags) {
    pr_info("[spy_patch] > patch successfully\n");
}

static int __init kprobe_init(void) {
    int ret;
    kp.pre_handler = handler_pre;
    kp.post_handler = handler_post;

    ret = register_kprobe(&kp);
    if (ret < 0) {
            pr_err("[spy_patch] > register_kprobe failed, returned %d\n", ret);
            return ret;
    }
    pr_info("[spy_patch] > Planted kprobe at %p\n", kp.addr);
    return spy_device_init();
}

static void __exit kprobe_exit(void) {
    unregister_kprobe(&kp);
    pr_info("[spy_patch] > kprobe at %p unregistered\n", kp.addr);
    spy_device_exit();
}

module_init(kprobe_init)
module_exit(kprobe_exit)