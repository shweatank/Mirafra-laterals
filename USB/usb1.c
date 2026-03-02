/*
 * usb_detect.c
 * Simple USB detection driver
 * Compatible with Linux 6.x
 *
 * Build:
 * make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define DRIVER_NAME "usb_detect"

/* Replace with specific Vendor/Product if needed */
#define USB_VENDOR_ID_ANY   USB_ANY_ID
#define USB_PRODUCT_ID_ANY  USB_ANY_ID

/* USB Device ID Table */
static const struct usb_device_id usb_detect_table[] = {
    { USB_DEVICE(USB_VENDOR_ID_ANY, USB_PRODUCT_ID_ANY) },
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, usb_detect_table);

/* Probe Function - Called When Device is Plugged */
static int usb_detect_probe(struct usb_interface *interface,
                            const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);

    pr_info("USB Detect: Device Inserted\n");
    pr_info("Vendor ID  : 0x%04x\n", le16_to_cpu(udev->descriptor.idVendor));
    pr_info("Product ID : 0x%04x\n", le16_to_cpu(udev->descriptor.idProduct));

    if (udev->manufacturer)
        pr_info("Manufacturer: %s\n", udev->manufacturer);

    if (udev->product)
        pr_info("Product     : %s\n", udev->product);

    return 0;
}

/* Disconnect Function - Called When Device Removed */
static void usb_detect_disconnect(struct usb_interface *interface)
{
    pr_info("USB Detect: Device Removed\n");
}

/* USB Driver Structure */
static struct usb_driver usb_detect_driver = {
    .name       = DRIVER_NAME,
    .probe      = usb_detect_probe,
    .disconnect = usb_detect_disconnect,
    .id_table   = usb_detect_table,
};

/* Module Init */
static int __init usb_detect_init(void)
{
    pr_info("USB Detect Driver Init\n");
    return usb_register(&usb_detect_driver);
}

/* Module Exit */
static void __exit usb_detect_exit(void)
{
    pr_info("USB Detect Driver Exit\n");
    usb_deregister(&usb_detect_driver);
}

module_init(usb_detect_init);
module_exit(usb_detect_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Simple USB Detection Driver");
