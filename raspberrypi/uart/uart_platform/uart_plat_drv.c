#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>

#define UART_DR 0x00
#define UART_FR 0x18
#define FR_TXFF (1 << 5)

static void __iomem *uart_base;

/* ---------- UART TX ---------- */
static void uart_putc(char c)
{
    while (readl(uart_base + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart_base + UART_DR);
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

/* ---------- PROBE ---------- */
static int uart_probe(struct platform_device *pdev)
{
    struct resource *res;

    pr_info("UART Platform Driver Probe\n");

    /* get memory resource from device tree */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    uart_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(uart_base))
        return PTR_ERR(uart_base);

    uart_puts("Platform UART Driver Active\r\n");

    return 0;
}

/* ---------- REMOVE ---------- */
static int uart_remove(struct platform_device *pdev)
{
    pr_info("UART Platform Driver Removed\n");
    return 0;
}

/* ---------- DEVICE TREE MATCH ---------- */
static const struct of_device_id uart_of_match[] = {
    { .compatible = "karthik,uart-demo" },
    {},
};

MODULE_DEVICE_TABLE(of, uart_of_match);

/* ---------- PLATFORM DRIVER ---------- */
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
MODULE_DESCRIPTION("UART Platform Driver Example");
