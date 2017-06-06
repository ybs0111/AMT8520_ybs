#if !defined(AFX_DIALOG_MANUAL_PART_3_H__8DB0AD62_D0F5_40E1_A3E3_2C5D5C115942__INCLUDED_)
#define AFX_DIALOG_MANUAL_PART_3_H__8DB0AD62_D0F5_40E1_A3E3_2C5D5C115942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Manual_Part_3.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_3 dialog
#include "BtnST.h"          // Į�� ��ư ���� Ŭ���� �߰�  
#include "NewLabel.h"		// Į�� �ؽ�Ʈ �ڽ� ���� Ŭ���� �߰� 
#include "EditEx.h"			// Į�� ����Ʈ �ڽ� ���� Ŭ���� �߰� 
#include "EXDigitST.h"		// ������ ī���� Ŭ����
#include "LedButton.h"      // LED ��ư Ŭ����
#include "XPGroupBox.h"		// Į�� �׷� �ڽ� ���� Ŭ���� �߰� 
#include "SxLogFont.h"
#include "MacButtons.h"
#include "GradientStatic.h" // �׶��̼� Į�� �ؽ�Ʈ �ڽ� ���� Ŭ���� �߰� 
#include "XpButton.h"		// XP Į�� ��ư Ŭ����
#include "Dialog_Message.h"
#include "FastechPublic_IO.h"		// Fastech ���� ���� Class
#include "ComizoaPublic.h"
#include "GridControlAlg.h"
#include "Digit.h"

class CDialog_Manual_Part_3 : public CDialog
{
// Construction
public:
	CDialog_Manual_Part_3(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Manual_Part_3)
	enum { IDD = IDD_DIALOG_MANUAL_PART_3 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CFont*	mp_manual_font;			// ��Ʈ ���� ���� ���� 
	CPoint	mcp_coord;	
	int		mn_part;
	
public:
	BOOL Create();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Manual_Part_3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Manual_Part_3)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtn3Exit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_MANUAL_PART_3_H__8DB0AD62_D0F5_40E1_A3E3_2C5D5C115942__INCLUDED_)
