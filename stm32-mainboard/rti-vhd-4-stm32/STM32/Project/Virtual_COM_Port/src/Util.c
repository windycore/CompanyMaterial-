// power by kyq
#include "util.h"
#include "mcu.h"
#include "si_datatypes.h"
#include <stddef.h>
#include "MainApp.h"

#define CPU_INT_DISABLE
#define CPU_INT_ENABLE

#define USER_INPUT_QUEUE_MSGS 	5

#define USER_INPUT_MSG_SIZE 	10

#define USER_MAX_MSG_SIZE 	USER_INPUT_MSG_SIZE*USER_INPUT_QUEUE_MSGS

// command port index to output sii9396 instance map
BYTE mac_PortMap[MAX_OUT] =
{
#if (MACHINETYPE==MUH44A_H2)
    4, 	// Instance 4		//输出1
    5, 	// Instance 5		//输出2
    6, 	// Instance 6		//输出3
    7,	 	// Instance 7		//输出4
#else
    8, 		// Instance 8		//输出1
    9, 		// Instance 9		//输出2
    10, 		// Instance 10		//输出3
    11,	 	// Instance 11		//输出4
    12, 		// Instance 12		//输出5
    13, 		// Instance 13		//输出6
    14, 		// Instance 14		//输出7
    15,	 	// Instance 15		//输出8
#endif
};

BYTE mac_InstanceMap[MAX_OUT] =
{

    0, 	//输出1
#if(MAX_OUT>1)
    1, 	//输出2
#endif
#if(MAX_OUT>2)
    2, 	//输出3
#endif
#if(MAX_OUT>3)
    3,	 	//输出4
#endif
#if(MAX_OUT>4)
    4, 	//输出5
#endif
#if(MAX_OUT>5)
    5, 	//输出6
#endif
#if(MAX_OUT>6)
    6, 	//输出7
#endif
#if(MAX_OUT>7)
   7,	 	//输出8
#endif

};

typedef struct
{
/* --------------- BEGIN CUSTOM AREA [015] utilExecutive.c -------------------
 * CONFIGURATION: Structure definition for memory used to store message queues.
 * Each queue should be defined as a separate byte array in the structure. */
    BYTE                acUserInput[USER_MAX_MSG_SIZE];
/* ---------------------------- END CUSTOM AREA --------------------------- */
} MSG_QUEUES;

extern MainAppInstanceData_t *pMainAPP;
// ---------------------------------------------------------------------------
// Status data for each message queue
// ---------------------------------------------------------------------------

static MSG_QUEUE_INFO   m_asQInfo[mqINVALID];
static MSG_QUEUE_Information asQInfo_m[mqINVALID];
// ---------------------------------------------------------------------------
// Memory buffer for message queues
// ---------------------------------------------------------------------------
static BYTE m_acMsgQueues[sizeof(MSG_QUEUES)];

static BYTE m_MsgQueuesFifo[mqINVALID][USER_MAX_MSG_SIZE];

static const MSG_QUEUE_CONFIG m_asQConfig[mqINVALID] = {
/* --------------- BEGIN CUSTOM AREA [020] utilExecutive.c -------------------
 * CONFIGURATION: Size and location configuration for message queues.  The
 * order entries in this array MUST match the order of enumerations in
 * eMESSAGE_QUEUES type */
    {
        USER_INPUT_QUEUE_MSGS, USER_INPUT_MSG_SIZE, offsetof(MSG_QUEUES, acUserInput)
    },
};


////////////////////////////////////////////
/* Private functions ---------------------------------------------------------*/
/* 1US延时，已通过示波器验证过*/
void delay_nus(uint16_t time)
{
    uint8_t j;
    while (time-- != 0)
    {
        for (j = 0; j < 8; j++) ;
    }
}
/* 1MS延时，已通过示波器验证过*/
void delay_nms(uint16_t count)
{
    uint16_t j;
    while (count-- != 0)
    {
        IWDG_ReloadCounter(); //喂狗
        for (j = 0; j < 10000; j++) ;
    }
}



// ---------------------------------------------------------------------------
// This API sends a message to the specified queue.
//
// Params:
//  eQueue  : Queue to send message to
//  pacData : Pointer to message data
//
// Return: none
// ---------------------------------------------------------------------------

void utilExecMsgSend(eMESSAGE_QUEUES eQueue,
                     PBYTE           pacData)
{

    PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
    PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);

    CPU_INT_DISABLE;
    if (psQInfo->cQueueEmpty)
    {
        psQInfo->cQueueEmpty = FALSE;
    }
    else
    {
        psQInfo->cWriteIndex =
            (psQInfo->cWriteIndex == psQCfg->cQLength - 1) ?
            0 : psQInfo->cWriteIndex + 1;
        ASSERT(psQInfo->cWriteIndex != psQInfo->cReadIndex);
    }
    memcpy(&m_acMsgQueues
                   [psQCfg->wOffset + psQCfg->cMsgSize * psQInfo->cWriteIndex],
                   pacData,
                   psQCfg->cMsgSize);
    CPU_INT_ENABLE;

}

  

void utilExecOlnyMsgSend(eMESSAGE_QUEUES eQueue,
                     Event_t       EventData)
{
    DWORD DataLength=0;
    PBYTE  pacData=(BYTE *)&EventData;
  //  PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
  //  PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];
    PMSG_QUEUE_Information  psQInfo = &asQInfo_m[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);
	
   if ( pMainAPP->messageHandlers[ eQueue].pMsgHandler == 0) //if task  resgit
     return ;
	
 //   CPU_INT_DISABLE;
// copy masssage data
   for(DataLength=0;DataLength<(sizeof(Event_Head_t)+EventData.Head.argCount);DataLength++)
   {
      if( psQInfo->pWrite>(m_MsgQueuesFifo[eQueue]+USER_MAX_MSG_SIZE))
	  	psQInfo->pWrite=m_MsgQueuesFifo[eQueue];

      	*psQInfo->pWrite=*pacData;
     	psQInfo->pWrite++;
    	pacData++;
 
   }

 //   CPU_INT_ENABLE;



}
 void utilExecListMsgSend(BYTE cPortModules, va_list pArgs,BYTE cMsgTypes,PBYTE   pacData)
{
     eMESSAGE_QUEUES eQueue;
     va_list         args=pArgs;
    PMSG_QUEUE_INFO     psQInfo ;
    PCMSG_QUEUE_CONFIG  psQCfg; 
    BYTE i;
    CPU_INT_DISABLE;
   for (i = 0; i < cPortModules; i++)
   {
    eQueue  = va_arg(args, BYTE);
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);
   psQInfo = &m_asQInfo[eQueue];
   psQCfg= &m_asQConfig[eQueue];

    if (psQInfo->cQueueEmpty)
    {
        psQInfo->cQueueEmpty = FALSE;
    }
    else
    {
        psQInfo->cWriteIndex =
            (psQInfo->cWriteIndex == psQCfg->cQLength - 1) ?
            0 : psQInfo->cWriteIndex + 1;
        ASSERT(psQInfo->cWriteIndex != psQInfo->cReadIndex);
    }
    memcpy(&m_acMsgQueues
                   [psQCfg->wOffset + psQCfg->cMsgSize * psQInfo->cWriteIndex],
                   pacData,
                   psQCfg->cMsgSize);
   	}
    CPU_INT_ENABLE;
	va_end(args);
}

// ---------------------------------------------------------------------------
// This API reads a message from the specified queue.
//
// Params:
//  eQueue  : Queue to read message from
//  pacData : Pointer where message data will be stored
//
// Return:
//  rcSUCCESS if a message was read, rcERROR if queue was empty
// ---------------------------------------------------------------------------
eRESULT utilExecMsgRead(eMESSAGE_QUEUES eQueue,
                        PBYTE           pacData)
{
    BYTE                cDepth;
    PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
    PCMSG_QUEUE_CONFIG  psQCfg = &m_asQConfig[eQueue];

    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);

    CPU_INT_DISABLE;
    if (psQInfo->cWriteIndex == psQInfo->cReadIndex)
    {
        cDepth = psQInfo->cQueueEmpty ? 0 : 1;
    }
    else
    {
        cDepth = (psQInfo->cWriteIndex > psQInfo->cReadIndex) ?
            psQInfo->cWriteIndex - psQInfo->cReadIndex :
            psQInfo->cWriteIndex - psQInfo->cReadIndex + psQCfg->cQLength + 1;
    }

    if (cDepth)
    {
        memcpy(pacData,
                       &m_acMsgQueues[psQCfg->wOffset +
                           psQCfg->cMsgSize * psQInfo->cReadIndex],
                       psQCfg->cMsgSize);

        if (psQInfo->cReadIndex == psQInfo->cWriteIndex)
        {
            psQInfo->cQueueEmpty = TRUE;
        }
        else
        {
            psQInfo->cReadIndex =
                (psQInfo->cReadIndex ==
                 psQCfg->cQLength - 1) ? 0 : psQInfo->cReadIndex + 1;
        }
    }
    CPU_INT_ENABLE;

    return cDepth ? rcSUCCESS : rcERROR;
}


BOOL utilExecMsgHeadRead(eMESSAGE_QUEUES eQueue,
                        PBYTE           pacData)
{
    BYTE                cDepth;
  PMSG_QUEUE_Information psQInfo = &asQInfo_m[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);

    CPU_INT_DISABLE;
   for(cDepth=0;cDepth<sizeof(Event_Head_t);cDepth++)
   {
      if(psQInfo->pRead > (m_MsgQueuesFifo[eQueue]+USER_MAX_MSG_SIZE))
	  	psQInfo->pRead=m_MsgQueuesFifo[eQueue];
	  
        * pacData =*psQInfo->pRead;
	 pacData++;
	 psQInfo->pRead++;

   }

    CPU_INT_ENABLE;

    return cDepth ? rcSUCCESS : rcERROR;
}

eRESULT utilExecMsgDataRead(eMESSAGE_QUEUES eQueue,
                        pEvent_t EventData)
{
  BYTE                cDepth;
 PBYTE    pacData=(BYTE *)EventData;
 // Event_t       EventData;
  PMSG_QUEUE_Information psQInfo = &asQInfo_m[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
    ASSERT(pacData != NULL);

    CPU_INT_DISABLE;
//read head infor

   for(cDepth=0;cDepth<sizeof(Event_Head_t);cDepth++)
   {
      if(psQInfo->pRead > (m_MsgQueuesFifo[eQueue]+USER_MAX_MSG_SIZE))
	  	psQInfo->pRead=m_MsgQueuesFifo[eQueue];
	  
        * pacData =*psQInfo->pRead;
	 pacData++;
	 psQInfo->pRead++;

   }

//read msg data
   for(cDepth=0;cDepth<EventData->Head.argCount;cDepth++)
   //for(cDepth=0;cDepth<sizeof(Event_t);cDepth++)
   {
      if(psQInfo->pRead > (m_MsgQueuesFifo[eQueue]+USER_MAX_MSG_SIZE))
	  	psQInfo->pRead=m_MsgQueuesFifo[eQueue];
	  
        * pacData =*psQInfo->pRead;
	 pacData++;
	 psQInfo->pRead++;

   }

    CPU_INT_ENABLE;

    return cDepth ? rcSUCCESS : rcERROR;
}
// ---------------------------------------------------------------------------
// This API clears all messages from the specified queue.
//
// Params:
//  eQueue : Queue to clear
//
// Return: none
// ---------------------------------------------------------------------------
void utilExecMsgClear(eMESSAGE_QUEUES eQueue)
{
    PMSG_QUEUE_INFO     psQInfo = &m_asQInfo[eQueue];
    PMSG_QUEUE_Information psQInformation = &asQInfo_m[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);

    CPU_INT_DISABLE
    psQInfo->cReadIndex = 0;
    psQInfo->cWriteIndex = 0;
    psQInfo->cQueueEmpty = 1;
    psQInformation->pRead=m_MsgQueuesFifo[eQueue];
    psQInformation->pWrite=m_MsgQueuesFifo[eQueue];
    psQInformation->cEmpty=1;
    CPU_INT_ENABLE
}

// ---------------------------------------------------------------------------
// This API checks if a message is present in the specified queue.
//
// Params:
//  eQueue : Queue to check for messages
//
// Return:
//  TRUE if there is a message in the queue, FALSE otherwise
// ---------------------------------------------------------------------------
BOOL utilExecMsgPresent(eMESSAGE_QUEUES eQueue)
{
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);

    return !m_asQInfo[eQueue].cQueueEmpty;
}

BOOL utilExecMsgPreRead(eMESSAGE_QUEUES eQueue)
{
    // validate parameters in debug mode only
    // BOOL  fQueueEmpty;
    PMSG_QUEUE_Information psQInfo = &asQInfo_m[eQueue];
    // validate parameters in debug mode only
    ASSERT(eQueue < mqINVALID);
   if(psQInfo->pRead==psQInfo->pWrite)
       psQInfo->cEmpty=TRUE;
   else
     psQInfo->cEmpty=FALSE;
    return !psQInfo->cEmpty;
}

/*******************************************************************************/
// 端口号到实例转换
// cPort: from 0
BYTE utilOutputPortToInstance(BYTE cPort)
{
    assert_param(cPort < MAX_OUT);
    if (cPort < MAX_OUT)
    {
        return mac_PortMap[cPort];
    }
    else
        return 0;

}

// 实例到端口
// cPort: from 0
BYTE utilInstanceToOutputPort(BYTE cInstance)
{
    assert_param(cInstance < DevNum);
    if ((cInstance >= MAX_OUT)&&(cInstance < DevNum))
    {
        return mac_InstanceMap[cInstance-MAX_OUT];
    }
    else
        return 0;


}
/*******************************************************************************/

void utilVariableInit(void)
{
   BYTE cIndex;
    // initialize message queue control data
    for (cIndex = 0; cIndex < mqINVALID; cIndex++)
    {
        m_asQInfo[cIndex].cReadIndex = 0;
        m_asQInfo[cIndex].cWriteIndex = 0;
        m_asQInfo[cIndex].cQueueEmpty = TRUE;
	asQInfo_m[cIndex].pRead= m_MsgQueuesFifo[cIndex];
        asQInfo_m[cIndex].pWrite= m_MsgQueuesFifo[cIndex];
        asQInfo_m[cIndex].cEmpty = TRUE; 
	   // clear messsage queues
    memset(m_MsgQueuesFifo[cIndex],0,
                   USER_MAX_MSG_SIZE);
    }

    // clear messsage queues
    memset(&m_acMsgQueues,0,
                   sizeof(m_acMsgQueues));
}

