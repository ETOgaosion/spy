//
// Created by 蓝色空间 on 2022/9/1.
//

#include "include/ioctl.h"

#include <linux/kernel.h>
#include <linux/compiler_types.h>

int hv_enable(struct spy_system __user *arg) {
    pr_info("enabled succeed");
    return 0;
}

int hv_disable(void) {
    pr_info("disabled succeed");
    return 0;
}

int hv_target_create(struct spy_target_create __user *arg) {
    pr_info("target created succeed");
    return 0;
}

int hv_target_load(struct spy_target_load __user *arg) {
    pr_info("target loaded succeed");
    return 0;
}

int hv_target_start(const char __user *arg) {
    pr_info("target started succeed");
    return 0;
}

int hv_target_eliminate(const char __user *arg) {
    pr_info("target eliminated succeed");
    return 0;
}
