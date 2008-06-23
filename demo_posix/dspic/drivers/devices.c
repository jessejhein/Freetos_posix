/**
 * \file
 * POSIX standard API for file stream
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <errno.h>
#include <asm/types.h>

//-----------------------------------------------------------------------------------------
int _LIBC 
open(const char *pathname, int flags)
{
  int tmp;
  tmp = *pathname - 0x30;		// only support 10 devices, from 0x30 to 0x39

#ifdef UART_MOD
  if (tmp<(BASE_UART + NO_OF_UART))
    {
      return (uart_open(tmp - BASE_UART, flags) == 0)? tmp : -1;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (tmp==BASE_I2C_DAC)
    {
      return (i2c_dac_open(flags) == 0)? tmp : -1;
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (tmp==BASE_EEPROM)
    {
#ifdef NVM_I2C
      return (i2c_eeprom_open(flags) == 0)? tmp : -1;
#endif /* NVM_I2C */
#ifdef NVM_FLASH
      return (flash_eeprom_open(flags) == 0)? tmp : -1;
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return (eeprom_open(flags) == 0)? tmp : -1;
#endif /* NVM_ON_CHIP */
    }
#endif /* NVM_MOD */

#ifdef ADC_MOD
  if (tmp==BASE_ADC)
    {
      return (adc_open(flags) == 0)? tmp : -1;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (tmp==BASE_PWM)
    {
      return (pwm_open(flags) == 0)? tmp : -1;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (tmp==BASE_KB)
    {
      return (gpio_open(flags) == 0)? tmp : -1;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (tmp==BASE_ETHERNET)
    {
      return (dmfe_open(flags) == 0)? tmp : -1;
    }
#endif /* ETHERNET_MOD */

  errno = ENXIO;
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
close(int fd)
{
#ifdef ETHERNET_MOD
  //ETHERNET
  if (fd==BASE_ETHERNET)
    {
      return dmfe_close();
    }
#endif /* ETHERNET_MOD */

  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
write(int fd, void* buf, int count) 
{
  if(fd < 0)
    {
        return -1;
    }
#ifdef UART_MOD
  if (fd < (BASE_UART + NO_OF_UART) )
    {
      return uart_write(fd-BASE_UART, buf, count);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return i2c_dac_write(buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_EEPROM)
    {
#ifdef NVM_I2C
      return i2c_eeprom_write(buf, count);
#endif /* NVM_I2C */
#ifdef NVM_FLASH
      return flash_eeprom_write(buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_write(buf, count);
#endif /* NVM_ON_CHIP */
    }
#endif /* NVM_MOD */

#ifdef ADC_MOD
  if (fd==BASE_ADC)
    {
      return 0;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd==BASE_PWM)
    {
      return pwm_write(buf);
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd==BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd==BASE_ETHERNET)
    {
      return dmfe_write();
    }
#endif /* ETHERNET_MOD */

  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
read(int fd, void* buf, int count)
{
  if(fd < 0)
    {
      return -1;
    }
#ifdef UART_MOD
  if (fd<(BASE_UART + NO_OF_UART))
    {
      return uart_read(fd-BASE_UART, buf);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return i2c_dac_read(buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_EEPROM)
    {
#ifdef NVM_I2C
      return i2c_eeprom_read(buf, count);
#endif /* NVM_I2C */  
#ifdef NVM_FLASH
      return flash_eeprom_read(buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_read(buf, count);
#endif /* NVM_ON_CHIP */
    }
#endif /* NVM_MOD */

#ifdef ADC_MOD
  if (fd==BASE_ADC)
    {
      return adc_read(buf, count);
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd==BASE_PWM)
    {
      return 0;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd==BASE_KB)
    {
      return gpio_read(buf);
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd==BASE_ETHERNET)
    {
      return dmfe_read();
    }
#endif /* ETHERNET_MOD */

	return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
ioctl(int fd, int request, void* argp) 
{
  if(fd < 0)
    {
      return -1;
    }
#ifdef UART_MOD
  if (fd<(BASE_UART + NO_OF_UART))
    {
      return 0;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return i2c_dac_ioctl(request, argp);
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_EEPROM)
    {
      return 0;
    }
#endif /* NVM_MOD */

#ifdef ADC_MOD
  if (fd==BASE_ADC)
    {
      return adc_ioctl(request, argp);
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd==BASE_PWM)
    {
      return pwm_ioctl(request, argp);
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd==BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd==BASE_ETHERNET)
    {
      return 0;
    }
#endif /* ETHERNET_MOD */

	return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
lseek(int fd, int offset, int whence) 
{
  if(fd < 0)
    {
      return -1;
    }
#ifdef UART_MOD
  if (fd<(BASE_UART + NO_OF_UART))
    {
      return 0;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return 0;
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_EEPROM)
    {
#ifdef NVM_I2C
      return i2c_eeprom_lseek(offset, whence);
#endif /* NVM_I2C */   
#ifdef NVM_FLASH
      return flash_eeprom_lseek(offset, whence);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_lseek(offset, whence);
#endif /* NVM_ON_CHIP */
    }
#endif /* NVM_MOD */

#ifdef ADC_MOD
  if (fd==BASE_ADC)
    {
      return 0;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd==BASE_PWM)
    {
      return 0;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd==BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd==BASE_ETHERNET)
    {
      return 0;
    }
#endif /* ETHERNET_MOD */

  return -1;
}
