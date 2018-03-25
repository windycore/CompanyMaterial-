#include "stm32f10x.h"
#include "main.h"
#include "dvCPLD.h"
#include "appSystemMgrAPI.h"
#include "Util.h"
#include "dv74ls138.h"

void send_data_to_FPGA(unsigned char data)
{
    unsigned char i;

    dvSelectChilpEnable(eSil9396IN_EN);			//把LCD1602的使能脚拉高，即不使能LCD1602片选
    HC595_DATA_LOW;             //clean
    SHCP_LOW;
    STCP_LOW;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {HC595_DATA_HIGH;}   //1;
        else
        {HC595_DATA_LOW;}    //0;
        SHCP_HIGH;
        SHCP_LOW;
        data <<= 1;                //send sequence: MSB bit to LSB bit
    }
    delay_nus(5);
    STCP_HIGH;
    HC595_DATA_LOW;           //clean all port
    SHCP_LOW;
    STCP_LOW;
}

//0表示应答，1表示无应答
unsigned char check_ack(void)
{
    unsigned char errtime = 225;
    set_ack_high; //为低电平时就表示有应答，所以在这里先将总线拉高(这里不是真正意思的拉高，开漏输出只能输出0，高完全是上拉的作用)
    delay_nus(10);
    while (CPLD_ACK)
    {
        errtime--;
        if (errtime == 0)
        {
            return (1);
        }
    }
    return (0);
}
#ifdef PRD_IR_OUT
//  IR_ALL IN ---> ALL Remote IR(402P)
//  IR EYE ----> MCU IR
//
//  IR CTRL ---> MCU IR
//  IR CTRL 带去载波功能
//  IR Remote(R) ---> IR Out
//表示把第y路远端402的红外信号切到第x路本地IR OUT
void dvCPLDIRSwitchSet(BYTE cRemoteIRIn, BYTE cLocalIROut)
{


    BYTE i;
    send_data_to_FPGA(0xec);//表示把第y路远端402的红外信号切到第x路本地IR OUT
    for (i = 0; i < 2; i++)  // try 2 if fail
    {
        send_data_to_FPGA(cRemoteIRIn);//远端输入
        if (!check_ack())
            break;
    }
    for (i = 0; i < 2; i++)  // try 2 if fail
    {
        send_data_to_FPGA(cLocalIROut);//本地输出
        if (!check_ack())
            break;
    }
    
    send_data_to_FPGA(0xef);


}

//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0
// 0->1, 1->2, 3->4,4->0
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号

void dvCPLDIRSwitcherSet(BYTE cInput, BYTE cPortNumber, va_list pArgs)
{

    va_list         args;
    BYTE         i;
    BYTE         cOutput;
    args = pArgs;                    
    cInput = (cInput+1)%(MAX_IN+1); 
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = va_arg(args, BYTE);
        cOutput = (cOutput+1)%(MAX_IN+1);
        // change the map
        dvCPLDIRSwitchSet(cInput,cOutput);
    }
    va_end(args);
}
//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvCPLDIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList)
{


    BYTE         i;
    BYTE         cOutput;
    cInput = (cInput+1)%(MAX_IN+1);
    for (i = 0; i < cPortNumber; i++)
    {

        cOutput  = (pcOutPortList[i]+1)%(MAX_IN+1);
        // change the map
        dvCPLDIRSwitchSet(cInput,cOutput);
        
    }


    
}


#endif
//   bEnable - TRUE
//                   有载波就直通，没载波 CPLD 加上载波
//               -FASLE
//                  直通
void dvCPLDIRCarrierSet(BOOL bEnable)
{
  BYTE i;
  	send_data_to_FPGA(0xea);
    for (i = 0; i < 2; i++)
    {
        send_data_to_FPGA(bEnable ? 0xDF : 0xDE);
        if (!check_ack())
            break;
    }
    send_data_to_FPGA(0xef);
}


// 远端过来的数据
// 本机232信号(MCU)切到第y路远端。
// cRemoteUart - 0: 表示断开
//             - f: 表示开启所有远端过来的串口数据
void dvCpldUartRemote2McuSet(BYTE cRemoteUart)
{
    BYTE i;
    send_data_to_FPGA(0xed);//本地到远端402
    for (i = 0; i < 2; i++)
    {
        send_data_to_FPGA(0xf0 | (cRemoteUart & 0x0f));
        if (!check_ack())
            break;
    }
    send_data_to_FPGA(0xef);
}


//
//ee    : 数据包包头
//cRemote: 通道号, 从1 开始
//bps_rate: 以何种波特率发送串口数据给远端, 从1开始
//支持波特率2400；4800；9600；19200；38400；57600；115200；
//data_0
// ef:数据包结束
// this function test ok, excepte all port send
void dvCpldSendDataToRemote(BYTE cRemote, BYTE cbps_rate,  PBYTE pcData,BYTE cDataLength)
{
    BYTE i;   
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    if((cbps_rate == 0)||(cRemote == 0))
	return;
		
    send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
    send_data_to_FPGA(cRemote); // cRemote + 1
    send_data_to_FPGA(cbps_rate - 1);
#ifndef VHD4_CPLD
    for (i = 0; i < cDataLength; i++)
    {
        send_data_to_FPGA(*pcData++);
    }
#endif
   send_data_to_FPGA(0xEF);
   psSystemSettingData->acRemoteBaudRate[cRemote-1]=cbps_rate;
   utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#ifdef VHD4_CPLD
	   delay_nms(1);

	   for (i = 0; i < cDataLength; i++)
	   {
		   UART1_SendByte(*pcData++);
	   }
	   delay_nms(10); //稍微等CLPD 一会发数据
	   send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
	   send_data_to_FPGA(0); // cRemote + 1
	   send_data_to_FPGA(cbps_rate - 1);
	   send_data_to_FPGA(0xEF);

#endif
   
}

void dvCpldRemoteBraundSet(BYTE cRemote)
{
   // BYTE i;   
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);

		
    send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
    send_data_to_FPGA(cRemote); // cRemote + 1
    send_data_to_FPGA(psSystemSettingData->acRemoteBaudRate[cRemote-1] - 1);   
    send_data_to_FPGA(0xEF);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
}

// MCU 与 DB9( PC 通讯时候 调用这个函数? 数据不会发给402?
//        串口放松完数据后, 恢复
//bConnect - 0: 断开远端
void dvCpldMcuUartConnectToRemote(BOOL bConnect)
{
    //BYTE i;
    if (!bConnect)
    {
        send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
    }
    else
    {
         send_data_to_FPGA(0xEF);
    }
   
}

void dvCpldClose(void)
{
#ifndef PRD_IR_OUT
     //send_data_to_FPGA(0xFB);   //让远端码过来//
      send_data_to_FPGA(0xFA);  // 断开远端数据
#endif
     dvCpldUartRemote2McuSet(0x0);


}


void dvCpldOpen(void)
{
#ifndef PRD_IR_OUT
     send_data_to_FPGA(0xFB);   //让远端码过来//
 #endif
     // send_data_to_FPGA(0xFA);  // 断开远端数据
    dvCpldUartRemote2McuSet(0x0f);


}

//#ifdef VHD4_CPLD
void dvCpldOpenMCUToDB9(BOOL bEnable)
{
	   send_data_to_FPGA(0xEE); //通知CPLD置为标志位，那么下面发送数据给PC时就不会再发给远端402了
	   send_data_to_FPGA(0); // cRemote + 1
	   //send_data_to_FPGA(cbps_rate - 1);
	   send_data_to_FPGA(0xEF);
}

//#endif
BYTE dvCpldInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; //这个不能省
#if (MACHINETYPE==MUH44E_BYE)
  //CPLD
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
#elif (MACHINETYPE==MUH88E_BYE)
    //CPLD
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  			//配置为开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
#endif

  return 1;

}
