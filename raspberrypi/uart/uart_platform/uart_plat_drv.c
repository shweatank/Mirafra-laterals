#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>

#define UART_DR   0x00
#define UART_FR   0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR   0x30
#define UART_ICR  0x44

#define FR_TXFF (1<<5)

struct uart_dev {
    void __iomem *base;
};

/* ---------- UART TX ---------- */
static void uart_putc(struct uart_dev *dev, char c)
{
    while (readl(dev->base + UART_FR) & FR_TXFF);
    writel(c, dev->base + UART_DR);
}

static void uart_puts(struct uart_dev *dev, const char *s)
{
    while (*s)
        uart_putc(dev, *s++);
}

/* ---------- PROBE ---------- */
static int uart_probe(struct platform_device *pdev)
{
    struct uart_dev *dev;
    struct resource *res;

    pr_info("UART platform driver probe\n");

    dev = devm_kzalloc(&pdev->dev,
                       sizeof(*dev),
                       GFP_KERNEL);

    res = platform_get_resource(pdev,
                                IORESOURCE_MEM,
                                0);

    dev->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(dev->base))
        return PTR_ERR(dev->base);

    /* UART INIT */
    writel(0, dev->base + UART_CR);
    writel(0x7FF, dev->base + UART_ICR);

    writel(26, dev->base + UART_IBRD);
    writel(3,  dev->base + UART_FBRD);

    writel((1<<4)|(1<<5)|(1<<6),
           dev->base + UART_LCRH);

    writel((1<<0)|(1<<8)|(1<<9),
           dev->base + UART_CR);

    pr_info("UART initialized\n");

    uart_puts(dev,"\r\nHello from UART DTS Driver\r\n");

    return 0;
}

/* ---------- REMOVE ---------- */
static int uart_remove(struct platform_device *pdev)
{
    pr_info("UART driver removed\n");
    return 0;
}

/* ---------- DT MATCH ---------- */
static const struct of_device_id uart_of_match[] = {
    { .compatible = "karthik,uart-demo" },
    {},
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_driver = {
    .probe  = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = "uart_demo",
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple UART Platform Driver using DTS");
