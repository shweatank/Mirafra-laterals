#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE_PHYS 0xFE200000
#define GPIO_SIZE      0x1000

#define GPFSEL1 0x04
#define GPFSEL2 0x08
#define GPSET0  0x1C
#define GPCLR0  0x28
#define GPLEV0  0x34
#define GPIO_PUP_PDN_CNTRL_REG0 0xE4

#define GPIO_INPUT   17
#define GPIO_OUTPUT  27

static void __iomem *gpio_base;

static int __init gpio_test_init(void)
{
    u32 val;
    int prev, curr;

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base)
        return -ENOMEM;

    pr_info("GPIO mapped\n");

    /* GPIO27 as OUTPUT */
    val = readl(gpio_base + GPFSEL2);
    val &= ~(7 << 21);
    val |=  (1 << 21);
    writel(val, gpio_base + GPFSEL2);

    /* GPIO17 as INPUT */
    val = readl(gpio_base + GPFSEL1);
    val &= ~(7 << 21);
    writel(val, gpio_base + GPFSEL1);

    /* Enable Pull-Up on GPIO17 */
    val = readl(gpio_base + GPIO_PUP_PDN_CNTRL_REG0);
    val &= ~(3 << 2);      // clear bits
    val |=  (1 << 2);      // set pull-up (01)
    writel(val, gpio_base + GPIO_PUP_PDN_CNTRL_REG0);

    pr_info("GPIO17=INPUT with PULLUP\n");

    prev = !!(readl(gpio_base + GPLEV0) & (1 << GPIO_INPUT));

    while (1) {

        curr = !!(readl(gpio_base + GPLEV0) & (1 << GPIO_INPUT));

        if (curr != prev) {

            if (!curr) {   // LOW detected (GND touch)
                pr_info("GPIO17 connected to GND\n");
                writel(1 << GPIO_OUTPUT, gpio_base + GPSET0);
            } else {
                pr_info("GPIO17 released\n");
                writel(1 << GPIO_OUTPUT, gpio_base + GPCLR0);
            }

            prev = curr;
            msleep(200);
        }

        msleep(50);
    }

    return 0;
}

static void __exit gpio_test_exit(void)
{
    if (gpio_base)
        iounmap(gpio_base);

    pr_info("Module removed\n");
}

module_init(gpio_test_init);
module_exit(gpio_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shashank");
MODULE_DESCRIPTION("GPIO17 interrupt check using ioreamp");
