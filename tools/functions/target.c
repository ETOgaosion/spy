//
// Created by 蓝色空间 on 2022/9/1.
//

#include "../include/target.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "../../lib/string.h"
#include "../include/device.h"
#include "../include/spy.h"
#include "../../include/spy/target_common.h"

static char *read_target_string(const unsigned int id, const char *entry) {
    char *ret, buffer[128];
    size_t size;

    snprintf(buffer, sizeof(buffer), SPY_TARGETS "%u/%s", id, entry);
    ret = read_file(buffer, &size);

    /* entries in /sys/devices/spy/targets must not be empty */
    if (size == 0) {
        snprintf(buffer, sizeof(buffer), "reading " SPY_TARGETS "%u/%s", id, entry);
        perror(buffer);
        exit(1);
    }

    /* chop trailing linefeeds and enforce the string to be
     * null-terminated */
    if (ret[size-1] != '\n') {
        ret = realloc(ret, ++size);
        if (ret == NULL) {
            fprintf(stderr, "insufficient memory\n");
            exit(1);
        }
    }
    ret[size-1] = 0;

    return ret;
}

int target_create(int argc, char *argv[]) {
    struct spy_target_create target_create;
    size_t size;
    int err, fd;

    if (argc != 4)
        help(argv[0], 1);

    target_create.config_address = (unsigned long)read_file(argv[3], &size);
    target_create.config_size = size;

    fd = open_dev();

    err = ioctl(fd, STR_CREATE, &target_create);
    if (err)
        perror("SPY_TARGET_CREATE");

    close(fd);
    free((void *)(unsigned long)target_create.config_address);

    return err;
}

static int parse_target_id(struct spy_target_id *target_id, int argc, char *argv[]) {
    bool use_name = false;
    int arg_pos = 0;
    char *endptr;

    memset(target_id, 0, sizeof(*target_id));

    if (argc < 1)
        return 0;

    if ((strcmp(argv[0], "--name") == 0) || (strcmp(argv[0], "-n"))) {
        if (argc < 2) {
            return 0;
        }
        arg_pos++;
        use_name = true;
    } else {
        errno = 0;
        target_id->id = strtoll(argv[0], &endptr, 0);
        if (errno != 0 || *endptr != 0 || target_id->id < 0) {
            use_name = true;
        }
    }

    if (use_name) {
        target_id->id = SPY_TARGET_ID_UNUSED;
        /* target_id is initialized with zeros, so leaving out the last
         * byte ensures that the string is always terminated. */
        strncpy(target_id->name, argv[arg_pos], sizeof(target_id->name) - 1);
    }

    return arg_pos + 1;
}

static bool match_opt(const char *argv, const char *short_opt,
                      const char *long_opt) {
    return strcmp(argv, short_opt) == 0 ||
           strcmp(argv, long_opt) == 0;
}

static struct spy_target_info *get_target_info(const unsigned int id) {
    struct spy_target_info *target_info;
    char *tmp;

    target_info = malloc(sizeof(struct spy_target_info));
    if (target_info == NULL) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }

    /* set target id */
    target_info->id.id = id;

    /* get target name */
    tmp = read_target_string(id, "name");
    strncpy(target_info->id.name, tmp, SPY_TARGET_ID_NAMELEN);
    target_info->id.name[SPY_TARGET_ID_NAMELEN - 1] = 0;
    free(tmp);

    /* get target state */
    target_info->state = read_target_string(id, "state");

    /* get assigned cpu list */
    target_info->cpus_assigned_list =
            read_target_string(id, "cpus_assigned_list");

    /* get failed cpu list */
    target_info->cpus_failed_list = read_target_string(id, "cpus_failed_list");

    return target_info;
}

static void target_info_free(struct spy_target_info *target_info) {
    free(target_info->state);
    free(target_info->cpus_assigned_list);
    free(target_info->cpus_failed_list);
    free(target_info);
}

static int target_match(const struct dirent *dirent) {
    return dirent->d_name[0] != '.';
}

int target_list(int argc, char *argv[]) {
    struct dirent **namelist;
    struct spy_target_info *target_info;
    unsigned int id;
    int i, num_entries;
    (void)argv;

    if (argc != 3)
        help(argv[0], 1);

    num_entries = scandir(SPY_TARGETS, &namelist, target_match, alphasort);
    if (num_entries == -1) {
        /* Silently return if kernel module is not loaded */
        if (errno == ENOENT)
            return 0;

        perror("scandir");
        return -1;
    }

    if (num_entries > 0)
        printf("%-8s%-24s%-18s%-24s%-24s\n",
               "ID", "Name", "State", "Assigned CPUs", "Failed CPUs");
    for (i = 0; i < num_entries; i++) {
        id = (unsigned int)strtoul(namelist[i]->d_name, NULL, 10);

        target_info = get_target_info(id);
        printf("%-8d%-24s%-18s%-24s%-24s\n", target_info->id.id, target_info->id.name,
               target_info->state, target_info->cpus_assigned_list, target_info->cpus_failed_list);
        target_info_free(target_info);
        free(namelist[i]);
    }

    free(namelist);
    return 0;
}

int target_shutdown_load(int argc, char *argv[], unsigned long mode) {
    struct spy_preload_image *image;
    struct spy_target_load *target_load;
    struct spy_target_id target_id;
    int err, fd, id_args, arg_num;
    unsigned int n;
    size_t size;
    char *endptr;

    id_args = parse_target_id(&target_id, argc - 3, &argv[3]);
    arg_num = 3 + id_args;
    if (id_args == 0 || (mode == STR_SHUTDOWN && arg_num != argc) || (mode == STR_LOAD && arg_num == argc)) {
        help(argv[0], 1);
    }

    target_load = malloc(sizeof(*target_load));
    if (!target_load) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }
    target_load->target_id = target_id;

    arg_num = 3 + id_args;

    if (match_opt(argv[arg_num], "-t", "--target")) {
        arg_num++;
        image->source_address = (unsigned long)read_string(argv[arg_num++],
                                           &size);
    } else {
        image->source_address = (unsigned long)read_file(argv[arg_num++],
                                         &size);
    }
    image->size = size;
    image->target_address = 0;

    if (arg_num < argc && match_opt(argv[arg_num], "-a", "--address")) {
        errno = 0;
        image->target_address = strtoll(argv[arg_num + 1], &endptr, 0);
        if (errno != 0 || *endptr != 0) {
            help(argv[0], 1);
        }
        arg_num += 2;
    }

    fd = open_dev();

    err = ioctl(fd, STR_LOAD, target_load);
    if (err) {
        perror("SPY_TARGET_LOAD");
    }

    close(fd);
    free((void *) (unsigned long) image->source_address);
    free(target_load);

    return err;
}

int target_simple_cmd(int argc, char *argv[], unsigned long command) {
    struct spy_target_id target_id;
    int id_args, err, fd;

    id_args = parse_target_id(&target_id, argc - 3, &argv[3]);
    if (id_args == 0 || 3 + id_args != argc) {
        help(argv[0], 1);
    }

    fd = open_dev();

    err = ioctl(fd, command, &target_id);
    if (err) {
        perror(command == STR_START ?
               "SPY_TARGET_START" :
               command == STR_ELIMINATE ?
               "SPY_TARGET_ELIMINATE" :
               "<unknown command>");
    }

    close(fd);

    return err;
}

int target_stats(int argc, char *argv[]) {
    struct spy_target_id target_id;
    int id_args, err, fd;

    id_args = parse_target_id(&target_id, argc - 3, &argv[3]);
    if (id_args == 0 || 3 + id_args != argc) {
        help(argv[0], 1);
    }
}