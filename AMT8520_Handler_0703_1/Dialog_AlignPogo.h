#if !defined(AFX_DIALOG_ALIGNPOGO_H__FD5FC098_970D_47D7_A6DE_89294B1BD597__INCLUDED_)
#define AFX_DIALOG_ALIGNPOGO_H__FD5FC098_970D_47D7_A6DE_89294B1BD597__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_AlignPogo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_AlignPogo dialog

class CDialog_AlignPogo : public StandardDialog
{
// Construction
public:
	CDialog_AlignPogo(CWnd* pParent = NULL);   // standard constructor

	int mn_PogoMode;
	int mn_AlignMode;

// Dialog Data
	//{{AFX_DATA(CDialog_AlignPogo)
	enum { IDD = IDD_DIALOG_ALIGN_POGO };
	CButton	m_radio_pogouse;
	CButton	m_radio_alignnotuse;
	CButton	m_radio_alignuse;
	CXPGroupBox	m_group_pogo_type;
	CXPGroupBox	m_group_alignbuffer;
	CButton	m_radio_pogonotuse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_AlignPogo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnData_Change();
	void OnPogoMode_Change();
	void OnAlignMode_Change();

protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_AlignPogo)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioPogoMode();
	afx_msg void OnRadioPogoNotuse();
	afx_msg void OnRadioJigMode();
	afx_msg void OnRadioJigNotuse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_ALIGNPOGO_H__FD5FC098_970D_47D7_A6DE_89294B1BD597__INCLUDED_)
