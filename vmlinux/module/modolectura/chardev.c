/*
 * chardev.c: Creates a read-write char device that says how many times
 * you've written to the dev file
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> /* for put_user and get_user */

/*
 * Prototypes − this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "ucsp-c" /* Dev name as it appears in /proc/devices */
#define BUF_LEN 80 /* Max length of the message from the device */

/*
 * Global variables are declared as static, so are global within the file.
 */
static int Major; /* Major number assigned to our device driver */
static int Device_Open = 0; /* Is device open? Used to prevent multiple access to device */
static char msg[BUF_LEN]; /* The msg the device will give when asked */
static char *msg_Ptr;
static int write_counter = 0; /* Counter for the number of writes */
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void) {
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");
    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void) {
    unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file) {
    if (Device_Open)
        return -EBUSY;
    Device_Open++;
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file) {
    Device_Open--; /* We're now ready for our next caller */
    /*
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module.
     */
    module_put(THIS_MODULE);
    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h */
                           char *buffer, /* buffer to fill with data */
                           size_t length, /* length of the buffer */
                           loff_t *offset) {
    int bytes_read = 0;
    if (*msg_Ptr == 0)
        return 0;
    while (length && *msg_Ptr) {
        if (put_user(*(msg_Ptr++), buffer++)) {
            return -EFAULT;
        }
        length--;
        bytes_read++;
    }
    return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/ucsp-c
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
    int bytes_write = 0;
    const char *msg_Ptr_tmp = msg;//declara un puntero constante

    while (len && bytes_write < BUF_LEN - 1) {//longitud mayor que 0
        if (get_user(msg[bytes_write], buff++)) {//
            return -EFAULT;
        }
        len--;//decrementa la longitud
        bytes_write++;
    }
    msg[bytes_write] = '\0'; /* Null-terminate the string */
    msg_Ptr = msg;
    write_counter++;
    printk(KERN_INFO "Device has been written to %d times\n", write_counter);
    return bytes_write;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Character Device Driver");
MODULE_AUTHOR("JM");

