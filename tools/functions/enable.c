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

int enable(int argc, char *argv[]) {
    void *config;
    int err, fd;

    if (argc != 3)
        help(argv[0], 1);

    config = read_file(argv[2], NULL);

    fd = open_dev();

    err = ioctl(fd, JAILHOUSE_ENABLE, config);
    if (err)
        perror("JAILHOUSE_ENABLE");

    close(fd);
    free(config);

    return err;
}
