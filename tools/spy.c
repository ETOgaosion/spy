//
// Created by 蓝色空间 on 2022/9/1.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "lib/string.h"

#define SPY_EXEC_DIR	LIBEXECDIR "/spy"
#define SPY_DEVICE	"/dev/spy"
#define SPY_TARGETS		"/sys/devices/spy/targets/"

enum shutdown_load_mode {LOAD, SHUTDOWN};

struct instrfmt {
    char *cmd, *subcmd, *help;
};

struct spy_target_info {
    struct spy_target_id id;
    char *state;
    char *cpus_assigned_list;
    char *cpus_failed_list;
};

static const struct instrfmt instrfmts[] = {
        {"enable", "", ""},
        {"disable", "", ""},
        {"target", "create", "{TARGETCONFIG}"},
        {"target", "list", ""},
        {"target", "load", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"} TARGET_DIR: {{-t | --target} \"STRING\"}"},
        {"target", "start", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "shutdown", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "eliminate", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"target", "stats", "ID: {{-i | --id} DECIMAL | {-n | --name} \"NAME\"}"},
        {"hardware", "check", ""},
        {NULL}
};

static void __attribute__((noreturn)) help(char *prog, int exit_status)
{
    const struct instrfmt *ext;
    printf("Usage: %s {COMMAND | --help | --version}\n", basename(prog));
    for (ext = instrfmts; ext->cmd; ext++) {
        printf("\t%s %s %s\n", ext->cmd, ext->subcmd, ext->help);
    }
    exit(exit_status);
}

static int target_management(int argc, char *argv[])
{
    int err;

    if (argc < 3)
        help(argv[0], 1);

    switch (strhash(argv[2])) {
        case STR_CREATE:
            err = target_create(argc, argv);
        case STR_LIST:
            err = target_list(argc, argv);
        case STR_LOAD:
            err = target_shutdown_load(argc, argv, LOAD);
        case STR_START:
            err = target_simple_cmd(argc, argv, SPY_TARGET_START);
        case STR_SHUTDOWN:
            err = target_shutdown_load(argc, argv, SHUTDOWN);
        case STR_ELIMINATE:
            err = target_simple_cmd(argc, argv, SPY_TARGET_ELIMINATE);
        case STR_STATS:
            call_instrfmt_script("target", argc, argv);
            help(argv[0], 1);
    }

    return err;
}

int main(int argc, char *argv[])
{
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
        case STR_CONSOLE:
            err = console(argc, argv);
            break;
        case STR_CONFIG:
            call_instrfmt_script(argv[1], argc, argv);
            help(argv[0], 1);
            break;
        case STR_VERSION:
        case STR_V:
            printf("Jailhouse management tool %s\n", SPY_VERSION);
            return 0;
        case STR_HELP:
        case STR_H:
            help(argv[0], 0);
            break;
        default:
            help(argv[0], 1);
            break;
    }

    return err ? 1 : 0;
}
