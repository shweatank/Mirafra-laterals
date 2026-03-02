#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define DRIVER_NAME "usb_detect_demo"

// ✅ Replace with your device's VID/PID from `lsusb`
#define MY_USB_VID  0x30fa
#define MY_USB_PID  0x0400

// 1) USB Device ID table: tells kernel which devices this driver supports
static const struct usb_device_id usb_detect_table[] = {
    { USB_DEVICE(MY_USB_VID, MY_USB_PID) },
    { } // terminating entry
};
MODULE_DEVICE_TABLE(usb, usb_detect_table);

// 2) probe() is called when a matching device is plugged in
static int usb_detect_probe(struct usb_interface *intf,
                            const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(intf);

    pr_info(DRIVER_NAME ": ✅ Device CONNECTED! VID=0x%04x PID=0x%04x\n",
            le16_to_cpu(udev->descriptor.idVendor),
            le16_to_cpu(udev->descriptor.idProduct));

    pr_info(DRIVER_NAME ": Bus=%03d Device=%03d, Manufacturer=%s, Product=%s, Serial=%s\n",
            udev->bus->busnum,
            udev->devnum,
            udev->manufacturer ? udev->manufacturer : "(none)",
            udev->product ? udev->product : "(none)",
            udev->serial ? udev->serial : "(none)");

    // You can also inspect endpoints if you want (later step)
    // struct usb_host_interface *iface_desc = intf->cur_altsetting;

    return 0; // success -> driver binds to this interface
}

// 3) disconnect() is called when device is removed
static void usb_detect_disconnect(struct usb_interface *intf)
{
    pr_info(DRIVER_NAME ": ❌ Device DISCONNECTED!\n");
}

// 4) Register the USB driver with kernel
static struct usb_driver usb_detect_driver = {
    .name       = DRIVER_NAME,
    .probe      = usb_detect_probe,
    .disconnect = usb_detect_disconnect,
    .id_table   = usb_detect_table,
};

static int __init usb_detect_init(void)
{
    pr_info(DRIVER_NAME ": init\n");
    return usb_register(&usb_detect_driver);
}

static void __exit usb_detect_exit(void)
{
    pr_info(DRIVER_NAME ": exit\n");
    usb_deregister(&usb_detect_driver);
}

module_init(usb_detect_init);
module_exit(usb_detect_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("USB driver: detects a specific USB VID/PID on insertion");