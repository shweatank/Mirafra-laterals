// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define DRIVER_NAME "rpi4_keyboard_style_irq"

/* GPIO Definitions */
#define GPIO_OUT   23
#define GPIO_IN    24
#define GPIO_IRQ   25

static int irq_number;

/* -------------------------------------------------- */
/* IRQ Handler                                        */
/* -------------------------------------------------- */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    pr_info("Interrupt triggered! (Keyboard-style event)\n");
    return IRQ_HANDLED;
}

/* -------------------------------------------------- */
/* Init                                               */
/* -------------------------------------------------- */
static int __init gpio_irq_init(void)
{
    int ret;

    pr_info("Keyboard-style GPIO IRQ Driver Init\n");

    /* Request GPIOs */
    gpio_request(GPIO_OUT, "gpio_out");
    gpio_request(GPIO_IN,  "gpio_in");
    gpio_request(GPIO_IRQ, "gpio_irq");

    /* Configure directions */
    gpio_direction_output(GPIO_OUT, 0);
    gpio_direction_input(GPIO_IN);
    gpio_direction_input(GPIO_IRQ);

    /* Get IRQ number for interrupt pin */
    irq_number = gpio_to_irq(GPIO_IRQ);
    if (irq_number < 0) {
        pr_err("Failed to map IRQ\n");
        return irq_number;
    }

    pr_info("Mapped IRQ: %d\n", irq_number);

    /* Request interrupt (rising edge) */
    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_RISING,
                      DRIVER_NAME,
                      NULL);
    if (ret) {
        pr_err("Failed to request IRQ\n");
        return ret;
    }

    pr_info("Driver loaded successfully\n");

    /* ---- Simulate keyboard event ---- */
    pr_info("Generating test pulse...\n");

    gpio_set_value(GPIO_OUT, 1);
    msleep(100);
    gpio_set_value(GPIO_OUT, 0);

    return 0;
}

/* -------------------------------------------------- */
/* Exit                                               */
/* -------------------------------------------------- */
static void __exit gpio_irq_exit(void)
{
    free_irq(irq_number, NULL);

    gpio_free(GPIO_OUT);
    gpio_free(GPIO_IN);
    gpio_free(GPIO_IRQ);

    pr_info("Driver removed\n");
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Updated for RPi4");
MODULE_DESCRIPTION("RPi4 Keyboard-style GPIO Interrupt Demo");
