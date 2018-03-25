/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 * 2011-07-12     Lin          配合驱动修改本文件
  *这里将常用的外设宏定义是否使能，可以随时开关。由于STM32有管脚冲定义模式，所有有的外设例如串口，SPI等重映射后管脚也不一样了
 *所以在这里进行宏定义（即_REMAP），有些外设（SPI2就没有重映射）就只宏定义是否使能此晚上，通过这些进而确立此外设所用到的IO口
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

//[kyq add: 20150717 11:26:04]
  #define  OS_CFG_TICK_RATE_HZ            1000u               /* Tick rate in Hertz (10 to 1000 Hz)                     */
// <h>TIMEs Settings
// <e>Use TIME1
// 	<i>Enable TIME1
#define STM32_USING_TIME1       0
//	<o> Period: <1-65535>
//		<i>Default: 200
#define TIME1_DEFAULT_Period  200
//	<o> Prescaler: <1-65535>
//		<i>Default: 3599
#define TIME1_DEFAULT_Prescaler  359
// </e>
// <e>Use TIME2
// 	<i>Enable TIME2
#define STM32_USING_TIME2       1   // //[kyq add: 20150826 13:35:08]
//	<o> Period: <1-65535>
//		<i>Default: 200
#define TIME2_DEFAULT_Period  499
//	<o> Prescaler: <1-65535>
//		<i>Default: 3599
#define TIME2_DEFAULT_Prescaler  3599
// </e>
// <e>Use TIME3
// 	<i>Enable TIME3
#define STM32_USING_TIME3       1
//	<o> Period: <1-65535>
//		<i>Default: 200
#define TIME3_DEFAULT_Period  199
//	<o> Prescaler: <1-65535>
//		<i>Default: 3599
#define TIME3_DEFAULT_Prescaler  35//64
// </e>
// <e>Use TIME4
// 	<i>Enable TIME4
#define STM32_USING_TIME4       1
//	<o> Period: <1-65535>
//		<i>Default: 200
#define TIME4_DEFAULT_Period  200
//	<o> Prescaler: <1-65535>
//		<i>Default: 3599
#define TIME4_DEFAULT_Prescaler  3599
// </e>
// </h>



// <h>UARTs Settings
// <e>Use UART1
// 	<i>Enable UART1
#define STM32_USING_UART1       1
//	<o> Baudrate: <1-2000000>
//		<i>Default: 115200
#define UART1_DEFAULT_BAUDRATE  9600
//	<q.6> Stream Device
//		<i>Default: 0
//	<q.10> TxNonblock
//		<i>Default: 0
#define RT_UART1_FLAGx 0x000
//	<o> PIN: <0=> PA9,PA10 <1=>PB6,PB7
//		<i>Default: 0
#define UART1_REMAP    0
// </e>
// <e>Use UART2
// 	<i>Enable UART2
#define STM32_USING_UART2       1
//	<o> Baudrate: <1-2000000>
//		<i>Default: 115200
#define UART2_DEFAULT_BAUDRATE  115200
//	<q.6> Stream Device
//		<i>Default: 0
//	<q.10> TxNonblock
//		<i>Default: 0
#define RT_UART2_FLAGx 0x000
//	<o> PIN: <0=> PA2,PA3 <1=>PD5,PD6
//		<i>Default: 0
#define UART2_REMAP    0
// </e>
// <e>Use UART3
// 	<i>Enable UART3
#define STM32_USING_UART3       0
//	<o> Baudrate: <1-2000000>
//		<i>Default: 115200
#define UART3_DEFAULT_BAUDRATE  115200
//	<q.6> Stream Device
//		<i>Default: 0
//	<q.10> TxNonblock
//		<i>Default: 0
#define RT_UART3_FLAGx 0x000
//	<o> PIN: <0=> PB10,PB11 <1=>PC10,PC11 <3=>PD8,PD9
//		<i>Default: 0
#define UART3_REMAP    1
// </e>
// <e>Use UART4
// 	<i>Enable UART4
#define STM32_USING_UART4       0
//	<o> Baudrate: <1-2000000>
//		<i>Default: 115200
#define UART4_DEFAULT_BAUDRATE  115200
//	<q.6> Stream Device
//		<i>Default: 0
//	<q.10> TxNonblock
//		<i>Default: 0
#define RT_UART4_FLAGx 0x040
// </e>
// <e>Use UART5
// 	<i>Enable UART5
#define STM32_USING_UART5       0
//	<o> Baudrate: <1-2000000>
//		<i>Default: 115200
#define UART5_DEFAULT_BAUDRATE  115200
//	<q.6> Stream Device
//		<i>Default: 0
//	<q.10> TxNonblock
//		<i>Default: 0
#define RT_UART5_FLAGx 0x000
// </e>
// </h>

// <h>SPIs Settings
// <e>Use SPI1
// 	<i>Enable SPI1
#define STM32_USING_SPI1       1
//	<o.3..5> Speed: <0=>CLK/2 <1=>CLK/4 <2=>CLK/8 <3=>CLK/16 <4=>CLK/32 <5=>CLK/64 <6=>CLK/128 <7=>CLK/256
//		<i>Default: 0
//	<o.7> Format: <0=>MSB First <1=> LSB First
//		<i>Default: 0
//	<o.0..1> Mode: <0=>00 <1=> 01 <2=> 10 <3=> 11
//		<i>Default: 0
//	<o.11> Bits: <0=>8Bits <1=> 16Bits
//		<i>Default: 0
#define SPI1_WMODEX    	0x0008
#define SPI1_WMODE		(SPI_CONF_2LINES_FULLDUPLEX|SPI_CONF_MASTER|SPI1_WMODEX)
//	<o> Pin: <0=>PA5,PA6,PA7 <1=> PB3,PB4,PB5
//		<i>Default: 0
#define SPI1_REMAP    0
// </e>
// <e>Use SPI2
// 	<i>Enable SPI2
#define STM32_USING_SPI2       0
//	<o.3..5> Speed: <0=>CLK/2 <1=>CLK/4 <2=>CLK/8 <3=>CLK/16 <4=>CLK/32 <5=>CLK/64 <6=>CLK/128 <7=>CLK/256
//		<i>Default: 0
//	<o.7> Format: <0=>MSB First <1=> LSB First
//		<i>Default: 0
//	<o.0..1> Mode: <0=>00 <1=> 01 <2=> 10 <3=> 11
//		<i>Default: 0
//	<o.11> Bits: <0=>8Bits <1=> 16Bits
//		<i>Default: 0
#define SPI2_WMODEX    	0x0023
#define SPI2_WMODE		(SPI_CONF_2LINES_FULLDUPLEX|SPI_CONF_MASTER|SPI2_WMODEX)
//	</e>
// <e>Use SPI3
// 	<i>Enable SPI3
#define STM32_USING_SPI3       0
//	<o.3..5> Speed: <0=>CLK/2 <1=>CLK/4 <2=>CLK/8 <3=>CLK/16 <4=>CLK/32 <5=>CLK/64 <6=>CLK/128 <7=>CLK/256
//		<i>Default: 0
//	<o.7> Format: <0=>MSB First <1=> LSB First
//		<i>Default: 0
//	<o.0..1> Mode: <0=>00 <1=> 01 <2=> 10 <3=> 11
//		<i>Default: 0
//	<o.11> Bits: <0=>8Bits <1=> 16Bits
//		<i>Default: 0
#define SPI3_WMODEX    	0x0003
#define SPI3_WMODE		(SPI_CONF_2LINES_FULLDUPLEX|SPI_CONF_MASTER|SPI3_WMODEX)
//	<o> Pin: <0=> PB3,PB4,PB5 <1=>PC10,PC11,PC12
//		<i>Default: 0
#define SPI3_REMAP    0
// </e>
// </h>

#endif

// <<< Use Configuration Wizard in Context Menu >>>
