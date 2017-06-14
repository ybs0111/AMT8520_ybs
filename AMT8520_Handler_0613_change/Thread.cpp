// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iostream.h"
#include "afxmt.h"
#include "Thread.h"

#include "Variable.h"  // 전역 변수 정의 클래스

//******************************************************************************
//쓰레드 동작 관련 클래스 추가                                                  
//******************************************************************************
#include "Run_Handler_Check.h"
#include "Run_TowerLamp.h"
#include "RUN_IO_ReadWrite.h"

#include "Public_Function.h"

#include "Run_Bin_Stacker.h"
#include "Run_COK_Robot.h"
#include "Run_Reject_Stacker.h"
#include "Run_Reject_Tray_Transfer.h"
#include "Run_Robot.h"
#include "Run_Test_Site.h"
#include "Run_Motors.h"
#include "SrcPart/APartNetworkManager.h"
//******************************************************************************

#include "MyJamData.h"  // 알람 출력 정보 로딩 및 알람 메시지 저장 클래스 
#include "SrcPart/APartHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//알람 출력 클래스 변수 선언                                                    
//******************************************************************************
CMyJamData   mcls_alarm_disp;  // 발생 알람 출력 정보 로딩 클래스 변수
//******************************************************************************

//******************************************************************************
//쓰레드 및 핸들 저장 변수 선언                                                 
//******************************************************************************
CWinThread  *m_thread[THREAD_COUNT];
HANDLE		hThrHandle[THREAD_COUNT];
//******************************************************************************

//******************************************************************************

CMutex mutex;
int All_Stop;		// 쓰레드 상태 플래그

//******************************************************************************
// 스위치 검사 동작 제어 쓰레드 함수                                            
//******************************************************************************
UINT OnThread_Handler_Check(LPVOID lParam)  // m_Thread[0]
{
	CSingleLock sing(&mutex);

	while(!All_Stop)
	{
		sing.Lock();

		Run_Handler_Check.ButtonCheck_Start();
 		Run_Handler_Check.ButtonCheck_Stop();
 		Run_Handler_Check.ButtonCheck_Alarm_Clear();
 		Run_Handler_Check.ButtonCheck_BuzzerOff();
		Run_Handler_Check.ButtonCheck_ManualAutoKey();
		Run_Handler_Check.ButtonCheck_Z_Axis_Up();
		Run_Handler_Check.ButtonCheck_Z_Axis_Down();
		Run_Handler_Check.SafetyChk();
		g_handler.Thread_Run();
		sing.Unlock();
		Sleep(1);
	}

	return TRUE;
}
//******************************************************************************

//******************************************************************************
// 타워 램프 출력 동작 제어 쓰레드 함수                                         
//******************************************************************************
UINT OnThread_Tower_Lamp_Check(LPVOID lParam)  // m_Thread[1]
{
	CSingleLock sing(&mutex);

	while(!All_Stop)
	{
		sing.Lock();

		Run_TowerLamp.Thread_Run();
		Run_TowerLamp.Run_FormChange();

		sing.Unlock();
		Sleep(1);
	}

	return TRUE;
}
//******************************************************************************

//******************************************************************************
// 장비 동작 중 발생한 알람 출력하기 위한 쓰레드 함수                           
//******************************************************************************
UINT OnThread_Alarm_Display(LPVOID lParam)  // m_Thread[2]
{
	CSingleLock sing(&mutex);

	while(!All_Stop)
	{
		sing.Lock();

 		mcls_alarm_disp.On_Alarm_Display(); // 알람 발생한 경우 알람 화면 출력 함수
		 
		sing.Unlock();
		Sleep(1);
	}
	return TRUE;
}
//******************************************************************************

//******************************************************************************
// IO 동작을 위한 쓰레드 함수
//******************************************************************************
UINT OnThread_IO(LPVOID lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		RUN_IO_ReadWrite.Run_Total_IORead();
	
		sing.Unlock();
		Sleep(1);
	}

	return TRUE;
}

UINT OnThread_Network(LPVOID lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		g_Net.Run_Move();
	
		sing.Unlock();
		Sleep(1);
	}

	return TRUE;
}
//******************************************************************************

UINT OnThread_Bin_Stacker_Reject(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Bin_Stacker[2].Thread_Run();
		//Run_Bin_Stacker[3].Thread_Run();//ybs
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Bin_Stacker_1(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Bin_Stacker[0].Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;	
}

UINT OnThread_Motors(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		g_Run_Motors.Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;	
}

UINT OnThread_Bin_Stacker_2(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Bin_Stacker[1].Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

//UINT OnThread_Bin_Stacker_3(LPVOID  lParam)
//{
//	CSingleLock sing(&mutex);
//	
//	while (!All_Stop)
//	{
//		sing.Lock();
//		
//		Run_Bin_Stacker[2].Thread_Run();
//		
//		sing.Unlock();
//		Sleep(1);
//	}
//	
//	return TRUE;
//}

UINT OnThread_COK_Robot(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_COK_Robot.Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Reject_Stacker(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Reject_Stacker.Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Reject_Tray_Transfer(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Reject_Tray_Transfer.Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Robot(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Robot.Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Test_Site_1(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	while (!All_Stop)
	{
		sing.Lock();
		

		Run_Test_Site[0].Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}

UINT OnThread_Test_Site_2(LPVOID  lParam)
{
	CSingleLock sing(&mutex);
	
	while (!All_Stop)
	{
		sing.Lock();
		
		Run_Test_Site[1].Thread_Run();
		
		sing.Unlock();
		Sleep(1);
	}
	
	return TRUE;
}