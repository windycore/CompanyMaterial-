#include "bsp_i2c.h"
#include "bsp_gpio.h"
#include "board.h"
#define  INT_DEFAULT_GROUP   NVIC_PriorityGroup_2
#define  INT_UART1_PRE_PRTY              2
#define  INT_UART1_SUB_PRTY              0
#define  INT_UART2_PRE_PRTY               2
#define  INT_UART2_SUB_PRTY               0
// 按键固定扫描中断,每隔一段时间扫描一次按键
#define  INT_TIM2_PRE_PRTY                   3   // lowest, maybe conflict with UART,( long data will be lost data)
#define  INT_TIM2_SUB_PRTY                  0
// 红外计时中断
#define  INT_TIM3_PRE_PRTY                   1
#define  INT_TIM3_SUB_PRTY                  0
// 按键外部中断
#define  INT_KEY_PRE_PRTY                    2
#define  INT_KEY_SUB_PRTY                    0
// LED 计时外部中断
#define  INT_TIM4_PRE_PRTY                    2
#define  INT_TIM4_SUB_PRTY                    1


extern void IR_interruptConfig(void);

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    :   
USB上拉在USB配置里   红外引脚配置在红外配置里
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure; //这个不能省

#if (MACHINETYPE==MUH44E_BYE)
   //9533 的RESET
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //远端输出供电引脚
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //9539  reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  // AP3012电源开关
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
  //9533 的RESET
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //远端输出供电引脚
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_14|GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //9539  reset
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  // AP3012电源开关
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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  			//配置为开漏输出
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
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  			//CPLD_ACK配置为开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // 4052
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // EDID拨码开关检测
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif

  AFIO->MAPR = (0x00FFFFFF & AFIO->MAPR)|0x02000000;//释放除SWD之外的JTAG的IO口 
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
//配置系统时钟(这里主要是IO口的)
void RCC_Configuration(void)
{
	//SystemInit();//配置系统时钟   这个在USB框架中已经配置了
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB 
	                       |RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD 
						   |RCC_APB2Periph_AFIO, ENABLE );
	//这里只是初始化了IO口和AFIO得时钟，其它的外设像串口 定时器等等
	//都在对应初始化函数中单独初始化了这些时钟
	//外部中断的时钟就是AFIO
}
//配置串口1
void USART1_Configuration(void)
{
	  #if(STM32_USING_UART1 == 1)	  
	  USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //这个不能省
    NVIC_InitTypeDef NVIC_InitStructure;
   /*下面是配置串口1的时钟*/
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //最先要配置时钟，有了时钟才可以配置外设
   /*下面是配置串口1的管脚*/
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
	
	 /*下面是配置串口1的中断向量*/
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_UART1_PRE_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_UART1_SUB_PRTY;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);    
   #endif 
}
//配置串口2
void USART2_Configuration(void)
{
	  #if(STM32_USING_UART2 == 1)
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //这个不能省
    NVIC_InitTypeDef NVIC_InitStructure; 
		/*下面是配置串口3的时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //最先要配置时钟，有了时钟才可以配置外设
    /*下面是配置串口2的管脚*/
    //GPIO_PinRemapConfig(GPIO_PartialRemap_USART2, ENABLE);//重定义功能开启
		/* Configure USART2 Rx (PA3) as input floating                             */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
    /* Configure USART2Tx (PA2) as alternate function push-pull                */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
		/*下面是配置串口2的工作模式即使能*/
		USART_InitStructure.USART_BaudRate            = UART2_DEFAULT_BAUDRATE  ;	
		USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits            = USART_StopBits_1;
		USART_InitStructure.USART_Parity              = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART2, ENABLE);
	
	/*下面是配置串口2的中断向量*/
    NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_UART2_PRE_PRTY; //
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_UART2_SUB_PRTY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);     
    #endif
}
//配置串口3
void USART3_Configuration(void)
{
	  #if(STM32_USING_UART3 == 1)
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure; //这个不能省
    NVIC_InitTypeDef NVIC_InitStructure; 
		/*下面是配置串口3的时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //最先要配置时钟，有了时钟才可以配置外设
    /*下面是配置串口3的管脚*/
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);//重定义功能开启
		  /* Configure USART3 Rx (PC11) as input floating                             */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    /* Configure USART3Tx (PC10) as alternate function push-pull                */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
		/*下面是配置串口3的工作模式即使能*/
		USART_InitStructure.USART_BaudRate            = UART3_DEFAULT_BAUDRATE  ;	
		USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits            = USART_StopBits_1;
		USART_InitStructure.USART_Parity              = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART3, ENABLE);
	
	/*下面是配置串口3的中断向量*/
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
 * Description    : 定时器初始化 TIM2,TIM3
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  定时器溢出时间10豪秒        
 * 计算：(36MHz * 2)/(3599+1)/200   的倒数  这个*2是系统自己完成的，如果是TIM1的话，这里就是*1了
 * 说明：36MHz：AHB1时钟    (36MHz * 2):TIM2CLK 
 *       3599+1 :分频Prescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : 最大计数值     TIMx->CNT初始值为0,
 *重设初始值函数：void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *
 *    72/3600/500 = 0.00004MHZ = 25000us = 25ms
 *******************************************************************************/ 
void TIM2_Configuration(void) //50ms
{
	 #if(STM32_USING_TIME2 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*下面是配置定时器2的时钟*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE );  //最先要配置时钟，有了时钟才可以配置外设
   /*下面是配置定时器2的工作模式即使能*/ 
   TIM_DeInit(TIM2);                              //复位TIM2定时器
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME2_DEFAULT_Period;        //TIM2最大计数值200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME2_DEFAULT_Prescaler;    // 分频3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分割,这里为0的话那么TIM_Period 就为2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数 
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[清除TIM2溢出中断标志] */ 
   TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2溢出中断允许]*/ 
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [允许tim2计数]*/ 
   TIM_Cmd(TIM2, ENABLE);
   /*下面是配置定时器2的中断向量*/ 
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
 * Description    : 定时器初始化 TIM2,TIM3
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  定时器溢出时间10豪秒        
 * 计算：(36MHz * 2)/(3599+1)/200   的倒数  这个*2是系统自己完成的，如果是TIM1的话，这里就是*1了
 * 说明：36MHz：AHB1时钟    (36MHz * 2):TIM2CLK 
 *       3599+1 :分频Prescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : 最大计数值     TIMx->CNT初始值为0,
 *重设初始值函数：void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *  72/(TIME3_DEFAULT_Prescaler+1)/TIME3_DEFAULT_Period
 *  72/36/200 = 0.01Mhz = 100us = 0.1ms
 *******************************************************************************/ 
void TIM3_Configuration(void) //180us--->100us
{
	 #if(STM32_USING_TIME3 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*下面是配置定时器2的时钟*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE );  //最先要配置时钟，有了时钟才可以配置外设
   /*下面是配置定时器2的工作模式即使能*/ 
   TIM_DeInit(TIM3);                              //复位TIM2定时器
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME3_DEFAULT_Period;        //TIM2最大计数值200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME3_DEFAULT_Prescaler;    // 分频3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分割,这里为0的话那么TIM_Period 就为2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数 
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[清除TIM2溢出中断标志] */ 
   TIM_ClearFlag(TIM3, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2溢出中断允许]*/ 
   TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [允许tim2计数]*/ 
   TIM_Cmd(TIM3, DISABLE);
   /*下面是配置定时器2的中断向量*/ 
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);// 
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_TIM3_PRE_PRTY;// 指定抢占式优先级别
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_TIM3_SUB_PRTY;// 指定响应优先级别
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
   NVIC_Init(&NVIC_InitStructure);
   #endif
}

  /******************************************************************************* 
 * Function Name  : TIMx_Configuration
 * Description    : 定时器初始化
 * Input          : None 
 * Output         : None 
 * Return         : None 
 *  定时器溢出时间10豪秒        
 * 计算：(36MHz * 2)/(3599+1)/200   的倒数  这个*2是系统自己完成的，如果是TIM1的话，这里就是*1了
 * 说明：36MHz：AHB1时钟    (36MHz * 2):TIM2CLK 
 *       3599+1 :分频Prescaler  (36MHz * 2)/(3599+1): TIMx counter clock
 *       200 : 最大计数值     TIMx->CNT初始值为0,
 *重设初始值函数：void TIM_SetCounter(TIM_TypeDef* TIMx, u16 Counter)
 *  72/(TIME3_DEFAULT_Prescaler+1)/TIME3_DEFAULT_Period
 *  72/36/200 = 0.01Mhz = 100us = 0.1ms
 *******************************************************************************/ 
void TIM4_Configuration(void) //180us--->200us
{
 #if(STM32_USING_TIME4 == 1)
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef NVIC_InitStructure;  
    //  TIM_OCInitTypeDef  TIM_OCInitStructure ;
      /*下面是配置定时器2的时钟*/ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE );  //最先要配置时钟，有了时钟才可以配置外设
   /*下面是配置定时器2的工作模式即使能*/ 
   TIM_DeInit(TIM4);                              //复位TIM2定时器
   /* TIM2 configuration */ 
   TIM_TimeBaseStructure.TIM_Period = TIME4_DEFAULT_Period;        //TIM2最大计数值200       
   TIM_TimeBaseStructure.TIM_Prescaler = TIME4_DEFAULT_Prescaler;    // 分频3599+1       
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分割,这里为0的话那么TIM_Period 就为2500;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数 
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
   /* Clear TIM2 update pending flag[清除TIM2溢出中断标志] */ 
   TIM_ClearFlag(TIM4, TIM_FLAG_Update); 
   /* Enable TIM2,TIM3 Update interrupt [TIM2溢出中断允许]*/ 
   TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);   
   /* TIM2enable counter [允许tim2计数]*/ 
   TIM_Cmd(TIM4, ENABLE);
   /*下面是配置定时器2的中断向量*/ 
   NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);// 
   NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_TIM4_PRE_PRTY;// 指定抢占式优先级别
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_TIM4_SUB_PRTY;// 指定响应优先级别
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
  除以1000 就是1ms 延时
  这里可以不用管系统频率是多少	 RCC_Clocks.SYSCLK_Frequency / 1	
  除数50就是频率，	 这里是1000HZ（1mS）
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
  SysTick_Config（）在_cm3.h文件中	使能了SysTick的中断
 * @brief  Initialize and start the SysTick counter and its interrupt.
 * Initialise the system tick timer and its interrupt and start the
 * system tick timer / counter in free running mode to generate 
 * periodical interrupts.
 */ 
}
/*看门狗初始化*   2.0的库和3.0的库有点区别*/
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

//SPI总线初始化
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
  /*下面是配置IO作输入时的三个中断线*/ 
  /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

  // PB11 is key int input
  /*下面是配置IO作输入时的三个管脚*/ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//转换速度 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /*下面是配置红外接收的管脚的中断线的中断向量*/ 
  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_PriorityGroupConfig(INT_DEFAULT_GROUP);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 //PC3公用
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_KEY_PRE_PRTY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_KEY_SUB_PRTY;	  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//配置所有外设
void Init_All_Periph(void)
{
	__disable_irq();//关总中断	
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
    //主程序中有按键扫描，在这里中断中还加按键扫描是为了防止程序运行在某个延时处理中，此时有按键主程序是检测不到的，只能靠中断先检测出来
    //current_key_temp = Key_Scan(); //这个函数处理的时间比较久
    //if ((current_key_temp != No_key) && (current_key_temp != MULTI_PRESSED))
    //{
    //    have_key_process = 1;
    //    current_key = current_key_temp;
    //}
    /* 不灵敏
      	halUICtrlEventHandler();  //[kyq ADD: 20150824 20:28:48] TEST
      	EXTI->IMR |= EXTI_Line11;
    */
 
}
