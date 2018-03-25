#ifndef _CMDFUNCTION_H_
#define _CMDFUNCTION_H_

#ifdef _CMDFUNCITON_C_
#define INTERFACE   
#else
#define INTERFACE   extern
#endif

typedef BYTE (* CmdFun_t)(unsigned char *USART1_rxtailpoint, unsigned char *USART1_rxprocesspoint);

#define Flag_WholeWord      0x01
#define Flag_HeadMatch      0x02
#define Flag_MiddleMatch    0x04
#define Flag_SternMatch     0x08
#define Flag_End  0xFF

#define UserOffset      0
#define UseStrlen       0

typedef struct _stCmdListType
{
    BYTE Flags;     // 指令标号
    char*   ucmd;   // 指令或者指令开头部分
    int     cmdlen; // 指令长度
    int     cmdoffset;  // 指令偏移量，一般都是指令结尾符的长度
    CmdFun_t    CmdFun; // 指令功能
}stCmdListType;

INTERFACE stCmdListType CmdListToMode1[];
INTERFACE stCmdListType CmdListToMode2[];

INTERFACE int GetCmdListToMode1_Len(void);
INTERFACE int GetCmdListToMode2_Len(void);

#endif //#ifndef _CMDFUNCTION_H_
