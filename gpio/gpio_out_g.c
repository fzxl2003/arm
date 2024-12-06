#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   
#include "GPIO_CTL.h"

//两灯交替闪烁
int main()
{
    // int ledfd1 = 37;
    // int ledfd2 = 32;
	int ledfd1=35;
	int ledfd2=24;
    int sed = 0;
    int value1;
    int value2;

    // 打开两个LED的GPIO文件，设置为输出
    gpio_open(ledfd1, SYSFS_GPIO_RST_DIR_VAL_OUT);
    gpio_open(ledfd2, SYSFS_GPIO_RST_DIR_VAL_OUT);

    for(sed = 0; sed >-1; sed++)
    {
        printf("round %d\n", sed);

        // 点亮第一个LED，熄灭第二个LED
        gpio_set_value(ledfd1, SYSFS_GPIO_RST_VAL_H);
        gpio_set_value(ledfd2, SYSFS_GPIO_RST_VAL_L);

        usleep(1000000);  // 延时1秒

        // 获取LED的当前状态
        value1 = gpio_get_value(ledfd1);
        value2 = gpio_get_value(ledfd2);

        // 打印LED状态
        printf("before: %d led value is %d\n", ledfd1, value1);
        printf("before: %d led value is %d\n", ledfd2, value2);

        // Todo: 设置电平交换，交替闪烁
        gpio_set_value(ledfd1, SYSFS_GPIO_RST_VAL_L);  // 熄灭第一个LED
        gpio_set_value(ledfd2, SYSFS_GPIO_RST_VAL_H);  // 点亮第二个LED

        usleep(1000000);  // 延时1秒

        // 再次获取LED的状态
        value1 = gpio_get_value(ledfd1);
        value2 = gpio_get_value(ledfd2);

        // 打印LED状态
        printf("after: %d led value is %d\n", ledfd1, value1);
        printf("after: %d led value is %d\n", ledfd2, value2);

        usleep(1000000);  // 延时1秒
    }

    // 关闭两个LED的GPIO文件
    gpio_close(ledfd1);
    gpio_close(ledfd2);

    return 0;
}
