// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>


#define DRIVER_NAME "rpi4_gpio_ioremap"

/* BCM2711 GPIO Base */
#define GPIO_BASE_PHYS  0xFE200000
#define GPIO_REGION_SIZE 0x100

/* GPIO Register Offsets */
#define GPFSEL1   0x04
#define GPEDS0    0x40
#define GPREN0    0x4C

#define GPIO_PIN 25

static void __iomem *gpio_base;
static int irq_number;

/* Interrupt Handler */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    u32 status;

    status = readl(gpio_base + GPEDS0);

    if (status & (1 << GPIO_PIN)) {
        pr_info("GPIO17 Interrupt Triggered!\n");

        /* Clear interrupt by writing 1 */
        writel((1 << GPIO_PIN), gpio_base + GPEDS0);

        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static int __init gpio_ioremap_init(void)
{
    u32 val;

    pr_info("RPI4 GPIO ioremap IRQ Driver Init\n");

    /* Map GPIO registers */
    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_REGION_SIZE);
    if (!gpio_base) {
        pr_err("Failed to ioremap GPIO\n");
        return -ENOMEM;
    }

    /* -------- Configure GPIO17 as INPUT -------- */
    val = readl(gpio_base + GPFSEL1);
    val &= ~(7 << 21);     // Clear bits 21-23
    writel(val, gpio_base + GPFSEL1);

    /* -------- Enable Rising Edge Detect -------- */
    val = readl(gpio_base + GPREN0);
    val |= (1 << GPIO_PIN);
    writel(val, gpio_base + GPREN0);

    /* -------- Get IRQ number from device tree -------- */
    irq_number = irq_of_parse_and_map(of_find_compatible_node(NULL, NULL, "brcm,bcm2711-gpio"), 0);
    if (!irq_number) {
        pr_err("Failed to map IRQ\n");
        iounmap(gpio_base);
        return -EINVAL;
    }

    pr_info("Mapped IRQ: %d\n", irq_number);

    /* -------- Request IRQ -------- */
    if (request_irq(irq_number,
                    gpio_irq_handler,
                    IRQF_SHARED,
                    DRIVER_NAME,
                    &gpio_ioremap_init)) {
        pr_err("Failed to request IRQ\n");
        iounmap(gpio_base);
        return -EBUSY;
    }

    pr_info("GPIO17 Interrupt Driver Loaded\n");
    return 0;
}

static void __exit gpio_ioremap_exit(void)
{
    free_irq(irq_number, &gpio_ioremap_init);
    iounmap(gpio_base);
    pr_info("GPIO17 Interrupt Driver Removed\n");
}

module_init(gpio_ioremap_init);
module_exit(gpio_ioremap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Raspberry Pi 4B BCM2711 GPIO Interrupt using ioremap only");
