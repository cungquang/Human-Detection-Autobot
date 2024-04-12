#include "display.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "camera.h"
#include "app_helper.h"
#include "hal_helper.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x02
#define REG_DIRB 0x03
#define REG_OUTA 0x00
#define REG_OUTB 0x01

#define gpio_61_value_path "/sys/class/gpio/gpio61/value"
#define gpio_44_value_path "/sys/class/gpio/gpio44/value"

int bottom_patterns[11] = {0xD0,0x00,0x98,0x03,0xC8,0x23,0x58,0x01,0xD8,0x23,0x00};
int top_patterns[11] = {0xA1,0xC0,0x83,0xD8,0x22,0x58,0xA3,0xC0,0xA3,0xD8,0x00};


static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void internalCleanupSegDisplay();


static int i2cFileDesc;
static bool shutdownComplete = false;

static bool continueRunning = true;

static int segDisplayFirstDigit = 10;
static int segDisplaySecondDigit = 10;

static pthread_t id;

void initSegDisplay(){
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);

	setIntValueInFile(gpio_61_value_path, 0);
	setIntValueInFile(gpio_44_value_path, 0);

	pthread_create(&id, NULL, segMain, NULL);
}

void setSegDisplay(int output){
	if(output < 0){
		segDisplayFirstDigit = 10;
		segDisplaySecondDigit = 10;
	}
	else if(output < 100){
		segDisplayFirstDigit = output / 10;
		segDisplaySecondDigit = output % 10;
	}
	else{
		segDisplayFirstDigit = 9;
		segDisplaySecondDigit = 9;
	}
}

void* segMain()
{	
	while(continueRunning){
		// Display the first digit
		setIntValueInFile(gpio_61_value_path, 0);
		setIntValueInFile(gpio_44_value_path, 0);
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom_patterns[segDisplayFirstDigit]);
		writeI2cReg(i2cFileDesc, REG_OUTB, top_patterns[segDisplayFirstDigit]);
		setIntValueInFile(gpio_61_value_path, 1);
		setIntValueInFile(gpio_44_value_path, 0);
		

		//sleep 5 ms
		usleep(5000);
				
		// Display the second digit
		setIntValueInFile(gpio_61_value_path, 0);
		setIntValueInFile(gpio_44_value_path, 0);
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom_patterns[segDisplaySecondDigit]);
		writeI2cReg(i2cFileDesc, REG_OUTB, top_patterns[segDisplaySecondDigit]);
		setIntValueInFile(gpio_61_value_path, 0);
		setIntValueInFile(gpio_44_value_path, 1);

		//sleep 5 ms
		usleep(5000);
        
	}

	// Turn off the display
	writeI2cReg(i2cFileDesc, REG_OUTA, bottom_patterns[10]);
	writeI2cReg(i2cFileDesc, REG_OUTB, top_patterns[10]);
	
	internalCleanupSegDisplay();
	return NULL;
}

pthread_t cleanupSegDisplay(){
	continueRunning = false;
	while(!shutdownComplete){
		usleep(10000);
	}
	return id;
}

static int initI2cBus(char* bus, int address)
{
	// Run the config-pin command
    system("config-pin p9_18 i2c  > /dev/null 2>&1");
	system("config-pin p9_17 i2c  > /dev/null 2>&1");

	system("echo out > /sys/class/gpio/gpio61/direction");
	system("echo out > /sys/class/gpio/gpio44/direction");

	system("i2cset -y 1  0x20 0x02 0x00  > /dev/null 2>&1");
	system("i2cset -y 1  0x20 0x03 0x00  > /dev/null 2>&1");


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


static void internalCleanupSegDisplay(){
	// Cleanup I2C access;
	close(i2cFileDesc);
	shutdownComplete = true;
}