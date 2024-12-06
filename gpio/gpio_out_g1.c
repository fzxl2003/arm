#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   
#include "GPIO_CTL.h"


int main() 
{
    // LED对应的GPIO编号
    int leds[] = {24, 30, 26, 27, 37, 32, 29}; // LED1到LED7的GPIO编号
    int i, j;
    int fd;

    // 打开7个GPIO文件，设置为输出模式
    for (i = 0; i < 7; i++) {
        fd = gpio_open(leds[i], SYSFS_GPIO_RST_DIR_VAL_OUT); // 打开GPIO，设置为输出
        if (fd == -1) {
            printf("Failed to open GPIO %d\n", leds[i]);
            return -1;
        }
    }

    // 每个LED交替闪烁两次
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++) {
            // 点亮当前LED
            gpio_set_value(leds[i], SYSFS_GPIO_RST_VAL_H);  
            usleep(500000);  // 延时0.5秒

            // 熄灭当前LED
            gpio_set_value(leds[i], SYSFS_GPIO_RST_VAL_L);  
            usleep(500000);  // 延时0.5秒
        }
    }

    // 关闭7个GPIO文件
    for (i = 0; i < 7; i++) {
        gpio_close(leds[i]);  // 关闭GPIO
    }

    return 0;
}

