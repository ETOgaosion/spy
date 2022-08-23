#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Blue Space");
MODULE_DESCRIPTION("Use Linux module to implement lightweight virtualization");
MODULE_VERSION("0.01");
#define DEVICE_NAME "spy"


static char func_name[NAME_MAX] = "tboot_shutdown";

static struct kprobe kp = {
        .symbol_name    = func_name,
};

int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    return 0;
}

void handler_post(struct kprobe *p, struct pt_regs *regs,
                  unsigned long flags) {
    pr_info("[spy] > patch successfully\n");
    pr_info("[spy] > <%s> post_handler: p->addr = 0x%p, flags = 0x%lx\n",
                p->symbol_name, p->addr, regs->flags);
}

int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
    pr_info("[spy] > patch failed\n");
    pr_info("[spy] > fault_handler: p->addr = 0x%p, trap #%dn", p->addr, trapnr);
        /* Return 0 because we don't handle the fault. */
    return 0;
}

static int __init kprobe_init(void)
{
    int ret;
    kp.pre_handler = handler_pre;
    kp.post_handler = handler_post;
    kp.fault_handler = handler_fault;

    ret = register_kprobe(&kp);
    if (ret < 0) {
            pr_err("[spy] > register_kprobe failed, returned %d\n", ret);
            return ret;
    }
    pr_info("[spy] > Planted kprobe at %p\n", kp.addr);
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    pr_info("[spy] > kprobe at %p unregistered\n", kp.addr);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");