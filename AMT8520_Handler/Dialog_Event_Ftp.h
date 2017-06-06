#if !defined(AFX_DIALOG_EVENT_FTP_H__9131CE82_7D59_4B3C_A195_30BF678FAFF9__INCLUDED_)
#define AFX_DIALOG_EVENT_FTP_H__9131CE82_7D59_4B3C_A195_30BF678FAFF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Event_Ftp.h : header file
//
#include "Variable.h"		// 전역 변수 정의 클래스 추가 
#include "Public_Function.h"
#include "MacButtons.h"

#include "NewLabel.h"		// 칼라 텍스트 박스 생성 클래스 추가 
/////////////////////////////////////////////////////////////////////////////
// CDialog_Event_Ftp dialog

class CDialog_Event_Ftp : public StandardDialog
{
// Construction
public:
	CDialog_Event_Ftp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Event_Ftp)
	enum { IDD = IDD_DIALOG_EVENT_FTP };
	CMacButton	m_btn_ok;
	CMacButton	m_btn_cancel;
	CNewLabel	m_label_event_ftp;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Attributes
private:
	CButton *mp_btn_ok;
	// 	CFont* mp_event_font;						// 폰트 설정 변수 선언 

public:
	void OnEventFtp_Button_Set();
	void OnEventFtp_Text_Set();
	void OnEventFtp_Label_Set();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Event_Ftp)
	public:
	virtual BOOL Create();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Event_Ftp)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_EVENT_FTP_H__9131CE82_7D59_4B3C_A195_30BF678FAFF9__INCLUDED_)
