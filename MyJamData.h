// MyJamData.h: interface for the CMyJamData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYJAMDATA_H__CB595225_06C8_44CA_9BC6_742D8AC656E1__INCLUDED_)
#define AFX_MYJAMDATA_H__CB595225_06C8_44CA_9BC6_742D8AC656E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Variable.h"       // 전역 변수 정의 클래스

class CMyJamData  
{
public:
	CMyJamData();
	virtual ~CMyJamData();
	int mn_noError;

// Attributes
public:
//	CAlarmListFile mcls_alarm_list;  // 알람 리스트 파일 생성 클래스 변수 선언

// Operations
public:
	//2013,0306
	void On_UnderDaily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg);
	void On_Daily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg);
	CString On_Alarm_Display_Info_Load(CString strPath, CString strDataFile, CString strActionFile, CString strCode);  // 발생 알람 정보 설정 함수

	CString On_Alarm_Bmp_Data_Load(CString strPath, CString strDataFile, CString strCode);       // 발생 알람 부위 표시용 그림 파일명 설정 함수
	CString On_Alarm_Style_Data_Load(CString strPath, CString strDataFile, CString strCode);     // 알람 발생 부위 표시 형태 및 출력 위치 설정 함수
	CString On_Alarm_Repair_Data_Load(CString strPath, CString strActionFile, CString strCode);  // 발생한 알람에 대한 조치 사항 설정 함수
	
	void On_Alarm_Display();					// 발생한 알람 화면 출력 함수
	void On_Occured_Alarm_Saving_to_File();		// 발생한 알람 로그 파일에 저장하는 함수
	void On_Alarm_Info_Load();					// 파일에 저장된 모든 알람 정보 전역 변수에 설정하는 함수
	void On_Alarm_Info_Set_to_Variable(CString strErrCode);  // 발생한 알람 메시지 전역 변수에 설정하는 함수
};
extern CMyJamData			MyJamData;

CString GetJamCode_GripperDn_Time( int iOn );
CString GetJamCode_GripperOn_Time( int iOn );
CString GetJamCode_VacuumDn_Time( int iIdx, int iOn );
CString GetJamCode_TS_Plate_Time( int nSite, int iOn );
CString GetJamCode_TS_Insert_Time( int nSite, int iIdx, int iOn );
CString GetJamCode_TS_Pogo_Time( int nSite, int iIdx, int iOn );//2012,1225
CString GetJamCode_TS_Lock_Time( int nSite, int iOn );
CString GetJamCode_GripperDn_Chk( int iOn );
CString GetJamCode_VacuumDn_Chk( int iIdx, int iOn );
CString GetJamCode_TS_Plate_Chk( int nSite, int iOn );
CString GetJamCode_TS_Insert_Chk( int nSite, int iIdx, int iOn );
CString GetJamCode_TS_Lock_Chk( int nSite, int iOn );
CString GetJamCode_HF_Clamp_Chk( int nSite, int iOn );

CString GetJamCode_TrayGuide_Chk( int nTray, int iOn );
CString GetJamCode_TrayPusher_Chk( int nTray, int iOn );
CString GetJamCode_TrayExist_Chk( int nTray, int iOn );

CString GetJamCode_ModuleExist_Chk( int nPicker, int iOn );

CString GetJamCode_TrayGuide_Time( int nTray, int iOn );
CString GetJamCode_TrayPusher_Time( int nTray, int iOn );

CString GetJamCode_AlignChk( int nIdx, int iOn );
CString GetJamCode_StopperChk( int nIdx, int iOn );
CString GetJamCode_ClampChk( int nIdx, int iOn );

CString GetJamCode_Align_Time( int nIdx, int iOn );
CString GetJamCode_Stopper_Time( int nIdx, int iOn );
CString GetJamCode_Clamp_Time( int nIdx, int iOn );

CString GetJamCode_Elev_Z( int nIdx, int nPos );
CString GetJamCode_Elev_R( int nIdx, int nPos );

CString GetJamCode_ConvTray_Chk( int nIdx, int iOn );

CString GetJamCode_Transfer_Y( int npos );
CString GetJamCode_Transfer_C( int npos );
CString GetJamCode_Transfer_Dn( int iOn );

CString GetJamCode_Variable( int iOn );

CString GetJamCode_TransferTray( int iOn );

CString GetJamCode_CokLock( int iOn );

CString GetJamCode_GripperOn_Chk( int iOn );

CString GetJamCode_Rej_Guide_Time( int iOn );
CString GetJamCode_Rej_Seperator_Time( int iOn );
CString GetJamCode_Rail_Time( int iOn );
CString GetJamcode_Rej_Transfer_Down_Time( int iOn );
CString GetJamCode_Pogo_Insert_Before_Chk( int nSite, int iIdx, int iOn );

#endif // !defined(AFX_MYJAMDATA_H__CB595225_06C8_44CA_9BC6_742D8AC656E1__INCLUDED_)
