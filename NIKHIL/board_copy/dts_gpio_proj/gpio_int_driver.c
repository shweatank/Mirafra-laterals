// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#define DRIVER_NAME "gpio_int_driver"

static struct gpio_desc *gpio_desc;
static int irq_number;

/* Interrupt handler */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    pr_info("GPIO Interrupt Occurred! IRQ: %d\n", irq);
    return IRQ_HANDLED;
}

static int gpio_int_probe(struct platform_device *pdev)
{
    int ret;

    pr_info("GPIO Interrupt Driver Probe Start\n");

    /* Get GPIO from device tree */
    gpio_desc = gpiod_get(&pdev->dev, NULL, GPIOD_IN);
    if (IS_ERR(gpio_desc)) {
        pr_err("Failed to get GPIO\n");
        return PTR_ERR(gpio_desc);
    }

    /* Convert GPIO to IRQ */
    irq_number = gpiod_to_irq(gpio_desc);
    if (irq_number < 0) {
        pr_err("Failed to get IRQ number\n");
        gpiod_put(gpio_desc);
        return irq_number;
    }

    pr_info("GPIO mapped to IRQ: %d\n", irq_number);

    /* Request IRQ */
    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_RISING,
                      DRIVER_NAME,
                      NULL);
    if (ret) {
        pr_err("Failed to request IRQ\n");
        gpiod_put(gpio_desc);
        return ret;
    }

    pr_info("GPIO Interrupt Driver Loaded Successfully\n");
    return 0;
}

static void gpio_int_remove(struct platform_device *pdev)
{
    free_irq(irq_number, NULL);
    gpiod_put(gpio_desc);
    pr_info("GPIO Interrupt Driver Removed\n");
    //return 0;
}

static const struct of_device_id gpio_dt_ids[] = {
    { .compatible = "techdhaba,gpio-int" },
    { }
};
MODULE_DEVICE_TABLE(of, gpio_dt_ids);

static struct platform_driver gpio_int_driver = {
    .probe  = gpio_int_probe,
    .remove = gpio_int_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = gpio_dt_ids,
    },
};

module_platform_driver(gpio_int_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIKHIL");
MODULE_DESCRIPTION("GPIO Interrupt Driver for Raspberry Pi 4B (BCM2711)");
