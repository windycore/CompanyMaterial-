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
    BYTE Flags;     // ָ����
    char*   ucmd;   // ָ�����ָ�ͷ����
    int     cmdlen; // ָ���
    int     cmdoffset;  // ָ��ƫ������һ�㶼��ָ���β���ĳ���
    CmdFun_t    CmdFun; // ָ���
}stCmdListType;

INTERFACE stCmdListType CmdListToMode1[];
INTERFACE stCmdListType CmdListToMode2[];

INTERFACE int GetCmdListToMode1_Len(void);
INTERFACE int GetCmdListToMode2_Len(void);

#endif //#ifndef _CMDFUNCTION_H_
