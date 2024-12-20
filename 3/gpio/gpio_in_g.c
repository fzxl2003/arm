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

int main(int argc, char *argv[])
{
    int ret;
    fd_set fdset;
    int value;
    int led_num =26; /* gpio LED 的编号 */
    int key_num = 170; /* gpio KEY 的编号 */
    int keyfd;
    int ledfd;

    /*
    * 打开LED的gpio文件，设置为输出 */
    ledfd = gpio_open(led_num, SYSFS_GPIO_RST_DIR_VAL_OUT);
    if(ledfd == -1)
    {
        printf("OPEN GPIO ERROR.\n");
    }

    /*
    * 打开KEY的GPIO文件，设置为输入 */
    keyfd = gpio_open(key_num, SYSFS_GPIO_RST_DIR_VAL_IN);
    if(keyfd == -1)
    {
        printf("OPEN GPIO ERROR1.\n");
    }

    FD_ZERO(&fdset);
    // 设置下降沿触发中断
    gpiokey_set_edge(key_num);

    char path[100];
    sprintf(path, "/sys/class/gpio/gpio%d/value", key_num);
    keyfd = open(path, O_RDONLY);
    if(keyfd == -1)
    {
        printf("open gpio value ERROR.\n");
        return EXIT_FAILURE;
    }

    // // gpio LED 初始化
    // if(gpio_init() != 0)
    // {
    //     printf("init failed!\n");
    // }

    int led_val;

    while (1)
    {
        // 使用select模型
        FD_SET(keyfd, &fdset);
        ret = select(keyfd + 1, &fdset, NULL, NULL, NULL);

        if(ret == 1)
        {
            // 触发中断
            if(FD_ISSET(keyfd, &fdset))
            {
                // 按键触发操作
                value = gpio_read_keyval(key_num);
                if(value == 0)
                {
                    led_val = gpio_get_value(led_num);
                    if(led_val == 1)
                    {
                        // 如果LED亮，关闭LED
                        gpio_set_value(led_num, SYSFS_GPIO_RST_VAL_L); // 关闭LED
                    }
                    else
                    {
                        // 如果LED灭，开启LED
                        gpio_set_value(led_num, SYSFS_GPIO_RST_VAL_H); // 打开LED
                    }
                }
                else
                {
                    continue;
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

    close(keyfd);
    return 0;
}
