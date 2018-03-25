#include <bsp_Gpio.h>

const BSP_GPIO_ConfigTypeDef BSP_GPIO_CONFIG[] =
{
    //egpioSii9533Reset,         // C13
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_13,
        GPIO_Mode_Out_PP,
        TRUE,
    },
    //egpioSii9679Reset,         // C6
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_6,
        GPIO_Mode_Out_PP,
        TRUE,
    },
    // egpioPCA9539_Reset,             // B10
    {
        GPIOB,
        RCC_APB2Periph_GPIOB,
        GPIO_Pin_10,
        GPIO_Mode_Out_PP,
        TRUE,
    },
    // egpioFAN_PWRCTRL,             
#if (MACHINETYPE==MHUB4K431)
    {                       // PC12
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_12,
        GPIO_Mode_Out_PP,
        TRUE,
    },
#else
    {                        // PC06
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_6,
        GPIO_Mode_Out_PP,
        TRUE,
    },
#endif   
    // egpioAP3012_PWRCTRL,               //C11
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_11,
        GPIO_Mode_Out_PP,
        TRUE,
    },
#if ((MACHINETYPE==MUH44E_BYE)||(MACHINETYPE==MHUB4K431))
    //egpioPOE_CTRL1,                    //C0  // MUST keep order
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_0,
        GPIO_Mode_Out_PP,
        FALSE,
    },
#elif ((MACHINETYPE==MUH88E_BYE)||(MACHINETYPE==MUH4K862))
    //egpioPOE_CTRL1,                    //B11  // MUST keep order
    {
        GPIOB,
        RCC_APB2Periph_GPIOB,
        GPIO_Pin_11,
        GPIO_Mode_Out_PP,
        FALSE,
    },
#endif

    //egpio4604_PWRCTRL,               //C14
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_14,
        GPIO_Mode_Out_PP,
        FALSE,						//低电平供电，高电平断电
    },

    //egpio5V_PWRCTRL,               //C15
    {
        GPIOC,
        RCC_APB2Periph_GPIOC,
        GPIO_Pin_15,
        GPIO_Mode_Out_PP,
        FALSE,
    },
    //  egpioLED,                    //A15
    {
        GPIOA,
        RCC_APB2Periph_GPIOA,
        GPIO_Pin_15,
        GPIO_Mode_Out_PP,
        TRUE,
    },


    //egpioANDReset,                    // PD2

    {
        GPIOD,
        RCC_APB2Periph_GPIOD,
        GPIO_Pin_2,
        GPIO_Mode_Out_PP,
        TRUE,
    }


};

void BSP_Gpio_Init(void)
{
    int i;
    GPIO_InitTypeDef	GPIO_InitStructure;

    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    for (i = 0; i < sgpINVALID; i++)
    {
        GPIO_InitStructure.GPIO_Mode = BSP_GPIO_CONFIG[i].GPIO_MODE;
        RCC_APB2PeriphClockCmd(BSP_GPIO_CONFIG[i].GPIO_CLK, ENABLE);
        GPIO_InitStructure.GPIO_Pin = BSP_GPIO_CONFIG[i].GPIO_PIN;
        if(BSP_GPIO_CONFIG[i].InitValue)
        {
            GPIO_SetBits(BSP_GPIO_CONFIG[i].GPIO_PORT, BSP_GPIO_CONFIG[i].GPIO_PIN);
        }
        else
        {
    	    GPIO_ResetBits(BSP_GPIO_CONFIG[i].GPIO_PORT, BSP_GPIO_CONFIG[i].GPIO_PIN); 
        }
        GPIO_Init(BSP_GPIO_CONFIG[i].GPIO_PORT, &GPIO_InitStructure);
    }





}

// OUT PUT PIN IS LOW
void BSP_Gpio_Low(WORD id)
{
    int i;
    if (sgpINVALID == id)
    {
        for (i = 0; i < sgpINVALID; i++)
        {
            if (BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_Out_OD ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_Out_PP ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_AF_OD ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_AF_PP)
                GPIO_ResetBits(BSP_GPIO_CONFIG[i].GPIO_PORT, BSP_GPIO_CONFIG[i].GPIO_PIN);
        }
    }
    else
    {
        if (BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_OD ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_PP ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_OD ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_PP)
            GPIO_ResetBits(BSP_GPIO_CONFIG[id].GPIO_PORT, BSP_GPIO_CONFIG[id].GPIO_PIN);
    }
}

// OUPUT PIN IS HIG
void BSP_Gpio_High(WORD id)
{
    int i;
    if (sgpINVALID == id)
    {
        for (i = 0; i < sgpINVALID; i++)
        {
            if (BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_Out_OD ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_Out_PP ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_AF_OD ||
                BSP_GPIO_CONFIG[i].GPIO_MODE == GPIO_Mode_AF_PP)

                GPIO_SetBits(BSP_GPIO_CONFIG[i].GPIO_PORT, BSP_GPIO_CONFIG[i].GPIO_PIN);
        }
    }

    else
    {
        if (BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_OD ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_PP ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_OD ||
            BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_PP)

            GPIO_SetBits(BSP_GPIO_CONFIG[id].GPIO_PORT, BSP_GPIO_CONFIG[id].GPIO_PIN);
    }
}

void BSP_Gpio_Toggle(WORD id)
{
    uint16_t pins;


    if (BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_OD ||
        BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_Out_PP ||
        BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_OD ||
        BSP_GPIO_CONFIG[id].GPIO_MODE == GPIO_Mode_AF_PP)
    {

        pins =  GPIO_ReadOutputData(BSP_GPIO_CONFIG[id].GPIO_PORT);
        if (pins & BSP_GPIO_CONFIG[id].GPIO_PIN)
            GPIO_ResetBits(BSP_GPIO_CONFIG[id].GPIO_PORT, BSP_GPIO_CONFIG[id].GPIO_PIN);
        else
            GPIO_SetBits(BSP_GPIO_CONFIG[id].GPIO_PORT, BSP_GPIO_CONFIG[id].GPIO_PIN);
    }

}



void BSP_GPIO_LED_Toggle(void)
{
    BSP_Gpio_Toggle(egpioLED);
}




// gpio HIG will volume

BOOL BSP_Gpio_GetBitValue(WORD id)
{
    if (id < sgpINVALID)
        return ((BSP_GPIO_CONFIG[id].GPIO_PORT->IDR & BSP_GPIO_CONFIG[id].GPIO_PIN) ? TRUE : FALSE);
    else
        return FALSE;

}
void BSP_GPIO_FanPower_EN(uint8_t en)
{
        if (en == 1)
        BSP_Gpio_High(egpioFAN_PWRCTRL);
    else
        BSP_Gpio_Low(egpioFAN_PWRCTRL);

}
void BSP_GPIO_AP3012_EN(uint8_t en)     //高电平AP3012给运放供电，低电平AP3012断运放的电
{
    #if (MACHINETYPE == MUH4K862)
    en = en;
    #else
    if (en == 1)
        BSP_Gpio_High(egpioAP3012_PWRCTRL);
    else
        BSP_Gpio_Low(egpioAP3012_PWRCTRL);
    #endif
}

void BSP_GPIO_4604_EN(uint8_t en)     
{
    if (en == 1)
        BSP_Gpio_High(egpio4604_PWRCTRL);
    else
        BSP_Gpio_Low(egpio4604_PWRCTRL);
}

void BSP_GPIO_5V_EN(uint8_t en)     
{
    if (en == 1)
        BSP_Gpio_High(egpio5V_PWRCTRL);
    else
        BSP_Gpio_Low(egpio5V_PWRCTRL);
}







