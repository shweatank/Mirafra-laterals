#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE_PHYS  0xFE200000
#define GPIO_SIZE       0xB4

/* Registers */
#define GPFSEL1 0x04
#define GPSET0  0x1C
#define GPCLR0  0x28

#define GPIO_PIN 17

static void __iomem *gpio_base;

/* -------- Set GPIO as Output -------- */
static void gpio_output(void)
{
    u32 val;

    val = readl(gpio_base + GPFSEL1);

    /* clear bits */
    val &= ~(7 << 21);

    /* set as output (001) */
    val |= (1 << 21);

    writel(val, gpio_base + GPFSEL1);
}

/* -------- GPIO HIGH -------- */
static void gpio_set(void)
{
    writel(1 << GPIO_PIN, gpio_base + GPSET0);
}

/* -------- GPIO LOW -------- */
static void gpio_clear(void)
{
    writel(1 << GPIO_PIN, gpio_base + GPCLR0);
}

/* -------- INIT -------- */
static int __init gpio_init(void)
{
    pr_info("RPI4 GPIO ioremap Driver Loaded\n");

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);

    if (!gpio_base)
        return -ENOMEM;

    gpio_output();

    /* Blink LED */
    gpio_set();
    msleep(500);
    gpio_clear();
    msleep(500);
    gpio_set();

    pr_info("GPIO17 toggled\n");

    return 0;
}

/* -------- EXIT -------- */
static void __exit gpio_exit(void)
{
    gpio_clear();

    if (gpio_base)
        iounmap(gpio_base);

    pr_info("GPIO Driver Removed\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Raspberry Pi 4 GPIO Driver using ioremap");
