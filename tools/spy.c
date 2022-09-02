//
// Created by 蓝色空间 on 2022/9/1.
//

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../lib/string.h"
#include "include/device.h"
#include "include/enable.h"
#include "include/target.h"
#include "include/hardware.h"

#define SPY_EXEC_DIR	LIBEXECDIR "/spy"
#define SPY_DEVICE	"/dev/spy"
#define SPY_TARGETS		"/sys/devices/spy/targets/"

struct instrfmt {
    char *cmd, *subcmd, *help;
};

static const struct instrfmt instrfmts[] = {
        {"enable", "", "{SYS CONFIG}"},
        {"disable", "", ""},
        {"target", "create", "{TARGET CONFIG}"},
        {"target", "list", ""},
        {"target", "load", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"} TARGET_DIR: {{-t | --target} \"STRING\"} {{-a | --address} ADDRESS}"},
        {"target", "start", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "shutdown", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "eliminate", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "stats", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"hardware", "check", ""},
        {NULL}
};

void __attribute__((noreturn)) help(char *func, int exit_status) {
    const struct instrfmt *ext;
    printf("Usage: %s {COMMAND | --help | --version}\n", basename(func));
    for (ext = instrfmts; ext->cmd; ext++) {
        printf("\t%s %s %s\n", ext->cmd, ext->subcmd, ext->help);
    }
    exit(exit_status);
}

static int target_management(int argc, char *argv[]) {
    int err;

    if (argc < 3)
        help(argv[0], 1);

    switch (strhash(argv[2])) {
        case STR_CREATE:
            err = target_create(argc, argv);
            break;
        case STR_LIST:
            err = target_list(argc, argv);
            break;
        case STR_LOAD:
            err = target_shutdown_load(argc, argv, STR_LOAD);
            break;
        case STR_START:
            err = target_simple_cmd(argc, argv, STR_START);
            break;
        case STR_SHUTDOWN:
            err = target_shutdown_load(argc, argv, STR_SHUTDOWN);
            break;
        case STR_ELIMINATE:
            err = target_simple_cmd(argc, argv);
            break;
        case STR_STATS:
            err = target_stats(argc, argv);
            break;
        default:
            help(argv[0], 1);
            break;
    }

    return err;
}

int main(int argc, char *argv[]) {
    int fd;
    int err;

    if (argc < 2)
        help(argv[0], 1);

    switch (strhash(argv[1])) {
        case STR_ENABLE:
            err = enable(argc, argv);
            break;
        case STR_DISABLE:
            fd = open_dev();
            err = ioctl(fd, SPY_DISABLE);
            if (err)
                perror("SPY_DISABLE");
            close(fd);
            break;
        case STR_TARGET:
            err = target_management(argc, argv);
            break;
        case STR_VERSION:
        case STR_V:
            printf("Spy management tool %s\n", SPY_VERSION);
            return 0;
            break;
        case STR_HELP:
        case STR_H:
            help(argv[0], 0);
            break;
        case STR_HARDWARE:
            hardware_check();
            break;
        default:
            help(argv[0], 1);
            break;
    }

    return err ? 1 : 0;
}
