#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <stdlib.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JulianGindi");
MODULE_DESCRIPTION("A simple hello world module");

static struct miscdevice hello_dev;

static ssize_t hello_read(struct file *file, char *buf,
			  size_t count, loff_t *off)
{
	const char *my_id = "f09605a798d4";
	if (*off == 0) {
		if (copy_to_user(buf, &my_id, 12) != 0) {
			return -EFAULT;
		} else {
			(*off)++;
			return 12;
		}
	} else
		return 0;
};

static ssize_t hello_write(struct file *file, char *buf,
			   size_t count, loff_t *off)
{
	char user_input[count];

	if (count <= 0)
		return -EFAULT;
	else if (copy_from_user(&user_input, buf, count) != 0)
		return -EFAULT;
	else if (strcmp(user_input, "f09605a798d4") == 0)
		return count;
	else
		return -EINVAL;
};

static const struct file_operations hello_fops = {
	owner: THIS_MODULE,
	read: hello_read,
	write: hello_write,
};

static int dev_init(char *ident, const struct file_operations *fops)
{
	hello_dev.minor = MISC_DYNAMIC_MINOR;
	hello_dev.name = ident;
	hello_dev.fops = fops;

	return 0;
};

static int __init hello_init(void)
{
	int ret;

	dev_init("eudyptula", &hello_fops);

	ret = misc_register(&hello_dev);
	if (returnVal)
		printk(KERN_ERR "Error loading hello world misc device");

	/* Never getting rid of this line */
	printk(KERN_DEBUG "Hello world!\n");

	return ret;
};


static void __exit hello_cleanup(void)
{
	misc_deregister(&hello_dev);
	printk(KERN_DEBUG "Cleaning up module.\n");
};

module_init(hello_init);
module_exit(hello_cleanup);
