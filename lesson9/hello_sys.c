#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JulianGindi");
MODULE_DESCRIPTION("A simple module using sysfs");


static char foo_input[PAGE_SIZE];
static int foo_len;


static ssize_t hello_read(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	const char *my_id = "f09605a798d4";

	return sprintf(buf, "%s\n", my_id);
};

static ssize_t hello_write(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	char *user_input;

	user_input = kmalloc(sizeof(char) * 15, GFP_KERNEL);

	if (strncmp(buf, "f09605a798d4", 12) == 0)
		return 12;

	return -EINVAL;
};

static struct kobj_attribute hello_attribute =
	__ATTR(id, 0666, hello_read, hello_write);


static ssize_t jiffies_read(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	unsigned long cur = jiffies;
	const int n = snprintf(NULL, 0, "%lu", cur);

	if (n < 0) 
		return -EINVAL;

	char jstring[n+1];
	snprintf(jstring, n+1, "%lu", cur);

	return sprintf(buf, "%s\n", jstring);
};

static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, 0444, jiffies_read, NULL);


static ssize_t foo_read(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	if (foo_len > PAGE_SIZE)
		return -EINVAL;

	return sprintf(buf, "%s\n", foo_input);
};

static ssize_t foo_write(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf, size_t count)
{

	if (count > PAGE_SIZE)
		return -EINVAL;

	strncpy(foo_input, buf, count);

	foo_len = count;
	return count;
};

static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0644, foo_read, foo_write);

static struct attribute *attrs[] = {
	&hello_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *hello_kobj;

static int __init hello_init(void)
{
	int retval;

	/* Never getting rid of this line */
	printk(KERN_DEBUG "Hello world!\n");


	hello_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!hello_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(hello_kobj, &attr_group);
	if (retval)
		kobject_put(hello_kobj);

	return retval;
};


static void __exit hello_exit(void)
{
	kobject_put(hello_kobj);
}

module_init(hello_init);
module_exit(hello_exit);
