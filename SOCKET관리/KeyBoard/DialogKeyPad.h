#if !defined(AFX_DIALOGKEYPAD_H__EA330A97_3839_4B3F_8E74_3BF91B742803__INCLUDED_)
#define AFX_DIALOGKEYPAD_H__EA330A97_3839_4B3F_8E74_3BF91B742803__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogKeyPad.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogKeyPad dialog
#include "KeyVariable.h"
#include "GradientStatic.h"

struct tagKeyPad
{
	char cNormal;
	char cShifted;
	int cWidth;
};

class CDialogKeyPad : public CDialog
{
// Construction
public:
	CDialogKeyPad(CWnd* pParent = NULL);   // standard constructor

public:
	void		LabelInit();
	void		KeySend(CDC *dc, CRect rectArea, BOOL bFlag, tagKeyPad * keydef);
	void		KeyInit();
	void		KeyAdd(char pNormal,char pShifted,int pWidth);
	void		KeyArea();
	void		KeyDraw();
	void		KeyDraw(CDC * dc, CRect & rc, tagKeyPad * key, BOOL cHilight=FALSE);
	int			KeyState();
	
	CFont		*m_pFont;
	
	CBitmap		m_bmOk;
	CBitmap		m_bmCancel;
	CBitmap		m_bmClear;
	
	int					m_nMode;
	
	CString				m_strMin;
	CString				m_strMax;
	CString				m_strCurrent;
	
	CGradientStatic		m_msgMin;
	CGradientStatic		m_msgMinData;
	CGradientStatic		m_msgMax;
	CGradientStatic		m_msgMaxData;
	CGradientStatic		m_msgCurrent;
	CGradientStatic		m_msgCurrentData;
	CGradientStatic		m_msgNew;
	
	int			m_nKeyState;
	int			m_nKeyWidth;
	int			m_nKeyHeight;
	
	CString		m_strKeyData;
	
	CTypedPtrList<CPtrList, tagKeyPad*> m_pKeys;

// Dialog Data
	//{{AFX_DATA(CDialogKeyPad)
	enum { IDD = IDD_DIALOG_KEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogKeyPad)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogKeyPad)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGKEYPAD_H__EA330A97_3839_4B3F_8E74_3BF91B742803__INCLUDED_)
