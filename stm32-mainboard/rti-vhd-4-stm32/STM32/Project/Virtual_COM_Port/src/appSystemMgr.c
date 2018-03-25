#include "si_datatypes.h"
#include "utilDataMgrAPI.h"
#include "appSystemMgrAPI.h"
#include "util.h"
#include "dvNVRAM.h"
#include "sii_api.h"

#define PRD_EDID_NAME "____"
extern char LCD1_IDLE_TEXT[17];
extern char LCD2_IDLE_TEXT[17];
extern char m_acProduct_Type[17];
extern char m_acProduct_Name[17];



extern uint8_t g_Init_EdidFlashEdidTable[];

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
            psSystemSettingData->bUartReturnMessage_EN = TRUE;           // Re_Message //系统信息反馈状态变?
            psSystemSettingData->bSystemLock = FALSE;                     // System  //系统锁定状态变量
             for (i = 0; i < MAX_IN; i++)
            psSystemSettingData->acIChannellock[i] = 0;           //对应输入通道解锁
            
            psSystemSettingData->cIR_Carrier_Mode = 0;                // IR_carrier_mode  1:红外载波强制 0:红外载波跟随
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
支持波特率2400；4800；9600；19200；38400；57600；115200；
*/
               psSystemSettingData->acRemoteBaudRate[i] = 3;
               //psSystemSettingData->bHDCP_AUTO[i] = TRUE;    // HDCP_AUTO_FLAG
            }
            psSystemSettingData->cDebugLevel = 0x00;
            memcpy(psSystemSettingData->acDefaultEdidProductName, PRD_EDID_NAME, strlen(PRD_EDID_NAME)); // 返回s的长度，不包括结束符NULL
           // memcpy(psSystemSettingData->acDefaultEdidProductName, PRD_EDID_NAME, strlen(PRD_EDID_NAME)); // 返回s的长度，不包括结束符NULL
                                                                                                         //psSystemSettingData->acLCDLine1DefaultTXT[16];              // LCD1_UPdata
                                                                                                         //psSystemSettingData->acLCDLine2DefaultTXT[16];              // LCD2_UPdata
            if (nveINITIALIZE == eEvent)
            {
               memcpy(psSystemSettingData->acLCDLine1DefaultTXT, PRD_TYPE, strlen(PRD_TYPE)); // 返回s的长度，不包括结束符NULL
               memcpy(psSystemSettingData->acLCDLine2DefaultTXT, PRD_NAME, strlen(PRD_NAME)); // 返回s的长度，不包括结束符NULL
            }
            else if (nveRESETALL == eEvent)
            {
               memcpy(psSystemSettingData->acLCDLine1DefaultTXT, LCD1_IDLE_TEXT, strlen(LCD1_IDLE_TEXT));
               memcpy(psSystemSettingData->acLCDLine2DefaultTXT, LCD2_IDLE_TEXT, strlen(LCD2_IDLE_TEXT));
            }
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
               psOutputPortMgrData->abHDMIAudio_EN[i] = SII_AUDIO__SPDIF;
               psOutputPortMgrData->abHDCP_EN[i] = TRUE;
               psOutputPortMgrData->abHDCP_AUTO[i] = TRUE;
#ifdef PRD_IR_OUT
               psOutputPortMgrData->acIRInPortSelect[i] = i; // from remote IR input to local IR out
               psOutputPortMgrData->acIRCloseInPortSelect[i] = 0xff;
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

