#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JulianGindi");
MODULE_DESCRIPTION("A simple hello world module");

static struct dentry *dir = 0;
static char foo_input[4096];
static int foo_len = 0;



static ssize_t hello_read(struct file *file, char *buf,
			  size_t count, loff_t *off)
{
	const char *my_id = "f09605a798d4";

	if (*off == 0) {
		if (copy_to_user(buf, &my_id, count) != 0) {
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
	char user_input[15];

	if (count <= 0 || count > 15)
		return -EFAULT;
	if (copy_from_user(&user_input, buf, count) != 0)
		return -EFAULT;
	if (strncmp(user_input, "f09605a798d4", 12) == 0)
		return count;
	
	return -EINVAL;
};

static const struct file_operations hello_fops = {
	owner: THIS_MODULE,
	read: hello_read,
	write: hello_write,
};

static ssize_t jiffies_read(struct file *file, char *buf,
			  size_t count, loff_t *off)
{
	unsigned long cur = jiffies;
	const int n = snprintf(NULL, 0, "%lu", cur);

	char jstring[n+1];
	snprintf(jstring, n+1, "%lu", cur);


	if (*off == 0) {
		if (copy_to_user(buf, &jstring, count) != 0) {
			return -EFAULT;
		} else {
			(*off)++;
			return 12;
		}
	} else
		return 0;
};

static const struct file_operations jiffies_fops = {
	owner: THIS_MODULE,
	read: jiffies_read,
};

static ssize_t foo_read(struct file *file, char *buf,
			  size_t count, loff_t *off)
{


	if (*off == 0) {
		if (copy_to_user(buf, &foo_input, foo_len) != 0) {
			return -EFAULT;
		} else {
			(*off)++;
			return count;
		}
	} else
		return 0;
};

static ssize_t foo_write(struct file *file, char *buf,
			   size_t count, loff_t *off)
{
	
	if (count > 4096)
		return -EFAULT;
	if (copy_from_user(&foo_input, buf, count) != 0)
		return -EFAULT;
	
	foo_len = count;
	return count;
};

static const struct file_operations foo_fops = {
	owner: THIS_MODULE,
	read: foo_read,
	write: foo_write,
};

int init_module(void)
{
	struct dentry *junk;

	dir = debugfs_create_dir("eudyptula", 0);
	if (!dir) {
		printk(KERN_DEBUG "hello_debug: failed to create dir");
		return -1;
	}

	junk = debugfs_create_file(
			"id",
			0666,
			dir,
			NULL,
			&hello_fops);

	if (!junk) {
		printk(KERN_DEBUG "hello_debug: could not create id");
		return -1;
	}

	junk = debugfs_create_file(
			"jiffies",
			0444,
			dir,
			NULL,
			&jiffies_fops);

	if (!junk) {
		printk(KERN_DEBUG "hello debug: could not create jiffies");
		return -1;
	}

	junk = debugfs_create_file(
			"foo",
			0644,
			dir,
			foo_input,
			&foo_fops);

	if (!junk) {
		printk(KERN_DEBUG "hello debug: could now create foo");
		return -1;
	}


	/* Never getting rid of this line */
	printk(KERN_DEBUG "Hello world!\n");

	return 0;
};


void cleanup_module(void)
{
	debugfs_remove_recursive(dir);
};
