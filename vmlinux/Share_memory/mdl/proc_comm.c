#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_NAME "proc_comm"
#define BUFFER_SIZE 128

static char *proc_buffer;
static unsigned long proc_buffer_size = 0;

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;

    if (*pos > 0 || count < proc_buffer_size) {
        return 0;
    }

    if (copy_to_user(usr_buf, proc_buffer, proc_buffer_size)) {
        rv = -EFAULT;
    } else {
        rv = proc_buffer_size;
        *pos = proc_buffer_size;
    }

    return rv;
}

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    proc_buffer_size = count;
    if (proc_buffer_size > BUFFER_SIZE) {
        proc_buffer_size = BUFFER_SIZE;
    }

    if (copy_from_user(proc_buffer, usr_buf, proc_buffer_size)) {
        return -EFAULT;
    }

    return proc_buffer_size;
}

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};

static int __init proc_comm_init(void) {
    proc_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!proc_buffer) {
        return -ENOMEM;
    }

    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    return 0;
}

static void __exit proc_comm_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    kfree(proc_buffer);
}

MODULE_LICENSE("GPL");
module_init(proc_comm_init);
module_exit(proc_comm_exit);
