#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/delay.h>

/* PL011 Registers */
#define UART_DR     0x00
#define UART_FR     0x18
#define UART_IBRD   0x24
#define UART_FBRD   0x28
#define UART_LCRH   0x2C
#define UART_CR     0x30
#define UART_ICR    0x44

#define FR_TXFF     (1 << 5)
#define FR_RXFE     (1 << 4)

struct uart_priv {
    void __iomem *base;
};

static void uart_putc(void __iomem *base, char c)
{
    while (readl(base + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, base + UART_DR);
}

static char uart_getc(void __iomem *base)
{
    while (readl(base + UART_FR) & FR_RXFE)
        cpu_relax();

    return readl(base + UART_DR) & 0xFF;
}

static int uart_probe(struct platform_device *pdev)
{
    struct uart_priv *priv;
    char tx = 'A';
    char rx;

    dev_info(&pdev->dev, "UART probe called\n");

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    priv->base = devm_platform_ioremap_resource(pdev, 0);
    if (IS_ERR(priv->base))
        return PTR_ERR(priv->base);

    platform_set_drvdata(pdev, priv);

    /* Disable UART */
    writel(0x0, priv->base + UART_CR);

    /* Clear interrupts */
    writel(0x7FF, priv->base + UART_ICR);

    /* 115200 baud (48MHz clock assumption) */
    writel(26, priv->base + UART_IBRD);
    writel(3,  priv->base + UART_FBRD);

    /* 8N1 + FIFO */
    writel((1 << 4) | (1 << 5) | (1 << 6),
           priv->base + UART_LCRH);

    /* Enable UART, TX, RX */
    writel((1 << 0) | (1 << 8) | (1 << 9),
           priv->base + UART_CR);

    dev_info(&pdev->dev, "Sending: %c\n", tx);

    uart_putc(priv->base, tx);

    udelay(100);

    rx = uart_getc(priv->base);

    dev_info(&pdev->dev, "Received: %c\n", rx);

    if (rx == tx)
        dev_info(&pdev->dev, "Loopback SUCCESS\n");
    else
        dev_err(&pdev->dev, "Loopback FAILED\n");

    return 0;
}

static void uart_remove(struct platform_device *pdev)
{
    struct uart_priv *priv = platform_get_drvdata(pdev);

    writel(0x0, priv->base + UART_CR);
    dev_info(&pdev->dev, "UART removed\n");
    
}

static const struct of_device_id uart_of_match[] = {
    { .compatible = "brcm,isshika-uart" },
    { }
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_driver = {
    .probe  = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = "isshika_uart",
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("isshika sinha");
MODULE_DESCRIPTION("DT-based UART Loopback Driver (No Manual ioremap)");
