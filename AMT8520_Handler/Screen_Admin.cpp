// Screen_Admin.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Screen_Admin.h"

// ******************************************************************************
// 대화 상자 클래스 추가
// ******************************************************************************
#include "Dialog_Select.h"
#include "Dialog_Message.h"
#include "Dialog_KeyBoard.h"

#include "Dialog_KeyPad.h"
#include "Srcbase\ALocalization.h"
#include "Run_Network.h"

#include "SrcPart/APartNetworkManager.h"
#include "SrcPart/APartHandler.h"

#include "MainFrm.h"
#include "Screen_Pgm_Info.h"
#include "Dialog_Pass_Check.h"
#include "Dialog_Infor.h"
#include "AVersion.h"
#include "ComizoaPublic.h"
#include "SrcPart/APartTestSite.h"

#include "Dialog_MgrSocket.h"
#include "ManagerSocket.h"
// ******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Admin

IMPLEMENT_DYNCREATE(CScreen_Admin, CFormView)

CScreen_Admin::CScreen_Admin()
	: CFormView(CScreen_Admin::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Admin)
	//}}AFX_DATA_INIT
}

CScreen_Admin::~CScreen_Admin()
{
}

void CScreen_Admin::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Admin)
	DDX_Control(pDX, IDC_RADIO_XGEM_OLD, m_radio_xgem_old);
	DDX_Control(pDX, IDC_RADIO_XGEM_NEW, m_radio_xgem_new);
	DDX_Control(pDX, IDC_GROUP_LOAD_SOCKET, m_group_load_socket);
	DDX_Control(pDX, IDC_BTN_LOAD_FILE, m_btn_fileload);
	DDX_Control(pDX, IDC_BTN_SAVE_FILE, m_btn_filesave);
	DDX_Control(pDX, IDC_GROUP_Y_SAFETY, m_group_y_safety);
	DDX_Control(pDX, IDC_GROUP_X_SAFETY, m_group_x_safety);
	DDX_Control(pDX, IDC_GROUP_RBTXY_SAFETY, m_group_rbtxy_safety);
	DDX_Control(pDX, IDC_BTN_CNT_XGem, m_btn_connect_xgem);
	DDX_Control(pDX, IDC_BTN_CIP_SET_XGem, m_btn_cip_set_xgem);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_XGem, m_btn_cport_set_xgem);
	DDX_Control(pDX, IDC_BTN_TST_XGem, m_btn_test_xgem);
	DDX_Control(pDX, IDC_CIP_XGEM, m_cip_xgem);
	DDX_Control(pDX, IDC_MSG_CPORT_XGem, m_msg_cport_xgem);
	DDX_Control(pDX, IDC_MSG_IP_XGem, m_msg_cip_xgem);
	DDX_Control(pDX, IDC_GROUP_CLIENT_XGEM, m_group_client_xgem);
	DDX_Control(pDX, IDC_GROUP_ALIGN_TYPE, m_group_align_type);
	DDX_Control(pDX, IDC_GROUP_POGO_TYPE, m_group_pogo_type);
	DDX_Control(pDX, IDC_GROUP_MIRROR_TYPE, m_group_mirror_type);
	DDX_Control(pDX, IDC_FULL_INLINE_CHK, m_chk_fullinline);
	DDX_Control(pDX, IDC_RADIO_ALIGN_NOTUSE, m_radio_alignnotuse);
	DDX_Control(pDX, IDC_RADIO_ALIGN_USE, m_radio_alignuse);
	DDX_Control(pDX, IDC_RADIO_POGO_USE, m_radio_pogo_use);
	DDX_Control(pDX, IDC_RADIO_POGO_NOTUSE, m_radio_pogo_notuse);
	DDX_Control(pDX, IDC_RADIO_MIRROR_USE, m_radio_mirror_use);
	DDX_Control(pDX, IDC_RADIO_MIRROR_NOTUSE, m_radio_mirror_notuse);
	DDX_Control(pDX, IDC_BUFFER_IN_CHK, m_chk_bufferin_chk);
	DDX_Control(pDX, IDC_CIP_ROUTER, m_cip_router);
	DDX_Control(pDX, IDC_MSG_IP_ROUTER, m_msg_cip_router);
	DDX_Control(pDX, IDC_MSG_CPORT_ROUTER, m_msg_cport_router);
	DDX_Control(pDX, IDC_BTN_TST_ROUTER, m_btn_test_router);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_ROUTER, m_btn_cport_set_router);
	DDX_Control(pDX, IDC_BTN_CNT_ROUTER, m_btn_connect_router);
	DDX_Control(pDX, IDC_BTN_CIP_SET_ROUTER, m_btn_cip_set_router);
	DDX_Control(pDX, IDC_GROUP_CLIENT_ROUTER, m_group_client_router);
	DDX_Control(pDX, IDC_CHK_VIRTUAL_SUPPLY, m_chk_virtual_supply);
	DDX_Control(pDX, IDC_RADIO_POSITION_REAR, m_radio_pos_rear);
	DDX_Control(pDX, IDC_RADIO_POSITION_FRONT, m_radio_pos_front);
	DDX_Control(pDX, IDC_RADIO_POSITION_ALONE, m_radio_pos_alone);
	DDX_Control(pDX, IDC_GROUP_CLIENT_NEXT, m_group_client_next);
	DDX_Control(pDX, IDC_GROUP_CLIENT_FRONT, m_group_client_front);
	DDX_Control(pDX, IDC_GROUP_CLIENT_EC, m_group_client_ec);
	DDX_Control(pDX, IDC_MSG_IP_NEXT, m_msg_cip_next);
	DDX_Control(pDX, IDC_MSG_CPORT_NEXT, m_msg_cport_next);
	DDX_Control(pDX, IDC_GROUP_SERVER, m_group_server);
	DDX_Control(pDX, IDC_CIP_NEXT, m_cip_next);
	DDX_Control(pDX, IDC_BTN_TST_NEXT, m_btn_test_next);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_NEXT, m_btn_cport_set_next);
	DDX_Control(pDX, IDC_BTN_CNT_NEXT, m_btn_connect_next);
	DDX_Control(pDX, IDC_BTN_CIP_SET_NEXT, m_btn_cip_set_next);
	DDX_Control(pDX, IDC_BTN_SPORT_SET_NEXT, m_btn_sport_set_next);
	DDX_Control(pDX, IDC_BTN_SPORT_SET_FRONT, m_btn_sport_set_front);
	DDX_Control(pDX, IDC_BTN_SPORT_SET_BPC, m_btn_sport_set_bpc);
	DDX_Control(pDX, IDC_MSG_SPORT_NEXT, m_msg_sport_next);
	DDX_Control(pDX, IDC_MSG_SPORT_FRONT, m_msg_sport_front);
	DDX_Control(pDX, IDC_MSG_SPORT_BPC, m_msg_sport_bpc);
	DDX_Control(pDX, IDC_BTN_TST_FRONT, m_btn_test_front);
	DDX_Control(pDX, IDC_BTN_TST_EC, m_btn_test_ec);
	DDX_Control(pDX, IDC_BTN_TST_BPC, m_btn_test_bpc);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_FRONT, m_btn_cport_set_front);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_EC, m_btn_cport_set_ec);
	DDX_Control(pDX, IDC_BTN_CPORT_SET_BPC, m_btn_cport_set_bpc);
	DDX_Control(pDX, IDC_BTN_CNT_FRONT, m_btn_connect_front);
	DDX_Control(pDX, IDC_BTN_CNT_EC, m_btn_connect_ec);
	DDX_Control(pDX, IDC_BTN_CNT_BPC, m_btn_connect_bpc);
	DDX_Control(pDX, IDC_BTN_CIP_SET_FRONT, m_btn_cip_set_front);
	DDX_Control(pDX, IDC_BTN_CIP_SET_EC, m_btn_cip_set_ec);
	DDX_Control(pDX, IDC_BTN_CIP_SET_BPC, m_btn_cip_set_bpc);
	DDX_Control(pDX, IDC_CIP_FRONT, m_cip_front);
	DDX_Control(pDX, IDC_CIP_EC, m_cip_ec);
	DDX_Control(pDX, IDC_CIP_BPC, m_cip_bpc);
	DDX_Control(pDX, IDC_MSG_CPORT_FRONT, m_msg_cport_front);
	DDX_Control(pDX, IDC_MSG_CPORT_EC, m_msg_cport_ec);
	DDX_Control(pDX, IDC_MSG_CPORT_BPC, m_msg_cport_bpc);
	DDX_Control(pDX, IDC_MSG_IP_FRONT, m_msg_cip_front);
	DDX_Control(pDX, IDC_MSG_IP_EC, m_msg_cip_ec);
	DDX_Control(pDX, IDC_MSG_IP_BPC, m_msg_cip_bpc);
	DDX_Control(pDX, IDC_CHK_VIRTUAL, m_chk_Virtual);
	DDX_Control(pDX, IDC_BTN_BUFFERDB_DELETE, m_btn_bufferdb_delete);
	DDX_Control(pDX, IDC_GROUP_NEXT_APCT_SET, m_group_next_apct_set);
	DDX_Control(pDX, IDC_RADIO_VERSION_MODE_USE, m_radio_version_mode_use);
	DDX_Control(pDX, IDC_RADIO_VERSION_MODE_NOTUSE, m_radio_version_mode_notuse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreen_Admin, CFormView)
	//{{AFX_MSG_MAP(CScreen_Admin)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHK_VIRTUAL, OnChkVirtual)
	ON_BN_CLICKED(IDC_BTN_FILE_EDITOR, OnBtnFileEditor)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_BPC, OnBtnCipSetBpc)
	ON_BN_CLICKED(IDC_BTN_TST_BPC, OnBtnTstBpc)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_EC, OnBtnCipSetEc)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_FRONT, OnBtnCipSetFront)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_NEXT, OnBtnCipSetNext)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_BPC, OnBtnCportSetBpc)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_EC, OnBtnCportSetEc)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_FRONT, OnBtnCportSetFront)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_NEXT, OnBtnCportSetNext)
	ON_BN_CLICKED(IDC_BTN_SPORT_SET_FRONT, OnBtnSportSetFront)
	ON_BN_CLICKED(IDC_BTN_SPORT_SET_BPC, OnBtnSportSetBpc)
	ON_BN_CLICKED(IDC_BTN_SPORT_SET_NEXT, OnBtnSportSetNext)
	ON_BN_CLICKED(IDC_RADIO_POSITION_FRONT, OnRadioPositionFront)
	ON_BN_CLICKED(IDC_RADIO_POSITION_REAR, OnRadioPositionRear)
	ON_BN_CLICKED(IDC_RADIO_POSITION_ALONE, OnRadioPositionAlone)
	ON_BN_CLICKED(IDC_BTN_TST_NEXT, OnBtnTstNext)
	ON_BN_CLICKED(IDC_CHK_VIRTUAL_SUPPLY, OnChkVirtualSupply)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_ROUTER, OnBtnCipSetRouter)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_ROUTER, OnBtnCportSetRouter)
	ON_BN_CLICKED(IDC_BTN_TST_ROUTER, OnBtnTstRouter)
	ON_BN_CLICKED(IDC_DGT_RETEST_CNT, OnDgtRetestCnt)
	ON_BN_CLICKED(IDC_DGT_AUTO_SOCKET_OFF, OnDgtAutoSocketOff)
	ON_BN_CLICKED(IDC_DGT_BEAT_CNT, OnDgtBeatCnt)
	ON_BN_CLICKED(IDC_BTN_GET_COMMON_TEACHING, OnBtnGetCommonTeaching)
	ON_BN_CLICKED(IDC_BUFFER_IN_CHK, OnBufferInChk)
	ON_BN_CLICKED(IDC_BTN_PGMINFO, OnBtnPgminfo)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_USE, OnRadioMirrorUse)
	ON_BN_CLICKED(IDC_RADIO_MIRROR_NOTUSE, OnRadioMirrorNotuse)
	ON_BN_CLICKED(IDC_RADIO_POGO_USE, OnRadioPogoUse)
	ON_BN_CLICKED(IDC_RADIO_POGO_NOTUSE, OnRadioPogoNotuse)
	ON_BN_CLICKED(IDC_RADIO_ALIGN_USE, OnRadioAlignUse)
	ON_BN_CLICKED(IDC_RADIO_ALIGN_NOTUSE, OnRadioAlignNotuse)
	ON_BN_CLICKED(IDC_BTN_DELFRONT_BUFFER, OnBtnDelfrontBuffer)
	ON_BN_CLICKED(IDC_BTN_DELREAR_BUFFER, OnBtnDelrearBuffer)
	ON_BN_CLICKED(IDC_BTN_TEST_MODE, OnBtnTestMode)
	ON_BN_CLICKED(IDC_BTN_SEND_TEST_MODE, OnBtnSendTestMode)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BTN_VERSION, OnBtnVersion)
	ON_BN_CLICKED(IDC_FULL_INLINE_CHK, OnFullInlineChk)
	ON_BN_CLICKED(IDC_BTN_CIP_SET_XGem, OnBTNCIPSETXGem)
	ON_BN_CLICKED(IDC_BTN_CPORT_SET_XGem, OnBTNCPORTSETXGem)
	ON_BN_CLICKED(IDC_BTN_TST_XGem, OnBTNTSTXGem)
	ON_BN_CLICKED(IDC_BTN_REQ_EMPTY, OnBtnReqEmpty)
	ON_BN_CLICKED(IDC_BTN_SEND_TOOL, OnBtnSendTool)
	ON_BN_CLICKED(IDC_BTN_EMPTY_CANCLE, OnBtnEmptyCancle)
	ON_BN_CLICKED(IDC_BTN_TOOL_CANCLE, OnBtnToolCancle)
	ON_BN_CLICKED(IDC_DGT_RETEST_CNT2, OnDgtRetestCnt2)
	ON_BN_CLICKED(IDC_RADIO_VERSION_MODE_USE, OnRadioVersionModeUse)
	ON_BN_CLICKED(IDC_RADIO_VERSION_MODE_NOTUSE, OnRadioVersionModeNotuse)
	ON_BN_CLICKED(IDC_BTN_SAVE_FILE, OnBtnSaveFile)
	ON_BN_CLICKED(IDC_BTN_LOAD_FILE, OnBtnLoadFile)
	ON_BN_CLICKED(IDC_BTN_BUFFERDB_DELETE, OnBtnBufferdbDelete)
	ON_BN_CLICKED(IDC_BTN_LOAD_SOK, OnBtnLoadSok)
	ON_BN_CLICKED(IDC_BTN_SAVE_SOK, OnBtnSaveSok)
	ON_BN_CLICKED(IDC_RADIO_XGEM_NEW, OnRadioXgemNew)
	ON_BN_CLICKED(IDC_RADIO_XGEM_OLD, OnRadioXgemOld)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Admin diagnostics

#ifdef _DEBUG
void CScreen_Admin::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Admin::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Admin message handlers

void CScreen_Admin::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	// **************************************************************************
	// 컨트롤에 설정할 폰트 생성한다
	// **************************************************************************
// 	mp_admin_font = NULL;
// 	mp_admin_font = new CFont;
// 	mp_admin_font->CreateFont(15,8,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");
	// **************************************************************************

	mn_buffer_ready_step = 0;
	mn_buffer_in_step = 0;

	OnAdmin_GroupBox_Set();
	OnAdmin_EditBox_Set();
	OnAdmin_Label_Set();

	OnAdmin_Data_Set();

	OnAdmin_Dgt_Set();

	OnAdmin_Init_PickerPosition();
	OnAdmin_Display_PickerPosition();

	OnAdmin_Init_TrayPlaceOffset();
	OnAdmin_Display_TrayPlaceOffset();

	OnAdmin_Init_COK_Teaching();
	OnAdmin_Display_COK_Teaching();

	OnAdmin_Init_CokIndex();
	OnAdmin_Display_CokIndex();

	OnAdmin_Init_XSafetyPosition();
	OnAdmin_Init_YSafetyPosition();
	OnAdmin_Display_SafetyPosition();

	//2013,1001
// 	if(CHINA_VER == 1)
// 	{
// 		m_group_mirror_type.ShowWindow(false);
// 		m_radio_mirror_notuse.ShowWindow(false);
// 		m_radio_mirror_use.ShowWindow(false);
// 
// 		m_group_pogo_type.ShowWindow(false);
// 		m_radio_pogo_use.ShowWindow(false);
// 		m_radio_pogo_notuse.ShowWindow(false);
// 
// 		m_group_align_type.ShowWindow(false);
// 		m_radio_alignnotuse.ShowWindow(false);
// 		m_radio_alignuse.ShowWindow(false);
//	}

	m_chk_bufferin_chk.ShowWindow(false);

	//2016.0329
	if( st_basic.n_file_save == CTL_YES )
	{
		m_btn_filesave.ShowWindow( FALSE );
		m_btn_fileload.ShowWindow( FALSE );
	}
}

void CScreen_Admin::OnDestroy() 
{
	// **************************************************************************
	// 생성한 폰트 정보 삭제한다
	// **************************************************************************
// 	delete mp_admin_font;
// 	mp_admin_font = NULL;
	// **************************************************************************

	CFormView::OnDestroy();
}

void CScreen_Admin::OnTimer(UINT nIDEvent) 
{
	CFormView::OnTimer(nIDEvent);
}

void CScreen_Admin::OnAdmin_GroupBox_Set()
{
	CSxLogFont admin_font(15,FW_SEMIBOLD,false,"Arial");
	
	m_group_client_router.SetFont(admin_font);
	m_group_client_router.SetCatptionTextColor(RGB(145,25,0));
	m_group_client_router.SetFontBold(TRUE);

	m_group_next_apct_set.SetFont(admin_font);
	m_group_next_apct_set.SetCatptionTextColor(RGB(145,25,0));
	m_group_next_apct_set.SetFontBold(TRUE);

	m_group_client_ec.SetFont(admin_font);
	m_group_client_ec.SetCatptionTextColor(RGB(145,25,0));
	m_group_client_ec.SetFontBold(TRUE);

	m_group_client_front.SetFont(admin_font);
	m_group_client_front.SetCatptionTextColor(RGB(145,25,0));
	m_group_client_front.SetFontBold(TRUE);

	m_group_client_next.SetFont(admin_font);
	m_group_client_next.SetCatptionTextColor(RGB(145,25,0));
	m_group_client_next.SetFontBold(TRUE);

	m_group_server.SetFont(admin_font);
	m_group_server.SetCatptionTextColor(RGB(145,25,0));
	m_group_server.SetFontBold(TRUE);

	m_group_client_xgem.SetFont(admin_font);
	m_group_client_xgem.SetCatptionTextColor(RGB(145,25,0));
	m_group_client_xgem.SetFontBold(TRUE);

	m_group_rbtxy_safety.SetFont(admin_font);
	m_group_rbtxy_safety.SetCatptionTextColor(RGB(145,25,0));
	m_group_rbtxy_safety.SetFontBold(TRUE);

	m_group_x_safety.SetFont(admin_font);
	m_group_x_safety.SetCatptionTextColor(RGB(145,25,0));
	m_group_x_safety.SetFontBold(TRUE);

	m_group_y_safety.SetFont(admin_font);
	m_group_y_safety.SetCatptionTextColor(RGB(145,25,0));
	m_group_y_safety.SetFontBold(TRUE);

	m_group_load_socket.SetFont(admin_font);
	m_group_load_socket.SetCatptionTextColor(RGB(145,25,0));
	m_group_load_socket.SetFontBold(TRUE);

}

void CScreen_Admin::OnAdmin_EditBox_Set()
{
	m_edit_cport_router.SubclassDlgItem(IDC_EDIT_CPORT_ROUTER, this);
	m_edit_cport_router.bkColor(RGB(50, 100, 150));
	m_edit_cport_router.textColor(RGB(255, 255,255));
	m_edit_cport_router.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_cport_bpc.SubclassDlgItem(IDC_EDIT_CPORT_BPC, this);
	m_edit_cport_bpc.bkColor(RGB(50, 100, 150));
	m_edit_cport_bpc.textColor(RGB(255, 255,255));
	m_edit_cport_bpc.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_cport_ec.SubclassDlgItem(IDC_EDIT_CPORT_EC, this);
	m_edit_cport_ec.bkColor(RGB(50, 100, 150));
	m_edit_cport_ec.textColor(RGB(255, 255,255));
	m_edit_cport_ec.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_cport_front.SubclassDlgItem(IDC_EDIT_CPORT_FRONT, this);
	m_edit_cport_front.bkColor(RGB(50, 100, 150));
	m_edit_cport_front.textColor(RGB(255, 255,255));
	m_edit_cport_front.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_cport_next.SubclassDlgItem(IDC_EDIT_CPORT_NEXT, this);
	m_edit_cport_next.bkColor(RGB(50, 100, 150));
	m_edit_cport_next.textColor(RGB(255, 255,255));
	m_edit_cport_next.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_cport_xgem.SubclassDlgItem(IDC_EDIT_CPORT_XGem, this);
	m_edit_cport_xgem.bkColor(RGB(50, 100, 150));
	m_edit_cport_xgem.textColor(RGB(255, 255,255));
	m_edit_cport_xgem.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_sport_bpc.SubclassDlgItem(IDC_EDIT_SPORT_BPC, this);
	m_edit_sport_bpc.bkColor(RGB(50, 100, 150));
	m_edit_sport_bpc.textColor(RGB(255, 255,255));
	m_edit_sport_bpc.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_sport_front.SubclassDlgItem(IDC_EDIT_SPORT_FRONT, this);
	m_edit_sport_front.bkColor(RGB(50, 100, 150));
	m_edit_sport_front.textColor(RGB(255, 255,255));
	m_edit_sport_front.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	m_edit_sport_next.SubclassDlgItem(IDC_EDIT_SPORT_NEXT, this);
	m_edit_sport_next.bkColor(RGB(50, 100, 150));
	m_edit_sport_next.textColor(RGB(255, 255,255));
	m_edit_sport_next.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");

}

void CScreen_Admin::OnAdmin_Label_Set()
{
	m_msg_cport_router.SetFont(Func.mp_admin_font);
	m_msg_cport_router.SetWindowText(_T("Port"));
	m_msg_cport_router.SetCenterText();
	m_msg_cport_router.SetColor(RGB(0,0,255));
	m_msg_cport_router.SetGradientColor(RGB(0,0,0));
	m_msg_cport_router.SetTextColor(RGB(255,255,255));

	m_msg_cport_bpc.SetFont(Func.mp_admin_font);
	m_msg_cport_bpc.SetWindowText(_T("Port"));
	m_msg_cport_bpc.SetCenterText();
	m_msg_cport_bpc.SetColor(RGB(0,0,255));
	m_msg_cport_bpc.SetGradientColor(RGB(0,0,0));
	m_msg_cport_bpc.SetTextColor(RGB(255,255,255));

	m_msg_cport_ec.SetFont(Func.mp_admin_font);
	m_msg_cport_ec.SetWindowText(_T("Port"));
	m_msg_cport_ec.SetCenterText();
	m_msg_cport_ec.SetColor(RGB(0,0,255));
	m_msg_cport_ec.SetGradientColor(RGB(0,0,0));
	m_msg_cport_ec.SetTextColor(RGB(255,255,255));

	m_msg_cport_front.SetFont(Func.mp_admin_font);
	m_msg_cport_front.SetWindowText(_T("Port"));
	m_msg_cport_front.SetCenterText();
	m_msg_cport_front.SetColor(RGB(0,0,255));
	m_msg_cport_front.SetGradientColor(RGB(0,0,0));
	m_msg_cport_front.SetTextColor(RGB(255,255,255));

	m_msg_cport_next.SetFont(Func.mp_admin_font);
	m_msg_cport_next.SetWindowText(_T("Port"));
	m_msg_cport_next.SetCenterText();
	m_msg_cport_next.SetColor(RGB(0,0,255));
	m_msg_cport_next.SetGradientColor(RGB(0,0,0));
	m_msg_cport_next.SetTextColor(RGB(255,255,255));

	m_msg_cip_router.SetFont(Func.mp_admin_font);
	m_msg_cip_router.SetWindowText(_T("IP Address"));
	m_msg_cip_router.SetCenterText();
	m_msg_cip_router.SetColor(RGB(0,0,255));
	m_msg_cip_router.SetGradientColor(RGB(0,0,0));
	m_msg_cip_router.SetTextColor(RGB(255,255,255));

	m_msg_cip_bpc.SetFont(Func.mp_admin_font);
	m_msg_cip_bpc.SetWindowText(_T("IP Address"));
	m_msg_cip_bpc.SetCenterText();
	m_msg_cip_bpc.SetColor(RGB(0,0,255));
	m_msg_cip_bpc.SetGradientColor(RGB(0,0,0));
	m_msg_cip_bpc.SetTextColor(RGB(255,255,255));

	m_msg_cip_ec.SetFont(Func.mp_admin_font);
	m_msg_cip_ec.SetWindowText(_T("IP Address"));
	m_msg_cip_ec.SetCenterText();
	m_msg_cip_ec.SetColor(RGB(0,0,255));
	m_msg_cip_ec.SetGradientColor(RGB(0,0,0));
	m_msg_cip_ec.SetTextColor(RGB(255,255,255));

	m_msg_cip_front.SetFont(Func.mp_admin_font);
	m_msg_cip_front.SetWindowText(_T("IP Address"));
	m_msg_cip_front.SetCenterText();
	m_msg_cip_front.SetColor(RGB(0,0,255));
	m_msg_cip_front.SetGradientColor(RGB(0,0,0));
	m_msg_cip_front.SetTextColor(RGB(255,255,255));

	m_msg_cip_next.SetFont(Func.mp_admin_font);
	m_msg_cip_next.SetWindowText(_T("IP Address"));
	m_msg_cip_next.SetCenterText();
	m_msg_cip_next.SetColor(RGB(0,0,255));
	m_msg_cip_next.SetGradientColor(RGB(0,0,0));
	m_msg_cip_next.SetTextColor(RGB(255,255,255));

	m_msg_sport_bpc.SetFont(Func.mp_admin_font);
	m_msg_sport_bpc.SetWindowText(_T("BPC Port"));
	m_msg_sport_bpc.SetCenterText();
	m_msg_sport_bpc.SetColor(RGB(0,0,255));
	m_msg_sport_bpc.SetGradientColor(RGB(0,0,0));
	m_msg_sport_bpc.SetTextColor(RGB(255,255,255));

	m_msg_sport_front.SetFont(Func.mp_admin_font);
	m_msg_sport_front.SetWindowText(_T("Front Machine Port"));
	m_msg_sport_front.SetCenterText();
	m_msg_sport_front.SetColor(RGB(0,0,255));
	m_msg_sport_front.SetGradientColor(RGB(0,0,0));
	m_msg_sport_front.SetTextColor(RGB(255,255,255));

	m_msg_sport_next.SetFont(Func.mp_admin_font);
	m_msg_sport_next.SetWindowText(_T("Next Interface Port"));
	m_msg_sport_next.SetCenterText();
	m_msg_sport_next.SetColor(RGB(0,0,255));
	m_msg_sport_next.SetGradientColor(RGB(0,0,0));
	m_msg_sport_next.SetTextColor(RGB(255,255,255));

	m_msg_cip_xgem.SetFont(Func.mp_admin_font);
	m_msg_cip_xgem.SetWindowText(_T("IP Address"));
	m_msg_cip_xgem.SetCenterText();
	m_msg_cip_xgem.SetColor(RGB(0,0,255));
	m_msg_cip_xgem.SetGradientColor(RGB(0,0,0));
	m_msg_cip_xgem.SetTextColor(RGB(255,255,255));

	m_msg_cport_xgem.SetFont(Func.mp_admin_font);
	m_msg_cport_xgem.SetWindowText(_T("XGem Port"));
	m_msg_cport_xgem.SetCenterText();
	m_msg_cport_xgem.SetColor(RGB(0,0,255));
	m_msg_cport_xgem.SetGradientColor(RGB(0,0,0));
	m_msg_cport_xgem.SetTextColor(RGB(255,255,255));
}

void CScreen_Admin::OnAdmin_Data_Set()
{
	// network
	DWORD dwIP = g_client_router.GetDwIP();
	m_cip_router.SetAddress( dwIP );
	m_edit_cport_router.SetWindowText( g_client_router.GetPortStr() );

	dwIP = g_client_bpc.GetDwIP();
	m_cip_bpc.SetAddress( dwIP );
	m_edit_cport_bpc.SetWindowText( g_client_bpc.GetPortStr() );

	dwIP = g_client_ec.GetDwIP();
	m_cip_ec.SetAddress( dwIP );
	m_edit_cport_ec.SetWindowText( g_client_ec.GetPortStr() );

	dwIP = g_client_front.GetDwIP();
	m_cip_front.SetAddress( dwIP );
	m_edit_cport_front.SetWindowText( g_client_front.GetPortStr() );

	dwIP = g_client_next.GetDwIP();
	m_cip_next.SetAddress( dwIP );
	m_edit_cport_next.SetWindowText( g_client_next.GetPortStr() );

	dwIP = g_client_xgem.GetDwIP();
	m_cip_xgem.SetAddress( dwIP );
	m_edit_cport_xgem.SetWindowText( g_client_xgem.GetPortStr() );

	m_edit_sport_bpc.SetWindowText( g_server_bpc.GetPortStr() );
	m_edit_sport_front.SetWindowText( g_server_front.GetPortStr() );
	m_edit_sport_next.SetWindowText( g_server_next.GetPortStr() );

	// virtual
	m_chk_Virtual.SetCheck( st_handler.mn_virtual_mode );
	m_chk_virtual_supply.SetCheck( st_handler.mn_virtual_supply );
	m_chk_bufferin_chk.SetCheck( st_handler.mn_buffer_in);
	m_chk_fullinline.SetCheck( st_handler.mn_fullinline_mode);
	// machine position
	if( g_handler.GetMachinePos() == EMPOS_ALONE )		m_radio_pos_alone.SetCheck( true );
	else if( g_handler.GetMachinePos() == EMPOS_FRONT )	m_radio_pos_front.SetCheck( true );
	else if( g_handler.GetMachinePos() == EMPOS_REAR )	m_radio_pos_rear.SetCheck( true );

	if(st_handler.mn_pogo_type == CTL_YES)
	{
		m_radio_pogo_use.SetCheck( true );
		m_radio_pogo_notuse.SetCheck( false );
	}
	else
	{
		m_radio_pogo_use.SetCheck( false );
		m_radio_pogo_notuse.SetCheck( true );
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		m_radio_mirror_use.SetCheck( true );
		m_radio_mirror_notuse.SetCheck( false );
	}
	else
	{
		m_radio_mirror_use.SetCheck( false );
		m_radio_mirror_notuse.SetCheck( true );
	}

	
	if(st_basic.mn_alignBuffer_type == CTL_YES)
	{
		m_radio_alignuse.SetCheck( true );
		m_radio_alignnotuse.SetCheck( false );
	}
	else
	{
		m_radio_alignuse.SetCheck( false );
		m_radio_alignnotuse.SetCheck( true );
	}

	////2015.0604
	if(st_handler.mn_version_mode == CTL_YES)
	{
		m_radio_version_mode_use.SetCheck( true );
		m_radio_version_mode_notuse.SetCheck( false );
	}
	else
	{
		m_radio_version_mode_use.SetCheck( false );
		m_radio_version_mode_notuse.SetCheck( true );
	}
	////

}




void CScreen_Admin::OnAdmin_Dgt_Set()
{
	m_dgt_retest_cnt.SubclassDlgItem(IDC_DGT_PICK_COUNT, this);
	m_dgt_retest_cnt.SetStyle(IDB_BIG3, 4);
	m_dgt_retest_cnt.SetValue(st_handler.mn_retest_cnt );

	////2015.0518
	m_dgt_multi_retest_cnt.SubclassDlgItem(IDC_DGT_RETEST_CNT2, this);
	m_dgt_multi_retest_cnt.SetStyle(IDB_BIG3, 4);
	m_dgt_multi_retest_cnt.SetValue(st_handler.mn_multi_retest_cnt );
	////

	m_dgt_beat_cnt.SubclassDlgItem(IDC_DGT_BEAT_CNT, this);
	m_dgt_beat_cnt.SetStyle(IDB_BIG3, 4);
	m_dgt_beat_cnt.SetValue(st_handler.mn_beat_cnt );

	m_dgt_autosocketoff_cnt.SubclassDlgItem(IDC_DGT_AUTO_SOCKET_OFF, this);
	m_dgt_autosocketoff_cnt.SetStyle(IDB_BIG3, 4);
	m_dgt_autosocketoff_cnt.SetValue(st_handler.mn_AutoSocketOff_Cnt );

}

void CScreen_Admin::OnBtnBufferdbDelete() 
{
	int n_response;  // 대화 상자 리턴 플래그
	
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("BUFFER DB를 삭제하시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("You want to delete BUFFER DB?");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
		{
			if(Mysql.Database_Select("AMT8520"))
			{	
				Mysql.Table_Delete("BUFFER_DATA");
				
				AfxMessageBox("BUFFER_DATA DELETE");
			}
		}
	}
	else if (n_response == IDCANCEL)
	{
		
	}
}

void CScreen_Admin::OnChkVirtual() 
{
	// TODO: Add your control notification handler code here
	CString mstr_temp;
	
	st_handler.mn_virtual_mode = !st_handler.mn_virtual_mode;
	
	m_chk_Virtual.SetCheck(st_handler.mn_virtual_mode);
	
	mstr_temp.Format("%d", st_handler.mn_virtual_mode);
	:: WritePrivateProfileString("BasicData", "Virtual_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
}

void CScreen_Admin::OnBtnFileEditor() 
{
	::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 8, 2);
}

void CScreen_Admin::OnBtnCipSetBpc() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_bpc.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "BPC IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the IP Next Step APCT");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_bpc.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_bpc.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_BPC", LPCTSTR(strip), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnTstBpc() 
{
	// TODO: Add your control notification handler code here
	g_client_bpc.PushSendMsg( "BPC Client Test" );
}

void CScreen_Admin::OnBtnCipSetEc() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_ec.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "EC Server IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the IP Next Step APCT");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_ec.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_ec.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_EC", LPCTSTR(strip), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnCipSetFront() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_front.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "FRONT Tester IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the IP Next Step APCT");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_front.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_front.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_FRONT", LPCTSTR(strip), st_path.mstr_basic);
	}
	
}

void CScreen_Admin::OnBtnCipSetNext() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_next.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "Next Machine IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the Next Machine IP");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_next.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_next.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_NEXT", LPCTSTR(strip), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnCportSetBpc() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_BPC))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_bpc.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_BPC))->SetWindowText(strport);

		g_client_bpc.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_BPC", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnCportSetEc() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_EC))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_ec.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_EC))->SetWindowText(strport);

		g_client_ec.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_EC", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnCportSetFront() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_FRONT))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_front.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_FRONT))->SetWindowText(strport);

		g_client_front.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_FRONT", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnCportSetNext() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_NEXT))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_next.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_NEXT))->SetWindowText(strport);

		g_client_next.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_NEXT", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnSportSetFront() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_SPORT_FRONT))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_sport_set_front.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_SPORT_FRONT))->SetWindowText(strport);

		g_server_front.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "SPORT_FRONT", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnSportSetBpc() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_SPORT_BPC))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_sport_set_bpc.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_SPORT_BPC))->SetWindowText(strport);

		g_server_bpc.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "SPORT_BPC", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnSportSetNext() 
{
	// TODO: Add your control notification handler code here
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_SPORT_NEXT))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Rear APCT port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_sport_set_next.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_SPORT_NEXT))->SetWindowText(strport);

		g_server_next.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "SPORT_NEXT", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnRadioPositionFront() 
{
	// TODO: Add your control notification handler code here
	m_radio_pos_front.SetCheck( true );
	m_radio_pos_rear.SetCheck( false );
	m_radio_pos_alone.SetCheck( false );

	g_handler.SetMachinePos( EMPOS_FRONT );
	:: WritePrivateProfileString("ADMIN", "EMPOS", "0", st_path.mstr_basic);
}

void CScreen_Admin::OnRadioPositionRear() 
{
	// TODO: Add your control notification handler code here
	m_radio_pos_front.SetCheck( false );
	m_radio_pos_rear.SetCheck( true );
	m_radio_pos_alone.SetCheck( false );

	g_handler.SetMachinePos( EMPOS_REAR );
	:: WritePrivateProfileString("ADMIN", "EMPOS", "1", st_path.mstr_basic);
}

void CScreen_Admin::OnRadioPositionAlone() 
{
	// TODO: Add your control notification handler code here
	m_radio_pos_front.SetCheck( false );
	m_radio_pos_rear.SetCheck( false );
	m_radio_pos_alone.SetCheck( true );

	g_handler.SetMachinePos( EMPOS_ALONE );
	:: WritePrivateProfileString("ADMIN", "EMPOS", "2", st_path.mstr_basic);
}

void CScreen_Admin::OnBtnTstNext() 
{
	// TODO: Add your control notification handler code here
	g_client_next.PushSendMsg( "NEXT Client Test" );
}

void CScreen_Admin::OnAdmin_Init_XSafetyPosition()
{
	TSpread* Grid;
	int row = 1;
	int col = 2;
	
	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_XSAFETY_POSITION );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid -> SetColWidthInPixels(1, 110 );
	Grid -> SetColWidthInPixels(2, 50 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 22 );
		
		CString strIdx;
		strIdx.Format( "%d", iw + 1 );
		GridData( IDC_CUSTOM_XSAFETY_POSITION, iw + 1, 1, strIdx );
		for( int ih=0; ih<col; ih++ )
		{
			GridFont( IDC_CUSTOM_XSAFETY_POSITION, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_XSAFETY_POSITION, STATIC, iw + 1, ih + 1, 0);
			if( ih % 2 == 1 )
			{
				GridData( IDC_CUSTOM_XSAFETY_POSITION, iw + 1, ih + 1, "Read" );
				GridColor( IDC_CUSTOM_XSAFETY_POSITION, iw + 1, ih + 1, BLUE_L, BLACK_C );
			}
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Init_YSafetyPosition()
{
	TSpread* Grid;
	int row = 2;
	int col = 3;
	
	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_YSAFETY_POSITION );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid -> SetColWidthInPixels(1, 50 );
	Grid -> SetColWidthInPixels(2, 70 );
	Grid -> SetColWidthInPixels(3, 40 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 22 );
		
		CString strIdx;
		strIdx.Format( "%d", iw + 1 );
		GridData( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, 1, strIdx );
		for( int ih=0; ih<col; ih++ )
		{
			if     ( iw == 0 && ih == 0)
			{
				GridData( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, "Tray" );
				GridColor( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, GREEN_L, BLACK_C );
			}
			else if( iw == 1 && ih == 0)
			{
				GridData( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, "Reject" );
				GridColor( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, GREEN_L, BLACK_C );
			}
			GridFont( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_PICKER_POSITION, STATIC, iw + 1, ih + 1, 0);
			if( ih % 3 == 2 )
			{
				GridData( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, "Read" );
				GridColor( IDC_CUSTOM_YSAFETY_POSITION, iw + 1, ih + 1, BLUE_L, BLACK_C );
			}
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Init_PickerPosition()
{
	TSpread* Grid;
	int row = 4;
	int col = 2;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_PICKER_POSITION );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 40 );
	Grid -> SetColWidthInPixels(2, 116 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 20 );

		CString strIdx;
		strIdx.Format( "%d", iw + 1 );
		GridData( IDC_CUSTOM_PICKER_POSITION, iw + 1, 1, strIdx );
		for( int ih=0; ih<col; ih++ )
		{
			GridFont( IDC_CUSTOM_PICKER_POSITION, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_PICKER_POSITION, STATIC, iw + 1, ih + 1, 0);
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	Grid->SetColor(col, row, bk, tk);
	Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::GridControl(UINT nID, int type, int row, int col, int pos)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	SS_CELLTYPE CellType;
	SS_DATEFORMAT dateFormat = { TRUE, '/', IDF_DDMMYY, FALSE };
	SS_TIMEFORMAT tmFormat;
	CString strList;
	strList = "combo1\tcombo2\tcombo3\tcombo4\tcombo5\tcombo6";
	double a = 0.5;

	switch (type)
	{
		case DATE:			
			Grid->SetTypeDate(&CellType, 0 , &dateFormat, NULL, NULL);
			break;

		case EDIT:
			Grid->SetTypeEdit(&CellType,ES_CENTER,60,SS_CHRSET_CHR,SS_CASE_NOCASE);
			break;

		case COMBO:
			// SetValue(col, row, data); data = ""0"(1선택) data = "1"(2선택) data = "2"(3선택)
			Grid->SetTypeComboBox(&CellType, 0, strList);
			break;

		case BUTTON:
			if (pos == 0)
			{
				Grid->SetTypeButton(&CellType, SBS_DEFPUSHBUTTON, "Go", NULL, SUPERBTN_PICT_NONE,
					NULL, SUPERBTN_PICT_NONE, SUPERBTN_TYPE_NORMAL, 2, NULL);
			}
			else
			{
				Grid->SetTypeButton(&CellType, SBS_DEFPUSHBUTTON, "Read", NULL, SUPERBTN_PICT_NONE,
					NULL, SUPERBTN_PICT_NONE,SUPERBTN_TYPE_NORMAL, 2, NULL);
			}
			break;

		case STATIC:
			Grid->SetTypeStaticText(&CellType, SS_TEXT_CENTER | SS_TEXT_VCENTER);
			break;

		case TIME:
			tmFormat.b24Hour=TRUE;
			tmFormat.bSpin=FALSE;
			tmFormat.bSeconds=FALSE;
			tmFormat.cSeparator=':';
			Grid->SetTypeTime(&CellType, 0, &tmFormat, NULL, NULL);
			break;

		case PERCENT:
			Grid->SetTypePercentEx(&CellType,ES_LEFTALIGN,2,.01,100,".",0,0,0,0,1.001);
			break;

		case CHECK1:
			Grid->SetTypeCheckBox(&CellType,BS_CENTER, "", 
				"CheckUp", BT_BITMAP,
				"CheckDown", BT_BITMAP,
				"CheckFocusUp", BT_BITMAP,
				"CheckFocusDown", BT_BITMAP,
				"CheckDisabledUp", BT_BITMAP,
				"CheckDisabledDown", BT_BITMAP);
			break;

		case NUMBER:
			Grid->SetTypeNumberEx(&CellType,0,pos, 0, 10000,0,".",",",0,0,1,0,1.001);
			break;
	}
	
	Grid->SetCellType(col,row,&CellType);

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

CString CScreen_Admin::GetGridData( UINT nID, int row, int col )
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);

	Grid = NULL;
	delete Grid;

	return szBuf;
}

void CScreen_Admin::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Display_PickerPosition()
{
	CString strPos0, strPos1, strPos2, strPos3;
	strPos0.Format( "%.03f", st_handler.md_picker_position[0] );
	strPos1.Format( "%.03f", st_handler.md_picker_position[1] );
	strPos2.Format( "%.03f", st_handler.md_picker_position[2] );
	strPos3.Format( "%.03f", st_handler.md_picker_position[3] );
	
	GridData( IDC_CUSTOM_PICKER_POSITION, 1, 2, strPos0 );
	GridData( IDC_CUSTOM_PICKER_POSITION, 2, 2, strPos1 );
	GridData( IDC_CUSTOM_PICKER_POSITION, 3, 2, strPos2 );
	GridData( IDC_CUSTOM_PICKER_POSITION, 4, 2, strPos3 );
}

void CScreen_Admin::OnAdmin_Display_SafetyPosition()
{
	CString strPosX, strPosY[2];
	strPosX.Format( "%.03f", st_handler.md_safty_rbtx );
	strPosY[0].Format( "%.03f", st_handler.md_safty_rbty[0] );
	strPosY[1].Format( "%.03f", st_handler.md_safty_rbty[1] );
	
	GridData( IDC_CUSTOM_XSAFETY_POSITION, 1, 1, strPosX );
	GridData( IDC_CUSTOM_YSAFETY_POSITION, 1, 2, strPosY[0] );
	GridData( IDC_CUSTOM_YSAFETY_POSITION, 2, 2, strPosY[1] );
}

void CScreen_Admin::OnCellClick( WPARAM wParam, LPARAM lParam )
{
 	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;
 
 	if(wParam==IDC_CUSTOM_PICKER_POSITION )
 	{
		if( lpcc->Col == 1 )
			return;

		int iSelectedIdx = lpcc->Row - 1;
		CString strText;
		st_msg.mstr_keypad_msg.Format("[%d PICKER POSITION]", lpcc->Row );
		strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_picker_position[iSelectedIdx], st_msg.mstr_keypad_msg);
		st_handler.md_picker_position[iSelectedIdx] = atof(strText);

		OnAdmin_Display_PickerPosition();

		CString strKey;
		strKey.Format( "PICKER_POSITION_%d", iSelectedIdx );
		CString str_temp;
		str_temp.Format( "%.03f", st_handler.md_picker_position[iSelectedIdx] );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(str_temp), st_path.mstr_basic);

////2015.0623
		if(iSelectedIdx == 1)	
		{
			st_motor[MOTOR_ROBOT_Y].d_pos[27] = st_handler.md_picker_position[1] ;
		}
////
	}
	else if( wParam == IDC_CUSTOM_COK_INDEX )
	{
		//COKINDEX_GS1
		if( lpcc->Col == 1 )
			return;

		int iSelectedIdx = lpcc->Row - 1;
		CString strIndex[5] = { "GS1", "GS2", "MSATA", "25_REVERSE", "25_FRONT" };

		CString strText;
		st_msg.mstr_keypad_msg.Format("[COK INDEX for %s]", strIndex[iSelectedIdx] );
		strText = KeyPad.GetNumEditString_I( 0, 6, st_handler.mn_cokIndex[iSelectedIdx] + 1, st_msg.mstr_keypad_msg );
		st_handler.mn_cokIndex[iSelectedIdx] = atoi( strText ) - 1;

		OnAdmin_Display_CokIndex();

		CString strKey;
		strKey.Format( "COKINDEX_%s", strIndex[iSelectedIdx] );
		CString str_temp;
		str_temp.Format( "%d", st_handler.mn_cokIndex[iSelectedIdx] );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(str_temp), st_path.mstr_basic);

	}
	else if( wParam == IDC_CUSTOM_TRAY_PLACE_OFFSET )
	{
		if( lpcc->Col == 1 )
			return;

		int iSelectedIdx = lpcc->Row - 1;
		CString strText;

		if( iSelectedIdx == 0 )
			st_msg.mstr_keypad_msg.Format("[TRAY PLACE OFFSET (X)]" );
		else
			st_msg.mstr_keypad_msg.Format("[TRAY PLACE OFFSET (Y)]" );

		strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_tray_place_offset[iSelectedIdx], st_msg.mstr_keypad_msg);
		st_handler.md_tray_place_offset[iSelectedIdx] = atof(strText);

		OnAdmin_Display_TrayPlaceOffset();

		CString strKey;
		strKey.Format( "TRAY_PLACE_OFFSET_%d", iSelectedIdx );
		CString str_temp;
		str_temp.Format( "%.03f", st_handler.md_tray_place_offset[iSelectedIdx] );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(str_temp), st_path.mstr_basic);
	}

	else if( wParam == IDC_CUSTOM_COK_TEACHING )
	{
		if( lpcc->Col == 1 )
			return;

		CString str_tmp;
		int iSelectedIdx = lpcc->Row - 1;
// 		m_grid_motor_info = (TSpread*)GetDlgItem(IDC_CUSTOM_COK_TEACHING);


		CString strName = GetGridData( wParam, lpcc->Row, 1 );
		st_msg.mstr_keyboard_msg.Format( "%s", strName );
		double dOldteach = st_handler.md_common_teaching[iSelectedIdx];
		CString strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_common_teaching[iSelectedIdx], st_msg.mstr_keypad_msg);
		st_handler.md_common_teaching[iSelectedIdx] = atof( strText );
		
		OnAdmin_Display_COK_Teaching();
		
		CString strKey;
		strKey.Format( "COMMON_TEACHING_%02d", iSelectedIdx );
		CString strTemp;
		strTemp.Format( "%.03f", st_handler.md_common_teaching[iSelectedIdx] );
		::WritePrivateProfileString( "BasicData", strKey, LPCTSTR(strTemp), st_path.mstr_basic );


		str_tmp.Format( "Admin [%s] Axis teaching save : %.3f -> %.3f", strName, float(dOldteach), float(st_handler.md_common_teaching[iSelectedIdx]) );
		Func.On_LogFile_Add(0, str_tmp);
		Func.On_LogFile_Add(99, str_tmp);

	}//2016.0329
	else if(wParam==IDC_CUSTOM_XSAFETY_POSITION )
	{
		if( lpcc->Row != 1 || ( lpcc->Col != 1 && lpcc->Col != 2 ) )
			return;

		int iSelectedIdx = lpcc->Row - 1;
		CString strText, str_temp, str_tmp;
		double dOldteach = 0.0, value = 0.0;
		dOldteach = st_handler.md_safty_rbtx;
		if( lpcc->Col == 1 )
		{
			st_msg.mstr_keypad_msg = _T(" ROBOT X SAFETY POSITION ");
			strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_safty_rbtx, st_msg.mstr_keypad_msg);
			st_handler.md_safty_rbtx = atof(strText);
			
			OnAdmin_Display_SafetyPosition();
			
			str_temp.Format( "%.03f", st_handler.md_safty_rbtx );
			:: WritePrivateProfileString("BasicData", "ROBOT_X_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);

			str_tmp.Format( "Admin [%s] Axis save : %.3f -> %.3f", st_msg.mstr_keypad_msg, float(dOldteach), float(st_handler.md_safty_rbtx) );
			Func.On_LogFile_Add(0, str_tmp);
			Func.On_LogFile_Add(99, str_tmp);

		}
		else
		{
			if (COMI.mn_motorbd_init_end != TRUE)  
			{
				if (DoModal_Msg( g_local.Get("L_M_0000") ) == IDOK)
					return ;
			}
			
			value = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);  // FASTECH 모터 특정 축의 커맨트 위치 리턴 함수 //
			
			int nResponse = Func.On_Encoder_Check(value);
			
			if (nResponse == CTL_YES)
			{
				st_handler.md_safty_rbtx = value;
				OnAdmin_Display_SafetyPosition();
				str_temp.Format( "%.03f", st_handler.md_safty_rbtx );
				:: WritePrivateProfileString("BasicData", "ROBOT_X_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);

				str_tmp.Format( "Admin [ROBOT_X_SAFETY] Axis save : %.3f -> %.3f", float(dOldteach), float(st_handler.md_safty_rbtx) );
				Func.On_LogFile_Add(0, str_tmp);
				Func.On_LogFile_Add(99, str_tmp);
			}
		}
	}
	else if(wParam==IDC_CUSTOM_YSAFETY_POSITION )
	{
		if( ( lpcc->Row != 1 && lpcc->Row != 2 ) || ( lpcc->Col != 2 && lpcc->Col != 3 ) )
			return;
		
		int iSelectedIdx = lpcc->Row - 1;
		CString strText, str_temp, str_tmp;
		double dOldteach = 0.0, dOldteach1 = 0.0, value = 0.0;
		if( lpcc->Col == 2 )
		{
			if( lpcc->Row == 1 )
			{
				dOldteach = st_handler.md_safty_rbty[0];
				strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_safty_rbty[0], "ROBOT_Y_TRAY_SAFETY");
				st_handler.md_safty_rbty[iSelectedIdx] = atof(strText);
				
				OnAdmin_Display_SafetyPosition();
				
				str_temp.Format( "%.03f", st_handler.md_safty_rbty[0] );
				:: WritePrivateProfileString("BasicData", "ROBOT_Y_TRAY_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);
				str_tmp.Format( "Admin [ROBOT_Y_TRAY_SAFETY] Axis save : %.3f -> %.3f", float(dOldteach), float(st_handler.md_safty_rbty[0]) );
				Func.On_LogFile_Add(0, str_tmp);
				Func.On_LogFile_Add(99, str_tmp);
			}
			else
			{
				dOldteach = st_handler.md_safty_rbty[1];
				strText = KeyPad.GetNumEditString_D(-10000, 10000, st_handler.md_safty_rbty[1], "ROBOT_Y_REJECT_SAFETY");
				st_handler.md_safty_rbty[iSelectedIdx] = atof(strText);
				
				OnAdmin_Display_SafetyPosition();
				
				str_temp.Format( "%.03f", st_handler.md_safty_rbty[1] );
				:: WritePrivateProfileString("BasicData", "ROBOT_Y_REJECT_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);
				str_tmp.Format( "Admin [ROBOT_Y_REJECT_SAFETY] Axis save : %.3f -> %.3f", float(dOldteach), float(st_handler.md_safty_rbty[1]) );
				Func.On_LogFile_Add(0, str_tmp);
				Func.On_LogFile_Add(99, str_tmp);
			}
		}
		else
		{
// 			if (COMI.mn_motorbd_init_end != TRUE)  
// 			{
// 				if (DoModal_Msg( g_local.Get("L_M_0000") ) == IDOK)
// 					return ;
// 			}
			dOldteach = st_handler.md_safty_rbty[0];			
			dOldteach1 = st_handler.md_safty_rbty[1];			
			value = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);  // FASTECH 모터 특정 축의 커맨트 위치 리턴 함수 //
			
			int nResponse = Func.On_Encoder_Check(value);
			
			if( lpcc->Row == 1 )
			{
				if (nResponse == CTL_YES)
				{
					st_handler.md_safty_rbty[0] = value;

					OnAdmin_Display_SafetyPosition();
					str_temp.Format( "%.03f", st_handler.md_safty_rbty[0] );
					:: WritePrivateProfileString("BasicData", "ROBOT_Y_TRAY_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);
					
					str_tmp.Format( "Admin [ROBOT_Y_TRAY_SAFETY] Axis save : %.3f -> %.3f", float(dOldteach), float(st_handler.md_safty_rbty[0]) );
					Func.On_LogFile_Add(0, str_tmp);
					Func.On_LogFile_Add(99, str_tmp);
				}
			}
			else
			{
				if (nResponse == CTL_YES)
				{
					st_handler.md_safty_rbty[1] = value;

					OnAdmin_Display_SafetyPosition();
					str_temp.Format( "%.03f", st_handler.md_safty_rbty[1] );
					:: WritePrivateProfileString("BasicData", "ROBOT_Y_REJECT_SAFETY", LPCTSTR(str_temp), st_path.mstr_basic);

					str_tmp.Format( "Admin [ROBOT_Y_REJECT_SAFETY] Axis save : %.3f -> %.3f", float(dOldteach1), float(st_handler.md_safty_rbty[1]) );
					Func.On_LogFile_Add(0, str_tmp);
					Func.On_LogFile_Add(99, str_tmp);
				}
			}
		}
	}
}

void CScreen_Admin::OnChkVirtualSupply() 
{
	// TODO: Add your control notification handler code here
	CString mstr_temp;
	
	st_handler.mn_virtual_supply = !st_handler.mn_virtual_supply;
	
	m_chk_virtual_supply.SetCheck(st_handler.mn_virtual_supply);
	
	mstr_temp.Format("%d", st_handler.mn_virtual_supply);
	:: WritePrivateProfileString("BasicData", "Virtual_Supply", LPCTSTR(mstr_temp), st_path.mstr_basic);
}

void CScreen_Admin::OnBtnCipSetRouter() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_router.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "ROUTER IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the IP Router");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_router.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_router.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_ROUTER", LPCTSTR(strip), st_path.mstr_basic);
	}
	
}

void CScreen_Admin::OnBtnCportSetRouter() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_ROUTER))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("Router Port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_router.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_ROUTER))->SetWindowText(strport);

		g_client_router.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_ROUTER", LPCTSTR(strport), st_path.mstr_basic);
	}
}

void CScreen_Admin::OnBtnTstRouter() 
{
	g_client_router.PushSendMsg( "BPC Client Test" );
	
}

void CScreen_Admin::OnDgtRetestCnt() 
{
 	char chr_buf[20] ;
 	
	int mn_count = m_dgt_retest_cnt.GetValue();
	CString	mstr_temp = LPCTSTR(_itoa(mn_count, chr_buf, 10));
 	
 	st_msg.mstr_keypad_msg = _T("RETEST MAX");
 	st_msg.mstr_keypad_val = mstr_temp;
 	mstr_temp = KeyPad.GetNumEditString_I(0, 9, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	st_handler.mn_retest_cnt = atoi( mstr_temp );
	m_dgt_retest_cnt.SetValue( st_handler.mn_retest_cnt );

	mstr_temp.Format("%d", st_handler.mn_retest_cnt );
 	:: WritePrivateProfileString("BasicData", "RETEST_CNT", LPCTSTR(mstr_temp), st_path.mstr_basic);
}

void CScreen_Admin::OnDgtAutoSocketOff() 
{
	char chr_buf[20] ;
 	
	int mn_count = m_dgt_autosocketoff_cnt.GetValue();
	CString	mstr_temp = LPCTSTR(_itoa(mn_count, chr_buf, 10));
 	
 	st_msg.mstr_keypad_msg = _T("AUTO SOCKET OFF 연속 실패 횟수 (0 = 사용안함)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_keypad_msg = _T("AUTO SOCKET OFF consecutive failures (0 = Do not use)");//20130930

 	st_msg.mstr_keypad_val = mstr_temp;
 	mstr_temp = KeyPad.GetNumEditString_I(0, 10, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	st_handler.mn_AutoSocketOff_Cnt = atoi( mstr_temp );
	m_dgt_autosocketoff_cnt.SetValue( st_handler.mn_AutoSocketOff_Cnt );

	mstr_temp.Format("%d", st_handler.mn_AutoSocketOff_Cnt );
 	:: WritePrivateProfileString("BasicData", "AUTO_SOCKET_OFF_CNT", LPCTSTR(mstr_temp), st_path.mstr_basic);
}

void CScreen_Admin::OnAdmin_Init_TrayPlaceOffset()
{
	TSpread* Grid;
	int row = 2;
	int col = 2;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_TRAY_PLACE_OFFSET );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 40 );
	Grid -> SetColWidthInPixels(2, 116 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 20 );
		for( int ih=0; ih<col; ih++ )
		{
			GridFont( IDC_CUSTOM_TRAY_PLACE_OFFSET, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_TRAY_PLACE_OFFSET, STATIC, iw + 1, ih + 1, 0);
		}
	}
	
	GridData( IDC_CUSTOM_TRAY_PLACE_OFFSET, 1, 1, "X" );
	GridData( IDC_CUSTOM_TRAY_PLACE_OFFSET, 2, 1, "Y" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Display_TrayPlaceOffset()
{
	CString strOffsetX, strOffsetY;
	strOffsetX.Format( "%.03f", st_handler.md_tray_place_offset[0] );
	strOffsetY.Format( "%.03f", st_handler.md_tray_place_offset[1] );
	
	GridData( IDC_CUSTOM_TRAY_PLACE_OFFSET, 1, 2, strOffsetX );
	GridData( IDC_CUSTOM_TRAY_PLACE_OFFSET, 2, 2, strOffsetY );
}

void CScreen_Admin::OnDgtBeatCnt() 
{
	// TODO: Add your control notification handler code here
	char chr_buf[20] ;
 	
	int mn_count = m_dgt_beat_cnt.GetValue();
	CString	mstr_temp = LPCTSTR(_itoa(mn_count, chr_buf, 10));
 	
 	st_msg.mstr_keypad_msg = _T("BEAT MAX");
 	st_msg.mstr_keypad_val = mstr_temp;
 	mstr_temp = KeyPad.GetNumEditString_I(0, 3, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	st_handler.mn_beat_cnt = atoi( mstr_temp );
	m_dgt_beat_cnt.SetValue( st_handler.mn_beat_cnt );

	mstr_temp.Format("%d", st_handler.mn_beat_cnt );
 	:: WritePrivateProfileString("BasicData", "BEAT_CNT", LPCTSTR(mstr_temp), st_path.mstr_basic);
}

void CScreen_Admin::OnAdmin_Init_COK_Teaching()
{
	TSpread* Grid;
	int row = 26;
	int col = 2;

	int iGrid = IDC_CUSTOM_COK_TEACHING;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 100 );
	Grid -> SetColWidthInPixels(2, 100 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 16 );
		for( int ih=0; ih<col; ih++ )
		{
			GridFont( iGrid, iw + 1, ih + 1, 15);
			GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
		}
// 		for( int ix=0; ix<col; ix++ )
// 		{
// 			if( ix % 3 == 2 )
// 			{
// 				GridColor( iGrid, iw + 1, ix + 1, BLUE_L, BLACK_C );
// 				GridData( iGrid, iw + 1, ix + 1, "Go" );
// 			}
// 			else if( ix > 0 && ix % 3 == 0 )
// 			{
// 				GridColor( iGrid, iw + 1, ix + 1, BLUE_L, BLACK_C );
// 				GridData( iGrid, iw + 1, ix + 1, "Read" );
// 			}
// 		}
	}
	if( g_local.GetLocalType() == LOCAL_KOR )	//20130930
	{
		GridData( iGrid, 1, 1, "SITE 1 Z" );				GridColor( iGrid, 1, 1, GREEN_L, BLACK_C );
		GridData( iGrid, 2, 1, "SITE 2 Z" );				GridColor( iGrid, 2, 1, GREEN_L, BLACK_C );
		GridData( iGrid, 3, 1, "R PLACE Z" );				GridColor( iGrid, 3, 1, GRAY_C, BLACK_C );
		GridData( iGrid, 4, 1, "R S1 시작 X" );				GridColor( iGrid, 4, 1, RED_A, BLACK_C );
		GridData( iGrid, 5, 1, "R S1 시작 Y" );				GridColor( iGrid, 5, 1, RED_A, BLACK_C );
		GridData( iGrid, 6, 1, "R S1 시작 Z" );				GridColor( iGrid, 6, 1, RED_A, BLACK_C );
		GridData( iGrid, 7, 1, "R S1 끝 X" );				GridColor( iGrid, 7, 1, RED_C, BLACK_C );
		GridData( iGrid, 8, 1, "R S1 끝 Y" );				GridColor( iGrid, 8, 1, RED_C, BLACK_C );
		GridData( iGrid, 9, 1, "R S1 끝 Z" );				GridColor( iGrid, 9, 1, RED_C, BLACK_C );
		GridData( iGrid, 10, 1, "R S2 시작 X" );			GridColor( iGrid, 10, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 11, 1, "R S2 시작 Y" );			GridColor( iGrid, 11, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 12, 1, "R S2 시작 Z" );			GridColor( iGrid, 12, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 13, 1, "R S2 끝 X" );				GridColor( iGrid, 13, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 14, 1, "R S2 끝 Y" );				GridColor( iGrid, 14, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 15, 1, "R S2 끝 Z" );				GridColor( iGrid, 15, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 16, 1, "R COK 시작 X" );			GridColor( iGrid, 16, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 17, 1, "R COK 시작 Y" );			GridColor( iGrid, 17, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 18, 1, "R COK 시작 Z" );			GridColor( iGrid, 18, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 19, 1, "R COK 끝 X" );				GridColor( iGrid, 19, 1, BLACK_L, WHITE_C );
		GridData( iGrid, 20, 1, "R COK 끝 Y" );				GridColor( iGrid, 20, 1, BLACK_L, WHITE_C );
		GridData( iGrid, 21, 1, "R COK 끝 Z" );				GridColor( iGrid, 21, 1, BLACK_L, WHITE_C );
		
		GridData( iGrid, 22, 1, "COK Y 시작(1)" );			GridColor( iGrid, 22, 1, BLUE_A, WHITE_C );
		GridData( iGrid, 23, 1, "COK Y 끝 (6)" );			GridColor( iGrid, 23, 1, BLUE_A, WHITE_C );
		GridData( iGrid, 24, 1, "COK Z Safety" );				GridColor( iGrid, 24, 1, BLUE_D, WHITE_C );
		GridData( iGrid, 25, 1, "COK Z 0" );				GridColor( iGrid, 25, 1, BLUE_D, WHITE_C );
		GridData( iGrid, 26, 1, "COK Z 19" );				GridColor( iGrid, 26, 1, BLUE_D, WHITE_C );

	}
	else
	{
		GridData( iGrid, 1, 1, "SITE 1 Z" );				GridColor( iGrid, 1, 1, GREEN_L, BLACK_C );
		GridData( iGrid, 2, 1, "SITE 2 Z" );				GridColor( iGrid, 2, 1, GREEN_L, BLACK_C );
		GridData( iGrid, 3, 1, "R PLACE Z" );				GridColor( iGrid, 3, 1, GRAY_C, BLACK_C );
		GridData( iGrid, 4, 1, "R S1 Start X" );				GridColor( iGrid, 4, 1, RED_A, BLACK_C );
		GridData( iGrid, 5, 1, "R S1 Start Y" );				GridColor( iGrid, 5, 1, RED_A, BLACK_C );
		GridData( iGrid, 6, 1, "R S1 Start Z" );				GridColor( iGrid, 6, 1, RED_A, BLACK_C );
		GridData( iGrid, 7, 1, "R S1 End X" );				GridColor( iGrid, 7, 1, RED_C, BLACK_C );
		GridData( iGrid, 8, 1, "R S1 End Y" );				GridColor( iGrid, 8, 1, RED_C, BLACK_C );
		GridData( iGrid, 9, 1, "R S1 End Z" );				GridColor( iGrid, 9, 1, RED_C, BLACK_C );
		GridData( iGrid, 10, 1, "R S2 Start X" );			GridColor( iGrid, 10, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 11, 1, "R S2 Start Y" );			GridColor( iGrid, 11, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 12, 1, "R S2 Start Z" );			GridColor( iGrid, 12, 1, YELLOW_L, BLACK_C );	
		GridData( iGrid, 13, 1, "R S2 End X" );				GridColor( iGrid, 13, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 14, 1, "R S2 End Y" );				GridColor( iGrid, 14, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 15, 1, "R S2 End Z" );				GridColor( iGrid, 15, 1, ORANGE_L, BLACK_C );
		GridData( iGrid, 16, 1, "R COK Start X" );			GridColor( iGrid, 16, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 17, 1, "R COK Start Y" );			GridColor( iGrid, 17, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 18, 1, "R COK Start Z" );			GridColor( iGrid, 18, 1, BLACK_C, WHITE_C );
		GridData( iGrid, 19, 1, "R COK End X" );				GridColor( iGrid, 19, 1, BLACK_L, WHITE_C );
		GridData( iGrid, 20, 1, "R COK End Y" );				GridColor( iGrid, 20, 1, BLACK_L, WHITE_C );
		GridData( iGrid, 21, 1, "R COK End Z" );				GridColor( iGrid, 21, 1, BLACK_L, WHITE_C );
		
		GridData( iGrid, 22, 1, "COK Y Start (1)" );			GridColor( iGrid, 22, 1, BLUE_A, WHITE_C );
		GridData( iGrid, 23, 1, "COK Y End (6)" );			GridColor( iGrid, 23, 1, BLUE_A, WHITE_C );
		GridData( iGrid, 24, 1, "COK Z Safety" );				GridColor( iGrid, 24, 1, BLUE_D, WHITE_C );
		GridData( iGrid, 25, 1, "COK Z 0" );				GridColor( iGrid, 25, 1, BLUE_D, WHITE_C );
		GridData( iGrid, 26, 1, "COK Z 19" );				GridColor( iGrid, 26, 1, BLUE_D, WHITE_C );
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Display_COK_Teaching()
{
	for( int i=0; i<COMMON_TEACHING_MAX; i++ )
	{
		CString strVal;
		strVal.Format( "%.03f", st_handler.md_common_teaching[i] );
		GridData( IDC_CUSTOM_COK_TEACHING, i + 1, 2, strVal );
	}
}

void CScreen_Admin::OnBtnGetCommonTeaching() 
{
	st_handler.md_common_teaching[CT_TSITE_1_Z] = st_motor[MOTOR_TESTSITE_1_Z].d_pos[P_SITE_COK_CHANGE];
	st_handler.md_common_teaching[CT_TSITE_2_Z] = st_motor[MOTOR_TESTSITE_2_Z].d_pos[P_SITE_COK_CHANGE];

	st_handler.md_common_teaching[CT_ROBOT_PLACE_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START]
		 - ( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PLACE_START] );

	st_handler.md_common_teaching[CT_ROBOT_SITE1_START_X] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START];
	st_handler.md_common_teaching[CT_ROBOT_SITE1_START_Y] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START];
	st_handler.md_common_teaching[CT_ROBOT_SITE1_START_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE1_END_X] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START]
		+ st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] - st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE1_END_Y] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START]
		+ st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] - st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE1_END_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START]
		+ st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_START_X] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START]
		+ st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_START_Y] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START]
		+ st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START] - st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_START_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START]
		+ st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_START] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_END_X] = st_handler.md_common_teaching[CT_ROBOT_SITE2_START_X]
		+ st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] - st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_END_Y] = st_handler.md_common_teaching[CT_ROBOT_SITE2_START_Y]
		+ st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] - st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START];

	st_handler.md_common_teaching[CT_ROBOT_SITE2_END_Z] = st_handler.md_common_teaching[CT_ROBOT_SITE2_START_Z]
		+ st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_END] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_START];

	st_handler.md_common_teaching[CT_ROBOT_COK_START_X] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_START];
	st_handler.md_common_teaching[CT_ROBOT_COK_START_Y] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_START];
	st_handler.md_common_teaching[CT_ROBOT_COK_START_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_START];

	st_handler.md_common_teaching[CT_ROBOT_COK_END_X] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_END];
	st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END];
	st_handler.md_common_teaching[CT_ROBOT_COK_END_Z] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_END];

	st_handler.md_common_teaching[CT_COK_Y_START] = st_motor[MOTOR_COK_ROBOT_Y].d_pos[P_CY_COK_0];
	st_handler.md_common_teaching[CT_COK_Y_END] = st_motor[MOTOR_COK_ROBOT_Y].d_pos[P_CY_COK_5];

	st_handler.md_common_teaching[CT_COK_Z_SAFETY] = st_motor[MOTOR_COK_ROBOT_Z].d_pos[P_CZ_SAFETY];
	st_handler.md_common_teaching[CT_COK_Z_0] = st_motor[MOTOR_COK_ROBOT_Z].d_pos[P_CZ_COK_0];
	st_handler.md_common_teaching[CT_COK_Z_19] = st_motor[MOTOR_COK_ROBOT_Z].d_pos[P_CZ_COK_19];


	for( int i=0; i<COMMON_TEACHING_MAX; i++ )
	{
		CString strKey;
		strKey.Format( "COMMON_TEACHING_%02d", i );
		CString strTemp;
		strTemp.Format( "%.03f", st_handler.md_common_teaching[i] );
		::WritePrivateProfileString( "BasicData", strKey, LPCTSTR(strTemp), st_path.mstr_basic );	
	}
	
	OnAdmin_Display_COK_Teaching();
}

void CScreen_Admin::OnAdmin_Init_CokIndex()
{
	TSpread* Grid;
	int row = 5;
	int col = 2;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_COK_INDEX );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 80 );
	Grid -> SetColWidthInPixels(2, 20 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 20 );

		for( int ih=0; ih<col; ih++ )
		{
			GridFont( IDC_CUSTOM_COK_INDEX, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_COK_INDEX, STATIC, iw + 1, ih + 1, 0);
		}
	}

	GridData( IDC_CUSTOM_COK_INDEX, 1, 1, "GS1" );
	GridData( IDC_CUSTOM_COK_INDEX, 2, 1, "GS2" );
	GridData( IDC_CUSTOM_COK_INDEX, 3, 1, "mSATA" );
	GridData( IDC_CUSTOM_COK_INDEX, 4, 1, "2.5 Reverse" );
	GridData( IDC_CUSTOM_COK_INDEX, 5, 1, "2.5 Front" );
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Admin::OnAdmin_Display_CokIndex()
{
 	CString strGS1, strGS2, strMSATA, str25Rev, str25Front;
 	strGS1.Format( "%d", st_handler.mn_cokIndex[COKINDEX_GS1] + 1 );
	strGS2.Format( "%d", st_handler.mn_cokIndex[COKINDEX_GS2] + 1);
	strMSATA.Format( "%d", st_handler.mn_cokIndex[COKINDEX_MSATA] + 1);
	str25Rev.Format( "%d", st_handler.mn_cokIndex[COKINDEX_25_REVERSE] + 1 );
	str25Front.Format( "%d", st_handler.mn_cokIndex[COKINDEX_25_FRONT] + 1 );

 	GridData( IDC_CUSTOM_COK_INDEX, 1, 2, strGS1 );
	GridData( IDC_CUSTOM_COK_INDEX, 2, 2, strGS2 );
	GridData( IDC_CUSTOM_COK_INDEX, 3, 2, strMSATA );
	GridData( IDC_CUSTOM_COK_INDEX, 4, 2, str25Rev );
	GridData( IDC_CUSTOM_COK_INDEX, 5, 2, str25Front );
}

void CScreen_Admin::OnBufferInChk() 
{
	CString mstr_temp;
	
	st_handler.mn_buffer_in = !st_handler.mn_buffer_in;
	
	m_chk_bufferin_chk.SetCheck(st_handler.mn_buffer_in);
	
	mstr_temp.Format("%d", st_handler.mn_buffer_in);
	:: WritePrivateProfileString("BasicData", "Buffer_In", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
}

void CScreen_Admin::OnBtnPgminfo() 
{
	CDialog_Pass_Check pass_dlg;
	int n_response;
	
	CMainFrame* pMainFrm = (CMainFrame*)::AfxGetMainWnd();
	int nmenu_chk = pMainFrm->OnMenu_Change_Checking(); // 메뉴 사용 가능 여부 검사 함수
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// 메뉴 사용 불가능한 경우에 대한 조건 검사한다                              
	// -> 메인트 모드 또는 티칭 모드가 설정되어 있는지 검사                      
	// **************************************************************************
	st_handler.mstr_pass_level = "Master";
	
	n_response = pass_dlg.DoModal();
	
	if (n_response != IDOK)
	{
		return;
	}
	

	if(pMainFrm->GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Pgm_Info)))   return;
	
	pMainFrm->OnSwitchToForm(IDW_SCREEN_PGM_INFO);		
}

void CScreen_Admin::OnRadioMirrorUse() 
{
	// TODO: Add your control notification handler code here
	m_radio_mirror_use.SetCheck( true );
	m_radio_mirror_notuse.SetCheck( false );

	st_handler.mn_mirror_type = CTL_YES;
	
	:: WritePrivateProfileString("BasicData", "Mirror_Type", "1", st_path.mstr_basic);
	
}

void CScreen_Admin::OnRadioMirrorNotuse() 
{
	m_radio_mirror_use.SetCheck( false );
	m_radio_mirror_notuse.SetCheck( true );
	
	st_handler.mn_mirror_type = CTL_NO;
	:: WritePrivateProfileString("BasicData", "Mirror_Type", "0", st_path.mstr_basic);
	
}

void CScreen_Admin::OnRadioPogoUse() 
{
	// TODO: Add your control notification handler code here
	m_radio_pogo_use.SetCheck( true );
	m_radio_pogo_notuse.SetCheck( false );
	
	st_handler.mn_pogo_type = CTL_YES;
	:: WritePrivateProfileString("BasicData", "Pogo_Type", "1", st_path.mstr_basic);

	CDialog_Infor info_dlg;
	st_msg.mstr_confirm_msg = _T("Pogo 타입이 변경 되었을 경우 반드시 초기화 해 주세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("You must initialize in case of Pogo type changed");
	info_dlg.DoModal();
	
}

void CScreen_Admin::OnRadioPogoNotuse() 
{
	m_radio_pogo_use.SetCheck( false );
	m_radio_pogo_notuse.SetCheck( true );
	
	st_handler.mn_pogo_type = CTL_NO;
	:: WritePrivateProfileString("BasicData", "Pogo_Type", "0", st_path.mstr_basic);

	CDialog_Infor info_dlg;
	st_msg.mstr_confirm_msg = _T("Pogo 타입이 변경 되었을 경우 반드시 초기화 해 주세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("You must initialize in case of Pogo type changed");
	info_dlg.DoModal();
	
}

void CScreen_Admin::OnRadioAlignUse() 
{
	CString str_save_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // 티칭 데이터 저장 파일 설정

	m_radio_alignuse.SetCheck( true );
	m_radio_alignnotuse.SetCheck( false );
	
	st_basic.mn_alignBuffer_type = CTL_YES;
	:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", "1", str_save_file);

	CDialog_Infor info_dlg;
	st_msg.mstr_confirm_msg = _T("ALIGN BUFFER 타입이 변경 되었을 경우 반드시 초기화 해 주세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("You must initialize in case of ALIGN BUFFER changed");
	info_dlg.DoModal();
	
}

void CScreen_Admin::OnRadioAlignNotuse() 
{
	CString str_save_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // 티칭 데이터 저장 파일 설정

	m_radio_alignuse.SetCheck( false );
	m_radio_alignnotuse.SetCheck( true );
	
	st_basic.mn_alignBuffer_type = CTL_NO;
	:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", "1", str_save_file);
	
	CDialog_Infor info_dlg;
	st_msg.mstr_confirm_msg = _T("ALIGN BUFFER 타입이 변경 되었을 경우 반드시 초기화 해 주세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("You must initialize in case of ALIGN BUFFER changed");
	info_dlg.DoModal();
	
}

void CScreen_Admin::OnBtnDelfrontBuffer() 
{
	int n_response = IDCANCEL;
	
	CDialog_Select select_dlg;
	st_msg.mstr_confirm_msg = _T("Front장비에서 오는 트레이를 강제로 제거하시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Do you want to remove front buffer by force");
	
	n_response = select_dlg.DoModal();
	if (n_response == IDCANCEL)
		return ; 
	
	g_server_front.SetBufferIn(NR_NONE);
	Run_Bin_Stacker[0].SetRecvTrayNone(RECV_TRAY_NONE);
	if (st_handler.cwnd_list != NULL)
	{
		sprintf(st_msg.c_normal_msg, "[ADMIN]Front Buffer Error! 강제 제거");
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}
	st_handler.m_nfront_request = CTL_REQ;//2014,0731
}

void CScreen_Admin::OnBtnDelrearBuffer() 
{
	int n_response = IDCANCEL;
	
	CDialog_Select select_dlg;
	st_msg.mstr_confirm_msg = _T("Rear장비로 보내는 트레이를 강제로 제거하시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Do you want to remove Rear buffer by force");
	
	n_response = select_dlg.DoModal();
	if (n_response == IDCANCEL)
		return ; 
	
	int nSiteID = 2;
	g_client_next.ResetBufferIn();
	
	if (st_handler.cwnd_list != NULL)
	{
		sprintf(st_msg.c_normal_msg, "[ADMIN]Rear Buffer Error! 강제 제거");
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}
// 	g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV1_TO_CONV2 + nSiteID), ETRAY_POS(TPOS_CONV1 + nSiteID + 1) );
// 	
// 	Run_Bin_Stacker[2].SetTraySendNone(TRAY_SEND_NONE);	
	
}

void CScreen_Admin::OnBtnTestMode() 
{
	int n_response = IDCANCEL;
	
	CDialog_Select select_dlg;
	st_msg.mstr_confirm_msg = _T("테스터의 Hot FIx 변환 모드를 클리어 하시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Do you want to clear change mode of Tester");
	
	n_response = select_dlg.DoModal();
	if (n_response == IDCANCEL)
		return ; 
	
	g_client_bpc.CleaeTestMode();
	
	if (st_handler.cwnd_list != NULL)
	{
		sprintf(st_msg.c_normal_msg, "[ADMIN]테스터의 Hot FIx 변환 모드를 클리어 실행");
		if ( g_local.GetLocalType() == LOCAL_ENG )	sprintf(st_msg.c_normal_msg, "[ADMIN] Run Clear Change mode of Hot Fix");
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}	
}

void CScreen_Admin::OnBtnSendTestMode() 
{
	int n_response = IDCANCEL;

	CDialog_Select select_dlg;
	st_msg.mstr_confirm_msg = _T("테스터의 Hot FIx 변환 모드를 전환하시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Do you want to change Hot Fix mode?");

	n_response = select_dlg.DoModal();
	if (n_response == IDCANCEL)
		return ; 

	if(st_basic.mn_hotfix_pos <= 0 || st_basic.mn_hotfix_pos >= 6)
	{
		st_msg.str_fallacy_msg = _T("Basica메뉴에서 HitFix 세팅값이 0이거나 6보다 큽니다.");
		if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("HitFix is 0 or more than 6 in Basica menu.");

		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[ADMIN]Basica메뉴에서 HitFix 세팅값이 0이거나 6보다 큽니다.");
			if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("HitFix is 0 or more than 6 in Basica menu.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
		}	
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	g_client_bpc.SetTestMode( "TestMode_HotFix" );//2012,0430
	
}

void CScreen_Admin::OnButton7() 
{
	g_client_front.ClearClient();
	g_client_front.SetUsable( true );
	g_client_front.PushSendMsg("Front Server Test");
	
}

void CScreen_Admin::OnButton8() 
{
	g_client_next.ClearClient();
	g_client_next.PushSendMsg( "NEXT Client Test" );
	g_client_next.SetUsable( true );
	
}

void CScreen_Admin::OnBtnVersion() 
{
	int n_response;
	CDialog_Message dlg;

	st_msg.str_fallacy_msg = _T("Version을 업데이트 하시겠습니까?.");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Would you like to update Version?.");//20130930
	n_response = dlg.DoModal();

	if(n_response == IDOK)
	{
		g_ver.SetVersion();	
	}
}

void CScreen_Admin::OnFullInlineChk() 
{
	CString mstr_temp;
	
	st_handler.mn_fullinline_mode = !st_handler.mn_fullinline_mode;
	
	m_chk_fullinline.SetCheck(st_handler.mn_fullinline_mode);
	
	mstr_temp.Format("%d", st_handler.mn_fullinline_mode);
	:: WritePrivateProfileString("BasicData", "FullInline_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
}

void CScreen_Admin::OnBTNTSTXGem() 
{
	// TODO: Add your control notification handler code here
	g_client_xgem.PushSendMsg( "XGem Client Test" );
}

void CScreen_Admin::OnBTNCIPSETXGem() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}

	BYTE nField[4];
	m_cip_xgem.GetAddress( nField[0], nField[1], nField[2], nField[3] );
	
	CString strip;
	strip.Format( "%d.%d.%d.%d", nField[0], nField[1], nField[2], nField[3] );

	st_msg.mstr_keypad_msg = "XGem IP를 입력하세요.";
	if (g_local.GetLocalType() != LOCAL_KOR)	st_msg.mstr_keypad_msg = _T("Enter the IP XGem");

	if (strip.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();  // 키보드 대화 상자에 출력할 정보 저장 변수 초기화 
	}
	else 
	{
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strip;
	}

	CDialog_Keyboard keyboard_dlg;
	int n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strip = st_msg.mstr_typing_msg;
		
		strip.MakeUpper();
		strip.TrimLeft(' ');               
		strip.TrimRight(' ');

		
		sscanf( (LPCSTR)strip, "%d.%d.%d.%d", &nField[0], &nField[1], &nField[2], &nField[3] );
		
		m_cip_xgem.SetAddress(nField[0], nField[1], nField[2], nField[3]);

		g_client_xgem.SetIP( strip );
		:: WritePrivateProfileString("ADMIN", "CIP_XGEM", LPCTSTR(strip), st_path.mstr_basic);
	}	
}

void CScreen_Admin::OnBTNCPORTSETXGem() 
{
	if( st_handler.mn_level_teach != TRUE )
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if( g_local.GetLocalType() != LOCAL_KOR )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		CDialog_Message msg_dlg;
		msg_dlg.DoModal();
		return;
	}
	
	CString strport;
	((CEdit*)GetDlgItem(IDC_EDIT_CPORT_XGem))->GetWindowText(strport);
	strport.TrimLeft(' ');	
	strport.TrimRight(' ');
	
	st_msg.mstr_keypad_msg = _T("XGem port Setting");
	
	st_msg.mstr_keypad_val = strport;
	
	st_msg.mstr_pad_high_limit = "99999";
	st_msg.mstr_pad_low_limit = "0";
	
	st_msg.mn_dot_use = FALSE;
	
	CRect r;
	
	m_btn_cport_set_xgem.GetWindowRect(&r);
	
	CDialog_KeyPad pad_dlg;
	pad_dlg.m_ptRef = CPoint(r.right, r.top);
	
	if (pad_dlg.DoModal() == IDOK)
	{
		strport = st_msg.mstr_keypad_val;	
		((CEdit*)GetDlgItem(IDC_EDIT_CPORT_XGem))->SetWindowText(strport);

		g_client_xgem.SetPort( atoi(strport) );
		:: WritePrivateProfileString("ADMIN", "CPORT_XGEM", LPCTSTR(strport), st_path.mstr_basic);
	}	
}

void CScreen_Admin::OnBtnReqEmpty() 
{
	// TODO: Add your control notification handler code here
	int n_response;
	CDialog_Message dlg;
	
	st_msg.str_fallacy_msg = _T("Empty 트레이를 요청하시겠습니까?.");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Would you like to request Empty Tray?.");//20130930
	n_response = dlg.DoModal();
	
	if(n_response == IDOK)
	{
		g_client_ec.OnReqEmptyTray(true);	
	}		
}

void CScreen_Admin::OnBtnSendTool() 
{
	// TODO: Add your control notification handler code here
	int n_response;
	CDialog_Message dlg;
	
	// 	st_lamp.mstr_tool_id = "IF-25INCH00000";
	
	st_msg.str_fallacy_msg = _T("Tool 정보를 전송하시겠습니까?.");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Would you like to send tool information?.");//20130930
	n_response = dlg.DoModal();
	
	if(n_response == IDOK)
	{
		g_client_ec.OnSendToolInform(true);	
	}		
}

void CScreen_Admin::OnBtnEmptyCancle() 
{
	// TODO: Add your control notification handler code here
	int n_response;
	CDialog_Message dlg;
	
	
	
	st_msg.str_fallacy_msg = _T("Empty 트레이를 요청을 취소 하시겠습니까?.");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Would you like to cancle Empty Tray?.");//20130930
	n_response = dlg.DoModal();
	
	if(n_response == IDOK)
	{
		g_client_ec.OnReqEmptyTray(false);
		g_handler.ClearReqEmpty();
	}	
}

void CScreen_Admin::OnBtnToolCancle() 
{
	// TODO: Add your control notification handler code here
	int n_response;
	CDialog_Message dlg;
	
	st_msg.str_fallacy_msg = _T("Tool 정보를 전송 취소 하시겠습니까?.");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Would you like to cancle tool information?.");//20130930
	n_response = dlg.DoModal();
	
	if(n_response == IDOK)
	{
		g_client_ec.OnSendToolInform(false);	
	}		
}

////2015.0604
void CScreen_Admin::OnDgtRetestCnt2() 
{
	// TODO: Add your control notification handler code here
	char chr_buf[20] ;
	
	int mn_count = m_dgt_multi_retest_cnt.GetValue();
	CString	mstr_temp = LPCTSTR(_itoa(mn_count, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("MULTI RETEST MAX");
	st_msg.mstr_keypad_val = mstr_temp;
	mstr_temp = KeyPad.GetNumEditString_I(0, 9, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);
	
	st_handler.mn_multi_retest_cnt = atoi( mstr_temp );
	m_dgt_multi_retest_cnt.SetValue( st_handler.mn_multi_retest_cnt );
	
	mstr_temp.Format("%d", st_handler.mn_multi_retest_cnt );
	:: WritePrivateProfileString("BasicData", "MULTI_RETEST_CNT", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
}
void CScreen_Admin::OnRadioVersionModeUse() 
{
	// TODO: Add your control notification handler code here
	m_radio_version_mode_use.SetCheck( true );
	m_radio_version_mode_notuse.SetCheck( false );
	
	st_handler.mn_version_mode = CTL_YES;
	
	:: WritePrivateProfileString("BasicData", "Version_Mode", "1", st_path.mstr_basic);
	
}
void CScreen_Admin::OnRadioVersionModeNotuse() 
{
	// TODO: Add your control notification handler code here
	m_radio_version_mode_use.SetCheck( false );
	m_radio_version_mode_notuse.SetCheck( true );
	
	st_handler.mn_version_mode = CTL_NO;
	
	:: WritePrivateProfileString("BasicData", "Version_Mode", "0", st_path.mstr_basic);
	
}
////

//2017.0602
void CScreen_Admin::OnRadioXgemNew() 
{
	m_radio_xgem_new.SetCheck( true );
	m_radio_xgem_old.SetCheck( false );
	
	st_handler.mn_xgem_mode = CTL_YES;
	
	:: WritePrivateProfileString("BasicData", "XGEM_Mode", "1", st_path.mstr_basic);


}

void CScreen_Admin::OnRadioXgemOld() 
{
	m_radio_xgem_new.SetCheck( false );
	m_radio_xgem_old.SetCheck( true );
	
	st_handler.mn_xgem_mode = CTL_YES;
	
	:: WritePrivateProfileString("BasicData", "XGEM_Mode", "0", st_path.mstr_basic);
	
}



void CScreen_Admin::OnBtnSaveFile() 
{
	int n_response;  // 대화 상자 리턴 플래그
	
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("basic, teach 및 system 데이이타 저장 형식을 바꿉니다. 모르면 cancle하세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	
		st_msg.mstr_confirm_msg = _T("We are going to change basic, teach and system data save type. If you don't know, press cancle.");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		st_basic.n_file_save = CTL_YES;
		mcls_basic.OnBasic_Data_Save();				// 전역 변수에 저장된 Basic 셋팅 정보를 파일에 저장하는 함수
		mcls_basic.OnWaitTime_Data_Save();			// 전역 변수에 저장된 Wait Time 셋팅 정보를 파일에 저장하는 함수
		mcls_basic.OnMotorSpeed_Set_Data_Save();		// 전역 변수에 저장된 Motor Speed 셋팅 정보를 파일에 저장하는 함수.
		mcls_basic.On_Teach_Data_Save();
		mcls_basic.OnMaintenance_Data_Save();

	}	
}

void CScreen_Admin::OnBtnLoadFile() 
{
	int n_response;  // 대화 상자 리턴 플래그
	
	CDialog_Select  select_dlg;
	st_msg.mstr_confirm_msg = _T("basic, teach 및 system 데이이타 로드 형식을 바꿉니다. 모르면 cancle하세요");
	if ( g_local.GetLocalType() == LOCAL_ENG )	
		st_msg.mstr_confirm_msg = _T("We are going to change basic, teach and system data load type. If you don't know, press cancle.");	
	
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		mcls_basic.OnBasic_Data_Load();				// 전역 변수에 저장된 Basic 셋팅 정보를 파일에 저장하는 함수
		mcls_basic.OnWaitTime_Data_Load();			// 전역 변수에 저장된 Wait Time 셋팅 정보를 파일에 저장하는 함수
		mcls_basic.OnMotorSpeed_Set_Data_Load();		// 전역 변수에 저장된 Motor Speed 셋팅 정보를 파일에 저장하는 함수.
		mcls_basic.On_Teach_Data_Load();
		mcls_basic.OnMaintenance_Data_Load();
		
	}	
	
}

void CScreen_Admin::OnBtnLoadSok() 
{
	int n_response;  // 대화 상자 리턴 플래그
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("저장된 SOCKET 정보를 파일에서 읽어 오시겠습니까?(SOCKETSKIP_RULE file)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	
		st_msg.mstr_confirm_msg = _T("Do you want to get socket information from file(SOCKETSKIP_RULE)?.");
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		Func.LoadSocketSkipRule();
	}
}

void CScreen_Admin::OnBtnSaveSok() 
{
	int n_response;  // 대화 상자 리턴 플래그
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("현재 SOCKET 정보를 파일로 저장하시겠습니까?(SOCKETSKIP_RULE file)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	
		st_msg.mstr_confirm_msg = _T("Do you want to save socket information on file(SOCKETSKIP_RULE)?.");
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		Func.SaveSocketSkipRule();
	}	
}

