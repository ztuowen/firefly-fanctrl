#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h> 
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define LED_GPIO 258
#define OUT_GPIO 263

#define FANBUFSIZE 10

int len,temp;
char *msg;

int read_proc(struct file *filp,char *buf,size_t count,loff_t *offp ) 
{
    size_t result;
    if(count>temp)
    {
        count=temp;
    }
    temp=temp-count;
    result = copy_to_user(buf,msg, count);
    if (result) {
        printk(KERN_INFO "copy_to_user failed, returned: %d\n.", result);
        return -EINVAL; 
    }
    if(count==0)
        temp=len;
           
    return count;
}

int write_proc(struct file *filp,const char *buf,size_t count,loff_t *offp)
{
    size_t result;
    char c;
    int st;
    if (count>10)
        count=10;
    result = copy_from_user(msg,buf,count);
    if (result) {
        printk(KERN_INFO "copy_from_user failed, returned: %d\n.", result);
        return -EINVAL; 
    }
    sscanf(msg,"%c %d",&c,&st);
    switch (c)
    {
        case 'l':
            gpio_set_value(LED_GPIO,st&1);
            break;
        case 'f':
            gpio_set_value(OUT_GPIO,st&1);
            break;
        default:
            printk(KERN_INFO"Fanio received wrong parameter: %s\n",msg);
    }
    len=count;
    temp=len;
    return count;
}

struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read = read_proc,
    .write = write_proc,
};

static int __init fanio_init(void)
{
    gpio_direction_output(OUT_GPIO,0);
    printk(KERN_INFO"gpio setup\n");
    proc_create("fanio",0666,NULL,&proc_fops);
    msg=kmalloc(GFP_KERNEL,FANBUFSIZE*sizeof(char));
    printk(KERN_INFO"proc file created\n");
    printk(KERN_INFO"fanio initialized\n");
    return 0;
}

static void __exit fanio_exit(void)
{
    remove_proc_entry("fanio",NULL);
    printk(KERN_INFO"exit fanio\n");
}

module_init(fanio_init);
module_exit(fanio_exit);

MODULE_AUTHOR("Joe Zhao <ztuowen@gmail.com>");
MODULE_DESCRIPTION("Fan ctrl IO");
MODULE_LICENSE("GPL");

