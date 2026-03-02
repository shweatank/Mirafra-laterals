#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/delay.h>

/* UART Registers */
#define UART_DR     0x00
#define UART_FR     0x18
#define UART_IBRD   0x24
#define UART_FBRD   0x28
#define UART_LCRH   0x2C
#define UART_CR     0x30
#define UART_ICR    0x44

#define FR_TXFF     (1 << 5)
#define FR_RXFE     (1 << 4)

/* GPIO */
#define GPIO_BASE_PHYS  0xFE200000
#define GPIO_SIZE       0xB4
#define GPFSEL1         0x04   // GPIO10–19

static void __iomem *uart_base;
static void __iomem *gpio_base;

/* Set GPIO14 & GPIO15 to ALT0 */
static void configure_uart_pins(void)
{
    u32 val;

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("GPIO ioremap failed\n");
        return;
    }

    val = readl(gpio_base + GPFSEL1);

    /* Clear bits for GPIO14 (bits 12–14) */
    val &= ~(0x7 << 12);
    /* Clear bits for GPIO15 (bits 15–17) */
    val &= ~(0x7 << 15);

    /* Set ALT0 (100) */
    val |= (0x4 << 12);  // GPIO14 ALT0
    val |= (0x4 << 15);  // GPIO15 ALT0

    writel(val, gpio_base + GPFSEL1);

    pr_info("GPIO14 & GPIO15 set to ALT0\n");
}

static void uart_putc(char c)
{
    while (readl(uart_base + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart_base + UART_DR);
}

static char uart_getc(void)
{
    while (readl(uart_base + UART_FR) & FR_RXFE)
        cpu_relax();

    return readl(uart_base + UART_DR) & 0xFF;
}

static int uart_probe(struct platform_device *pdev)
{
    struct resource *res;
    char tx = 'A';
    char rx;

    pr_info("Rani UART Probe Called\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res)
        return -ENODEV;

    uart_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(uart_base))
        return PTR_ERR(uart_base);

    /* Configure GPIO pins manually */
    configure_uart_pins();

    /* Disable UART */
    writel(0x0, uart_base + UART_CR);

    /* Clear interrupts */
    writel(0x7FF, uart_base + UART_ICR);

    /* 115200 baud @ 48MHz */
    writel(26, uart_base + UART_IBRD);
    writel(3,  uart_base + UART_FBRD);

    /* 8N1 + FIFO */
    writel((1 << 4) | (1 << 5) | (1 << 6),
           uart_base + UART_LCRH);

    /* Enable UART, TX, RX */
    writel((1 << 0) | (1 << 8) | (1 << 9),
           uart_base + UART_CR);

    pr_info("Transmitting character: %c\n", tx);

    uart_putc(tx);

    udelay(100);

    rx = uart_getc();

    pr_info("Received character: %c\n", rx);

    if (rx == tx)
        pr_info("Loopback SUCCESS\n");
    else
        pr_info("Loopback FAILED\n");

    return 0;
}

static void uart_remove(struct platform_device *pdev)
{
    writel(0x0, uart_base + UART_CR);

    if (gpio_base)
        iounmap(gpio_base);

    pr_info("Rani UART Removed\n");
}

static const struct of_device_id uart_of_match[] = {
    { .compatible = "brcm,rani-uart" },
    { }
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_driver = {
    .probe  = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = "rani_uart",
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("UART TX/RX Loopback with Manual GPIO ALT0 Config");
