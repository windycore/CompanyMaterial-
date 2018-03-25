// file :appSystemMgrAPI.h
#ifndef _APPSYSTEMMGR_H_
#define _APPSYSTEMMGR_H_
#include "mcu.h"
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "util.h"

#define PRD_TYPE  "HDBaseT Matrix"
#if (MACHINETYPE==MUH44TPR2_N)
#define PRD_NAME "MUH44TPR2_N"
#elif (MACHINETYPE==MUH88E_BYE)
#define PRD_NAME "MUH88E"
#elif(MACHINETYPE==PRD_VHD4)
#define PRD_NAME "VHD-4"
#elif(MACHINETYPE==MUH44E_BYE)
#define PRD_NAME "MUH44E"
#elif (MACHINETYPE==MUH4K862)
#define PRD_NAME "MHUB4K862"
#elif(MACHINETYPE==MHUB4K431)
#define PRD_NAME "MHUB4K431"

#else
#define PRD_NAME "VHD-4"

#endif

//extern uint8_t first_run;


typedef struct
{
/*
BYTE  cCONFIG_VALID;
BYTE  cEDID_LOADSEL;
BYTE  cDB_VERSION;
BYTE  cUART_MSG_LEVEL;
BYTE  cEDID_VALID;
*/
    BYTE  acEDID_EEPROM_BOOT_VALID[DevNum];
//BYTE  cPowerStatus;
    BYTE  acProduct_Type[17]; // sys_type
    BYTE  acProduct_Name[17]; // sys_name
    BYTE  acIP_Address[4];    // IP_add
//BOOL bHDCP_AUTO[MAX_OUT];    // HDCP_AUTO_FLAG; TRUE: Following input , FALSE: Setting by user
    BOOL bNVARMEDID_Inited; // the user edid is inited??

    BOOL  bUartReturnMessage_EN;           // Re_Message //ϵͳ��Ϣ����״̬��?
    BOOL  bSystemLock;                     // System  //ϵͳ����״̬����, true: lock, false; unlock
    BYTE  cIR_Carrier_Mode;                // IR_carrier_mode  1:�����ز�ǿ�� 0:�����ز�����
#ifdef PRD_POWER_ONOFF_MESSAGE
    BYTE  acPowerOnMsgBaudRate[MAX_OUT];    //PWON_baud
    BYTE  acPowerOnMsgLength[MAX_OUT];
    BYTE  acPowerOnMsg[MAX_OUT][PWON_OFF_Lenth]; // PWON_rBuff

    BYTE  acPowerOffMsgBaudRate[MAX_OUT];         //PWOFF_baud
    BYTE  acPowerOffMsgLength[MAX_OUT];
    BYTE  acPowerOffMsg[MAX_OUT][PWON_OFF_Lenth]; // PWOFF_rBuff
#endif
    BYTE  acRemoteBaudRate[MAX_OUT];   //[kyq add: 20150901 9:47:08]
    BYTE  acDefaultEdidProductName[4];           // EDID_name
//BYTE  acIRMatrix[MAX_OUT];                   // IR_before_close
    BYTE  cDebugLevel;
    BYTE  cPOSTEnable;     //The first step of the BIOS is the power-on self test 
    BYTE   acIChannellock[MAX_OUT];//xfd add:20160316 for 44E     // 1  ��ͨ������
    BYTE  acLCDLine1DefaultTXT[17];              // LCD1_UPdata
    BYTE  acLCDLine2DefaultTXT[17];              // LCD2_UPdata
    #ifdef ENABLE_DB9_BAUDRATE_SWITCH
    BYTE  PcUartBaudrateSelect;                 // DB9 baudrate
    #endif
    BOOL  acRemoteCtrlMcuFlag;                 // DB9 baudrate
} SYSTEM_SETTING_DATA,*PSYSTEM_SETTING_DATA;  // psSystemSettingData


typedef struct
{
    BYTE  acEdidSelect[MAX_IN];                     // ѡ���ĸ�EDID�� Ŀǰû����
    BOOL  abInputHDCP_EN[MAX_IN]; // HDCP_IN_EN
    BOOL  InputHDCP[MAX_IN]; // check input HDCP
}INPUTPORT_MANAGER_DATA,*PINPUTPORT_MANAGER_DATA;  // psInputPortMgrData



typedef struct
{
    BYTE  acInPortSelect[MAX_OUT];             // vedio_before_close  , ��ǰ�л�״̬
    BYTE  acCloseInPortSelect[MAX_OUT]; //   ����ر�֮ǰ��״̬
    BOOL  abHDMIAudio_EN[MAX_OUT]; // digit_audio
    BOOL  abHDCP_EN[MAX_OUT]; // HDCP_IN_EN
    BOOL  abHDCP_AUTO[MAX_OUT];
    BYTE  acScenceInPortSelect[SCENCES_MAX][MAX_OUT];             // ���������л���ϵ
#ifdef PRD_IR_OUT
    BYTE  acIRCloseInPortSelect[MAX_OUT]; //   ����ر�֮ǰ��״̬
    BYTE  acIRInPortSelect[MAX_OUT]; // from remote IR input to local IR out
    BYTE  acScenceIRInPortSelect[SCENCES_MAX][MAX_OUT]; 
	
#endif

}OUTPUTPORT_MANAGER_DATA,*POUTPUTPORT_MANAGER_DATA;  // psOutputPortMgrData


void appSystemSettingNodeCallback(eNODE_NAME eNodeID,
                                  eNV_EVENT  eEvent);


void EDIDDataProcess(BYTE *TheData);  // wyl

void appOutputHDCPProcess(void);
void appInputHDCPCompatible(void);
void appInputHDCPCheck(void);
void appClearInputHDCP(void);
// ��������ڵ�EDID,
void appSystemInputPortEdidSet(BYTE cInputPort, eEDIDSELECT cEdidSelect);
eEDIDSELECT appSystemInputPortEdidGet(BYTE cInputPort);
void appSystemInputPortEdidDataGet(BYTE cInputPort,PBYTE pEdidData);

//  ��EDID ���ݱ��浽NVRAM, �������õ�����оƬ��
//  ����˿ڵ�EDID ѡ��
//  cEdidSelect:
//
void appSystemInputPortEdidDataSet(BYTE cInputPort, eEDIDSELECT cEdidSelect, PCBYTE pcEdidData);





// HDMI �������أ����ұ���״̬��NVRAM,���������̫��ѧ
void appSystemHDMITXAudioOutEnableSet(BYTE cInstance, BOOL bEnable);

//void appSystemSwitchDemoProcessEnable(BOOL bDemo);
void appSystemSwitchDemoModeEnableSet(BOOL bEnable);
BOOL appSystemSwitchDemoModeEnableGet(void);


void appSystemSwitchDemoModeProcess(void);



#ifdef PRD_IR_OUT
//�л�Զ�˺��������������IR_OUT �˿�
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherSet(BYTE cInput, BYTE cPortNumber, ...);
// ע�������INPUT ��ָIR OUT 1~4
// �������cInput �е�ָ�����������
//cInput: ����˿ں�,Begin from 0,MAX_IN will be mute the output
//cPortNumber: ����˿���Ŀ Begin from 1
//...  : ����˿ں�
void appSystemIRSwitcherPortSet(BYTE cInput, BYTE cPortNumber, PBYTE cOutPortList);

void appSystemIRSwitcherSetStart(void);


#endif

#endif

