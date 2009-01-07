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
#define ERR_LED0_EN()               (TRISC &= 0xDFFF)   //_RC13
#define ERR_LED1_EN()               (TRISC &= 0x7FFF)   //_RC15
#define ERR_LED0(x)                 do{ \
                                        if((x)==0) LATC &= 0xDFFF; \
                                        else LATC |= 0x2000; \
                                     }while(0) 
#define ERR_LED1(x)                 do{ \
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
#define RS485_CTL_EN()              TRISF &= 0xFFBF;

/**************************************************************************
 * Function:        void RS485_TX_ON(void)
 * 
 * Description:     set the RS485 digital control pin to tx state
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define RS485_TX_ON()               _RF6=1

/**************************************************************************
 * Function:        void RS485_TX_OFF(void)
 * 
 * Description:     set the RS485 digital control pin to rx state
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define RS485_TX_OFF()              _RF6=0
#endif //UART_MOD



//  ===============================LCD=====================================
/**************************************************************************
 * Function:        void mwlike8_hw_init(void)
 * 
 * Description:     initialize IOs for lcd display
 *                    LCD_BACKLIGHT: output, [led back light on]
 *                    LCD_RES#: output, [do not select chip]
 *                    LCD_CS1#: output, [do not select chip]
 *                    ADDR0: output, [addressing mode]
 *                    IOR/IOW#: output, [read mode]
 *                    ENABLE CLK: output, [disable clk]
 *  
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/
#define mwlike8_hw_init()       do {\
                                 } while (0)



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
    unsigned char ctrl_buf = 11;        /*Port 0: Pin 14 (TEMP 0)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=2)
#define TEMP_PIN_2() {\
    unsigned char ctrl_buf = 15;        /*Port EXT: Pin 5 (TEMP 1)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=3)
#define TEMP_PIN_3() {\
    unsigned char ctrl_buf = 3;        /*Port EXT: Pin 9 (TEMP 2)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=4)
#define TEMP_PIN_4() {\
    unsigned char ctrl_buf = 4;         /*CASE_TEMP*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=5)
#define TEMP_PIN_5() {\
    unsigned char ctrl_buf = 11;\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TEC_T>=6)
#define TEMP_PIN_6() {\
    unsigned char ctrl_buf = 11;
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
    unsigned char ctrl_buf = 9;        /*Port 0: Pin 9 (PWR 0)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=2)
#define PWR_PIN_2() {\
    unsigned char ctrl_buf = 8;        /*Port 0: Pin 4 (PWR 1)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=3)
#define PWR_PIN_3() {\
    unsigned char ctrl_buf = 13;        /*Port EXT: Pin 2 (PWR 2)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=4)
#define PWR_PIN_4() {\
    unsigned char ctrl_buf = 12;        /*Port EXT: Pin 1 (PWR 3)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=5)
#define PWR_PIN_5() {\
    unsigned char ctrl_buf = 1;        /*Port EXT: Pin 4 (PWR 4)*/\
    ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_PWR>=6)
#define PWR_PIN_6() {\
    unsigned char ctrl_buf = 0;        /*Port EXT: Pin 10 (PWR 5)*/\
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
   unsigned char ctrl_buf = 10;        /*Port 0: Pin 12 (CUR 0)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=2)
#define CUR_PIN_2() {\
   unsigned char ctrl_buf = 14;        /*Port EXT: Pin 3 (CUR 1)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=3)
#define CUR_PIN_3() {\
   unsigned char ctrl_buf = 2;        /*Port EXT: Pin 7 (CUR 2)*/\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=4)
#define CUR_PIN_4() {\
   unsigned char ctrl_buf = 10;\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=5)
#define CUR_PIN_5() {\
   unsigned char ctrl_buf = 10;\
   ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);\
}
#endif
#if (TOTAL_LD_CUR>=6)
#define CUR_PIN_6() {\
   unsigned char ctrl_buf = 10;\
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
#define rd_sw0()    _RD3
#define rd_sw1()    _RD5
#define rd_sw2()    _RD4
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
#define init_interlock_pin()    TRISG |= 0x8000

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
#define interlock_pin()         _RG15
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
                                    TRISD &= 0xFF7F; /*_RD7*/\
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
                                    if(mode == CC_MODE) LATD |= 0x0080;\
                                    else LATD &= 0xFF7F;\
                                 }while(0)
#endif



//  ===========================ABC CTRL====================================
#ifdef AUTO_BIAS_CTRL
/**************************************************************************
 * Function:        void init_abc_ctrl(void)
 * 
 * Description:     initialize automatic bisas ctrl do pins
 * 
 * Input:           null
 * 
 * Output:          null
 **************************************************************************/   
#define init_abc_ctrl()             //do{\
                                        TRISD &= 0xFFF0; /*_RD0, 1, 2, 3*/\
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
    #define ABC_VGAIN2PERCENT(vgain)    (100.0f - ((float)vgain*100.0f)/ABC_VGAIN_MAX)
    #define ABC_PERCENT2VGAIN(percent)  (ABC_VGAIN_MAX - ((float)percent*ABC_VGAIN_MAX)/100.0f)
#define ABC_MBIAS_DAC_INDEX         3
    #define ABC_MBIAS_MAX           (15)
    #define ABC_MBIAS_MIN           (-15)
#endif

#endif /*PIN_DEFINE_H_*/
