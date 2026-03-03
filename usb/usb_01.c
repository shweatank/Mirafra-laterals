#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define VENDOR_ID  0x062a   // Change to your device VID
#define PRODUCT_ID 0x4c01   // Change to your device PID

static int simple_usb_probe(struct usb_interface *interface,
                            const struct usb_device_id *id)
{
    struct usb_device *udev =
        interface_to_usbdev(interface);

    pr_info("USB Device Plugged\n");
    pr_info("Vendor ID: 0x%04x\n", id->idVendor);
    pr_info("Product ID: 0x%04x\n", id->idProduct);

    pr_info("USB Device Speed: %d\n", udev->speed);

    return 0;
}

static void simple_usb_disconnect(struct usb_interface *interface)
{
    pr_info("USB Device Removed\n");
}

/* USB Device Table */
static struct usb_device_id simple_usb_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    {}
};

MODULE_DEVICE_TABLE(usb, simple_usb_table);

static struct usb_driver simple_usb_driver = {
    .name       = "simple_usb_driver",
    .probe      = simple_usb_probe,
    .disconnect = simple_usb_disconnect,
    .id_table   = simple_usb_table,
};

module_usb_driver(simple_usb_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple USB Device Driver");
