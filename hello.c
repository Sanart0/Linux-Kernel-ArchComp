// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Sanytskyi Artur <artursanytkpi@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training with timing");
MODULE_LICENSE("GPL");

struct hello_event {
	struct list_head list;
	ktime_t timestamp;
};

static LIST_HEAD(hello_list);

static uint repeat = 1;
module_param(repeat, uint, 0444);
MODULE_PARM_DESC(repeat, "Number of times to print 'Hello, world!' (default=1)");

static int __init hello_init(void)
{
	int i;
	struct hello_event *event;

	if (repeat > 10) {
		pr_err("Invalid repeat: %u. Must not exceed 10.\n", repeat);
		return -EINVAL;
	}

	if (repeat == 0 || (repeat >= 5 && repeat <= 10))
		pr_warn("Strange repeat: %u is suspicious.\n", repeat);

	for (i = 0; i < repeat; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event) {
			struct hello_event *tmp;

			list_for_each_entry_safe(
				event,
				tmp,
				&hello_list,
				list
			) {
				list_del(&event->list);
				kfree(event);
			}
			pr_err("Failed alloc memory for event %d\n", i + 1);
			return -ENOMEM;
		}
		event->timestamp = ktime_get();
		list_add_tail(&event->list, &hello_list);
		pr_info("Hello, world! %u\n", i + 1);
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(
		event,
		tmp,
		&hello_list,
		list
	) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(event->timestamp));
		list_del(&event->list);
		kfree(event);
	}
}

module_init(hello_init);
module_exit(hello_exit);
