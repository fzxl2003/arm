#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>   
#include <string.h>
#include <unistd.h>

#define SYSFS_GPIO_EXPORT           "/sys/class/gpio/export"  //The path of export
#define SYSFS_GPIO_UNEXPORT         "/sys/class/gpio/unexport"//The path of unexport
#define SYSFS_GPIO_RST_DIR_VAL_OUT  "out"                     //The value of direction out
#define SYSFS_GPIO_RST_DIR_VAL_IN   "in" 					  //The value of direction in
#define SYSFS_GPIO_RST_VAL_H        "1"						  //THe high level
#define SYSFS_GPIO_RST_VAL_L        "0"						  //The low level
#define SYSFS_GPIO_PROC             "/etc/proc"    //The path of gpio value file
#define SYSFS_GPIO_PROC_CUR    		"/etc/proc.cf" //the path of gpio current file

#define SYSFS_GPIO_EDGE_VAL         "falling"			//the falling edge
/***
*init the gpio led value 
*set value to high level
*
*********/
int gpio_init()
{
	int gpio_set[8] = {35,24,30,26,27,37,32,29};
	FILE *fd;
	char buff[8];
	int i;

	fd = fopen(SYSFS_GPIO_PROC,"w+");
	if(fd == NULL)
	{
		printf("GPIO_LED init filed!\n");
		return -1;
	}else
	{
		for(i=0; i< 8; i++)
		{
			sprintf(buff,"%d %d",gpio_set[i],1);
			fprintf(fd,"%s\n",buff);
		}
		
	}
	fclose(fd);
	return 1;

}
//open the GPIO_PIN
/*
*int gpio_pin : the number of gpio
*char* dir_val : the direction of gpio
*
*/
int gpio_open(int gpio_pin,char* dir_val)
{
	int fd;
	char GPIO_DIRE[100];
	char pin_str[10];
	sprintf(pin_str,"%d",gpio_pin);
	//export the GPIO_PIN ID
	fd = open(SYSFS_GPIO_EXPORT,O_WRONLY);
	if(fd == -1)
	{
		printf("OPEN EXPORT FILE ERROR.\n");
		return -1;
		return EXIT_FAILURE;
	}
	write(fd,pin_str,sizeof(pin_str));
	close(fd);
	//set the GPIO direction
	sprintf(GPIO_DIRE,"/sys/class/gpio/gpio%d/direction",gpio_pin);
	printf("%s\n", GPIO_DIRE);
	fd = open(GPIO_DIRE,O_WRONLY);
	if(fd == -1)
	{
		printf("OPEN direction FILE ERROR.\n");
		return -1;
		return EXIT_FAILURE;
	}
	write(fd,dir_val,sizeof(dir_val));
	close(fd);

	return gpio_pin;

}

//set the gpio edge val
int gpio_set_edge(int gpio_pin,char* edge)
{
	int fd;
	char GPIO_EDGE[100];
	sprintf(GPIO_EDGE,"/sys/class/gpio/gpio%d/edge",gpio_pin);

	fd = open(GPIO_EDGE,O_WRONLY);
	if(fd == -1)
	{
		printf("open gpio edge ERROR.\n");
		return EXIT_FAILURE;

	}
	write(fd,edge,sizeof(edge));
	close(fd);
}

//set the gpio value
/*
* int gpio_pin : the number of gpio
* char* value : SYSFS_GPIO_RST_VAL_H        "1"
				SYSFS_GPIO_RST_VAL_L        "0"
*/
int gpio_set_value(int gpio_pin,char* value)
{
	int fd;
	FILE* ff_cur;
	FILE* ff;
	char GPIO_RST_VAL[100];

	char buff[10];
	char buff_cur[10];
	sprintf(GPIO_RST_VAL,"/sys/class/gpio/gpio%d/value",gpio_pin);

	printf("Path:%s\n", GPIO_RST_VAL);

	fd = open(GPIO_RST_VAL,O_RDWR);
	write(fd,value,sizeof(value));
	close(fd);

	//write the value to the current file proc
	while(1)
	{
		ff = fopen(SYSFS_GPIO_PROC,"r+");
		if(ff == NULL)
		{
			printf("Open gpio file_value ERROR!\n");
			continue;
		}else
		break;
	}
	
	//open the curent file
	ff_cur = fopen(SYSFS_GPIO_PROC_CUR,"w");
	if(ff_cur == NULL)
	{
		printf("Open gpio file cur ERROR!\n");
	}
	int i = 0;
	int a = gpio_pin/10;//ascii of gpio_pin
	int b = gpio_pin%10;

	for(i = 0;i < 8;i++)
		{
			if(fgets(buff,8,ff) != NULL)
			{
				printf("%s", buff);
				if(a == (buff[0]-48) && b == (buff[1]-48))//find the gpio_pin
				{
					//write the gpio curent state
					sprintf(buff_cur,"%d %s",gpio_pin,value);
					fprintf(ff_cur, "%s\n", buff_cur);
				}else
				{
					fprintf(ff_cur, "%s", buff);
				}
			}
		}
	fclose(ff);
	fclose(ff_cur);
	remove(SYSFS_GPIO_PROC);
	rename(SYSFS_GPIO_PROC_CUR,SYSFS_GPIO_PROC);

	return gpio_pin;
}
//get the gpio value
/**
*return the value
*/
int gpio_get_value(int gpio_num)
{
	FILE *fd;
    char buff[10];
    int i=0;
    int val = 1;
    int a = gpio_num/10;
    int b = gpio_num%10;
    while(1)
    {
    	fd = fopen(SYSFS_GPIO_PROC,"r+");
    	if(fd == NULL)
    	{
        	printf("Open file proc error!\n");
        	continue;
        //return -1;
    	}else
    		break;
    }
    
    if(fd == NULL)
    {
        printf("Open file proc error!\n");
        //return -1;
    }
    for(i=0;i<8;i++)
    {
        if(fgets(buff,8,fd) != NULL)
        {
        
            if(a==(buff[0]-48) && b==(buff[1]-48))
            {
                val = (buff[3]=='1')?1:0;
                break;
            }
        }
    }
    fclose(fd);
    return val;
}
int gpio_close(int gpio_pin)
{
	int fd;
	char pin_str[10];
	sprintf(pin_str,"%d",gpio_pin);
	fd = open(SYSFS_GPIO_UNEXPORT,O_WRONLY);
	write(fd,pin_str,sizeof(pin_str));
	close(fd);
}



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
    int led_num = 24; /* gpio LED 的编号 */
    int key_num = 36; /* gpio KEY 的编号 */
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
        printf("OPEN GPIO ERROR.\n");
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

    // gpio LED 初始化
    if(gpio_init() != 0)
    {
        printf("init failed!\n");
    }

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
                        gpio_set_value(led_num, 0); // 关闭LED
                    }
                    else
                    {
                        // 如果LED灭，开启LED
                        gpio_set_value(led_num, 1); // 打开LED
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
