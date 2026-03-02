#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#define DR      0x00
#define FR      0x18

#define FR_TXFF (1 << 5)
#define FR_RXFE (1 << 4)

static void __iomem *uart_base;

/* -------- TX Function -------- */
static void uart_send_char(char c)
{
    /* Wait until TX FIFO not full */
    while (readl(uart_base + FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart_base + DR);
}

/* -------- RX Function -------- */
static char uart_recv_char(void)
{
    /* Wait until RX FIFO not empty */
    while (readl(uart_base + FR) & FR_RXFE)
        cpu_relax();

    return readl(uart_base + DR);
}

/* -------- Probe Function -------- */
static int uart_probe(struct platform_device *pdev)
{
    struct resource *res;

    pr_info("UART0 driver probe called\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    uart_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(uart_base))
        return PTR_ERR(uart_base);

    pr_info("UART0 mapped successfully\n");

    /* Validation TX */
    uart_send_char('H');
    uart_send_char('I');
    uart_send_char('\n');

    pr_info("Sent HI via UART0\n");
    char c;

    uart_send_char('A');
    c = uart_recv_char();
    pr_info("Received: %c\n", c);
    return 0;
}

/* -------- Remove Function -------- */
static void uart_remove(struct platform_device *pdev)
{
    pr_info("UART0 driver removed\n");
}

/* -------- OF Match Table -------- */
static const struct of_device_id uart_of_match[] = {
    { .compatible = "brcm,bcm2835-pl011" },
    { },
};
MODULE_DEVICE_TABLE(of, uart_of_match);

/* -------- Platform Driver -------- */
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
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("UART0 Platform Driver for Raspberry Pi");