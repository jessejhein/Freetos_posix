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
  tmp = *pathname - 'A';		// only support 26 devices, from 0x41 to 0x5a

#ifdef UART_MOD
  if (tmp<(BASE_COM + NO_OF_UART))
    {
      return (uart_open(tmp - BASE_COM, flags) == 0)? tmp : -1;
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (tmp==BASE_I2C_DAC)
    {
      return (i2c_dac_open(flags) == 0)? tmp : -1;
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (tmp==BASE_NVM)
    {
#ifdef NVM_FLASH
      return (flash_eeprom_open(flags) == 0)? tmp : -1;
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return (eeprom_open(flags) == 0)? tmp : -1;
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if( tmp == (BASE_NVM+1) )
    {
      return (i2c_eeprom_open(flags) == 0)? tmp : -1;
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (tmp==BASE_I2C_MOD_DSPIC)
    {
      return (i2c_mod_master_open(flags) == 0)? tmp : -1;
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (tmp==BASE_I2C_MOD_DSPIC)
    {
      return (i2c_mod_slave_open(flags) == 0)? tmp : -1;
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

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
      return (kb_open(flags) == 0)? tmp : -1;
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (tmp==BASE_ETHERNET)
    {
      return (dmfe_open(flags) == 0)? tmp : -1;
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (tmp==BASE_LED)
    {
      return (led_open(flags) == 0)? tmp : -1;
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (tmp==BASE_LCD)
    {
      return tmp;
    }
#endif /* LCD_MOD */

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
  if (fd < (BASE_COM + NO_OF_UART) )
    {
      return uart_write(fd-BASE_COM, buf, count);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return i2c_dac_write(buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_write(buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_write(buf, count);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if( fd == (BASE_NVM+1) )
    {
      return i2c_eeprom_write(buf, count);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_write(buf);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_write(buf);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

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

#ifdef LED_MOD
  if (fd==BASE_LED)
    {
      return led_write(buf);
    }
#endif /* LED_MOD */

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
  if (fd<(BASE_COM + NO_OF_UART))
    {
      return uart_read(fd-BASE_COM, buf);
    }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
  if (fd==BASE_I2C_DAC)
    {
      return i2c_dac_read(buf);
    }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
  if (fd==BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_read(buf, count);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_read(buf, count);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if( fd == (BASE_NVM+1) )
    {
      return i2c_eeprom_read(buf, count);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_read(buf);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_read(buf);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

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
      return kb_read(buf);
    }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
  if (fd==BASE_ETHERNET)
    {
      return dmfe_read();
    }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
  if (fd==BASE_LED)
    {
      return 0;
    }
#endif /* LED_MOD */

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
  if (fd<(BASE_COM + NO_OF_UART))
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
  if( (fd >= BASE_NVM) && (fd<(BASE_NVM+1)) )
    {
      return 0;
    }
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_master_ioctl(request, argp);
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return i2c_mod_slave_ioctl(request, argp);
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

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

#ifdef LED_MOD
  if (fd==BASE_LED)
    {
      return led_ioctl(request, argp);
    }
#endif /* LED_MOD */

#ifdef LCD_MOD
  if (fd==BASE_LCD)
    {
      return lcd_ioctl(request, argp);
    }
#endif /* LCD_MOD */

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
  if (fd<(BASE_COM + NO_OF_UART))
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
  if (fd==BASE_NVM)
    {
#ifdef NVM_FLASH
      return flash_eeprom_lseek(offset, whence);
#endif /* NVM_FLASH */
#ifdef NVM_ON_CHIP
      return eeprom_lseek(offset, whence);
#endif /* NVM_ON_CHIP */
    }
#ifdef NVM_I2C
  if( fd == (BASE_NVM+1) )
    {
      return i2c_eeprom_lseek(offset, whence);
    }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return 0;
    }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
  if (fd==BASE_I2C_MOD_DSPIC)
    {
      return 0;
    }
#endif /* I2C_MOD_SLAVE_DSPIC */

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

#ifdef LED_MOD
  if (fd==BASE_LED)
    {
      return 0;
    }
#endif /* LED_MOD */

  return -1;
}
