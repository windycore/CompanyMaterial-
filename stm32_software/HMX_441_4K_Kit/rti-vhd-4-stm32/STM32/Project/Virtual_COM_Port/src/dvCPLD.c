#include "stm32f10x.h"
#include "main.h"
#include "dvCPLD.h"
#include "appSystemMgrAPI.h"
#include "Util.h"

extern BYTE u8RemoteBaudRate;

void send_data_to_FPGA(unsigned char data)
{
    unsigned char i;

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

//0��ʾӦ��1��ʾ��Ӧ��
unsigned char check_ack(void)
{
    unsigned char errtime = 225;
    set_ack_high; //Ϊ�͵�ƽʱ�ͱ�ʾ��Ӧ�������������Ƚ���������(���ﲻ��������˼�����ߣ���©���ֻ�����0������ȫ������������)
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
//  IR CTRL ��ȥ�ز�����
//  IR Remote(R) ---> IR Out
//��ʾ�ѵ�y·Զ��402�ĺ����ź��е���x·����IR OUT
void dvCPLDIRSwitchSet(BYTE cRemoteIRIn, BYTE cLocalIROut)
{


    BYTE i;
    send_data_to_FPGA(0xec);//��ʾ�ѵ�y·Զ��402�ĺ����ź��е���x·����IR OUT
    for (i = 0; i < 2; i++)  // try 2 if fail
    {
        send_data_to_FPGA(cRemoteIRIn);//Զ������
        if (!check_ack())
            break;
    }
    for (i = 0; i < 2; i++)  // try 2 if fail
    {
        send_data_to_FPGA(cLocalIROut);//�������
        if (!check_ack())
            break;
    }
    
    send_data_to_FPGA(0xef);


}

//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
// 0->1, 1->2, 3->4,4->0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�

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
//�������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0, MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
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
//                   ���ز���ֱͨ��û�ز� CPLD �����ز�
//               -FASLE
//                  ֱͨ
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


// Զ�˹���������
// ����232�ź�(MCU)�е���y·Զ�ˡ�
// cRemoteUart - 0: ��ʾ�Ͽ�
//             - f: ��ʾ��������Զ�˹����Ĵ�������
void dvCpldUartRemote2McuSet(BYTE cRemoteUart)
{
    BYTE i;
    send_data_to_FPGA(0xed);//���ص�Զ��402
    for (i = 0; i < 2; i++)
    {
        send_data_to_FPGA(0xf0 | (cRemoteUart & 0x0f));
        if (!check_ack())
            break;
    }
    send_data_to_FPGA(0xef);
}


//
//ee    : ���ݰ���ͷ
//cRemote: ͨ����, ��1 ��ʼ
//bps_rate: �Ժ��ֲ����ʷ��ʹ������ݸ�Զ��, ��1��ʼ
//֧�ֲ�����2400��4800��9600��19200��38400��57600��115200��
//data_0
// ef:���ݰ�����
// this function test ok, excepte all port send
void dvCpldSendDataToRemote(BYTE cRemote, BYTE cbps_rate,  PBYTE pcData,BYTE cDataLength)
{
    BYTE i;   
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(nnSYSTEM_SETTING_DATA);

    if((cbps_rate == 0)||(cRemote == 0))
	return;
		
    send_data_to_FPGA(0xEE); //֪ͨCPLD��Ϊ��־λ����ô���淢�����ݸ�PCʱ�Ͳ����ٷ���Զ��402��
    send_data_to_FPGA(cRemote); // cRemote + 1
    #if (MACHINETYPE == MUH4K862)
    send_data_to_FPGA(cbps_rate);
    #else
    send_data_to_FPGA(cbps_rate);//ycq
    #endif
#ifndef VHD4_CPLD
    for (i = 0; i < cDataLength; i++)
    {
        send_data_to_FPGA(*pcData++);
    }
#endif
   send_data_to_FPGA(0xEF);
   psSystemSettingData->acRemoteBaudRate[cRemote-1]=cbps_rate;
   u8RemoteBaudRate = cbps_rate;
   utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, TRUE);
#ifdef VHD4_CPLD
	   delay_nms(1);

	   for (i = 0; i < cDataLength; i++)
	   {
		   UART1_SendByte(*pcData++);
	   }
	   delay_nms(10); //��΢��CLPD һ�ᷢ����
	   send_data_to_FPGA(0xEE); //֪ͨCPLD��Ϊ��־λ����ô���淢�����ݸ�PCʱ�Ͳ����ٷ���Զ��402��
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

    if((cRemote == 0))
        return;
    send_data_to_FPGA(0xEE); //֪ͨCPLD��Ϊ��־λ����ô���淢�����ݸ�PCʱ�Ͳ����ٷ���Զ��402��
    send_data_to_FPGA(cRemote); // cRemote + 1
    #if (MACHINETYPE == MUH4K862)
    send_data_to_FPGA(psSystemSettingData->acRemoteBaudRate[cRemote-1]);   
    //send_data_to_FPGA(0x03);
    #else
    send_data_to_FPGA(psSystemSettingData->acRemoteBaudRate[cRemote-1]);  //ycq 
    #endif
    //send_data_to_FPGA(0x20);
    send_data_to_FPGA(0xEF);
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
}

// MCU �� DB9( PC ͨѶʱ�� �����������? ���ݲ��ᷢ��402?
//        ���ڷ��������ݺ�, �ָ�
//bConnect - 0: �Ͽ�Զ��
void dvCpldMcuUartConnectToRemote(BOOL bConnect)
{
    //BYTE i;
    if (!bConnect)
    {
        send_data_to_FPGA(0xEE); //֪ͨCPLD��Ϊ��־λ����ô���淢�����ݸ�PCʱ�Ͳ����ٷ���Զ��402��
    }
    else
    {
        send_data_to_FPGA(0xEF);
    }
   
}

void dvCpldClose(void)
{
#ifndef PRD_IR_OUT
     //send_data_to_FPGA(0xFB);   //��Զ�������//
      send_data_to_FPGA(0xFA);  // �Ͽ�Զ������
#endif
     dvCpldUartRemote2McuSet(0x0);


}


void dvCpldOpen(void)
{
#ifndef PRD_IR_OUT
     send_data_to_FPGA(0xFB);   //��Զ�������//
 #endif
     // send_data_to_FPGA(0xFA);  // �Ͽ�Զ������
    dvCpldUartRemote2McuSet(0x0f);


}

//#ifdef VHD4_CPLD
void dvCpldOpenMCUToDB9(BOOL bEnable)
{
	   send_data_to_FPGA(0xEE); //֪ͨCPLD��Ϊ��־λ����ô���淢�����ݸ�PCʱ�Ͳ����ٷ���Զ��402��
	   send_data_to_FPGA(0); // cRemote + 1
	   //send_data_to_FPGA(cbps_rate - 1);
	   send_data_to_FPGA(0xEF);
}

//#endif


void InitRemoteBaudrate(void)
{
    unsigned int j;
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    for (j = 0; j < MAX_OUT; j++)
    {
        psSystemSettingData->acRemoteBaudRate[j] = 3;
        u8RemoteBaudRate = 3;
        dvCpldRemoteBraundSet(j+1);
    }
#ifdef USE_FULL_ASSERT
utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, false);
#else
utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, true);
#endif
    
}

void dvCpld_RemoteCtrlMcuMode(BOOL bMode)
{
    unsigned char i;
    send_data_to_FPGA(0xEA);
    if(bMode == TRUE)
    {
        for(i=0;i<2;i++)
        {
            send_data_to_FPGA(0xD1);
            if (!check_ack())
                break;
        }
    }
    else
    {
        for(i=0;i<2;i++)
        {
            send_data_to_FPGA(0xD0);
            if (!check_ack())
                break;
        }
    }
    send_data_to_FPGA(0xEF);
}

void InitRemoteCtrlMcu(void)
{
    PSYSTEM_SETTING_DATA  psSystemSettingData;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);    
    dvCpld_RemoteCtrlMcuMode(psSystemSettingData->acRemoteCtrlMcuFlag); 
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
    delay_nms(10);
}

#ifdef ENABLE_DB9_BAUDRATE_SWITCH
void SetDB9Baudrate(unsigned char tBaudrate)
{
    switch(tBaudrate)
    {
        case 1: // 2400
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC1);
            send_data_to_FPGA(0xef);
            break;
        case 2: // 4800
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC2);
            send_data_to_FPGA(0xef);
            break;
        case 3: // 9600
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC3);
            send_data_to_FPGA(0xef);
            break;
        case 4: // 19200
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC4);
            send_data_to_FPGA(0xef);
            break;
        case 5: // 38400
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC5);
            send_data_to_FPGA(0xef);
            break;
        case 6: // 57600
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC6);
            send_data_to_FPGA(0xef);
            break;
        case 7: // 115200
            send_data_to_FPGA(0xea);
            send_data_to_FPGA(0xC7);
            send_data_to_FPGA(0xef);
            break;
        default:
            //send_data_to_FPGA(0xea);
            //send_data_to_FPGA(0xC0);
            //send_data_to_FPGA(0xef);
            break;
    }
}

void InitDB9Baudrate(void)
{
    PSYSTEM_SETTING_DATA  psSystemSettingData;
    unsigned char tBaud=0;
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    tBaud=psSystemSettingData->PcUartBaudrateSelect;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA, FALSE);
    SetDB9Baudrate(tBaud);
    delay_nms(10);
}
#endif

