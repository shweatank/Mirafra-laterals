#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE_PHYS 0xFE200000   // BCM2711 (Raspberry Pi 4)
#define GPIO_SIZE      0xB4

/* Register offsets */
#define GPSET0  0x1C
#define GPCLR0  0x28
#define GPLEV0  0x34

#define GPIO_OUT 17
#define GPIO_IN  27

static void __iomem *gpio_base;

/* ---------- GPIO CONFIG ---------- */

static void gpio_set_output(int pin)
{
    u32 reg = pin / 10;
    u32 shift = (pin % 10) * 3;
    u32 val;

    val = readl(gpio_base + reg * 4);
    val &= ~(7 << shift);
    val |=  (1 << shift);   // 001 = output
    writel(val, gpio_base + reg * 4);
}

static void gpio_set_input(int pin)
{
    u32 reg = pin / 10;
    u32 shift = (pin % 10) * 3;
    u32 val;

    val = readl(gpio_base + reg * 4);
    val &= ~(7 << shift);   // 000 = input
    writel(val, gpio_base + reg * 4);
}

/* ---------- GPIO ACCESS ---------- */

static void gpio_write(int pin, int value)
{
    if (value)
        writel(1 << pin, gpio_base + GPSET0);
    else
        writel(1 << pin, gpio_base + GPCLR0);
}

static int gpio_read(int pin)
{
    u32 val = readl(gpio_base + GPLEV0);
    return (val & (1 << pin)) ? 1 : 0;
}

/* ---------- MODULE INIT ---------- */

static int __init gpio_loopback_init(void)
{
    int val;

    pr_info("GPIO Loopback Test Start\n");

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("ioremap failed\n");
        return -ENOMEM;
    }

    gpio_set_output(GPIO_OUT);
    gpio_set_input(GPIO_IN);

    /* Write HIGH */
    gpio_write(GPIO_OUT, 1);
    udelay(10);
    val = gpio_read(GPIO_IN);
    pr_info("Write 1 -> Read %d\n", val);

    /* Write LOW */
    gpio_write(GPIO_OUT, 0);
    udelay(10);
    val = gpio_read(GPIO_IN);
    pr_info("Write 0 -> Read %d\n", val);

    pr_info("GPIO Loopback Test Done\n");
    return 0;
}

/* ---------- MODULE EXIT ---------- */

static void __exit gpio_loopback_exit(void)
{
    if (gpio_base)
        iounmap(gpio_base);

    pr_info("GPIO module removed\n");
}

module_init(gpio_loopback_init);
module_exit(gpio_loopback_exit);

MODULE_LICENSE("GPL");
