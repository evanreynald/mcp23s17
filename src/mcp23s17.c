#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "../include/mcp23s17.h"

int mcp23s17_init(mcp23s17_driver_t *const drv, unsigned char addr)
{
    static int tmp = 0;
    const uint8_t ioconfig = HAEN_ON | SEQOP_OFF;

    drv->spi_fd = open(drv->spi_name, O_RDWR);
    if (drv->spi_fd < 0) 
    {
        printf("Unable to initialize SPIDEV, open file descriptor failed\n");
        return -1;
    }

    // Set spi speed
    tmp = 1000000;
    if (ioctl(drv->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &tmp) < 0) 
    {
        printf("Unable to initialize SPIDEV, setting SPI speed failed\n");
        return -1;
    }
    
    // Set bitorder
    tmp = 0;                                   // 0: MSB-FIRST, 1: LSB-FIRST
    if (ioctl(drv->spi_fd, SPI_IOC_WR_LSB_FIRST, &tmp) < 0) 
    {
        printf("Unable to initialize SPIDEV, setting bit order failed\n");
        return -1;
    }

    // Set bits per word
    tmp = 8;
    if (ioctl(drv->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &tmp) < 0) 
    {
        printf("Unable to initialize SPIDEV, setting bits per word failed\n");
        return -1;
    }

    // Set SPI transaction mode
    tmp = 0;
    if (ioctl(drv->spi_fd, SPI_IOC_WR_MODE, &tmp) < 0) 
    {
        printf("Unable to initialize SPIDEV, setting mode failed\n");
        return -1;
    }

    // Set SPI CS Active Low
    tmp = 0;
    if (ioctl(drv->spi_fd, SPI_IOC_RD_MODE, &tmp) < 0) 
    {
      printf("Unable to initialize SPIDEV, setting SPI CS Active Low failed\n");
      return -1;
    }

    tmp = tmp & ~SPI_CS_HIGH;
    if (ioctl(drv->spi_fd, SPI_IOC_WR_MODE, &tmp) < 0) 
    {
      printf("Unable to initialize SPIDEV, setting SPI CS Active Low Failed\n");
      return -1;
    }

    // set SPI enable CS
    tmp = 0;
    if (ioctl(drv->spi_fd, SPI_IOC_RD_MODE, &tmp) < 0) 
    {
        printf("Unable to initialize SPIDEV, setting SPI Enable CS failed\n");
        return -1;
    }

    tmp = tmp & ~SPI_NO_CS;
    if (ioctl(drv->spi_fd, SPI_IOC_WR_MODE, &tmp) < 0)
    {
        printf("Unable to initialize SPIDEV, setting SPI Enable CS Failed\n");
        return -1;
    }

    // print debug
    printf("\ndebug: spidev_fd = %d\n", drv->spi_fd);
    printf("\ndebug: spidev_name = %s\n", drv->spi_name);

    // set IOCON
    if (mcp23s17_spi_write(drv,addr, IOCON, ioconfig) < 0)
    {
        printf("Error writing IODIRA");
        return -1;
    } 

    // set pull-up resistor enabled
    if (mcp23s17_spi_write(drv,addr, GPPUA, 0xFF) < 0)
    {
        printf("Error writing GPPUA");
        return -1;
    } 

    if (mcp23s17_spi_write(drv,addr, GPPUB, 0xFF) < 0)
    {
        printf("Error writing GPUUB");
        return -1;
    } 

    return drv->spi_fd;

} /* mcp23s17_init() */

int mcp23s17_spi_write(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char val)
{
    unsigned char tx_buf[3]; 
    unsigned char rx_buf[sizeof tx_buf];

    // Write data to the MCP23S17 register
    tx_buf[0] = 0x40 | (addr << 1); // Slave address
    tx_buf[1] = reg; // Register address
    tx_buf[2] = val; // Data to write
    struct spi_ioc_transfer tr = 
    {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = sizeof tx_buf,
        .delay_usecs = sizeof tx_buf * 1040,
    };

    if (ioctl(drv->spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) 
    {
        perror("Failed to write to SPI device");
        return -1;
    }

    return 0;

} /* mcp23s17_spi_write() */

int mcp23s17_spi_read(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char *val)
{
    unsigned char tx_buf[3];
    unsigned char rx_buf[sizeof tx_buf];
    
    // Read data from the MCP23S17 register
    tx_buf[0] = 0x41 | (addr << 1); // Slave address + read bit
    tx_buf[1] = reg; // Register address
    tx_buf[2] = 0; // Register value
    struct spi_ioc_transfer tr = 
    {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = sizeof tx_buf,
        //.delay_usecs = sizeof tx_buf * 1040,
    };

    if (ioctl(drv->spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) 
    {
        perror("Failed to read from SPI device");
        return -1;
    }
    *val = rx_buf[2];

    printf("\nVal = 0x%02x\n", *val);

    return *val;

} /* mcp23s17_spi_read() */

int set_port_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char val)
{  
    if (mcp23s17_spi_write(drv,addr, reg, val) < 0)
    {
        printf("Error writing to selected port");
        return -1;
    }

    return 0;

} /* set_port_direction() */

int get_port_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char *val)
{
    if (mcp23s17_spi_read(drv,addr, reg, val) < 0)
    {
        printf("Error reading from selected port");
        return -1;
    }
    
    return *val;

} /* get_port_direction() */

int set_pin_direction(mcp23s17_driver_t *const drv, unsigned char addr,
                      unsigned char reg, unsigned int pin, unsigned char dir)
{
    uint8_t mask;
    uint8_t rx_data;

    // Get Port
    if (get_port_direction(drv, addr, reg, &rx_data) < 0) 
    {
        printf("Error reading from selected port");
        return -1;
    }

    // Mask Port & Pin
    mask = rx_data;

    if (dir == INPUT_PIN) 
    {
        mask |= (1 << pin);
    } 
    else if (dir == OUTPUT_PIN) 
    {
        mask &= ~(1 << pin);
    } 
    else 
    {
        printf("Wrong pin value");
        return -1;
    }

    // Print Shifted and Masked values
    printf("\nHasil Mask = 0x%02x\n", mask);

    // Write to the pin using SPI
    if (mcp23s17_spi_write(drv, addr, reg, mask) < 0) 
    {
        printf("Error writing to selected pin");
        return -1;
    }

    // Print data after mask
    printf("\nData Setelah Mask= 0x%02x\n", rx_data);

    return rx_data;

} /* set_pin_direction() */

int get_pin_direction(mcp23s17_driver_t *const drv, unsigned char addr,
                      unsigned char reg, unsigned int pin, unsigned char *dir)
{
    uint8_t mask;
    uint8_t rx_data;

    // Get Port
    if (get_port_direction(drv, addr, reg, &rx_data) < 0) 
    {
        printf("Error reading from selected port");
        return -1;
    }

    // Shift dan Mask Pin
    mask = rx_data;
    
    mask &= (1 << pin);

    // Check value dir = 1 atau dir = 0
    *dir = mask ? 1 : 0;

    printf("\nValue pin %d = %d\n", pin, *dir);

    return 0;

} /* get_pin_direction() */

int reset_all_pin(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg)
{
    if (mcp23s17_spi_write(drv,addr, reg, 0x00) < 0)
    {
        printf("Error resetting all pin");
        return -1;
    }

    return 0;

} /* reset_all_pin() */


/*** end of file ***/
