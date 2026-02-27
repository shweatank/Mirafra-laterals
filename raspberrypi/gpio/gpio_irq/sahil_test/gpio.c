#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

static struct gpio_desc *gpio_out;
static struct gpio_desc *gpio_irq;
static int irq_number;

static irqreturn_t irq_handler(int irq, void *dev_id)
{
    pr_info("Interrupt Triggered!\n");
    return IRQ_HANDLED;
}

static int __init test_init(void)
{
    int ret;

    gpio_out = gpio_to_desc(23);
    gpio_irq = gpio_to_desc(25);

    if (!gpio_out || !gpio_irq)
        return -EINVAL;

    gpiod_direction_output(gpio_out, 0);
    gpiod_direction_input(gpio_irq);

    irq_number = gpiod_to_irq(gpio_irq);
    if (irq_number < 0)
        return irq_number;

    ret = request_irq(irq_number,
                      irq_handler,
                      IRQF_TRIGGER_RISING,
                      "gpio_irq_test",
                      NULL);
    if (ret)
        return ret;

    gpiod_set_value(gpio_out, 1);
    msleep(50);
    gpiod_set_value(gpio_out, 0);

    return 0;
}

static void __exit test_exit(void)
{
    free_irq(irq_number, NULL);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
