
#ifndef _INPUTHANDLE_H_
#define _INPUTHANDLE_H_
#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "util.h"

typedef enum
{
	eEDID_BYPASS=0,  //ֱͨ
	eEDID_720P_2D_BYPASS,
	eEDID_720P_3D_BYPASS,
	eEDID_1080P_2D_BYPASS,
	eEDID_1080P_3D_BYPASS,
	eEDID_4K_30HZ_BYPASS,
	eEDID_4K_30HZ_2CH,
	eEDID_4K_30HZ_8CH,
	eEDID_4K_60HZ_420_BYPASS,
	eEDID_4K_60HZ_420_8CH,
	eEDID_4K_60HZ_444_BYPASS,
	eEDID_4K_60HZ_444_8CH,
	
	eEDID_RS232_GUI_CONTROL=0x0f,
	
}eEDID_NUMBER;

typedef enum
{
	eFIRST_POWERON=0,  			//ÿ�ε�һ���ϵ�״̬��־
	eALREADY_POWERON,			//�Ѿ����������е��ϵ�״̬��ʾ
	
}ePOWER_STATUS;


void EDIDDataProcess(BYTE *TheData);  // wyl


void appInputHDCPCompatible(void);
// ��������ڵ�EDID,
void appSystemInputPortEdidSet(BYTE cInputPort, eEDIDSELECT cEdidSelect, PCBYTE pcEdidData,eEDIDINPORTSELECT cINPORTSELECT );
eEDIDSELECT appSystemInputPortEdidGet(BYTE cInputPort);
void appSystemInputPortEdidDataGet(BYTE cInputPort,PBYTE pEdidData);

//  ��EDID ���ݱ��浽NVRAM, �������õ�����оƬ��
//  ����˿ڵ�EDID ѡ��
//  cEdidSelect:
//
void appSystemInputPortEdidDataSet(BYTE cInputPort, eEDIDSELECT cEdidSelect, PCBYTE pcEdidData);
void InternalEdidProcess(unsigned char EdidSel, unsigned char en);
void GuiUartInternalEdidProcess(unsigned char inport, unsigned char EdidSel);
void appInputPortDialSwitchEdidDataDetect(void);
unsigned char appGetDialSwitchEdidStatus(void);

//void appInputPortDialSwitchEdidDataTest(void);

#endif