#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE_PHYS 0xFE200000
#define GPIO_SIZE      0x100

/* Offsets */
#define GPFSEL1 0x04
#define GPFSEL2 0x08
#define GPSET0  0x1C
#define GPCLR0  0x28
#define GPLEV0  0x34

static void __iomem *gpio_base;

static int __init gpio_test_init(void)
{
    u32 val;

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("GPIO ioremap failed\n");
        return -ENOMEM;
    }

    pr_info("GPIO mapped\n");


    val = readl(gpio_base + GPFSEL2);
    val &= ~(7 << 21);     // clear FSEL27
    val |=  (1 << 21);     // output
    writel(val, gpio_base + GPFSEL2);


    val = readl(gpio_base + GPFSEL1);
    val &= ~(7 << 21);     // clear → input
    writel(val, gpio_base + GPFSEL1);


    for(int i = 0; i < 20; i++)
    {
        writel(1 << 27, gpio_base + GPSET0);
        msleep(500);

        val = readl(gpio_base + GPLEV0);
        pr_info("GPIO17 state = %d\n", !!(val & (1 << 17)));

        writel(1 << 27, gpio_base + GPCLR0);
        msleep(500);

        val = readl(gpio_base + GPLEV0);
        pr_info("GPIO17 state = %d\n", !!(val & (1 << 17)));
    }

    return 0;
}

static void __exit gpio_test_exit(void)
{
    iounmap(gpio_base);
    pr_info("GPIO module removed\n");
}

module_init(gpio_test_init);
module_exit(gpio_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIKHIL_SINGH");
MODULE_DESCRIPTION("GPIO27 toggle + GPIO17 read");
