/*******************************************************************************
Copyright:
File_name:       hal_timer1_pwm.c
Version:	     0.0
Revised:        $Date:2016-3-25 ; $
Description:    hal of zigbee stack
Notes:          This version targets the CC2530
Editor:		    Mr.****

*******************************************************************************/

//#define NEW  //新的调制记录方式
/*==============================================================================
@ Include files
*/

#include "hal_timer1_pwm.h"
#include "iocc2530.h"

/*==============================================================================
@ Global variable
*/
#ifndef NEW
volatile uint8 remoteCodeBuff[380] =
{
    0x84, 0xD8, 0x82, 0xA6, 0x2E, 0x6A, 0x2E, 0x22, 0x2C, 0x23, 0x2C, 0x6A, 0x2E, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x6A, 0x2E, 0x23, 0x2C,
    0x6A, 0x2E, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x6A, 0x2E, 0x22, 0x2E, 0x22, 0x2C, 0x23,
    0x2C, 0x23, 0x2C, 0x23, 0x2C, 0x23, 0x2C, 0x6A, 0x2E, 0x22, 0x2C, 0x6A, 0x2E, 0x22, 0x2C, 0x23, 0x2C, 0x6A, 0x2E, 0x22, 0x2C, 0x8A, 0xAB, 0x2E, 0x22, 0x2D, 0x22,
    0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2D, 0x22, 0x2D, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x6A, 0x2E, 0x23, 0x2C,
    0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x22, 0x2C, 0x23, 0x2C, 0x22, 0x2C, 0x6A,
    0x2E, 0x6A, 0x2E, 0x6A, 0x2E, 0x6A, 0x2E, 0x6A, 0x2E, 0x6A, 0x2E
};

#else
uint16 remoteCodeBuff[380] =
{
    0x157, 0xA8, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x3C,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A,
    0x13,  0x1A, 0x2F5,0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13,
    0x1A,  0x13, 0x1A, 0x13, 0x19, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13,
    0x1A,  0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x5ED,0x158,0xA8, 0x1A, 0x3C,
    0x1A,  0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13,
    0x1A,  0x13, 0x19, 0x3C, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13,
    0x1A,  0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C,
    0x1A,  0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C,
    0x1A,  0x3C, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x3C, 0x1A, 0x13, 0x1A, 0x2F5, 0x1A,
    0x13,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A,
    0x13,  0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13,
    0x1A,  0x13, 0x1A, 0x13 ,0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13 ,0x1A, 0x13, 0x1A, 0x13,
    0x1A,  0x13, 0x1A, 0x13, 0x1A, 0x13, 0x1A, 0x13 ,0x1A ,0x13 ,0x1A, 0x13, 0x1A, 0x3C, 0x1A,
    0x13,  0x1A, 0x3C, 0x1A, 0x3C, 0x1A, 0xED2
};

#endif
/*==============================================================================
@ All functions  as follow
*/
static void PWM_Pin_config(void);
static void timer3_pwmconfig(unsigned char frequency, unsigned duty_ratio);

/*******************************************************************************
Function:  hal_pwm_config
Description:

Input:        frequency:  PWM basic frequecy
duty_ratio:   1 - 50 means 1%- 50%
Output:       None
Return:       0: config faild
1: config
Editor:       Mr.Kon
Others:	      .eg data
*******************************************************************************/
uint16 hal_pwm_config(uint16 frequency,uint16 duty_ratio)
{

    if(frequency < 20 || frequency > 56)
    {
        return 0;
    }
    if(duty_ratio < 1 || duty_ratio > 50)
    {
        return 0;
    }

    /* config IO port */
    PWM_Pin_config();
    timer3_pwmconfig(frequency,duty_ratio);

#ifdef NEW
    //ENABLE_TIM3_CH1_PWM;
    DISABLE_TIM3_CH1_PWM ;
#endif
    /* config tim1 */
    /*  DIV[3:2]  : 00    div1
    : 01    div8
    : 10    div32
    : 11    div128
    MODE[1:0]     : 00    stop count
    ：01    run from 0x0000 to 0xFFFF
    : 10    run from 0x0000 to T1CC0
    : 11    run from 0x0000 to 0xFFFF then  from 0xFFFF to 0x0000
    */
#ifdef NEW
    IRCTL = 0x01;
#endif

    /* compare mode */
    T1CCTL0 |= 0x04;

    /* T1CC0 = 45*/
    T1CC0H = 0x00;
    T1CC0L = 0x02;

    /* T1CCTL1 */

    T1CCTL1 = 0x24;//2C  open CH1 interrupt
    /* config tim1 */

    /* T1CC1 = 20 */
    T1CC1H = 0x00;
    T1CC1L = 0x01;

    T1OVFIM = 0; //溢出中断开启

#ifdef NEW
    T1IE = 1;//设定timer1中断开启
    /* config tim1 */
    T1CTL = 0x02;//div = 0
#else
    T1CTL = 0x0a;//div = 32  1 MHZ
#endif
    //P2SEL |=0x20;


    return 1;
}

/*******************************************************************************
Function:  hal_pwm_config
Description:

Input:        bit_map[0:5]: bit0 - bit5 control channel_0 - channel_5
Output:       None
Return:       None
Editor:       Mr.Kon
Others:	      .eg data
*******************************************************************************/
void channle_choice(unsigned char bit_map)
{
   IR_TX_START;
}

/*******************************************************************************
Function:  imer3_pwmconfig
Description:

Input:        frquency:  PWM basic frequecy
duty_ratio:   1 - 50 means 1%- 50%
Output:       None
Return:       None
Editor:       Mr.Kon
Others:	      .eg data
*******************************************************************************/
void timer3_pwmconfig(unsigned char frequency, unsigned duty_ratio)
{
    /*
    BIT[7:5]   DIV  = 4  //32MHZ/8 = 4000K
    BIT[4]     START
    BIT[3]     OVFIM
    BIT[2]     CLR
    BIT[1:0]   MODE
    */
    if(frequency < 38)
    {

        /* DIV  = 8  //32MHZ/8 = 4000K */
        T3CTL = 0x62;

        /* compare value */
        T3CC0 = 4000 / frequency - 1;// 4000k / <38k
    }
    else
    {
        /* DIV  = 4  //32MHZ/4 = 8000K */
        T3CTL = 0x42;

        /* compare value */
        T3CC0 = 8000/ frequency ;// 8000k / >37k
    }

    /*
    CMP[2:0] = 4
    MODE   = 1
    */
    T3CCTL1 = 0x24;

    /* set duty */
    T3CC1 = T3CC0 * duty_ratio / 100;//pulse duty = T3CC1 / T3CC0

}
/*******************************************************************************
Function:     PWM_Pin_config
Description:  P1
Input:    None
Return:   None
Editor:   Mr.Kon
Others:	 .eg date
*******************************************************************************/
static void PWM_Pin_config(void)
{
#ifndef NEW
    /* output GPIO config P1.7 */
    P1DIR |= 0x80;
    P1SEL |= 0x80;
    P2SEL |= 0x20;
    PERCFG |= 0x20;
#else
    P0DIR |= 0x08;
    P0SEL |= 0x08;
    P2DIR |= 0x80;//P0定时器0-1通道优先
#endif
}

/*******************************************************************************
Function:     CompleteTransmit
Description:  interrupt service
Input:    None
Return:   None
Editor:   Mr.Kon
Others:	 .eg date
*******************************************************************************/
#ifndef NEW
unsigned char CompleteTransmit(void)
{
    static uint16 trans_cnt = 0;
    static uint16 trans_cnt_next = 1;
    static uint8  out_put_status = 1;
    uint16 period = 0;


    /* switch output status */
    if(out_put_status )
    {
        /* force_clear output  */
        DISABLE_TIM3_CH1_PWM;
        out_put_status = 0;
    }
    else
    {
        ENABLE_TIM3_CH1_PWM;
        out_put_status = 1;
    }

    if(remoteCodeBuff[trans_cnt] & 0x80 && remoteCodeBuff[trans_cnt_next] & 0x80)
    {
        /* low pulse */
        period = remoteCodeBuff[trans_cnt] & 0x7F;
        period <<= 7;
        period |=  (remoteCodeBuff[trans_cnt_next] & 0x7F);
        period = period* 15 ;//turn to microsecond
        trans_cnt += 2;
        trans_cnt_next += 2;

    }
    else
    {
        period = remoteCodeBuff[trans_cnt];
        period = period * 15 ;
        trans_cnt ++;
        trans_cnt_next ++;
    }
    T1CC0L = period;
    T1CC0H = period >> 8;

    /* T = T_low + T_high */

    //   period  += h_pulse ;

    //    T1CC1H = h_pulse >> 8; // T1CC1  = Hig Pulse time
    //    T1CC1L = h_pulse;

    //T1CC0 - T1CC1 = Low Pulse time


    if( remoteCodeBuff[trans_cnt_next] == 0 ||
       remoteCodeBuff[trans_cnt] == 0)
    {
        trans_cnt = 0;
        trans_cnt_next = 1;
        DISABLE_TIM3_CH1_PWM;
        out_put_status = 1;
        T1IE = 0;//close interrupt
    }
    return 0;
}

#else
unsigned char CompleteTransmit(void)
{
    static uint16 trans_cnt = 0;
    static uint16 trans_cnt_next = 1;
    uint16 period = 0;



    T1CC1H = remoteCodeBuff[trans_cnt] >> 8; // T1CC1  = Hig Pulse time
    T1CC1L = remoteCodeBuff[trans_cnt] - 1;



    /* T = T_low + T_high */

    period = remoteCodeBuff[trans_cnt_next] + remoteCodeBuff[trans_cnt] -1;


    T1CC0H = period >> 8;
    T1CC0L = period ;   //T1CC0 - T1CC1 = Low Pulse time

    trans_cnt += 2;
    trans_cnt_next += 2;

    if( remoteCodeBuff[trans_cnt_next] == 0 ||
       remoteCodeBuff[trans_cnt] == 0)
    {
        trans_cnt = 0;
        trans_cnt_next = 1;
        DISABLE_TIM3_CH1_PWM;
    }
    return 0;
}
#endif
/*@*****************************end of file**********************************@*/
