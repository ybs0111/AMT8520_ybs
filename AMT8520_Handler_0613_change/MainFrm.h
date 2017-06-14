// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__2C6D2CE2_883C_498B_9324_12F2734B9EC3__INCLUDED_)
#define AFX_MAINFRM_H__2C6D2CE2_883C_498B_9324_12F2734B9EC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TrueColorToolBar.h"
#include "Variable.h"		// ���� ���� ���� Ŭ���� �߰� 
#include "SerialComm.h"     // �ø��� ��� Ŭ����

#include "NetWork_Comm.h"   // NetWork
class  CNetWork_Comm;

#include "Dialog_Alarm.h"	// �˶� ��� ȭ�� Ŭ���� �߰� 
class  CDialog_Alarm;		// ��޸��� �˶� ȭ�� ��ȭ ���ڿ� ���� �ν��Ͻ� ����

#include "Dialog_Event_Msg.h"

// ******************************************************************************
// Initial Dialog Bar ���� Ŭ���� �߰� �κ�
// ******************************************************************************
#include "Dialog_Title.h"	// Ÿ��Ʋ ���� ��� ��ȭ ���� Ŭ���� �߰� 
#include "Dialog_List.h"	// ����Ʈ ���� ��� ��ȭ ���� Ŭ���� �߰� 
//  ******************************************************************************

#include "MyBasicData.h"

#define TMR_FILE_CREATE			861
#define TMR_MAIN_REFRESH		862
#define TMR_MAIN_ELEC_CHK		863
#define TMR_BOARD_LIVE_CHK		865
#define TMR_XGEM_CHK			867

#define CALPELLA_A_TIMER	100
#define CALPELLA_B_TIMER	200
#define CALPELLA_C_TIMER	300
#define CALPELLA_D_TIMER	400
#define CALPELLA_E_TIMER	500
#define CALPELLA_F_TIMER	600
#define CALPELLA_G_TIMER	700
#define CALPELLA_H_TIMER	800
#define MAIN_TIMER			900
#define HEATER_READ			1000
#define HEATER_SET			1010


#define TOOLBAR_DRAW_BUTTON_WIDTH 32

class CDialog_XgemInterface;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	// **************************************************************************
	// Initial Dialog Bar ���� Ŭ���� ������ ���� ����
	// **************************************************************************
	CDialog_Title	m_wndTitleBar;  // Ÿ��Ʋ �� ��ȭ ���ڿ� ���� Ŭ���� ���� ���� 
	CDialog_List    m_wndListBar;   // ����Ʈ ���� ��ȭ ���ڿ� ���� Ŭ���� ���� ���� 
	// **************************************************************************

	CMyBasicData    mcls_m_basic;		// �⺻ ���� ���� �ε� �� ���� Ŭ����
	CMyJamData      mcls_frm_alarm;		// �˶� ��� Ŭ����
	CDialog_Alarm*  mp_alarm_dlg;		// �˶� ȭ�鿡 ���� ������ ���� ����

	/* ************************************************************************** */
	/* �ø��� ��� ���� ���� ����                                                 */
	/* ************************************************************************** */
	SerialComm m_ports[MAX_PORT];       // �ø��� ��Ʈ Ŭ���� ����
	CString mstr_received[MAX_PORT];    // ���� ������ �ӽ� ���� ����
	/* ************************************************************************** */

	CDialog_Event_Msg*  mp_msg_dlg;

	int nElectrostaticCheckStep;
	int nXgemStep;
	CString strElectCommand;
	int nElectChannel;
	long lElectSerialTime[3];
	long lXgemTime[3];

	int mn_calpella_pos;
	int mn_part;
	int mn_board;
	int mn_power_on_pos;
	int mn_power_on_step;
	int mn_power_off_pos;
	int mn_power_off_step;
	int mn_a_station_step;
	int mn_calpella_display_error;
	long mn_Calpella_Wait[3];
	int mn_calpella_display_old_pos;
	int mn_power_off_chk_step;
	int mn_power_off_chk_pos;
	int mn_video_chk_step;
	int mn_video_chk_pos;
	CString mstr_received_calpella[MAX_PORT];    // ���� ������ �ӽ� ���� ����
	//kwlee 2015.1005
	CString mstr_Data[21][21]; 
	int mn_Cnt;
	CString mstrHeaterWCommand;
	long lTempSerialTime[3];

	int mn_new_lot_in_step;

	//2015.0914
	int mn_Db_Cnt; 
	int mn_Count;
	
	bool mb_Ten ;
	bool mb_intervalCheck;

	long ml_wait_time[3];
	long ml_retry_wait[3];

	int mn_lot_name_step;

	int mnQueryStep;

	SYSTEMTIME m_timeLastDay;

	CDialog_XgemInterface* pSgem;
	CFtpConnection		*m_pConnection;

	CFtpClient	m_pFtp;
	CString		m_strPathRemote;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnAlarm_Destroy();
	void OnMain_Rcv_GMS(WPARAM port, LPARAM ch);
	void OnMain_Snd_Serial(WPARAM wParam, LPARAM lParam);
	void OnMainFrame_SelectMessageDisplay();
	void OnMain_Motor_Destroy();
	void OnMain_Port_Close();
	void OnExitData_Saving();
	void OnMain_Thread_Destroy();
	void OnMain_Port_Create();
	void OnMain_Thread_Creating();
	///2015.0914
	void DB_Data_Read(int nCount);
	void DB_MonthCal_Data_Read(int nCount);
	void DB_MonthCal_Data_Add();
	void DB_Now_Data();
	void DB_Data_Init();
	void IntervalCheck();
	void CSV_File_Save();


	////////////
	int OnElectrostatic_Check();
	void OnMain_Var_Default_Set();
	void OnMain_Path_Set();
	void OnMain_Recipe_Data();
	void OnSwitchToForm(int nForm);
	int OnMenu_Change_Checking();
	int OnXgmInterface();

	void FtpConnect();
	void FtpFileDown();
	void RecipeRuleFileDown();
	void OnUpdateRemote(CString strPath);

	virtual ~CMainFrame();



#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CTrueColorToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMain();
	afx_msg void OnBasic();
	afx_msg void OnSetting();
	afx_msg void OnWait();
	afx_msg void OnMotor();
	afx_msg void OnIo();
	afx_msg void OnList();
	afx_msg void OnLevel();
	afx_msg void OnSave();
	afx_msg void OnAlarm();
	afx_msg void OnAbout();
	afx_msg void OnReset();
	afx_msg void OnLock();
	afx_msg void OnExit();
	afx_msg void OnMotTeach();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMotSpeed();
	afx_msg void OnSetInterface();
	afx_msg void OnSetMaintenance();
	afx_msg void OnAdmin();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnListOper();
	afx_msg void OnListDataview();
	afx_msg void OnFileEditor();
	afx_msg void OnMotWithIO();
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnViewChangeMode(WPARAM wParam,LPARAM lParam) ;		// Post Message�� ���� ȭ�� ��ȯ ����� ����� ���� �޽��� ���� 
	afx_msg LRESULT OnMainframe_Work(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainMessageEvent(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnCommunication(UINT, LONG);							// RS-232C �ø��� ��Ʈ ���� �޽���
	afx_msg LONG OnCommunicationEvent(UINT, LONG);						// RS-232C �ø��� ��Ʈ �̺�Ʈ ���� �޽���
	afx_msg LRESULT OnDataSend(WPARAM wParam, LPARAM lParam);			// RS-232C �ø��� ��Ʈ ���� ������ �۽� �޽���
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__2C6D2CE2_883C_498B_9324_12F2734B9EC3__INCLUDED_)
