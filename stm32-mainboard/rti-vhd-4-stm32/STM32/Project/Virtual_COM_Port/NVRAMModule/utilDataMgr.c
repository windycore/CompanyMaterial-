
//[kyq Begin: 20150804 19:45:35]

#include "utilDataMgrAPI.h"
#include <stddef.h>
#include "appSystemMgrAPI.h"
#include "appPowerMgrAPI.h"
#include "dvNVRAM.h"
#include "utilCRC.h"
#include "uart2.h"
#include "MainApp.h"
#define NONVOLATILE_DATA_STORAGE TRUE
#define NVRAM_CRC_CHECK    TRUE
#if defined(NVRAM_CRC_CHECK)
#define NODECRC(x) WORD x
#define CRCLEN    2
#else
#define NODECRC(x)
#define CRCLEN    0
#endif
#define  SYSTEM_DATA_VERSION  28  //new product you can  chang，but  old product  you must don't  chang this value 
                                                             //because  chang   name of  product  when  make  OEM   in the factory
                                                           // if you don't  know ,you can tell  your leader....
                                                          


// ---------------------------------------------------------------------------
// sentinel values for node state
// ---------------------------------------------------------------------------
#define DIRTY           0x80
#define WRITECOUNTER    0xff
#define INITMASK        0xffff




#define EDID_BLOCK_LEN 128

#define EDID_BL0_ADR__HEADER						0x00
#define EDID_BL0_LEN__HEADER						8

// ---------------------------------------------------------------------------
// node status information structure
// ---------------------------------------------------------------------------
typedef struct
{
    BYTE                cLockCounter;
    BYTE                cFlag;
} NODE_CONTROL_INFO;

typedef struct
{
    WORD    wSysVer;          // store the system data version.
} DATAMGR_VERSION_DATA, *PDATAMGR_VERSION_DATA;


// ---------------------------------------------------------------------------
// node configuration information structure
// ---------------------------------------------------------------------------
typedef struct
{
    void (*fpCallBack)(eNODE_NAME eNodeID,
                       eNV_EVENT nveEvent);
    WORD                wNodeSize;
    DWORD                wNodeOffset;
} NODE_INFO;


typedef struct
{
/* ---------------- BEGIN CUSTOM AREA [020] utilDataMgr.c --------------------
 * CONFIGURATION: Add structure definitions for all data stored by Data
 * Manager */
    // Important NVRAM data should be place first
    SYSTEM_SETTING_DATA sSystem_Setting;
    NODECRC(crc_System_Setting);  // + node crc field.
/*
    FACTORY_DATA sFactoryDataInfo;
    NODECRC(crc_Factory_Data);    // + node crc field.
*/
    POWER_MANAGER_DATA sPowerMgrData;
    NODECRC(crc_PowerMgrData);        // + node crc field.

    OUTPUTPORT_MANAGER_DATA sOutputPortMgrData;
    NODECRC(crc_OutputPortMgrData);        // + node crc field.

    INPUTPORT_MANAGER_DATA  sInputPortMgrData;
    NODECRC(crc_InputPortMgrData);        // + node crc field.

    BYTE cNotUsedSource;
    NODECRC(crc_NotUsed);
    BYTE cNVRAM_BLANK;
/* ---------------------------- END CUSTOM AREA --------------------------- */
} NVRAM_DATA;

static const NODE_INFO m_asNodeInfo[nnINVALID_NODE] = {
/* CONFIGURATION: Add configuration data for each data node (callback
 * routines, sizes and location) The order of this data must match the
 * corresponding names in eNODE_NAME. */
    {   // Repeat the initialization of the front end adc if the eliza front end not present
        appSystemSettingNodeCallback,
        sizeof(SYSTEM_SETTING_DATA) + CRCLEN,
        offsetof(NVRAM_DATA, sSystem_Setting)
    },
/*
    {
        appUIFactoryDataNodeCallback,     //
        sizeof(FACTORY_DATA) + CRCLEN,
        offsetof(NVRAM_DATA, sFactoryDataInfo)
    },
  */
    {
        appPowerMgrNodeCallback,          // Power manager
        sizeof(POWER_MANAGER_DATA) + CRCLEN,
        offsetof(NVRAM_DATA, sPowerMgrData)
    },
    {
        appSystemSettingNodeCallback,          // out port manager data
        sizeof(OUTPUTPORT_MANAGER_DATA) + CRCLEN,
        offsetof(NVRAM_DATA, sOutputPortMgrData)
    },
    //nnINPUTPORT_MAMAGER_DATA
    {
        appSystemSettingNodeCallback,          // In port manager data
        sizeof(INPUTPORT_MANAGER_DATA) + CRCLEN,
        offsetof(NVRAM_DATA, sInputPortMgrData)
    },


};








//--------------------------------CONFIGURATION-------------------------------
//  Maximum number of bytes to be written at-a-time
//--------------------------------CONFIGURATION-------------------------------
// For large nodes, the inital boot-up read from nvram can cause a timeout on
// the device bus. Set the block size to less than 16K.  This was moved from
// custom to raw because it's not likely to change between projects.
#define UTIL_DATAMGR_MAX_BLOCK_XFER (4096)

static NVRAM_DATA       m_sRamMirror;
//static NVRAM_DATA       m_sScratchRamMirror;

static BOOL             m_bUpdateInhibit = FALSE;

// ---------------------------------------------------------------------------
// Node status information array
// ---------------------------------------------------------------------------
static NODE_CONTROL_INFO m_asNodeControlInfo[nnINVALID_NODE];

extern uint8_t g_Init_EdidFlashEdidTable[];


/*****************************************************************************/
/**
* @brief Reset the chip by toggling the reset pin
*
*
* @note This function is used during intialization, needs to be called before
*   register reading and SiiDrvAdaptStart().
*
*****************************************************************************/


static bool_t EdidBlock0Check(uint8_t EdidData[EDID_BLOCK_LEN])
{
    uint8_t check_sum = 0;
    uint8_t i;
    const uint8_t aEdidHeader[EDID_BL0_LEN__HEADER] =
    { 0x00,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0x00 };

    //check check_sum
    for (i = 0; i < EDID_BLOCK_LEN; i++)
    {
        check_sum += EdidData[i];
    }

    if (check_sum)
    {
        return false;
    }

    // Check EDID header
    if (0 != memcmp(EdidData, (char *)aEdidHeader, EDID_BL0_LEN__HEADER))
        return false;

    return true;
}

// Load NVRam data into RAM mirror.
//
// Params:
//  eNodeID : node ID
//
// Return: none
// ---------------------------------------------------------------------------
// Load NVRam data into RAM mirror.
//
// Params:
//  eNodeID : node ID
//
// Return: none
// ---------------------------------------------------------------------------
static void LoadNvramToRamMirror(eNODE_NAME eNodeID)
{
    PBYTE               pcRamMirror;
    WORD                wCopySize, wStartByte;


    pcRamMirror = (PBYTE)&m_sRamMirror +
        m_asNodeInfo[eNodeID].wNodeOffset;

    wStartByte = m_asNodeInfo[eNodeID].wNodeOffset + sizeof(DATAMGR_VERSION_DATA);
    wCopySize = m_asNodeInfo[eNodeID].wNodeSize;

    //if (rcSUCCESS == RESOURCE_BUSY_SET(reDEVICE_BUS))
    {

        WORD  wIndex = 0;
        while (wIndex < m_asNodeInfo[eNodeID].wNodeSize)
        {

            if (UTIL_DATAMGR_MAX_BLOCK_XFER > (m_asNodeInfo[eNodeID].wNodeSize - wIndex))
            {
                wCopySize =  m_asNodeInfo[eNodeID].wNodeSize - wIndex;
            }
            else
            {
                wCopySize =  UTIL_DATAMGR_MAX_BLOCK_XFER;
            }
            //*
            if (rcSUCCESS != dvNVRAMBytesRead(wStartByte,
                                              wCopySize,
                                              pcRamMirror))
            {
                ASSERT_ALWAYS();
            }
            //*/

            wIndex += wCopySize;
            pcRamMirror += wCopySize;
            wStartByte += wCopySize;
        }
        //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);
    }
    /*
    else
    {
        ASSERT_ALWAYS();
    }
    */

}

// ---------------------------------------------------------------------------
// Write RAM mirror data to NVRAM.
//
// Params:
//  eNodeID : node ID
//
// Return: none
// ---------------------------------------------------------------------------
static void WriteRamMirrorToNvram(eNODE_NAME eNodeID)
{
    PBYTE               pcRamMirror;
    WORD                wNodeSize, wNodeOffset;

    pcRamMirror = (PBYTE)&m_sRamMirror +
        m_asNodeInfo[eNodeID].wNodeOffset;

    wNodeSize = m_asNodeInfo[eNodeID].wNodeSize;
    wNodeOffset = m_asNodeInfo[eNodeID].wNodeOffset;

    // if (rcSUCCESS == RESOURCE_BUSY_SET(reDEVICE_BUS))
    {


        if (rcSUCCESS != dvNVRAMBytesWrite(wNodeOffset + sizeof(DATAMGR_VERSION_DATA),
                                           wNodeSize,
                                           pcRamMirror))
        {
            ASSERT_ALWAYS();
        }

        // RESOURCE_BUSY_CLEAR(reDEVICE_BUS);
    }
    //else
    //{
    //    ASSERT_ALWAYS();
    //}
}


// ---------------------------------------------------------------------------
// calculate CRC for the nodes in RAM mirror.
//
// Params:
//  eNodeID : node ID
//
// Return: none
//
// Notes:
//  CRC is calculated and recorded before the RAM data is written to NVRAM
// ---------------------------------------------------------------------------
static void setNodeCRC(eNODE_NAME eNodeID)
{
    PWORD               pCRC;
    PBYTE               pData;

    pData = (PBYTE)&m_sRamMirror +
        m_asNodeInfo[eNodeID].wNodeOffset;
    pCRC = (PWORD)((PBYTE)&m_sRamMirror +
                   (m_asNodeInfo[eNodeID].wNodeOffset +
                    m_asNodeInfo[eNodeID].wNodeSize - CRCLEN));

    *pCRC = utilCRC16Calc(pData,
                          m_asNodeInfo[eNodeID].wNodeSize - CRCLEN);
}

// ---------------------------------------------------------------------------
// This API initializes the Data Manager component, and also calls all client
// callback routines to distribute node ID information to the client
// components.
//
// Params: none
//
// Return: none
// ---------------------------------------------------------------------------
bool_t NVRAMInitialize(void)
{

#if defined(NVRAM_CRC_CHECK)
    PBYTE                 pData;
    PWORD                 pCRC;
#endif
    eNV_EVENT             eNvEvent = nveINVALID;
    DATAMGR_VERSION_DATA  sVersion;
    eRESULT               eResult = rcSUCCESS;

    INT                   i;
    BYTE   acDeafultEdid[EDID_TABLE_LEN];
    if (sizeof(m_sRamMirror) + sizeof(DATAMGR_VERSION_DATA) > USER_EDID_ADDR-1) // to do size
    {
        ASSERT_ALWAYS();
    }

    // Initialize Ram mirror - m_sRamMirror to 0
    memset(&m_sRamMirror,
           0,
           sizeof(m_sRamMirror)
          );



    // initialize node control information
    memset(&m_asNodeControlInfo,
           0,
           sizeof(m_asNodeControlInfo)
          );

    // initialize sgDATA_MGR signal groups
    /*
    for (i = 0; i < ((nnINVALID_NODE + 15) / 16); i++)
    {
        ASSERT((sgDATA_MGR + i)<sgWIN_MGR0); // kyq
        utilExecSignalSet(sgDATA_MGR + i,
                          INITMASK);
    }
    */


    //eResult = RESOURCE_BUSY_SET(reDEVICE_BUS);
    ASSERT(rcSUCCESS == eResult);

    // read data version from NVRAM if available (version
    // numbers are always at start of NVRAM)
    eResult = dvNVRAMBytesRead(0,
                               sizeof(DATAMGR_VERSION_DATA),
                               (PBYTE)&sVersion);


    ASSERT(eResult == rcSUCCESS);

    // if calibration data version is different than
    // the stored data, then "RESETALL"
    if (SYSTEM_DATA_VERSION != sVersion.wSysVer)
    {
        // if system data version is different than the stored
        // data, then "INITIALIZE except calibration data"
        eNvEvent = nveINITIALIZE;
    }
    //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);  //kyq move to here
    if (nveINVALID != eNvEvent)			//EEPROM是空片时，第一次上电的初始化
    {
        // set version numbers to current values and store

        sVersion.wSysVer = SYSTEM_DATA_VERSION;
        //eResult = RESOURCE_BUSY_SET(reDEVICE_BUS);
        ASSERT(rcSUCCESS == eResult);

        eResult = dvNVRAMBytesWrite(0,
                                    sizeof(DATAMGR_VERSION_DATA),
                                    (PBYTE)&sVersion);

        ASSERT(eResult == rcSUCCESS);
        //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);  //kyq move to up
        // send NVRAM event to all nodes with valid callback routines
#if defined(NVRAM_CRC_CHECK)
        for (i = 0; i < nnINVALID_NODE; i++)
        {
            utilDataMgrWriteLockData((eNODE_NAME)i);
            LoadNvramToRamMirror((eNODE_NAME)i);
            m_asNodeControlInfo[(eNODE_NAME)i].cLockCounter = 0;

        }
#endif
        // end add kyq
        //eResult = RESOURCE_BUSY_SET(reDEVICE_BUS); //[kyq:  20070806 14:38:03 ]
        //ASSERT (eResult == rcSUCCESS);
        utilDataMgrReset(eNvEvent);
        //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);

    }

    // RESOURCE_BUSY_CLEAR(reDEVICE_BUS);  //kyq move to up


    if (nveINVALID == eNvEvent)
    {
        for (i = 0; i < nnINVALID_NODE; i++)
        {
#if defined(NVRAM_CRC_CHECK)
            // load NVRam data to Ram mirror when CRC is correct
            utilDataMgrWriteLockData((eNODE_NAME)i);
            LoadNvramToRamMirror((eNODE_NAME)i);
            // unlock w/o the overhead of utilDataMgrUnlock()
            m_asNodeControlInfo[(eNODE_NAME)i].cLockCounter = 0;
            //utilExecSignalSet(sgDATA_MGR + (i / 16),
            //                  1 << (i % 16));

            pData = (PBYTE)&m_sRamMirror + m_asNodeInfo[(eNODE_NAME)i].wNodeOffset;
            pCRC = (PWORD)((PBYTE)&m_sRamMirror +
                           m_asNodeInfo[(eNODE_NAME)i].wNodeOffset +
                           m_asNodeInfo[(eNODE_NAME)i].wNodeSize - CRCLEN);

            //eResult = RESOURCE_BUSY_SET(reDEVICE_BUS); //[kyq:  20070806 14:38:03 ]
            ASSERT(eResult == rcSUCCESS);
            // if CRC is wrong, call client to initialize RAM mirror to default
            // [kyq:  20070813 17:54:58 ] to save time
            if (m_asNodeInfo[i].fpCallBack != NULL)
            {
                if (*pCRC != utilCRC16Calc(
                        pData,
                        m_asNodeInfo[(eNODE_NAME)i].wNodeSize - CRCLEN))
                {
                    // Ensure that callback is not NULL.
                    if (NULL != m_asNodeInfo[i].fpCallBack)
                    {
                        m_asNodeInfo[i].fpCallBack((eNODE_NAME)i,
                                                   nveRESETALL);
                    }
                }
            }
            //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);
#else // no CRC check

            // load NVRam data to Ram mirror without checking CRC
            utilDataMgrWriteLockData((eNODE_NAME)i);
            LoadNvramToRamMirror((eNODE_NAME)i);
            // unlock w/o the overhead of utilDataMgrUnlock()
            m_asNodeControlInfo[(eNODE_NAME)i].cLockCounter = 0;
            //utilExecSignalSet(sgDATA_MGR + (i / 16),
            //                  1 << (i % 16));

#endif // NVRAM_CRC_CHECK
        }
    }


     dvNVRAMUserEdidRead(edsEDID_Default,acDeafultEdid ); //[kyq add: 20150820 20:13:32]
    if (EdidBlock0Check(acDeafultEdid))
        {
             memcpy(g_Init_EdidFlashEdidTable,acDeafultEdid,EDID_TABLE_LEN);
	     // how to check block2??
        }
     SiiPlatformTimerSet(etNVRAM_UPDATE, mS_TO_TICKS(NVRAM_UPDATE_DELAY),0xFF);     // Set poll timer
 return 1;


}

// ---------------------------------------------------------------------------
// This API calls all client callback routines with a reset event, so the
// client components can reset the data they have stored in non-volatile
// memory.
//
// Params:
//  eResetType : Reset type, sent to node callback functions
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrReset(eNV_EVENT eResetType)
{
    int                 i;
    eRESULT eResult = rcSUCCESS; //RESOURCE_BUSY_SET(reDEVICE_BUS);
    ASSERT(eResult == rcSUCCESS);

    ASSERT(eResetType < nveINVALID);

    // send NVRAM event to all nodes with valid callback routines
    for (i = 0; i < nnINVALID_NODE; i++)
    {
        if (NULL != m_asNodeInfo[i].fpCallBack)
        {
            m_asNodeInfo[i].fpCallBack((eNODE_NAME)i,
                                       eResetType);
        }
    }
    //RESOURCE_BUSY_CLEAR(reDEVICE_BUS);
}

//#pragma argsused
// ---------------------------------------------------------------------------
// This API transfers data stored in the RAM mirror into non-volatile memory.
//
// Params:
//  eNodeID : The node identifier
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdate(eNODE_NAME eNodeID)
{
#if defined(NONVOLATILE_DATA_STORAGE)
    int                 i;

    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE || UPDATE_ALL_NODEID == eNodeID);

    // if update request is for a single node, update it if dirty
    if (UPDATE_ALL_NODEID != eNodeID)
    {
        if (0 != (m_asNodeControlInfo[eNodeID].cFlag & DIRTY))
        {
            // Read lock is used. Ram mirror is still available for read
            utilDataMgrReadLockData(eNodeID);
            setNodeCRC(eNodeID);

            // load NVRam data to Ram mirror when CRC is correct
            WriteRamMirrorToNvram(eNodeID);
            m_asNodeControlInfo[eNodeID].cFlag &= ~DIRTY;

            utilDataMgrUnlock(eNodeID,
                              FALSE);
        }
    }
    // otherwise (request is for all nodes), update if not inhibited
    else if (!m_bUpdateInhibit)
    {
        for (i = 0; i < nnINVALID_NODE; i++)
        {
            if (0 != (m_asNodeControlInfo[i].cFlag & DIRTY))
            {
                // Read lock is used. Ram mirror is still available for read
                utilDataMgrReadLockData((eNODE_NAME)i);
                setNodeCRC((eNODE_NAME)i);

                // load NVRam data to Ram mirror when CRC is correct
                WriteRamMirrorToNvram((eNODE_NAME)i);
                m_asNodeControlInfo[(eNODE_NAME)i].cFlag &= ~DIRTY;
                utilDataMgrUnlock((eNODE_NAME)i,
                                  FALSE);
            }
        }
    }
#endif
}

// ---------------------------------------------------------------------------
// This API locks the specified node for reading and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM
// ---------------------------------------------------------------------------
PVOID utilDataMgrReadLockData(eNODE_NAME eNodeID)
{

    BYTE *pacRamNodeAddress;
    //WORD  wMask;
    BOOL  bNodeLocked = FALSE;

    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);

    pacRamNodeAddress =
        (PBYTE)&m_sRamMirror + m_asNodeInfo[eNodeID].wNodeOffset;
    //wMask = 1 << (eNodeID % 16);

    while (!bNodeLocked)
    {
        // signal is available only if node is not being written
        //while (0 == utilExecSignalWaitAny(sgDATA_MGR + (eNodeID / 16),
        //                                  wMask,
        //                                  1))
        //{
        //    //;//  ==0  有人操作
        //    if ((WRITECOUNTER - 1) > m_asNodeControlInfo[eNodeID].cLockCounter)
        //        break;  //  是读操作
                        //APP_TRACE("Write locking %d\n");
                        //BSP_OS_TimeDlyMs(1);// 写占用，挂起

       // }


        //CPU_INT_DISABLE;

        // as long as it is not being written, the node is available for read
        if ((WRITECOUNTER - 1) > m_asNodeControlInfo[eNodeID].cLockCounter)
        {
            bNodeLocked = TRUE;
            m_asNodeControlInfo[eNodeID].cLockCounter++;
            //utilExecSignalClear(sgDATA_MGR + (eNodeID / 16),
            //                    wMask);
        }
        ASSERT(WRITECOUNTER > m_asNodeControlInfo[eNodeID].cLockCounter);

        // CPU_INT_ENABLE;
    }


    return (PVOID)pacRamNodeAddress;
}

// ---------------------------------------------------------------------------
// This API locks the specified node for writing and returns a pointer to the
// node data in the RAM mirror.
//
// Params:
//  eNodeID : The node identifier
//
// Return:
//  Address of the node data in RAM.
// ---------------------------------------------------------------------------
PVOID utilDataMgrWriteLockData(eNODE_NAME eNodeID)
{

    //WORD  wMask;
    BOOL  bNodeLocked = FALSE;
    BYTE *pacRamNodeAddress = NULL;
    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);
    if (0 == m_asNodeInfo[eNodeID].wNodeSize)
    {
        return NULL;
    }
    //APP_TRACE("WriteLockData\n");
    //wMask = 1 << (eNodeID % 16);


    while (!bNodeLocked)
    {
        // signal is available only if node is not being written
        //
        // while (0 == utilExecSignalWaitAny(sgDATA_MGR + (eNodeID / 16),
        //                                   wMask,
        //                                   1))
        //  {
        //      APP_TRACE("Some One Lock\n");
        //      BSP_OS_TimeDlyMs(1);// //  有人占用资源， 等待
        //  }

        // CPU_INT_DISABLE;

        // node is available for write only if it is not being read or written
        if (0 == m_asNodeControlInfo[eNodeID].cLockCounter)
        {
            bNodeLocked = TRUE;
            m_asNodeControlInfo[eNodeID].cLockCounter = WRITECOUNTER;
            // utilExecSignalClear(sgDATA_MGR + (eNodeID / 16),
            //                     wMask);
            pacRamNodeAddress =
                (PBYTE)&m_sRamMirror + m_asNodeInfo[eNodeID].wNodeOffset;
        }
        else
        {
            ASSERT_ALWAYS();
            //BSP_OS_TimeDlyMs(1);
        }

        //CPU_INT_ENABLE;
    }



    return (PVOID)pacRamNodeAddress;
}

// ---------------------------------------------------------------------------
// This API unlocks the specified node.
//
// Params:
//  eNodeID : The node identifier
//  bWrite  : TRUE releases the write lock for this node and task
//            (multi-threaded systems). The node will be marked dirty to force
//            an update to NVRAM.   FALSE releases read lock.
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUnlock(eNODE_NAME eNodeID,
                       BOOL       bWrite)
{
    //WORD                wMask;
    // validate arguments in debug mode only
    ASSERT(eNodeID < nnINVALID_NODE);
    //APP_TRACE("utilDataMgrUnlock %d\n",bWrite);
    //CPU_INT_DISABLE;
    if (bWrite)
    {
        // Has to be 255. This node has to be write locked.
        ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter == WRITECOUNTER);

        m_asNodeControlInfo[eNodeID].cLockCounter = 0;
        m_asNodeControlInfo[eNodeID].cFlag |= DIRTY;
//[kyq add: 20140517 10:12:10]
// wMask = 1 << (eNodeID % 16);
//   ASSERT((sgDATA_MGR + (eNodeID / 16))<sgWIN_MGR0); // kyq
//utilExecSignalSet(sgDATA_MGR + (eNodeID / 16),
//              wMask);
//[kyq End: 20140517 10:12:12]
    }
    else
    {
        if (WRITECOUNTER == m_asNodeControlInfo[eNodeID].cLockCounter)
        {
            // allows writing nothing
            m_asNodeControlInfo[eNodeID].cLockCounter = 0;
        }
        else
        {
            // Can't be 0. avoid unlock read on a node which is not read
            // locked
            ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter > 0);
            // Can't be 255. avoid unlock read on a node which is write locked
            ASSERT(m_asNodeControlInfo[eNodeID].cLockCounter < WRITECOUNTER);
            if (m_asNodeControlInfo[eNodeID].cLockCounter > 0) // add by kyq
            {
                m_asNodeControlInfo[eNodeID].cLockCounter--;
            }
//[kyq add: 20140517 10:13:31]
            if (m_asNodeControlInfo[eNodeID].cLockCounter == 0)
            {
                //wMask = 1 << (eNodeID % 16);
                // ASSERT((sgDATA_MGR + (eNodeID / 16))<sgWIN_MGR0); // kyq
                //utilExecSignalSet(sgDATA_MGR + (eNodeID / 16),
                //                  wMask);

            }
//[kyq End: 20140517 10:13:33]
        }
    }
    //CPU_INT_ENABLE;
    /* //[kyq cancel: 20140517 10:12:29]
wMask = 1 << (eNodeID % 16);
    ASSERT((sgDATA_MGR + (eNodeID / 16))<sgWIN_MGR0); // kyq
   utilExecSignalSet(sgDATA_MGR + (eNodeID / 16),
                     wMask);
*/ //[kyq End: 20140517 10:12:32]

}


bool_t NVRAMCtrlEventHandler(void)
{

           if (SiiPlatformTimerExpired(etNVRAM_UPDATE))
        {
            SiiPlatformTimerSet(etNVRAM_UPDATE, mS_TO_TICKS(NVRAM_UPDATE_DELAY),0xFF);     // Reset poll timer
            utilDataMgrUpdate(UPDATE_ALL_NODEID);
        }
    return 1;

}
bool_t   NVRAMRxMsgHandler (SendData_t *pMsg )
{


return true;

}

// ---------------------------------------------------------------------------
// This API enables or inhibits updates of the RAM mirror into non-volatile
// memory.
//
// Params:
//  bInhibit : TRUE to prevent updates, FALSE otherwise
//
// Return: none
// ---------------------------------------------------------------------------
void utilDataMgrUpdateInhibit(BOOL bInhibit)
{
    m_bUpdateInhibit = bInhibit;
}

#if 0
//
#define NVRAM_ADDR(NODE_NAME,NAME)    offsetof(S_##NODE_NAME,NAME),sizeof(NAME)

void utilDataMgrReadData(eNODE_NAME eNodeID,WORD wOffset,WORD size,BYTE *pData)
{
    WORD  wAddr = 0;
    wAddr = m_asNodeInfo[eNodeID].wNodeOffset+wOffset;
    dvNVRAMBytesRead(wAddr, size, pData);

}

utilDataMgrReadData(nnWINDOW_MGR_DATA,NVRAM_ADDR(nnSYSTEM_SETTING_DATA,cCONFIG_VALID),pcReadData);

#endif

//Huang 2016-Jul-11, 10:24:10
bool_t Init_NVRAM_module(BYTE MsgId)
{
    bool_t      success = true;
  //regit Non-Volatile Random Access Memory
  if(!NVRAMInitialize())
  	 success = false;
  if(!AppCallbackRegisterParser((eMESSAGE_QUEUES)MsgId,NVRAMRxMsgHandler ,false))   //注册事件,不轮训
  	 success = false;
   if(!AppCallbackRegisterTask((eMESSAGE_QUEUES)MsgId,NVRAMCtrlEventHandler ,true ))  //注册任务
  	 success = false;
  return success;
}




//[kyq End: 20150804 19:45:42]


