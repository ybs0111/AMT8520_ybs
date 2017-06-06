#if !defined(AFX_DIALOGKEYBOARD_H__6419B72D_A0AE_410B_8253_9F1B4ED5BE25__INCLUDED_)
#define AFX_DIALOGKEYBOARD_H__6419B72D_A0AE_410B_8253_9F1B4ED5BE25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogKeyBoard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogKeyBoard dialog
#include "KeyVariable.h"


struct tagKeyData
{
	char cNormal;
	char cShifted;
	int cWidth;
};

class CDialogKeyBoard : public CDialog
{
// Construction
public:
	CDialogKeyBoard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogKeyBoard)
	enum { IDD = IDD_DIALOG_KEYBOARD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogKeyBoard)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
		void		KeySend(CDC *dc, CRect rectArea, BOOL bFlag, tagKeyData * keydef);
		void		KeyInit();
		void		KeyAdd(char pNormal,char pShifted,int pWidth);
		void		KeyArea();
		void		KeyDraw();
		void		KeyDraw(CDC * dc, CRect & rc, tagKeyData * key, BOOL cHilight=FALSE);
		int			KeyState();
		
		CFont		*m_pFont;
		
		CBitmap		m_bmOk;
		CBitmap		m_bmCancel;
		
		int			m_nKeyState;
		int			m_nKeyWidth;
		int			m_nKeyHeight;
		
		CString		m_strKeyData;
		
		CTypedPtrList<CPtrList, tagKeyData*> m_pKeys;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogKeyBoard)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGKEYBOARD_H__6419B72D_A0AE_410B_8253_9F1B4ED5BE25__INCLUDED_)
