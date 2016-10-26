#include "FeelClothControlHandle.h"
#include "windows.h"
#include <assert.h>
#include <QDebug>

#define  DEFAULT_BADURATE              115200
#define  DEFAULT_BYTESIZE              8
#define  DEFAULT_PARITY                NOPARITY
#define  DEFAULT_STOPBITS              ONESTOPBIT     
#define  DEFAULT_WRITEBUFFERSIZE       8192  
#define  DEFAULT_READBUFFERSIZE        8192  

#define  WAIT_CONNECT_TIME_CNT         500 
#define  WAIT_SET_POINT_NUM_TIMECNT    500
//
// close port 
//
void CFeelClothCommunicate::ClosePort()
{
//	SetEvent(m_hShutdownEvent);
	if(HandleBag.ComOpenFlag)
	{
	   ThreadFlag=FALSE;
	   HandleBag.ComOpenFlag=FALSE;
	   SetEvent( ReadOverlapped.hEvent );
       SetEvent( WriteOverlapped.hEvent );
	   Sleep(20);

       if(read_thread.joinable())
           read_thread.join();
       if(write_thread.joinable())
           write_thread.join();

       CloseHandle(m_hComm);
       m_hComm=INVALID_HANDLE_VALUE;
    }
}

//
// Constructor
//
CFeelClothCommunicate::CFeelClothCommunicate():read_thread(), write_thread()
{
//-------------------------------------------
//  com base configuration 
	ComConfiguration.BaudRate=115200;
	ComConfiguration.ByteSize=8;
	ComConfiguration.Parity=NOPARITY;
	ComConfiguration.StopBits=ONESTOPBIT;
	ComConfiguration.ReadBufferSize=DEFAULT_WRITEBUFFERSIZE;
	ComConfiguration.WriteBufferSize=DEFAULT_READBUFFERSIZE;
//-----------------------
//	receive one byte handler
    FunctionBack = DataReceiveHandler;
	MPData = this;
	GetBackDataEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	CntEverySend = CNT_EVERY_SEND;

    m_hComm=INVALID_HANDLE_VALUE;
    ThreadFlag=FALSE;
    HandleBag.ComOpenFlag=FALSE;
}

//
// Delete dynamic memory
//
CFeelClothCommunicate::~CFeelClothCommunicate()
{
    ClosePort();
}

//
//
//
BOOL CFeelClothCommunicate::OpenSerialPort()
{
   char *szPort = new char[50];
   int i;
   for(i=0;i<50;i++)
   {
      szPort[i]=0x00;
   }
//-------------------------------------------------
   if(HandleBag.ComIndex<10)
   {
      szPort[0]='C';
      szPort[1]='O';
      szPort[2]='M';
      switch(HandleBag.ComIndex)
      {
         case 0:szPort[3]='0';break;
         case 1:szPort[3]='1';break;
         case 2:szPort[3]='2';break;
         case 3:szPort[3]='3';break;
         case 4:szPort[3]='4';break;
         case 5:szPort[3]='5';break;
         case 6:szPort[3]='6';break;
         case 7:szPort[3]='7';break;
         case 8:szPort[3]='8';break;
         case 9:szPort[3]='9';break;
      }
   }
   else
   {
      szPort[0] = '\\';
      szPort[1] = '\\';
      szPort[2] = '.';
      szPort[3] = '\\';
      szPort[4] = '\\';
      szPort[5] = 'C';
      szPort[6] = 'O';
      szPort[7] = 'M';
      switch(HandleBag.ComIndex)
      {
         case 10:szPort[8]='1';
			     szPort[9]='0';
			     break;
         case 11:szPort[8]='1';
			     szPort[9]='1';
			     break;
         case 12:szPort[8]='1';
			     szPort[9]='2';
			     break;
         case 13:szPort[8]='1';
			     szPort[9]='3';
			     break;
         case 14:szPort[8]='1';
			     szPort[9]='4';
			     break;
         case 15:szPort[8]='1';
			     szPort[9]='5';
			     break;
      }
   }
   // sprintf(szPort,"COM%d",ComIndex);
   // get a handle to the port
   m_hComm = CreateFileA(szPort,                         // communication port string (COMX)
                        GENERIC_READ | GENERIC_WRITE,   // read/write types
                        0,                              // comm devices must be opened with exclusive access
                        NULL,                           // no security attributes
                        OPEN_EXISTING,                  // comm devices must use OPEN_EXISTING
                        FILE_FLAG_OVERLAPPED,           // Async I/O
                        0);	                            // template must be 0 for comm devices

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
		delete [] szPort;
	    HandleBag.ComOpenFlag=FALSE;
		return FALSE;
	}
	delete [] szPort;

    SetCommMask(m_hComm,EV_RXCHAR|EV_TXEMPTY );
	SetupComm(m_hComm,ComConfiguration.ReadBufferSize,ComConfiguration.WriteBufferSize) ; //设置输入、输出缓冲区的大小
    PurgeComm(m_hComm,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//清干净输入、输出缓冲区
 
    DCB   dcb ; 
    GetCommState(m_hComm, &dcb ) ; //读串口原来的参数设置
	dcb.BaudRate =ComConfiguration.BaudRate;
	dcb.ByteSize =ComConfiguration.ByteSize;
	dcb.Parity = ComConfiguration.Parity;
	dcb.StopBits = ComConfiguration.StopBits ;
	dcb.fRtsControl = ComConfiguration.RtsEnableFlag;
    SetCommState(m_hComm, &dcb ) ; //串口参数配置
    PurgeComm(m_hComm,PURGE_TXCLEAR|PURGE_RXCLEAR);
//----------------------------------------------------
    ThreadFlag=TRUE;
    HandleBag.ComOpenFlag=TRUE;
	StartMonitorThread();
	return TRUE;
}
//
//
//
void CFeelClothCommunicate::StartMonitorThread()
{
    memset(&WriteOverlapped,0,sizeof(OVERLAPPED));
    WriteOverlapped.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
    
    memset(&ReadOverlapped,0,sizeof(OVERLAPPED));
    ReadOverlapped.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

    //创建写报告的线程
    StartWriteThread();

    //创建一个读报告的线程
    StartReadThread();

	ResetEvent(ReadOverlapped.hEvent);
	ResetEvent(WriteOverlapped.hEvent);
}

//
//
//
UINT  CFeelClothCommunicate::WriteComThread(LPVOID pParam)
{
    if (pParam == NULL)return 0;
	CFeelClothCommunicate *pData = (CFeelClothCommunicate*)pParam;
    while(pData->ThreadFlag)
	{
        ResetEvent(pData->WriteOverlapped.hEvent);                   //设置事件为无效状态
	    WaitForSingleObject(pData->WriteOverlapped.hEvent,INFINITE); //等待事件触发
        ResetEvent(pData->WriteOverlapped.hEvent);                   //设置事件为无效状态
		//pData->HandleContinueSend();                                 //Handle Continue
	}
	return 0;
}
//
//
//
UINT  CFeelClothCommunicate::ReadComThread(LPVOID pParam)
{
    if (pParam == NULL)return 0;
	CFeelClothCommunicate *pData = (CFeelClothCommunicate*)pParam;

    BYTE readdat;
    while(1)
    {
		if(pData->ThreadFlag == FALSE)
       {
          break;  
       }
	   if(pData->HandleBag.ComOpenFlag)
	   {
			if(pData->m_hComm!=INVALID_HANDLE_VALUE) 
			{
				ReadFile(pData->m_hComm,&readdat,1,NULL,&pData->ReadOverlapped);
			}
			WaitForSingleObject(pData->ReadOverlapped.hEvent,INFINITE);

     		if(pData->FunctionBack != NULL)
			{
				pData->FunctionBack(readdat,pData->MPData);
			}
	   }
    }
	return 0;
}




void CFeelClothCommunicate::StartReadThread()
{
    // This will start the thread. Notice move semantics!
    read_thread = std::thread(&ReadComThread, this);
}

void CFeelClothCommunicate::StartWriteThread()
{
    // This will start the thread. Notice move semantics!
    write_thread = std::thread(&WriteComThread, this);
}

//
//
//
void CFeelClothCommunicate::WriteToPort(unsigned char* string,int scount)
{		
	DWORD dwBytesWrite;
    WriteFile(m_hComm,string,scount,&dwBytesWrite,&WriteOverlapped); 
}
//
//
//
void CFeelClothCommunicate::HandleContinueSend()
{
    DWORD dwBytesWrite;
	//int sendcount;

	if(WritePortIndex >= WritePortCnt)
	{
		HandleBag.DataInSending = FALSE;
		return;
	}

	WriteFile(m_hComm,&WritePortPoint[WritePortIndex],CntEverySend,&dwBytesWrite,&WriteOverlapped); 
	WritePortIndex = WritePortIndex + CntEverySend;
}
//
//
//
void __stdcall DataReceiveHandler(BYTE ribuf,LPVOID pdata)
{
	unsigned char step;
	if(pdata == NULL)
		return;
	CFeelClothCommunicate *pdat = (CFeelClothCommunicate*)pdata;
    
	if( ribuf == CMD_HEAD_VALUE )
	{
		pdat->HandleBag.RStep = 1;
		pdat->HandleBag.ReceiveBag.Buffer[0] = ribuf;
		return;
	}
	else
	{
	    step = pdat->HandleBag.RStep;
		switch(step)
		{
		   case 1:pdat->HandleBag.ReceiveBag.Buffer[1] = ribuf;
			      step++;
			      break;
		   case 2:pdat->HandleBag.ReceiveBag.Buffer[2] = ribuf;
			      pdat->HandleBag.ReceiveCnt = ribuf;
				  pdat->HandleBag.ReceiveIndex = 0;
			      step++;
			      break;
		   case 3:pdat->HandleBag.ReceiveBag.Buffer[CONTENT_BEGIN_INDEX + pdat->HandleBag.ReceiveIndex] = ribuf;
			      pdat->HandleBag.ReceiveIndex++;
				  if( pdat->HandleBag.ReceiveIndex == pdat->HandleBag.ReceiveCnt )
				  {
                     pdat->HandleBag.ReceiveFlag = TRUE;
					 step = 0;
				  }
			      break;
		   default:break;
		}
	}
    pdat->HandleBag.RStep = step;
}
//
//
//
BOOL CFeelClothCommunicate::AutoConnectSerialPort()
{
    int i;

	ClosePort();
	for(i=1;i<17;i++)
	{
       HandleBag.ComIndex=i;
       if( OpenSerialPort() == TRUE )
	   {
          if(ConnectFeelCloth()==TRUE)
		  {
             return TRUE;
		  }
		  else
          {
		     ClosePort();
		  }
	   }
	}
	return FALSE;
}
//
//
//
BOOL CFeelClothCommunicate::ConnectFeelCloth()
{
	int i;
	int currenttick;
	unsigned char connectstr[15]="CONNECT DEVICE";
	for(i=0;i<COMMUNICATE_CONTENT_CNT;i++)
	{
		HandleBag.SendBag.Buffer[i]=0;
	}
	HandleBag.SendBag.Content.HeadTmp=0x80;

	HandleBag.SendBag.Content.CmdTmp = CONNECT_COMMAND;
	HandleBag.SendBag.Content.Length = 14; 
	for(i=0;i<14;i++)
	{
	    HandleBag.SendBag.Buffer[i+3]=connectstr[i];
	}

	HandleBag.ReceiveFlag = FALSE;
	HandleBag.RStep=0;

	currenttick = GetTickCount();
	WriteToPort(HandleBag.SendBag.Buffer,COMMUNICATE_CONTENT_CNT);

	while(1)
	{
		if(((GetTickCount()-currenttick) > WAIT_CONNECT_TIME_CNT) || (HandleBag.ReceiveFlag == TRUE))
		   break;

	}
	currenttick = GetTickCount()-currenttick;
	return HandleBag.ReceiveFlag;
}
//
//  发送控制的内容
//
BOOL   CFeelClothCommunicate::SendControlContent( unsigned char cmdindex )
{
	unsigned char cmdcnttab[COMMAND_COUNT] = 
	{
		0,
		0,
		SINGLE_POINT_CONTENT_CNT,
		0,
		0,
		0,
		GLOVE_CONTENT_CNT,
		MALE_HOSPITAL_CONTENT_CNT
	};
 
 	if( (cmdindex == SET_SINGLE_POINT_CONTENT) || 
		(cmdindex == SET_GLOVE_CONTENT) ||
        (cmdindex == SET_MALE_HOSPITAL_CONTENT) 	)
	{
	   HandleBag.SendBag.Content.CmdTmp = cmdindex;
	   HandleBag.SendBag.Content.Length = cmdcnttab[cmdindex];
 	}
	else
	{
	   return FALSE;
	}
	HandleBag.SendBag.Content.HeadTmp = CMD_HEAD_VALUE;
//--------------------------------------------------------------------------------
	HandleBag.ReceiveFlag = FALSE;
	HandleBag.RStep=0;
	WriteToPort(HandleBag.SendBag.Buffer,COMMUNICATE_CONTENT_CNT);
	return TRUE;
}

//
//  设置单一的节点
//
BOOL CFeelClothCommunicate::SetSinglePoint()
{
	int currrentcnt;
 	HandleBag.SendBag.Content.HeadTmp = CMD_HEAD_VALUE;
	HandleBag.SendBag.Content.CmdTmp = SET_SINGLE_POINT_COMMAND;
	HandleBag.SendBag.Content.Length = SET_POINT_CONTENT_LEN;
//--------------------------------------------------------------------------------
	HandleBag.ReceiveFlag = FALSE;
	HandleBag.RStep=0;
	WriteToPort(HandleBag.SendBag.Buffer,COMMUNICATE_CONTENT_CNT);

    currrentcnt = GetTickCount();
	while(1)
	{
       if(((GetTickCount()-currrentcnt) > WAIT_SET_POINT_NUM_TIMECNT) || (HandleBag.ReceiveFlag))
         break;
	}
	currrentcnt = GetTickCount() - currrentcnt;
	return HandleBag.ReceiveFlag;
}
//
//
//
void CFeelClothCommunicate::ClearSendBag()
{
   int i;
   for(i=0;i<COMMUNICATE_CONTENT_CNT;i++)
   {
	   HandleBag.SendBag.Buffer[i]=0x00;
   }
}
//
//  send datbuffer
//
void CFeelClothCommunicate::SendBuffer( unsigned char *datbuf,int count )
{
    int i;
	if( count>20 )
       return;

	for(i = 0;i<count;i++)
	{
       HandleBag.SendBag.Buffer[i] = datbuf[i];
	}

	WriteToPort(HandleBag.SendBag.Buffer,count);
}
