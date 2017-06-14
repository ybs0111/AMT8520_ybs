// XgemInterfaceDlg.h : header file
//
//{{AFX_INCLUDES()
#include "exgem.h"
//}}AFX_INCLUDES

#if !defined(AFX_XGEMINTERFACEDLG_H__588A4790_64C6_4AFE_A767_D61CBA0EE647__INCLUDED_)
#define AFX_XGEMINTERFACEDLG_H__588A4790_64C6_4AFE_A767_D61CBA0EE647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXgemInterfaceDlg dialog
#include "XGem.h"
#include "GradientStatic.h"
#include "sxlogfont.h"
#include "TrayIcon.h" 
#include "ColorListBox.h"
#include "XPGroupBox.h"

class  CClientSocket;
class  CServerSocket;

class CXgemInterfaceDlg : public CDialog
{
public:
	CServerSocket	*m_pServer[10];
	CClientSocket	*m_pClient[10];

	CString		m_strOldMsg;
	CString		m_strRecvData;
	CString		m_strPPID;
	CString		m_strPPBody;

	int			m_nRunMode;
	int			m_nInterfaceMode;
	int			m_nXgemOpenFlag;
	int			m_nXgemStartFlag;

	long		m_lNormalTime[3];
	CTrayIcon m_TrayIcon; 

	CFont* m_pFont;
// Construction
public:
	BOOL CreateFolder(LPCTSTR szPath);
	void OnLogFileSave(CString strLog);
	void OnXgemVariable();
	void OnListMessageClear();
	void OnListNormalMsgDisplay();
	CString OnEqToXgemFunction(CString strFind, CString strRecv);
	void OnEqToXgemCommand(CString strRecv);
	void OnMachineStatus(int nStatus);
	void OnEqConnectStatus(int nStatus);
	void OnHostConnectStatus(int nStatus);
	void OnLabelSet();
	CXgemInterfaceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXgemInterfaceDlg)
	enum { IDD = IDD_XGEMINTERFACE_DIALOG };
	CColorListBox	m_list_data;
	CGradientStatic	m_msgMachineStatus;
	CGradientStatic	m_msgHostConnt;
	CGradientStatic	m_msgEqConnt;
	CXGem	m_hXgem;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXgemInterfaceDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXgemInterfaceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OneGEMCommStateChangedExgemctrlGem(long nState);
	afx_msg void OneGEMControlStateChangedExgemctrlGem(long nState);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OneXGEMStateEventExgemctrlGem(long nState);
	afx_msg void OnExit();
	afx_msg void OnView();
	afx_msg void OneGEMReqRemoteCommandExgemctrlGem(long nMsgId, LPCTSTR sRcmd, long nCount, BSTR FAR* psNames, BSTR FAR* psVals);
	afx_msg void OneGEMTerminalMessageExgemctrlGem(long nTid, LPCTSTR sMsg);
	afx_msg void OneGEMTerminalMultiMessageExgemctrlGem(long nTid, long nCount, BSTR FAR* psMsg);
	afx_msg void OneGEMErrorEventExgemctrlGem(long nErrorCode);
	afx_msg void OneGEMRspGetDateTimeExgemctrlGem(LPCTSTR sSystemTime);
	afx_msg void OneGEMReqDateTimeExgemctrlGem(long nMsgId, LPCTSTR sSystemTime);
	afx_msg void OneGEMReqGetDateTimeExgemctrlGem(long nMsgId);
	afx_msg void OnBtnHide();
	afx_msg void OneGEMReportedEventExgemctrlGem(long nEventID);
	afx_msg void OneGEMReqOfflineExgemctrlGem(long nMsgId, long nFromState, long nToState);
	afx_msg void OneGEMReqOnlineExgemctrlGem(long nMsgId, long nFromState, long nToState);
	afx_msg void OneGEMECVChangedExgemctrlGem(long nCount, long FAR* pnEcIds, BSTR FAR* psVals);
	afx_msg void OneGEMReqChangeECVExgemctrlGem(long nMsgId, long nCount, long FAR* pnEcIds, BSTR FAR* psVals);
	afx_msg void OneSECSMessageReceivedExgemctrlGem(long nObjectID, long nStream, long nFunction, long nSysbyte);
	afx_msg void OneGEMReqPPLoadInquireExgemctrlGem(long nMsgId, LPCTSTR sPpid, long nLength);
	afx_msg void OneGEMRspPPLoadInquireExgemctrlGem(LPCTSTR sPpid, long nResult);
	afx_msg void OneGEMReqPPSendExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sBody);
	afx_msg void OneGEMRspPPSendExgemctrlGem(LPCTSTR sPpid, long nResult);
	afx_msg void OneGEMReqPPExgemctrlGem(long nMsgId, LPCTSTR sPpid);
	afx_msg void OneGEMRspPPExgemctrlGem(LPCTSTR sPpid, LPCTSTR sBody);
	afx_msg void OneGEMReqPPDeleteExgemctrlGem(long nMsgId, long nCount, BSTR FAR* psPpid);
	afx_msg void OneGEMReqPPListExgemctrlGem(long nMsgId);
	afx_msg void OneGEMReqPPFmtSendExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames);
	afx_msg void OneGEMRspPPFmtSendExgemctrlGem(LPCTSTR sPpid, long nResult);
	afx_msg void OneGEMReqPPFmtExgemctrlGem(long nMsgId, LPCTSTR sPpid);
	afx_msg void OneGEMRspPPFmtExgemctrlGem(LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames);
	afx_msg void OneGEMRspPPFmtVerificationExgemctrlGem(LPCTSTR sPpid, long nResult);
	afx_msg void OneGEMRspAllECInfoExgemctrlGem(long lCount, long FAR* plVid, BSTR FAR* psName, BSTR FAR* psValue, BSTR FAR* psDefault, BSTR FAR* psMin, BSTR FAR* psMax, BSTR FAR* psUnit);
	afx_msg void OneGEMReqPPSendExExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sRecipePath);
	afx_msg void OneGEMRspPPExExgemctrlGem(LPCTSTR sPpid, LPCTSTR sRecipePath);
	afx_msg void OneGEMReqPPExExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sRecipePath);
	afx_msg void OneGEMRspPPSendExExgemctrlGem(LPCTSTR sPpid, LPCTSTR sRecipePath, long nResult);
	afx_msg void OneGEMRspLoopbackExgemctrlGem(long nCount, long FAR* pnAbs);
	afx_msg void OneGEMNotifyPerformanceWarningExgemctrlGem(long nLevel);
	afx_msg void OneGEMSecondaryMsgReceivedExgemctrlGem(long nS, long nF, long nSysbyte, LPCTSTR sParam1, LPCTSTR sParam2, LPCTSTR sParam3);
	afx_msg void OneGEMReqPPFmtSend2ExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCoode, long FAR* pnParamCount, BSTR FAR* psParamNames, BSTR FAR* psParamValues);
	afx_msg void OneGEMRspPPFmt2ExgemctrlGem(LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames, BSTR FAR* psParamValues);
	afx_msg void OnSecsEventExcomctrl31(short nEventId, long lParam);
	afx_msg void OnButton1();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg LONG OnTrayNotification(UINT wParam, LONG lParam);
	afx_msg LRESULT OnCommand_Client_1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommand_Client_2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommand_Server_1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListDataUpdate(WPARAM wParam,LPARAM lParam) ;  // 수신 메시지 리스트 박스 컨트롤에 추가 메시지 선언
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XGEMINTERFACEDLG_H__588A4790_64C6_4AFE_A767_D61CBA0EE647__INCLUDED_)
