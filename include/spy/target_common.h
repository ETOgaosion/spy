//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_TARGET_H
#define SPY_TARGET_H

#include "type.h"

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
    struct spy_target_id cell_id;
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

#endif //SPY_TARGET_H
