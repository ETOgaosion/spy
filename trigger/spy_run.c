//
// Created by 蓝色空间 on 2022/8/31.
//

#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include <linux/kthread.h>
#include <asm/smp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Blue Space");
MODULE_DESCRIPTION("Use Linux module to implement lightweight virtualization");
MODULE_VERSION("0.01");
#define DEVICE_NAME "spy_run"

#define BUFFER_SIZE 100

static struct task_struct *kthread_core_1;
static struct task_struct *kthread_core_2;

int cpu_online_spy_thread(void *cpuid_ptr) {
    int cpuid = *(int *)cpuid_ptr;
    native_cpu_up(cpuid, NULL);
}

int cpu_offline_spy_thread(void *cpuid_ptr) {
    native_play_dead();
}

int run_init() {
    printf("before offline, cpu online list:\n");
    char buffer[BUFFER_SIZE];
    fp = fopen("/sys/devices/system/cpu/online", "r");
    fread(buffer, BUFFER_SIZE, 1, fp);
    printf("%s\n", buffer);
    fclose(fp);
}

module_init(run_init)
module_exit(run_exit)