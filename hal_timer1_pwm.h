/*******************************************************************************

File_name:      hal_timer1_pwm.h
Description:    the header file of hal_timer1_pwm_c .

*******************************************************************************/
#ifndef     __HAL_TIMER1_PWM_H__
#define     __HAL_TIMER1_PWM_H__

/*==============================================================================
@ Include files
*/
#include "hal_types.h"
/*==============================================================================
@ Typedefs
*/
#define ENABLE_TIM3_CH1_PWM   T3CTL |= 0x10;T3CCTL1 = 0x24////start tim3
#define DISABLE_TIM3_CH1_PWM  T3CTL  &=0xEF;T3CCTL1 = 0x3C////clear bit4
#define IR_TX_START           T1IE = 1 //complete IR transimit
/*==============================================================================
@ Constants and defines
*/
uint16 hal_pwm_config(uint16 frquency,uint16 duty_ratio);
void channle_choice(unsigned char bit_map);

unsigned char CompleteTransmit(void);





#endif
/*@*****************************end of file**********************************@*/


