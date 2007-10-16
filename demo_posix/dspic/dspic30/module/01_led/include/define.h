/************************************************************************************************
 * File: 			define.h
 * Description:		Header file for kernel-driver interface
 ***********************************************************************************************/

 
#ifndef DEFINE_H_
#define DEFINE_H_


/********************************************************************************
 * Configurations for FreeRTOS and its POSIX
 ********************************************************************************/

// Platform header file**********************************************************
#include <p30f5011.h>

// Define User Idle Task Here **************************************************
#define idle_process                UserIdleTask

// Enable FreeRTOS Scheduler ***************************************************
#define FREERTOS_SCHE_ENABLE        1           // if disabled, use coroutine_st
#if(FREERTOS_SCHE_ENABLE == 1)
#   define start_process()          while(1){
#   define end_process()            }
    //==========Enable Coroutine Thread Scheduler in FREERTOS Scheduler========
    #define CRTHREAD_ENABLE             1
    #if(CRTHREAD_ENABLE == 1)
    #define MAX_CRTHREAD                10
    #endif
#else
#   include <coroutine_st.h>
#   define start_process()          scrBegin
#   define end_process()            scrFinish((void*)0)
#endif

// System Settings **************************************************************
#define XTAL_FREQ_HZ                7372800
#define PLL                         16
#define SYSTEM_CLK_HZ               ((unsigned long)(XTAL_FREQ_HZ*PLL/4))   // 29,491,200
#define SYSTEM_VOLT                 ((float)5.0)                            // Can be used for ADC reading calculation

// UART Module ******************************************************************
#define UART_MOD                    1           // uart enable
#if (UART_MOD>0)
//  ============================Device Handler=================================== 
#define BASE_UART                   0           // base uart no.
#define NO_OF_UART                  2
#define UARTA                       "0"         // Using A, B to avoid conflicts in 
#define UARTB                       "1"         // definitions with p30f5011.h
//  ============================Device Config==================================== 
#define UARTA_RS485                 0           // Set to 1 if UARTA used as RS485
#define UARTB_RS485                 1           // Set to 1 if UARTB used as RS485
#define AUTO_BAUD_DECT              0           // Autobaud rate detect enable
#define UART_DEFAULT_BAUDRATE       19200   
#define UART_DEFAULT_BRG            ( (unsigned int) \
                                          ( (unsigned long) SYSTEM_CLK_HZ \
                                        /   ( (unsigned long) 16*UART_DEFAULT_BAUDRATE) ) \
                                        - 1 \
                                     )
#define RS485_CTL_EN()              _TRISF6=0   //enable port F pin 6
#define RS485_TX_ON()               _RF6=1
#define RS485_TX_OFF()              _RF6=0
//  ============================Buffer Size====================================== 
#define MAX_UART_RX_BUF             30
#define MAX_UART_TX_BUF             30
#endif //UART_MOD

// I2C Module ******************************************************************
#define I2C_MOD                     0
#if (I2C_MOD>0)
//  ==========================I2C Speed control================================== 
//  I2C: Baudrate options for I2C
//   +-- I2CBRG = (Fcy/Fscl - Fcy/1111111)-1
//   +-- Fcy = 30MHz, Fscl = 100kHz, 400kHz, or 1MHz
#define I2C_BAUDRATE_HZ             400000      // I2C BAUD RATE IN HZ, 100kHz-400kHz
#define I2C_BRG                     ( (unsigned int) \
                                          ( (unsigned long)SYSTEM_CLK_HZ/I2C_BAUDRATE_HZ) \
                                        - ( (unsigned long)SYSTEM_CLK_HZ/1111111) \
                                        - 1 \
                                     )
#endif //I2C_MOD

// I2C DAC Module ***************************************************************
#define I2C_DAC_MOD                 0
#if (I2C_DAC_MOD>0 & I2C_MOD==0)
    #error "I2C_MOD is off. Please set I2C_MOD = 1 in <define.h>"
#endif
#if (I2C_DAC_MOD>0)
//  ============================Devices Handler================================== 
#define BASE_I2C_DAC                2           // base i2c dac no.
#define I2C_DAC                      "2"        // i2c device id for DAC
//  ======================I2C control for DAC==================================== 
//  [Factory Preset] [A1] [A0] [R/^W]
//     1 0 0 1 1       1    1     0        => 0x9E
#define I2C_DAC_ADDR                0x9E        // DAC address for Writing
//  Asynchrous update of DAC (L1,L0) at Selected channel, no power-down
//  [A3] [A2] [L1] [L0] [x] [Sel1] [Sel2] [PD0]
//   0     0    0    1   x     Channel      0    
#define DAC_CHA                     0x10
#define DAC_CHB                     0x12
#define DAC_CHC                     0x14
#define DAC_CHD                     0x1E
#endif //I2C_DAC_MOD

// Non-volatile Memory *********************************************************
#define NVM_MOD                     0
#if (NVM_MOD>0)
//  ============================Device Handler=================================== 
#define BASE_EEPROM                 3           // base EEPROM no.
#define EEPROM                      "3"         // device id for EEPROM
//  ============================Device Config==================================== 
    #define NVM_SRC_ON_CHIP         0
    #define NVM_SRC_FLASH           (NVM_SRC_ON_CHIP+1)
    #define NVM_SRC_I2C             (NVM_SRC_FLASH+1)    
#define NVM_SRC                      NVM_SRC_ON_CHIP   //Select which EEPROM is used
//  ==========================I2C EEPROM Config==================================
// [Factory Preset] [A2] [A1] [A0] [R/W#]
//    1 0 1 0         0   0    0        => 0xA0
#if(NVM_SRC==NVM_SRC_I2C)
#if (I2C_DAC_MOD>0 & I2C_MOD==0)
    #error "I2C_MOD is off. Please set I2C_MOD = 1 in <define.h>"
#endif
#define I2C_EEPROM_ADDR             0xA0        // EEPROM address for Writing
#define I2C_EEPROM_SIZE             0x8000      // EEPROM Size (in byte)
#define I2C_EEPROM_PAGE_SIZE        64          // EEPROM Page size for writing (in byte)
#endif //NVM_SRC_I2C
//  =========================Flash EEPROM Config================================= 
#if(NVM_SRC==NVM_SRC_FLASH)
#define FLASH_EEPROM_BYTE_PER_WORD  2
#define FLASH_EEPROM_WORD_PER_ROW   32
#define FLASH_EEPROM_ROW_PER_PAGE   1
#define FLASH_EEPROM_SIZE           (FLASH_EEPROM_BYTE_PER_WORD* \
                                      FLASH_EEPROM_WORD_PER_ROW* \
                                      FLASH_EEPROM_ROW_PER_PAGE)
#endif //NVM_SRC_FLASH
//  =======================On-chip EEPROM Config================================= 
#if(NVM_SRC==NVM_SRC_ON_CHIP)
#ifndef MPLAB_DSPIC30_PORT
    #error "Target chip has no on-chip EEPROM"
#endif
#define EEPROM_SIZE                 1024        // EEPROM Size (in byte)
#endif //NVM_SRC_ON_CHIP
#endif //NVM_MOD

// ADC Module ***************************************************************
#define ADC_MOD                     0           // adc enable
#if (ADC_MOD>0)
//  ============================Devices Handler================================== 
#define BASE_ADC                    4           // base adc no.
#define ADC                         "4"         // adc device id
//  ============================Device Config==================================== 
#define ADC_MAX_CH                  16          // maximum adc channels
#define ADC_ACQ_TIME                16          // acquisation time i.e. SAMC<4:0>, [#of TAD]
#define ADC_TAD_MIN                 334         // in ns, see Electrical Specification on Datasheet
#define ADC_SAMP_RATE_HZ            50000       // adc sampling rate in Hz, <200kHz
#define ADC_TAD_REQ                 ( (int) \
                                         ( (double)1e9 \
                                       /   ( (14.0 + ADC_ACQ_TIME)*(double)ADC_SAMP_RATE_HZ ) ) \
                                       + 1)         
#define ADC_ADCS                    ( (int) \
                                         ( (double)(2*ADC_TAD_REQ*1e-9*SYSTEM_CLK_HZ)+0.5) \
                                       - 1)     // ADCS<5:0> Setting for sampling rate
//  NOTE: make sure ADC_TAD_ACT >= ADC_TAD_MIN
#define ADC_TAD_ACT                 ( ( (double)1e9*0.5(ADC_ADCS+1))/SYSTEM_CLK_HZ)    //Actual TAD 
#endif //ADC_MOD

// PWM Module *******************************************************************
#define PWM_MOD                     0           // pwm enable
#if (PWM_MOD>0)
//  ============================Device Handler=================================== 
#define BASE_PWM                    5           // base pwm no.
#define PWM                         "5"         // pwm device id
#endif //PWM_MOD

// Keyboard Module ***************************************************************
#define KB_MOD                      0           // keyboard enable
#if (KB_MOD>0)
//  ============================Device Handler=================================== 
#define BASE_KB                     6           // base keyboard no.
#define KB                          "6"         // keyboard device id
//  ============================Device Config==================================== 
#define MAX_GPIO_BUF                10          // max number of keys stored
//  ============================Key Map for PUSH KEY============================= 
#define BASE_PUSH_KEY                   0       // starting key id for push keys
#define TOTAL_PUSH_KEY                  1       // Total number of push keys
#define PUSH_KEY0                       BASE_PUSH_KEY
    #define EN_PKEY0                        //_TRISG14
    #define IO_PKEY0                        //_RG14
#define PUSH_KEY1                       PUSH_KEY0 + 1
//  ============================Key Map for ROTARY KEY===========================
#define BASE_ROTARY_KEY                 100     // starting key id for rotary keys
#define TOTAL_ROTARY_KEY                1       // Total number of rotary key, each key has 2 id (up and down)
#define ROTARY_KEY0_UP                  BASE_ROTARY_KEY
    #define EN_RKEY0_UP                     //_TRISG6
    #define IO_RKEY0_UP                     //_RG6
    #define CN_RKEY0_UP                     //_CN8IE
#define ROTARY_KEY0_DN                  ROTARY_KEY0_UP + 1
    #define EN_RKEY0_DN                     //_TRISG7
    #define IO_RKEY0_DN                     //_RG7
    #define CN_RKEY0_DN                     //_CN9IE
#define ROTARY_KEY1_UP                  ROTARY_KEY0_DN + 1
#define ROTARY_KEY1_DN                  ROTARY_KEY1_UP + 1
//  ============================MARCOS===========================================
//  KEYCONFIG()
//  Initialise the key here, EN = 1 (pin is input), CN = 1 (enable change notification)
#define KEYCONFIG()                 do{\
                                        EN_PKEY0 = 1; Nop(); \
                                        \
                                        EN_RKEY0_UP = 1; CN_RKEY0_UP = 1; \
                                        EN_RKEY0_DN = 1; CN_RKEY0_DN = 1; \
                                        _CNIF = 0; \
                                        _CNIE = 1; \
                                     }while(0)
//  KEY_PRESS(id, ans)                                     
//  id is the id in the key map, 
//  ans = 1 if the key has pressed
#define KEY_PRESS(id, ans)          do{\
                                         switch(id){\
                                             case PUSH_KEY0: ans = (IO_PKEY0 == 0)? 1 : 0;  break;\
                                         }\
                                     }while(0)
//  RKEY_STATE(id, ans)                                    
//  id is the id in the key map, 
//  ans = 1 if the key at high state
#define RKEY_STATE(id, ans)         do{\
                                         switch(id){\
                                            case ROTARY_KEY0_UP: ans = (IO_RKEY0_UP == 1)? 1 : 0; break; \
                                            case ROTARY_KEY0_DN: ans = (IO_RKEY0_DN == 1)? 1 : 0; break; \
                                         }\
                                     }while(0)                                         
#endif //KB_MOD

// Pin usage setting ************************************************************
#define ERR_LED0_EN()               (TRISC &= 0x7FFF)   //_RC15
#define ERR_LED1_EN()               (TRISC &= 0xDFFF)   //_RC13 
#define ERR_LED0(x)                 do{ \
                                        if((x)==0) LATC &= 0x7FFF; \
                                        else LATC |= 0x8000; \
                                     }while(0) 
#define ERR_LED1(x)                 do{ \
                                        if((x)==0) LATC &= 0xDFFF; \
                                        else LATC |= 0x2000; \
                                     }while(0)  

#endif /* DEFINE_H_ */
