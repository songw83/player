/**
  ******************************************************************************
  * @file    TransportProtocol.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    8 - October - 2016
  * @brief   Header for all transport protocol
  ******************************************************************************
  **/
//------------------------------------------
//this protocol for bluetooth
#ifndef __TRANSPORT_PROTOCOL_H_H__
#define __TRANSPORT_PROTOCOL_H_H__
//------------------------------------------------------------
#define   PC_MCU_FLAG            1  //1:PC port   0:MCU port    

#if PC_MCU_FLAG
   typedef unsigned char uint8_t; 
#else
   #include  "stm32f10x.h"
#endif

#define   CMD_HEAD_VALUE                    0x80
//---------------------------------------
#define   COMMUNICATE_CONTENT_CNT           20
#define   COMMAND_HEAD_CNT                  3
#define   CONTENT_BEGIN_INDEX               3

#define   SET_POINT_CONTENT_LEN             2
#define   ACK_SET_SINGLE_POINT_CNT          6   
#define   COMMAND_COUNT                     8
enum{
    NO_ACTION = 0,                          // no action
    STATUS_DISABLE,                         // status disable
    STATUS_ENABLE,                          // status enable
};
enum{
    INVALID_COMMAND = 0,                    // invalid command
    CONNECT_COMMAND,                        // connect command
    SET_SINGLE_POINT_CONTENT,               // content command
    WRITE_24C16_COMMAND,                    // test write 24c16 command
    READ_24C16_COMMAND,                     // test read 24c16 command
//----------------------------    
    SET_SINGLE_POINT_COMMAND,               // set single point command
//---------------------------- 
    SET_GLOVE_CONTENT,                      // set glove content
    SET_MALE_HOSPITAL_CONTENT,              // set male content
//----------------------------
};
//---------------------------------------------------------------
#define   SINGLE_POINT_CONTENT_CNT          10
#define   SINGLE_POINT_REVERSE_CNT          ( COMMUNICATE_CONTENT_CNT - COMMAND_HEAD_CNT - SINGLE_POINT_CONTENT_CNT )
//----------------------------------------------------------------
typedef struct 
{
    uint8_t ChannelNum;                     //single point number
    uint8_t EShockSta;                      //eshock status                      
    uint8_t EShockPower;
    uint8_t EShockCnt;

    uint8_t HeatSta;                        //heat status 
    uint8_t HeatTemp;
    uint8_t HeatTimeCnt;

    uint8_t PShockSta;                      //pshock status
    uint8_t PShockPower;
    uint8_t PShockTimeCnt;

    uint8_t Reverse[ SINGLE_POINT_REVERSE_CNT ];
}SINGLE_POINT;
//------------------------------------------------------------------------------
#define   GLOVE_CONTENT_CNT                    17
#define   FIGURE_CNT                           5
typedef struct
{
    uint8_t  ChannelNum;                       //                        0   

	uint8_t  PShockSta1:2;                     // shock 1 status
	uint8_t  PShockSta2:2;                     // shock 2 status
	uint8_t  PShockSta3:2;                     // shock 3 status
	uint8_t  Reverse1:2;                       // reverse1 status        1

	uint8_t  PShockSta4:2;                     // shock 4 status
	uint8_t  PShockSta5:2;                     // shock 5 status
	uint8_t  EShockSta:2;                      // electronics status    
	uint8_t  Reverse2:2;                       // reverse2               2

	uint8_t  EShockPower;                      // electronics power      
	uint8_t  EShockCnt;                        // electronics count      4

    uint8_t  PShockPower[FIGURE_CNT];          // pshock power           9             
    uint8_t  PShockTimeCnt[FIGURE_CNT];        // pshock timecnt         14 

    uint8_t  HeatSta:2;                    // heat status            
    uint8_t  HeatTemp:4;                   // heat temp              
    uint8_t  Reverse3:2;                   // reverse3               15 

    uint8_t  HeatTimecnt;                  // heat timecnt           16
}GLOVE_CONTENT;                               
//---------------------------------------------------------------------
#define   MALE_HEAT_CONTENT_CNT                3
#define   MALE_TELESCOPIC_CONTENT_CNT          3
#define   MALE_HOSPITAL_CONTENT_CNT            ( MALE_HEAT_CONTENT_CNT + MALE_TELESCOPIC_CONTENT_CNT )

#define   MALE_HOSPITAL_REVERSE_CNT            ( COMMUNICATE_CONTENT_CNT - MALE_HOSPITAL_CONTENT_CNT - COMMAND_HEAD_CNT )
//---------------------------------------------------------------------
typedef struct 
{
    uint8_t HeatSta;                                                   
    uint8_t HeatTemp;                        
    uint8_t HeatTimeLevel;                     //2
    
	uint8_t TelescopicSta; 
	uint8_t TelescopicMode;
	uint8_t TelescopicPower;                   //5

    uint8_t Reverse[MALE_HOSPITAL_REVERSE_CNT];
    
}MALE_CONTENT;
//------------------------------------------------------------------------------
typedef union  __OPERATE_CONTENT__
{ 
   SINGLE_POINT      SinglePoint;              // control single 
   GLOVE_CONTENT     GloveContent;             // glove
   MALE_CONTENT      MailContent;              // male content
   
}OPERATE_CONTENT;
//------------------------------------------------------
typedef union __COMMUNICATE_CONTENT__                        //20160814  
{
   uint8_t Buffer[COMMUNICATE_CONTENT_CNT];
   struct
   {
      uint8_t HeadTmp;                            // head tmp            //...0
      uint8_t CmdTmp;                             // cmd tmp             //...1
      uint8_t Length;                             // content length      //...2 
      OPERATE_CONTENT  Operate_Content;           // operate content
      
   }Content;
}COMMUNICATE_CONTENT;
//------------------------------------------------------------------------------
#endif   //__TRANSPORT_PROTOCOL_H_H__ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
