
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
//��������
void Delay(uint);		//��ʱ����
void rf_init();
void uart0_init();  
void timer1_init();  
void rf_send(u8 *pbuf,int len);
 
/****************************
//��ʱ
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
���ڷ����ַ�������			
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
//��ʼ��IO�ڳ���
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
�趨ϵͳ��ʱ�Ӻ���
*****************************************/
void  SET_MAIN_CLOCK(unsigned char source)
{
  if(source)
  {
    CLKCONCMD |= 0x40;          /*RC*/
    while(!(CLKCONSTA &0X40));  /*����*/
  }
  else
  {
    CLKCONCMD = 0;              /*HSE*/
    while((CLKCONSTA &0X40));  /*����*/
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
//������
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
��ʼ�� RF ����
*******************************************************************************/
void rf_init()  
{  
  FRMFILT0  =0x0C;                //��ֹ���չ��ˣ��������������ݰ�     
  TXPOWER   =0xF5;                //���书��Ϊ4.5dBm  
  FREQCTRL  =0x06;                //ѡ��ͨ��2400MHZ
  
  CCACTRL0  =0xF8;                //�Ƽ�ֵ smartRF�������  
  FSCAL1 =   0x00;                  
  TXFILTCFG =0x09;  
  AGCCTRL1 = 0x15;  
  AGCCTRL2 = 0xFE;        
  TXFILTCFG =0x09;                  
  
  RFIRQM0 |=(1<<6);               //ʹ��RF���ݰ������ж�  
  IEN2 |=(1<<0);                  //ʹ��RF�ж�  
  
  RFST =0xED;                     //���RF���ջ����� ISFLUSHRX  
  RFST =0xE3;                     // RF����ʹ�� ISRXON 
  
  
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
RF ��������
***************************/
void rf_send(u8* pbuf,int len)  
{  
  LED1 = 1;
  RFST =0xE3;                     // RF����ʹ�� ISRXON  
  // �ȴ�����״̬����Ծ����û�н��յ�SFD  
  while( FSMSTAT1&((1<<1)|(1<<5)));  
//  PA_ENTX;
    
  RFIRQM0 &=~(1<<6);              //��ֹ�������ݰ��ж�  
  IEN2 &=~(1<<0);                 //���RFȫ���ж�  
  
  RFST =0xEE;                     //������ͻ����� ISFLUSHTX  
  RFIRQF1 =~(1<<1);               //���������ɱ�־  
  
  // ��仺������������Ҫ����2�ֽڣ�CRCУ���Զ����  
  RFD = len+2;         
  for(int i=0; i< len; i++)  
  {  
    RFD=*pbuf++;  
  }  
  
  RFST =0xE9;                     //�������ݰ� ISTXON  
  while(!(RFIRQF1&(1<<1)));     //�ȴ��������  
  RFIRQF1 =~(1<<1);               //���������ɱ�־λ  
//  PA_ENRX;
  RFIRQM0 |=(1<<6);               // RX�����ж�  
  IEN2 |=(1<<0);  
  LED1 = 0;
} 

/***************************
RF �յ����ݴ����ж�
***************************/
#pragma vector=RF_VECTOR  
__interrupt void rf_isr(void)  
{  
  EA =0;  
  LED2 = 1;
  // ���յ�һ�����������ݰ�  
  if(RFIRQF0&(1<<6))  
  {  
    rf_receive_isr();                          //���ý����жϴ�����  
    S1CON =0;                                   //���RF�жϱ�־  
    RFIRQF0 &=~(1<<6);                        //���RF����������ݰ��ж�  
  }
  LED2 = 0;  
  EA =1;  
}  

/***************************
RF ���մ�����������
***************************/
void rf_receive_isr()  
{  
  int rf_rx_len=0;  
  
  rf_rx_len = RFD-2;                       //����ȥ�����ֽڸ��ӽ��  
  rf_rx_len &=0x7F;  
  for(int i=0; i< rf_rx_len; i++)  
  {  
    rf_rx_buf[i]= RFD;                      //������ȡ���ջ���������  
  }  
  
//  rf_rx_buf[rf_rx_len++] = RFD-73;                           //��ȡRSSI���  
//  rf_rx_buf[rf_rx_len++] = RFD;                              //��ȡCRCУ���� BIT7  
  volatile unsigned char rssi = RFD;
  volatile unsigned char crc_ok = RFD;
  
  RFST =0xED;                               //������ջ�����  
    if( crc_ok&0x80) // У��CRC
  {  
   // UartTX_Send_String(rf_rx_buf, rf_rx_len+1);   //���ڷ���   
    u8* pdata = NewMem(rf_rx_len + 1);
    *pdata++ = 'R'; 
    memcopy(rf_rx_buf, pdata,rf_rx_len);
    //    printf("[%d]",rssi);  
  }  

}  

/***************************
���ڳ�ʼ��
***************************/
void uart0_init()  
{  
  PERCFG =0x00;             // UART0ѡ��λ��0 TX@P0.3 RX@P0.2  
  P0SEL|=0x0C;             // P0.3 P0.2ѡ�����蹦��  
  U0CSR|=0xC0;             // UARTģʽ������ʹ��  
  U0GCR|=11;               //����� U0GCR�� U0BAUD  
  U0BAUD =216;              // 115200  
  UTX0IF =1;  
  
  URX0IE =1;                   //ʹ�ܽ����ж� IEN0@BIT2  
} 

/************************************
��ʱ�� 1 ��ʼ�� �������ڽ��մ�����ʱ
************************************/
void timer1_init()  
{  
  T1CTL=0x0C;               // @DIV��Ƶϵ�� 128 @MODE��ͣ����  
  T1CCTL0 =0x44;            // @IMͨ��0�ж�ʹ�� @MODE�Ƚ�ƥ��ģʽ  
  T1STAT =0x00;              //��������жϱ�־  
  
  T1IE =1;                // IEN1@BIT1ʹ�ܶ�ʱ��1�ж�  
  
  T1CC0L =250;               //�������Ϊ2ms  
  T1CC0H =0;  
}  

void timer1_disbale()  
{  
  T1CTL&=~(1<<1);       //�ָ�Ϊֹͣģʽ  
}  

void timer1_enable()  
{  
  T1CTL|=(1<<1);      //�ı�ģʽΪ�Ƚ�ƥ��ģʽ MODE = 0x10;  
  T1STAT =0x00;              //����жϱ�־λ   
  T1CNTH =0;              //���¿�ʼ����  
  T1CNTL =0;  
}  

#pragma vector=URX0_VECTOR  
__interrupt void UART0_ISR(void)  
{  
  URX0IF =0;                                  //��������жϱ�־  
  serial_rxbuf[serial_rxpos]= U0DBUF;    //��仺����  
  serial_rxpos++;  
  serial_rxlen++;  
  
  timer1_enable();                             //��ʱ�����¿�ʼ����  
}  

#pragma vector=T1_VECTOR  
__interrupt void Timer1_ISR(void)  
{  
  T1STAT &=~(1<<0);                         //�����ʱ��T1ͨ��0�жϱ�־  
  
  if(serial_rxlen)
  {//is_serial_receive =1;                       //�������ݵ���  
    
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