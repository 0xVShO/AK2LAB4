// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Vlad");
MODULE_DESCRIPTION("Hello world module with ktime and list");
MODULE_LICENSE("GPL");

static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print Hello, world!");

struct hello_time {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	uint i;
	struct hello_time *ptr;
	struct hello_time *md, *tmp;

	if (count == 0 || (count >= 5 && count <= 10)) {
		pr_warn("Warning: count is %u (expected 1-4)\n", count);
	} else if (count > 10) {
		pr_err("Error: count %u is greater than 10\n", count);
		return -EINVAL;
	}

	for (i = 0; i < count; i++) {
		ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
		if (!ptr) {
			list_for_each_entry_safe(md, tmp, &hello_list, list) {
				list_del(&md->list);
				kfree(md);
			}
			return -ENOMEM;
		}

		ptr->time = ktime_get();
		list_add_tail(&ptr->list, &hello_list);
		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_time *md, *tmp;

	list_for_each_entry_safe(md, tmp, &hello_list, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(md->time));
		list_del(&md->list);
		kfree(md);
	}
	pr_info("Module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
