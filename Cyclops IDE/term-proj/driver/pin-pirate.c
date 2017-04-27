/**
 * @file	pin-pirate.c 
 * @author	Yi-Fan Zhang
 * @date	22 OCT 2016
 * @version 0.1
 * @brief Kernel module that allows users to force pinmux configurations from user-space.
 * @see Based on example code from Derek Molloy http://www.derekmolloy.ie/.
 */

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/device.h>           // Header to support the kernel Driver Model
#include <linux/io.h>
#include <linux/fs.h>               // Header for the Linux file system support
#include <asm/uaccess.h>            // Required for the copy to user function
#define  DEVICE_NAME "pinpirate"    // The device will appear at /dev/ebbchar using this value
#define  CLASS_NAME  "pinmux"       // The device class -- this is a character device driver

MODULE_LICENSE("GPL");				// The license type -- this affects runtime behavior
MODULE_AUTHOR("Yi-Fan Zhang");		// The author -- visible when you use modinfo
MODULE_DESCRIPTION("Hijack pins");	// The description -- see modinfo
MODULE_VERSION("0.1");				// The version of the module

#define PINMUX_IO_BASE 0x44e10800
#define PINMUX_IO_SIZE 0x238

static void *pinmux_io_base;

static int    majorNumber;                  // Stores the device number -- determined automatically
static char   message[256] = {0};           // Memory for the string that is passed from userspace
static struct class*  pinpirate_class  = NULL; // The device-driver class struct pointer
static struct device* pinpirate_dev = NULL; // The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init pinpirate_init(void){
	printk(KERN_INFO DEVICE_NAME": module init\n");

	// Try to dynamically allocate a major number for the device -- more difficult but worth it
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0){
		printk(KERN_ALERT "EBBChar failed to register a major number\n");
		return majorNumber;
	}
	printk(KERN_INFO DEVICE_NAME": registered correctly with major number %d\n", majorNumber);

	// Register the device class
	// We do this so we don't have to manually use mknod to create the device file.
	pinpirate_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pinpirate_class)){
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT DEVICE_NAME": Failed to register device class\n");
		return PTR_ERR(pinpirate_class);
	}
	printk(KERN_INFO DEVICE_NAME": device class registered correctly\n");

	// Register the device driver
	pinpirate_dev = device_create(pinpirate_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(pinpirate_dev)){
		class_destroy(pinpirate_class);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT DEVICE_NAME": Failed to create the device\n");
		return PTR_ERR(pinpirate_dev);
	}
	printk(KERN_INFO DEVICE_NAME": device class created correctly\n");

	return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit pinpirate_exit(void){
	printk(KERN_INFO DEVICE_NAME": module exit\n");
	device_destroy(pinpirate_class, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(pinpirate_class);                          // unregister the device class
	class_destroy(pinpirate_class);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number 
}

/** @brief The device open function that is called each time the device is opened
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
	printk(KERN_INFO DEVICE_NAME": Device has been opened.\n");

	pinmux_io_base = ioremap(PINMUX_IO_BASE, PINMUX_IO_SIZE);
	if (pinmux_io_base == NULL) {
		return -ENODEV;
	}

	return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
/*
	printk(KERN_INFO DEVICE_NAME": offset: 0x%llu\n", *offset);
	int error_count = 0;
	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	error_count = copy_to_user(buffer, message, size_of_message);

	if (error_count==0){            // if true then have success
		printk(KERN_INFO DEVICE_NAME": Sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);  // clear the position to the start and return 0
	}
	else {
		printk(KERN_INFO DEVICE_NAME": Failed to send %d characters to the user\n", error_count);
		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
	}
*/
	return 0;
}

int read_message(const char *buffer, size_t len, unsigned long* val)
{
	int i;

	for (i = 0; i < len; i++)
	{
		char c = buffer[i];

		if (c == ' ' || i == len - 1 || i == sizeof message - 1)
		{
			message[i] = '\0';
			break;
		}

		message[i] = c; 
    }

	if (kstrtol(message, 0, val))
		goto ERROR;

	return i;
ERROR:
	return -1;
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	unsigned long register_offset = 0;
	unsigned long register_value = 0;
	int i;

	i = read_message(buffer, len, &register_offset);

	if (i == -1)
		goto ERROR_INPUT;

	i++;
	if (i >= len || read_message(buffer+i, len-i, &register_value) == -1)
		goto ERROR_INPUT;

	if (register_offset >= PINMUX_IO_SIZE) {
		printk(KERN_ERR DEVICE_NAME": offset 0x%lx out of bounds\n", register_offset);
		return len;
	}

	if (register_value >= 128) {
		printk(KERN_ERR DEVICE_NAME": value 0x%lx out of bounds\n", register_value);
		return len;
	}

	printk(KERN_INFO DEVICE_NAME": Register offset: 0x%lx value: 0x%lx\n", register_offset, register_value);
	writeb(register_value, pinmux_io_base + register_offset);

	return len;

ERROR_INPUT:
	printk(KERN_ERR DEVICE_NAME": invalid input\n");
	return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO DEVICE_NAME": Device successfully closed\n");
	iounmap(pinmux_io_base);
	return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(pinpirate_init);
module_exit(pinpirate_exit);
