// usb_detect_basic_68.c
// Ultra-basic USB plug/unplug detection using USB notifier (Linux 6.8).
// - No binding to USB interfaces
// - No claiming of devices
// - Just logs ADD/REMOVE with VID:PID
//
// Build:
//   make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
// Load:
//   sudo insmod usb_detect_basic_68.ko
// Watch:
//   dmesg -w

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/notifier.h>

#define DRV_NAME "usb_detect_basic_68"

static int usb_notify_cb(struct notifier_block *nb,
                         unsigned long action,
                         void *data)
{
    struct usb_device *udev = (struct usb_device *)data;

    if (!udev)
        return NOTIFY_DONE;

    if (action == USB_DEVICE_ADD) {
        pr_info(DRV_NAME ": ADD vid=0x%04x pid=0x%04x\n",
                le16_to_cpu(udev->descriptor.idVendor),
                le16_to_cpu(udev->descriptor.idProduct));
    } else if (action == USB_DEVICE_REMOVE) {
        pr_info(DRV_NAME ": REMOVE vid=0x%04x pid=0x%04x\n",
                le16_to_cpu(udev->descriptor.idVendor),
                le16_to_cpu(udev->descriptor.idProduct));
    }

    return NOTIFY_OK;
}

static struct notifier_block usb_nb = {
    .notifier_call = usb_notify_cb,
};

static int __init usb_detect_init(void)
{
    pr_info(DRV_NAME ": init\n");
    usb_register_notify(&usb_nb);   // Now void in 6.x
    return 0;
}

static void __exit usb_detect_exit(void)
{
    usb_unregister_notify(&usb_nb);
    pr_info(DRV_NAME ": exit\n");
}

module_init(usb_detect_init);
module_exit(usb_detect_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Ultra-basic USB add/remove detector (notifier) for Linux 6.8");
