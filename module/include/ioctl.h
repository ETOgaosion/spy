//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_IOCTL_H
#define SPY_IOCTL_H

#include <linux/types.h>
#include <linux/compiler_types.h>

#include "../../include/spy/spy_config.h"
#include "../../include/spy/target_common.h"

int hv_enable(struct spy_system __user *arg);
int hv_disable(void);
int hv_target_create(struct spy_target_create __user *arg);
int hv_target_load(struct spy_target_load __user *arg);
int hv_target_start(const char __user *arg);
int hv_target_eliminate(const char __user *arg);

#endif //SPY_IOCTL_H
