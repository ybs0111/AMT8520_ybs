#if !defined(AFX_DIALOG_LIST_H__8F57858E_4E20_4848_B58B_76FBBF404CCC__INCLUDED_)
#define AFX_DIALOG_LIST_H__8F57858E_4E20_4848_B58B_76FBBF404CCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_List.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_List dialog
#include "Variable.h"			// 전역 변수 정의 클래스 추가 
//#include "Public_Function.h"	// 공용 함수 정의 클래스 추가
#include "ColorListBox.h"		// 칼라 리스트 박스 생성 클래스 추가 
#include "InitDialogBar.h"		// Initial Dialog Bar 생성 클래스 추가 
#include "ClockST.h"			// 디지털 시계 생성 클래스 추가 

#include "NewLabel.h"			// 칼라 텍스트 박스 생성 클래스 추가 
#include "GradientStatic.h"		// 그라데이션 칼라 텍스트 박스 생성 클래스 추가 

#include "ServerSocket.h"
#include "ClientSocket.h"
#include <afxmt.h>

#include <string>

#define MAX_CONNECTION		100		// 최대 소켓 연결 정보 정의
#define MAX_SERVER_NUM		1		// 앞 장비 통신 서버

class CDialog_List : public CInitDialogBar
{
// Construction
public:
	void OnList_Message_Clear();
	void OnList_Check_Tab_Stops(int mb_use_tab_stops);
	void OnList_Lable_Set();
	void OnList_Digital_Clock_Set();
	CDialog_List();

// Dialog Data
	//{{AFX_DATA(CDialog_List)
	enum { IDD = IDD_DIALOG_LIST };
	CGradientStatic	m_msg_eqp_id;
	CGradientStatic	m_msg_device_name;
	CNewLabel	m_label_eqp_id;
	CNewLabel	m_label_device_name;
	CColorListBox	m_list_data;
	CClockST	m_list_clock;
	
	////2015.0604
	CNewLabel	m_label_ftp;
	CGradientStatic	m_msg_ftp;
	////

	//}}AFX_DATA

// Attributes
private:
// 	CFont* mp_list_font;			// 폰트 설정 변수 선언 

	char   m_server_rev[5000];
	char   m_client_rev[5000];
	
	CString mstr_device_name;		// 현재 작업 진행할 Device 정보 저장 변수 
	CString mstr_normal_old_msg;
//	std::string mstr_abnormal_old_msg;
	CString mstr_abnormal_old_msg;

	// List에 출력 하다가 죽는 현상을 막기 위해서... 2K11/09/01/ViboX
	long ml_list_msgterm_time_normal[4]; 
	long ml_list_msgterm_time_abnormal[4]; //2011.0504  
	long ml_listmsg_write_time;
	
	// **************************************************************************
	// TCP/IP 통신 클래스 변수 선언                                              
	// **************************************************************************
	CServerSocket* mp_server[MAX_SERVER_NUM];
	int mn_port[MAX_SERVER_NUM];
	CString		   mstr_rpy_msg[10];
	CString		   mstr_body[10];
	CString		   mstr_body_C[10];
	int			   mn_header_C[10];
	CClientSocket	*mp_client[MAX_SERVER_NUM];
	int mn_header[10];
	long mn_wait_time[3];
	// **************************************************************************
	
	
// Operation
public:
	void OnList_Machine_Data_Set();
	void OnList_Receive_Msg_Display(int n_mode);
	void OnList_Send_Msg_Display(int n_mode);
	void OnList_Abnormal_Msg_Display();
	void OnList_Normal_Msg_Display();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_List)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_List)
	virtual BOOL OnInitDialogBar();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkListData();
	//}}AFX_MSG
	afx_msg LRESULT OnListDataUpdate(WPARAM wParam,LPARAM lParam) ;  // 수신 메시지 리스트 박스 컨트롤에 추가 메시지 선언
	afx_msg LRESULT OnSocketAccept(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_LIST_H__8F57858E_4E20_4848_B58B_76FBBF404CCC__INCLUDED_)
