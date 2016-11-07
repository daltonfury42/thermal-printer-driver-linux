#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
 
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))
#define BULK_EP_OUT 0x01
#define BULK_EP_IN 0x81
#define MAX_PKT_SIZE 64
 
static struct usb_device *device;
static struct usb_class_driver class;
static unsigned char bulk_buf[MAX_PKT_SIZE];
 
static int node_open(struct inode *i, struct file *f)
{
	    return 0;

}
static int node_close(struct inode *i, struct file *f)
{
	    return 0;

}
static ssize_t node_read(struct file *f, char __user *buf, size_t cnt, loff_t *off)
{
	    int retval;
	        int read_cnt;
		 
		    /* Read the data from the bulk endpoint */
		    retval = usb_bulk_msg(device, usb_rcvbulkpipe(device, BULK_EP_IN),
				                bulk_buf, MAX_PKT_SIZE, &read_cnt, 5000);
		        if (retval)
			{
				        printk(KERN_ERR "Bulk message returned %d\n", retval);
					        return retval;
						    
			}
			    if (copy_to_user(buf, bulk_buf, MIN(cnt, read_cnt)))
			    {
				            return -EFAULT;
					        
			    }
			     
			        return MIN(cnt, read_cnt);

}
static ssize_t node_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off)
{
	    int retval;
	        int wrote_cnt = MIN(cnt, MAX_PKT_SIZE);
		 
		    if (copy_from_user(bulk_buf, buf, MIN(cnt, MAX_PKT_SIZE)))
		    {
			            return -EFAULT;
				        
		    }
		     
		        /* Write the data into the bulk endpoint */
		        retval = usb_bulk_msg(device, usb_sndbulkpipe(device, BULK_EP_OUT),
					            bulk_buf, MIN(cnt, MAX_PKT_SIZE), &wrote_cnt, 5000);
			    if (retval)
			    {
				            printk(KERN_ERR "Bulk message returned %d\n", retval);
					            return retval;
						        
			    }
			     
			        return wrote_cnt;

}
 
static struct file_operations fops =
{
	    .open = node_open,
	        .release = node_close,
		    .read = node_read,
		        .write = node_write,

};
 
static int tl80_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    int retval;
 
    device = interface_to_usbdev(interface);
 
    class.name = "usb/tl80%d";
    class.fops = &fops;
    if ((retval = usb_register_dev(interface, &class)) < 0)
    {
            /* Something prevented us from registering this driver */
            err("Not able to get a minor for this device.");
        }
    else
    {
            printk(KERN_INFO "Minor obtained: %d\n", interface->minor);
        }
 
    return retval;
}
 
static void tl80_disconnect(struct usb_interface *interface)
{
    usb_deregister_dev(interface, &class);
}
 
/* Table of devices that work with this driver */
static struct usb_device_id tl80_table[] =
{
    { USB_DEVICE(0x1DD4, 0x01AC)  },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, tl80_table);
 
static struct usb_driver pen_driver =
{
    .name = "tl80_driver",
    .probe = tl80_probe,
    .disconnect = tl80_disconnect,
    .id_table = tl80_table,
};
 
static int __init tl80_init(void)
{
    int result;
 
    /* Register this driver with the USB subsystem */
    if ((result = usb_register(&tl80_driver)))
    {
            err("usb_register failed. Error number %d", result);
        }
    return result;
}
 
static void __exit tl80_exit(void)
{
    /* Deregister this driver with the USB subsystem */
    usb_deregister(&tl80_driver);
}
 
module_init(tl80_init);
module_exit(tl80_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Navaneeth Kishore <daltonfury42-gmail_dot_com>");
MODULE_DESCRIPTION("TL80 USB Thermal Printer Device Driver");
