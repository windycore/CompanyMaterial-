//
// file:  si_app_adapter_rx.h
//
#ifndef _SI_APP_ADAPTER_RX_H_
#define _SI_APP_ADAPTER_RX_H_
#include "si_datatypes.h"
#include "Si_drv_adapter_tx.h"
#include "si_drv_adapter_rx.h"


void dv9679AdapterSystemInit(void);
// 控制9679 输入的HPD
void dvSiiDrvAdaptRxHpdSet(BYTE cInstance, SiiDrvAdaptInpCtrl_t hpd);
//更新输入端的Edid
void dvSiiDrvAdaptRxEdidSramWrite(BYTE cInstance,
                                  uint16_t offset,
                                  uint8_t  *pData,
                                  uint16_t length);



// 设置输9679 是否输出信号
void  dvSiiDrvAdaptTxTmdsOutSet(BYTE cInstance,  SiiDrvAdaptTmdsOut_t tmdsCtrl);
void dvSiiDrvAdaptTaskExecute(BYTE cInstance);
// return : true have HDCP
//             false no HDCP
BOOL dvSiiDrvAdaptRxHDCPGet(BYTE cInstance);
// 输入端时候有接信号
// return TRUE: connected
BOOL dvSiiDrvAdaptRxConnectGet(BYTE cInstance);

// 得到9679  输出端是否使能?// return TRUE: Is Enable
//           Fasle is Disable
BOOL dvSiiDrvAdaptTxTmdsOutEnGet(BYTE cInstance);


#endif

