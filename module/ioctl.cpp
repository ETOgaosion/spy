//
// Created by 蓝色空间 on 2022/9/1.
//

#include "ioctl.h"

#include <linux/kernel.h>

int hv_enable(struct jailhouse_system __user *arg) {
    pr_info("enabled succeed");
}

int hv_disable(void) {
    pr_info("disabled succeed");
}

int hv_target_create(struct jailhouse_cell_create __user *arg) {
    pr_info("target created succeed");
}

int hv_target_load(struct jailhouse_cell_load __user *arg) {
    pr_info("target loaded succeed");
}

int hv_target_start(const char __user *arg) {
    pr_info("target started succeed");
}

int hv_target_eliminate(const char __user *arg) {
    pr_info("target eliminated succeed");
}
