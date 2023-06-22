// Register addresses
#define IODIRA 0x00  // I/O direction A
#define IODIRB 0x01  // I/O direction B
#define IPOLA 0x02  // I/O polarity A
#define IPOLB 0x03  // I/O polarity B
#define GPINTENA 0x04  // Interupt enable A
#define GPINTENB 0x05  // Interupt enable B
#define DEFVALA 0x06  // Register default value A (interupts)
#define DEFVALB 0x07  // Register default value B (interupts)
#define INTCONA 0x08  // Interupt control A
#define INTCONB 0x09  // Interupt control B
#define IOCON 0x0A  // I/O config (also 0x0B)
#define GPPUA 0x0C  // Port A pullups
#define GPPUB 0x0D  // Port B pullups
#define INTFA 0x0E  // Interupt flag A (where the interupt came from)
#define INTFB 0x0F  // Interupt flag B
#define INTCAPA 0x10  // Interupt capture A (value at interupt is saved here)
#define INTCAPB 0x11  // Interupt capture B
#define GPIOA 0x12  // Port A
#define GPIOB 0x13  // Port B
#define OLATA 0x14  // Output latch A
#define OLATB 0x15  // Output latch B

// I/O Config (IOCON)
#define BANK_ON 0x80 // Addressing mode
#define MIRROR_ON 0x40 // INT pins mirror bit
#define SEQOP_OFF 0x20 // Sequential mode disabled
#define DISSLW_OFF 0x10 // Slew rate disabled
#define HAEN_ON 0x08  // Hardware addressing
#define ODR_ON 0x04 // INT pin open-drain output (WARNING: overrides INTPOL bit)
#define INTPOL_ON 0x02 // INT polarity Active-high

// Set Port
#define INPUT_PORT 0xFF 
#define OUTPUT_PORT 0x00 

// Set Pin
#define INPUT_PIN 0x01
#define OUTPUT_PIN 0x00 

typedef struct 
{
    char spi_name[40];
    int spi_fd;
    unsigned char dev_addr;

} mcp23s17_driver_t;

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   initialization of mcp23s17
 * @details begin and init spi connection with mcp23s17 device
 *          set register IOCON
 *          set register GPPUA and GPPUB
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @return  0 if success, otherwise error code
 */
int mcp23s17_init(mcp23s17_driver_t *const drv, unsigned char addr);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   write data to register
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to write
 * @param   val value to write
 * @return  0 if success, otherwise error code
 */
int mcp23s17_spi_write(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char val);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   read data from register
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to read
 * @param   *val store read value
 * @return  val if success, otherwise error code
 */
int mcp23s17_spi_read(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char *val);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   select register to write 
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to write
 * @param   val value to write
 * @return  0 if success, otherwise error code
 */
int set_port_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char val);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   select register to read 
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to read
 * @param   *val store read value 
 * @return  val if success, otherwise error code
 */
int get_port_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned char *val);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   select one of register pin to write
 * @param   drv mcp23s17 driver struct
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to read
 * @param   pin register's pin (0-7)
 * @param   dir pin direction to write (0 = output, 1 = input)
 * @return  val if success, otherwise error code
 */
int set_pin_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                        unsigned char reg, unsigned int pin, unsigned char dir);

/**
 * @author  Evandrew Reynald Collin & Azzahra Nadya Kahpiasa
 * @brief   select one of register pin to read
 * @param   drv mcp23s17 driver struct 
 * @param   addr base address of mcp23s17 device
 * @param   reg register address to read
 * @param   pin register's pin (0-7)
 * @param   *dir store pin direction (0 = output, 1 = input)
 * @return  0 if success, otherwise error code
 */
int get_pin_direction(mcp23s17_driver_t *const drv, unsigned char addr, 
                       unsigned char reg, unsigned int pin, unsigned char *dir);


/*** end of file ***/