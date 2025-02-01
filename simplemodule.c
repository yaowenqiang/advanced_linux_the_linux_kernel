#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

int init_simple(void);
void cleanup_simple(void);
int init_simple(void)
{
    printk("in init module simple\n");
    return 0;
}

void cleanup_simple(void)
{
    printk("in cleanup module simple\n");
}

module_init(init_simple);
module_exit(cleanup_simple);
MODULE_LICENSE("GPL");
