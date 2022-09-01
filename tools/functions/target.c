//
// Created by 蓝色空间 on 2022/9/1.
//

#include "../include/target.h"
#include "../lib/string.h"

char *read_sysfs_target_string(const unsigned int id, const char *entry)
{
    char *ret, buffer[128];
    size_t size;

    snprintf(buffer, sizeof(buffer), SPY_TARGETS "%u/%s", id, entry);
    ret = read_file(buffer, &size);

    /* entries in /sys/devices/spy/targets must not be empty */
    if (size == 0) {
        snprintf(buffer, sizeof(buffer),
                 "reading " SPY_TARGETS "%u/%s", id, entry);
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

int enable(int argc, char *argv[])
{
    void *config;
    int err, fd;

    if (argc != 3)
        help(argv[0], 1);

    config = read_file(argv[2], NULL);

    fd = open_dev();

    err = ioctl(fd, SPY_ENABLE, config);
    if (err)
        perror("SPY_ENABLE");

    close(fd);
    free(config);

    return err;
}

int target_create(int argc, char *argv[])
{
    struct spy_target_create target_create;
    size_t size;
    int err, fd;

    if (argc != 4)
        help(argv[0], 1);

    target_create.config_address = (unsigned long)read_file(argv[3], &size);
    target_create.config_size = size;

    fd = open_dev();

    err = ioctl(fd, SPY_TARGET_CREATE, &target_create);
    if (err)
        perror("SPY_TARGET_CREATE");

    close(fd);
    free((void *)(unsigned long)target_create.config_address);

    return err;
}

int parse_target_id(struct spy_target_id *target_id, int argc,
                           char *argv[])
{
    bool use_name = false;
    int arg_pos = 0;
    char *endp;

    memset(target_id, 0, sizeof(*target_id));

    if (argc < 1)
        return 0;

    if (strcmp(argv[0], "--name") == 0) {
        if (argc < 2)
            return 0;
        arg_pos++;
        use_name = true;
    } else {
        errno = 0;
        target_id->id = strtoll(argv[0], &endp, 0);
        if (errno != 0 || *endp != 0 || target_id->id < 0)
            use_name = true;
    }

    if (use_name) {
        target_id->id = SPY_TARGET_ID_UNUSED;
        /* target_id is initialized with zeros, so leaving out the last
         * byte ensures that the string is always terminated. */
        strncpy(target_id->name, argv[arg_pos],
                sizeof(target_id->name) - 1);
    }

    return arg_pos + 1;
}

bool match_opt(const char *argv, const char *short_opt,
                      const char *long_opt)
{
    return strcmp(argv, short_opt) == 0 ||
           strcmp(argv, long_opt) == 0;
}

struct spy_target_info *get_target_info(const unsigned int id)
{
    struct spy_target_info *cinfo;
    char *tmp;

    cinfo = malloc(sizeof(struct spy_target_info));
    if (cinfo == NULL) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }

    /* set target id */
    cinfo->id.id = id;

    /* get target name */
    tmp = read_sysfs_target_string(id, "name");
    strncpy(cinfo->id.name, tmp, SPY_TARGET_ID_NAMELEN);
    cinfo->id.name[SPY_TARGET_ID_NAMELEN] = 0;
    free(tmp);

    /* get target state */
    cinfo->state = read_sysfs_target_string(id, "state");

    /* get assigned cpu list */
    cinfo->cpus_assigned_list =
            read_sysfs_target_string(id, "cpus_assigned_list");

    /* get failed cpu list */
    cinfo->cpus_failed_list = read_sysfs_target_string(id, "cpus_failed_list");

    return cinfo;
}

void target_info_free(struct spy_target_info *cinfo)
{
    free(cinfo->state);
    free(cinfo->cpus_assigned_list);
    free(cinfo->cpus_failed_list);
    free(cinfo);
}

int target_match(const struct dirent *dirent)
{
    return dirent->d_name[0] != '.';
}

int target_list(int argc, char *argv[])
{
    struct dirent **namelist;
    struct spy_target_info *cinfo;
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

        cinfo = get_target_info(id);
        printf("%-8d%-24s%-18s%-24s%-24s\n", cinfo->id.id, cinfo->id.name,
               cinfo->state, cinfo->cpus_assigned_list, cinfo->cpus_failed_list);
        target_info_free(cinfo);
        free(namelist[i]);
    }

    free(namelist);
    return 0;
}

int target_shutdown_load(int argc, char *argv[],
                                enum shutdown_load_mode mode)
{
    struct spy_preload_image *image;
    struct spy_target_load *target_load;
    struct spy_target_id target_id;
    int err, fd, id_args, arg_num;
    unsigned int images, n;
    size_t size;
    char *endp;

    id_args = parse_target_id(&target_id, argc - 3, &argv[3]);
    arg_num = 3 + id_args;
    if (id_args == 0 || (mode == SHUTDOWN && arg_num != argc) ||
        (mode == LOAD && arg_num == argc))
        help(argv[0], 1);

    images = 0;
    while (arg_num < argc) {
        if (match_opt(argv[arg_num], "-s", "--string")) {
            if (arg_num + 1 >= argc)
                help(argv[0], 1);
            arg_num++;
        }

        images++;
        arg_num++;

        if (arg_num < argc &&
            match_opt(argv[arg_num], "-a", "--address")) {
            if (arg_num + 1 >= argc)
                help(argv[0], 1);
            arg_num += 2;
        }
    }

    target_load = malloc(sizeof(*target_load) + sizeof(*image) * images);
    if (!target_load) {
        fprintf(stderr, "insufficient memory\n");
        exit(1);
    }
    target_load->target_id = target_id;
    target_load->num_preload_images = images;

    arg_num = 3 + id_args;

    for (n = 0, image = target_load->image; n < images; n++, image++) {
        if (match_opt(argv[arg_num], "-s", "--string")) {
            arg_num++;
            image->source_address =
                    (unsigned long)read_string(argv[arg_num++],
                                               &size);
        } else {
            image->source_address =
                    (unsigned long)read_file(argv[arg_num++],
                                             &size);
        }
        image->size = size;
        image->target_address = 0;

        if (arg_num < argc &&
            match_opt(argv[arg_num], "-a", "--address")) {
            errno = 0;
            image->target_address =
                    strtoll(argv[arg_num + 1], &endp, 0);
            if (errno != 0 || *endp != 0)
                help(argv[0], 1);
            arg_num += 2;
        }
    }

    fd = open_dev();

    err = ioctl(fd, SPY_TARGET_LOAD, target_load);
    if (err)
        perror("SPY_TARGET_LOAD");

    close(fd);
    for (n = 0, image = target_load->image; n < images; n++, image++)
        free((void *)(unsigned long)image->source_address);
    free(target_load);

    return err;
}

int target_simple_cmd(int argc, char *argv[], unsigned int command)
{
    struct spy_target_id target_id;
    int id_args, err, fd;

    id_args = parse_target_id(&target_id, argc - 3, &argv[3]);
    if (id_args == 0 || 3 + id_args != argc)
        help(argv[0], 1);

    fd = open_dev();

    err = ioctl(fd, command, &target_id);
    if (err)
        perror(command == SPY_TARGET_START ?
               "SPY_TARGET_START" :
               command == SPY_TARGET_ELIMINATE ?
               "SPY_TARGET_ELIMINATE" :
               "<unknown command>");

    close(fd);

    return err;
}