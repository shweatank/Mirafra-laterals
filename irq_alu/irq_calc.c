

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>       
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/io.h>       
#include <linux/kthread.h>
#include <linux/slab.h>

#define DRIVER_NAME     "irq_calc"
#define IRQ_NUM         1           
#define PS2_DATA_PORT   0x60        
#define INPUT_FILE      "/tmp/calc_input.txt"
#define BUF_SIZE        64

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("IRQ-driven keyboard calculator – results to dmesg");


#define SC_A   0x1E   /* A */
#define SC_S   0x1F   /* S */
#define SC_D   0x20   /* D */
#define SC_M   0x32   /* M */

static long operand_a = 0;
static long operand_b = 0;
static bool operands_loaded = false;


static volatile u8 last_scancode = 0;


static int load_operands(void)
{
    struct file *f;
    char buf[BUF_SIZE];
    ssize_t n;
    loff_t pos = 0;

    f = filp_open(INPUT_FILE, O_RDONLY, 0);
    if (IS_ERR(f)) {
        pr_err("%s: cannot open %s (err %ld)\n",
               DRIVER_NAME, INPUT_FILE, PTR_ERR(f));
        return PTR_ERR(f);
    }

    memset(buf, 0, sizeof(buf));
    n = kernel_read(f, buf, sizeof(buf) - 1, &pos);
    filp_close(f, NULL);

    if (n <= 0) {
        pr_err("%s: read error from %s\n", DRIVER_NAME, INPUT_FILE);
        return -EIO;
    }

    /* Expect format: "A B\n" or "A B" */
    if (sscanf(buf, "%ld %ld", &operand_a, &operand_b) != 2) {
        pr_err("%s: bad format in %s – expected \"<num> <num>\"\n",
               DRIVER_NAME, INPUT_FILE);
        return -EINVAL;
    }

    pr_info("%s: operands loaded  A=%ld  B=%ld\n",
            DRIVER_NAME, operand_a, operand_b);
    return 0;
}


static void do_calc(u8 sc)
{
    switch (sc) {
    case SC_A:
        pr_info("%s: [ADD] %ld + %ld = %ld\n",
                DRIVER_NAME, operand_a, operand_b,
                operand_a + operand_b);
        break;

    case SC_S:
        pr_info("%s: [SUB] %ld - %ld = %ld\n",
                DRIVER_NAME, operand_a, operand_b,
                operand_a - operand_b);
        break;

    case SC_M:
        pr_info("%s: [MUL] %ld * %ld = %ld\n",
                DRIVER_NAME, operand_a, operand_b,
                operand_a * operand_b);
        break;

    case SC_D:
        if (operand_b == 0) {
            pr_warn("%s: [DIV] division by zero!\n", DRIVER_NAME);
        } else {
            pr_info("%s: [DIV] %ld / %ld = %ld (remainder %ld)\n",
                    DRIVER_NAME, operand_a, operand_b,
                    operand_a / operand_b,
                    operand_a % operand_b);
        }
        break;

    default:
        break;
    }
}


static irqreturn_t irq_calc_top(int irq, void *dev_id)
{
    u8 sc = inb(PS2_DATA_PORT);

    
    if (sc & 0x80)
        return IRQ_HANDLED;   

    last_scancode = sc;

    
    if (sc == SC_A || sc == SC_S || sc == SC_M || sc == SC_D)
        return IRQ_WAKE_THREAD;

    return IRQ_HANDLED;
}


static irqreturn_t irq_calc_thread(int irq, void *dev_id)
{
    u8 sc = last_scancode;   

    if (!operands_loaded) {
        pr_warn("%s: operands not loaded yet\n", DRIVER_NAME);
        return IRQ_HANDLED;
    }

    do_calc(sc);
    return IRQ_HANDLED;
}


static int __init irq_calc_init(void)
{
    int ret;

    pr_info("%s: Loaded \n", DRIVER_NAME);

    ret = load_operands();
    if (ret) {
        pr_err("%s: failed to load operands  \n",
               DRIVER_NAME);
        return ret;
    }
    operands_loaded = true;

    ret = request_threaded_irq(
            IRQ_NUM,
            irq_calc_top,     /* top half  */
            irq_calc_thread,  /* bottom half (thread) */
            IRQF_SHARED,      /* share IRQ 1 with existing keyboard driver */
            DRIVER_NAME,
            (void *)irq_calc_thread   
    );

    if (ret) {
        pr_err("%s: request_threaded_irq failed: %d\n", DRIVER_NAME, ret);
        return ret;
    }

    pr_info("%s: registered on IRQ %d\n", DRIVER_NAME, IRQ_NUM);
    pr_info("%s: press A=add  S=sub  M=mul  D=div \n",
            DRIVER_NAME);
    return 0;
}

static void __exit irq_calc_exit(void)
{
    free_irq(IRQ_NUM, (void *)irq_calc_thread);
    pr_info("%s: IRQ %d Unloaded\n", DRIVER_NAME, IRQ_NUM);
}

module_init(irq_calc_init);
module_exit(irq_calc_exit);
