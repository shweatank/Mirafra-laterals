#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/delay.h>

#define UART2_BASE 0xFE201400
#define UART_SIZE  0x90

#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_ICR   0x44

#define FR_TXFF (1<<5)
#define FR_RXFE (1<<4)

struct mydev {
    struct gpio_desc *out;
    struct gpio_desc *irq_gpio;
    int irq;
    void __iomem *uart2;
};

static irqreturn_t gpio_irq_handler(int irq, void *data)
{
    struct mydev *dev = data;
    char rx;

    pr_info("GPIO Interrupt Triggered\n");

    /* UART transmit */
    while (readl(dev->uart2 + UART_FR) & FR_TXFF);
    writel('K', dev->uart2 + UART_DR);

    msleep(10);

    /* UART receive */
    while (readl(dev->uart2 + UART_FR) & FR_RXFE);
    rx = readl(dev->uart2 + UART_DR) & 0xFF;

    pr_info("UART Loopback RX = %c\n", rx);

    return IRQ_HANDLED;
}

static int my_probe(struct platform_device *pdev)
{
    struct mydev *dev;
    int ret;

    pr_info("GPIO-UART IRQ Driver Probe\n");

    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);

    dev->out = devm_gpiod_get(&pdev->dev, "out", GPIOD_OUT_LOW);
    dev->irq_gpio = devm_gpiod_get(&pdev->dev, "irq", GPIOD_IN);

    dev->irq = gpiod_to_irq(dev->irq_gpio);

    ret = devm_request_irq(&pdev->dev,
                           dev->irq,
                           gpio_irq_handler,
                           IRQF_TRIGGER_RISING,
                           "gpio_uart_irq",
                           dev);
    if (ret)
        return ret;

    /* UART2 setup */
    dev->uart2 = ioremap(UART2_BASE, UART_SIZE);

    writel(0, dev->uart2 + UART_CR);
    writel(0x7FF, dev->uart2 + UART_ICR);
    writel(26, dev->uart2 + UART_IBRD);
    writel(3,  dev->uart2 + UART_FBRD);
    writel((1<<4)|(1<<5)|(1<<6), dev->uart2 + UART_LCRH);
    writel((1<<0)|(1<<8)|(1<<9), dev->uart2 + UART_CR);

    pr_info("UART2 initialized\n");

    /* Trigger interrupt by toggling output */
    gpiod_set_value(dev->out, 1);
    msleep(50);
    gpiod_set_value(dev->out, 0);

    pr_info("Output toggled to trigger IRQ\n");

    return 0;
}

static const struct of_device_id my_of_match[] = {
    { .compatible = "karthik,gpio-uart-irq" },
    {},
};
MODULE_DEVICE_TABLE(of, my_of_match);

static struct platform_driver my_driver = {
    .probe = my_probe,
    .driver = {
        .name = "gpio_uart_irq",
        .of_match_table = my_of_match,
    },
};

module_platform_driver(my_driver);

MODULE_LICENSE("GPL");
