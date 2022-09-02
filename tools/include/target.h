//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_TARGET_H
#define SPY_TARGET_H

#include "types.h"

int target_create(int argc, char *argv[]);
int target_list(int argc, char *argv[]);
int target_shutdown_load(int argc, char *argv[], unsigned long mode);
int target_simple_cmd(int argc, char *argv[], unsigned long command);
int target_stats(int argc, char *argv[]);

#endif //SPY_TARGET_H
