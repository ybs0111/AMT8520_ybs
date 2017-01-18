// Dialog_Socket_Comment.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Socket_Comment.h"
#include "AMTRegistry.h"
#include "AMTLotManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Socket_Comment dialog


CDialog_Socket_Comment::CDialog_Socket_Comment(int nId, CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Socket_Comment::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Socket_Comment)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	CString strVal;
	m_niid = nId;
	strVal.Format( "%s_%d_%2d", REG_VAL_SOCKET_TS, st_basic.mn_hotfix_pos, m_niid );
	m_strPrev = AMTRegistry::RegReadString(REG_KEY_SOCKET_CAUSE, strVal, "").c_str();

}


void CDialog_Socket_Comment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Socket_Comment)
	DDX_Control(pDX, IDC_EDIT_PREV_CAUSE, m_editPrevStr);
	DDX_Control(pDX, IDC_EDIT_NEXT_CAUSE, m_editNextStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Socket_Comment, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_Socket_Comment)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, SaveOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Socket_Comment message handlers

void CDialog_Socket_Comment::OnDestroy() 
{
	delete mp_msg_ok;  // OK 버튼에 설정된 정보 삭제 

	if (mn_dialog == 1)
	{
		st_handler.mn_switch_lock = FALSE;
	}

	StandardDialog::OnDestroy();
	
	
}

BOOL CDialog_Socket_Comment::OnInitDialog() 
{
	StandardDialog::OnInitDialog();

	// Start Switch의 동작을 막는다.
	if (st_handler.mn_switch_lock == FALSE)
	{
		st_handler.mn_switch_lock = TRUE;
		mn_dialog = 1;
	}
	else	mn_dialog = 0;


	m_editPrevStr.SetWindowText(m_strPrev);
	
	OnMsg_Button_Set(); // 대화 상자 칼라 버튼 생성 함수
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Socket_Comment::OnMsg_Button_Set()
{
	mp_msg_ok = CreateOkButton(IDOK, this);
}

void CDialog_Socket_Comment::SaveOK() 
{
	((CEdit*)GetDlgItem(IDC_EDIT_NEXT_CAUSE))->GetWindowText(m_strNext);

	if(m_strNext != "")
	{
		m_strPrev.Format("%s", m_strNext);
	}

	CString strVal;
	strVal.Format( "%s_%d_%2d", REG_VAL_SOCKET_TS, st_basic.mn_hotfix_pos, m_niid );
	m_strPrev = AMTRegistry::RegWriteString(REG_KEY_SOCKET_CAUSE, strVal, m_strPrev);

	StandardDialog::OnOK();
}
