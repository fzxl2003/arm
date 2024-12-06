#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>   
#include <string.h>
#include <unistd.h>
#include "GPIO_CTL.h"

int gpio_read_keyval(int gpio_pin)
{
    int fd;
    char GPIO_VAL[100];
    char value_str[3];

    sprintf(GPIO_VAL, "/sys/class/gpio/gpio%d/value", gpio_pin);
    fd = open(GPIO_VAL, O_RDONLY);
    if(fd == -1)
    {
        printf("OPEN FILE GPIO VALUE ERROR.\n");
        return EXIT_FAILURE;
    }

    if(read(fd, value_str, 3) < 0)
    {
        printf("READ GPIO VALUE ERROR\n");
        return EXIT_FAILURE;
    }
    close(fd);

    //返回gpio的值
    if(value_str[0] == '1')
    {
        return 1;
    }
    else if(value_str[0] == '0')
    {
        return 0;
    }
    else
        return -1;
}

int gpiokey_set_edge(int gpio_pin)
{
    int fd;
    char GPIO_EDGE[100];
    char value_str[10];
    sprintf(GPIO_EDGE, "/sys/class/gpio/gpio%d/edge", gpio_pin);
    printf("%s\n", GPIO_EDGE);
    fd = open(GPIO_EDGE, O_WRONLY);
    if(fd == -1)
    {
        printf("open gpio edge ERROR.\n");
        return EXIT_FAILURE;
    }
    if(write(fd, SYSFS_GPIO_EDGE_VAL, sizeof(SYSFS_GPIO_EDGE_VAL)) < 0)
        printf("write gpio edge ERROR.\n");
    read(fd, value_str, 10);
    printf("The edge is %s\n", value_str);
    close(fd);
    return 0; 
}


int main()
{
    int ret;
    fd_set fdset;
    int value1, value2;
    int led1_num = 35;  /* KEY1 控制的 LED 编号 */
    int led2_num = 24;  /* KEY2 控制的 LED 编号 */
    int key1_num = 36;  /* KEY1 的编号 */
    int key2_num = 38;  /* KEY2 的编号 */
    int key1fd, key2fd;
    int led1fd, led2fd;

    // 打开LED1的GPIO文件，设置为输出
    led1fd = gpio_open(led1_num, SYSFS_GPIO_RST_DIR_VAL_OUT);
    if(led1fd == -1)
    {
        printf("OPEN GPIO LED1 ERROR.\n");
    }

    // 打开LED2的GPIO文件，设置为输出
    led2fd = gpio_open(led2_num, SYSFS_GPIO_RST_DIR_VAL_OUT);
    if(led2fd == -1)
    {
        printf("OPEN GPIO LED2 ERROR.\n");
    }

    // 打开KEY1的GPIO文件，设置为输入
    key1fd = gpio_open(key1_num, SYSFS_GPIO_RST_DIR_VAL_IN);
    if(key1fd == -1)
    {
        printf("OPEN GPIO KEY1 ERROR.\n");
    }

    // 打开KEY2的GPIO文件，设置为输入
    key2fd = gpio_open(key2_num, SYSFS_GPIO_RST_DIR_VAL_IN);
    if(key2fd == -1)
    {
        printf("OPEN GPIO KEY2 ERROR.\n");
    }

    FD_ZERO(&fdset);
    // 设置下降沿触发中断
    gpiokey_set_edge(key1_num);
    gpiokey_set_edge(key2_num);

    char path1[100], path2[100];
    sprintf(path1, "/sys/class/gpio/gpio%d/value", key1_num);
    sprintf(path2, "/sys/class/gpio/gpio%d/value", key2_num);

    key1fd = open(path1, O_RDONLY);
    if(key1fd == -1)
    {
        printf("open gpio value KEY1 ERROR.\n");
        return EXIT_FAILURE;
    }

    key2fd = open(path2, O_RDONLY);
    if(key2fd == -1)
    {
        printf("open gpio value KEY2 ERROR.\n");
        return EXIT_FAILURE;
    }

    // gpio 初始化
    if(gpio_init() != 0)
    {
        printf("init failed!\n");
    }

    int led1_val, led2_val;

    while (1)
    {
        // 使用select模型，监听两个按键
        FD_SET(key1fd, &fdset);
        FD_SET(key2fd, &fdset);

        ret = select((key1fd > key2fd ? key1fd : key2fd) + 1, &fdset, NULL, NULL, NULL);

        if(ret > 0)
        {
            // 检测KEY1触发
            if(FD_ISSET(key1fd, &fdset))
            {
                value1 = gpio_read_keyval(key1_num);
                if(value1 == 0)
                {
                    led1_val = gpio_get_value(led1_num);
                    if(led1_val == 1)
                    {
                        gpio_set_value(led1_num, 0);  // 关闭LED1
                    }
                    else
                    {
                        gpio_set_value(led1_num, 1);  // 打开LED1
                    }
                }
            }

            // 检测KEY2触发
            if(FD_ISSET(key2fd, &fdset))
            {
                value2 = gpio_read_keyval(key2_num);
                if(value2 == 0)
                {
                    led2_val = gpio_get_value(led2_num);
                    if(led2_val == 1)
                    {
                        gpio_set_value(led2_num, 0);  // 关闭LED2
                    }
                    else
                    {
                        gpio_set_value(led2_num, 1);  // 打开LED2
                    }
                }
            }
        }
        else
        {
            printf("select NULL.\n");
        }

        FD_ZERO(&fdset);
        usleep(500 * 1000);  // 延时500ms
    }

    close(key1fd);
    close(key2fd);
    return 0;
}
