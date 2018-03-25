/**********************************************************************************************************
* Copyright       : COPYRIGHT(C) 1997-2005 PUTRON TECHNOLOGY (CHINA)  CO., LTD.
                    All RIGHTS RESERVED.
* Website         : Http://WWW.PUTRON.COM
* TEL             : 00
 
* Project Name    : 1U Series Matrix.
* File name       : IR.C
* PCB P/N         : --
* Comments        : IR process routine.
* ----------< History >---------------------------------------------------------
* File Version    : 2.0             
* Author          : sam 
* E-Mail          : putrony@163.com 
* Date            : 2005.Aug.20
* Comments        : add this title    
**********************************************************************************************************/

/**********************************************************************************************************
**********************************************************************************************************/
#include <string.h>
#include "main.h"
#include "stm32f10x.h"
#include "IR.h"
#include "uart.h"
#include "KeyTasks.h"

void TIM3_Configuration(void);

//#define  IR_port		 (GPIOA->IDR  & GPIO_Pin_1)
static WORD	IrIndex = IR_INDEXDUMMY;
static WORD 	IrLWord = IR_DUMMY;
static BYTE   	B_IrClockCounter;   // ��������Ŀ��ʱ��
static BYTE  	B_IrPacketCounter; // �ж�������TIMER OUT ��ʱ��
static WORD	LastIRKey = IR_DUMMY;
//static BYTE    cFactoryID = 0xff;


BYTE	IRKey = IR_DUMMY;


void IR_interruptConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
    
  /*����������IO������ʱ�������ж���*/ 
  /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
  /*����������IO������ʱ�������ܽ�*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ת���ٶ� 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure Button EXTI line */        //  PA1 IR input
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling ; //�������жϴ���  EXTI_Trigger_Rising
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /*���������ú�����յĹܽŵ��ж��ߵ��ж�����*/ 
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;	 //PC3����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_IR_PRE_PRTY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_IR_SUB_PRTY;	  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void TIM3_IRQHandler(void)   //100US
{
    TIM_ClearFlag(TIM3, TIM_FLAG_Update); //һ��Ҫ���ж�

    IRTimerInt();

}
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line1); //һ��Ҫ���ж�

    IREdgeInterrupt();

    //appIRHandle();
}
bool_t  IRInit(void)
{
    IRKey = IR_DUMMY;
    IrIndex = IR_INDEXDUMMY;
    IrLWord = 0;
    B_IrClockCounter = 0;
    B_IrPacketCounter = 0;
    //cFactoryID = 0xff;
    IR_interruptConfig();
    //TIM3_Configuration();
    //EXTI->IMR |= EXTI_Line1; //���ⲿ�ж�
	return true;
}
//******************************************************************************
//
// FUNCTION:    interrupt IRTimerInt(void)
// USAGE:       Timer1 interrupt service routine.
//
// INPUT:       None
// OUTPUT:      None
// GLOBALS:		None
// USED_REGS:	T1CON, EOI
//
//******************************************************************************
void  IRTimerInt(void)
{
    TIM_Cmd(TIM3, ENABLE); //����tTIM3����
                           //  TIM3_Configuration();

    B_IrClockCounter++;
    //W_IrPacketCounter++;  // use byte to reduce the variable space and up the speed

    if (B_IrClockCounter == 200) // 20ms  per  ++
    {
        //B_IrClockCounter = 0;
        B_IrPacketCounter++;
    }
    /*  
    IrPacketCounter :   ��־�೤ʱ��������û���յ����ݣ��ͱ�ʾ���ٷ������ݡ�
                          �ɴ˿��Թر�TIMER1,
                          �ڽ��ܵ����ݺ�Ҫ���㡣
         */
    if (B_IrPacketCounter > (BYTE)(IR_AUTOREPEAT_RESET / 200))
    {
        // Clear key
        //IRKey = IR_DUMMY;
        LastIRKey = IR_DUMMY;
        IrLWord = IR_DUMMY;  //   here is add by kyq 20040421
        IrIndex = IR_INDEXDUMMY;
        //cFactoryID = 0xff;
        // disable timer interrupt
        //TR1 = 0;
        TIM_Cmd(TIM3, DISABLE);
        B_IrClockCounter = 0;
        B_IrPacketCounter = 0;
    }
}
//******************************************************************************
//
// FUNCTION:    interrupt IRStartInterrupt(void)
// USAGE:       Rising edge interrupt service routine
//
// INPUT:       None
// OUTPUT:      None
// GLOBALS:		None
// USED_REGS:	T1CNT, T1CON, IMASK, EOI
//
//   NOTE :  here maybe exist some bug:
//              that when  oter plus couse it will as  repeat data,
//
//
//
//******************************************************************************
void  IREdgeInterrupt(void)
{
    BYTE  Is_Remote_Plus = 0;
    BYTE  B_IrClockCounter_temp = 0;
    B_IrClockCounter_temp = B_IrClockCounter;
    B_IrClockCounter = 0;
    TIM_Cmd(TIM3, ENABLE); //����tTIM3����
                           // TIM3_Configuration();
    B_IrPacketCounter = 0;
    if (IrIndex == IR_INDEXDUMMY)
    {
        if ((B_IrClockCounter_temp < IR_LEADER_MAX) && (B_IrClockCounter_temp > IR_LEADER_MIN)) // leader come
        {
            IrIndex = 0;
            IrLWord = 0;
            B_IrPacketCounter = 0;
            Is_Remote_Plus = 1;
        }
    }
    else
    {
//#if IR_REPEAT_MAX
        if ((B_IrClockCounter_temp < IR_REPEAT_MAX) && (B_IrClockCounter_temp > IR_REPEAT_MIN))
        {
            B_IrPacketCounter = 0;

            // not let repeater function kyq 20070625
            //IrIndex = 0;
            //IrLWord = IR_DUMMY;
            IrLWord = LastIRKey;
            Is_Remote_Plus = 1;

        }
//#endif
        if ((B_IrClockCounter_temp < IR_LOGIC0_MAX) && (B_IrClockCounter_temp > IR_LOGIC0_MIN))
        {
            IrLWord >>= 1;
            IrIndex++;
            Is_Remote_Plus = 1;
            // B_IrClockCounter=0;
        }
        if ((B_IrClockCounter_temp < IR_LOGIC1_MAX) && (B_IrClockCounter_temp > IR_LOGIC1_MIN))
        {
            IrLWord >>= 1;
            IrLWord |= 0x8000;
            IrIndex++;
            Is_Remote_Plus = 1;
            //  B_IrClockCounter=0;
        }
/*        
        if (!Is_Remote_Plus)
        {
            DEBUG_PRINT( MSG_DBG, "B_IrClockCounter_temp =%d, %d\r\n",IrIndex,B_IrClockCounter_temp ); 
        }
*/        
// here change by kyq the device code and operation is 0
        if ((IrIndex == IR_DEVICE_LENGTH) && Is_Remote_Plus)
        {
#if 0
            if((IrLWord&0xFF)&(IrLWord>>8))
            {
                IrIndex = IR_INDEXDUMMY;
                B_IrPacketCounter = 0;
                cFactoryID = 0xff;

            }
            else
#endif
            {
              //DEBUG_PRINT( MSG_DBG, "IR_DEVICE_CODE = %4x\r\n",IrLWord );
                if (IrLWord == IR_DEVICE_CODE)
                {
                   ;//cFactoryID = 0x00;
                }

                else
                {
                    IrIndex = IR_INDEXDUMMY;
                    B_IrPacketCounter = 0;
			 return;
                    //cFactoryID = 0xff;
		      //DEBUG_PRINT( MSG_DBG, "IR_DEVICE_CODE = %4x\r\n",IrLWord );
                }
            }


        }
        if ((IrIndex >= IR_DATA_LENGTH) && Is_Remote_Plus)
        {

            if (IrLWord & (IrLWord >> 8))
            {
                IrLWord = IR_DUMMY;  //check data
                //cFactoryID = 0xff;
            }
            IRKey = (BYTE)IrLWord;
            LastIRKey = IrLWord;
            //UART1_SendByte(IRKey);
            IrLWord = IR_DUMMY;  //cancel by kyq so we can get the repeat key to fit nec proto
            //if(IRKey == 0xff)
            //DEBUG_PRINT( MSG_DBG, "IRKey = %d\r\n",IRKey ); 

        }

        if (IrIndex > IR_DATA_LENGTH)
        {
            IrIndex = IR_INDEXDUMMY;
            IrLWord = IR_DUMMY;
            //cFactoryID = 0xff;
        }
    }

    //B_IrClockCounter = 0;
}




