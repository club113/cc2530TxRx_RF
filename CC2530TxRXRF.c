
#include "msgmanager.h"
#include <ioCC2530.h>
#include "hal_timer1_pwm.h"

/* RF observable control register value to output PA signal */
#define RFC_OBS_CTRL_PA_PD_INV        0x68

/* RF observable control register value to output LNA signal */
#define RFC_OBS_CTRL_LNAMIX_PD_INV    0x6A

/* RF observable control register value to output LNA signal 
* for CC2591 compression workaround. 
*/
#define RFC_OBS_CTRL_DEMOD_CCA        0x0D

/* OBSSELn register value to select RF observable 0 */
#define OBSSEL_OBS_CTRL0             0xFB

/* OBSSELn register value to select RF observable 1 */
#define OBSSEL_OBS_CTRL1             0xFC

#define uint  unsigned int
#define uchar unsigned char

#define PA_ENTX   P1_0 = 0
#define PA_ENRX   P1_0 = 1


#define LED1  P0_5
#define LED2  P0_6

uint8  rf_rx_buf[128];
uint8   serial_rxbuf[128];  
volatile uint8   serial_rxpos = 0;  
volatile uint8   serial_rxlen = 0;  
volatile uint8  is_serial_receive;//flag
void rf_receive_isr();
//函数声明
void Delay(uint);		//延时函数
void rf_init();
void uart0_init();  
void timer1_init();  
void rf_send(u8 *pbuf,int len);
 
/****************************
//延时
*****************************/
void Delay(uint n)
{
  uint i;
  for(i = 0;i<n;i++);
  for(i = 0;i<n;i++);
  for(i = 0;i<n;i++);
  for(i = 0;i<n;i++);
  for(i = 0;i<n;i++);
}

/****************************************************************
串口发送字符串函数			
****************************************************************/
void UartTX_Send_String(unsigned char *Data,int len)
{
  int j;
  for(j=0;j<len;j++)
  {
    U0DBUF = *Data++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
}
/****************************
//初始化IO口程序
*****************************/
void InitIO(void)
{
  
  /* Set UART1 I/O to alternate 2 location on P1 pins. */       
  PERCFG |= 0x02;  /* U1CFG */                                  
  /* Set UART0 I/O to alternate 2 location on P1 pins. */       
  PERCFG |= 0x01;  /* U1CFG */                                  
  
  P2DIR  |= 0x80;
  
  
  P0SEL |= ( 0x04 | 0x08 | 0x10 | 0x20 | 0x40); /* P0.2:6 */          
  P0DIR |= ( 0x04 | 0x08 | 0x10 | 0x20 | 0x40); /* P0.2:6 */           
  
  
}

/*****************************************
设定系统主时钟函数
*****************************************/
void  SET_MAIN_CLOCK(unsigned char source)
{
  if(source)
  {
    CLKCONCMD |= 0x40;          /*RC*/
    while(!(CLKCONSTA &0X40));  /*待稳*/
  }
  else
  {
    CLKCONCMD = 0;              /*HSE*/
    while((CLKCONSTA &0X40));  /*待稳*/
  }
}

/*****************************************
LED Indicator Pin init
*****************************************/
void LED_Init(void)
{
   /* P0_5 -> LED1 */
  P0DIR |= 0x01<<5;
  P0_5 = 0;
  /* P0_6 -> LED2 */
  P0DIR |= 0x01<<6;
  P0_6 = 0;

}
//#define DEBUG
#ifdef DEBUG
u8 n = 0;
#endif
/***************************
//主函数
***************************/
void main(void)
{
  /* set system clk = 32Mhz */
  SET_MAIN_CLOCK(0);
  
  rf_init();
  uart0_init();  
  timer1_init(); 
  LED_Init();
  InitMsgQue();
  
  EA = 1;
  for(;;)
  {
//    if(is_serial_receive)
//    {
//      rf_send(serial_rxbuf, serial_rxlen);
//      
//      
//    }
    if(IsListEmpty(&ListHead) == FALSE)
    {
#ifdef DEBUG
      if( n >4)
      {
#endif      
        //     FreeMem((u8*)RemoveTailList(&ListHead));
        
        u8 size = 0;
        u8 * pmsg = GetMsgMemSize(&ListHead,&size);
       if( *pmsg++ == 'T')
       {
         rf_send(pmsg, size - 1);
       }
       else
       {
         UartTX_Send_String(pmsg,size - 1);
       }
        FreeMem(size);
  
        
#ifdef DEBUG
      }
#endif   
    }
  }
}


#pragma vector =  P0INT_VECTOR
__interrupt void P0INT__ISR(void)
{
  static uint8 cn = 0;
  P0IF  = 0;
  P0IFG = 0x00;//clear interrupt flag
  P0IE  = 0X00;//close P0 interrupt
  
  /* start half period=1.06ms delay */
  T1CNTH = 0;
  T1CNTL = 0;
  T1IE = 1;//enable TIM1 Interrupt
  
  cn ++ ;
  
}

/******************************************************************************
初始化 RF 功能
*******************************************************************************/
void rf_init()  
{  
  FRMFILT0  =0x0C;                //静止接收过滤，即接收所有数据包     
  TXPOWER   =0xF5;                //发射功率为4.5dBm  
  FREQCTRL  =0x06;                //选择通道2400MHZ
  
  CCACTRL0  =0xF8;                //推荐值 smartRF软件生成  
  FSCAL1 =   0x00;                  
  TXFILTCFG =0x09;  
  AGCCTRL1 = 0x15;  
  AGCCTRL2 = 0xFE;        
  TXFILTCFG =0x09;                  
  
  RFIRQM0 |=(1<<6);               //使能RF数据包接收中断  
  IEN2 |=(1<<0);                  //使能RF中断  
  
  RFST =0xED;                     //清除RF接收缓冲区 ISFLUSHRX  
  RFST =0xE3;                     // RF接收使能 ISRXON 
  
  
  //Set PA/LNA HGM control P0_7                  
  P0DIR |= 0x80;
  P0_7 = 1;
//  /* P1_1 -> PAEN */
//  P1DIR |= 0x02;
//  P1_1 = 0;
//  /* P1_0 -> EN (LNA control) */
//  P1DIR |= 0x01;
//  P1_0 = 1;

  
    /* P1_1 -> PAEN */
    RFC_OBS_CTRL0 = RFC_OBS_CTRL_PA_PD_INV;
    OBSSEL1       = OBSSEL_OBS_CTRL0;
    
    /* P1_0 -> EN (LNA control) */
    RFC_OBS_CTRL1 = RFC_OBS_CTRL_LNAMIX_PD_INV;
    OBSSEL0       = OBSSEL_OBS_CTRL1;  
}  

/***************************
RF 发送数据
***************************/
void rf_send(u8* pbuf,int len)  
{  
  LED1 = 1;
  RFST =0xE3;                     // RF接收使能 ISRXON  
  // 等待发送状态不活跃并且没有接收到SFD  
  while( FSMSTAT1&((1<<1)|(1<<5)));  
//  PA_ENTX;
    
  RFIRQM0 &=~(1<<6);              //禁止接收数据包中断  
  IEN2 &=~(1<<0);                 //清除RF全局中断  
  
  RFST =0xEE;                     //清除发送缓冲区 ISFLUSHTX  
  RFIRQF1 =~(1<<1);               //清除发送完成标志  
  
  // 填充缓冲区填充过程需要增加2字节，CRC校验自动填充  
  RFD = len+2;         
  for(int i=0; i< len; i++)  
  {  
    RFD=*pbuf++;  
  }  
  
  RFST =0xE9;                     //发送数据包 ISTXON  
  while(!(RFIRQF1&(1<<1)));     //等待发送完成  
  RFIRQF1 =~(1<<1);               //清除发送完成标志位  
//  PA_ENRX;
  RFIRQM0 |=(1<<6);               // RX接收中断  
  IEN2 |=(1<<0);  
  LED1 = 0;
} 

/***************************
RF 收到数据处理中断
***************************/
#pragma vector=RF_VECTOR  
__interrupt void rf_isr(void)  
{  
  EA =0;  
  LED2 = 1;
  // 接收到一个完整的数据包  
  if(RFIRQF0&(1<<6))  
  {  
    rf_receive_isr();                          //调用接收中断处理函数  
    S1CON =0;                                   //清除RF中断标志  
    RFIRQF0 &=~(1<<6);                        //清除RF接收完成数据包中断  
  }
  LED2 = 0;  
  EA =1;  
}  

/***************************
RF 接收处理，读出数据
***************************/
void rf_receive_isr()  
{  
  int rf_rx_len=0;  
  
  rf_rx_len = RFD-2;                       //长度去除两字节附加结果  
  rf_rx_len &=0x7F;  
  for(int i=0; i< rf_rx_len; i++)  
  {  
    rf_rx_buf[i]= RFD;                      //连续读取接收缓冲区内容  
  }  
  
//  rf_rx_buf[rf_rx_len++] = RFD-73;                           //读取RSSI结果  
//  rf_rx_buf[rf_rx_len++] = RFD;                              //读取CRC校验结果 BIT7  
  volatile unsigned char rssi = RFD;
  volatile unsigned char crc_ok = RFD;
  
  RFST =0xED;                               //清除接收缓冲区  
    if( crc_ok&0x80) // 校验CRC
  {  
   // UartTX_Send_String(rf_rx_buf, rf_rx_len+1);   //串口发送   
    u8* pdata = NewMem(rf_rx_len + 1);
    *pdata++ = 'R'; 
    memcopy(rf_rx_buf, pdata,rf_rx_len);
    //    printf("[%d]",rssi);  
  }  

}  

/***************************
串口初始化
***************************/
void uart0_init()  
{  
  PERCFG =0x00;             // UART0选择位置0 TX@P0.3 RX@P0.2  
  P0SEL|=0x0C;             // P0.3 P0.2选择外设功能  
  U0CSR|=0xC0;             // UART模式接收器使能  
  U0GCR|=11;               //查表获得 U0GCR和 U0BAUD  
  U0BAUD =216;              // 115200  
  UTX0IF =1;  
  
  URX0IE =1;                   //使能接收中断 IEN0@BIT2  
} 

/************************************
定时器 1 初始化 用作串口接收处理延时
************************************/
void timer1_init()  
{  
  T1CTL=0x0C;               // @DIV分频系数 128 @MODE暂停运行  
  T1CCTL0 =0x44;            // @IM通道0中断使能 @MODE比较匹配模式  
  T1STAT =0x00;              //清除所有中断标志  
  
  T1IE =1;                // IEN1@BIT1使能定时器1中断  
  
  T1CC0L =250;               //溢出周期为2ms  
  T1CC0H =0;  
}  

void timer1_disbale()  
{  
  T1CTL&=~(1<<1);       //恢复为停止模式  
}  

void timer1_enable()  
{  
  T1CTL|=(1<<1);      //改变模式为比较匹配模式 MODE = 0x10;  
  T1STAT =0x00;              //清除中断标志位   
  T1CNTH =0;              //重新开始计数  
  T1CNTL =0;  
}  

#pragma vector=URX0_VECTOR  
__interrupt void UART0_ISR(void)  
{  
  URX0IF =0;                                  //清除接收中断标志  
  serial_rxbuf[serial_rxpos]= U0DBUF;    //填充缓冲区  
  serial_rxpos++;  
  serial_rxlen++;  
  
  timer1_enable();                             //定时器重新开始计数  
}  

#pragma vector=T1_VECTOR  
__interrupt void Timer1_ISR(void)  
{  
  T1STAT &=~(1<<0);                         //清除定时器T1通道0中断标志  
  
  if(serial_rxlen)
  {//is_serial_receive =1;                       //串口数据到达  
    
    u8* pdata = NewMem(serial_rxlen + 1);
    *pdata++ = 'T'; 
    memcopy(serial_rxbuf, pdata,serial_rxlen);

    
    serial_rxpos = 0;  
    serial_rxlen = 0;  
#ifdef DEBUG
    n ++;
#endif
  }
  //  is_serial_receive = 0;
  
  timer1_disbale();  
}  