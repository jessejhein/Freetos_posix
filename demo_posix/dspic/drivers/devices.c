/**
 * \file
 * POSIX standard API for file stream
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <errno.h>
#include <asm/types.h>
#include <adc.h>
#ifdef ETHERNET_MOD
#include <dm9000a.h>
#endif /* ETHERNET_MOD */
#include <flash_eeprom.h>
#include <gpdi.h>
#include <i2c_adc.h>
#include <i2c_dac.h>
#include <i2c_temp.h>
#include <i2c_eeprom.h>
#include <i2c_mod_master_dspic.h>
#include <i2c_mod_slave_dspic.h>
#include <i2c_led_driver.h>
#include <kb.h>
#include <led.h>
#include <pwm.h>
#include <uart.h>

//-----------------------------------------------------------------------------------------
/**
 * \remarks 
 * \li valid pathname spans from 'A' - 'Z' (i.e. support 26 devices at max)
 * \li return file handler 3 -28 respectively
 * \li file handler 0, 1, 2 are reserved for stdout, stdin, and stderr respectively     
 */
int _LIBC 
open (const char *pathname, int flags)
{
  int tmp;
  tmp = *pathname - 0x3E; //0x3E = '>'
  
  //STDOUT, STDIN, STDERR
  if (tmp < 3)
    {
      return tmp;
    }

#ifdef UART_MOD
  if (tmp < (BASE_COM + NO_OF_UART))
    {
      return (uart_open (tmp - BASE_COM, flags) == 0)? tmp : -1;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (tmp == BASE_I2C_DAC)
    {
      return (i2c_dac_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
  if (tmp == BASE_I2C_ADC)
    {
      return (i2c_adc_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
  if (tmp == BASE_I2C_TEMP)
    {
      return (i2c_temp_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
  if (tmp == BASE_NVM)
    {
#ifdef NVM_FLASH
      return (flash_eeprom_open (flags) == 0)? tmp : -1;
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return (eeprom_open (flags) == 0)? tmp : -1;
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if (tmp == (BASE_NVM + 1))
    {
      return (i2c_eeprom_open (flags) == 0)? tmp : -1;
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (tmp == BASE_I2C_MOD_DSPIC)
    {
      return (i2c_mod_master_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (tmp == BASE_I2C_MOD_DSPIC)
    {
      return (i2c_mod_slave_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
  if (tmp == BASE_I2C_LED_DRIVER)
    {
      return (i2c_led_driver_open (flags) == 0)? tmp : -1;
    }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef ADC_MOD
  if (tmp == BASE_ADC)
    {
      return (adc_open (flags) == 0)? tmp : -1;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (tmp == BASE_PWM)
    {
      return (pwm_open (flags) == 0)? tmp : -1;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (tmp == BASE_KB)
    {
      return (kb_open (flags) == 0)? tmp : -1;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (tmp == BASE_ETHERNET)
    {
      return (dmfe_open (flags) == 0)? tmp : -1;
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (tmp == BASE_LED)
    {
      return (led_open (flags) == 0)? tmp : -1;
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (tmp == BASE_LCD)
    {
      return tmp;
    }
#endif /* LCD_MOD */

#ifdef GPDI_MOD
  if (tmp == BASE_GPDI)
    {
      return (gpdi_open (flags) == 0)? tmp : -1;
    }
#endif /* GPDI_MOD */
  
  errno = ENXIO;
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
close (int fd)
{
#ifdef UART_MOD
  if (fd < (BASE_COM + NO_OF_UART))
    {
      return uart_close (fd-BASE_COM);
    }
#endif /* UART_MOD */
#ifdef ETHERNET_MOD
  if (fd == BASE_ETHERNET)
    {
      return dmfe_close ();
    }
#endif /* ETHERNET_MOD */

  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
write (int fd, void* buf, int count) 
{
  if (fd < 0)
    {
      return -1;
    }
  //DEBUG PORT
  if (fd < 3)
    {
      return uart_write (DEBUG_UART, buf, count);
    }

#ifdef UART_MOD
  if (fd < (BASE_COM + NO_OF_UART))
    {
      return uart_write (fd-BASE_COM, buf, count);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd == BASE_I2C_DAC)
    {
      return i2c_dac_write (buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
  if (fd == BASE_I2C_ADC)
    {
      return 0;
    }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
  if (fd == BASE_I2C_TEMP)
    {
      return 0;
    }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
  if (fd == BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_write (buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_write (buf, count);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if (fd == (BASE_NVM + 1))
    {
      return i2c_eeprom_write (buf, count);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_write (buf);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_write (buf);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
  if (fd == BASE_I2C_LED_DRIVER)
    {
      return i2c_led_driver_write (buf);
    }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef ADC_MOD
  if (fd == BASE_ADC)
    {
      return 0;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd == BASE_PWM)
    {
      return pwm_write (buf);
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd == BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd == BASE_ETHERNET)
    {
      return dmfe_write ();
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (fd == BASE_LED)
    {
      return led_write (buf);
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (fd == BASE_LCD)
    {
      return 0;
    }
#endif /* LCD_MOD */

#ifdef GPDI_MOD
  if (fd == BASE_GPDI)
    {
      return 0;
    }
#endif /* GPDI_MOD */
  
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
read (int fd, void* buf, int count)
{
  if (fd < 3)
    {
      return -1;
    }
#ifdef UART_MOD
  if (fd < (BASE_COM + NO_OF_UART))
    {
      return uart_read (fd-BASE_COM, buf);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd == BASE_I2C_DAC)
    {
      return i2c_dac_read (buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
  if (fd == BASE_I2C_ADC)
    {
      return i2c_adc_read (buf, count);
    }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
  if (fd == BASE_I2C_TEMP)
    {
      return i2c_temp_read (buf, count);
    }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
  if (fd == BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_read (buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_read (buf, count);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if (fd == (BASE_NVM + 1))
    {
      return i2c_eeprom_read (buf, count);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_read (buf);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_read (buf);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
  if (fd == BASE_I2C_LED_DRIVER)
    {
      return 0;
    }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef ADC_MOD
  if (fd == BASE_ADC)
    {
      return adc_read (buf, count);
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd == BASE_PWM)
    {
      return 0;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd == BASE_KB)
    {
      return kb_read (buf);
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd == BASE_ETHERNET)
    {
      return dmfe_read ();
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (fd == BASE_LED)
    {
      return 0;
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (fd == BASE_LCD)
    {
      return 0;
    }
#endif /* LCD_MOD */

#ifdef GPDI_MOD
  if (fd == BASE_GPDI)
    {
      return gpdi_read (buf);
    }
#endif /* GPDI_MOD */

	return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
ioctl (int fd, int request, void* argp) 
{
  if (fd < 3)
    {
      return -1;
    }

#ifdef UART_MOD
  if (fd < (BASE_COM + NO_OF_UART))
    {
      return uart_ioctl (fd - BASE_COM, request, argp);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd == BASE_I2C_DAC)
    {
      return i2c_dac_ioctl (request, argp);
    }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
  if (fd == BASE_I2C_ADC)
    {
      return i2c_adc_ioctl (request, argp);
    }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
  if (fd == BASE_I2C_TEMP)
    {
      return i2c_temp_ioctl (request, argp);
    }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
  if((fd >= BASE_NVM) && (fd<(BASE_NVM+1)))
    {
      return 0;
    }
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_ioctl (request, argp);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_ioctl (request, argp);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
  if (fd == BASE_I2C_LED_DRIVER)
    {
    return i2c_led_driver_ioctl (request, argp);
    }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef ADC_MOD
  if(fd==BASE_ADC)
    {
      return adc_ioctl(request, argp);
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd == BASE_PWM)
    {
      return pwm_ioctl (request, argp);
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd == BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd == BASE_ETHERNET)
    {
      return 0;
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (fd == BASE_LED)
    {
      return led_ioctl (request, argp);
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (fd == BASE_LCD)
    {
      return lcd_ioctl (request, argp);
    }
#endif /* LCD_MOD */

#ifdef GPDI_MOD
  if (fd == BASE_GPDI)
    {
      return gpdi_ioctl (request, argp);
    }
#endif /* GPDI_MOD */

  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
lseek (int fd, int offset, int whence) 
{
  if (fd < 3)
    {
      return -1;
    }
#ifdef UART_MOD
  if (fd < (BASE_COM + NO_OF_UART))
    {
      return 0;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd == BASE_I2C_DAC)
    {
      return 0;
    }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
  if (fd == BASE_I2C_ADC)
    {
      return 0;
    }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
  if (fd == BASE_I2C_TEMP)
    {
      return 0;
    }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
  if (fd == BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_lseek (offset, whence);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_lseek (offset, whence);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if (fd == (BASE_NVM + 1))
    {
      return i2c_eeprom_lseek (offset, whence);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return 0;
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd == BASE_I2C_MOD_DSPIC)
    {
      return 0;
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
  if (fd == BASE_I2C_LED_DRIVER)
    {
    return 0;
    }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef ADC_MOD
  if (fd == BASE_ADC)
    {
      return 0;
    }
#endif /* ADC_MOD */

#ifdef PWM_MOD
  if (fd == BASE_PWM)
    {
      return 0;
    }
#endif /* PWM_MOD */

#ifdef KB_MOD
  if (fd == BASE_KB)
    {
      return 0;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd == BASE_ETHERNET)
    {
      return 0;
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (fd == BASE_LED)
    {
      return 0;
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (fd == BASE_LCD)
    {
      return 0;
    }
#endif /* LCD_MOD */

#ifdef GPDI_MOD
  if (fd == BASE_GPDI)
    {
      return 0;
    }
#endif /* GPDI_MOD */

  return -1;
}
