/*
 * BUAA LDMC
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/sched.h>

#define ZLG7290_NAME        "zlg7290"
#define ZLG7290_LED_NAME    "zlg7290_led"

#define REG_DP_RAM0         0x10  //第一个数码管的寄存器
#define REG_DP_RAM1         0x11  //第二个数码管的寄存器
#define REG_DP_RAM2         0x12  //第三个数码管的寄存器
#define REG_DP_RAM3         0x13  //第四个数码管的寄存器
#define REG_DP_RAM4         0x14
#define REG_DP_RAM5         0x15
#define REG_DP_RAM6         0x16
#define REG_DP_RAM7         0x17

#define ZLG7290_LED_MAJOR   800  //主设备号
#define ZLG7290_LED_MINOR   0    //从设备号
#define ZLG7290_LED_DEVICES 1    //设备数量

#define WRITE_DPRAM _IO('Z', 0)

// 数码管设备结构体
struct zlg7290
{
    struct i2c_client *client;    //struct i2c_client代表一个挂载到i2c总线上的i2c从设备
    struct input_dev *input;      //struct input_dev代表一个输入设备
    struct delayed_work work;     //未使用，用于延迟工作的调度
    unsigned long delay;          //未使用，用于延迟工作的调度

    struct cdev cdev;             //cdev字符设备
};

// 数码管设备结构体全局实例
struct zlg7290 *ptr_zlg7290;

// 数码管位图，每一个bit代表一根灯管，置1:亮，置0:灭
unsigned char bit_map[8] = {//////////////////////////////////////////////////////////////
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

// 数码管显示0-F 16进制字符的位图数据
unsigned char hex_map[40] = {//////////////////////////////////////////////////////////////
    0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0,
    0xfe, 0xf6, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e
};

// 向i2c总线写数据
static int zlg7290_hw_write(struct zlg7290 *zlg7290, int len, size_t *retlen, char *buf)
{
    //len 为写入的字节数，retlen为返回的字节数，buf为写入的数据
    struct i2c_client *client = zlg7290->client;
    int ret;

    struct i2c_msg msg[] = {
        { client->addr, 0, len, buf },//写入寄存器地址,0表示写,1表示读, len为写入的字节数, buf为写入的数据
    };

    ret = i2c_transfer(client->adapter, msg, 1);/////////////////////////////////////////////////////
    if (ret < 0) 
    {
        dev_err(&client->dev, "i2c write error!\n");
        return -EIO;
    }

    *retlen = len;
    return 0;
}

// 从i2c总线读数据
static int zlg7290_hw_read(struct zlg7290 *zlg7290, int len, size_t *retlen, char *buf)
{//////////////////////////////////////////////////////////////////////////////////////////
    struct i2c_client *client = zlg7290->client;
    int ret;

    struct i2c_msg msg[] = {
        { client->addr, 0, 1, buf }, // 写入寄存器地址, 0表示写, 1表示读, 1为写入的字节数, buf为写入的数据
        { client->addr, I2C_M_RD, len, buf } // 读取数据, I2C_M_RD表示读, len为读取的字节数, buf为读取的数据
    };

    ret = i2c_transfer(client->adapter, msg, 2);
    if (ret < 0)
    {
        dev_err(&client->dev, "i2c read error!\n");
        return -EIO;
    }

    *retlen = len;
    return 0;
}

// 打开数码管设备
static int zlg7290_led_open(struct inode *inode, struct file *file)
{
    return 0;
}

// 释放数码管设备
static int zlg7290_led_release(struct inode *inode, struct file *file)
{
    return 0;
}

// 向上的接口，用户态通过调用ioctl系统函数，隐式调用该函数
static long zlg7290_led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

// file_operations: 把系统调用和驱动程序关联起来的关键数据结构
static struct file_operations zlg7290_led_fops = {
    .owner = THIS_MODULE,
    .open = zlg7290_led_open,
    .release = zlg7290_led_release,
    .unlocked_ioctl = zlg7290_led_ioctl,
};

// 注册数码管小灯设备主设备号：MAJOR，从设备号：MINOR
static int register_zlg7290_led(struct zlg7290 *zlg7290) 
{
    struct cdev *zlg7290_cdev;
    int ret;
    dev_t devid;

    devid = MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR);
    ret = register_chrdev_region(devid, ZLG7290_LED_DEVICES, ZLG7290_LED_NAME);
    if (ret < 0) {
        dev_err(&zlg7290->client->dev, "register chrdev fail!\n");
        return ret;
    }

    zlg7290_cdev = &zlg7290->cdev;
    cdev_init(zlg7290_cdev, &zlg7290_led_fops);
    zlg7290_cdev->owner = THIS_MODULE;
    ret = cdev_add(zlg7290_cdev, devid, 1);
    if (ret < 0) {
        dev_err(&zlg7290->client->dev, "cdev add fail!\n");
        goto err_unreg_chrdev;
    }
    return 0;

err_unreg_chrdev:
    unregister_chrdev_region(devid, ZLG7290_LED_DEVICES);
    return ret;
}

// 从系统中注销设备
static int unregister_zlg7290_led(struct zlg7290 *zlg7290) 
{
    cdev_del(&zlg7290->cdev);
    
    unregister_chrdev_region(MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR), ZLG7290_LED_DEVICES);
    
    return 0;
}

// 探测函数
static int 
zlg7290_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct input_dev *input_dev;
    int ret = 0;
    
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
        dev_err(&client->dev, "%s adapter not supported\n",
            dev_driver_string(&client->adapter->dev));
        return -ENODEV;
    }
    
    ptr_zlg7290 = kzalloc(sizeof(struct zlg7290), GFP_KERNEL);
    input_dev = input_allocate_device();
    if (!ptr_zlg7290 || !input_dev) {
        ret = -ENOMEM;
        goto err_free_mem;
    }
    
    input_dev->name = client->name;
    input_dev->phys = "zlg7290-keys/input0";
    input_dev->dev.parent = &client->dev;
    input_dev->id.bustype = BUS_I2C;
    input_dev->id.vendor = 0x0001;
    input_dev->id.product = 0x0001;
    input_dev->id.version = 0x0001;
    input_dev->evbit[0] = BIT_MASK(EV_KEY);
    
    ret = input_register_device(input_dev);
    if (ret) {
        dev_err(&client->dev, "unable to register input device\n");
        goto err_unreg_dev;
    }

    ptr_zlg7290->client = client;
    ptr_zlg7290->input = input_dev;
    i2c_set_clientdata(client, ptr_zlg7290);
    
    ret = register_zlg7290_led(ptr_zlg7290);
    if (ret < 0)
        return ret;

    printk("probe done! \n");
    return 0;

err_unreg_dev:
    input_unregister_device(input_dev);
    input_dev = NULL;
err_free_mem:
    input_free_device(input_dev);
    kfree(ptr_zlg7290);

    return ret;
}

// 释放函数
static int zlg7290_remove(struct i2c_client *client) 
{
	struct zlg7290 *zlg7290 = i2c_get_clientdata(client);
    unregister_zlg7290_led(zlg7290);
    printk("remove done! \n");
	return 0;
}

// 匹配设备和驱动
// 考点：为什么需要i2c_device_id？
// 提示：一个设备只有一个驱动，一个驱动可以对应多个设备
static const struct i2c_device_id zlg7290_id[] = {
	{ZLG7290_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, zlg7290_id);

#ifdef CONFIG_OF
static const struct of_device_id zlg7290_dt_ids[] = {
	{ .compatible = "myzr,zlg7290", },
	{ }
};
MODULE_DEVICE_TABLE(of, zlg7290_dt_ids);
#endif

// 驱动程序结构体（重要）
// 考点：请分析init，probe，remove和exit函数的执行顺序
// 提示：可通过printk来分析
static struct i2c_driver zlg7290_driver= {
	.driver	= {
		.name	= ZLG7290_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(zlg7290_dt_ids),
	},
	.probe		= zlg7290_probe,
    .remove     = zlg7290_remove,
	.id_table	= zlg7290_id
};

//find,根据读取到的值找到对应的字符
static unsigned char find(unsigned char val)
{
    int i;
    for(i = 0; i < 40; i++)
    {
        if(hex_map[i] == val)
        {
            if(i < 10)
                return i + '0';
            else
                return i - 10 + 'A';
        }
    }
    return 0;
}


// 初始化模块
static int __init zz_init(void)
{
	

	// 内核态i2c_transfer通信
	// Todo：保持8个数码管内容相同（只使用到4个），循环显示bit_map和hex_map中的所有字符
	// 提示：调用zlg7290_hw_write函数
	int i;
    int j;
	ssize_t len = 0;

	unsigned char write_val[2] = {0};
    /* write_val[0] : 待写入寄存器的地址
     * write_val[1] : 待写入寄存器的数据
     * 例如：write_val[0] = REG_DP_RAM0;  //写0号寄存器
     *      write_val[1] = 0xfc;         //写入字符“0”
     */
	// module_i2c_driver宏展开
	i2c_register_driver(THIS_MODULE, &zlg7290_driver);
	// 循环显示 bit_map 中的每个字符
	for(i=0; i<8; i++){ // 外层循环：bit_map的长度////////////////////////////////////////////////////////////////////////////////
		for(j = 0; j < 4; j++){ // 内层循环：4个数码管寄存器
			write_val[0] = REG_DP_RAM0 + j; // 写寄存器地址（REG_DP_RAM0到REG_DP_RAM3）
			write_val[1] = bit_map[i];      // 写入bit_map中的每一位字符
			zlg7290_hw_write(ptr_zlg7290, 2, &len, write_val); // 调用zlg7290_hw_write函数写入数据
            
			msleep(1); // 睡眠1毫秒
            //读取数码管的值
            zlg7290_hw_read(ptr_zlg7290, 1, &len, write_val);
            printk("read value is %x\n", write_val[0]);
            msleep(1); // 睡眠1毫秒

		}
		msleep(1000); // 睡眠1秒，显示完一个字符后暂停
	}

	// 循环显示 hex_map 中的每个字符
	for(i=0; i<16; i++){ // 外层循环：hex_map的长度
		for(j = 0; j < 4; j++){ // 内层循环：4个数码管寄存器
			write_val[0] = REG_DP_RAM0 + j; // 写寄存器地址（REG_DP_RAM0到REG_DP_RAM3）
			write_val[1] = hex_map[i];      // 写入hex_map中的每一位字符
			zlg7290_hw_write(ptr_zlg7290, 2, &len, write_val); // 调用zlg7290_hw_write函数写入数据
			msleep(1); // 睡眠1毫秒
            //读取数码管的值
            zlg7290_hw_read(ptr_zlg7290, 1, &len, write_val);
            printk("read value is %x %c\n", write_val[0], find(write_val[0]));
            msleep(1); // 睡眠1毫秒
		}
		msleep(1000); // 睡眠1秒，显示完一个字符后暂停
	}

	// 最后将所有数码管置零
	for(j = 0; j < 4; j++){ // 只有4个数码管需要处理
		write_val[0] = REG_DP_RAM0 + j; // 写寄存器地址（REG_DP_RAM0到REG_DP_RAM3）
		write_val[1] = 0x00;            // 将显示内容置为0
		zlg7290_hw_write(ptr_zlg7290, 2, &len, write_val); // 调用zlg7290_hw_write函数写入数据
		msleep(1); // 睡眠1毫秒
	}

	return 0;
}


// 卸载模块
static void __exit zz_exit(void)
{	
    i2c_del_driver(&zlg7290_driver);
	printk("886, The module has been removed\n");
}

module_init(zz_init);
module_exit(zz_exit);

MODULE_AUTHOR("buaa ldmc");
MODULE_DESCRIPTION("Keypad & Leds driver for ZLG7290");
MODULE_LICENSE("GPL");
