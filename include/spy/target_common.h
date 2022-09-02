//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_TARGET_COMMON_H
#define SPY_TARGET_COMMON_H

#include "../../lib/types.h"

#define SPY_TARGET_ID_NAMELEN	32

struct spy_target_create {
    u64 config_address;
    u32 config_size;
    u32 padding;
};

struct spy_preload_image {
    u64  source_address;
    u64  size;
    u64  target_address;
};

struct spy_target_id {
    int id;
    char name[SPY_TARGET_ID_NAMELEN];
};

struct spy_target_load {
    struct spy_target_id target_id;
    u32 num_preload_images;
    u32 padding;
    struct spy_preload_image image;
};

struct spy_target_info {
    struct spy_target_id id;
    char *state;
    char *cpus_assigned_list;
    char *cpus_failed_list;
};

#define SPY_TARGET_ID_UNUSED	(-1)

#define SPY_ENABLE		_IOW(0, 0, void *)
#define SPY_DISABLE		_IO(0, 1)
#define SPY_TARGET_CREATE		_IOW(0, 2, struct spy_target_create)
#define SPY_TARGET_LOAD		_IOW(0, 3, struct spy_target_load)
#define SPY_TARGET_START		_IOW(0, 4, struct spy_target_id)
#define SPY_TARGET_DESTROY		_IOW(0, 5, struct spy_target_id)

#endif //SPY_TARGET_COMMON_H
