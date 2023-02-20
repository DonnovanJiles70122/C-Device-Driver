/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Name: Donnovan Jiles
* Student ID: 920975689
* GitHub UserID: DonnovanJiles70122
* Project: Assignment 6 – Device Driver
*
* File: <test.c>
*
* Description: Create a simple device driver
*
**************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main()
{
    int fd;
    int32_t value, number;

    printf("\nOpening Driver\n");
    fd = open("/dev/dev_device", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file '/dev/dev_device'\n");
        return 0;
    }

    printf("Enter the Value to send\n");
    scanf("%d",&number);

    printf("Writing number to device driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &number); 

    printf("Reading number to device driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);

    printf("Value is %d\n", value);

    close(fd);
}
