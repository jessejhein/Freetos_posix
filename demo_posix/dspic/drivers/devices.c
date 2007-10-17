/***************************************************************************************
 * File: 			devices.c
 * Description:		file access using posix
 ***************************************************************************************/

#include <define.h>
#include <errno.h>

/***************************************************************************************
 * IO Stub Functions are placed in .libc section so that the standard libraries
 * can access these functions using short jumps.
 ***************************************************************************************/
#define LIBC_CODE_LOC       __attribute__ ((section(".libc")))

/****************************************************************************************
 * Function:    int open(const char *pathname, int flags)
 * 
 * Description: get the device ready
 *              +-- all devices can be open with O_RDWR/O_RDONLY/O_WRONLY except 
 *                  +-- ADC can only be opened for O_RDONLY mode
 *                  +-- PWM can be opened with all mode, but only write operation is supported
 *                  +-- when I2C EEPROM is in NON-BLOCKING mode, application can only write 
 *                      upto 64 bytes at a time to eeprom
 * 
 * Input:       pathname: (= device BASE (e.g. "0") + sub-item)
 *              flags: accessing mode
 * 
 * Output:      file descriptor: 0 - 9
 *              -1: when error occur, sub-functions should set errno appropricately
 ****************************************************************************************/
int LIBC_CODE_LOC open(const char *pathname, int flags)
{
    int tmp;
    tmp = *pathname - 0x30;		// only support 10 devices, from 0x30 to 0x39

#ifdef UART_MOD
    //UARTs
    if (tmp<(BASE_UART + NO_OF_UART))
    {
        return (uart_open(tmp - BASE_UART, flags) == 0)? tmp : -1;
    }
#endif

#ifdef I2C_DAC_MOD
    //I2C DAC 
    if (tmp==BASE_I2C_DAC)
    {
        return (i2c_dac_open(flags) == 0)? tmp : -1;
    }
#endif

#ifdef NVM_MOD
    if (tmp==BASE_EEPROM)
    {
        #ifdef NVM_I2C
            //I2C EEPROM
            return (i2c_eeprom_open(flags) == 0)? tmp : -1;
        #endif   
        #ifdef NVM_FLASH
            //FLASH EEPROM
            return (flash_eeprom_open(flags) == 0)? tmp : -1;
        #endif
        #ifdef NVM_ON_CHIP
            //ON-CHIP EEPROM
            return (eeprom_open(flags) == 0)? tmp : -1;
        #endif
    }
#endif

#ifdef ADC_MOD
    // ADC
	if (tmp==BASE_ADC)
	{
		return (adc_open(flags) == 0)? tmp : -1;
	}
#endif

#ifdef PWM_MOD
    //PWM
	if (tmp==BASE_PWM)
	{
		return (pwm_open(flags) == 0)? tmp : -1;
	}
#endif

#ifdef KB_MOD
    //KB
    if (tmp==BASE_KB)
    {
        return (gpio_open(flags) == 0)? tmp : -1;
    }
#endif

#ifdef ETHERNET_MOD
    //ETHERNET
    if (tmp==BASE_ETHERNET)
    {
        return (dmfe_open(flags) == 0)? tmp : -1;
    }
#endif

    errno = ENXIO;
	return -1;
}

/****************************************************************************************
 * Function:    int close(int fd)
 * 
 * Description: release resource
 * 
 * Input:       fd: file descriptor generated by open()
 * 
 * Output:      Upon successful completion, 0 shall be returned; 
 *              otherwise, -1 shall be returned and errno set to indicate the error.
 ****************************************************************************************/
int LIBC_CODE_LOC close(int fd)
{
#ifdef ETHERNET_MOD
    //ETHERNET
    if (fd==BASE_ETHERNET)
    {
        return dmfe_close();
    }
#endif

    return -1;    
}

/****************************************************************************************
 * Function:    int write(int fd, void* buf, int count)
 * 
 * Description: write $count bytes from $buf to device
 * 
 * Input:       fd: file descriptor generated by open()
 *              buf: buffer which contains data to write out
 *              count: length of data (in bytes) to write out
 *                      +-- when I2C EEPROM is in NON-BLOCKING mode, application can 
 *                          only write upto 64 bytes at a time to eeprom
 * 
 * Output:      number of bytes actually written to the file associated
 *              0: no byte is written (busy/count = 0/communication fail)
 *              -1: when error occur
 * 
 * Note:        For i2c devices, if there are multiple devices, write cannot be used in ISR
 ****************************************************************************************/
int LIBC_CODE_LOC write(int fd, void* buf, int count) 
{
#ifdef UART_MOD
    //UART
    if (fd < (BASE_UART + NO_OF_UART) )
    {
        return uart_write(fd-BASE_UART, buf, count);
    }
#endif

#ifdef I2C_DAC_MOD
    //I2C DAC
    if (fd==BASE_I2C_DAC)
    {
        return i2c_dac_write(buf);
    }
#endif

#ifdef NVM_MOD
    if (fd==BASE_EEPROM)
    {
        #ifdef NVM_I2C
            //I2C EEPROM
            return i2c_eeprom_write(buf, count);
        #endif   
        #ifdef NVM_FLASH
            //FLASH EEPROM
            return flash_eeprom_write(buf, count);
        #endif
        #ifdef NVM_ON_CHIP
            //ON-CHIP EEPROM
            return eeprom_write(buf, count);
        #endif
    }
#endif

#ifdef ADC_MOD
    //ADC
	if (fd==BASE_ADC)
	{
		return 0;
	}
#endif

#ifdef PWM_MOD
    //PWM
	if (fd==BASE_PWM)
	{
		return pwm_write(buf);
	}
#endif

#ifdef KB_MOD
    //KB
    if (fd==BASE_KB)
    {
        return 0;
    }
#endif

#ifdef ETHERNET_MOD
    //ETHERNET
    if (fd==BASE_ETHERNET)
    {
        return dmfe_write();
    }
#endif

    return -1;
}

/****************************************************************************************
 * Function:    int read(int fd, void* buf, int count)
 * 
 * Description: read $count bytes from device to $buf
 * 
 * Input:       fd: file descriptor generated by open()
 *              buf: buffer which contains allocation to read in data
 *              count: length of data (in bytes) to read from
 * 
 * Output:      number of bytes actually read from the file associated
 *              0: no byte is read (busy/count = 0/communication fail)
 *              -1: when error occur
 * 
 * Note:        For i2c devices, if there are multiple devices, read cannot be used in ISR
 ****************************************************************************************/
int LIBC_CODE_LOC read(int fd, void* buf, int count)
{
#ifdef UART_MOD
    //UART
    if (fd<(BASE_UART + NO_OF_UART))
    {
        return uart_read(fd-BASE_UART, buf);
    }
#endif

#ifdef I2C_DAC_MOD
    //I2C DAC
    if (fd==BASE_I2C_DAC)
    {
        return i2c_dac_read(buf);
    }
#endif

#ifdef NVM_MOD
    if (fd==BASE_EEPROM)
    {
        #ifdef NVM_I2C
            //I2C EEPROM
            return i2c_eeprom_read(buf, count);
        #endif   
        #ifdef NVM_FLASH
            //FLASH EEPROM
            return flash_eeprom_read(buf, count);
        #endif
        #ifdef NVM_ON_CHIP
            //ON-CHIP EEPROM
            return eeprom_read(buf, count);
        #endif
    }
#endif

#ifdef ADC_MOD
    //ADC
	if (fd==BASE_ADC)
	{
		return adc_read(buf, count);
	}
#endif

#ifdef PWM_MOD
    //PWM
	if (fd==BASE_PWM)
	{
		return 0;
	}
#endif

#ifdef KB_MOD
    //KB
    if (fd==BASE_KB)
    {
        return gpio_read(buf);
    }
#endif

#ifdef ETHERNET_MOD
    //ETHERNET
    if (fd==BASE_ETHERNET)
    {
        return dmfe_read();
    }
#endif

	return -1;
}

/****************************************************************************************
 * Function:    int ioctl(int fd, int request, void* argp)
 * 
 * Description: change the device control setting (type referenced by $request) using
 *              data in $argp
 * 
 * Input:       fd: file descriptor generated by open()
 *              request: a device-dependent request code, defined in <sys/ioctl.h>
 *              argp: argument pointer for control data
 * 
 * Output:      0: change is successful
 *              -1: when error occur
 ****************************************************************************************/
int LIBC_CODE_LOC ioctl(int fd, int request, void* argp) 
{
#ifdef UART_MOD
    //UART
    if (fd<(BASE_UART + NO_OF_UART))
    {
        return 0;
    }
#endif

#ifdef I2C_DAC_MOD
    //I2C DAC
    if (fd==BASE_I2C_DAC)
    {
        return i2c_dac_ioctl(request, argp);
    }
#endif

#ifdef NVM_MOD
    if (fd==BASE_EEPROM)
    {
        return 0;
    }
#endif

#ifdef ADC_MOD
    //ADC
	if (fd==BASE_ADC)
	{
		return adc_ioctl(request, argp);
	}
#endif

#ifdef PWM_MOD
    //PWM
	if (fd==BASE_PWM)
	{
		return pwm_ioctl(request, argp);
	}
#endif

#ifdef KB_MOD
    //KB
    if (fd==BASE_KB)
    {
        return 0;
    }
#endif

#ifdef ETHERNET_MOD
    //ETHERNET
    if (fd==BASE_ETHERNET)
    {
        return 0;
    }
#endif

	return -1;
}

/****************************************************************************************
 * Function:    int lseek(int fd, int offset, int whence)
 * 
 * Description: adjust the position of the file pointer
 * 
 * Input:       fd: file descriptor generated by open()
 *              offset: indicates how much offset is needed
 *              whence: indicates what kind of offset is needed, currently unimplemented
 * 
 * Output:      0: current offset from base address
 *              -1: when error occur
 ****************************************************************************************/
int LIBC_CODE_LOC lseek(int fd, int offset, int whence) 
{
#ifdef UART_MOD
     //UART
     if (fd<(BASE_UART + NO_OF_UART))
     {
        return 0;
     }
#endif

#ifdef I2C_DAC_MOD
    //I2C DAC
    if (fd==BASE_I2C_DAC)
    {
        return 0;
    }
#endif

#ifdef NVM_MOD
    if (fd==BASE_EEPROM)
    {
        #ifdef NVM_I2C
            //I2C EEPROM
            return i2c_eeprom_lseek(offset, whence);
        #endif   
        #ifdef NVM_FLASH
            //FLASH EEPROM
            return flash_eeprom_lseek(offset, whence);
        #endif
        #ifdef NVM_ON_CHIP
            //ON-CHIP EEPROM
            return eeprom_lseek(offset, whence);
        #endif
    }
#endif

#ifdef ADC_MOD
    //ADC
	if (fd==BASE_ADC)
	{
		return 0;
	}
#endif

#ifdef PWM_MOD
    //PWM
	if (fd==BASE_PWM)
	{
		return 0;
	}
#endif

#ifdef KB_MOD
    //KB
    if (fd==BASE_KB)
    {
        return 0;
    }
#endif

#ifdef ETHERNET_MOD
    //ETHERNET
    if (fd==BASE_ETHERNET)
    {
        return 0;
    }
#endif

	return -1;
}
