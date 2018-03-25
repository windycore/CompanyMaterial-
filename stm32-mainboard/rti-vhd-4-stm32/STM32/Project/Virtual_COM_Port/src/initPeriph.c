#include "bsp_i2c.h"
#include "bsp_gpio.h"
#include "board.h"
#define  INT_DEFAULT_GROUP   NVIC_PriorityGroup_2
#define  INT_UART1_PRE_PRTY              2
#define  INT_UART1_SUB_PRTY              0
#define  INT_UART2_PRE_PRTY               2
#define  INT_UART2_SUB_PRTY               0
// �����̶�ɨ���ж�,ÿ��һ��ʱ��ɨ��һ�ΰ���
#define  INT_TIM2_PRE_PRTY                   3   // lowest, maybe conflict with UART,( long data will be lost data)
#define  INT_TIM2_SUB_PRTY                  0
// �����ʱ�ж�
#define  INT_TIM3_PRE_PRTY                   1
#define  INT_TIM3_SUB_PRTY                  0
// �����ⲿ�ж�
#define  INT_KEY_PRE_PRTY                    2
#define  INT_KEY_SUB_PRTY                    0
// LED ��ʱ�ⲿ�ж�
#define  INT_TIM4_PRE_PRTY                    2
#define  INT_TIM4_SUB_PRTY                    1


extern void IR_interruptConfig(void);

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    :   
USB������USB������   �������������ں���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure; //�������ʡ

#if (MACHINETYPE==MUH44E_BYE)
   //9533 ��RESET
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //Զ�������������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //9539  reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  // AP3012��Դ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
 //LCD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#elif (MACHINETYPE==MUH88E_BYE)
  //9533 ��RESET
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //Զ�������������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //9539  reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  // AP3012��Դ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

//SWITCH    //9533 4052 W
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 /*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
*/
  
  //9679 4052 SWITCH
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  			//����Ϊ��©���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  #elif ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
  // 74HC138
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  //LCD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  			//CPLD_ACK����Ϊ��©���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // 4052
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // EDID���뿪�ؼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif

  AFIO->MAPR = (0x00FFFFFF & AFIO->MAPR)|0x02000000;//�ͷų�SWD֮���JTAG��IO�� 
}
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{ 	
  #ifdef PRD_USB_FUN
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);   //4000
  #else
	 #if 1//def USE_FULL_ASSERT  // VECT_TAB_RAM    // kyq
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	 #else  /* VECT_TAB_FLASH  */
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);   
	  #endif	  
 #endif  
}
//����ϵͳʱ��(������Ҫ��IO�ڵ�)
void RCC_Configuration(void)
{
	//SystemInit();//����ϵͳʱ��   �����USB������Ѿ�������
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB 
	                       |RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD 
						   |RCC_APB2Periph_AFIO, ENABLE );
	//����ֻ�ǳ�ʼ����IO�ں�AFIO��ʱ�ӣ������������񴮿� ��ʱ���ȵ�
	//���ڶ�Ӧ��ʼ�������е�����ʼ������Щʱ��
	//�ⲿ�жϵ�ʱ�Ӿ���AFIO
}
//���ô���1
void USART1_Configuration(void)
{
	  #if(STM32_USING_UART1 == 1)	  
	  USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //�������ʡ
    NVIC_InitTypeDef NVIC_InitStructure;
   /*���������ô���1��ʱ��*/
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
   /*���������ô���1�Ĺܽ�*/
   /* Configure USART1 Tx (PA9) as alternate function push-pull                */
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure); 	
   /* Configure USART1 Rx (PA10) as input floating                             */
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 USART_InitStructure.USART_BaudRate            = UART1_DEFAULT_BAUDRATE  ;	
	 USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	 USART_InitStructure.USART_Parity              = USART_Parity_No ;
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	 USART_Init(USART1, &USART_InitStructure);
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	 USART_Cmd(USART1, ENABLE);
	
	 /*���������ô���1���ж�����*/
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_UART1_PRE_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_UART1_SUB_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);    
   #endif 
}
//���ô���2
void USART2_Configuration(void)
{
	  #if(STM32_USING_UART2 == 1)
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //�������ʡ
    NVIC_InitTypeDef NVIC_InitStructure; 
		/*���������ô���3��ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
    /*���������ô���2�Ĺܽ�*/
    //GPIO_PinRemapConfig(GPIO_PartialRemap_USART2, ENABLE);//�ض��幦�ܿ���
		/* Configure USART2 Rx (PA3) as input floating                             */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
    /* Configure USART2Tx (PA2) as alternate function push-pull                */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
		/*���������ô���2�Ĺ���ģʽ��ʹ��*/
		USART_InitStructure.USART_BaudRate            = UART2_DEFAULT_BAUDRATE  ;	
		USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits            = USART_StopBits_1;
		USART_InitStructure.USART_Parity              = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART2, ENABLE);
	
	/*���������ô���2���ж�����*/
    NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_UART2_PRE_PRTY; //
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_UART2_SUB_PRTY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);     
    #endif
}
//���ô���3
void USART3_Configuration(void)
{
	  #if(STM32_USING_UART3 == 1)
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //�������ʡ
    NVIC_InitTypeDef NVIC_InitStructure; 
		/*���������ô���3��ʱ��*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
    /*���������ô���3�Ĺܽ�*/
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);//�ض��幦�ܿ���
		  /* Configure USART3 Rx (PC11) as input floating                             */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    /* Configure USART3Tx (PC10) as alternate function push-pull                */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
		/*���������ô���3�Ĺ���ģʽ��ʹ��*/
		USART_InitStructure.USART_BaudRate            = UART3_DEFAULT_BAUDRATE  ;	
		USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits            = USART_StopBits_1;
		USART_InitStructure.USART_Parity              = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART3, ENABLE);
	
	/*���������ô���3���ж�����*/
    NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_UART2_PRE_PRTY; //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);     
    #endif
}
 /******************************************************************************* 
 * Function Name  : TIMx_Configuration
 * Description    : ��ʱ����ʼ�� TIM2,TIM3
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  ��ʱ�����ʱ��10����        
 * ���㣺(36MHz * 2)/(3599+1)/200   �ĵ���  ���*2��ϵͳ�Լ���ɵģ������TIM1�Ļ����������*1��
 * ˵����36MHz��AHB1ʱ��    (36MHz * 2):TIM2CLK 
 *       3599+1 :��ƵPrescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : ������ֵ     TIMx->CNT��ʼֵΪ0,
 *�����ʼֵ������void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *
 *    72/3600/500 = 0.00004MHZ = 25000us = 25ms
 *******************************************************************************/ 
void TIM2_Configuration(void) //50ms
{
	 #if(STM32_USING_TIME2 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*���������ö�ʱ��2��ʱ��*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE );  //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
   /*���������ö�ʱ��2�Ĺ���ģʽ��ʹ��*/ 
   TIM_DeInit(TIM2);                              //��λTIM2��ʱ��
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME2_DEFAULT_Period;        //TIM2������ֵ200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME2_DEFAULT_Prescaler;    // ��Ƶ3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // ʱ�ӷָ�,����Ϊ0�Ļ���ôTIM_Period ��Ϊ2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ��� 
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[���TIM2����жϱ�־] */ 
   TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2����ж�����]*/ 
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [����tim2����]*/ 
   TIM_Cmd(TIM2, ENABLE);
   /*���������ö�ʱ��2���ж�����*/ 
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_TIM2_PRE_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_TIM2_SUB_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
   NVIC_Init(&NVIC_InitStructure);
   #endif
}
 /******************************************************************************* 
 * Function Name  : TIMx_Configuration
 * Description    : ��ʱ����ʼ�� TIM2,TIM3
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  ��ʱ�����ʱ��10����        
 * ���㣺(36MHz * 2)/(3599+1)/200   �ĵ���  ���*2��ϵͳ�Լ���ɵģ������TIM1�Ļ����������*1��
 * ˵����36MHz��AHB1ʱ��    (36MHz * 2):TIM2CLK 
 *       3599+1 :��ƵPrescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : ������ֵ     TIMx->CNT��ʼֵΪ0,
 *�����ʼֵ������void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *  72/(TIME3_DEFAULT_Prescaler+1)/TIME3_DEFAULT_Period
 *  72/36/200 = 0.01Mhz = 100us = 0.1ms
 *******************************************************************************/ 
void TIM3_Configuration(void) //180us--->100us
{
	 #if(STM32_USING_TIME3 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*���������ö�ʱ��2��ʱ��*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE );  //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
   /*���������ö�ʱ��2�Ĺ���ģʽ��ʹ��*/ 
   TIM_DeInit(TIM3);                              //��λTIM2��ʱ��
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME3_DEFAULT_Period;        //TIM2������ֵ200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME3_DEFAULT_Prescaler;    // ��Ƶ3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // ʱ�ӷָ�,����Ϊ0�Ļ���ôTIM_Period ��Ϊ2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ��� 
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[���TIM2����жϱ�־] */ 
   TIM_ClearFlag(TIM3, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2����ж�����]*/ 
   TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [����tim2����]*/ 
   TIM_Cmd(TIM3, DISABLE);
   /*���������ö�ʱ��2���ж�����*/ 
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);// 
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_TIM3_PRE_PRTY;// ָ����ռʽ���ȼ���
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_TIM3_SUB_PRTY;// ָ����Ӧ���ȼ���
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
   NVIC_Init(&NVIC_InitStructure);
   #endif
}

  /******************************************************************************* 
 * Function Name  : TIMx_Configuration
 * Description    : ��ʱ����ʼ��
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  ��ʱ�����ʱ��10����        
 * ���㣺(36MHz * 2)/(3599+1)/200   �ĵ���  ���*2��ϵͳ�Լ���ɵģ������TIM1�Ļ����������*1��
 * ˵����36MHz��AHB1ʱ��    (36MHz * 2):TIM2CLK 
 *       3599+1 :��ƵPrescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : ������ֵ     TIMx->CNT��ʼֵΪ0,
 *�����ʼֵ������void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *  72/(TIME3_DEFAULT_Prescaler+1)/TIME3_DEFAULT_Period
 *  72/36/200 = 0.01Mhz = 100us = 0.1ms
 *******************************************************************************/ 
void TIM4_Configuration(void) //180us--->200us
{
 #if(STM32_USING_TIME4 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*���������ö�ʱ��2��ʱ��*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE );  //����Ҫ����ʱ�ӣ�����ʱ�Ӳſ�����������
   /*���������ö�ʱ��2�Ĺ���ģʽ��ʹ��*/ 
   TIM_DeInit(TIM4);                              //��λTIM2��ʱ��
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME4_DEFAULT_Period;        //TIM2������ֵ200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME4_DEFAULT_Prescaler;    // ��Ƶ3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // ʱ�ӷָ�,����Ϊ0�Ļ���ôTIM_Period ��Ϊ2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ��� 
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[���TIM2����жϱ�־] */ 
   TIM_ClearFlag(TIM4, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2����ж�����]*/ 
   TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [����tim2����]*/ 
   TIM_Cmd(TIM4, ENABLE);
   /*���������ö�ʱ��2���ж�����*/ 
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);// 
   NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_TIM4_PRE_PRTY;// ָ����ռʽ���ȼ���
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_TIM4_SUB_PRTY;// ָ����Ӧ���ȼ���
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
   NVIC_Init(&NVIC_InitStructure);
   #endif
}
/*******************************************************************************
* Function Name  : SysTick_Config
* Description    : Configure a SysTick Base time to 20 ms.
* Input          : None
* Output         : None
* Return         : None
  ����1000 ����1ms ��ʱ
  ������Բ��ù�ϵͳƵ���Ƕ���	 RCC_Clocks.SYSCLK_Frequency / 1	
  ����50����Ƶ�ʣ�	 ������1000HZ��1mS��
*******************************************************************************/
void SysTick_Configuration(void)
{
  RCC_ClocksTypeDef   RCC_Clocks ;
  /* SystTick configuration: an interrupt every 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  //SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 50);
  SysTick_Config(RCC_Clocks.SYSCLK_Frequency / OS_CFG_TICK_RATE_HZ); //[kyq Begin: 20150717 11:27:12]
  /* Update the SysTick IRQ priority should be higher than the Ethernet IRQ */
  /* The Localtime should be updated during the Ethernet packets processing */
  NVIC_SetPriority (SysTick_IRQn, 1); 
  /**
  SysTick_Config������_cm3.h�ļ���	ʹ����SysTick���ж�
 * @brief  Initialize and start the SysTick counter and its interrupt.
 * Initialise the system tick timer and its interrupt and start the
 * system tick timer / counter in free running mode to generate 
 * periodical interrupts.
 */ 
}
/*���Ź���ʼ��*   2.0�Ŀ��3.0�Ŀ��е�����*/
void IWDG_Configuration(void)
{
  /* IWDG timeout equal to 2000 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  /* Set counter reload value to 1250 */
  IWDG_SetReload(2500);		// 2s 	//2S
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

//SPI���߳�ʼ��
void TM1623_key_led_Config_moni(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  #if(MACHINETYPE==MUH44E_BYE)
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;				//DIO,CLK ,STB
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#elif(MACHINETYPE==MUH88E_BYE)
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13;				//DIO,CLK ,STB
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
#elif ((MACHINETYPE==MUH44A_H2)||(MACHINETYPE==MUH88A_H2))
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;				//DIO,CLK ,STB
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
  
}
void KEY_In_interruptConfig(void)  		// PB11  key IR
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;	
  /*����������IO������ʱ�������ж���*/ 
  /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

  // PB11 is key int input
  /*����������IO������ʱ�������ܽ�*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ת���ٶ� 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����жϴ��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /*���������ú�����յĹܽŵ��ж��ߵ��ж�����*/ 
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 //PC3����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_KEY_PRE_PRTY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_KEY_SUB_PRTY;	  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//������������
void Init_All_Periph(void)
{
	__disable_irq();//�����ж�	
      //IWDG_Configuration();
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	TIM2_Configuration();
	TIM3_Configuration();

	//USART2_Configuration();
	BSP_Gpio_Init();
	dvI2CBusInit();
	IR_interruptConfig();
	__enable_irq();
}
/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External lines 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void) {
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI->IMR &= ~EXTI_Line11;
    //���������а���ɨ�裬�������ж��л��Ӱ���ɨ����Ϊ�˷�ֹ����������ĳ����ʱ�����У���ʱ�а����������Ǽ�ⲻ���ģ�ֻ�ܿ��ж��ȼ�����
    //current_key_temp = Key_Scan(); //������������ʱ��ȽϾ�
    //if ((current_key_temp != No_key) && (current_key_temp != MULTI_PRESSED))
    //{
    //    have_key_process = 1;
    //    current_key = current_key_temp;
    //}
    /* ������
      	halUICtrlEventHandler();  //[kyq ADD: 20150824 20:28:48] TEST
      	EXTI->IMR |= EXTI_Line11;
    */
 
}
