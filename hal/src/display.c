#include "display.h"
#include "hal_helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "camera.h"
#include "app_helper.h"

static pthread_t displayThread;
static bool endProgram = false;

static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);


#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

// for green zencape
/* #define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define GPIO_A  "/sys/class/gpio/gpio61/value"
#define GPIO_B  "/sys/class/gpio/gpio44/value" */

// for red zencape
#define REG_DIRA 0x02
#define REG_DIRB 0x03
#define REG_OUTA 0x00
#define REG_OUTB 0x01

#define GPIO_A  "/sys/class/gpio/gpio44/value"
#define GPIO_B  "/sys/class/gpio/gpio61/value"

typedef struct {
    unsigned char lower;
    unsigned char upper;
}hex_display_value;

// For green zen cape
/* static hex_display_value display_value_in_hex(int digit){
    unsigned char lower, upper;

    switch (digit) {
        case 0:
            lower = 0xA1 ; upper = 0x86;
            break;
        case 1:
            lower = 0x01; upper = 0x80;
            break;
        case 2:
            lower = 0x31; upper = 0x0E;
            break;
        case 3:
            lower = 0x21; upper = 0x8C;
            break;
        case 4:
            lower = 0x91; upper = 0x88;
            break;
        case 5:
            lower = 0xB0; upper = 0x8C;
            break;
        case 6:
            lower = 0xB0; upper = 0x8E;
            break;
        case 7:
            lower = 0x21; upper = 0x80;
            break;
        case 8:
            lower = 0xB1; upper = 0x8E;
            break;
        case 9:
            lower = 0xB1; upper = 0x8C;
            break;
        default:
            printf("Invalid digit\n");
            exit(1);
    }
    hex_display_value return_value;
    return_value.lower = lower; 
    return_value.upper = upper;     
    return return_value;
} */

// For red zen cape
static hex_display_value display_value_in_hex(int digit){
    unsigned char lower, upper;

    switch (digit) {
        case 0:
            lower = 0xD0 ; upper = 0xA1;
            break;
        case 1:
            lower = 0xC0; upper = 0x00;
            break;
        case 2:
            lower = 0x98; upper = 0x83;
            break;
        case 3:
            lower = 0xD8; upper = 0x03;
            break;
        case 4:
            lower = 0xC8; upper = 0x22;
            break;
        case 5:
            lower = 0x58; upper = 0x23;
            break;
        case 6:
            lower = 0x58; upper = 0xA3;
            break;
        case 7:
            lower = 0xC0; upper = 0x01;
            break;
        case 8:
            lower = 0xD8; upper = 0xA3;
            break;
        case 9:
            lower = 0xD8; upper = 0x23;
            break;
        default:
            printf("Invalid digit\n");
            exit(1);
    }
    hex_display_value return_value;
    return_value.lower = lower; 
    return_value.upper = upper;     
    return return_value;
}

static void* Display_show(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    hex_display_value first_value, second_value;
    int distance = 1;
    
    while(1){
        if(endProgram){
            write_to_file(GPIO_A, 0);
            write_to_file(GPIO_B, 0);
            close(i2cFileDesc);
            return 0;
        }
        distance = getPicCount();
        if(distance >= 100){
            first_value = display_value_in_hex(9);
            second_value = first_value;
        }
        else{
            int first_digit = distance % 10;
            distance = distance/10;
            int second_digit = distance % 10;
            first_value = display_value_in_hex(first_digit);
            second_value = display_value_in_hex(second_digit);
        }

        // displaying 8*8 digits on bbg
        write_to_file(GPIO_B,0);

        // Drive an hour-glass looking character (Like an X with a bar on top & bottom)
        writeI2cReg(i2cFileDesc, REG_OUTA, first_value.lower);
        writeI2cReg(i2cFileDesc, REG_OUTB, first_value.upper);

        write_to_file(GPIO_A,1);
        sleepForMs(5);

        write_to_file(GPIO_A,0);
        writeI2cReg(i2cFileDesc, REG_OUTA, second_value.lower);
        writeI2cReg(i2cFileDesc, REG_OUTB, second_value.upper);

        write_to_file(GPIO_B,1);
        sleepForMs(5);
    }
}

static void config_display(){
     runCommand("echo out > /sys/class/gpio/gpio61/direction");
     runCommand("echo out > /sys/class/gpio/gpio44/direction");
     runCommand("echo 1 > /sys/class/gpio/gpio61/value");
     runCommand("echo 1 > /sys/class/gpio/gpio44/value");
     runCommand("config-pin P9_18 i2c");
     runCommand("config-pin P9_17 i2c");
    
    
//  For green zencape    
    // runCommand("i2cset -y 1 0x20 0x00 0x00");
    // runCommand("i2cset -y 1 0x20 0x01 0x00");

//  For red zencape, set both to off to begin with
    runCommand("i2cset -y 1 0x20 0x02 0x00");
    runCommand("i2cset -y 1 0x20 0x03 0x00");
}

void Display_init(void) {
    config_display();
    pthread_create(&displayThread, NULL, Display_show, NULL);
}

void Display_cleanup(void) {
    endProgram = true;
    pthread_join(displayThread, NULL);
}

// Provided by Brian Fraser
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}