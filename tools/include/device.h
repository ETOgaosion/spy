//
// Created by 蓝色空间 on 2022/9/1.
//

#ifndef SPY_DEVICE_H
#define SPY_DEVICE_H

#include <stddef.h>

#define SPY_EXEC_DIR	LIBEXECDIR "/spy"
#define SPY_DEVICE	"/dev/spy"
#define SPY_TARGETS		"/sys/devices/spy/targets/"

int open_dev();
void *read_string(const char *string, size_t *size);
void *read_file(const char *name, size_t *size);

#endif //SPY_DEVICE_H
