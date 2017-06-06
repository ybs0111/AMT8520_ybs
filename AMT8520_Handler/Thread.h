#ifndef _AFX_THREAD_H__
#define _AFX_THREAD_H__

// ******************************************************************************
// ������ �� ������ �ڵ� ���� ���� ����                                          
// ******************************************************************************

enum 
{
	THREAD_HANDLER_CHK,
	THREAD_TOWER_LAMP_CHK,
	THREAD_ALARM_DISPLAY,
	THREAD_IO,
	THREAD_NETWORK,
	THREAD_MOTORS,
	
	THREAD_BIN_STACKER_1,
	THREAD_BIN_STACKER_2,
//	THREAD_BIN_STACKER_3,//ybs
	THREAD_BIN_STACKER_REJECT,
	THREAD_COK_ROBOT,
	THREAD_REJECT_STACKER,
	THREAD_REJECT_TRAY_TRANSFER,
	THREAD_ROBOT,
	THREAD_TEST_SITE_1,
	THREAD_TEST_SITE_2,
	
	THREAD_COUNT,
};

extern CWinThread  *m_thread[THREAD_COUNT];
extern HANDLE		hThrHandle[THREAD_COUNT];
// ******************************************************************************

// ******************************************************************************
// ���� ������ �Լ� �����Ѵ�                                                     
// ******************************************************************************
UINT OnThread_Handler_Check(LPVOID  lParam);		
UINT OnThread_Tower_Lamp_Check(LPVOID  lParam);		
UINT OnThread_Alarm_Display(LPVOID  lParam);		
UINT OnThread_IO(LPVOID  lParam);					
UINT OnThread_Network(LPVOID lParam);
UINT OnThread_Motors(LPVOID lParam);					
// ******************************************************************************

// ******************************************************************************
//  ���� ������ �Լ� �����Ѵ�                                                     
// ******************************************************************************
UINT OnThread_Bin_Stacker_Reject(LPVOID  lParam);	
UINT OnThread_Bin_Stacker_1(LPVOID  lParam);		
UINT OnThread_Bin_Stacker_2(LPVOID  lParam);		
//UINT OnThread_Bin_Stacker_3(LPVOID  lParam);		
UINT OnThread_COK_Robot(LPVOID  lParam);			
UINT OnThread_Reject_Stacker(LPVOID  lParam);		
UINT OnThread_Reject_Tray_Transfer(LPVOID  lParam);	
UINT OnThread_Robot(LPVOID  lParam);				
UINT OnThread_Test_Site_1(LPVOID  lParam);			
UINT OnThread_Test_Site_2(LPVOID  lParam);			
			
// ******************************************************************************

extern int All_Stop;  // ������ ���� �÷���

#endif