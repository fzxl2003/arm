#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>      

int set_parity(int fd, int databits, int stopbits, int parity) {
    int ret;
    struct termios options;

    ret = tcgetattr(fd, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (1);
    }

    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5:
            options.c_cflag |= CS5;
            break;

        case 6:
            options.c_cflag |= CS6;
            break;

        case 7:
            options.c_cflag |= CS7;
            break;

        case 8:
            options.c_cflag |= CS8;
            break;

        default:
            printf("Unsupported data size\n");
            return (2);
    }

    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;

        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;

        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;

        default:
            printf("Unsupported parity\n");
            return (3);
    }

    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            printf("Unsupported stop bits\n");
            return (4);
    }
    tcflush(fd, TCIOFLUSH);
    ret = tcsetattr(fd, TCSANOW, &options);
    if (ret != 0) {
        printf("Setup Serial failed");
        return (5);
    }
    return (0);
}

int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B38400, B19200, B9600, B4800, B2400, B1200, B300,
};

int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300,
                  38400, 19200, 9600, 4800, 2400, 1200, 300,
};

void set_speed(int fd, int speed) {
    int i;
    int status;
    struct termios Opt;

    tcgetattr(fd, &Opt);
    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
        if (speed == name_arr[i]) {
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0)
                printf("Set serial speed error\n");
            return;
        }
    }
    printf("Unsupported baudrate\n");
}

int fd_tty;
char buff[512];
int state = 0;
char str[1005];
void send_thread(void *arg) {
    int nwrite;
    while (1) {
        if (state == 0) {
            sleep(1);
            continue;
        }
        nwrite = write(fd_tty, str, strlen(str));

        if (nwrite > 0) {
            memset(str, 0, sizeof(str));
            state = 0;
        }
    }
}

void recv_thread(void *arg) {
    int nread;
    while (1) {
        if (state == 1) {
            sleep(1);
            continue;
        }
        nread = read(fd_tty, buff, 512);
        if (nread > 0) {
            sprintf(str, "%s", buff);
            memset(buff, 0, sizeof(buff));
            state = 1;
        }
    }

}


int main(int argc, char *argv[]) {
    int ret;
    int nread;
    
    

    if (argc < 2) {
        printf("useage: %s dev\n", argv[0]);
        return (-1);
    }

    fd_tty = open(argv[1], O_RDWR);
    if (fd_tty < 0) {
        printf("open device %s faild\n", argv[1]);
        return (-1);
    }

    set_speed(fd_tty, 115200);
    if (ret != 0) {
        close(fd_tty);
        return (-1);
    }
    ret = set_parity(fd_tty, 8, 1, 'n');
    if (ret != 0) {
        close(fd_tty);
        return (-1);
    }
    
    pthread_t send_tid, recv_tid;
    ret=pthread_create(&send_tid, NULL, send_thread, NULL);
    if (ret != 0) {
        printf("create send thread failed\n");
        exit(-1);
    }
    else
    {
        printf("create send thread success\n");
    }
    ret=pthread_create(&recv_tid, NULL, recv_thread, NULL);
    if (ret != 0) {
        printf("create recv thread failed\n");
        exit(-1);
    }
    else
    {
        printf("create recv thread success\n");
    }

    while (1) {
        sleep(10);
    }


    close(fd_tty);
    return (0);
}
