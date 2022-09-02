//
// Created by 蓝色空间 on 2022/9/1.
//

#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int open_dev() {
    int fd;

    fd = open(SPY_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("opening " SPY_DEVICE);
        exit(1);
    }
    return fd;
}

void *read_string(const char *string, size_t *size) {
    void *buffer;

    *size = strlen(string) + 1;

    buffer = strdup(string);
    if (!buffer) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }

    return buffer;
}

void *read_file(const char *name, size_t *size) {
    struct stat stat;
    ssize_t result;
    void *buffer;
    int fd;

    fd = open(name, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "opening %s: %s\n", name, strerror(errno));
        exit(1);
    }

    if (fstat(fd, &stat) < 0) {
        perror("fstat");
        exit(1);
    }

    buffer = malloc(stat.st_size);
    if (!buffer) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }

    result = read(fd, buffer, stat.st_size);
    if (result < 0) {
        fprintf(stderr, "reading %s: %s\n", name, strerror(errno));
        exit(1);
    }

    close(fd);

    if (size)
        *size = (size_t)result;

    return buffer;
}
