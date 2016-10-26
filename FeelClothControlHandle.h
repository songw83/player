#include "windows.h"
#include <thread>

#ifndef __FEEL_CLOTH_COMMUNICATE_H_H__
#define __FEEL_CLOTH_COMMUNICATE_H_H__

#include "TransportProtocol.h"

//----------------------------------------
#define   CNT_EVERY_SEND           1
//----------------------------------------
typedef struct __COMMUNICATE_STRUCT__  
{
	BOOL ComOpenFlag;     //串口打开标志
	BOOL DataInSending;   //数据正在发送中
    BOOL SendFlag;        //发送标志
    BOOL ReceiveFlag;     //接收处理标志
	int ComIndex;         //串口索引
    int RStep;            //串口接收step  

	int ReceiveCnt;       //receive count 
	int ReceiveIndex;     //receive index

    COMMUNICATE_CONTENT ReceiveBag;
    COMMUNICATE_CONTENT SendBag;
    
}COMMUNICATE_STRUCT;
//-------------
typedef struct
{
   BYTE   ByteSize; 
   BYTE   Parity; 
   BYTE   StopBits; 
   BYTE   RtsEnableFlag;
   DWORD  BaudRate;
   DWORD  WriteBufferSize;
   DWORD  ReadBufferSize;
	 
}COMCONFIGURATION;
//-----------------------------------------
typedef  void (__stdcall* FnCallBack)(BYTE ribuf,LPVOID pData);
class CFeelClothCommunicate
{														 
public:
	int WritePortIndex;
	int WritePortCnt;
	int CntEverySend;
	unsigned char *WritePortPoint;
    
	COMMUNICATE_STRUCT HandleBag;
	bool ThreadFlag;

    OVERLAPPED WriteOverlapped;       //send report use
    OVERLAPPED ReadOverlapped;        //read report use

    HANDLE m_hComm;
	HANDLE GetBackDataEvent;

    COMCONFIGURATION  ComConfiguration;
    void ClosePort();
	// contruction and destruction
    CFeelClothCommunicate();
    virtual ~CFeelClothCommunicate();

	friend void __stdcall DataReceiveHandler(BYTE ribuf,LPVOID pData);
	FnCallBack  FunctionBack;
	void  *MPData;

	void   SendBuffer(unsigned char *datbuf,int count);
	BOOL   AutoConnectSerialPort();
	BOOL   ConnectFeelCloth();
    BOOL   SetSinglePoint();
	void   ClearSendBag();
	BOOL   SendControlContent(unsigned char cmdindex);

	BOOL   OpenSerialPort();
	void   WriteToPort(unsigned char *string,int scount);
	void   HandleContinueSend();

protected:
	void   StartMonitorThread();

    void StartReadThread();
    void StartWriteThread();

    static UINT   WriteComThread(LPVOID pParam);
    static UINT   ReadComThread(LPVOID pParam);

    std::thread read_thread;
    std::thread write_thread;
};

#endif //__FEEL_CLOTH_COMMUNICATE_H_H__


