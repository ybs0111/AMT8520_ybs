#if !defined(AFX_DIALOG_MANUAL_PART_4_H__5A2EA455_F957_47CC_A90F_76879887A993__INCLUDED_)
#define AFX_DIALOG_MANUAL_PART_4_H__5A2EA455_F957_47CC_A90F_76879887A993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Manual_Part_4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_4 dialog
#include "BtnST.h"          // 칼라 버튼 생성 클래스 추가  
#include "NewLabel.h"		// 칼라 텍스트 박스 생성 클래스 추가 
#include "EditEx.h"			// 칼라 에디트 박스 생성 클래스 추가 
#include "EXDigitST.h"		// 디지털 카운터 클래스
#include "LedButton.h"      // LED 버튼 클래스
#include "XPGroupBox.h"		// 칼라 그룹 박스 생성 클래스 추가 
#include "SxLogFont.h"
#include "MacButtons.h"
#include "GradientStatic.h" // 그라데이션 칼라 텍스트 박스 생성 클래스 추가 
#include "XpButton.h"		// XP 칼라 버튼 클래스
#include "Dialog_Message.h"
#include "FastechPublic_IO.h"		// Fastech 관련 동작 Class
#include "ComizoaPublic.h"
#include "GridControlAlg.h"
#include "Digit.h"

class CDialog_Manual_Part_4 : public CDialog
{
// Construction
public:
	CDialog_Manual_Part_4(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Manual_Part_4)
	enum { IDD = IDD_DIALOG_MANUAL_PART_4 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CFont*	mp_manual_font;			// 폰트 설정 변수 선언 
	CPoint	mcp_coord;	
	int		mn_part;
	
public:
	BOOL Create();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Manual_Part_4)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Manual_Part_4)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtn4Exit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_MANUAL_PART_4_H__5A2EA455_F957_47CC_A90F_76879887A993__INCLUDED_)
