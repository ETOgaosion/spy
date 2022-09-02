//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_IOCTL_H
#define SPY_IOCTL_H

int hv_enable(struct jailhouse_system __user *arg);
int hv_disable(void);
int hv_target_create(struct jailhouse_cell_create __user *arg);
int hv_target_load(struct jailhouse_cell_load __user *arg);
int hv_target_start(const char __user *arg);
int hv_target_destroy(const char __user *arg);

#endif //SPY_IOCTL_H
