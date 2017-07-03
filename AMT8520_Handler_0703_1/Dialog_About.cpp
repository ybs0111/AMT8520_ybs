// Dialog_About.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_About.h"
#include "AVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_About dialog


CDialog_About::CDialog_About(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_About::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_About)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_caption_main = "SSD InLine About Dialog Box";
//	m_is_shape_3d = YES;
}


void CDialog_About::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_About)
	DDX_Control(pDX, IDC_PICTURE_ABOUT, m_picture_about);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_About, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_About)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_About message ICTs


BOOL CDialog_About::OnInitDialog() 
{
	StandardDialog::OnInitDialog();

	// Start Switch의 동작을 막는다.
	if (st_handler.mn_switch_lock == FALSE)
	{
		st_handler.mn_switch_lock = TRUE;
		mn_dialog = 1;
	}
	else	mn_dialog = 0;
	
	OnAbout_Button_Set();			// 대화 상자 칼라 버튼 생성 함수

	OnAbout_StarWars_Control_Set();	// 스타워즈 형태의 컨트롤 생성 함수
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_About::OnDestroy() 
{
	delete mp_about_ok;				// OK 버튼에 대한 정보 삭제 

	if (mn_dialog == 1)
	{
		st_handler.mn_switch_lock = FALSE;
	}

	StandardDialog::OnDestroy();
}

void CDialog_About::OnAbout_Button_Set()
{
	mp_about_ok = CreateOkButton(IDOK, this);
}

void CDialog_About::OnAbout_StarWars_Control_Set()
{
	m_picture_about.SetStarSpeed(30);	// 백 그라운드 (별 모양)이 움직이는 속도 설정 
	m_picture_about.SetScrollSpeed(2);	// 버전 정보가 움직이는 속도 설정 

	m_picture_about.AddTextLine("SSD INTERFACE TESTER AMT8520");
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("Motion Control System");
	m_picture_about.AddTextLine("&");
	m_picture_about.AddTextLine("Human Machine Interface");
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("Version : " + g_ver.GetVersion() );
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("Development Year : 2012-04-01");
	m_picture_about.AddTextLine("Update Info      : 2011-12-19");
	m_picture_about.AddTextLine("Development a company : AMT");
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("Copyright(C) 2002.05.01");
	m_picture_about.AddTextLine("Advanced Mechatronics Tech'. Co.");
	m_picture_about.AddTextLine("");
	m_picture_about.AddTextLine("http://www.imamt.com");
}
