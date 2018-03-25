#include "si_common.h"
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "appSystemMgrAPI.h"
#include "util.h"
#include "si_drv_adapter_rx.h"
#include "si_drv_adapter_tx.h"
#include "appUserInterface.h"
#include "dvADN4604.h"
#include "dvCPLD.h"
#include "dvNVRAM.h"
#include "Si_edid_tx_component.h"
#include "Si_app_adapter_rx.h"
#include "Sk_avr_repeater.h"
#include "Si_timer.h"
#include "appUserInterface.h"
#include "si_tx_component.h"
#include "si_drv_tpi_system.h"
#include "Uart.h"
#include "KeyTasks.h"

#define Timing1   54
#define Timing2    72
#define Timing3   90
#define Timing4    108
#define Checksum    127
#define NumberOffset   130
#define GeneralFormat   132
#define PRD_EDID_NAME "____"
extern char LCD1_IDLE_TEXT[17];
extern char LCD2_IDLE_TEXT[17];
extern char m_acProduct_Type[17];
extern char m_acProduct_Name[17];
//uint8_t first_run=1;
extern uint8_t g_Init_EdidFlashEdidTable[];
extern BYTE   u8IChannellock[MAX_OUT];
extern BOOL  System_UnLock_flag;

BYTE m_acLastIRInPortSelect[MAX_OUT]=
{
    0xff,
	#if(MAX_OUT>1)
    0xff,
	#endif
    #if(MAX_OUT>2)
    0xff,
    #endif
    #if(MAX_OUT>3)
    0xff,
	#endif
	#if(MAX_OUT>4)
    0xff,
	#endif
	#if(MAX_OUT>5)
    0xff,
	#endif
	#if(MAX_OUT>6)
    0xff,
	#endif
	#if(MAX_OUT>7)
    0xff,
	#endif
	
}; // ����ɵ��л���ϵ


// True: will be in demo mode

void appSystemSettingNodeCallback(eNODE_NAME eNodeID,
                                  eNV_EVENT  eEvent)
{
   BYTE i, j;
   switch (eEvent)
   {
   case nveRESETALL:
   case nveINITIALIZE:
      switch (eNodeID)
      {
/* -------------- BEGIN CUSTOM AREA [420] palWindowMgrPop.c ------------------
 * Add parameter nodes used in the system here. */
// Avoid defining local variable here.   (Especically, no assignment command follows!!! )
      case nnSYSTEM_SETTING_DATA:
         {
            PSYSTEM_SETTING_DATA         psSystemSettingData;
            psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrWriteLockData(eNodeID);
            memset(psSystemSettingData, 0, sizeof(SYSTEM_SETTING_DATA)); // Fill all zero the struct
            //psSystemSettingData->acEDID_EEPROM_BOOT_VALID[INSTANCE_9533]=; //SiiDrvDeviceNonVolatileInitialize
            //psSystemSettingData->cPowerStatus;
            if (nveINITIALIZE == eEvent)
            {
               memcpy(psSystemSettingData->acProduct_Type, PRD_TYPE, strlen(PRD_TYPE));
               memcpy(psSystemSettingData->acProduct_Name, PRD_NAME, strlen(PRD_NAME));
            }
            else if (nveRESETALL == eEvent)
            {
               memcpy(psSystemSettingData->acProduct_Type, m_acProduct_Type, strlen(m_acProduct_Type));
               memcpy(psSystemSettingData->acProduct_Name, m_acProduct_Name, strlen(m_acProduct_Name));
            }
            psSystemSettingData->acIP_Address[0] = 192;    // IP_add
            psSystemSettingData->acIP_Address[1] = 168;
            psSystemSettingData->acIP_Address[2] = 1;
            psSystemSettingData->acIP_Address[3] = 13;
            psSystemSettingData->bNVARMEDID_Inited = TRUE;
            psSystemSettingData->bUartReturnMessage_EN = TRUE;           // Re_Message //ϵͳ��Ϣ����״̬��?
            psSystemSettingData->bSystemLock = FALSE;                     // System  //ϵͳ����״̬����
            System_UnLock_flag = psSystemSettingData->bSystemLock;
            for (i = 0; i < MAX_IN; i++)
            {
                psSystemSettingData->acIChannellock[i] = 0;           //��Ӧ����ͨ������
                u8IChannellock[i]=psSystemSettingData->acIChannellock[i];
            }
            psSystemSettingData->cIR_Carrier_Mode = 0;                // IR_carrier_mode  1:�����ز�ǿ�� 0:�����ز�����
            for (i = 0; i < MAX_OUT; i++)
            {
                #ifdef PRD_POWER_ONOFF_MESSAGE
                psSystemSettingData->acPowerOnMsgBaudRate[i] = 0;    //PWON_baud
                memset(psSystemSettingData->acPowerOnMsg[i], 0, PWON_OFF_Lenth);
                psSystemSettingData->acPowerOffMsgBaudRate[i] = 0;         //PWOFF_baud
                memset(psSystemSettingData->acPowerOffMsg[i], 0, PWON_OFF_Lenth);
//                 psSystemSettingData->acIRMatrix[MAX_OUT];                   // IR_before_close
                #endif
                /*
                ֧�ֲ�����2400��4800��9600��19200��38400��57600��115200��
                */
                psSystemSettingData->acRemoteBaudRate[i] = 3;
                //psSystemSettingData->bHDCP_AUTO[i] = TRUE;    // HDCP_AUTO_FLAG
            }
            u8RemoteBaudRate = 3;
            psSystemSettingData->cDebugLevel = 0x00;
            memcpy(psSystemSettingData->acDefaultEdidProductName, PRD_EDID_NAME, strlen(PRD_EDID_NAME)); // ����s�ĳ��ȣ�������������NULL
           // memcpy(psSystemSettingData->acDefaultEdidProductName, PRD_EDID_NAME, strlen(PRD_EDID_NAME)); // ����s�ĳ��ȣ�������������NULL
                                                                                                         //psSystemSettingData->acLCDLine1DefaultTXT[16];              // LCD1_UPdata
                                                                                                         //psSystemSettingData->acLCDLine2DefaultTXT[16];              // LCD2_UPdata
            if (nveINITIALIZE == eEvent)
            {
               memcpy(psSystemSettingData->acLCDLine1DefaultTXT, PRD_TYPE, strlen(PRD_TYPE)); // ����s�ĳ��ȣ�������������NULL
               memcpy(psSystemSettingData->acLCDLine2DefaultTXT, PRD_NAME, strlen(PRD_NAME)); // ����s�ĳ��ȣ�������������NULL
            }
            else if (nveRESETALL == eEvent)
            {
               memcpy(psSystemSettingData->acLCDLine1DefaultTXT, LCD1_IDLE_TEXT, strlen(LCD1_IDLE_TEXT));
               memcpy(psSystemSettingData->acLCDLine2DefaultTXT, LCD2_IDLE_TEXT, strlen(LCD2_IDLE_TEXT));
            }
            #ifdef ENABLE_DB9_BAUDRATE_SWITCH
            psSystemSettingData->PcUartBaudrateSelect = 3;
            #endif
            psSystemSettingData->acRemoteCtrlMcuFlag = FALSE;
            utilDataMgrUnlock(eNodeID,
                              TRUE);
            // if(nveINITIALIZE == eEvent) //[kyq addapp: 20150907 17:16:05]
            //  dvNVRAMUserEdidInit(g_Init_EdidFlashEdidTable); // TO DO
            //
            dvNVRAMInputPortEdidInit(g_Init_EdidFlashEdidTable); //[kyq edid: 20150908 12:36:56]
         }
         break;
      case nnOUTPUTPORT_MANAGER_DATA:
         {
            POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
            psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(eNodeID);
            for (i = 0; i < MAX_OUT; i++)
            {
               psOutputPortMgrData->acInPortSelect[i] = i;
               psOutputPortMgrData->acCloseInPortSelect[i] = 0xff;
               psOutputPortMgrData->abHDMIAudio_EN[i] = TRUE;
               psOutputPortMgrData->abHDCP_EN[i] = TRUE;
               psOutputPortMgrData->abHDCP_AUTO[i] = TRUE;
#ifdef PRD_IR_OUT
               psOutputPortMgrData->acIRInPortSelect[i] = i; // from remote IR input to local IR out
               psOutputPortMgrData->acIRCloseInPortSelect[i] = 0xff;
#endif
            #if ((MACHINETYPE == MUH4K862)||(MACHINETYPE==MHUB4K431))
            KeyLedCloseSetOne(i,psOutputPortMgrData->acCloseInPortSelect[i]);
            Set_CloseOutputFlag(i,false);
            #endif
            }
            for (j = 0; j < SCENCES_MAX; j++)
            {
               for (i = 0; i < MAX_OUT; i++)
               {
                  psOutputPortMgrData->acScenceInPortSelect[j][i] = MAX_IN; //0xff;
#ifdef PRD_IR_OUT
                  psOutputPortMgrData->acScenceIRInPortSelect[j][i] = MAX_IN; // from remote IR input to local IR out
#endif
               }
            }
            utilDataMgrUnlock(eNodeID,
                              TRUE);
         }
         break;
      case nnINPUTPORT_MAMAGER_DATA:
         {
            PINPUTPORT_MANAGER_DATA    psInputPortMgrData;
            psInputPortMgrData  = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(eNodeID);
            for (i = 0; i < MAX_IN; i++)
            {
               psInputPortMgrData->abInputHDCP_EN[i] = TRUE; // HDCP_IN_EN
               psInputPortMgrData->acEdidSelect[i] = 0;
            }
         }
         utilDataMgrUnlock(eNodeID,
                           TRUE);
         break;
      default:    // unsupported data node
         ASSERT_ALWAYS();
/* ---------------------------- END CUSTOM AREA --------------------------- */
      }
      break;
   default:                      // unsupported event
      ASSERT_ALWAYS();
   }
}

/*
void appSystemSwitchProcessStar(void)
{
	m_eSystemState = essSWITCH_START;
}
void appSystemHDCPProcessStar(void)
{
	m_eSystemState = essSWITCH_START;
}*/
#ifdef PRD_IR_OUT
// ע�������INPUT ��ָIR OUT 1~4
//�л�Զ�˺��������������IR_OUT �˿�
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherSet(BYTE cInput, BYTE cPortNumber, ...)
{
    va_list         args;
    va_list         args2;
    BYTE i;
    BYTE cOutPort;
    // Start the varargs processing.
    va_start(args, cPortNumber);
    va_start(args2, cPortNumber);  //[kyq edid: 20150820 14:42:20]
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
   memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = va_arg(args, BYTE);
        assert_param(cOutPort < MAX_OUT);
        m_acLastIRInPortSelect[cOutPort] = psOutputPortMgrData->acIRInPortSelect[cOutPort];
        psOutputPortMgrData->acIRInPortSelect[cOutPort] = cInput;
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    dvCPLDIRSwitcherSet(cInput, cPortNumber, args2);
    va_end(args);
    va_end(args2);
}
// ע�������INPUT ��ָIR OUT 1~4
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0,MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList)
{
    BYTE cOutPort;
    BYTE i;
    // Start the varargs processing.
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    memset(m_acLastIRInPortSelect, 0xff, MAX_OUT); //[kyq Begin: 20150924 16:04:02] // clear first
    for (i = 0; i < cPortNumber; i++)
    {
        cOutPort = cOutPortList[i];
        assert_param(cOutPort < MAX_OUT);
        m_acLastIRInPortSelect[cOutPort] = psOutputPortMgrData->acIRInPortSelect[cOutPort];
        psOutputPortMgrData->acIRInPortSelect[cOutPort] = cInput;
        //psOutputPortMgrData->abHDMIAudio_EN[i] = TRUE;
        //psOutputPortMgrData->abHDCP_EN[i] = TRUE;
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    //dvADN4604SwitcherPortSet(cInput, cPortNumber, cOutPortList);
     dvCPLDIRSwitcherPortSet(cInput, cPortNumber, cOutPortList);
}
void appSystemIRSwitcherSetStart(void)
{
    BYTE cInput, cInSel, cOut;
    BYTE cCNT = 0;
    BYTE cOutList[MAX_OUT];
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    for (cInput = 0; cInput <= MAX_IN; cInput++)  // add MAX_IN , for mute function
    {
        cCNT = 0;
        for (cOut = 0; cOut < MAX_OUT; cOut++)
        {
            cInSel = psOutputPortMgrData->acIRInPortSelect[cOut];
            if (cInput == cInSel)
            {
                //m_acLastInPortSelect[cOut]=cInput;
                cOutList[cCNT] = cOut;
                cCNT++;
            }
        }
        if (cCNT>0)
        {
           // dvADN4604SwitcherPortSet( cInput, cCNT, cOutList);
           dvCPLDIRSwitcherPortSet(cInput, cCNT, cOutList);
        }
        
    }
	utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
}
#endif
/*----------------------------------------------18Bytes  Data Timing Processing------------------------------------------*/
void EDIDDataProcess(BYTE *TheData)
{	
	BYTE DataHighTemp = 0 , DataLow = 0, DataHigh= 0 ;
	BYTE HzTemp= 0;
	unsigned short int  DataGatherTemp = 0 ;
	BYTE DataLowTemp = 0;
	BYTE HighFour = 0;
	BYTE LowFour = 0;
	unsigned short int Data16 = 0;
	unsigned short int DataV16 = 0;
	unsigned int Data32 = 0;
	unsigned int DataM32 = 0;
	
	DataLowTemp  = *(TheData+2) ;   //18BYTES �� �׵�ַ ���� 36H
	DataLow= DataLowTemp&0x0f;     
	DataLowTemp  = *(TheData+2);
	DataHigh= (DataLowTemp&0xf0)>>4;     //ȡgao 4
	
	DataHighTemp = *(TheData+4);    
		
	HighFour = (DataHighTemp&0xf0) >> 4;   //ȡ��4λ 
	LowFour = (DataHighTemp&0x0f);    //ȡ��4λ
	DataGatherTemp |= HighFour;    //H - displaying
	DataGatherTemp = DataGatherTemp<<8;
	DataGatherTemp |=*(TheData+2);
	if(DataGatherTemp >= 3840)
		{
			Data16 |=LowFour ;//H-blanking
			Data16 = Data16<<8;
			Data16  |= *(TheData+3);
			Data32 |= *(TheData+1);  // Plck clock  Hz
			Data32 = Data32<<8;
			Data32 |= *TheData;    
			//count    H- tatol  =  H-displaying  +  H-blanking
			Data16 = Data16 + DataGatherTemp;	 		
			 //V - display
			DataGatherTemp  = 0;                                              
			DataGatherTemp |=  (*(TheData +7)&0xf0);
			DataGatherTemp = DataGatherTemp<<4;
			DataGatherTemp |=*(TheData +5 );
			 // v-blanking
			DataV16 |= (*(TheData +7)&0x0f);                        
			DataV16 = DataV16<<8;
			DataV16 |=*(TheData +6 ); 
			//count    V-tatol   =  V-displaying  +  V-blanking
			DataV16 += DataGatherTemp;                                 
			//      H-tatol  *  V-tatol 
			DataM32 = DataV16*Data16;  
			//FrequencyHz = Plck Clock /(H-tatol * V-tatol)
			HzTemp = Data32*10000/DataM32;  //eg: 594000000 /(4400*2250)
			
			 if(HzTemp >= 50)
				{	
					if(HzTemp == 60)
						{
							HzTemp = 30;
							Data32= (DataM32*HzTemp)/10000;
							*TheData = Data32&0x000000ff;
							*(TheData+1) = (Data32&0x0000ff00)>>8;  //charge  bandwidth  value
						}
					else if((HzTemp >= 50)&&(HzTemp<60))
						{
							HzTemp = 25;
							Data32= (DataM32*HzTemp)/10000;
							*TheData = Data32&0x000000ff;
							*(TheData+1) = (Data32&0x0000ff00)>>8;  //charge  bandwidth  value
						}
				}
		}
	
		
}
 //����˿ڵ�EDID ѡ��
// ��NVRAM ��ȡָ����EDID����д������оƬ�У����浽����ڵ�NVRAM EDID ��
// ���ߴ��ⲿ����ڶ�ȡEDID д��9679 �У����浽����ڵ�NVRAM EDID ��
//  cEdidSelect: 
//                     edsEDID_INTERN1~edsEDID_INTERN5, ��ȡNVRAM �е��ڲ�EDID.
//                    edsEDID_EXTERN1 ~edsEDID_EXTERN4 ��ȡ�ⲿ�ӿ�EDID,
//  cInputPort - ��0 ��ʼ��
void appSystemInputPortEdidSet(BYTE cInputPort,eEDIDSELECT cEdidSelect)
{
    BOOL                        bNVRAMEdidValid;
    //BYTE                         cInputEdidSelect = 0;
    WORD                         i = 0;
    PSYSTEM_SETTING_DATA         psSystemSettingData;
    PINPUTPORT_MANAGER_DATA      psInputPortMgrData;
    BYTE   acEdid[256];
    unsigned short int   CountEdid;   //wyl
	BYTE   NumTemp ; //wyl
	 unsigned short int  Value ; //wyl
	BYTE  ValueTemp= 0; //wyl
	BYTE  GeneralValue = 0; //wyl
	BYTE  GeneralValueHigh = 0; //wyl
	BYTE  GeneralValueLow = 0; //wyl
	unsigned short int SynthesizeValue = 0; //wyl
	BYTE  GeneralXunhuan = 0; //wyl
	BYTE  DetailTiming = 5 ; //wyl
	BYTE checksumFlag = 0; //wyl
    ASSERT(cInputPort < MAX_IN);
    ASSERT(cEdidSelect < eedsINVALID);
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    bNVRAMEdidValid = psSystemSettingData->bNVARMEDID_Inited;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      FALSE);
    psInputPortMgrData  = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
    psInputPortMgrData->acEdidSelect[cInputPort] = cEdidSelect;
    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      TRUE);
    if (cEdidSelect < edsEDID_EXTERN1)
    {
        // �ڲ�EDID
        if (bNVRAMEdidValid)
        {
            dvNVRAMUserEdidRead(cEdidSelect, acEdid);
        }
    }
    else
    {
        // ѡ���ⲿEDID
        // ���ѡ���ⲿEDID, �������HPD �仯ʱ����Ҫ���¶�ȡ
        SiiTxEdidGetEdidData(utilOutputPortToInstance(cEdidSelect - edsEDID_EXTERN1), acEdid);
    }
    if ((acEdid[0] != 0x00) || (acEdid[1] != 0xff) || (acEdid[7] != 0x00))
    {
        memcpy(acEdid, g_Init_EdidFlashEdidTable, 256);
    }
    #if 1//!((MACHINETYPE==MUH4K862)/*||(MACHINETYPE==MHUB4K431)*/)
// HDMI2.0��ΪHDMI1.3
    for( CountEdid = 0 ; CountEdid < 256 ;  CountEdid++)
   	{
   		NumTemp=CountEdid ; 
		 switch(NumTemp)
		 	{
				case Timing1:	
							if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFC))
								{
								 if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFD))
								 	{
										EDIDDataProcess(&acEdid[CountEdid]);
										checksumFlag = 1;
								 	}
								}
							break;
				case Timing2:
							if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFC))
								{
				
								if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFD))
									{
										EDIDDataProcess(&acEdid[CountEdid]);
										checksumFlag = 1;
									}
								}
							break;
				case Timing3:
							if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFC))
								{
								if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFD))
									{
										EDIDDataProcess(&acEdid[CountEdid]);
										checksumFlag = 1;
									}
								}
							break;
				case Timing4:
							if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFC))
								{
								       if(((acEdid[CountEdid]+acEdid[CountEdid+1]+acEdid[CountEdid+2])!=0)&&(acEdid[CountEdid+3]!=0xFD))
									{
										EDIDDataProcess(&acEdid[CountEdid]);
										checksumFlag = 1;
									}
								}
							break;
				case Checksum:	
							#if 0
							if(checksumFlag == 1)	
								{
									    for(Value = 0; Value < 127; Value++)  //У׼7fH Ϊ0
										{
											SynthesizeValue += acEdid[Value];
										}
										ValueTemp = SynthesizeValue&0x00ff;
										SynthesizeValue = 256;
										acEdid[127] = SynthesizeValue - ValueTemp;
										Value = 0;
										SynthesizeValue =0;
										ValueTemp = 0;
										checksumFlag = 0;
								}
							#endif
							for (i = 0; i < 127; i++)  ValueTemp += acEdid[i];
                                		acEdid[127] = ~(ValueTemp - 1);
							break;
			
				default:            break;
		 	}
  	
		 //�Ժ�128byte�Ĵ���
		
		 if(CountEdid ==NumberOffset )    //eg:82       ����18bytes Detailed Timing Descriptions�Ŀ�ʼ�ֽڵ�ַ
		 	{
		 		Value  = acEdid[CountEdid] + 128;
			}
		 if(CountEdid==GeneralFormat )
		 	{
				GeneralValue= acEdid[CountEdid] ;
				GeneralValueHigh = (GeneralValue & 0xE0)>>5;
				GeneralValueLow =  GeneralValue & 0x1F;
				if(GeneralValueHigh == 2)
					{
						for(GeneralXunhuan = 0; GeneralXunhuan <GeneralValueLow; GeneralXunhuan ++)
							{
								if(((acEdid[CountEdid + 1]==96)&&(acEdid[CountEdid + 1]==97))||(acEdid[CountEdid + 1]==101)||(acEdid[CountEdid + 1]==102)||(acEdid[CountEdid + 1]==106)||(acEdid[CountEdid + 1]==107))
									{
										acEdid[CountEdid + 1] = acEdid[CountEdid + 1] - 2; //���¸�ֵ ��5��ʼ�ۼ�
										checksumFlag = 1; 
									}
								CountEdid+=1;
								DetailTiming+=1;
							}
					}
				
		 	}
		 if(CountEdid ==Value)  //�Ժ�128byte�еķֱ��ʽ����ж�
		 	{
		 		while(Value<255)   
		 		{	
		 			
					if(((acEdid[Value]+acEdid[Value+1]+acEdid[Value+2])!=0)&&(acEdid[Value+3]!=0xFC))
					{
					       if(((acEdid[Value]+acEdid[Value+1]+acEdid[Value+2])!=0)&&(acEdid[Value+3]!=0xFD))
						{
							EDIDDataProcess(&acEdid[Value]);
							checksumFlag = 1;
						}
					}
					Value =Value+18; //��18bytes
		 		}
				
		 	}  
		 if(CountEdid == 255)   //�Ժ���128���ֽڽ���Checksum  ��ʽ��ǰ127byteһ��
		 	{
				#if 0
				if(checksumFlag == 1)
		 	     	{
					for(GeneralXunhuan = 0; GeneralXunhuan <128; GeneralXunhuan ++)
						{
							SynthesizeValue += acEdid[GeneralXunhuan+128];
						}
						ValueTemp = SynthesizeValue&0x00ff;
						SynthesizeValue = 256;
						acEdid[255] = SynthesizeValue - ValueTemp;
		 	     	}
				#endif
				ValueTemp =0;
				for (i = 0; i < 127; i++)  ValueTemp += acEdid[i+128];
                         acEdid[255] = ~(ValueTemp - 1);
		 	} 
   	}
    #endif
    #if 0
    for( i = 0 ; i < 256 ;  i++)
    {
		UART1_SendByte(acEdid[i]);
    }
    #endif
    dvNVRAMUserEdidWrite(edsEDID_IN1 + cInputPort, acEdid);
    dvSiiDrvAdaptRxHpdSet(cInputPort, SII_DRV_ADAPT_INP_CTRL__DISABLE);
    dvSiiDrvAdaptRxEdidSramWrite(cInputPort, 0, acEdid, 256);
    dvSiiDrvAdaptRxHpdSet(cInputPort, SII_DRV_ADAPT_INP_CTRL__ENABLE);     //�����Ȱβ��
}
//
//  ��EDID ���ݱ��浽NVRAM, �������õ�����оƬ��, ���ұ��浽��Ӧ����ڵ�EDID NVRAM ��
//  ����˿ڵ�EDID ѡ��
//  cEdidSelect:  - û����
//  pcEdidData:  - ָ��EDID ����
void appSystemInputPortEdidDataSet(BYTE cInputPort,eEDIDSELECT cEdidSelect,PCBYTE pcEdidData)
{
    BOOL                        bNVRAMEdidValid;
    //BYTE                         cInputEdidSelect = 0;
    PSYSTEM_SETTING_DATA         psSystemSettingData;     
    PINPUTPORT_MANAGER_DATA      psInputPortMgrData;
    //BYTE                         acEdid[256];
    ASSERT(cInputPort < MAX_IN);
    ASSERT(cEdidSelect < eedsINVALID);
    psSystemSettingData = (PSYSTEM_SETTING_DATA)utilDataMgrReadLockData(nnSYSTEM_SETTING_DATA);
    bNVRAMEdidValid = psSystemSettingData->bNVARMEDID_Inited;
    utilDataMgrUnlock(nnSYSTEM_SETTING_DATA,
                      FALSE);
    psInputPortMgrData  = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
    psInputPortMgrData->acEdidSelect[cInputPort] = cEdidSelect;
    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      TRUE);
   dvNVRAMUserEdidWrite(edsEDID_IN1+cInputPort, (PBYTE)pcEdidData);
	
/*
    if (cEdidSelect<edsEDID_EXTERN1)
    {
    // �ڲ�EDID
        if (bNVRAMEdidValid)
        {
            
            dvNVRAMUserEdidRead(cInputEdidSelect, acEdid);
        }
    }
    else
    {
    // ѡ���ⲿEDID
    // ���ѡ���ⲿEDID, �������HPD �仯ʱ����Ҫ���¶�ȡ
           SiiTxEdidGetEdidData(utilOutputPortToInstance( cEdidSelect-edsEDID_EXTERN1)  ,acEdid);
    }
*/	
    dvSiiDrvAdaptRxHpdSet(cInputPort, SII_DRV_ADAPT_INP_CTRL__DISABLE);
    dvSiiDrvAdaptRxEdidSramWrite(cInputPort, 0, (PBYTE)pcEdidData, 256);
    dvSiiDrvAdaptRxHpdSet(cInputPort, SII_DRV_ADAPT_INP_CTRL__ENABLE);     //�����Ȱβ��
    
}
eEDIDSELECT appSystemInputPortEdidGet(BYTE cInputPort)
{
    eEDIDSELECT eEdidSelect;
    PINPUTPORT_MANAGER_DATA      psInputPortMgrData;
    psInputPortMgrData  = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
    eEdidSelect = (eEDIDSELECT)psInputPortMgrData->acEdidSelect[cInputPort];
    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      FALSE);
    return eEdidSelect;
}
// ��������˿ڵ�EDID ����
void appSystemInputPortEdidDataGet(BYTE cInputPort,PBYTE pEdidData)
{
    
     dvNVRAMUserEdidRead(edsEDID_IN1+cInputPort, pEdidData);
}

void appInputHDCPCompatible(void)
{         BYTE i;
        PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
     psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
    for (i = 0; i < MAX_IN; i++)
    {
        BOOL bCurrentHDCP = dvSiiDrvAdaptRxHDCPGet(i);
        if (psInputPortMgrData->abInputHDCP_EN[i])
        {
            // �û�����֧��HDCP ����
            if (dvSiiDrvAdaptTxTmdsOutEnGet(i) == FALSE)
            {
                dvSiiDrvAdaptTxTmdsOutSet(i, SII_DRV_ADAPT_TMDS_OUT__ENABLE);
//[kyq Begin: 20151028 11:56:06]
                //appSystemSwitcherPortMuteSet(i,FALSE);
//[kyq End: 20151028 11:56:08]						
            }
        }
        else
        {
            // ֧��HDCP
            if (bCurrentHDCP)
            {
                // ��ǰ�Ǵ�HDCP, �ص����
                if (dvSiiDrvAdaptTxTmdsOutEnGet(i) == TRUE)
                    dvSiiDrvAdaptTxTmdsOutSet(i, SII_DRV_ADAPT_TMDS_OUT__DISABLE);
            	}
	    else
	    	{
	    	     // ��ǰ�ǲ�����HDCP, ǿ�����
	         if (dvSiiDrvAdaptTxTmdsOutEnGet(i) == FALSE)
                    dvSiiDrvAdaptTxTmdsOutSet(i, SII_DRV_ADAPT_TMDS_OUT__ENABLE);
	    	}	
   
        }
    }
    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      FALSE);


}

void appInputHDCPCheck(void)
{     BYTE cInPort;
         PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
    psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
    for (cInPort = 0;cInPort < MAX_IN; cInPort++)
        psInputPortMgrData->InputHDCP[cInPort] = dvSiiDrvAdaptRxHDCPGet(cInPort);
  	  utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      TRUE);


}

void appClearInputHDCP(void)
{   
      BYTE cInPort;
	         PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
    psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnINPUTPORT_MAMAGER_DATA);
     for (cInPort = 0;cInPort < MAX_IN; cInPort++)
        if(dvSiiDrvAdaptRxConnectGet(cInPort))
               psInputPortMgrData->InputHDCP[cInPort]=0;
		
     utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      TRUE);
}

void appOutputHDCPProcess(void)
{
    POUTPUTPORT_MANAGER_DATA     psOutputPortMgrData;
	  PINPUTPORT_MANAGER_DATA       psInputPortMgrData;
    BYTE cInPort = 0;
    BOOL bInHDCPEN;
    BOOL bOutHDCPEN;
    BOOL bHDCP_AUTO;
    BOOL bUserOutHDCPEN;
    BYTE i;
 
    psOutputPortMgrData = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);
    psInputPortMgrData = (PINPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnINPUTPORT_MAMAGER_DATA);
    for (i = 0; i < MAX_OUT; i++)
    {
	bHDCP_AUTO = psOutputPortMgrData->abHDCP_AUTO[i];
	cInPort =  psOutputPortMgrData->acInPortSelect[i];
	bInHDCPEN = dvSiiDrvAdaptRxHDCPGet(cInPort);//  huang 20161114
       bOutHDCPEN = dvSii9533TXHDCPGet(utilOutputPortToInstance(i));
       bUserOutHDCPEN = psOutputPortMgrData->abHDCP_EN[i];
        if (bHDCP_AUTO) // �Զ�����ģʽ
        {
            if (bInHDCPEN != bOutHDCPEN)
            {
                //DEBUG_PRINT(MSG_ALWAYS, "dvSii9533TXHDCPSetStart=%d\r\n",bInHDCPEN);
    //    #if (MACHINETYPE==MUH44F)
                if (i == 2||i==3)
                {
                    dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i), bInHDCPEN);
                    dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i) + 1, bInHDCPEN);
                }
                else
      //  #endif
                    dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i), bInHDCPEN);
            }
        }
        else if (bUserOutHDCPEN != bOutHDCPEN) // �ֶ�ģʽ
        {
        #if (MACHINETYPE==MUH44F)
            if (i == 0)
            {
                dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i), bUserOutHDCPEN);
                dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i) + 1, bUserOutHDCPEN);
            }
            else
        #endif
                dvSii9533TXHDCPSetStart(utilOutputPortToInstance(i), bUserOutHDCPEN);
        }
    }
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
	    utilDataMgrUnlock(nnINPUTPORT_MAMAGER_DATA,
                      FALSE);
}
// HDMI �������أ����ұ���״̬��NVRAM,���������̫��ѧ
void appSystemHDMITXAudioOutEnableSet(BYTE cInstance, BOOL bEnable)
{
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrWriteLockData(nnOUTPUTPORT_MANAGER_DATA);
    psOutputPortMgrData->abHDMIAudio_EN[cInstance]=bEnable;
    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      TRUE);
    dvSii9533TXAudioMixPassThroughConfig(cInstance,bEnable,bEnable);
}

