//
// Created by 蓝色空间 on 2022/9/1.
//

#include "../include/enable.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../include/spy.h"
#include "../include/device.h"
#include "../../lib/string.h"

int enable(int argc, char *argv[]) {
    void *config;
    int err, fd;

    if (argc != 3)
        help(argv[0], 1);

    config = read_file(argv[2], NULL);

    fd = open_dev();

    err = ioctl(fd, STR_ENABLE, config);
    if (err)
        perror("SPY_ENABLE");

    close(fd);
    free(config);

    return err;
}
