/*
 * pin_define.h
 */

#ifndef PIN_DEFINE_H_
#define PIN_DEFINE_H_

//  ===========================ERROR PIN===================================
/**************************************************************************
 * ==ERROR PIN==
 * *Flashing LED0:          Address Error
 * *Flashing LED1:          Maths Error
 * *LED0 On:                Stack Error
 * *LED1 On:                DMA Error (dspic33 only)
 * *Flashing LED0 & LED1:   Not enough memory to start FreeRTOS scheduler
 **************************************************************************/
#define ERR_LED0_EN()                (TRISG &= 0x7FFF)   //_RG15
#define ERR_LED1_EN()                (TRISC &= 0x7FFF)   //_RC15
#define ERR_LED0(x)                  do{ \
                                        if((x)==0) LATG &= 0x7FFF; \
                                        else LATG |= 0x8000; \
                                      }while(0) 
#define ERR_LED1(x)                  do{ \
                                        if((x)==0) LATC &= 0x7FFF; \
                                        else LATC |= 0x8000; \
                                      }while(0)  



//  ==============================UART=====================================
#ifdef UART_MOD
/**************************************************************************
 * Function:        void RS485_CTL_EN(void)
 * 
 * Description:     set the RS485 digital control pin as output
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define RS485_CTL_EN()              _TRISG1=0

/**************************************************************************
 * Function:        void RS485_TX_ON(void)
 * 
 * Description:     set the RS485 digital control pin to tx state
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define RS485_TX_ON()               _RG1=1

/**************************************************************************
 * Function:        void RS485_TX_OFF(void)
 * 
 * Description:     set the RS485 digital control pin to rx state
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define RS485_TX_OFF()              _RG1=0
#endif //UART_MOD



//  ==============================KB======================================
#ifdef KB_MOD
/**************************************************************************
 * ==KEY MAPS==
 * *PKEY: Push Key (i.e. enter key)
 * *RKEY: Rotary Key
 * *EN: input/output selection
 * *IO: logic state of key
 * *CN: change notification (interrupt enable pin to detect change of state)
 **************************************************************************/
//Enter key
#define EN_PKEY0                    _TRISG14
#define IO_PKEY0                    _RG14
//UP key
#define EN_RKEY0_UP                 _TRISC14
#define IO_RKEY0_UP                 _RC14
#define CN_RKEY0_UP                 _CN0IE    // change input register, interrupt setting 
//Down Key
#define EN_RKEY0_DN                 _TRISC13
#define IO_RKEY0_DN                 _RC13
#define CN_RKEY0_DN                 _CN1IE    // change input register, interrupt setting 
#endif //KB_MOD



//  ===========================ETHERNET PORT===============================
#ifdef ETHERNET_MOD
/**************************************************************************
 * ==KEY MAPS==
 * *Interrupt Key Map 
 **************************************************************************/
#define ETH_ISR_EP                  _INT0EP
#define ETH_ISR_IF                  _INT0IF
#define ETH_ISR_IP                  _INT0IP
#define ETH_ISR_IE                  _INT0IE
#define dmfe_interrupt              _ISR _INT0Interrupt

/**************************************************************************
 * Function:        void ETH_IOCONFIG(void)
 * 
 * Description:     initialize IOs for ethernet port
 *                      ETH_CS#: output, [do not select chip]
 *                      ADDR0: output, [addressing mode]
 *                      IOR#: output, [not read]
 *                      IOW#: output, [not write]
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define ETH_IOCONFIG()               do{ \
                                        TRISG &= 0xFC3F; \
                                        LATG |= 0x03C0; \
                                      } while(0)


/**************************************************************************
 * Function:        void ETH_IOCMD(int cmd)
 * 
 * Description:     set the io pins to read/write an address/data register
 * 
 * Input:           cmd
 *                   bit 0: read mode
 *                   bit 1: write mode
 *                   bit 2: address/data
 * 
 * Output:          null
 **************************************************************************/
#define ETH_IOCMD(x)                 ( LATG = ((PORTG & 0xFC3F) | ((((int)x) << 6) & 0x03C0)) ) 
#endif


//  ===========================DATA PORT===================================
/**************************************************************************
 * Function:        void PCONFIG(int cmd)
 * 
 * Description:     config data port pins to output or input
 * 
 * Input:           0: output
 *                  1: input
 * 
 * Output:          null
 **************************************************************************/
#define PCONFIG(x)                   do{ \
                                        if((x)==0) TRISD &= 0xF00F; \
                                        else TRISD |= 0x0FF0; \
                                      } while(0)

/**************************************************************************
 * Function:        void PWRITE(unsigned char data)
 * 
 * Description:     write a byte to data port
 * 
 * Input:           8-bit data
 * 
 * Output:          null
 **************************************************************************/
#define PWRITE(x)                    ( LATD = ((PORTD & 0xF00F) | ((((int)x) << 4) & 0x0FF0)) )

/**************************************************************************
 * Function:        unsigned char PREAD(void)
 * 
 * Description:     read a byte from data port
 * 
 * Input:           null
 * 
 * Output:          8-bit data
 **************************************************************************/
#define PREAD()                      ( (unsigned char)( (PORTD & 0x0FF0) >> 4 ) )                    



//  ===============================LCD=====================================
/**************************************************************************
 * Function:        void mwlike8_hw_init(void)
 * 
 * Description:     initialize IOs for lcd display
 *                    LCD_CS1#: output, [do not select chip]
 *                    ADDR0: output, [addressing mode]
 *                    DATA PORT: output
 *  
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define mwlike8_hw_init()       do {\
                                    TRISF &= 0xFFFC;\
                                    PCONFIG(0);\
                                } while (0)

/**************************************************************************
 * ==MAPS==
 **************************************************************************/
#define en_enCsP_io()           (TRISF &= 0xFFFE)
#define en_A0P_io()             (TRISF &= 0xFFFD)
#define enCsP                   do {\
                                    Nop();Nop();\
                                    _LATF0 = 1;\
                                } while (0)
#define disCsP                  _LATF0 = 0
#define lowA0P                  _LATF1 = 0
#define highA0P                 _LATF1 = 1
#define en_data_bus()           PCONFIG(0)

/**************************************************************************
 * Function:        void wr_data_bus(unsigned char wrData)
 * 
 * Description:     write a byte to lcd display
 *  
 * Input:           8-bit data
 * 
 * Output:          null
 **************************************************************************/
#define wr_data_bus(wrData)     PWRITE(wrData)

/**************************************************************************
 * Constant for Amonics Logo
 **************************************************************************/                            
//extern const unsigned char logo[];



//  ===========================LASER DCTRL=================================
/**************************************************************************
 * Function:        void laser_digit_enable(unsigned char which_laser);
 * 
 * Description:     set the digital control pin to enable state
 * 
 * Input:           laser id
 * 
 * Output:          null
 **************************************************************************/
extern void laser_digit_enable(unsigned char which_laser);


/**************************************************************************
 * Function:        void laser_digit_disable(unsigned char which_laser);
 * 
 * Description:     set the digital control pin to disable state
 * 
 * Input:           laser id
 * 
 * Output:          null
 **************************************************************************/
extern void laser_digit_disable(unsigned char which_laser);



//  ===========================LASER ADC===================================
/**************************************************************************
 * Function:        void init_adc(void);
 * 
 * Description:     initialize adc module
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#include <fcntl.h>
extern int fd_adc;
#define init_adc() {\
    fd_adc = open(ADC, O_RDONLY | O_NONBLOCK);\
}

/**************************************************************************
 * Function:        unsigned char TEMP_PIN_X(void);
 * 
 * Description:     add analog channel for temperature X for sampling
 * 
 * Input:           null
 * 
 * Output:          0
 **************************************************************************/
#if (TEC_T>=1)
#define TEMP_PIN_1() {\
    unsigned char ctrl_buf = 15;        /*Port 0: Pin 14 (TEMP 0)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=2)
#define TEMP_PIN_2() {\
    unsigned char ctrl_buf = 11;        /*Port 1: Pin 14 (TEMP 1)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=3)
#define TEMP_PIN_3() {\
    unsigned char ctrl_buf = 3;        /*Port EXT: Pin 5 (TEMP 2)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=4)
#define TEMP_PIN_4() {\
    unsigned char ctrl_buf = 15;\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=5)
#define TEMP_PIN_5() {\
    unsigned char ctrl_buf = 15;\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=6)
#define TEMP_PIN_6() {\
    unsigned char ctrl_buf = 2;        /*CASE_TEMP*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif

/**************************************************************************
 * Function:        unsigned char PWR_PIN_X(void);
 * 
 * Description:     add analog channel for power X for sampling
 * 
 * Input:           null
 * 
 * Output:          0
 **************************************************************************/
#if (TOTAL_LD_PWR>=1)
#define PWR_PIN_1() {\
    unsigned char ctrl_buf = 13;        /*Port 0: Pin 9 (PWR 1)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=2)
#define PWR_PIN_2() {\
    unsigned char ctrl_buf = 12;        /*Port 0: Pin 4 (PWR 0)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=3)
#define PWR_PIN_3() {\
    unsigned char ctrl_buf = 9;         /*Port 1: Pin 9 (PWR 3)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=4)
#define PWR_PIN_4() {\
    unsigned char ctrl_buf = 8;         /*Port 1: Pin 4 (PWR 2)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=5)
#define PWR_PIN_5() {\
    unsigned char ctrl_buf = 1;         /*Port EXT: Pin 2 (PWR 5)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=6)
#define PWR_PIN_6() {\
    unsigned char ctrl_buf = 5;         /*Port EXT: Pin 1 (PWR 4)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif

/**************************************************************************
 * Function:        unsigned char CUR_PIN_X(void);
 * 
 * Description:     add analog channel for feedback current X for sampling
 * 
 * Input:           null
 * 
 * Output:          0
 **************************************************************************/
#if (TOTAL_LD_CUR>=1)
#define CUR_PIN_1() {\
   unsigned char ctrl_buf = 14;        /*Port 0: Pin 12 (CUR 0)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=2)
#define CUR_PIN_2() {\
   unsigned char ctrl_buf = 10;        /*Port 1: Pin 12 (CUR 1)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=3)
#define CUR_PIN_3() {\
   unsigned char ctrl_buf = 4;         /*Port EXT: Pin 3 (CUR 2)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=4)
#define CUR_PIN_4() {\
   unsigned char ctrl_buf = 14;\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=5)
#define CUR_PIN_5() {\
   unsigned char ctrl_buf = 14;\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=6)
#define CUR_PIN_6() {\
   unsigned char ctrl_buf = 14;\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif



//  ===========================ABC CTRL====================================
#ifdef AUTO_BIAS_CTRL
#define ABC_OUT_PWR_PIN() {\
   unsigned char ctrl_buf = 2;         /*Port EXT: Pin 3 (CUR 2)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#define ABC_BIAS_MON_PIN() {\
   unsigned char ctrl_buf = 3;        /*Port EXT: Pin 5 (TEMP 2)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif



//  ===========================LASER SETPT=================================
#if (SETPT_T>0)
/**************************************************************************
 * Function:        void init_dac(void);
 * 
 * Description:     initialize dac module and digitial control pins
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
extern void init_dac(void);

/**************************************************************************
 * Function:        void wr_dac(unsigned char which_laser, int raw_dac);
 * 
 * Description:     write setpt value of laser (dac value) 
 * 
 * Input:           which_laser: laser id
 *                  raw_dac:     raw dac value
 * 
 * Output:          null
 **************************************************************************/
extern void wr_dac(unsigned char which_laser, int raw_dac);
#endif



//  =============================ZYGO=====================================
#ifdef ZYGO
/**************************************************************************
 * Function:        void swX(unsigned char ctrl);
 * 
 * Description:     3 switches ctrl 
 * 
 * Input:           ctrl -- 0 as logic 0, 
 *                          1 as logic 1
 * 
 * Output:          null
 **************************************************************************/
extern void sw0(unsigned char ctrl);
extern void sw1(unsigned char ctrl);
extern void sw2(unsigned char ctrl);

/**************************************************************************
 * Function:        void swX(unsigned char ctrl);
 * 
 * Description:     3 switches ctrl 
 * 
 * Input:           null
 * 
 * Output:          logic at switches
 **************************************************************************/                            
#define rd_sw0()    //_RD3
#define rd_sw1()    //_RD5
#define rd_sw2()    //_RD4
#endif



//  ===========================INTERLOCK===================================
#ifdef INTER_LOCK
/**************************************************************************
 * Function:        void init_interlock_pin(void) 
 * 
 * Description:     initialize interlock pin as input
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define init_interlock_pin()    //_TRISG1=1

/**************************************************************************
 * Function:        int interlock_pin()
 * 
 * Description:     logic level of interlock pin
 * 
 * Input:           null
 * 
 * Output:          0: logic low
 *                  1: logic high
 **************************************************************************/   
#define interlock_pin()         //_RG1
#endif



//  ===========================SWTICH MODE=================================
#ifdef SWITCH_MODE
/**************************************************************************
 * Function:        void init_sm_dctrl(void)
 * 
 * Description:     initialize switch mode pin as output and open drain
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define init_sm_dctrl()         do{\
                                    /*_ODCD1 = 1; open drain*/\
                                    /*TRISD &= 0xFFFD; /*_RD1*/\
                                 }while(0)

/**************************************************************************
 * Function:        void set_sm_dctrl(int mode)
 * 
 * Description:     set the switch mode digital control pin 
 * 
 * Input:           CC_MODE: cc mode
 *                  NON_CC_MODE: non cc mode
 * 
 * Output:          null
 **************************************************************************/   
#define set_sm_dctrl(mode)      do{\
                                    /*if(mode == CC_MODE) _LATD1 = 1;*/\
                                    /*else _LATD1 = 0;*/\
                                 }while(0)
#endif



//  ===========================ABC CTRL====================================
#ifdef AUTO_BIAS_CTRL
/**************************************************************************
 * Function:        void init_abc_ctrl(void)
 * 
 * Description:     initialize automatic bisas ctrl do pins (open drain)
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define init_abc_ctrl()             do{\
                                        /*ODCD |= 0x000F;\
                                        TRISD &= 0xFFF0;*/ /*_RD0, 1, 2, 3*/\
                                     }while(0)

/**************************************************************************
 * Function:        void set_abc_status_auto(void)
 *                  void set_abc_status_manual(void)
 * 
 * Description:     set abc status pin as auto/manual
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define set_abc_status_auto()       //LATD &= 0xFFFE    //_RD0
#define set_abc_status_manual()     //LATD |= 0x0001

/**************************************************************************
 * Function:        void set_abc_bias0(void)
 *                  void clr_abc_bias0(void)
 *                  void set_abc_bias1(void)
 *                  void clr_abc_bias1(void)
 * 
 * Description:     set and clear bias 0 and 1 pin
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define set_abc_bias0()             //LATD |= 0x0002    //_RD1
#define clr_abc_bias0()             //LATD &= 0xFFFD
#define set_abc_bias1()             //LATD |= 0x0008    //_RD3
#define clr_abc_bias1()             //LATD &= 0xFFF7

/**************************************************************************
 * Function:        void set_abc_reset(void)
 *                  void set_abc_normal(void)
 * 
 * Description:     set and clear reset pin
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define set_abc_reset()             //LATD |= 0x0004    //_RD2
#define set_abc_normal()            //LATD &= 0xFFFB

/**************************************************************************
 * Constant:        ABC_VGAIN_DAC_INDEX
 *                  ABC_MBIAS_DAC_INDEX
 * 
 * Description:     AO PIN assignments
 **************************************************************************/   
#define ABC_VGAIN_DAC_INDEX         2
    #define ABC_VGAIN_MAX           5
#define ABC_MBIAS_DAC_INDEX         3
    #define ABC_MBIAS_MAX           5


/**********************************************************************
 * Function:        void adc_abc_output_power_convert(int adc_raw, float *ans)
 * 
 * Description:     convert adc raw data to output power value (mW)
 * 
 * Input:           adc_raw: raw adc value
 *                  *ans: outpt power value to be stored (mW)
 * 
 * Output:          none
 **********************************************************************/
#define adc_abc_output_power_convert(adc_raw, pans)     do{\
                                                            *pans = adc_raw;\
                                                         }while(0)


/**********************************************************************
 * Function:        void adc_abc_bias_mon_convert(int adc_raw, float *ans)
 * 
 * Description:     convert adc raw data to bias voltage (V)
 * 
 * Input:           adc_raw: raw adc value
 *                  *ans: outpt power value to be stored (V)
 * 
 * Output:          none
 **********************************************************************/
#define adc_abc_bias_mon_convert(adc_raw, pans)         do{\
                                                            *pans = adc_raw;\
                                                         }while(0)

#endif

#endif /*PIN_DEFINE_H_*/
