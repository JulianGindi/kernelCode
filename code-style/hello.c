#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JulianGindi");
MODULE_DESCRIPTION("A simple hello world module");

static int __init hello_init(void)
{
	netdev_dbg("Hello world!\n");
	return 0;
}

static void __exit hello_cleanup(void)
{
	netdev_dbg("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
