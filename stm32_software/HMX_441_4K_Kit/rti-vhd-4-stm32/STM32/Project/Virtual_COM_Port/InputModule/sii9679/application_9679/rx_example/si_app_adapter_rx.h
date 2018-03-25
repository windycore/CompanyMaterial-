//
// file:  si_app_adapter_rx.h
//
#ifndef _SI_APP_ADAPTER_RX_H_
#define _SI_APP_ADAPTER_RX_H_
#include "si_datatypes.h"
#include "Si_drv_adapter_tx.h"
#include "si_drv_adapter_rx.h"


void dv9679AdapterSystemInit(void);
// ����9679 �����HPD
void dvSiiDrvAdaptRxHpdSet(BYTE cInstance, SiiDrvAdaptInpCtrl_t hpd);
//��������˵�Edid
void dvSiiDrvAdaptRxEdidSramWrite(BYTE cInstance,
                                  uint16_t offset,
                                  uint8_t  *pData,
                                  uint16_t length);



// ������9679 �Ƿ�����ź�
void  dvSiiDrvAdaptTxTmdsOutSet(BYTE cInstance,  SiiDrvAdaptTmdsOut_t tmdsCtrl);
void dvSiiDrvAdaptTaskExecute(BYTE cInstance);
// return : true have HDCP
//             false no HDCP
BOOL dvSiiDrvAdaptRxHDCPGet(BYTE cInstance);
// �����ʱ���н��ź�
// return TRUE: connected
BOOL dvSiiDrvAdaptRxConnectGet(BYTE cInstance);

// �õ�9679  ������Ƿ�ʹ��?// return TRUE: Is Enable
//           Fasle is Disable
BOOL dvSiiDrvAdaptTxTmdsOutEnGet(BYTE cInstance);


#endif

