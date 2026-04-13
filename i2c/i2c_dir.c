#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define OLED_ADDR   0x3C
#define I2C_BUS     1

#define OLED_WIDTH  128
#define OLED_PAGES  4       /* 128x32 → 4 pages (32 rows / 8 bits) */

/* ------------------------------------------------------------------ */
/*  Low-level I2C helpers                                               */
/* ------------------------------------------------------------------ */

static int oled_cmd(struct i2c_client *client, u8 cmd)
{
    u8 buf[2] = { 0x00, cmd };
    int ret = i2c_master_send(client, buf, 2);
    return (ret < 0) ? ret : 0;
}

static int oled_data_buf(struct i2c_client *client, const u8 *data, int len)
{
    u8 tbuf[129];

    if (len > 128)
        len = 128;
    tbuf[0] = 0x40;
    memcpy(&tbuf[1], data, len);
    return i2c_master_send(client, tbuf, len + 1) < 0 ? -EIO : 0;
}

/* ------------------------------------------------------------------ */
/*  SSD1306 init — 128×32, PAGE addressing mode                        */
/* ------------------------------------------------------------------ */

static int oled_init(struct i2c_client *client)
{
    int ret = 0;

    ret |= oled_cmd(client, 0xAE);  /* display OFF */

    ret |= oled_cmd(client, 0xD5);
    ret |= oled_cmd(client, 0x80);  /* clock div */

    ret |= oled_cmd(client, 0xA8);
    ret |= oled_cmd(client, 0x1F);  /* mux ratio = 31 (128x32) */

    ret |= oled_cmd(client, 0xD3);
    ret |= oled_cmd(client, 0x00);  /* display offset = 0 */

    ret |= oled_cmd(client, 0x40);  /* start line = 0 */

    ret |= oled_cmd(client, 0x8D);
    ret |= oled_cmd(client, 0x14);  /* charge pump ON */

    ret |= oled_cmd(client, 0x20);
    ret |= oled_cmd(client, 0x02);  /* PAGE addressing mode */

    ret |= oled_cmd(client, 0xA1);  /* segment remap */
    ret |= oled_cmd(client, 0xC8);  /* COM scan remapped */

    ret |= oled_cmd(client, 0xDA);
    ret |= oled_cmd(client, 0x02);  /* COM pins for 128x32 */

    ret |= oled_cmd(client, 0x81);
    ret |= oled_cmd(client, 0xFF);  /* max contrast */

    ret |= oled_cmd(client, 0xD9);
    ret |= oled_cmd(client, 0xF1);  /* pre-charge */

    ret |= oled_cmd(client, 0xDB);
    ret |= oled_cmd(client, 0x40);  /* VCOMH deselect */

    ret |= oled_cmd(client, 0xA4);  /* display from RAM */
    ret |= oled_cmd(client, 0xA6);  /* normal (non-inverted) */
    ret |= oled_cmd(client, 0xAF);  /* display ON */

    msleep(150);
    return ret;
}

/* ------------------------------------------------------------------ */
/*  Helper: set page + column, then push pixel data                    */
/* ------------------------------------------------------------------ */

static void oled_write_page(struct i2c_client *client,
                             u8 page, u8 col,
                             const u8 *data, u8 len)
{
    oled_cmd(client, 0xB0 | (page & 0x07));
    oled_cmd(client, 0x00 | (col & 0x0F));
    oled_cmd(client, 0x10 | ((col >> 4) & 0x0F));
    oled_data_buf(client, data, len);
}

/* ------------------------------------------------------------------ */
/*  Clear display                                                       */
/* ------------------------------------------------------------------ */

static void oled_clear(struct i2c_client *client)
{
    u8 blank[128];
    int page;

    memset(blank, 0x00, sizeof(blank));
    for (page = 0; page < OLED_PAGES; page++)
        oled_write_page(client, page, 0, blank, 128);
}

/* ------------------------------------------------------------------ */
/*  Draw "Hi" — two glyphs, each 16 columns wide, 4 pages tall         */
/*                                                                      */
/*  Display is 128px wide.                                              */
/*  Total glyph width = 16 + 4(gap) + 16 = 36px                       */
/*  Start col = (128 - 36) / 2 = 46                                    */
/*                                                                      */
/*  'H' at col 46,  'i' at col 66  (gap of 4px between them)          */
/*                                                                      */
/*  Bit layout per byte: bit0 = topmost pixel of that page slice,      */
/*                        bit7 = bottommost pixel.                      */
/* ------------------------------------------------------------------ */

/*
 * 'H' glyph — 16 columns wide
 *
 *  ##              ##
 *  ##              ##
 *  ##              ##
 *  ##################
 *  ##################
 *  ##              ##
 *  ##              ##
 *  ##              ##
 */
static const u8 H_page0[16] = {
    0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF
};
static const u8 H_page1[16] = {
    0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF
};
static const u8 H_page2[16] = {
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};
static const u8 H_page3[16] = {
    0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF
};

/*
 * 'i' glyph — 16 columns wide
 *
 *  ######  dot at top centre (cols 5-10)
 *
 *  (blank row)
 *
 *  ######  stem (cols 5-10)
 *  ######
 *  ######
 *
 *  ######## base wider (cols 3-12)
 */
static const u8 i_page0[16] = {
    /* dot: only centre 6 columns lit, top 2 bits */
    0x00, 0x00, 0x00, 0x00,
    0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 i_page1[16] = {
    /* gap then stem starts */
    0x00, 0x00, 0x00, 0x00,
    0xF0, 0xF0, 0xF0, 0xF0,
    0xF0, 0xF0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 i_page2[16] = {
    /* stem continues */
    0x00, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
static const u8 i_page3[16] = {
    /* base — slightly wider */
    0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00
};

static void oled_print_Hi(struct i2c_client *client)
{
    u8 H_col = 46;   /* (128 - 36) / 2 = 46 */
    u8 i_col = 66;   /* 46 + 16 + 4(gap) = 66 */

    /* Draw 'H' */
    oled_write_page(client, 0, H_col, H_page0, 16);
    oled_write_page(client, 1, H_col, H_page1, 16);
    oled_write_page(client, 2, H_col, H_page2, 16);
    oled_write_page(client, 3, H_col, H_page3, 16);

    /* Draw 'i' */
    oled_write_page(client, 0, i_col, i_page0, 16);
    oled_write_page(client, 1, i_col, i_page1, 16);
    oled_write_page(client, 2, i_col, i_page2, 16);
    oled_write_page(client, 3, i_col, i_page3, 16);
}

/* ------------------------------------------------------------------ */
/*  I2C driver callbacks                                                */
/* ------------------------------------------------------------------ */

static int oled_probe(struct i2c_client *client)
{
    int ret;

    dev_info(&client->dev, "SSD1306 128x32 probe at 0x%02x\n", client->addr);

    ret = oled_init(client);
    if (ret) {
        dev_err(&client->dev, "oled_init failed: %d\n", ret);
        return ret;
    }

    oled_clear(client);
    oled_print_Hi(client);

    dev_info(&client->dev, "'Hi' displayed on 128x32 OLED\n");
    return 0;
}

static void oled_remove(struct i2c_client *client)
{
    oled_clear(client);
    oled_cmd(client, 0xAE);
    dev_info(&client->dev, "OLED removed\n");
}

/* ------------------------------------------------------------------ */
/*  Driver / module boilerplate                                         */
/* ------------------------------------------------------------------ */

static const struct i2c_device_id oled_id[] = {
    { "ssd1306_oled", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, oled_id);

static struct i2c_driver oled_driver = {
    .driver = {
        .name  = "ssd1306_oled",
        .owner = THIS_MODULE,
    },
    .probe    = oled_probe,
    .remove   = oled_remove,
    .id_table = oled_id,
};

static struct i2c_adapter *oled_adapter;
static struct i2c_client  *oled_client;

static int __init oled_module_init(void)
{
    struct i2c_board_info board_info = {
        I2C_BOARD_INFO("ssd1306_oled", OLED_ADDR),
    };
    int ret;

    pr_info("oled_i2c: loading\n");

    oled_adapter = i2c_get_adapter(I2C_BUS);
    if (!oled_adapter) {
        pr_err("oled_i2c: i2c-%d not found\n", I2C_BUS);
        return -ENODEV;
    }

    ret = i2c_add_driver(&oled_driver);
    if (ret) {
        i2c_put_adapter(oled_adapter);
        return ret;
    }

    oled_client = i2c_new_client_device(oled_adapter, &board_info);
    if (IS_ERR(oled_client)) {
        i2c_del_driver(&oled_driver);
        i2c_put_adapter(oled_adapter);
        return PTR_ERR(oled_client);
    }

    i2c_put_adapter(oled_adapter);
    return 0;
}

static void __exit oled_module_exit(void)
{
    i2c_unregister_device(oled_client);
    i2c_del_driver(&oled_driver);
    pr_info("oled_i2c: removed\n");
}

module_init(oled_module_init);
module_exit(oled_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARANG");
MODULE_DESCRIPTION("SSD1306 128x32 OLED driver — displays 'Hi'");
