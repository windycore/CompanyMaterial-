#include "dvM21452.h"
#include "BSP_I2C.h"
#include "mcu.h"
#include "bsp_Gpio.h"
#include "Util.h"
#include "utilDataMgrAPI.h"
#include "OutputHandle.h"

#if(MACHINETYPE==MUH44A_H2)

unsigned char  OutRegMap[MAX_OUT]=			//输出通道0-4和M21452输出寄存器的对应关系
{
	0x0f  ,0x0e  ,0x13  ,0x12
};

unsigned char  InMap[8]=				//输入通道和连接到M21452实际物理引脚的对应关系
{
	4  ,5  ,6  ,7 ,0  ,1  ,2  ,3
};

unsigned char  OutCfgReg[8]=			//M21452实际物理输出通道寄存器表
{
	0x44  ,0x43  ,0x42  ,0x41, 0x4c  ,0x4b  ,0x4a  ,0x49
};

static const BYTE m_acInstanceAddress[M21452_NUM_INSTANCES] =
{
    0x42,   // U4 的地址					U4:0100 0010   U5:0101 1010
#if(M21452_NUM_INSTANCES>1)
    0x5a,   // U5 的地址
#endif
};

static const BYTE m_acBusID[M21452_NUM_INSTANCES] =
{
    eI2CBusM21452_EEPROM,
#if(M21452_NUM_INSTANCES>1)
    eI2CBusM21452_EEPROM,
#endif
};

//设备 6  M21452
eRESULT dvM21452Write(BYTE  cInstance, uint8_t addr, uint8_t value)
{
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusWrite(m_acBusID[cInstance],
                               m_acInstanceAddress[cInstance],
                               addr,
                               1,
                               &value,
                               0);
    return eResult;
}

uint8_t dvM21452Read(BYTE  cInstance,  uint8_t addr)
{
    BYTE cValue = 0;
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusRead(m_acBusID[cInstance],
                              m_acInstanceAddress[cInstance],
                              addr,
                              1,
                              &cValue,
                              0);
    ASSERT(eResult == rcSUCCESS);
    return cValue;
}

BYTE dvM21452Init(void)
{
  unsigned char i;
  //unsigned char j;
 
   for (i = 0; i < M21452_NUM_INSTANCES; i++)      //Intialize M21452
   {
    	dvM21452Write(i , 0xff, 0xaa); 		//将M21452 软件复位
    	delay_nms(10);
    	dvM21452Write(i , 0xff, 0x00);
    	delay_nms(1);
    	dvM21452Write(i , 0x03, 0x20);    
    	delay_nms(1);
    	dvM21452Write(i , 0x05, 0x00);
    	delay_nms(1);
		
	//for (j= 0; j< 8; j++)			//输入EQ设置
		//dvM21452Write(i , 0x1d+j, 0x00);
   }
   return true;
}



//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvM21452SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;
    // CH1|CH0            CH3|CH2
    // INPUTCH2|INPUTCH3, INPUTCH0|INPUTCH1
    BYTE         cMapCh; // = {INPUTCH2|INPUTCH3,INPUTCH0|INPUTCH1}
                            // Start the varargs processing.
   
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
	appInputHDMI14_20DetectAndOutputProcessTemp(cOutput);
	
       if(cInput < MAX_IN)
        {
	     cMapCh = (InMap[2*cInput]&0x0f) | (InMap[2*cInput+1]<<4);
            dvM21452Write(0, OutRegMap[cOutput], cMapCh);			//先确立M21452内部输入输出的关系
	     delay_nus(10);
	     dvM21452Write(1, OutRegMap[cOutput], cMapCh);           
            delay_nus(10);
	     dvM21452Write(0, OutCfgReg[2*cOutput], 0x60);  		//打开第一块M21452的输出通道
	     dvM21452Write(0, OutCfgReg[2*cOutput+1], 0x60);
	     dvM21452Write(1, OutCfgReg[2*cOutput], 0x60);  		//打开第二块M21452的输出通道
	     dvM21452Write(1, OutCfgReg[2*cOutput+1], 0x60);
        }
	 else		//关闭某路输出通道
	 {
	     dvM21452Write(0, OutCfgReg[2*cOutput], 0);  		//关闭第一块M21452的输出通道
	     dvM21452Write(0, OutCfgReg[2*cOutput+1], 0);
	     dvM21452Write(1, OutCfgReg[2*cOutput], 0);  		//关闭第二块M21452的输出通道
	     dvM21452Write(1, OutCfgReg[2*cOutput+1], 0);
	 }
    }
    dvM21452Write(0 , 0x05, 0x15);	//updata
    dvM21452Write(1 , 0x05, 0x15);	//updata
    
}


//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvM21452SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;

  
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        dvM21452Write(0, OutCfgReg[2*cOutput], 0);  	//0P	//关闭第一块M21452的输出通道
	// dvM21452Write(0, OutCfgReg[2*cOutput+1], 0);  //CP
	 dvM21452Write(1, OutCfgReg[2*cOutput], 0);  //  1P		//关闭第二块M21452的输出通道
	 dvM21452Write(1, OutCfgReg[2*cOutput+1], 0);//  2P

    }
}

#else				//MUH88A-H2

unsigned char  OutRegMap[MAX_OUT]=			//输出通道0-7和M21452输出寄存器的对应关系
{
	0x0f  ,0x0f  ,0x0e  ,0x0e  ,0x13  ,0x13  ,0x12 ,0x12
};

unsigned char  InMap[8]=				//输入通道和连接到M21452实际物理引脚的对应关系
{
	4  ,5  ,6  ,7 ,0  ,1  ,2  ,3
};

unsigned char  OutCfgReg[8]=			//M21452实际物理输出通道寄存器表
{
	0x44  ,0x43  ,0x42  ,0x41, 0x4c  ,0x4b  ,0x4a  ,0x49
};

static const BYTE m_acInstanceAddress[M21452_NUM_INSTANCES] =
{
    0x42,   // U12 的地址				U12:0100 0010		U11:0100 0100		U10:0100 0110		U9:0100 1010
#if(M21452_NUM_INSTANCES>1)
    0x44,   // U11 的地址
#endif
#if(M21452_NUM_INSTANCES>2)
    0x46,   // U10 的地址
#endif
#if(M21452_NUM_INSTANCES>3)
    0x4a,   // U9 的地址
#endif
};

static const BYTE m_acBusID[M21452_NUM_INSTANCES] =
{
    eI2CBusM21452_EEPROM,
#if(M21452_NUM_INSTANCES>1)
    eI2CBusM21452_EEPROM,
#endif
#if(M21452_NUM_INSTANCES>2)
    eI2CBusM21452_EEPROM,
#endif
#if(M21452_NUM_INSTANCES>3)
    eI2CBusM21452_EEPROM,
#endif
};

//设备 6  M21452
eRESULT dvM21452Write(BYTE  cInstance, uint8_t addr, uint8_t value)
{
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusWrite(m_acBusID[cInstance],
                               m_acInstanceAddress[cInstance],
                               addr,
                               1,
                               &value,
                               0);
    return eResult;
}

uint8_t dvM21452Read(BYTE  cInstance,  uint8_t addr)
{
    BYTE cValue = 0;
    eRESULT eResult = rcSUCCESS;
    eResult = dvDeviceBusRead(m_acBusID[cInstance],
                              m_acInstanceAddress[cInstance],
                              addr,
                              1,
                              &cValue,
                              0);
    ASSERT(eResult == rcSUCCESS);
    return cValue;
}

BYTE dvM21452Init(void)
{
  unsigned char i;
  //unsigned char j;
 
   for (i = 0; i < M21452_NUM_INSTANCES; i++)      //Intialize M21452
   {
    	dvM21452Write(i , 0xff, 0xaa); 		//将M21452 软件复位
    	delay_nms(10);
    	dvM21452Write(i , 0xff, 0x00);
    	delay_nms(1);
    	dvM21452Write(i , 0x03, 0x20);    
    	delay_nms(1);
    	dvM21452Write(i , 0x05, 0x00);
    	delay_nms(1);
		
	//for (j= 0; j< 8; j++)			//输入EQ设置
		//dvM21452Write(i , 0x1d+j, 0x00);
   }
   return true;
}



//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvM21452SwitcherPortSet(BYTE cInput, BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;
    BYTE         cMapCh;
    POUTPUTPORT_MANAGER_DATA    psOutputPortMgrData;
    psOutputPortMgrData  = (POUTPUTPORT_MANAGER_DATA)utilDataMgrReadLockData(nnOUTPUTPORT_MANAGER_DATA);

    utilDataMgrUnlock(nnOUTPUTPORT_MANAGER_DATA,
                      FALSE);
   
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        appInputHDMI14_20DetectAndOutputProcessTemp(cOutput);
       if(cInput < MAX_IN)
        {     
	     if(cOutput%2)			//奇数
	     {
             		cMapCh = (InMap[cInput]<<4) | (InMap[psOutputPortMgrData->acInPortSelect[cOutput-1]]&0x0f);
	     }
      	     else					//偶数
            {
			cMapCh = (InMap[cInput]&0x0f) | (InMap[psOutputPortMgrData->acInPortSelect[cOutput+1]]<<4);
	     }
	     
            dvM21452Write(0, OutRegMap[cOutput], cMapCh);			//先确立M21452内部输入输出的关系
	     delay_nus(10);
	     dvM21452Write(1, OutRegMap[cOutput], cMapCh);  
	     delay_nus(10);
	     dvM21452Write(2, OutRegMap[cOutput], cMapCh); 
	     delay_nus(10);
	     dvM21452Write(3, OutRegMap[cOutput], cMapCh); 
            delay_nus(10);
	     dvM21452Write(0, OutCfgReg[cOutput], 0x60);  		//打开第一块M21452的输出通道
	     dvM21452Write(1, OutCfgReg[cOutput], 0x60);  		//打开第二块M21452的输出通道
	     dvM21452Write(2, OutCfgReg[cOutput], 0x60);  		//打开第三块M21452的输出通道
	     dvM21452Write(3, OutCfgReg[cOutput], 0x60);  		//打开第四块M21452的输出通道
        }
	 else		//关闭某路输出通道
	 {
	     dvM21452Write(0, OutCfgReg[cOutput], 0);  		//关闭第一块M21452的输出通道
	     dvM21452Write(1, OutCfgReg[cOutput], 0);  		//关闭第二块M21452的输出通道
	     dvM21452Write(2, OutCfgReg[cOutput], 0);  		//关闭第三块M21452的输出通道
	     dvM21452Write(3, OutCfgReg[cOutput], 0);  		//关闭第四块M21452的输出通道
	 }
    }
    dvM21452Write(0 , 0x05, 0x15);	//updata
    dvM21452Write(1 , 0x05, 0x15);	//updata
    dvM21452Write(2 , 0x05, 0x15);	//updata
    dvM21452Write(3 , 0x05, 0x15);	//updata
}


//把输入口cInput 切到指定的输出口上
//cInput: 输入端口号,Begin from 0, MAX_IN will be mute the output
//cPortNumber: 输出端口数目 Begin from 1
//...  : 输出端口号
void dvM21452SwitcherClosePort( BYTE cPortNumber,PBYTE pcOutPortList)
{
    BYTE         i;
    BYTE         cOutput;

  
    for (i = 0; i < cPortNumber; i++)
    {
        cOutput  = pcOutPortList[i];
        dvM21452Write(0, OutCfgReg[cOutput], 0);  		//关闭第一块M21452的输出通道
	 dvM21452Write(1, OutCfgReg[cOutput], 0);  		//关闭第二块M21452的输出通道
	 dvM21452Write(2, OutCfgReg[cOutput], 0);  		//关闭第三块M21452的输出通道
	 dvM21452Write(3, OutCfgReg[cOutput], 0);  		//关闭第四块M21452的输出通道

    }
}

#endif

void dvM21452PowerSet(BOOL bPowerOn)
{
}

