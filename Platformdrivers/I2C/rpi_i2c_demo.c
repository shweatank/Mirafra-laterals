// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>

#define DRIVER_NAME "rpi_i2c_demo"

static int rpi_i2c_probe(struct i2c_client *client)
{
    int ret;
    u8 reg = 0x00;
    u8 data;
    struct i2c_msg msgs[2];

    pr_info("%s: Probed device at 0x%02x on adapter %d\n",
            DRIVER_NAME,
            client->addr,
            client->adapter->nr);

    /* Message 1: Write register address */
    msgs[0].addr  = client->addr;
    msgs[0].flags = 0; // Write
    msgs[0].len   = 1;
    msgs[0].buf   = &reg;

    /* Message 2: Read data */
    msgs[1].addr  = client->addr;
    msgs[1].flags = I2C_M_RD; // Read
    msgs[1].len   = 1;
    msgs[1].buf   = &data;

    ret = i2c_transfer(client->adapter, msgs, 2);
    if (ret < 0) {
        pr_err("%s: I2C transfer failed\n", DRIVER_NAME);
        return ret;
    }

    pr_info("%s: Read value 0x%02x from reg 0x00\n",
            DRIVER_NAME, data);

    return 0;
}

static void rpi_i2c_remove(struct i2c_client *client)
{
    pr_info("%s: Device removed\n", DRIVER_NAME);
}

static const struct of_device_id rpi_i2c_of_match[] = {
    { .compatible = "jj,rpi-i2c-demo" },
    { }
};
MODULE_DEVICE_TABLE(of, rpi_i2c_of_match);

static struct i2c_driver rpi_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = rpi_i2c_of_match,
    },
    .probe  = rpi_i2c_probe,
    .remove = rpi_i2c_remove,
};

module_i2c_driver(rpi_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jerin Jose");
MODULE_DESCRIPTION("Raspberry Pi BCM2711 I2C Demo Driver");
