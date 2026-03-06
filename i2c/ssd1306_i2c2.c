#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define OLED_PAGES (OLED_HEIGHT/8)

#define CMD  0x00
#define DATA 0x40

#define DEVICE_NAME "oled"

static const u8 font5x7[128][5] = {

['H'] = {0x7F,0x08,0x08,0x08,0x7F},
['E'] = {0x7F,0x49,0x49,0x49,0x41},
['L'] = {0x7F,0x40,0x40,0x40,0x40},
['O'] = {0x3E,0x41,0x41,0x41,0x3E},

['A'] = {0x7E,0x11,0x11,0x11,0x7E},
['B'] = {0x7F,0x49,0x49,0x49,0x36},
['C'] = {0x3E,0x41,0x41,0x41,0x22},
['D'] = {0x7F,0x41,0x41,0x22,0x1C},

['R'] = {0x7F,0x09,0x19,0x29,0x46},
['I'] = {0x41,0x7F,0x41,0x00,0x00},
['N'] = {0x7F,0x02,0x04,0x08,0x7F},

[' '] = {0x00,0x00,0x00,0x00,0x00}

};

static int major;

struct ssd1306 {
	struct i2c_client *client;
	u8 buffer[OLED_WIDTH * OLED_PAGES];
};

static struct ssd1306 *oled_dev;

static int ssd1306_cmd(struct i2c_client *client, u8 cmd)
{
	u8 buf[2] = {CMD, cmd};
	return i2c_master_send(client, buf, 2);
}

static int ssd1306_data(struct i2c_client *client, u8 *data, int len)
{
	u8 buf[129];

	buf[0] = DATA;
	memcpy(&buf[1], data, len);

	return i2c_master_send(client, buf, len + 1);
}

static void ssd1306_update(struct ssd1306 *dev)
{
	int page;

	for(page=0; page<OLED_PAGES; page++)
	{
		ssd1306_cmd(dev->client, 0xB0 + page);
		ssd1306_cmd(dev->client, 0x00);
		ssd1306_cmd(dev->client, 0x10);

		ssd1306_data(dev->client,
		             &dev->buffer[OLED_WIDTH*page],
		             OLED_WIDTH);
	}
}

static void ssd1306_clear(struct ssd1306 *dev)
{
	memset(dev->buffer,0,sizeof(dev->buffer));
	ssd1306_update(dev);
}

static void ssd1306_init(struct i2c_client *client)
{
	ssd1306_cmd(client,0xAE);
	ssd1306_cmd(client,0xD5); ssd1306_cmd(client,0x80);
	ssd1306_cmd(client,0xA8); ssd1306_cmd(client,0x1F);
	ssd1306_cmd(client,0xD3); ssd1306_cmd(client,0x00);
	ssd1306_cmd(client,0x40);
	ssd1306_cmd(client,0x8D); ssd1306_cmd(client,0x14);
	ssd1306_cmd(client,0x20); ssd1306_cmd(client,0x00);
	ssd1306_cmd(client,0xA1);
	ssd1306_cmd(client,0xC8);
	ssd1306_cmd(client,0xDA); ssd1306_cmd(client,0x02);
	ssd1306_cmd(client,0x81); ssd1306_cmd(client,0x8F);
	ssd1306_cmd(client,0xD9); ssd1306_cmd(client,0xF1);
	ssd1306_cmd(client,0xDB); ssd1306_cmd(client,0x40);
	ssd1306_cmd(client,0xA4);
	ssd1306_cmd(client,0xA6);
	ssd1306_cmd(client,0xAF);
}

static void draw_char(struct ssd1306 *dev, char c, int x)
{
    int i;

    for(i=0;i<5;i++)
        dev->buffer[x+i] = font5x7[(int)c][i];
}

static ssize_t oled_write(struct file *f,
                          const char __user *buf,
                          size_t len,
                          loff_t *off)
{
    char kbuf[32];
    int i;

    if(len > 20)
        len = 20;

    copy_from_user(kbuf, buf, len);

    ssd1306_clear(oled_dev);

    for(i = 0; i < len; i++)
    {
        if(kbuf[i] == '\n')
            break;

        draw_char(oled_dev, kbuf[i], i * 6);
    }

    ssd1306_update(oled_dev);

    return len;
}

static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.write=oled_write,
};

static int ssd1306_probe(struct i2c_client *client)
{
	oled_dev = devm_kzalloc(&client->dev,sizeof(*oled_dev),GFP_KERNEL);

	oled_dev->client = client;

	ssd1306_init(client);

	msleep(100);

	ssd1306_clear(oled_dev);

	major = register_chrdev(0,DEVICE_NAME,&fops);

	printk("OLED driver loaded Major=%d\n",major);

	return 0;
}

static void ssd1306_remove(struct i2c_client *client)
{
	unregister_chrdev(major,DEVICE_NAME);
	ssd1306_cmd(client,0xAE);
}

static const struct of_device_id ssd1306_of_match[] = {
	{ .compatible="mycompany,ssd1306"},
	{}
};

MODULE_DEVICE_TABLE(of,ssd1306_of_match);

static struct i2c_driver ssd1306_driver = {
	.driver={
		.name="ssd1306",
		.of_match_table=ssd1306_of_match,
	},
	.probe=ssd1306_probe,
	.remove=ssd1306_remove,
};

module_i2c_driver(ssd1306_driver);

MODULE_LICENSE("GPL");
