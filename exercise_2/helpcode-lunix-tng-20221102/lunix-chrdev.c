/*
 * lunix-chrdev.c
 *
 * Implementation of character devices
 * for Lunix:TNG
 *
 * Ioannis Dressos
 *
 */

#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mmzone.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>

#include "lunix.h"
#include "lunix-chrdev.h"
#include "lunix-lookup.h"

/*
 * Global data
 */
struct cdev lunix_chrdev_cdev;

/*
 * Just a quick [unlocked] check to see if the cached
 * chrdev state needs to be updated from sensor measurements.
 */
static int __attribute__((unused)) lunix_chrdev_state_needs_refresh(struct lunix_chrdev_state_struct *);
static int lunix_chrdev_state_needs_refresh(struct lunix_chrdev_state_struct *state)
{
	struct lunix_sensor_struct *sensor;
	
	WARN_ON ( !(sensor = state->sensor));
	
	if(state->buf_timestamp < (sensor->msr_data[state->type]->last_update)) {
		debug("update needed\n");
		return 1;
	}
	else {
		debug("no update needed\n");
		return 0;
	}
}

/*
 * Updates the cached state of a character device
 * based on sensor data. Must be called with the
 * character device state lock held.
 */
static int lunix_chrdev_state_update(struct lunix_chrdev_state_struct *state)
{
	/* ? */
	struct lunix_sensor_struct *sensor;
	uint32_t new_value;
	long parsed_value;
	int int_part, dec_part;
	/* ? */

	debug("leaving\n");

	/* ? */
	if(!lunix_chrdev_state_needs_refresh(state)) return -EAGAIN;
	/* ? */

	/*
	 * Grab the raw data quickly, hold the
	 * spinlock for as little as possible.
	 */
	/* Why use spinlocks? See LDD3, p. 119 */

	/*
	 * Any new data available?
	 */
	/* ? */
	sensor = state->sensor;
	WARN_ON(!sensor);

	spin_lock_irq(&sensor->lock);
	new_value = sensor->msr_data[state->type]->values[0];
	spin_unlock_irq(&sensor->lock);
	/* ? */

	/*
	 * Now we can take our time to format them,
	 * holding only the private state semaphore
	 */

	/* ? */
	switch(state->type) {
		case BATT:
			parsed_value = lookup_voltage[new_value];
			break;
		case TEMP:
			parsed_value = lookup_temperature[new_value];
			break;
		case LIGHT:
			parsed_value = lookup_light[new_value];
			break;
		default:
			debug("unknown measurement type");
	}

	debug("parsed_value = %ld\n", parsed_value);

	int_part = parsed_value / 1000;
	dec_part = parsed_value % 1000;

	debug("actual_value = %d.%d\n", int_part, dec_part);

	state->buf_lim = snprintf(state->buf_data, LUNIX_CHRDEV_BUFSZ, "%d.%d ", int_part, dec_part);

	if(state->buf_lim >= LUNIX_CHRDEV_BUFSZ) debug("snprintf truncated string\n");

	state->buf_timestamp = sensor->msr_data[state->type]->last_update;
	/* ? */

	debug("leaving\n");
	return 0;
}

/*************************************
 * Implementation of file operations
 * for the Lunix character device
 *************************************/

static int lunix_chrdev_open(struct inode *inode, struct file *filp)
{
	/* Declarations */
	/* ? */
	struct lunix_chrdev_state_struct *new_state;  
	int msr_type;
	int sensor_id;
	/* ? */
	int ret;

	debug("entering\n");
	ret = -ENODEV;
	if ((ret = nonseekable_open(inode, filp)) < 0)
		goto out;

	/*
	 * Associate this open file with the relevant sensor based on
	 * the minor number of the device node [/dev/sensor<NO>-<TYPE>]
	 */
	/* ? */
	sensor_id = iminor(inode) >> 3;
	debug("sensor_id  = %d\n", sensor_id);
	msr_type = iminor(inode) % 8;  
	debug("msr_type = %d\n", msr_type);
	if(msr_type >= N_LUNIX_MSR) { // Invalid measurement type
		ret = -ENODEV;
		goto out;
	}
	/* ? */

	/* Allocate a new Lunix character device private state structure */
	/* ? */
	new_state = kmalloc(sizeof(struct lunix_chrdev_state_struct), GFP_KERNEL);

	if(!new_state) {
		printk(KERN_ERR "lunix_chrdev_open: Could not allocate private state structure\n");
		ret = -EFAULT;
		goto out;
	}

	new_state->type = msr_type;
	new_state->sensor = &lunix_sensors[sensor_id];
	new_state->buf_lim = 0;
	new_state->buf_timestamp = 0;

	sema_init(&new_state->lock, 1);

	filp->private_data = new_state;
	/* ? */

out:
	debug("leaving, with ret = %d\n", ret);
	return ret;
}

static int lunix_chrdev_release(struct inode *inode, struct file *filp)
{
	/* ? */
	kfree(filp->private_data);
	/* ? */
	return 0;
}

static long lunix_chrdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	/* Why? */
	return -EINVAL;
}

static ssize_t lunix_chrdev_read(struct file *filp, char __user *usrbuf, size_t cnt, loff_t *f_pos)
{
	ssize_t ret;

	struct lunix_sensor_struct *sensor;
	struct lunix_chrdev_state_struct *state;

	/* ? */
	int temp;
	/* ? */

	state = filp->private_data;
	WARN_ON(!state);

	sensor = state->sensor;
	WARN_ON(!sensor);

	/* Lock? */
	/* ? */
	if(down_interruptible(&state->lock)) return -ERESTARTSYS;
	/* ? */
	/*
	 * If the cached character device state needs to be
	 * updated by actual sensor data (i.e. we need to report
	 * on a "fresh" measurement, do so
	 */
	if (*f_pos == 0) {
		while (lunix_chrdev_state_update(state) == -EAGAIN) {
			/* ? */
			/* The process needs to sleep */
			/* See LDD3, page 153 for a hint */
			up(&state->lock); 

			debug("no new data, sleeping..\n");
			if(wait_event_interruptible(sensor->wq, lunix_chrdev_state_needs_refresh(state))) {
				debug("interrupted\n");
				return -ERESTARTSYS;
			}
			debug("state updated, woken up\n");

			if(down_interruptible(&state->lock)) return -ERESTARTSYS;
			/* ? */
		}
	}

	/* End of file */
	/* ? */
	if(state->buf_lim == 0) {
        ret = 0;
        goto out;
    }
	/* ? */

	/* Determine the number of cached bytes to copy to userspace */
	/* ? */
	temp = state->buf_lim - *f_pos;

	if(temp < cnt) cnt = temp; 

	debug("copying data to user space..\n");
    if(copy_to_user(usrbuf, state->buf_data + *f_pos, cnt)) {
        ret = -EFAULT;
        goto out;
    }
    debug("copy_to_user succeeded\n");
	/* ? */

	/* Auto-rewind on EOF mode? */
	/* ? */
	*f_pos += cnt;

	if(*f_pos >= state->buf_lim) {
		*f_pos = 0;
		ret = cnt;
		goto out;
	}

	ret = cnt;
	/* ? */
out:
	/* Unlock? */
	/* ? */
	up(&state->lock);
	/* ? */
	return ret;
}

static int lunix_chrdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return -EINVAL;
}

static struct file_operations lunix_chrdev_fops = 
{
    .owner          = THIS_MODULE,
	.open           = lunix_chrdev_open,
	.release        = lunix_chrdev_release,
	.read           = lunix_chrdev_read,
	.unlocked_ioctl = lunix_chrdev_ioctl,
	.mmap           = lunix_chrdev_mmap
};

int lunix_chrdev_init(void)
{
	/*
	 * Register the character device with the kernel, asking for
	 * a range of minor numbers (number of sensors * 8 measurements / sensor)
	 * beginning with LINUX_CHRDEV_MAJOR:0
	 */
	int ret;
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;
	
	debug("initializing character device\n");
	cdev_init(&lunix_chrdev_cdev, &lunix_chrdev_fops);
	lunix_chrdev_cdev.owner = THIS_MODULE;

	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	/* ? */
	/* register_chrdev_region? */
	ret = register_chrdev_region(dev_no, lunix_minor_cnt, "lunix:TNG");
	/* ? */

	if (ret < 0) {
		debug("failed to register region, ret = %d\n", ret);
		goto out;
	}	
	/* ? */
	/* cdev_add? */
	ret = cdev_add(&lunix_chrdev_cdev, dev_no, lunix_minor_cnt);
	/* ? */
	if (ret < 0) {
		debug("failed to add character device\n");
		goto out_with_chrdev_region;
	}
	debug("completed successfully\n");
	return 0;

out_with_chrdev_region:
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
out:
	return ret;
}

void lunix_chrdev_destroy(void)
{
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;
		
	debug("entering\n");
	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	cdev_del(&lunix_chrdev_cdev);
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
	debug("leaving\n");
}
