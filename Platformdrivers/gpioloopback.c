#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE   0xFE200000
#define GPIO_SIZE   0x100

/* Registers */
#define GPFSEL1   0x04
#define GPFSEL2   0x08
#define GPSET0    0x1C
#define GPCLR0    0x28
#define GPLEV0    0x34

#define OUT_GPIO 17
#define IN_GPIO  27

static void __iomem *gpio;

static void gpio_config(void)
{
    u32 val;

    /* Configure GPIO17 as output */
    val = readl(gpio + GPFSEL1);
    val &= ~(7 << 21);          // Clear bits for GPIO17
    val |=  (1 << 21);          // Set as output (001)
    writel(val, gpio + GPFSEL1);

    /* Configure GPIO27 as input */
    val = readl(gpio + GPFSEL2);
    val &= ~(7 << 21);          // Clear bits for GPIO27 (27-10=17 -> wrong)
    writel(val, gpio + GPFSEL2);
}

static int __init gpio_init(void)
{
    u32 level;

    pr_info("GPIO ioremap Loopback Init\n");

    gpio = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio)
        return -ENOMEM;

    /* GPIO17 = output (GPFSEL1 bits 23:21) */
    {
        u32 val = readl(gpio + GPFSEL1);
        val &= ~(7 << 21);
        val |=  (1 << 21);
        writel(val, gpio + GPFSEL1);
    }

    /* GPIO27 = input (GPFSEL2 bits 21:23) */
    {
        u32 val = readl(gpio + GPFSEL2);
        val &= ~(7 << 21);
        writel(val, gpio + GPFSEL2);
    }

    msleep(100);

    /* Set GPIO17 high */
    writel(1 << OUT_GPIO, gpio + GPSET0);
    msleep(10);

    level = readl(gpio + GPLEV0);

    pr_info("Write 1 → Read %d\n",(level & (1 << IN_GPIO)) ? 1 : 0);

    /* Set GPIO17 low */
    writel(1 << OUT_GPIO, gpio + GPCLR0);
    msleep(10);

    level = readl(gpio + GPLEV0);

    pr_info("Write 0 → Read %d\n",(level & (1 << IN_GPIO)) ? 1 : 0);

    pr_info("GPIO Loopback Done\n");

    return 0;
}

static void __exit gpio_exit(void)
{
    if (gpio)
        iounmap(gpio);

    pr_info("GPIO Loopback Exit\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jerin Jose");
MODULE_DESCRIPTION("GPIO Loopback using ioremap");
