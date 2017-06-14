// Screen_Basic.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Screen_Basic.h"
#include "AMTLotManager.h"
#include "Srcbase\ALocalization.h"
#include "SrcPart\DataManager.h"
#include "Run_Network.h"
#include "SrcPart/APartHandler_COK.h"
#include "SrcPart/APartHandler.h"
#include "Dialog_AlignPogo.h"
#include "AClient_BPC.h"

#include "io.h"  // 파일 존재 유무 검사 함수 호출하기 위해서는 반드시 INCLUDE 필요

// ******************************************************************************
// 대화 상자 클래스 추가
// ******************************************************************************
#include "Dialog_Select.h"
#include "Dialog_Message.h"
#include "Dialog_KeyBoard.h"

#include "Dialog_KeyPad.h"
#include "AMTRegistry.h"
// ******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Basic

IMPLEMENT_DYNCREATE(CScreen_Basic, CFormView)

CScreen_Basic::CScreen_Basic()
	: CFormView(CScreen_Basic::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Basic)
	//}}AFX_DATA_INIT
}

CScreen_Basic::~CScreen_Basic()
{
}

void CScreen_Basic::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Basic)
	DDX_Control(pDX, IDC_RADIO_AUTO_TIMEOUT, m_radio_auto_timeout);
	DDX_Control(pDX, IDC_RADIO_CSERIAL2_NOTUSE, m_radio_cserial2_notuse);
	DDX_Control(pDX, IDC_GROUP_CSERIAL_MODE2, m_group_cserial2);
	DDX_Control(pDX, IDC_RADIO_RECIPE_NOTUSE, m_radio_recipe_notuse);
	DDX_Control(pDX, IDC_RADIO_RECIPE_USE, m_radio_recipe_use);
	DDX_Control(pDX, IDC_GROUP_RECIPE_MODE, m_group_recipe_mode);
	DDX_Control(pDX, IDC_RADIO_BOTH2, m_radio_site2_both);
	DDX_Control(pDX, IDC_RADIO_BOTH, m_radio_site1_both);
	DDX_Control(pDX, IDC_RADIO_22, m_radio_site2_2);
	DDX_Control(pDX, IDC_RADIO_21, m_radio_site2_1);
	DDX_Control(pDX, IDC_RADIO_2, m_radio_site1_2);
	DDX_Control(pDX, IDC_RADIO_1, m_radio_site1_1);
	DDX_Control(pDX, IDC_GROUP_SITE2, m_group_sie2);
	DDX_Control(pDX, IDC_GROUP_SITE1, m_group_sie1);
	DDX_Control(pDX, IDC_GROUP_PRODUCT_SITE2, m_group_product_site2);
	DDX_Control(pDX, IDC_GROUP_PRODUCT_SITE1, m_group_product_site1);
	DDX_Control(pDX, IDC_GROUP_PRODUCT, m_group_product);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_SITE2, m_radio_site2_notuse);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_SITE1, m_radio_site1_notuse);
	DDX_Control(pDX, IDC_RADIO_USE_SITE2, m_radio_site2_use);
	DDX_Control(pDX, IDC_RADIO_USE_SITE1, m_radio_site1_use);
	DDX_Control(pDX, IDC_RADIO_USE_PRODUCT, m_radio_product_use);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_PRODUCT, m_radio_product_notuse);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_RCMD, m_radio_notuse_rcmd);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_XGEM, m_radio_notuse_xgem);
	DDX_Control(pDX, IDC_COMBO_ADD_RECIPE, m_combo_recipe);
	DDX_Control(pDX, IDC_RADIO_USE_XGEM, m_radio_use_xgem);
	DDX_Control(pDX, IDC_RADIO_USE_RCMD, m_radio_use_rcmd);
	DDX_Control(pDX, IDC_GROUP_SECS_RCMD, m_broup_rcmd_mode);
	DDX_Control(pDX, IDC_GROUP_SECS_GEM, m_group_xgem_mode);
	DDX_Control(pDX, IDC_RADIO_PT_1V1M_USE4_2, m_radio_pt_1v1m_4use_2);
	DDX_Control(pDX, IDC_GROUP_SOCKET_MAX_LIMIT, m_group_socket_max_count);
	DDX_Control(pDX, IDC_GROUP_HOTFIX, m_group_hotfix_pos);
	DDX_Control(pDX, IDC_GROUP_HIFIX_MODE, m_group_hifix_mode);
	DDX_Control(pDX, IDC_COMBO_HIFIX_MODE, m_cbohifix_mode);
	DDX_Control(pDX, IDC_RADIO_RECOVERY_NOTUSE, m_radio_recovery_notuse);
	DDX_Control(pDX, IDC_RADIO_RECOVERY_MODE, m_radio_recovery_use);
	DDX_Control(pDX, IDC_GROUP_RECOVERY_MODE, m_group_recovery_mode);
	DDX_Control(pDX, IDC_RADIO_JIG_NOTUSE, m_radio_jig_notuse);
	DDX_Control(pDX, IDC_RADIO_JIG_MODE, m_radio_jig_use);
	DDX_Control(pDX, IDC_GROUP_JIG_MODE, m_group_jig_mode);
	DDX_Control(pDX, IDC_GROUP_MANUAL_SPEED, m_group_basic_manual_speed);
	DDX_Control(pDX, IDC_GROUP_RUN_SPEED, m_group_basic_run_speed);
	DDX_Control(pDX, IDC_GROUP_SPEED, m_group_basic_speed);
	DDX_Control(pDX, IDC_GROUP_ROBOT_INIT, m_group_robot_init);
	DDX_Control(pDX, IDC_RADIO_POGO_NOTUSE, m_radio_pogo_notuse);
	DDX_Control(pDX, IDC_RADIO_POGO_MODE, m_radio_pogo_mode);
	DDX_Control(pDX, IDC_GROUP_POGO_MODE, m_group_pogo_mode);
	DDX_Control(pDX, IDC_GROUP_EMPTY_TRAY, m_group_empty_tray);
	DDX_Control(pDX, IDC_RADIO_BYPASS_NOTUSE, m_radio_bypass_notuse);
	DDX_Control(pDX, IDC_RADIO_BYPASS_REAR, m_radio_bypass_rear);
	DDX_Control(pDX, IDC_RADIO_BYPASS_FRONT, m_radio_bypass_front);
	DDX_Control(pDX, IDC_RADIO_BYPASS_ALL, m_radio_bypass_all);
	DDX_Control(pDX, IDC_GROUP_TIME_OUT, m_group_time_out);
	DDX_Control(pDX, IDC_RADIO_ONLINE_MODE, m_radio_online_mode);
	DDX_Control(pDX, IDC_RADIO_OFFLINE_MODE, m_radio_offline_mode);
	DDX_Control(pDX, IDC_GROUP_ONLINE_MODE, m_group_online_mode);
	DDX_Control(pDX, IDC_RADIO_PT_2V1M_USE2, m_radio_pt_2v1m_2use);
	DDX_Control(pDX, IDC_RADIO_PT_1V1M_USE2, m_radio_pt_1v1m_2use);
	DDX_Control(pDX, IDC_RADIO_PT_1V1M_USE4, m_radio_pt_1v1m_4use);
	DDX_Control(pDX, IDC_GROUP_PICK_TYPE, m_group_pick_type);
	DDX_Control(pDX, IDC_RADIO_DEGREE_180, m_radio_degree_180);
	DDX_Control(pDX, IDC_RADIO_DEGREE_270, m_radio_degree_270);
	DDX_Control(pDX, IDC_RADIO_DEGREE_90, m_radio_degree_90);
	DDX_Control(pDX, IDC_RADIO_DEGREE_0, m_radio_degree_0);
	DDX_Control(pDX, IDC_GROUP_DEGREE, m_group_degree);
	DDX_Control(pDX, IDC_RADIO_COK_5, m_radio_cok_5);
	DDX_Control(pDX, IDC_RADIO_COK_4, m_radio_cok_4);
	DDX_Control(pDX, IDC_RADIO_COK_3, m_radio_cok_3);
	DDX_Control(pDX, IDC_RADIO_COK_2, m_radio_cok_2);
	DDX_Control(pDX, IDC_RADIO_COK_1, m_radio_cok_1);
	DDX_Control(pDX, IDC_RADIO_COK_0, m_radio_cok_0);
	DDX_Control(pDX, IDC_GROUP_COK_TYPE, m_group_cok_type);
	DDX_Control(pDX, IDC_GROUP_RECIPE_SELECT, m_group_recipe_select);
	DDX_Control(pDX, IDC_MSG_TRAY_Y, m_msg_tray_y);
	DDX_Control(pDX, IDC_MSG_TRAY_X, m_msg_tray_x);
	DDX_Control(pDX, IDC_GROUP_TRAY_SIZE_SETTING, m_group_tray_size_setting);
	DDX_Control(pDX, IDC_GROUP_ALARM_DELAY_TIME, m_group_alarm_delay_time);
	DDX_Control(pDX, IDC_GROUP_NETWORK_WAIT_TIME, m_group_network_wait_time);
	DDX_Control(pDX, IDC_GROUP_NETWORK_RETRY_COUNT, m_group_network_retry_count);
	DDX_Control(pDX, IDC_LABEL_ELECTROSTATIC_5, m_label_electrostatic_5);
	DDX_Control(pDX, IDC_LABEL_ELECTROSTATIC_4, m_label_electrostatic_4);
	DDX_Control(pDX, IDC_LABEL_ELECTROSTATIC_3, m_label_electrostatic_3);
	DDX_Control(pDX, IDC_LABEL_ELECTROSTATIC_2, m_label_electrostatic_2);
	DDX_Control(pDX, IDC_LABEL_ELECTROSTATIC_1, m_label_electrostatic_1);
	DDX_Control(pDX, IDC_MSG_ELECTROSTATIC_5, m_msg_electrostatic_5);
	DDX_Control(pDX, IDC_MSG_ELECTROSTATIC_4, m_msg_electrostatic_4);
	DDX_Control(pDX, IDC_MSG_ELECTROSTATIC_3, m_msg_electrostatic_3);
	DDX_Control(pDX, IDC_MSG_ELECTROSTATIC_2, m_msg_electrostatic_2);
	DDX_Control(pDX, IDC_MSG_ELECTROSTATIC_1, m_msg_electrostatic_1);
	DDX_Control(pDX, IDC_RADIO_DEVICE_MODE_USE, m_radio_device_mode_use);
	DDX_Control(pDX, IDC_RADIO_DEVICE_MODE_NOTUSE, m_radio_device_mode_notuse);
	DDX_Control(pDX, IDC_DGT_ELECTROSTATIC_CHECK_VALUE, m_dgt_electrostatic_check_value);
	DDX_Control(pDX, IDC_LIST_DEVICE_TYPE, m_list_device_type);
	DDX_Control(pDX, IDC_GROUP_ELECTROSTATIC_TEST, m_group_electrostatic_test);
	DDX_Control(pDX, IDC_GROUP_ELECTROSTATIC_CHECK_VALUE, m_group_electrostatic_check_value);
	DDX_Control(pDX, IDC_GROUP_ELECTROSTATIC_CHECK_GAP, m_group_electrostatic_check_gap);
	DDX_Control(pDX, IDC_GROUP_DEVICE_MODE, m_group_device_mode);
	DDX_Control(pDX, IDC_BTN_ELECTROSTATIC_TEST, m_btn_electostatic_test);
	DDX_Control(pDX, IDC_BTN_DVC_DELETE, m_btn_dvc_delete);
	DDX_Control(pDX, IDC_BTN_DVC_CREATE, m_btn_dvc_create);
	DDX_Control(pDX, IDC_BTN_BASIC_CANCLE, m_btn_basic_cancel);
	DDX_Control(pDX, IDC_BTN_BASIC_APPLY, m_btn_basic_apply);
	DDX_Control(pDX, IDC_RADIO_RETEST_JIG_NOTUSE, m_radio_retest_jig_notuse);
	DDX_Control(pDX, IDC_RADIO_RETEST_JIG_USE, m_radio_retest_jig_use);
	DDX_Control(pDX, IDC_GROUP_RETEST_JIG_MODE, m_group_retest_jig_mode);
	DDX_Control(pDX, IDC_RADIO_CSERIAL2_USE, m_radio_cserial2_use);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreen_Basic, CFormView)
	//{{AFX_MSG_MAP(CScreen_Basic)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_DEVICE_MODE_USE, OnRadioDeviceModeUse)
	ON_BN_CLICKED(IDC_RADIO_DEVICE_MODE_NOTUSE, OnRadioDeviceModeNotuse)
	ON_BN_CLICKED(IDC_DGT_ELECTROSTATIC_CHECK_GAP, OnDgtElectrostaticCheckGap)
	ON_BN_CLICKED(IDC_DGT_ELECTROSTATIC_CHECK_VALUE, OnDgtElectrostaticCheckValue)
	ON_BN_CLICKED(IDC_BTN_DVC_CREATE, OnBtnDvcCreate)
	ON_LBN_SELCHANGE(IDC_LIST_DEVICE_TYPE, OnSelchangeListDeviceType)
	ON_BN_CLICKED(IDC_BTN_DVC_DELETE, OnBtnDvcDelete)
	ON_BN_CLICKED(IDC_BTN_ELECTROSTATIC_TEST, OnBtnElectrostaticTest)
	ON_BN_CLICKED(IDC_BTN_BASIC_APPLY, OnBtnBasicApply)
	ON_BN_CLICKED(IDC_BTN_BASIC_CANCLE, OnBtnBasicCancle)
	ON_BN_CLICKED(IDC_DGT_NETWORK_WAIT_TIME, OnDgtNetworkWaitTime)
	ON_BN_CLICKED(IDC_DGT_NETWORK_RETRY_COUNT, OnDgtNetworkRetryCount)
	ON_BN_CLICKED(IDC_DGT_ALARM_DELAY_TIME, OnDgtAlarmDelayTime)
	ON_BN_CLICKED(IDC_BTN_IP_SAVE_ECSERVER, OnBtnIpSaveEcserver)
	ON_BN_CLICKED(IDC_BTN_PORT_SAVE_ECSERVER, OnBtnPortSaveEcserver)
	ON_BN_CLICKED(IDC_BTN_CONNECT_ECSERVER, OnBtnConnectEcserver)
	ON_BN_CLICKED(IDC_BTN_TEST_ECSERVER, OnBtnTestEcserver)
	ON_BN_CLICKED(IDC_DGT_TRAY_X, OnDgtTrayX)
	ON_BN_CLICKED(IDC_DGT_TRAY_Y, OnDgtTrayY)
	ON_BN_CLICKED(IDC_RADIO_COK_0, OnRadioCok0)
	ON_BN_CLICKED(IDC_RADIO_COK_1, OnRadioCok1)
	ON_BN_CLICKED(IDC_RADIO_COK_2, OnRadioCok2)
	ON_BN_CLICKED(IDC_RADIO_COK_3, OnRadioCok3)
	ON_BN_CLICKED(IDC_RADIO_COK_4, OnRadioCok4)
	ON_BN_CLICKED(IDC_RADIO_COK_5, OnRadioCok5)
	ON_BN_CLICKED(IDC_RADIO_DEGREE_0, OnRadioDegree0)
	ON_BN_CLICKED(IDC_RADIO_DEGREE_90, OnRadioDegree90)
	ON_BN_CLICKED(IDC_RADIO_DEGREE_180, OnRadioDegree180)
	ON_BN_CLICKED(IDC_RADIO_DEGREE_270, OnRadioDegree270)
	ON_BN_CLICKED(IDC_RADIO_PT_1V1M_USE4, OnRadioPt1v1mUse4)
	ON_BN_CLICKED(IDC_RADIO_PT_1V1M_USE2, OnRadio1v1mUse2)
	ON_BN_CLICKED(IDC_RADIO_PT_2V1M_USE2, OnRadio2v1mUse2)
	ON_BN_CLICKED(IDC_RADIO_ONLINE_MODE, OnRadioOnlineMode)
	ON_BN_CLICKED(IDC_RADIO_OFFLINE_MODE, OnRadioOfflineMode)
	ON_BN_CLICKED(IDC_DGT_TIME_OUT, OnDgtTimeOut)
	ON_BN_CLICKED(IDC_RADIO_BYPASS_FRONT, OnRadioBypassFront)
	ON_BN_CLICKED(IDC_RADIO_BYPASS_REAR, OnRadioBypassRear)
	ON_BN_CLICKED(IDC_RADIO_BYPASS_ALL, OnRadioBypassAll)
	ON_BN_CLICKED(IDC_RADIO_BYPASS_NOTUSE, OnRadioBypassNotuse)
	ON_BN_CLICKED(IDC_DGT_EMPTY, OnDgtEmpty)
	ON_BN_CLICKED(IDC_RADIO_POGO_MODE, OnRadioPogoMode)
	ON_BN_CLICKED(IDC_RADIO_POGO_NOTUSE, OnRadioPogoNotuse)
	ON_BN_CLICKED(IDC_DGT_ROBOT_INIT_TIME, OnDgtRobotInitTime)
	ON_BN_CLICKED(IDC_DGT_RUN_SPEED, OnDgtRunSpeed)
	ON_BN_CLICKED(IDC_DGT_MANUAL_SPEED, OnDgtManualSpeed)
	ON_BN_CLICKED(IDC_RADIO_JIG_MODE, OnRadioJigMode)
	ON_BN_CLICKED(IDC_RADIO_JIG_NOTUSE, OnRadioJigNotuse)
	ON_BN_CLICKED(IDC_RADIO_RECOVERY_MODE, OnRadioRecoveryMode)
	ON_BN_CLICKED(IDC_RADIO_RECOVERY_NOTUSE, OnRadioRecoveryNotuse)
	ON_BN_CLICKED(IDC_DGT_HOTFIX_POS, OnDgtHotfixPos)
	ON_BN_CLICKED(IDC_DGT_SOCKET_MAX_COUNT, OnDgtSocketMaxCount)
	ON_BN_CLICKED(IDC_RADIO_PT_1V1M_USE4_2, OnRadioPt1v1mUse42)
	ON_BN_CLICKED(IDC_RADIO_USE_XGEM, OnRadioUseXgem)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_XGEM, OnRadioNotuseXgem)
	ON_BN_CLICKED(IDC_RADIO_USE_RCMD, OnRadioUseRcmd)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_RCMD, OnRadioNotuseRcmd)
	ON_CBN_SELCHANGE(IDC_COMBO_ADD_RECIPE, OnSelchangeComboAddRecipe)
	ON_BN_CLICKED(IDC_RADIO_USE_PRODUCT, OnRadioUseProduct)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_PRODUCT, OnRadioNotuseProduct)
	ON_BN_CLICKED(IDC_RADIO_USE_SITE1, OnRadioUseSite1)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_SITE1, OnRadioNotuseSite1)
	ON_BN_CLICKED(IDC_RADIO_USE_SITE2, OnRadioUseSite2)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_SITE2, OnRadioNotuseSite2)
	ON_BN_CLICKED(IDC_RADIO_1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO_2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO_BOTH, OnRadioBoth)
	ON_BN_CLICKED(IDC_RADIO_21, OnRadio21)
	ON_BN_CLICKED(IDC_RADIO_22, OnRadio22)
	ON_BN_CLICKED(IDC_RADIO_BOTH2, OnRadioBoth2)
	ON_CBN_SELCHANGE(IDC_COMBO_HIFIX_MODE, OnSelchangeComboHifixMode)
	ON_BN_CLICKED(IDC_RADIO_RETEST_JIG_USE, OnRadioRetestJigUse)
	ON_BN_CLICKED(IDC_RADIO_RETEST_JIG_NOTUSE, OnRadioRetestJigNotuse)
	ON_BN_CLICKED(IDC_RADIO_RECIPE_USE, OnRadioRecipeUse)
	ON_BN_CLICKED(IDC_RADIO_RECIPE_NOTUSE, OnRadioRecipeNotuse)
	ON_BN_CLICKED(IDC_RADIO_CSERIAL2_USE, OnRadioCserial2Use)
	ON_BN_CLICKED(IDC_RADIO_CSERIAL2_NOTUSE, OnRadioCserial2Notuse)
	ON_BN_CLICKED(IDC_RADIO_AUTO_TIMEOUT, OnRadioAutoTimeout)
	ON_BN_CLICKED(IDC_DGT_TIME_OUT_PER, OnDgtTimeOutPer)
	ON_BN_CLICKED(IDC_BTN_SEND_HIFIX, OnBtnSendHifix)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)
	ON_MESSAGE(WM_BASIC_APPLY, OnBasic_Info_Apply)  // BASIC DATA를 APPLY 시키기 위한 사용자 정의 메시지 선언
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Basic diagnostics

#ifdef _DEBUG
void CScreen_Basic::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Basic::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Basic message handlers

void CScreen_Basic::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

//	Loadhifixmode();//2015.0518	

	// **************************************************************************
	// 컨트롤에 설정할 폰트 생성한다
	// **************************************************************************
// 	mp_basic_font = NULL;
// 	mp_basic_font = new CFont;
// 	mp_basic_font->CreateFont(15,8,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");
	// **************************************************************************
 	OnBasic_Init_TrayOffset( MPOS_INDEX_BIN1 );
	OnBasic_Init_TrayOffset( MPOS_INDEX_BIN2 );
	//ybs
	OnBasic_Init_TrayOffset( MPOS_INDEX_BIN3);
	OnBasic_Init_TrayOffset( MPOS_INDEX_REJ );

 	OnBasic_Data_Set();				// 전역 변수의 Data를 받아온다.
	
	OnBasic_GroupBox_Set();
	OnBasic_EditBox_Set();
 	OnBasic_Digital_Count_Set();
	OnBasic_Label_Set();

	OnBasic_Data_Change();

	
	// **************************************************************************
    // 칼라 리스트 박스 컨트롤 생성한다
    // **************************************************************************
	OnBasic_Create_ImageList();		// 이미지 리스트 생성 함수
	
	// Associate image list to list box
	m_list_device_type.SetImageList(&mp_device_image_list);
	
	OnBasic_Init_List(st_path.mstr_path_dvc);  // 파일 폴더 초기화 함수
	// **************************************************************************
	
	OnBasic_Device_Focus_Set();  // 선택된 디바이스명 포커스 설정 함수

	st_handler.cwnd_basic = this;  // BASIC 화면에 대한 핸들 정보 설정

	OnShow_Bypass_Box();
	LoadRegRecipeIdData();
	LoadRegHifixdData();//2015.0604

	OnBasic_Data_Display();
}

void CScreen_Basic::LoadRegRecipeIdData()
{
	BOOL m_bRecipe = FALSE;
	for( int i=0; i< st_handler.m_nTotal; i++ )
	{
		if(st_handler.m_strRecipe[i] != "")
		{
			m_combo_recipe.AddString(st_handler.m_strRecipe[i]);
		}
		if(st_basic.mstr_revipe_name == st_handler.m_strRecipe[i])
		{
			m_bRecipe = TRUE;
		}
	}
	if(m_bRecipe == TRUE)
	{
		m_combo_recipe.SetWindowText(st_basic.mstr_revipe_name);
	}
	else
	{//2014,0623
		m_combo_recipe.SetWindowText("AMT8520");
	}

}

////2015.0604
void CScreen_Basic::LoadRegHifixdData()
{
	BOOL m_bHifix = FALSE;
	for( int i=0; i< st_handler.m_nhifix_mode_cnt; i++ )
	{
		if(st_handler.mstr_hifix_mode[i] != "")
		{
			m_cbohifix_mode.AddString(st_handler.mstr_hifix_mode[i]);
		}
		if(st_basic.n_str_hifix_mode == st_handler.mstr_hifix_mode[i])
		{
			m_bHifix = TRUE;
		}
	}
	if(m_bHifix == TRUE)
	{
		m_cbohifix_mode.SetWindowText(st_basic.n_str_hifix_mode);
	}
	else
	{
		m_cbohifix_mode.SetWindowText("MSATA");
	}	
}
////


void CScreen_Basic::OnDestroy() 
{
	int n_response = 0;  // 대화 상자에 대한 리턴 값 저장 변수
	int Ret = 0;
	
	CDialog_Select select_dlg;
	
	mp_device_image_list.DeleteImageList();
	
	// **************************************************************************
	// 생성한 폰트 정보 삭제한다
	// **************************************************************************
// 	delete mp_basic_font;
// 	mp_basic_font = NULL;
	// **************************************************************************
	
	st_handler.cwnd_basic = NULL;  // BASIC 화면 핸들 초기화

	CFormView::OnDestroy();	
}

void CScreen_Basic::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnTimer(nIDEvent);
}

void CScreen_Basic::OnBasic_GroupBox_Set()
{
	CSxLogFont basic_font(15,FW_SEMIBOLD,false,"Arial");
	
	m_group_device_mode.SetFont(basic_font);
	m_group_device_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_device_mode.SetFontBold(TRUE);

	m_group_recipe_select.SetFont(basic_font);
	m_group_recipe_select.SetCatptionTextColor(RGB(145,25,0));
	m_group_recipe_select.SetFontBold(TRUE);

	m_group_electrostatic_check_gap.SetFont(basic_font);
	m_group_electrostatic_check_gap.SetCatptionTextColor(RGB(145,25,0));
	m_group_electrostatic_check_gap.SetFontBold(TRUE);

	m_group_electrostatic_check_value.SetFont(basic_font);
	m_group_electrostatic_check_value.SetCatptionTextColor(RGB(145,25,0));
	m_group_electrostatic_check_value.SetFontBold(TRUE);

	m_group_electrostatic_test.SetFont(basic_font);
	m_group_electrostatic_test.SetCatptionTextColor(RGB(145,25,0));
	m_group_electrostatic_test.SetFontBold(TRUE);

	m_group_alarm_delay_time.SetFont(basic_font);
	m_group_alarm_delay_time.SetCatptionTextColor(RGB(145,25,0));
	m_group_alarm_delay_time.SetFontBold(TRUE);

	m_group_network_retry_count.SetFont(basic_font);
	m_group_network_retry_count.SetCatptionTextColor(RGB(145,25,0));
	m_group_network_retry_count.SetFontBold(TRUE);

	m_group_network_wait_time.SetFont(basic_font);
	m_group_network_wait_time.SetCatptionTextColor(RGB(145,25,0));
	m_group_network_wait_time.SetFontBold(TRUE);

	m_group_tray_size_setting.SetFont(basic_font);
	m_group_tray_size_setting.SetCatptionTextColor(RGB(145,25,0));
	m_group_tray_size_setting.SetFontBold(TRUE);
	
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		m_group_pogo_mode.SetFont(basic_font);
		m_group_pogo_mode.SetCatptionTextColor(RGB(145,25,0));
		m_group_pogo_mode.SetFontBold(TRUE);
	}
	else
	{
		m_group_pogo_mode.ShowWindow(FALSE);
		m_radio_pogo_mode.ShowWindow(FALSE);
		m_radio_pogo_notuse.ShowWindow(FALSE);
	}

	m_group_robot_init.SetFont(basic_font);
	m_group_robot_init.SetCatptionTextColor(RGB(145,25,0));
	m_group_robot_init.SetFontBold(TRUE);

	m_group_basic_speed.SetFont(basic_font);
	m_group_basic_speed.SetCatptionTextColor(RGB(145,25,0));
	m_group_basic_speed.SetFontBold(TRUE);

	m_group_basic_run_speed.SetFont(basic_font);
	m_group_basic_run_speed.SetCatptionTextColor(RGB(145,25,0));
	m_group_basic_run_speed.SetFontBold(TRUE);
	
	m_group_basic_manual_speed.SetFont(basic_font);
	m_group_basic_manual_speed.SetCatptionTextColor(RGB(145,25,0));
	m_group_basic_manual_speed.SetFontBold(TRUE);

	m_group_hotfix_pos.SetFont(basic_font);
	m_group_hotfix_pos.SetCatptionTextColor(RGB(145,25,0));
	m_group_hotfix_pos.SetFontBold(TRUE);

	////2015.0518
	m_group_hifix_mode.SetFont(basic_font);
	m_group_hifix_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_hifix_mode.SetFontBold(TRUE);
	////

	//2013,0215
	if(st_basic.mn_alignBuffer_type == CTL_YES)
	{
		m_group_jig_mode.SetFont(basic_font);
		m_group_jig_mode.SetCatptionTextColor(RGB(145,25,0));
		m_group_jig_mode.SetFontBold(TRUE);
		m_group_jig_mode.ShowWindow(TRUE);
		m_radio_jig_notuse.ShowWindow(TRUE);
		m_radio_jig_use.ShowWindow(TRUE);
	}
	else
	{
		m_group_jig_mode.ShowWindow(FALSE);
		m_radio_jig_notuse.ShowWindow(FALSE);
		m_radio_jig_use.ShowWindow(FALSE);
	}

	m_group_recovery_mode.SetFont(basic_font);
	m_group_recovery_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_recovery_mode.SetFontBold(TRUE);

	m_group_socket_max_count.SetFont(basic_font);
	m_group_socket_max_count.SetCatptionTextColor(RGB(145,25,0));
	m_group_socket_max_count.SetFontBold(TRUE);

	m_group_product.SetFont(basic_font);
	m_group_product.SetCatptionTextColor(RGB(145,25,0));
	m_group_product.SetFontBold(TRUE);
	
	m_group_product_site1.SetFont(basic_font);
	m_group_product_site1.SetCatptionTextColor(RGB(145,25,0));
	m_group_product_site1.SetFontBold(TRUE);

	m_group_product_site2.SetFont(basic_font);
	m_group_product_site2.SetCatptionTextColor(RGB(145,25,0));
	m_group_product_site2.SetFontBold(TRUE);
	
	m_group_sie1.SetFont(basic_font);
	m_group_sie1.SetCatptionTextColor(RGB(145,25,0));
	m_group_sie1.SetFontBold(TRUE);

	m_group_sie2.SetFont(basic_font);
	m_group_sie2.SetCatptionTextColor(RGB(145,25,0));
	m_group_sie2.SetFontBold(TRUE);

	////2015.0914
	m_group_retest_jig_mode.SetFont(basic_font);
	m_group_retest_jig_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_retest_jig_mode.SetFontBold(TRUE);
	////

	m_group_recipe_mode.SetFont(basic_font);
	m_group_recipe_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_recipe_mode.SetFontBold(TRUE);

	//2015.1123
	m_group_cserial2.SetFont(basic_font);
	m_group_cserial2.SetCatptionTextColor(RGB(145,25,0));
	m_group_cserial2.SetFontBold(TRUE);

}

void CScreen_Basic::OnBasic_EditBox_Set()
{
	m_edit_device_type.SubclassDlgItem(IDC_EDIT_DEVICE_TYPE, this);
	m_edit_device_type.bkColor(RGB(50, 100, 150));
	m_edit_device_type.textColor(RGB(255, 255,255));
	m_edit_device_type.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "Arial");
}

void CScreen_Basic::OnBasic_Digital_Count_Set()
{	
	m_dgt_electrostatic_check_gap.SubclassDlgItem(IDC_DGT_ELECTROSTATIC_CHECK_GAP, this);
	m_dgt_electrostatic_check_gap.SetStyle(IDB_BIG3, 4);
	m_dgt_electrostatic_check_gap.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_electrostatic_check_gap ) );
		
	m_dgt_electrostatic_check_value.SetStyle(CDigit::DS_FLOAT3, 7, CDigit::DC_BBLACK);

	m_dgt_alarm_delay_time.SubclassDlgItem(IDC_DGT_ALARM_DELAY_TIME, this);
	m_dgt_alarm_delay_time.SetStyle(IDB_BIG3, 4);
	m_dgt_alarm_delay_time.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_alarm_delay_time ) );

	m_dgt_network_retry_cnt.SubclassDlgItem(IDC_DGT_NETWORK_RETRY_COUNT, this);
	m_dgt_network_retry_cnt.SetStyle(IDB_BIG3, 4);
	m_dgt_network_retry_cnt.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_network_retry_cnt ) );

	m_dgt_network_wait_time.SubclassDlgItem(IDC_DGT_NETWORK_WAIT_TIME, this);
	m_dgt_network_wait_time.SetStyle(IDB_BIG3, 4);
	m_dgt_network_wait_time.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_network_wait_time ) );

	m_dgt_time_out.SubclassDlgItem(IDC_DGT_TIME_OUT, this);
	m_dgt_time_out.SetStyle(IDB_BIG3, 4);
	m_dgt_time_out.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_time_out ) );

	m_dgt_tray_x.SubclassDlgItem(IDC_DGT_TRAY_X, this);
	m_dgt_tray_x.SetStyle(IDB_BIG3, 2);
	m_dgt_tray_x.SetValue( g_dataMgr.GetBackupData( &st_basic.n_tray_x ) );

	m_dgt_tray_y.SubclassDlgItem(IDC_DGT_TRAY_Y, this);
	m_dgt_tray_y.SetStyle(IDB_BIG3, 2);
	m_dgt_tray_y.SetValue( g_dataMgr.GetBackupData( &st_basic.n_tray_y ) );

	m_dgt_empty_tray_out.SubclassDlgItem(IDC_DGT_EMPTY, this);
	m_dgt_empty_tray_out.SetStyle(IDB_BIG3, 4);
	m_dgt_empty_tray_out.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_empty_tray_out ) );

	m_dgt_robot_init_time.SubclassDlgItem(IDC_DGT_ROBOT_INIT_TIME, this);
	m_dgt_robot_init_time.SetStyle(IDB_BIG3, 4);
	m_dgt_robot_init_time.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_robot_init_time ) );

	m_dgt_basic_run_speed.SubclassDlgItem(IDC_DGT_RUN_SPEED, this);
	m_dgt_basic_run_speed.SetStyle(IDB_BIG3, 3);
	m_dgt_basic_run_speed.SetValue(g_dataMgr.GetBackupData( &st_basic.nRunSpeed ));
	
	m_dgt_basic_manual_speed.SubclassDlgItem(IDC_DGT_MANUAL_SPEED, this);
	m_dgt_basic_manual_speed.SetStyle(IDB_BIG3, 3);
	m_dgt_basic_manual_speed.SetValue(g_dataMgr.GetBackupData( &st_basic.nManualSpeed ));

	m_dgt_hotfix_pos.SubclassDlgItem(IDC_DGT_HOTFIX_POS, this);
	m_dgt_hotfix_pos.SetStyle(IDB_BIG3, 1);
	m_dgt_hotfix_pos.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_hotfix_pos ) );
	
 	m_dgt_socket_max_count.SubclassDlgItem(IDC_DGT_SOCKET_MAX_COUNT, this);
 	m_dgt_socket_max_count.SetStyle(IDB_BIG3, 6);
 	m_dgt_socket_max_count.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_socket_max_count ) );

	m_dgt_time_out_per.SubclassDlgItem(IDC_DGT_TIME_OUT_PER, this);
	m_dgt_time_out_per.SetStyle(IDB_BIG3, 3);
	m_dgt_time_out_per.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_time_out_per ) );


}

void CScreen_Basic::OnBasic_Data_Set()
{
	g_dataMgr.BackupDatas();
}

void CScreen_Basic::OnBasic_Data_Change()
{
	OnBasic_ProductSite2Use_Change();
	OnBasic_ProductSite1Use_Change();
	OnBasic_ProductMode_Change();
	OnBasic_ProductSite1Mode_Change();
	OnBasic_ProductSite2Mode_Change();
	OnBasic_DeviceMode_Change();
	OnBasic_PogoMode_Change();
	OnBasic_JigMode_Change();
	OnBasic_RecoveryMode_Change();
	OnBasic_ByPassMode_Change();
	OnBasic_CokType_Change();
	OnBasic_Degree_Change();
	OnBasic_PickType_Change();
	OnBasic_RunMode_Change();
	OnBasic_XgemMode_Change();
	OnBasic_RcmdMode_Change();

	OnBasic_TrayOffset_Change( MPOS_INDEX_BIN1 );
	OnBasic_TrayOffset_Change( MPOS_INDEX_BIN2 );
	//ybs
	OnBasic_TrayOffset_Change( MPOS_INDEX_BIN3 );
	OnBasic_TrayOffset_Change( MPOS_INDEX_REJ );

	OnBasic_Retest_JigMode_Change();//2015.0914
	OnBasic_Recipe_Mode_Change();

	//2015.1123
	OnBasic_CSerial2Mode_Change();

	//2016.0112
	OnBasic_Auto_TimeOut_mode_change();

}

void CScreen_Basic::OnBasic_Label_Set()
{
	m_msg_electrostatic_1.SetFont(Func.mp_basic_font);
	m_msg_electrostatic_1.SetWindowText(_T(""));
	m_msg_electrostatic_1.SetCenterText();
	m_msg_electrostatic_1.SetColor(YELLOW_C);
	m_msg_electrostatic_1.SetGradientColor(YELLOW_C);
	m_msg_electrostatic_1.SetTextColor(RGB(0,0,0));

	m_msg_electrostatic_2.SetFont(Func.mp_basic_font);
	m_msg_electrostatic_2.SetWindowText(_T(""));
	m_msg_electrostatic_2.SetCenterText();
	m_msg_electrostatic_2.SetColor(YELLOW_C);
	m_msg_electrostatic_2.SetGradientColor(YELLOW_C);
	m_msg_electrostatic_2.SetTextColor(RGB(0,0,0));

	m_msg_electrostatic_3.SetFont(Func.mp_basic_font);
	m_msg_electrostatic_3.SetWindowText(_T(""));
	m_msg_electrostatic_3.SetCenterText();
	m_msg_electrostatic_3.SetColor(YELLOW_C);
	m_msg_electrostatic_3.SetGradientColor(YELLOW_C);
	m_msg_electrostatic_3.SetTextColor(RGB(0,0,0));

	m_msg_electrostatic_4.SetFont(Func.mp_basic_font);
	m_msg_electrostatic_4.SetWindowText(_T(""));
	m_msg_electrostatic_4.SetCenterText();
	m_msg_electrostatic_4.SetColor(YELLOW_C);
	m_msg_electrostatic_4.SetGradientColor(YELLOW_C);
	m_msg_electrostatic_4.SetTextColor(RGB(0,0,0));

	m_msg_electrostatic_5.SetFont(Func.mp_basic_font);
	m_msg_electrostatic_5.SetWindowText(_T(""));
	m_msg_electrostatic_5.SetCenterText();
	m_msg_electrostatic_5.SetColor(YELLOW_C);
	m_msg_electrostatic_5.SetGradientColor(YELLOW_C);
	m_msg_electrostatic_5.SetTextColor(RGB(0,0,0));

	m_label_electrostatic_1.SetFont(Func.mp_basic_font);
	m_label_electrostatic_1.SetWindowText(_T("Robot Z Axis"));
	m_label_electrostatic_1.SetCenterText();
	m_label_electrostatic_1.SetColor(RGB(0,0,255));
	m_label_electrostatic_1.SetGradientColor(RGB(0,0,0));
	m_label_electrostatic_1.SetTextColor(RGB(255,255,255));
	
	m_label_electrostatic_2.SetFont(Func.mp_basic_font);
	m_label_electrostatic_2.SetWindowText(_T("LOB Buffer 1"));
	m_label_electrostatic_2.SetCenterText();
	m_label_electrostatic_2.SetColor(RGB(0,0,255));
	m_label_electrostatic_2.SetGradientColor(RGB(0,0,0));
	m_label_electrostatic_2.SetTextColor(RGB(255,255,255));
	
	m_label_electrostatic_3.SetFont(Func.mp_basic_font);
	m_label_electrostatic_3.SetWindowText(_T("LOB Buffer 2"));
	m_label_electrostatic_3.SetCenterText();
	m_label_electrostatic_3.SetColor(RGB(0,0,255));
	m_label_electrostatic_3.SetGradientColor(RGB(0,0,0));
	m_label_electrostatic_3.SetTextColor(RGB(255,255,255));
	
	m_label_electrostatic_4.SetFont(Func.mp_basic_font);
	m_label_electrostatic_4.SetWindowText(_T("Reject Buffer"));
	m_label_electrostatic_4.SetCenterText();
	m_label_electrostatic_4.SetColor(RGB(0,0,255));
	m_label_electrostatic_4.SetGradientColor(RGB(0,0,0));
	m_label_electrostatic_4.SetTextColor(RGB(255,255,255));
	
	m_label_electrostatic_5.SetFont(Func.mp_basic_font);
	m_label_electrostatic_5.SetWindowText(_T("Retest Buffer"));
	m_label_electrostatic_5.SetCenterText();
	m_label_electrostatic_5.SetColor(RGB(0,0,255));
	m_label_electrostatic_5.SetGradientColor(RGB(0,0,0));
	m_label_electrostatic_5.SetTextColor(RGB(255,255,255));

	m_msg_tray_x.SetFont(Func.mp_basic_font);
	m_msg_tray_x.SetWindowText(_T("X size"));
	m_msg_tray_x.SetCenterText();
	m_msg_tray_x.SetColor(RGB(0,0,255));
	m_msg_tray_x.SetGradientColor(RGB(0,0,0));
	m_msg_tray_x.SetTextColor(RGB(255,255,255));

	m_msg_tray_y.SetFont(Func.mp_basic_font);
	m_msg_tray_y.SetWindowText(_T("Y size"));
	m_msg_tray_y.SetCenterText();
	m_msg_tray_y.SetColor(RGB(0,0,255));
	m_msg_tray_y.SetGradientColor(RGB(0,0,0));
	m_msg_tray_y.SetTextColor(RGB(255,255,255));
}

void CScreen_Basic::OnRadioDeviceModeUse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_device ) == CTL_YES )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_mode_device, (int)CTL_YES );
	OnBasic_DeviceMode_Change();
}

void CScreen_Basic::OnRadioDeviceModeNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_device ) == CTL_NO )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_mode_device, (int)CTL_NO );
	OnBasic_DeviceMode_Change();
}

void CScreen_Basic::OnBasic_DeviceMode_Change()
{
	int nDeviceMode = g_dataMgr.GetBackupData( &st_basic.n_mode_device );
	if (nDeviceMode < 0 || nDeviceMode > 1)
	{
		nDeviceMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_device, (int)0 );
	}
	
	switch (nDeviceMode)
	{
	case 0:
		m_radio_device_mode_use.SetCheck(FALSE);
		m_radio_device_mode_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_device_mode_use.SetCheck(TRUE);
		m_radio_device_mode_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnBasic_CokType_Change()
{
	int nCokType = g_dataMgr.GetBackupData( &st_basic.n_cok_type );
	if( nCokType < 0 || nCokType >= COK_TYPE_CNT )
	{
		nCokType = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)0 );
	}

	m_radio_cok_0.SetCheck( FALSE );
	m_radio_cok_1.SetCheck( FALSE );
	m_radio_cok_2.SetCheck( FALSE );
	m_radio_cok_3.SetCheck( FALSE );
	m_radio_cok_4.SetCheck( FALSE );
	m_radio_cok_5.SetCheck( FALSE );
	
	switch( nCokType )
	{
	case 0:	m_radio_cok_0.SetCheck( TRUE );	break;
	case 1:	m_radio_cok_1.SetCheck( TRUE );	break;
	case 2:	m_radio_cok_2.SetCheck( TRUE );	break;
	case 3:	m_radio_cok_3.SetCheck( TRUE );	break;
	case 4:	m_radio_cok_4.SetCheck( TRUE );	break;
	case 5:	m_radio_cok_5.SetCheck( TRUE );	break;
	}
}

void CScreen_Basic::OnIP_Address(CString str_ip, int n_section)
{
	int nDot1, nDot2, nDot3, nStart;
	
	nStart = 0;
	nDot1 = str_ip.Find('.');
	nDot2 = str_ip.Find('.', nDot1 + 1);
	nDot3 = str_ip.Find('.', nDot2 + 1);
	
	if (n_section < NETWORK_ECSERVER || n_section > NETWORK_ECSERVER)		// 0: TESTER, 1: FTP
	{
		return;
	}
	
	nIPFiled0[n_section] = atoi(str_ip.Mid(nStart, nDot1 - nStart));
	nStart = nDot1 + 1;
	nIPFiled1[n_section] = atoi(str_ip.Mid(nStart, nDot2 - nStart));
	nStart = nDot2 + 1;
	nIPFiled2[n_section] = atoi(str_ip.Mid(nStart, nDot3 - nStart));
	nStart = nDot3 + 1;
	nIPFiled3[n_section] = atoi(str_ip.Mid(nStart));
}

void CScreen_Basic::OnDgtElectrostaticCheckGap() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	char chr_buf[20] ;
	int mn_minute;
	
	mn_minute = m_dgt_electrostatic_check_gap.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_minute, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("정전기 확인 시간 (분)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_keypad_msg = _T("Check the electrostatic time (minutes)");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(1, 1440, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp(&st_basic.mn_electrostatic_check_gap, iTemp );
	m_dgt_electrostatic_check_gap.SetValue( iTemp );
}

void CScreen_Basic::OnDgtElectrostaticCheckValue() 
{
// 	int mn_response;	// 대화 상자에 대한 리턴 값 저장 변수
 	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
 	double d_dgt_data;

	d_dgt_data = (double)m_dgt_electrostatic_check_value.GetFloatVal();

	mstr_temp = _T("그라운드 설정값");
 	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Setting ground");
	
	CRect r;
	
	m_dgt_electrostatic_check_value.GetWindowRect(&r);
	
	mstr_temp = KeyPad.GetNumEditString_D((double)0, (double)120, d_dgt_data, mstr_temp, &r);
	
	d_dgt_data = atof(mstr_temp);

	g_dataMgr.SetDataTemp( &st_basic.md_electrostatic_value, d_dgt_data );
	m_dgt_electrostatic_check_value.SetFloatVal((float)d_dgt_data);
}

void CScreen_Basic::OnBtnDvcCreate() 
{
	int n_response;									// 대화 상자 리턴 플래그
	CString str_selected_dvc;						// 선택된 디바이스 이름 저장 변수

	str_selected_dvc = Keyboard.GetEditString("RECEPI NAME", "");

	if (str_selected_dvc != "")
	{
		mstr_new_device = str_selected_dvc;
		
		if (OnBasic_New_Device_Check() == FALSE)	return;
		
		//2013,0215
		CDialog_AlignPogo dlg;
		n_response = dlg.DoModal();

		OnBasic_Create_Device();
		
		// ******************************************************************
		// 폴더에 존재하는 파일 목록 리스트 박스에 재설정한다
		// ******************************************************************
		m_list_device_type.ResetContent();		// 리스트 박스의 모든 목록 삭제
		mp_device_image_list.DeleteImageList();	// 이미지 리스트에 설정된 정보 삭제
		
		OnBasic_Create_ImageList();				// 이미지 리스트 생성 함수
		
		// Associate image list to list box
		m_list_device_type.SetImageList(&mp_device_image_list);
		
		OnBasic_Init_List(st_path.mstr_path_dvc);// 파일 폴더 초기화 함수
		OnSelchangeListDeviceType();
		OnBasic_Device_Focus_Set();				// 선택된 디바이스명 포커스 설정 함수
		
		if (mn_device_name > 0)
		{
			m_list_device_type.GetText(mn_device_name, str_selected_dvc);
		}
		
		OnBasic_Device_Display(str_selected_dvc);	// 선택된 디바이스 이름 출력 함수
		// ******************************************************************

// 		CString strfile = st_path.mstr_path_dvc + str_selected_dvc;
// 		mcls_basic.OnPogoAlignType_Save(strfile);
	}
	
// 	CDialog_Keyboard  keyboard_dlg;
// 	
// 	(st_msg.mstr_typing_msg).Empty();				// 키보드 상자 출력 데이터 초기화
// 	
// 	n_response = keyboard_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		mstr_new_device = st_msg.mstr_typing_msg;  // 키보드 상자 입력 정보 클래스 변수에 설정
// 		
// 		if (OnBasic_New_Device_Check() == FALSE)	return;
// 		
// 		OnBasic_Create_Device();
// 		
// 		// ******************************************************************
// 		// 폴더에 존재하는 파일 목록 리스트 박스에 재설정한다
// 		// ******************************************************************
// 		m_list_device_type.ResetContent();		// 리스트 박스의 모든 목록 삭제
// 		mp_device_image_list.DeleteImageList();	// 이미지 리스트에 설정된 정보 삭제
// 		
// 		OnBasic_Create_ImageList();				// 이미지 리스트 생성 함수
// 		
// 		// Associate image list to list box
// 		m_list_device_type.SetImageList(&mp_device_image_list);
// 		
// 		OnBasic_Init_List(st_path.mstr_path_dvc);// 파일 폴더 초기화 함수
// 		OnSelchangeListDeviceType();
// 		OnBasic_Device_Focus_Set();				// 선택된 디바이스명 포커스 설정 함수
// 		
// 		if (mn_device_name > 0)
// 		{
// 			m_list_device_type.GetText(mn_device_name, str_selected_dvc);
// 		}
// 		
// 		OnBasic_Device_Display(str_selected_dvc);	// 선택된 디바이스 이름 출력 함수
// 		// ******************************************************************
// 	}
// 	else if (n_response == IDCANCEL)
// 	{
// 	}
}

int CScreen_Basic::OnBasic_New_Device_Check()
{
	CString str_chk_file;  // 생성할 [폴더]+[파일명] 저장 변수
	CString str_chk_ext;   // 확장자 저장 변수
	int n_pos;
	
	// **************************************************************************
    // 입력된 디바이스명 설정한다
    // **************************************************************************
	mstr_new_device.MakeUpper();
	mstr_new_device.TrimLeft(' ');               
	mstr_new_device.TrimRight(' ');
	
	if (mstr_new_device.IsEmpty())  
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
//			st_msg.mstr_abnormal_msg = _T("[DEVICE] A name input error occurrence.");
			sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name input error occurrence.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return FALSE;
	}
	// **************************************************************************
	
	str_chk_file = st_path.mstr_path_dvc + mstr_new_device;  // 생성할 [폴더]+[파일명] 설정

	n_pos = str_chk_file.Find(".");  // 확장자 검사
	if (n_pos == -1) 
		str_chk_file += _T(".TXT");
	else 
	{
		str_chk_ext = str_chk_file.Mid(n_pos);  // 확장자 정보 설정
		if (str_chk_ext != _T(".TXT"))  
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE] A name extension input error occurrence.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name extension input error occurrence.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			return FALSE;
		}
	}
	
	mstr_load_file = str_chk_file;  // 생성할 [폴더]+[파일명]+[확장자] 설정
	
	return TRUE;
}

void CScreen_Basic::OnBasic_Create_Device()
{
	int n_existence;								// 파일 존재 유무 플래그
	char chr_file_name[256];
	CString str_device_temp;
	CString str_original_path;
	CString str_original_file;
	CString str_copy_file;
	CString str_chk_ext;
	CString str_copy_info,mstr_temp;
	
	sprintf(chr_file_name, "%s", mstr_load_file);	// [폴더]+[파일명] 설정
	
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)							// 파일 존재
		return ;
	else  
	{
		str_device_temp = st_basic.mstr_device_name;
		st_basic.mstr_device_name = mstr_new_device;
		
		:: WritePrivateProfileString("Device", "DeviceFile", LPCTSTR(mstr_new_device), mstr_load_file); 
		
		//2013,0215
		mstr_temp.Format("%d", st_handler.mn_temp_pogo_type);
		:: WritePrivateProfileString("BasicData", "Pogo_Type", LPCTSTR(mstr_temp), mstr_load_file);
		
		mstr_temp.Format("%d", st_handler.mn_temp_alignBuffer_type);
		:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", LPCTSTR(mstr_temp), mstr_load_file);
		
		mcls_basic.On_Teach_Data_Save();
		
		st_basic.mstr_device_name = str_device_temp;
	}
}

void CScreen_Basic::OnSelchangeListDeviceType() 
{
	CString str_selected_dvc;						// 선택된 디바이스 이름 저장 변수
	
	int n_index = m_list_device_type.GetCurSel();	// 선택된 리스트 위치 설정
	if (n_index != LB_ERR)							// 에러 검사
	{
		m_list_device_type.GetText(n_index, str_selected_dvc);
		
		OnBasic_Device_Display(str_selected_dvc);	// 선택된 디바이스 이름 출력 함수
	}
	else
		OnBasic_Device_Display("");	// 빈 이름 출력 함수
}

void CScreen_Basic::OnBasic_Device_Focus_Set()
{
	if (mn_device_name == -1)
		return;
	
	m_list_device_type.SetCurSel(mn_device_name);
}

void CScreen_Basic::OnBasic_Device_Display(CString str_dvc)
{
	((CEdit*)GetDlgItem(IDC_EDIT_DEVICE_TYPE))->SetWindowText(_T(str_dvc));
}

void CScreen_Basic::OnBasic_Create_ImageList()
{
	BOOL bRetValue = FALSE;
	
	// Create image list
	bRetValue = mp_device_image_list.Create(32, 32, ILC_COLOR32 | ILC_MASK, 5, 1);
	ASSERT(bRetValue == TRUE);
}

int CScreen_Basic::OnBasic_Init_List(LPCTSTR pszPath)
{
	int n_chk;						// 함수 리턴 플래그
    CString str_path = pszPath;		// 폴더 설정
	
	HANDLE hFind;
    WIN32_FIND_DATA fd;
	int n_count = 0;				// 리스트 박스에 추가한 목록 갯수 저장 변수

	mn_device_name = -1;
	
	if (str_path.Right (1) != "\\")
        str_path += "\\";
	
	str_path += "*.*";
    
    if ((hFind = ::FindFirstFile ((LPCTSTR) str_path, &fd)) != INVALID_HANDLE_VALUE) 
	{
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			n_chk = AddItem(n_count, &fd);
			if (n_chk == TRUE)
				n_count++;
        }
		
        while (::FindNextFile (hFind, &fd)) 
		{
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				n_chk = AddItem(n_count, &fd);
				if (n_chk == FALSE) 
					break;
			}
			if (n_chk != CTL_READY)
				n_count++;
        }
		::FindClose(hFind);
    }
	
	return n_count;
}

BOOL CScreen_Basic::AddItem(int nIndex, WIN32_FIND_DATA *pfd)
{
	CString str_temp_dvc;
	CString str_device;
	CString str_expansion;
	int n_pos;
	
	HICON	hIcon = NULL;
	
	str_temp_dvc = (pfd->cFileName);	// 파일 이름 설정
	
	str_temp_dvc.MakeUpper();
	
	// **************************************************************************
	// 파일 이름 중 확장자 확인
	// **************************************************************************
	n_pos = str_temp_dvc.Find('.');		// 확장자 위치 설정
	
	if (n_pos == -1)					// 확장자 미존재
		str_expansion = "";
	else								// 확장자 존재
		str_expansion = str_temp_dvc.Mid(n_pos + 1);
	// **************************************************************************
	
	if (str_expansion != "TXT")
	{
		return CTL_READY;	
	}
	
	str_device = str_temp_dvc;			// 파일명 전체 화면 출력
	
	if (str_device == g_dataMgr.GetBackupData( &st_basic.mstr_device_name ) )
		mn_device_name = nIndex-1;
	// **************************************************************************
	
	hIcon = AfxGetApp()->LoadIcon(IDI_MODULE);
	mp_device_image_list.Add(hIcon);
	
    if (m_list_device_type.AddString(_T(str_device), nIndex - 1) == -1)
		return FALSE;
	
    return TRUE;
}

void CScreen_Basic::OnBtnDvcDelete() 
{
	int n_response;				// 대화 상자 리턴 플래그
	int n_existence;			// 파일 존재 유무 플래그
	CString str_file_name;
	CString str_selected_dvc;
	int n_pos;
	
	char chr_file_name[256];
	char chr_speed_file_name[256];
	char chr_teach_file_name[256];
	char chr_time_file_name[256];
	
	CDialog_Select  select_dlg;
	CDialog_Message msg_dlg;
	
	if (OnBasic_Input_Device_Check() == FALSE)  // 입력된 디바이스 이름 정상 유무 검사 함수
		return ;
	
	st_msg.mstr_confirm_msg = _T(mstr_temp_device + " Device Type Delete?");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		sprintf(chr_file_name, "%s", mstr_load_file);  // [폴더]+[파일명] 설정

		n_pos = mstr_load_file.Find(".");  // 확장자 검사
		
		if (n_pos == -1) 
		{
			sprintf(chr_speed_file_name, "%s_Speed.DAT", mstr_load_file);  // [폴더]+[파일명] 설정
			sprintf(chr_teach_file_name, "%s_Teach.DAT", mstr_load_file);  // [폴더]+[파일명] 설정
			sprintf(chr_time_file_name, "%s_Time.DAT", mstr_load_file);  // [폴더]+[파일명] 설정
		}
		else 
		{
			sprintf(chr_speed_file_name, "%s_Speed.DAT", mstr_load_file.Mid(0, n_pos));  // [폴더]+[파일명] 설정
			sprintf(chr_teach_file_name, "%s_Teach.DAT", mstr_load_file.Mid(0, n_pos));  // [폴더]+[파일명] 설정
			sprintf(chr_time_file_name, "%s_Time.DAT", mstr_load_file.Mid(0, n_pos));  // [폴더]+[파일명] 설정
		}
		
		n_existence = access(chr_file_name, 0) ;
		
		if (n_existence != -1)					// 파일 존재
		{
			DeleteFile(chr_file_name);			// 해당 파일 삭제
			DeleteFile(chr_speed_file_name);	// 해당 파일 삭제
			DeleteFile(chr_teach_file_name);	// 해당 파일 삭제
			DeleteFile(chr_time_file_name);		// 해당 파일 삭제
			
			// ******************************************************************
			// 폴더에 존재하는 파일 목록 리스트 박스에 재설정한다
			// ******************************************************************
			m_list_device_type.ResetContent();		// 리스트 박스의 모든 목록 삭제
			mp_device_image_list.DeleteImageList();	// 이미지 리스트에 설정된 정보 삭제
			
			OnBasic_Create_ImageList();				// 이미지 리스트 생성 함수
			
			// Associate image list to list box
			m_list_device_type.SetImageList(&mp_device_image_list);
			
			OnBasic_Init_List(st_path.mstr_path_dvc);// 파일 폴더 초기화 함수
			OnSelchangeListDeviceType();
			OnBasic_Device_Focus_Set();				// 선택된 디바이스명 포커스 설정 함수
			
			if (mn_device_name > -1)
			{
				m_list_device_type.GetText(mn_device_name, str_selected_dvc);
				
				OnBasic_Device_Display(str_selected_dvc);	// 선택된 디바이스 이름 출력 함수
			}
			// ******************************************************************
		}
		else  
			return ;
	}
	else if (n_response == IDCANCEL)
	{
	}
}

int CScreen_Basic::OnBasic_Input_Device_Check()
{
	CString str_chk_file;  // 생성할 [폴더]+[파일명] 저장 변수
	CString str_chk_ext;   // 확장자 저장 변수
	int n_pos;
	
	// **************************************************************************
    // 입력된 디바이스명 설정한다                                                
    // **************************************************************************
	((CEdit*)GetDlgItem(IDC_EDIT_DEVICE_TYPE))->GetWindowText(mstr_temp_device) ;
	mstr_temp_device.MakeUpper();
	mstr_temp_device.TrimLeft(' ');               
	mstr_temp_device.TrimRight(' ');
	
	if (mstr_temp_device.IsEmpty())  
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
//			st_msg.mstr_abnormal_msg = _T("[DEVICE] A name input error occurrence.");
			sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name input error occurrence.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return FALSE;
	}
	// **************************************************************************
	
	str_chk_file = st_path.mstr_path_dvc + mstr_temp_device;  // 생성할 [폴더]+[파일명] 설정
	n_pos = str_chk_file.Find(".");  // 확장자 검사
	if (n_pos == -1) 
		str_chk_file += _T(".TXT");
	else 
	{
		str_chk_ext = str_chk_file.Mid(n_pos);  // 확장자 정보 설정
		if (str_chk_ext != _T(".TXT"))  
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE] A name extension input error occurrence.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name extension input error occurrence.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			return FALSE;
		}
	}
	
	mstr_load_file = str_chk_file;  // 생성할 [폴더]+[파일명]+[확장자] 설정
	
	return TRUE;
}

void CScreen_Basic::OnBtnElectrostaticTest() 
{
	m_msg_electrostatic_1.SetWindowText(_T(""));
	m_msg_electrostatic_2.SetWindowText(_T(""));
	m_msg_electrostatic_3.SetWindowText(_T(""));
	m_msg_electrostatic_4.SetWindowText(_T(""));
	m_msg_electrostatic_5.SetWindowText(_T(""));
	
	::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 2, 0);
	
	m_btn_electostatic_test.EnableWindow(FALSE);
}

void CScreen_Basic::OnBasic_Electrostatic_Data_View(int nChannel)
{
	CString strData;
	switch (nChannel)
	{
	case 1:
		if (st_work.mdReadElectData[nChannel-1] == -99999)
		{
			strData = "OPEN";
		}
		else if (st_work.mdReadElectData[nChannel-1] == -99998)
		{
			strData = "OVER";
		}
		else
		{
			strData.Format("%0.3f", st_work.mdReadElectData[nChannel-1]);
		}
		m_msg_electrostatic_1.SetWindowText(_T(strData));
		break;
		
	case 2:
		if (st_work.mdReadElectData[nChannel-1] == -99999)
		{
			strData = "OPEN";
		}
		else if (st_work.mdReadElectData[nChannel-1] == -99998)
		{
			strData = "OVER";
		}
		else
		{
			strData.Format("%0.3f", st_work.mdReadElectData[nChannel-1]);
		}
		m_msg_electrostatic_2.SetWindowText(_T(strData));
		break;
		
	case 3:
		if (st_work.mdReadElectData[nChannel-1] == -99999)
		{
			strData = "OPEN";
		}
		else if (st_work.mdReadElectData[nChannel-1] == -99998)
		{
			strData = "OVER";
		}
		else
		{
			strData.Format("%0.3f", st_work.mdReadElectData[nChannel-1]);
		}
		m_msg_electrostatic_3.SetWindowText(_T(strData));
		break;
		
	case 4:
		if (st_work.mdReadElectData[nChannel-1] == -99999)
		{
			strData = "OPEN";
		}
		else if (st_work.mdReadElectData[nChannel-1] == -99998)
		{
			strData = "OVER";
		}
		else
		{
			strData.Format("%0.3f", st_work.mdReadElectData[nChannel-1]);
		}
		m_msg_electrostatic_4.SetWindowText(_T(strData));
		break;
		
	case 5:
		if (st_work.mdReadElectData[nChannel-1] == -99999)
		{
			strData = "OPEN";
		}
		else if (st_work.mdReadElectData[nChannel-1] == -99998)
		{
			strData = "OVER";
		}
		else
		{
			strData.Format("%0.3f", st_work.mdReadElectData[nChannel-1]);
		}
		m_msg_electrostatic_5.SetWindowText(_T(strData));
		m_btn_electostatic_test.EnableWindow(TRUE);
		break;
	}
}

LRESULT CScreen_Basic::OnBasic_Info_Apply(WPARAM wParam, LPARAM lParam)  
{
	if (wParam == 0)
	{
		g_dataMgr.CheckAndLogAll();
		OnBasic_Data_Apply();			// 베이식 화면 셋팅 정보 전역 변수에 전송 함수
		
		OnBasic_Data_Set();
		
		OnBasic_TitleBar_Info_Send();   // 타이틀 바 화면에 정보 출력 요청하는 함수
	}
	else if (wParam == 1)
	{
		OnBasic_Electrostatic_Data_View(lParam);
	}
	
	return 0;
}

void CScreen_Basic::OnBasic_Data_Apply()
{
	((CEdit*)GetDlgItem(IDC_EDIT_DEVICE_TYPE))->GetWindowText(mstr_temp_device);
	mstr_temp_device.MakeUpper();
	mstr_temp_device.TrimLeft(' ');               
	mstr_temp_device.TrimRight(' ');

	CString sText;
	CString strRecipename;
	m_combo_recipe.GetWindowText(sText);
	// 	g_dataMgr.SetDataTemp( &st_basic.mstr_revipe_name, sText );
	//2013,1216
	strRecipename = "AMT8520";
	if(sText == "" || sText == " " || sText.GetLength() < 2)
	{
		g_dataMgr.SetDataTemp( &st_basic.mstr_revipe_name, strRecipename );		
	}
	else
	{
		g_dataMgr.SetDataTemp( &st_basic.mstr_revipe_name, sText );		
	}
	
	////2015.0604
	CString strText;
	m_cbohifix_mode.GetWindowText(strText);
	
	CString strHifixname;
	strHifixname = "SATA";
	if(strText == "" || strText == " " || strText.GetLength() < 2)
	{
		g_dataMgr.SetDataTemp( &st_basic.n_str_hifix_mode, strHifixname );		
	}
	else
	{
		g_dataMgr.SetDataTemp( &st_basic.n_str_hifix_mode, strText );		
	}	
	////

	if (st_basic.mstr_device_name != mstr_temp_device)
	{
		if( g_lotMgr.GetLotCount() > 0 )
		{
			m_strTempMSG = "진행중인 랏이 있습니다.";
			if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = "There is a Running LOT.";
			DoModal_Msg( m_strTempMSG );
			OnBasic_Device_Focus_Set();
			return;
		}

		OnBasic_DeviceData_Apply();
		
		OnBasic_Data_Set();				// 전역 변수의 Data를 받아온다.


		OnBasic_Data_Change();
		////2015.0518
		m_cbohifix_mode.SetWindowText(st_basic.n_str_hifix_mode);
		////
		m_combo_recipe.SetWindowText(st_basic.mstr_revipe_name);		//2014,0808
		OnBasic_Data_Display();

		//2014,0310
		if( st_basic.mn_hotfix_pos <= 0 || st_basic.mn_hotfix_pos >= 6 )
		{
			st_msg.str_fallacy_msg = _T("Basica메뉴에서 HitFix 세팅값이 0이거나 6보다 큽니다.");
			if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("HitFix is 0 or more than 6 in Basica menu.");

			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[ADMIN]Basica메뉴에서 HitFix 세팅값이 0이거나 6보다 큽니다.");
				if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("HitFix is 0 or more than 6 in Basica menu.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}	
		}
////2015.0623
// 		else if( g_handler_cok.GetHifixPos() != st_basic.mn_hotfix_pos )
// 		{
// 			g_client_bpc.SetTestMode( "TestMode_HotFix" );
// 		}
//		g_client_bpc.SetTestMode( "TestMode_HotFix" );//2016.0223	
////
		if(st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES)
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "[JIG] 얼라인 버퍼가 있는지 체크해 주세요.");
				if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "[JIG] Is there a Align Buffer, Check Plaease.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			CDialog_Message msg_dlg;
			st_msg.str_fallacy_msg = _T("얼라인 버퍼가 있는지 체크해 주세요");			
			if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.str_fallacy_msg = _T("Is there a Align Buffer, Check Plaease.");	
			msg_dlg.DoModal();

		}
		if(st_basic.mn_time_out < 300)
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "[Time_Out] 타임아웃 시간:%d 체크해 주세요.",st_basic.mn_time_out);
				if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "[Time_Out] TimeOut : %d Check Please.",st_basic.mn_time_out);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			CDialog_Message msg_dlg;
			st_msg.str_fallacy_msg.Format("%s",st_msg.c_normal_msg);			
			msg_dlg.DoModal();
		}

		//2016.0709
		Func.LoadSocketSkipRule();

	}
	else
	{
		CString sText;
		m_combo_recipe.GetWindowText(sText);
		CString strRecipename;
		strRecipename = "AMT8520";
		if(sText == "" || sText == " " || sText.GetLength() < 2)
		{
			g_dataMgr.SetDataTemp( &st_basic.mstr_revipe_name, strRecipename );		
		}
		else
		{
			g_dataMgr.SetDataTemp( &st_basic.mstr_revipe_name, sText );		
		}
		
		////2015.0604
		CString strText;
		m_cbohifix_mode.GetWindowText(strText);
		
		CString strHifixname;
		strHifixname = "SATA";
		if(strText == "" || strText == " " || strText.GetLength() < 2)
		{
			g_dataMgr.SetDataTemp( &st_basic.n_str_hifix_mode, strHifixname );		
		}
		else
		{
			g_dataMgr.SetDataTemp( &st_basic.n_str_hifix_mode, strText );		
		}
		
// 		g_client_bpc.SetTestMode( "TestMode_HotFix" );//2015.0623//2016.0223			
		////

		if(st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig  == CTL_YES)
		{
			g_dataMgr.UpdateDatas();
		}
		else
		{
			g_dataMgr.UpdateDatas();
			if(st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig  == CTL_YES)
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "[JIG] 얼라인 버퍼가 있는지 체크해 주세요.");
					if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "[JIG] Is there a Align Buffer, Check Please.");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				CDialog_Message msg_dlg;
				st_msg.str_fallacy_msg = _T("얼라인 버퍼가 있는지 체크해 주세요");			
				if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.str_fallacy_msg = _T("Is there a Align Buffer, Check Please.");
				msg_dlg.DoModal();
			}
		}
		if(st_basic.mn_time_out < 300)
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "[Time_Out] 타임아웃 시간:%d 체크해 주세요.",st_basic.mn_time_out);
				if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "[Time_Out] TimeOut :%d Check Please.",st_basic.mn_time_out);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			CDialog_Message msg_dlg;
			st_msg.str_fallacy_msg.Format("%s",st_msg.c_normal_msg);			
			msg_dlg.DoModal();
		}

		//2016.0709
		//Func.LoadSocketSkipRule();

	}

	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
	}
}

void CScreen_Basic::OnBasic_TitleBar_Info_Send()
{
	if(st_handler.cwnd_title != NULL)
	{
		int nDeviceMode = g_dataMgr.GetBackupData( &st_basic.n_mode_device );
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, DEVICE_MODE, nDeviceMode );			// Device Mode
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, AOTO_TIMEOUT_MODE, 0);
	}
	
	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, DEVICE_INFO);
	}
}

void CScreen_Basic::OnBasic_DeviceData_Apply()
{
	int n_response = 0;  // 대화 상자 리턴 플래그
	int n_check;
	CString str_msg;
	
	n_check = OnBasic_Input_Data_Check();  // 입력 정보 정상 유무 검사 함수
	
	if (n_check == TRUE)  
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, DEVICE_INFO);  // 디바이스명 출력 요청
		
		
		g_dataMgr.CheckAndLogByPointer( &st_basic.mstr_device_name );
	}
	else if (n_check == FALSE)  
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
//			st_msg.mstr_abnormal_msg = _T("[LOT INFORMATION] An input device setting up error.");
			sprintf(st_msg.c_abnormal_msg, "[LOT INFORMATION] An input device setting up error.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return ;
	}
	
	g_dataMgr.UpdateData( &st_basic.mstr_device_name );
	
	// **********************************************************************
	// 디바이스 관련 화면 설정 데이터 로딩 부분                              
	// **********************************************************************
	mcls_basic.OnBasic_Data_Load();
	mcls_basic.On_Teach_Data_Load();
	// **********************************************************************
		
	st_work.mn_reinstatement_mode = 0;		// 복구 동작용 위치값을 다시 읽어라. 2K4/09/21/ViboX
	
	// **************************************************************************
	// 폴더에 존재하는 파일 목록 리스트 박스에 재설정한다                        
	// **************************************************************************
	m_list_device_type.ResetContent();			// 리스트 박스의 모든 목록 삭제
	mp_device_image_list.DeleteImageList();		// 이미지 리스트에 설정된 정보 삭제
	
	OnBasic_Create_ImageList();					// 이미지 리스트 생성 함수
	
	// Associate image list to list box
	m_list_device_type.SetImageList(&mp_device_image_list);
	
	OnBasic_Init_List(st_path.mstr_path_dvc);	// 파일 폴더 초기화 함수
	
	OnBasic_Device_Focus_Set();					// 선택된 디바이스명 포커스 설정 함수
	// **************************************************************************
	
	if (st_handler.cwnd_list != NULL)			// 리스트 바 화면 존재
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, DEVICE_INFO);  // 디바이스 종류 출력 요청
	
	//2013,0215
	OnBasic_GroupBox_Set();	
}

void CScreen_Basic::OnBasic_Data_Display()
{
	m_dgt_electrostatic_check_gap.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_electrostatic_check_gap ) );
	m_dgt_electrostatic_check_value.SetFloatVal((float)g_dataMgr.GetBackupData( &st_basic.md_electrostatic_value) );
	
	((CEdit*)GetDlgItem(IDC_EDIT_DEVICE_TYPE))->SetWindowText( g_dataMgr.GetBackupData( &st_basic.mstr_device_name ) );
	
	m_dgt_tray_x.SetValue( g_dataMgr.GetBackupData( &st_basic.n_tray_x ) );
	m_dgt_tray_y.SetValue( g_dataMgr.GetBackupData( &st_basic.n_tray_y ) );

	m_dgt_time_out.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_time_out ) );
	m_dgt_empty_tray_out.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_empty_tray_out ) );

	m_dgt_robot_init_time.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_robot_init_time ) );

	m_dgt_basic_manual_speed.SetValue(g_dataMgr.GetBackupData( &st_basic.nManualSpeed ));
	m_dgt_basic_run_speed.SetValue(g_dataMgr.GetBackupData( &st_basic.nRunSpeed ));

	//2013,0503
	m_dgt_hotfix_pos.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_hotfix_pos ) );
	//2013,0906
 	m_dgt_socket_max_count.SetValue( g_dataMgr.GetBackupData( &st_basic.mn_socket_max_count ) );

//	int recipe = -1;
//	int nCnt = m_combo_recipe.GetCount();
//	for (int i = 0; i < nCnt; i++)
//	{
//		CString	strVal,m_strData;
//		strVal.Format( "%s_%d", REG_VAL_ID, i );
//		m_combo_recipe.GetLBText(i, m_strData);
//
//		if(m_strData == st_basic.mstr_revipe_name)
//		{
//			recipe = i;
//		}
//		AMTRegistry::RegWriteString( REG_KEY_RECIPE, (LPCSTR)strVal, m_strData);
//	}
//
//	if( recipe == -1)
//		m_combo_recipe.SetCurSel(0);
//	else
//		m_combo_recipe.SetCurSel(recipe);
}

int CScreen_Basic::OnBasic_Input_Data_Check()
{
	CString str_chk_ext;   // 확장자 저장 변수
	int n_pos;
	
	/* ************************************************************************** */
    /* 입력된 디바이스명 설정한다                                                 */
    /* ************************************************************************** */
	((CEdit*)GetDlgItem(IDC_EDIT_DEVICE_TYPE))->GetWindowText(mstr_temp_device) ;
	mstr_temp_device.MakeUpper();
	mstr_temp_device.TrimLeft(' ');               
	mstr_temp_device.TrimRight(' ');
	
	if (mstr_temp_device.IsEmpty())  
	{
		if (st_handler.cwnd_list != NULL)			// 리스트 바 화면 존재
		{
//			st_msg.mstr_abnormal_msg = _T("[DEVICE] A name input error occurrence.");
			sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name input error occurrence.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return FALSE;
	}
	/* ************************************************************************** */
	
	n_pos = mstr_temp_device.Find(".");				// 확장자 검사
	if (n_pos == -1) 
		mstr_temp_device += _T(".TXT");
	else 
	{
		str_chk_ext = mstr_temp_device.Mid(n_pos);  // 확장자 정보 설정
		if (str_chk_ext != _T(".TXT"))  
		{
			if (st_handler.cwnd_list != NULL)		// 리스트 바 화면 존재
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE] A name extension input error occurrence.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE] A name extension input error occurrence.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			return FALSE;
		}
	}
	
	g_dataMgr.SetDataTemp( &st_basic.mstr_device_name, mstr_temp_device );
	/* ************************************************************************** */
	
	return TRUE;
}

void CScreen_Basic::OnBtnBasicApply() 
{

	CString str_selected_dvc;
	int nDeviceCur = m_list_device_type.GetCurSel();

	// 바꿀 수 없는 상황 고려하시오.

	int n_response;  // 대화 상자 리턴 플래그
	
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("Basic : Setting Data Apply?");
	
	n_response = select_dlg.DoModal();
	
	st_handler.m_nRemote = CTL_NO;
	if (n_response == IDOK)
	{
		g_dataMgr.CheckAndLogAll();
		//2013,0204
		mcls_basic.OnDaily_Count_Save();
		OnBasic_Data_Apply();			// 베이식 화면 셋팅 정보 전역 변수에 전송 함수
		
		OnBasic_Data_Set();
		
		OnBasic_TitleBar_Info_Send();    // 타이틀 바 화면에 정보 출력 요청하는 함수

		//2016.0201
		if( st_basic.mstr_device_name != st_handler.mstr_device_name)
		{
			g_client_bpc.SetTestMode( "TestMode_HotFix" );//2016.0223		
			st_handler.mstr_device_name = st_basic.mstr_device_name;
		}
		Func.OnSavePPID();//2013,1105

		//2016.0129
		::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1004, 0);//RECIPE DOWNLOAD


	}
	else if (n_response == IDCANCEL)
	{
		
	}
}

void CScreen_Basic::OnBtnBasicCancle() 
{
	int n_response;  // 대화 상자 리턴 플래그
	
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("Basic : Previous Data Recovery?");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		OnBasic_Data_Set();					// Recovery		// 베이식 화면 셋팅 정보 백업 받아놓은 변수로 복구 함수
		
		OnBasic_Data_Change();				// Data에 맞게 변경.
		
		OnBasic_Data_Display();
		
		OnBasic_TitleBar_Info_Send();		// 타이틀 바 화면에 정보 출력 요청하는 함수
	}
	else if (n_response == IDCANCEL)
	{
		
	}
}

void CScreen_Basic::OnDgtNetworkWaitTime() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_minute;
		
	mn_minute = m_dgt_network_wait_time.GetValue();
	
	mstr_temp = _T("Network 메세지 대기 시간 (초)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Network wait time (sec)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	
	mstr_temp = KeyPad.GetNumEditString_I(0, 100, mn_minute, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_network_wait_time, iTemp );
	m_dgt_network_wait_time.SetValue(iTemp);
}

void CScreen_Basic::OnDgtNetworkRetryCount() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	char chr_buf[20] ;
	int mn_minute;
	
	mn_minute = m_dgt_network_retry_cnt.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_minute, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("Network 메세지 재전송 횟수");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_keypad_msg = _T("Network retry count");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(1, 120, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_network_retry_cnt, iTemp );
	m_dgt_network_retry_cnt.SetValue( iTemp );
}

void CScreen_Basic::OnDgtAlarmDelayTime() 
{
	// TODO: Add your control notification handler code here
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	char chr_buf[20] ;
	int mn_Sec;
	
	mn_Sec = m_dgt_alarm_delay_time.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_Sec, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("Alarm Delay Time (Sec)");
    
	st_msg.mstr_keypad_val = mstr_temp;
		
	mstr_temp = KeyPad.GetNumEditString_I(1, 1000, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_alarm_delay_time, iTemp ); 
	m_dgt_alarm_delay_time.SetValue( iTemp );
}

void CScreen_Basic::OnBtnIpSaveEcserver() 
{
	
}

void CScreen_Basic::OnBtnPortSaveEcserver() 
{
	int n_response;	// 대화 상자에 대한 리턴 값 저장 변수
	CString str_temp;  // 저장할 정보 임시 저장 변수 
	
	CDialog_Message msg_dlg;
	
	if (st_handler.mn_level_teach != TRUE)
	{
		st_msg.str_fallacy_msg = _T("Level2 에서 동작 가능합니다.");
		if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.str_fallacy_msg = _T("Level2 is able to operate in");
		
		n_response = msg_dlg.DoModal();
		return;
	}
	
	CString strGetText;
	((CEdit*)GetDlgItem(IDC_EDIT_PORT_ECSERVER))->GetWindowText(strGetText);
	strGetText.TrimLeft(' ');	
	strGetText.TrimRight(' ');
	
	str_temp = strGetText;
	
	st_msg.mstr_keypad_msg = _T("EC Server port Setting");
	
	st_msg.mstr_keypad_val = str_temp;

	str_temp = KeyPad.GetNumEditString_I(0, 99999, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

//	g_dataMgr.SetDataTemp( Run_Network.m_Net_ECServer.GetPointer_Port(), atoi((LPCSTR)str_temp) );
//	((CEdit*)GetDlgItem(IDC_EDIT_PORT_ECSERVER))->SetWindowText(str_temp);
}

void CScreen_Basic::OnBtnConnectEcserver() 
{

}

void CScreen_Basic::OnBtnTestEcserver() 
{
	// TODO: Add your control notification handler code here
	
}

void CScreen_Basic::OnDgtTrayX() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	char chr_buf[20] ;
	int mn_x;
	
	mn_x = m_dgt_tray_x.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_x, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("Tray X Size [EA]");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(1, 5, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.n_tray_x, iTemp );
	m_dgt_tray_x.SetValue( iTemp );
}

void CScreen_Basic::OnDgtTrayY() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	char chr_buf[20] ;
	int mn_y;
	
	mn_y = m_dgt_tray_y.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_y, chr_buf, 10));
	
	st_msg.mstr_keypad_msg = _T("Tray Y Size [EA]");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(1, 5, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.n_tray_y, iTemp );
	m_dgt_tray_y.SetValue( iTemp );
}

void CScreen_Basic::OnRadioCok0() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 0 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)0 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioCok1() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 1 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)1 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioCok2() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 2 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)2 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioCok3() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 3 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)3 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioCok4() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 4 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)4 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioCok5() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_cok_type ) == 5 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_cok_type, (int)5 );
	OnBasic_CokType_Change();
}

void CScreen_Basic::OnRadioDegree0() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_degree ) == EDEG_0 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_degree, (int)EDEG_0 );
	OnBasic_Degree_Change();
}

void CScreen_Basic::OnBasic_Degree_Change()
{
 	int nDegree = g_dataMgr.GetBackupData( &st_basic.n_degree );
 	if( nDegree < EDEG_0 || nDegree >= MAX_EDEG )
 	{
 		nDegree = EDEG_0;
 		g_dataMgr.SetDataTemp( &st_basic.n_degree, nDegree );
 	}
 
 	m_radio_degree_0.SetCheck( FALSE );
	m_radio_degree_90.SetCheck( FALSE );
	m_radio_degree_180.SetCheck( FALSE );
	m_radio_degree_270.SetCheck( FALSE );

	switch( nDegree )
	{
	case EDEG_0:	m_radio_degree_0.SetCheck( TRUE );		break;
	case EDEG_90:	m_radio_degree_90.SetCheck( TRUE );		break;
	case EDEG_180:	m_radio_degree_180.SetCheck( TRUE );	break;
	case EDEG_270:	m_radio_degree_270.SetCheck( TRUE );	break;
 	}
}

void CScreen_Basic::OnRadioDegree90() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_degree ) == EDEG_90 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_degree, (int)EDEG_90 );
	OnBasic_Degree_Change();
}

void CScreen_Basic::OnRadioDegree180() 
{
	// TODO: Add your control notification handler code here
	if( g_dataMgr.GetBackupData( &st_basic.n_degree ) == EDEG_180 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_degree, (int)EDEG_180 );
	OnBasic_Degree_Change();
}

void CScreen_Basic::OnRadioDegree270() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_degree ) == EDEG_270 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_degree, (int)EDEG_270 );
	OnBasic_Degree_Change();
}

void CScreen_Basic::OnRadioPt1v1mUse4() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_pick_type ) == EPT_1V1M_4USE )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_1V1M_4USE );
	OnBasic_PickType_Change();
}

void CScreen_Basic::OnRadio1v1mUse2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_pick_type ) == EPT_1V1M_2USE )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_1V1M_2USE );
	OnBasic_PickType_Change();
}

void CScreen_Basic::OnRadio2v1mUse2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_pick_type ) == EPT_2V1M_2USE )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_2V1M_2USE );
	//g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_1V1M_2USE );
	OnBasic_PickType_Change();
}

void CScreen_Basic::OnRadioPt1v1mUse42() //2013,1031
{
	if( g_dataMgr.GetBackupData( &st_basic.n_pick_type ) == EPT_1V1M_4USE_2 )
		return;

	g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_1V1M_4USE_2 );
	//g_dataMgr.SetDataTemp( &st_basic.n_pick_type, (int)EPT_1V1M_2USE );
	OnBasic_PickType_Change();	
}


void CScreen_Basic::OnBasic_PickType_Change()
{
	int nPickType = g_dataMgr.GetBackupData( &st_basic.n_pick_type );
 	if( nPickType < EPT_1V1M_4USE || nPickType >= MAX_EPT )
 	{
 		nPickType = EPT_1V1M_4USE;
 		g_dataMgr.SetDataTemp( &st_basic.n_pick_type, nPickType );
 	}
 
	m_radio_pt_1v1m_4use.SetCheck( FALSE );
	m_radio_pt_1v1m_2use.SetCheck( FALSE );
	m_radio_pt_2v1m_2use.SetCheck( FALSE );
	m_radio_pt_1v1m_4use_2.SetCheck( FALSE ); //2013,1031
	
	switch( nPickType )
	{
	case EPT_1V1M_4USE:	m_radio_pt_1v1m_4use.SetCheck( TRUE );		break;
	case EPT_1V1M_2USE:	m_radio_pt_1v1m_2use.SetCheck( TRUE );		break;
 	case EPT_2V1M_2USE:	m_radio_pt_2v1m_2use.SetCheck( TRUE );		break;
	case EPT_1V1M_4USE_2: m_radio_pt_1v1m_4use_2.SetCheck( TRUE );	break;
	}
}

void CScreen_Basic::OnBasic_RunMode_Change()
{
	int nRunMode = g_dataMgr.GetBackupData( &st_basic.mn_mode_run );
	if (nRunMode < 0 || nRunMode > 1)
	{
		nRunMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.mn_mode_run, (int)0 );
	}
	
	switch (nRunMode)
	{
	case 0:
		m_radio_online_mode.SetCheck(FALSE);
		m_radio_offline_mode.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_online_mode.SetCheck(TRUE);
		m_radio_offline_mode.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnRadioOnlineMode() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_run ) == CTL_YES )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_run, (int)CTL_YES );
	OnBasic_RunMode_Change();
}

void CScreen_Basic::OnRadioOfflineMode() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_run ) == CTL_NO )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_run, (int)CTL_NO );
	OnBasic_RunMode_Change();
}

void CScreen_Basic::OnDgtTimeOut() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
		
	mn_sec = m_dgt_time_out.GetValue();

	m_dgt_time_out.SetFocus();
	
	mstr_temp = _T("테스트 타임아웃 (초)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Test Time Out (sec)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(0, 9999, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_time_out, iTemp );
	m_dgt_time_out.SetValue(iTemp);
}

void CScreen_Basic::OnDgtEmpty() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
		
	mn_sec = m_dgt_empty_tray_out.GetValue();
	
	mstr_temp = _T("빈트레이 아웃 개수(개)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Empty Tray out (num)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(0, 2, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_empty_tray_out, iTemp );
	m_dgt_empty_tray_out.SetValue(iTemp);

	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
	}
}

void CScreen_Basic::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	COLORREF bk_old, tk_old;
	Grid->GetColor(col, row, &bk_old, &tk_old);
	if( bk_old != bk || tk_old != tk )
	{
		Grid->SetColor(col, row, bk, tk);
		Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Basic::GridControl(UINT nID, int type, int row, int col, int pos)
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
			Grid->SetTypeNumberEx(&CellType,0, 0, 0, 9999,0,".",",",0,0,1,0,1.001);
			break;
	}
	
	Grid->SetCellType(col,row,&CellType);

	Grid = NULL;
	delete Grid;
}

void CScreen_Basic::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);
	if( data == szBuf )
	{
		Grid = NULL;
		delete Grid;
		return;
	}

	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

CString CScreen_Basic::GetGridData( UINT nID, int row, int col )
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);

	Grid = NULL;
	delete Grid;

	return szBuf;
}


void CScreen_Basic::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CScreen_Basic::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CScreen_Basic::OnBasic_Init_TrayOffset( ENUM_MODULE_POSITION empos )
{
		TSpread *Grid;
	CString str_pos, str_temp;
	int row = 20;
	int col = 3;
	int i, j;

	int iGrid = IDC_CUSTOM_TRAY_OFFSET_1;
	if( empos == MPOS_INDEX_BIN2 )		iGrid = IDC_CUSTOM_TRAY_OFFSET_2;
	else if( empos == MPOS_INDEX_BIN3 )		iGrid = IDC_CUSTOM_TRAY_OFFSET_2_1;//ybs
	else if( empos == MPOS_INDEX_REJ )	iGrid = IDC_CUSTOM_TRAY_OFFSET_3;
	
	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, TRUE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, TRUE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for (i = 0; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 20);
			
			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 32);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 52);
			}
			
			if (i == 0 && j == 1)
			{
				GridColor(iGrid, i, j, ORANGE_C, WHITE_C);
			}
			else if (i == 0 || j == 1)
			{
				GridColor(iGrid, i, j, PINK_D, BLACK_C);
			}
			else
			{
				if (j == 4)
				{
					GridColor(iGrid, i, j, BLACK_C, RED_C);
				}
				else
				{
					GridColor(iGrid, i, j, BLACK_C, GREEN_B);
				}
			}
			GridControl(iGrid, STATIC, i, j, 0);
			GridFont(iGrid, i, j, 15);
		}
	}
	
	GridData(iGrid, 0, 1, "Index");
	GridData(iGrid, 0, 2, "X");
	GridData(iGrid, 0, 3, "Y");
	
	for (i = 1; i < row + 1; i++)
	{
		str_temp.Format("%d", i);
		GridData(iGrid, i, 1, str_temp);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Basic::OnBasic_TrayOffset_Change( ENUM_MODULE_POSITION empos )
{
	int iGrid = IDC_CUSTOM_TRAY_OFFSET_1;
	if( empos == MPOS_INDEX_BIN2 )		iGrid = IDC_CUSTOM_TRAY_OFFSET_2;
	else if( empos == MPOS_INDEX_BIN3 )		iGrid = IDC_CUSTOM_TRAY_OFFSET_2_1;//ybs
	else if( empos == MPOS_INDEX_REJ )	iGrid = IDC_CUSTOM_TRAY_OFFSET_3;

	for( int i=0; i<MAX_TRAYCELL; i++ )
	{
		if( empos == MPOS_INDEX_BIN1 )
		{
			CString strTmp;
			double dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_1_x[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_1_x[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 2, strTmp );

			dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_1_y[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_1_y[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 3, strTmp );
		}
		else if( empos == MPOS_INDEX_BIN2 )
		{
			CString strTmp;
			double dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_2_x[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_2_x[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 2, strTmp );

			dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_2_y[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_2_y[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 3, strTmp );
		}
		else if( empos == MPOS_INDEX_BIN3 )//ybs
		{
			CString strTmp;
			double dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_3_x[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_3_x[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 2, strTmp );

			dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_3_y[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_3_y[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 3, strTmp );
		}
		else// if( empos == EMPOS_INDEX_REJ )
		{
			CString strTmp;
			double dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_rej_x[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_rej_x[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 2, strTmp );

			dVal = g_dataMgr.GetBackupData( &st_basic.md_tray_offset_rej_y[i] );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_rej_y[i], dVal );
			strTmp.Format( "%.03f", dVal );
			GridData( iGrid, i + 1, 3, strTmp );
		}
	}
}

void CScreen_Basic::OnCellClick( WPARAM wParam, LPARAM lParam )
{
	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	switch( wParam )
	{
	case IDC_CUSTOM_TRAY_OFFSET_1:
		if( lpcc->Col == 1 )
			break;

		OnClickTrayOffset( MPOS_INDEX_BIN1, lpcc->Col - 2, lpcc->Row - 1 );
		break;

	case IDC_CUSTOM_TRAY_OFFSET_2:
		if( lpcc->Col == 1 )
			break;

		OnClickTrayOffset( MPOS_INDEX_BIN2, lpcc->Col - 2, lpcc->Row - 1 );
		break;

	case IDC_CUSTOM_TRAY_OFFSET_2_1://ybs
		if( lpcc->Col == 1 )
			break;

		OnClickTrayOffset( MPOS_INDEX_BIN3, lpcc->Col - 2, lpcc->Row - 1 );
		break;
	case IDC_CUSTOM_TRAY_OFFSET_3:
		if( lpcc->Col == 1 )
			break;

		OnClickTrayOffset( MPOS_INDEX_REJ, lpcc->Col - 2, lpcc->Row - 1 );
		break;
	}
}

void CScreen_Basic::OnClickTrayOffset( ENUM_MODULE_POSITION empos, int nx, int ny )
{
	CString strTray = "1";
	if( empos == MPOS_INDEX_BIN2 )	strTray = "2";
	else if( empos == MPOS_INDEX_BIN3 )	strTray = "3"; //ybs
	else if( empos == MPOS_INDEX_REJ )	strTray = "REJ";

	CString strXY = "X";
	if( nx == 1 )	strXY = "Y";
	
	
	CString strText;
	st_msg.mstr_keypad_msg.Format("[%s Tray Offset (%s, %d)]", strTray, strXY, ny + 1);

	switch( empos )
	{
	case MPOS_INDEX_BIN1:
		if( nx == 0 )
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_1_x[ny] ), st_msg.mstr_keypad_msg);
			//st_basic.md_tray_offset_1_x[ny] = atof( strText );
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_1_x[ny], atof(strText) );
		}
		else
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_1_y[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_1_y[ny], atof(strText) );
		}
		OnBasic_TrayOffset_Change(MPOS_INDEX_BIN1);
		break;

	case MPOS_INDEX_BIN2:
		if( nx == 0 )
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_2_x[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_2_x[ny], atof(strText) );
		}
		else
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_2_y[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_2_y[ny], atof(strText) );
		}
		OnBasic_TrayOffset_Change(MPOS_INDEX_BIN2);
		break;

	case MPOS_INDEX_BIN3://ybs
		if( nx == 0 )
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_3_x[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_3_x[ny], atof(strText) );
		}
		else
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_3_y[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_3_y[ny], atof(strText) );
		}
		OnBasic_TrayOffset_Change(MPOS_INDEX_BIN3);
		break;

	case MPOS_INDEX_REJ:
		if( nx == 0 )
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_rej_x[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_rej_x[ny], atof(strText) );
		}
		else
		{
			strText = KeyPad.GetNumEditString_D(-9999, 9999, g_dataMgr.GetBackupData( &st_basic.md_tray_offset_rej_y[ny] ), st_msg.mstr_keypad_msg);
			g_dataMgr.SetDataTemp( &st_basic.md_tray_offset_rej_y[ny], atof(strText) );
		}
		OnBasic_TrayOffset_Change(MPOS_INDEX_REJ);
		break;
	}
	
	

}

void CScreen_Basic::OnRadioBypassFront() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_bypass_mode ) == BYPASS_FRONT )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_bypass_mode, (int)BYPASS_FRONT );
	OnBasic_ByPassMode_Change();
}

void CScreen_Basic::OnRadioBypassRear() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_bypass_mode ) == BYPASS_REAR )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_bypass_mode, (int)BYPASS_REAR );
	OnBasic_ByPassMode_Change();
}

void CScreen_Basic::OnRadioBypassAll() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_bypass_mode ) == BYPASS_ALL )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_bypass_mode, (int)BYPASS_ALL );
	OnBasic_ByPassMode_Change();
}

void CScreen_Basic::OnRadioBypassNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_bypass_mode ) == BYPASS_NONE )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_bypass_mode, (int)BYPASS_NONE );
	OnBasic_ByPassMode_Change();
}

void CScreen_Basic::OnBasic_ByPassMode_Change()
{
	int nBypassMode = g_dataMgr.GetBackupData( &st_basic.mn_bypass_mode );
	if (nBypassMode < BYPASS_NONE || nBypassMode > BYPASS_ALL )
	{
		nBypassMode = BYPASS_NONE;
		g_dataMgr.SetDataTemp( &st_basic.mn_bypass_mode, nBypassMode );
	}
	
	switch (nBypassMode)
	{
	case BYPASS_NONE:
		m_radio_bypass_notuse.SetCheck( TRUE );
		m_radio_bypass_front.SetCheck(FALSE);
		m_radio_bypass_rear.SetCheck(FALSE);
		m_radio_bypass_all.SetCheck(FALSE);
		break;
		
	case BYPASS_FRONT:
		m_radio_bypass_notuse.SetCheck(FALSE);
		m_radio_bypass_front.SetCheck(TRUE);
		m_radio_bypass_rear.SetCheck(FALSE);
		m_radio_bypass_all.SetCheck(FALSE);
		break;

	case BYPASS_REAR:
		m_radio_bypass_notuse.SetCheck(FALSE);
		m_radio_bypass_front.SetCheck(FALSE);
		m_radio_bypass_rear.SetCheck(TRUE);
		m_radio_bypass_all.SetCheck(FALSE);
		break;

	case BYPASS_ALL:
		m_radio_bypass_notuse.SetCheck(FALSE);
		m_radio_bypass_front.SetCheck(FALSE);
		m_radio_bypass_rear.SetCheck(FALSE);
		m_radio_bypass_all.SetCheck(TRUE);
		break;
	}
}

void CScreen_Basic::OnShow_Bypass_Box()
{
	if( g_handler.GetMachinePos() != EMPOS_FRONT )
	{
		GetDlgItem( IDC_GROUP_BYPASS )->ShowWindow( FALSE );
		GetDlgItem( IDC_RADIO_BYPASS_NOTUSE )->ShowWindow( FALSE );
		GetDlgItem( IDC_RADIO_BYPASS_FRONT )->ShowWindow( FALSE );
		GetDlgItem( IDC_RADIO_BYPASS_REAR )->ShowWindow( FALSE );
		GetDlgItem( IDC_RADIO_BYPASS_ALL )->ShowWindow( FALSE );
	}
}


void CScreen_Basic::OnRadioPogoMode() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_pogo ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_pogo, (int)CTL_YES );
	OnBasic_PogoMode_Change();	
	
}

void CScreen_Basic::OnRadioPogoNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_pogo ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_pogo, (int)CTL_NO );
	OnBasic_PogoMode_Change();	
	
}

void CScreen_Basic::OnBasic_PogoMode_Change()
{
	int nPogoMode = g_dataMgr.GetBackupData( &st_basic.n_mode_pogo );
	if (nPogoMode < 0 || nPogoMode > 1)
	{
		nPogoMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_pogo, (int)0 );
	}
	
	switch (nPogoMode)
	{
	case 0:
		m_radio_pogo_mode.SetCheck(FALSE);
		m_radio_pogo_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_pogo_mode.SetCheck(TRUE);
		m_radio_pogo_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnRadioJigMode() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_jig ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_jig, (int)CTL_YES );
	OnBasic_JigMode_Change();	
	
}

void CScreen_Basic::OnRadioJigNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_jig ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_jig, (int)CTL_NO );
	OnBasic_JigMode_Change();		
}

void CScreen_Basic::OnBasic_JigMode_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_jig );
	if (nJigMode < 0 || nJigMode > 1)
	{
		nJigMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_jig, (int)0 );
	}
	
	switch (nJigMode)
	{
	case 0:
		m_radio_jig_use.SetCheck(FALSE);
		m_radio_jig_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_jig_use.SetCheck(TRUE);
		m_radio_jig_notuse.SetCheck(FALSE);
		break;
	}
}


void CScreen_Basic::OnRadioRecoveryMode() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_recovery ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_recovery, (int)CTL_YES );
	OnBasic_RecoveryMode_Change();	
}

void CScreen_Basic::OnRadioRecoveryNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_recovery ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_recovery, (int)CTL_NO );
	OnBasic_RecoveryMode_Change();	
	
}

void CScreen_Basic::OnBasic_RecoveryMode_Change()
{
	int nRecoveryMode = g_dataMgr.GetBackupData( &st_basic.n_mode_recovery );
	if (nRecoveryMode < 0 || nRecoveryMode > 1)
	{
		nRecoveryMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_recovery, (int)0 );
	}
	
	switch (nRecoveryMode)
	{
	case 0:
		m_radio_recovery_use.SetCheck(FALSE);
		m_radio_recovery_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_recovery_use.SetCheck(TRUE);
		m_radio_recovery_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnDgtRobotInitTime() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
	
	mn_sec = m_dgt_robot_init_time.GetValue();
	
	mstr_temp = _T("XYZ 로봇 초기화 시간(hour)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("XYZ Robot Init Time (hour)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(0, 24, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_robot_init_time, iTemp );
	m_dgt_robot_init_time.SetValue(iTemp);		
}

void CScreen_Basic::OnDgtRunSpeed() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
	
	mn_sec = m_dgt_basic_run_speed.GetValue();
	
	mstr_temp = _T("로봇 Run 동작 속도(%)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Robot Run Speed (%)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(1, 100, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.nRunSpeed, iTemp );
	m_dgt_basic_run_speed.SetValue(iTemp);		
}

void CScreen_Basic::OnDgtManualSpeed() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
	
	mn_sec = m_dgt_basic_manual_speed.GetValue();
	
	mstr_temp = _T("로봇 Manual 동작 속도(%)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Robot Manual Speed (%)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(1, 100, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.nManualSpeed, iTemp );
	m_dgt_basic_manual_speed.SetValue(iTemp);		
}

void CScreen_Basic::OnDgtHotfixPos() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
	
	mn_sec = m_dgt_hotfix_pos.GetValue();
	
	mstr_temp = _T("Hit Fix 위치(pos)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Hit Fix(pos)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(1, 5, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_hotfix_pos, iTemp );
	m_dgt_hotfix_pos.SetValue(iTemp);
	
	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
	}	
}

#include "Dialog_Pass_Check.h"
void CScreen_Basic::OnDgtSocketMaxCount() 
{
	//2013,0906
 	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
 	char chr_buf[20] ;
 	int mn_Sec;

	


	int nResponse;								// 대화 상자에 대한 리턴 값 저장 변수
	
	CDialog_Pass_Check pass_chk_dlg ;			// 암호 검사 대화 상자 클래스 변수 선언 
	
	st_handler.mstr_pass_level = _T("MAX_COUNT");		// 암호 레벨 정보 설정 
	
	nResponse = pass_chk_dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		st_handler.mn_system_lock = FALSE;	// 시스템 잠금 상태 해제
		
		//:: WritePrivateProfileString("Password", "MAX_COUNT", LPCTSTR("NO"), st_path.mstr_basic);
		
		if (st_handler.cwnd_list != NULL)
		{
			//			st_msg.mstr_normal_msg = "System Unlocking.";
			sprintf(st_msg.c_normal_msg, "Max count Unlocking.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}		
	}
	else if (nResponse == IDCANCEL)
	{
		return;
	}

 	
 	mn_Sec = m_dgt_socket_max_count.GetValue();
 	mstr_temp = LPCTSTR(_itoa(mn_Sec, chr_buf, 10));
 	
 	st_msg.mstr_keypad_msg = _T("Socket Max Count");
     
 	st_msg.mstr_keypad_val = mstr_temp;
 	
 	mstr_temp = KeyPad.GetNumEditString_I(1, 1000000, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);
 	
 	int iTemp = atoi(mstr_temp);
 	g_dataMgr.SetDataTemp( &st_basic.mn_socket_max_count, iTemp ); 
 	m_dgt_socket_max_count.SetValue( iTemp );		
}


void CScreen_Basic::OnRadioUseXgem() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_xgem ) == CTL_YES )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_xgem, (int)CTL_YES );
	OnBasic_XgemMode_Change();	
	
}

void CScreen_Basic::OnRadioNotuseXgem() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_xgem ) == CTL_NO )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_xgem, (int)CTL_NO );
	OnBasic_XgemMode_Change();	
	
}

void CScreen_Basic::OnRadioUseRcmd() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_rcmd ) == CTL_YES )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_rcmd, (int)CTL_YES );
	OnBasic_RcmdMode_Change();		
	
}

void CScreen_Basic::OnRadioNotuseRcmd() 
{
	if( g_dataMgr.GetBackupData( &st_basic.mn_mode_rcmd ) == CTL_NO )
		return;

	g_dataMgr.SetDataTemp( &st_basic.mn_mode_rcmd, (int)CTL_NO );
	OnBasic_RcmdMode_Change();		
	
}

void CScreen_Basic::OnBasic_XgemMode_Change()
{
	int nXgemMode = g_dataMgr.GetBackupData( &st_basic.mn_mode_xgem );
	if (nXgemMode < 0 || nXgemMode > 1)
	{
		nXgemMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.mn_mode_xgem, (int)0 );
	}

	//2013,1216
	st_basic.m_bFDC_Data = st_basic.mn_mode_xgem;
	
	switch (nXgemMode)
	{
	case 0:
		m_radio_use_xgem.SetCheck(FALSE);
		m_radio_notuse_xgem.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_use_xgem.SetCheck(TRUE);
		m_radio_notuse_xgem.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnBasic_RcmdMode_Change()
{
	int nRcmdMode = g_dataMgr.GetBackupData( &st_basic.mn_mode_rcmd );
	if (nRcmdMode < 0 || nRcmdMode > 1)
	{
		nRcmdMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.mn_mode_rcmd, (int)0 );
	}
	
	switch (nRcmdMode)
	{
	case 0:
		m_radio_use_rcmd.SetCheck(FALSE);
		m_radio_notuse_rcmd.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_use_rcmd.SetCheck(TRUE);
		m_radio_notuse_rcmd.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnSelchangeComboAddRecipe() 
{
	CString sText;
	m_combo_recipe.GetWindowText(sText);
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_device, sText );	
}

void CScreen_Basic::OnBasic_ProductMode_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_product );
	if (nJigMode < 0 || nJigMode > 1)
	{
		nJigMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_product, (int)0 );
	}
	
	switch (nJigMode)
	{
	case 0:
		m_radio_product_use.SetCheck(FALSE);
		m_radio_product_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_product_use.SetCheck(TRUE);
		m_radio_product_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnRadioUseProduct() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product, (int)CTL_YES );
	OnBasic_ProductMode_Change();	
	
}

void CScreen_Basic::OnRadioNotuseProduct() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product, (int)CTL_NO );
	OnBasic_ProductMode_Change();		
}

void CScreen_Basic::OnBasic_ProductSite1Mode_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1 );
	if (nJigMode < 0 || nJigMode > 1)
	{
		nJigMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1, (int)0 );
	}
	
	switch (nJigMode)
	{
	case 0:
		m_radio_site1_use.SetCheck(FALSE);
		m_radio_site1_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_site1_use.SetCheck(TRUE);
		m_radio_site1_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnRadioUseSite1() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1 ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1, (int)CTL_YES );
	OnBasic_ProductSite1Mode_Change();	
	
}

void CScreen_Basic::OnRadioNotuseSite1() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1 ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1, (int)CTL_NO );
	OnBasic_ProductSite1Mode_Change();	
	
}

void CScreen_Basic::OnBasic_ProductSite2Mode_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2 );
	if (nJigMode < 0 || nJigMode > 1)
	{
		nJigMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2, (int)0 );
	}
	
	switch (nJigMode)
	{
	case 0:
		m_radio_site2_use.SetCheck(FALSE);
		m_radio_site2_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_site2_use.SetCheck(TRUE);
		m_radio_site2_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnRadioUseSite2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2 ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2, (int)CTL_YES );
	OnBasic_ProductSite2Mode_Change();	
	
}

void CScreen_Basic::OnRadioNotuseSite2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2 ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2, (int)CTL_NO );
	OnBasic_ProductSite2Mode_Change();	
	
}

void CScreen_Basic::OnBasic_ProductSite1Use_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1_mode );
	if (nJigMode < 0 || nJigMode > 3)
	{
		nJigMode = 3;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2_mode, (int)3 );
	}
	
	switch (nJigMode)
	{
	case 1:
		m_radio_site1_1.SetCheck(TRUE);
		m_radio_site1_2.SetCheck(FALSE);
		m_radio_site1_both.SetCheck(FALSE);
		break;
		
	case 2:
		m_radio_site1_1.SetCheck(FALSE);
		m_radio_site1_2.SetCheck(TRUE);
		m_radio_site1_both.SetCheck(FALSE);
		break;
		
	case 3:
		m_radio_site1_1.SetCheck(FALSE);
		m_radio_site1_2.SetCheck(FALSE);
		m_radio_site1_both.SetCheck(TRUE);
		break;
	}
}

void CScreen_Basic::OnRadio1() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1_mode ) == 1 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1_mode, (int)1 );
	OnBasic_ProductSite1Use_Change();		
}

void CScreen_Basic::OnRadio2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1_mode ) == 2 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1_mode, (int)2 );
	OnBasic_ProductSite1Use_Change();	
}

void CScreen_Basic::OnRadioBoth() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site1_mode ) == 3 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site1_mode, (int)3 );
	OnBasic_ProductSite1Use_Change();		
	
}

void CScreen_Basic::OnBasic_ProductSite2Use_Change()
{
	int nJigMode = g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2_mode );
	if (nJigMode < 0 || nJigMode > 3)
	{
		nJigMode = 3;
		g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2_mode, (int)3 );
	}
	
	switch (nJigMode)
	{
	case 1:
		m_radio_site2_1.SetCheck(TRUE);
		m_radio_site2_2.SetCheck(FALSE);
		m_radio_site2_both.SetCheck(FALSE);
		break;
		
	case 2:
		m_radio_site2_1.SetCheck(FALSE);
		m_radio_site2_2.SetCheck(TRUE);
		m_radio_site2_both.SetCheck(FALSE);
		break;

	case 3:
		m_radio_site2_1.SetCheck(FALSE);
		m_radio_site2_2.SetCheck(FALSE);
		m_radio_site2_both.SetCheck(TRUE);
		break;
	}
	
}

void CScreen_Basic::OnRadio21() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2_mode ) == 1 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2_mode, (int)1 );
	OnBasic_ProductSite1Use_Change();		
	
}

void CScreen_Basic::OnRadio22() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2_mode ) == 2 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2_mode, (int)2 );
	OnBasic_ProductSite2Use_Change();		
	
}

void CScreen_Basic::OnRadioBoth2() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_mode_product_site2_mode ) == 3 )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_mode_product_site2_mode, (int)3 );
	OnBasic_ProductSite2Use_Change();		
	
}

////2015.0518
void CScreen_Basic::OnSelchangeComboHifixMode() 
{
	// TODO: Add your control notification handler code here
	CString sText;
	m_cbohifix_mode.GetWindowText(sText);
	g_dataMgr.SetDataTemp( &st_basic.n_str_hifix_mode, sText );	
	
}

void CScreen_Basic::Loadhifixmode()
{
	Func.Load_hifix_mode_Data();
	for( int i=0; i< st_handler.m_nhifix_mode_cnt; i++ )
	{
		if(st_handler.mstr_hifix_mode[i] != "")
		{
			m_cbohifix_mode.AddString(st_handler.mstr_hifix_mode[i]);
		}
	}
	m_cbohifix_mode.SetWindowText(st_basic.n_str_hifix_mode);
}

void CScreen_Basic::OnBasic_Hifix_Set()
{
	m_cbohifix_mode.GetWindowText(st_basic.n_str_hifix_mode);
	st_handler.mstr_hifix_mode[0] = st_basic.n_str_hifix_mode;	
}
////

////2015.0914
void CScreen_Basic::OnRadioRetestJigUse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_retest_jig ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_retest_jig, (int)CTL_YES );
	OnBasic_Retest_JigMode_Change();	
	
}

void CScreen_Basic::OnRadioRetestJigNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_retest_jig ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_retest_jig, (int)CTL_NO );
	OnBasic_Retest_JigMode_Change();		
}

void CScreen_Basic::OnBasic_Retest_JigMode_Change()
{
	int nRetestJigMode = g_dataMgr.GetBackupData( &st_basic.n_retest_jig );
	if (nRetestJigMode < 0 || nRetestJigMode > 1)
	{
		nRetestJigMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_retest_jig, (int)0 );
	}
	
	switch (nRetestJigMode)
	{
	case 0:
		m_radio_retest_jig_use.SetCheck(FALSE);
		m_radio_retest_jig_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_retest_jig_use.SetCheck(TRUE);
		m_radio_retest_jig_notuse.SetCheck(FALSE);
		break;
	}
}
////


void CScreen_Basic::OnRadioRecipeUse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_recipe_mode ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_recipe_mode, (int)CTL_YES );
	OnBasic_Recipe_Mode_Change();	
}

void CScreen_Basic::OnRadioRecipeNotuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_recipe_mode ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_recipe_mode, (int)CTL_NO );
	OnBasic_Recipe_Mode_Change();	
}

void CScreen_Basic::OnBasic_Recipe_Mode_Change()
{
	int nRecipeMode = g_dataMgr.GetBackupData( &st_basic.n_recipe_mode );
	if (nRecipeMode < 0 || nRecipeMode > 1)
	{
		nRecipeMode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_recipe_mode, (int)0 );
	}
	
	switch (nRecipeMode)
	{
	case 0:
		m_radio_recipe_use.SetCheck(FALSE);
		m_radio_recipe_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_recipe_use.SetCheck(TRUE);
		m_radio_recipe_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnBasic_CSerial2Mode_Change()
{
	int nCserial2Mode = g_dataMgr.GetBackupData( &st_basic.n_cserial2_mode );
	if (nCserial2Mode < 0 || nCserial2Mode > 1)
	{
		nCserial2Mode = 0;
		g_dataMgr.SetDataTemp( &st_basic.n_cserial2_mode, (int)0 );
	}
	
	switch (nCserial2Mode)
	{
	case 0:
		m_radio_cserial2_use.SetCheck(FALSE);
		m_radio_cserial2_notuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_cserial2_use.SetCheck(TRUE);
		m_radio_cserial2_notuse.SetCheck(FALSE);
		break;
	}
}

void CScreen_Basic::OnBasic_Auto_TimeOut_mode_change()
{
	int nAuto_timeOut_mode = g_dataMgr.GetBackupData( &st_basic.n_auto_timeout_mode );
	if( nAuto_timeOut_mode < 0 || nAuto_timeOut_mode > 1 )
	{
		nAuto_timeOut_mode = 0;
	}

	if( nAuto_timeOut_mode == 0)
	{
		m_radio_auto_timeout.SetCheck(FALSE);
		m_dgt_time_out.ShowWindow(TRUE);
		m_dgt_time_out_per.ShowWindow(FALSE);
		g_dataMgr.SetDataTemp( &st_basic.n_auto_timeout_mode, (int)0 );
	}
	else
	{
		m_radio_auto_timeout.SetCheck(TRUE);
		m_dgt_time_out.ShowWindow(FALSE);
		m_dgt_time_out_per.ShowWindow(TRUE);
		g_dataMgr.SetDataTemp( &st_basic.n_auto_timeout_mode, (int)1 );
	}
}

void CScreen_Basic::OnRadioCserial2Use() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_cserial2_mode ) == CTL_YES )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_cserial2_mode, (int)CTL_YES );
	OnBasic_CSerial2Mode_Change();
}

void CScreen_Basic::OnRadioCserial2Notuse() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_cserial2_mode ) == CTL_NO )
		return;
	
	g_dataMgr.SetDataTemp( &st_basic.n_cserial2_mode, (int)CTL_NO );
	OnBasic_CSerial2Mode_Change();		
}

void CScreen_Basic::OnRadioAutoTimeout() 
{
	if( g_dataMgr.GetBackupData( &st_basic.n_auto_timeout_mode ) == CTL_YES )
	{
		g_dataMgr.SetDataTemp( &st_basic.n_auto_timeout_mode, (int)CTL_NO );
		OnBasic_Auto_TimeOut_mode_change();		
	}	
	else
	{
		g_dataMgr.SetDataTemp( &st_basic.n_auto_timeout_mode, (int)CTL_YES );
		OnBasic_Auto_TimeOut_mode_change();		
	}
}

void CScreen_Basic::OnDgtTimeOutPer() 
{
	CString mstr_temp;  // 저장할 정보 임시 저장 변수 
	int mn_sec;
	
	mn_sec = m_dgt_time_out_per.GetValue();
	
	m_dgt_time_out_per.SetFocus();
	
	mstr_temp = _T("테스트 타임아웃 (%)");
	if ( g_local.GetLocalType() == LOCAL_ENG )	mstr_temp = _T("Test Time Out (%)");
	
	CRect r;
	
	m_dgt_network_wait_time.GetWindowRect(&r);
	mstr_temp = KeyPad.GetNumEditString_I(1, 300, mn_sec, mstr_temp, &r);
	
	int iTemp = atoi(mstr_temp);
	g_dataMgr.SetDataTemp( &st_basic.mn_time_out_per, iTemp );
	m_dgt_time_out_per.SetValue(iTemp);	
}

void CScreen_Basic::OnBtnSendHifix() 
{
	CDialog_Select  select_dlg;
	st_msg.mstr_confirm_msg = _T("HiFix 모드를 다시 보내시겠습니까?");
	if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_keypad_msg = _T("Send again Hifix Mode");
	
	int n_response = select_dlg.DoModal();
	if (n_response == IDOK)
		g_client_bpc.SetTestMode( "TestMode_HotFix" );
	
}
