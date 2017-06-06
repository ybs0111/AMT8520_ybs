// Screen_Set_Maintenance.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Screen_Set_Maintenance.h"

#include "io.h"  // ���� ���� ���� �˻� �Լ� ȣ���ϱ� ���ؼ��� �ݵ�� INCLUDE �ʿ�
#include "FileDialogST.h"
// ******************************************************************************
// ��ȭ ���� Ŭ���� �߰�                                                         
// ******************************************************************************
//#include "Dialog_Infor.h"
#include "Dialog_Select.h"
#include "Dialog_Message.h"

#include "Dialog_KeyPad.h"
#include "Dialog_Keyboard.h"

#include "Dialog_Pass_Change.h"
// ******************************************************************************

#include "AMTLotManager.h"
#include "AVersion.h"

#include "SrcPart\DataManager.h"//2015.0604
#include "Dialog_Pass_Check.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Set_Maintenance

IMPLEMENT_DYNCREATE(CScreen_Set_Maintenance, CFormView)

CScreen_Set_Maintenance::CScreen_Set_Maintenance()
	: CFormView(CScreen_Set_Maintenance::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Set_Maintenance)
	//}}AFX_DATA_INIT
}

CScreen_Set_Maintenance::~CScreen_Set_Maintenance()
{
}

void CScreen_Set_Maintenance::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Set_Maintenance)
	DDX_Control(pDX, IDC_BTN_PASSWORD_CHANGE_LEVLE2, m_btn_password_change_level2);
	DDX_Control(pDX, IDC_BTN_PASSWORD_CHANGE_LEVLE1, m_btn_password_change_level1);
	DDX_Control(pDX, IDC_BTN_DATA_BACKUP, m_btn_data_backup);
	DDX_Control(pDX, IDC_BTN_STEP_SAVE, m_btn_step_save);
	DDX_Control(pDX, IDC_GROUP_LAMP_CHANGE_TIME, m_group_lamp_change_time);
	DDX_Control(pDX, IDC_PAD_EQUIP_NO, m_pad_equip_no);
	DDX_Control(pDX, IDC_PAD_EQUIP_ID, m_pad_equip_id);
	DDX_Control(pDX, IDC_MSG_EQUIP_NO, m_msg_equip_no);
	DDX_Control(pDX, IDC_MSG_EQUIP_ID, m_msg_equip_id);
	DDX_Control(pDX, IDC_LAMP_STOP, m_lamp_stop);
	DDX_Control(pDX, IDC_LAMP_SELFCHECK, m_lamp_selfcheck);
	DDX_Control(pDX, IDC_LAMP_WARNING, m_lamp_warning);
	DDX_Control(pDX, IDC_LAMP_RUN, m_lamp_run);
	DDX_Control(pDX, IDC_LAMP_LOTEND, m_lamp_lotend);
	DDX_Control(pDX, IDC_LAMP_LOCK, m_lamp_lock);
	DDX_Control(pDX, IDC_LAMP_INITIAL, m_lamp_initial);
	DDX_Control(pDX, IDC_LAMP_ALARM, m_lamp_alarm);
	DDX_Control(pDX, IDC_GROUP_TOWERLAMP, m_group_towerlamp);
	DDX_Control(pDX, IDC_GROUP_PASSWORD_CHANGE, m_group_password_change);
	DDX_Control(pDX, IDC_GROUP_LAMP_WARNING, m_group_lamp_warning);
	DDX_Control(pDX, IDC_GROUP_LAMP_STOP, m_group_lamp_stop);
	DDX_Control(pDX, IDC_GROUP_LAMP_SELFCHECK, m_group_lamp_selfcheck);
	DDX_Control(pDX, IDC_GROUP_LAMP_RUN, m_group_lamp_run);
	DDX_Control(pDX, IDC_GROUP_LAMP_LOTEND, m_group_lamp_lotend);
	DDX_Control(pDX, IDC_GROUP_LAMP_LOCK, m_group_lamp_lock);
	DDX_Control(pDX, IDC_GROUP_LAMP_INITIAL, m_group_lamp_initial);
	DDX_Control(pDX, IDC_GROUP_LAMP_FLICKER_TIME, m_group_lamp_flicker_time);
	DDX_Control(pDX, IDC_GROUP_LAMP_ALARM, m_group_lamp_alarm);
	DDX_Control(pDX, IDC_GROUP_EQP_SET, m_group_eqp_set);
	DDX_Control(pDX, IDC_GROUP_DATA_BACKUP, m_group_data_backup);
	DDX_Control(pDX, IDC_GROUP_BUZZER, m_group_buzzer);
	DDX_Control(pDX, IDC_CHK_BUZZER, m_chk_buzzer);
	DDX_Control(pDX, IDC_BTN_MAINTENANCE_APPLY, m_btn_maintenance_apply);
	DDX_Control(pDX, IDC_MSG_EMTY_ID, m_msg_empty_id);
	DDX_Control(pDX, IDC_PAD_EMPTY_ID, m_pad_empty_id);

	////2015.0604
	DDX_Control(pDX, IDC_PAD_FTP_IP, m_pad_ftp_ip);
	DDX_Control(pDX, IDC_PAD_FTP_USER, m_pad_ftp_user);
	DDX_Control(pDX, IDC_PAD_FTP_PW, m_pad_ftp_pw);
	DDX_Control(pDX, IDC_PAD_FTP_PORT, m_pad_ftp_port);
	
	DDX_Control(pDX, IDC_LIST_DIRECTORY, m_listRemote);
	DDX_Control(pDX, IDC_GROUP_FTP_IP, m_group_ftp_ip);

	DDX_Control(pDX, IDC_GROUP_FTP_MODE, m_group_ftp_mode);
	DDX_Control(pDX, IDC_RADIO_USE_FTP_MODE, m_radio_use_ftp);
	DDX_Control(pDX, IDC_RADIO_USE_FTP_MODE2, m_radio_use_local);
	DDX_Control(pDX, IDC_RADIO_NOTUSE_FTP_MODE, m_radio_notuse_ftp);

	////
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreen_Set_Maintenance, CFormView)
	//{{AFX_MSG_MAP(CScreen_Set_Maintenance)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DGT_FLICKER_TIME, OnDgtFlickerTime)
	ON_BN_CLICKED(IDC_CHK_BUZZER, OnChkBuzzer)
	ON_BN_CLICKED(IDC_BTN_DATA_BACKUP, OnBtnDataBackup)
	ON_BN_CLICKED(IDC_PAD_EQUIP_NO, OnPadEquipNo)
	ON_BN_CLICKED(IDC_PAD_EQUIP_ID, OnPadEquipId)
	ON_BN_CLICKED(IDC_BTN_PASSWORD_CHANGE_LEVLE1, OnBtnPasswordChangeLevle1)
	ON_BN_CLICKED(IDC_BTN_PASSWORD_CHANGE_LEVLE2, OnBtnPasswordChangeLevle2)
	ON_BN_CLICKED(IDC_BTN_MAINTENANCE_APPLY, OnBtnMaintenanceApply)
	ON_EN_SETFOCUS(IDC_EDIT_EQUIP_ID, OnSetfocusEditEquipId)
	ON_EN_SETFOCUS(IDC_EDIT_EQUIP_NO, OnSetfocusEditEquipNo)
	ON_BN_CLICKED(IDC_DGT_CHANGE_TIME, OnDgtChangeTime)
	ON_BN_CLICKED(IDC_BTN_STEP_SAVE, OnBtnStepSave)
	ON_BN_CLICKED(IDC_PAD_EMPTY_ID, OnPadEmptyId)
	ON_EN_SETFOCUS(IDC_EDIT_EMPTY_ID, OnSetfocusEditEmptyId)
	ON_BN_CLICKED(IDC_BTN_FTP_CONNECT, OnBtnFtpConnect)
	ON_BN_CLICKED(IDC_PAD_FTP_IP, OnPadFtpIp)
	ON_BN_CLICKED(IDC_PAD_FTP_PW, OnPadFtpPw)
	ON_BN_CLICKED(IDC_PAD_FTP_PORT, OnPadFtpPort)
	ON_BN_CLICKED(IDC_PAD_FTP_USER, OnPadFtpUser)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIRECTORY, OnDblclkListDirectory)
	ON_BN_CLICKED(IDC_BTN_SCRAP_CODE_READ, OnBtnScrapCodeRead)
	ON_BN_CLICKED(IDC_BTN_FTP_FILE_DOWN, OnBtnFtpFileDown)
	ON_BN_CLICKED(IDC_RADIO_USE_FTP_MODE, OnRadioUseFtpMode)
	ON_BN_CLICKED(IDC_RADIO_NOTUSE_FTP_MODE, OnRadioNotuseFtpMode)
	ON_BN_CLICKED(IDC_RADIO_USE_FTP_MODE2, OnRadioUseFtpMode2)
	ON_BN_CLICKED(IDC_BTN_RECIIPE_RULE, OnBtnReciipeRule)
	ON_EN_CHANGE(IDC_EDIT_FTP_SCRAP_CODE_NAME, OnChangeEditFtpScrapCodeName)
	ON_EN_CHANGE(IDC_EDIT_FTP_RECIPE_RULE_NAME, OnChangeEditFtpRecipeRuleName)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_LAMP_RLAMP, ID_LAMP_RLAMP+SELFCHECK, OnRLampClick)
	ON_COMMAND_RANGE(ID_LAMP_YLAMP, ID_LAMP_YLAMP+SELFCHECK, OnYLampClick)
	ON_COMMAND_RANGE(ID_LAMP_GLAMP, ID_LAMP_GLAMP+SELFCHECK, OnGLampClick)
	ON_MESSAGE(WM_MAINTENANCE_APPLY, OnMaintenance_Info_Apply)				// Tower Lamp DATA�� APPLY ��Ű�� ���� ����� ���� �޽��� ����
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIRECTORY, OnDblclkListDirectory)
	////
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Set_Maintenance diagnostics

#ifdef _DEBUG
void CScreen_Set_Maintenance::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Set_Maintenance::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Set_Maintenance message ICTs

void CScreen_Set_Maintenance::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	// **************************************************************************
	// ��Ʈ�ѿ� ������ ��Ʈ �����Ѵ�                                             
	// **************************************************************************
// 	mp_maintenance_font = NULL;
// 	mp_maintenance_font = new CFont;
// 	mp_maintenance_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"MS Sans Serif");

// 	mp_combo_font = NULL;
// 	mp_combo_font = new CFont;
// 	mp_combo_font->CreateFont(23, 0, 0, 0, 300, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");
	// **************************************************************************
	
	OnMaintenance_GroupBox_Set();
	OnMaintenance_Button_Set();
	OnMaintenance_EditBox_Set();
	OnMaintenance_Label_Set();
	OnMaintenance_Digital_Count_Set();
	OnMaintenance_ComboBox_Set();

	OnMaintenance_Data_Set();
	mb_first = TRUE;
	OnMaintenance_Data_BackUp();

	OnMaintenance_Data_Display();

	OnMaintenance_Buzzer_Change();

	////2015.0604
	OnInitListRemote();
	OnMaintenance_FtpMode_Change();
	////

	st_handler.cwnd_maintenance = this;  // Tower Lamp ȭ�鿡 ���� �ڵ� ���� ���� 
	//st_lamp.mstr_ftp_recipe_rule_name = "RECIPE_RULE.TXT";
}

void CScreen_Set_Maintenance::OnDestroy() 
{
	int mn_response;  // ��ȭ ���ڿ� ���� ���� �� ���� ����
	int Ret;

	CDialog_Select select_dlg;

	//���α׷��� ���������鼭 Save ���� �ʰ�, Form�� Destroy�Ǵ°��̶�� Setting�� �����Ѵٴ� ���̴�.
	//�񱳵� ���� ����.
	if (st_handler.b_program_exit == false)
	{
		Ret = OnMaintenance_Data_Comparison();

		if (Ret == RET_ERROR)
		{
			st_msg.mstr_confirm_msg = _T("Changed Data! Apply?");
			mn_response = select_dlg.DoModal();
			
			if (mn_response == IDOK)
			{
				OnMaintenance_Data_Apply();

				OnMaintenance_LogFile_Create();

				OnMaintenance_ListBar_Info_Send();
			}
			else if (mn_response == IDCANCEL)
			{
				OnMaintenance_Data_Recovery();

				OnMaintenance_Data_Display();

				OnMaintenance_Data_Apply();

				OnMaintenance_ListBar_Info_Send();
			}	
		}
	}

	// **************************************************************************
	// ������ ��Ʈ ���� �����Ѵ�                                                 
	// **************************************************************************
// 	delete mp_maintenance_font;
// 	mp_maintenance_font = NULL;

// 	delete mp_combo_font;
// 	mp_combo_font = NULL;
	// **************************************************************************

	st_handler.cwnd_maintenance = NULL;	// Maintenance ȭ�鿡 ���� �ڵ� ���� ���� ���� �ʱ�ȭ 

	CFormView::OnDestroy();	
}

void CScreen_Set_Maintenance::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (mb_first)          // Ÿ�� ���� ó�� ���� ���� �˻�
	{
		mb_first = FALSE;  // Ÿ�� ���� ó�� ���� ���� �÷��� ����

		// **********************************************************************
		// Ÿ�� ���� ����� ��Ʈ�� ���� ũ�� ��´�                              
		// -> Ÿ�� ���� ��� ���� ���� �Ŀ��� �ش� ��Ʈ�� ������ �ʰ� �����     
		// -> Ÿ�� ���� ���������� ����ϴ� ���� ����̱� �����̴�               
		// -> GetWindowRect() : �����츦 �ѷ��δ� �簢���� ũ�⸦ ����           
		//    lprc�� ����Ű�� RECT ����ü�� �� ���� �����Ѵ�                     
		//  : ����� ��ǥ�� �ȼ� �����̸� ȭ���� ���� ����� �������� ����Ѵ�   
		//  : ���� �������� ũ��� ��ġ�� ȭ�� ��� ��ǥ�� �˰� ���� �� ����Ѵ� 
		// **********************************************************************
		CRect r;
		GetWindowRect(&r);

		m_lamp_run.GetWindowRect(&mr_area[RUN]);
		m_lamp_stop.GetWindowRect(&mr_area[STOP]);
		m_lamp_alarm.GetWindowRect(&mr_area[ALARM]);
		m_lamp_lock.GetWindowRect(&mr_area[LOCK]);
		m_lamp_lotend.GetWindowRect(&mr_area[LOTEND]);
		m_lamp_initial.GetWindowRect(&mr_area[INIT]);
		m_lamp_warning.GetWindowRect(&mr_area[WARNING]);
		m_lamp_selfcheck.GetWindowRect(&mr_area[SELFCHECK]);

		for(int i=0; i<=SELFCHECK; i++)
		{
			mr_area[i].left -= r.left;
			mr_area[i].right -= r.left;
			mr_area[i].top -= r.top;
			mr_area[i].bottom -= r.top;
		}

		m_lamp_run.ShowWindow(SW_HIDE);
		m_lamp_stop.ShowWindow(SW_HIDE);
		m_lamp_alarm.ShowWindow(SW_HIDE);
		m_lamp_lock.ShowWindow(SW_HIDE);
		m_lamp_lotend.ShowWindow(SW_HIDE);
		m_lamp_initial.ShowWindow(SW_HIDE);
		m_lamp_warning.ShowWindow(SW_HIDE);
		m_lamp_selfcheck.ShowWindow(SW_HIDE);
		// **********************************************************************

		for(int j=0; j<=SELFCHECK; j++)
			DrawButton(j);							// Ÿ�� ���� ��ư ���� �Լ�

		for(int k=0; k<=SELFCHECK; k++)
		{
			OnMaintenance_Red_State_Change(k);		// RED Ÿ�� ���� ��ư ���� ��ȯ �Լ�
			OnMaintenance_Yellow_State_Change(k);	// YELLOW Ÿ�� ���� ��ư ���� ��ȯ �Լ�
			OnMaintenance_Green_State_Change(k);	// GREEN Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		}
	}
}

void CScreen_Set_Maintenance::OnMaintenance_GroupBox_Set()
{
	CSxLogFont maintenance_font(15,FW_SEMIBOLD,false,"MS Sans Serif");

//	m_group_towerlamp.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_towerlamp.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_towerlamp.SetFont(maintenance_font);
	m_group_towerlamp.SetCatptionTextColor(RGB(145,25,0));
	m_group_towerlamp.SetFontBold(TRUE);

//	m_group_lamp_run.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_run.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_run.SetFont(maintenance_font);
	m_group_lamp_run.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_run.SetFontBold(TRUE);

//	m_group_lamp_stop.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_stop.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_stop.SetFont(maintenance_font);
	m_group_lamp_stop.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_stop.SetFontBold(TRUE);

//	m_group_lamp_alarm.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_alarm.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_alarm.SetFont(maintenance_font);
	m_group_lamp_alarm.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_alarm.SetFontBold(TRUE);

//	m_group_lamp_lock.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_lock.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_lock.SetFont(maintenance_font);
	m_group_lamp_lock.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_lock.SetFontBold(TRUE);

//	m_group_lamp_lotend.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_lotend.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_lotend.SetFont(maintenance_font);
	m_group_lamp_lotend.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_lotend.SetFontBold(TRUE);

//	m_group_lamp_initial.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_initial.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_initial.SetFont(maintenance_font);
	m_group_lamp_initial.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_initial.SetFontBold(TRUE);
	m_group_lamp_initial.ShowWindow(FALSE);

//	m_group_lamp_warning.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_warning.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_warning.SetFont(maintenance_font);
	m_group_lamp_warning.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_warning.SetFontBold(TRUE);

//	m_group_lamp_selfcheck.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_selfcheck.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_selfcheck.SetFont(maintenance_font);
	m_group_lamp_selfcheck.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_selfcheck.SetFontBold(TRUE);
//	m_group_lamp_selfcheck.ShowWindow(FALSE);

//	m_group_lamp_flicker_time.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_lamp_flicker_time.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_lamp_flicker_time.SetFont(maintenance_font);
	m_group_lamp_flicker_time.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_flicker_time.SetFontBold(TRUE);

//	m_group_buzzer.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_buzzer.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_buzzer.SetFont(maintenance_font);
	m_group_buzzer.SetCatptionTextColor(RGB(145,25,0));
	m_group_buzzer.SetFontBold(TRUE);

//	m_group_password_change.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_password_change.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_password_change.SetFont(maintenance_font);
	m_group_password_change.SetCatptionTextColor(RGB(145,25,0));
	m_group_password_change.SetFontBold(TRUE);

//	m_group_eqp_set.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_eqp_set.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_eqp_set.SetFont(maintenance_font);
	m_group_eqp_set.SetCatptionTextColor(RGB(145,25,0));
	m_group_eqp_set.SetFontBold(TRUE);

//	m_group_data_backup.SetXPGroupStyle(CXPGroupBox::XPGB_WINDOW);
//	m_group_data_backup.SetBackgroundColor(RGB(201, 201, 151), RGB(255, 255, 204));
	m_group_data_backup.SetFont(maintenance_font);
	m_group_data_backup.SetCatptionTextColor(RGB(145,25,0));
	m_group_data_backup.SetFontBold(TRUE);

	m_group_lamp_change_time.SetFont(maintenance_font);
	m_group_lamp_change_time.SetCatptionTextColor(RGB(145,25,0));
	m_group_lamp_change_time.SetFontBold(TRUE);

	////2015.0604
	m_group_ftp_ip.SetFont(maintenance_font);
	m_group_ftp_ip.SetCatptionTextColor(RGB(145,25,0));
	m_group_ftp_ip.SetFontBold(TRUE);

	m_group_ftp_mode.SetFont(maintenance_font);
	m_group_ftp_mode.SetCatptionTextColor(RGB(145,25,0));
	m_group_ftp_mode.SetFontBold(TRUE);
	
	////
}

void CScreen_Set_Maintenance::DrawButton(int i)
{
	// **************************************************************************
    // Ÿ�� ���� ����� ��Ʈ�ѿ� ���� ���� ��´�                                
    // **************************************************************************
	CRect r;
	r = mr_area[i];  // ��Ʈ�� ���� ũ�� ����

	//Warning�ϰ�, Self Check�� �����.
//	if (i == dSELFCHECK || i == dINIT)	return;
	if (i == dINIT)	return;

	mn_pos_x[i][0] = r.left;
	mn_pos_x[i][1] = r.left + ((int)r.Width()-4);
	mn_pos_y[i][0] = r.top;
	mn_pos_y[i][1] = r.bottom-4;
	mn_y_diff[i] = (int) (mn_pos_y[i][1] - mn_pos_y[i][0])/7;
	mn_y_space[i] = (int) mn_y_diff[i]/6;
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� ���� ��� �̹��� ���� ������ ���� ���� �����Ѵ�                      
    // **************************************************************************
	CRgn b1, b2, e1, e2;
	COLORREF cr;
	int x1=0, x2, y1=0, y2, w=mn_pos_x[i][1] - mn_pos_x[i][0];
	CPoint p;
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� ���� ���� ���� �ձ� ���� ����� ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�      
	// -> CreateEllipticRgn() : �־��� �簢�� �������� Ÿ�� ������ �����Ѵ�      
	//  : ���������� ������ �����Ǿ����� ������ �ڵ� ���� ����                   
	//  : ������ �������� �ʾ����� NULL�� ����                                   
    // **************************************************************************
	p.x = mn_pos_x[i][0];
	p.y = mn_pos_y[i][1] - mn_y_space[i] * 4;
	x2 = w;	
	y2 = mn_y_space[i] * 4;
	e1.CreateEllipticRgn(x1, y1, x2, y2);

	mp_btn_base[i].Create("", WS_CHILD|WS_VISIBLE, p, e1, this, IDC_LAMP_BASE+i, 1, RGB(150, 159, 183), RGB(150, 159, 183), RGB(150, 159, 183), RGB(150, 159, 183));
	mp_btn_base[i].EnableWindow(FALSE);

	e1.DeleteObject();
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� ������ �ϴ� ���� �̹��� ��� ��ġ ����Ͽ� ��Ʈ�ѿ� ����Ѵ�         
	// -> CreateRectRgn() : �־��� ��ǥ�� �簢 ������ �׸���                     
	// -> OffsetRgn() : ������ �簢���� ��ǥ�� �̵��Ѵ�                          
	// -> CombineRgn() : �����ϴ� �� ���� �������� ���ļ� �� ��° ������ �����  
	//  : �� ��° ������ �ڵ��� �� �Լ��� ���̱� ���� �ݵ�� ��������߸�        
	//    ���ο� �������� ��ü�ȴ�                                               
	// -> GetRgnBox() : �־��� ������ ������ �ѷ��δ� ���� ���� ���簢�� �׸���  
    // **************************************************************************
	x2 = (int) w / 3;
	y2 = (int) mn_y_space[i] * 4 / 3;
	e1.CreateEllipticRgn(x1, y1, x2, y2);

	y2 = mn_y_diff[i] * 2 - mn_y_space[i] * 2;
	b1.CreateRectRgn(x1, y1, x2, y2);

	x2 = w;
	y2 = mn_y_space[i] * 4;
	e2.CreateEllipticRgn(x1, y1, x2, y2);

	e1.OffsetRgn(0, mn_y_diff[i]*2 - mn_y_space[i]*2 - (int)mn_y_space[i]*2/3);
	e2.OffsetRgn(-(int)w/3, -mn_y_space[i]);
	b1.CombineRgn(&e1, &b1, RGN_OR);
	b1.CombineRgn(&b1, &e2, RGN_DIFF);

	CRect rr;
	b1.GetRgnBox(&rr);

	p.y = rr.top;
	p.x = mn_pos_x[i][0] + (int) w/3;
	p.y += (mn_pos_y[i][1] - mn_y_diff[i] * 2);
	mp_btn_pole[i].Create("", WS_CHILD|WS_VISIBLE, p, b1, this, IDC_LAMP_POLE+i, 1, RGB(203, 185, 156), RGB(203, 185, 156), RGB(203, 185, 156), RGB(203, 185, 156));
	mp_btn_pole[i].EnableWindow(FALSE);

	e1.DeleteObject();
	e2.DeleteObject();
	b1.DeleteObject();
	// **************************************************************************

	// **************************************************************************
    // GREEN ���� �ؿ� �����ϴ� �ö�ƽ ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�         
    // **************************************************************************
	x2 = w;
	y2 = mn_y_diff[i];
	b1.CreateRectRgn(x1, y1, x2, y2);

	y2 = mn_y_space[i] * 4;
	e1.CreateEllipticRgn(x1, y1, x2, y2);

	y2 = mn_y_space[i] * 4;
	e2.CreateEllipticRgn(x1, y1, x2, y2);

	e1.OffsetRgn(0, -mn_y_space[i]*2);
	b1.CombineRgn(&b1, &e1, RGN_DIFF);
	e2.OffsetRgn(0, mn_y_diff[i] - mn_y_space[i]*2);
	b1.CombineRgn(&b1, &e2, RGN_OR);

	p.x = mn_pos_x[i][0];
	p.y = mn_pos_y[i][1] - mn_y_diff[i] * 3 + mn_y_space[i];
	mp_btn_lower[i].Create("", WS_CHILD|WS_VISIBLE, p, b1, this, IDC_LAMP_LOWER+i, 1, RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217));
	mp_btn_lower[i].EnableWindow(FALSE);

	e1.OffsetRgn(0, mn_y_space[i]*2);
	e2.OffsetRgn(0, -(mn_y_diff[i] - mn_y_space[i]*2));

	b1.DeleteObject();
	// **************************************************************************

	// **************************************************************************
    // GREEN ���� �ٷ� �ؿ� �����ϴ� �׵θ� ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�      
    // **************************************************************************
	y2 = mn_y_space[i];
	b1.CreateRectRgn(x1, y1, x2, y2);

	e1.OffsetRgn(0, -mn_y_space[i]);
	b1.CombineRgn(&b1, &e1, RGN_OR);
	e2.OffsetRgn(0, -mn_y_space[i]*2);
	b1.CombineRgn(&b1, &e2, RGN_DIFF);

	p.y -= mn_y_space[i];
	mp_btn_step1[i].Create("", WS_CHILD|WS_VISIBLE, p, b1, this, IDC_LAMP_SEP1+i, 1, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0));
	mp_btn_step1[i].EnableWindow(FALSE);
	e1.OffsetRgn(0, mn_y_space[i]);
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� GREEN ���� ��� ��ġ ����Ͽ� ��Ʈ�ѿ� ����Ѵ�                      
    // **************************************************************************
	y2 = mn_y_diff[i] - mn_y_space[i];
	b2.CreateRectRgn(x1, y1, x2, y2);

	e1.OffsetRgn(0, mn_y_diff[i] - mn_y_space[i]*3);
	b2.CombineRgn(&b2, &e1, RGN_OR);
	b2.CombineRgn(&b2, &e2, RGN_DIFF);

	CString title;  // ���� ���� ���� ����

	p.y -= (mn_y_diff[i] - mn_y_space[i]);
	if (mn_g_lamp[1][i]==0) 
	{
		cr = G_OFF;
		title = "OFF";
	}
	else if (mn_g_lamp[1][i]==1)
	{
		cr = G_ON;
		title = "ON";
	}
	else if (mn_g_lamp[1][i]==2)
	{
		cr = G_OFF;
		title = "Flicker";
	}

	mp_btn_g_lamp[i].Create(title, WS_CHILD|WS_VISIBLE, p, b2, this, ID_LAMP_GLAMP+i, 1, cr, G_ON, cr, cr);
	e1.OffsetRgn(0, -(mn_y_diff[i] - mn_y_space[i]*3));
	// **************************************************************************

	// **************************************************************************
    // GREEN ������ YELLOW ���� ������ �׵θ� ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�    
    // **************************************************************************
	p.y -= mn_y_space[i];

	mp_btn_step2[i].Create("", WS_CHILD|WS_VISIBLE, p, b1, this, IDC_LAMP_SEP2+i, 1, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0));
	mp_btn_step2[i].EnableWindow(FALSE);
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� YELLOW ���� ��� ��ġ ����Ͽ� ��Ʈ�ѿ� ����Ѵ�                     
    // **************************************************************************
	p.y -= (mn_y_diff[i] - mn_y_space[i]);
	if (mn_y_lamp[1][i]==0) 
	{
		cr = Y_OFF;
		title = "OFF";
	}
	else if (mn_y_lamp[1][i]==1)
	{
		cr = Y_ON;
		title = "ON";
	}
	else if (mn_y_lamp[1][i]==2)
	{
		cr = Y_OFF;
		title = "Flicker";
	}

	mp_btn_y_lamp[i].Create(title, WS_CHILD|WS_VISIBLE, p, b2, this, ID_LAMP_YLAMP+i, 1, cr, Y_ON, cr, cr);
	// **************************************************************************

	// **************************************************************************
    // YELLOW ������ RED ���� ������ �׵θ� ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�      
    // **************************************************************************
	p.y -= mn_y_space[i];

	 mp_btn_step3[i].Create("", WS_CHILD|WS_VISIBLE, p, b1, this, IDC_LAMP_SEP3+i, 1, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0));
	 mp_btn_step3[i].EnableWindow(FALSE);
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� RED ���� ��� ��ġ ����Ͽ� ��Ʈ�ѿ� ����Ѵ�                        
    // **************************************************************************
	p.y -= (mn_y_diff[i] - mn_y_space[i]);
	if (mn_r_lamp[1][i]==0) 
	{
		cr = R_OFF;
		title = "OFF";
	}
	else if (mn_r_lamp[1][i]==1)
	{
		cr = R_ON;
		title = "ON";
	}
	else if (mn_r_lamp[1][i]==2)
	{
		cr = R_OFF;
		title = "Flicker";
	}

	mp_btn_r_lamp[i].Create(title, WS_CHILD|WS_VISIBLE, p, b2, this, ID_LAMP_RLAMP+i, 1, cr, R_ON, cr, cr);
	// **************************************************************************

	// **************************************************************************
    // RED ���� ���� �����ϴ� �ö�ƽ ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�           
    // **************************************************************************
	p.y -= (mn_y_diff[i]-mn_y_space[i]);

	mp_btn_upper[i].Create("", WS_CHILD|WS_VISIBLE, p, b2, this, IDC_LAMP_UPPER+i, 1, RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217));
	mp_btn_upper[i].EnableWindow(FALSE);
	// **************************************************************************

	// **************************************************************************
    // Ÿ�� ���� �� �� �ձ� �ö�ƽ ���� ����Ͽ� ��Ʈ�ѿ� ����Ѵ�             
    // **************************************************************************
	p.y -= mn_y_space[i]*2;

	mp_btn_roof[i].Create("", WS_CHILD|WS_VISIBLE, p, e1, this, IDC_LAMP_ROOF+i, 1, RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217), RGB(217, 217, 217));
	mp_btn_roof[i].EnableWindow(FALSE);
	// **************************************************************************
}

void CScreen_Set_Maintenance::OnMaintenance_Red_State_Change(int idx)
{
	//Wanring�ϰ�, Self Check�� �����.
//	if (idx == dSELFCHECK || idx == dINIT)	return;
	if (idx == dINIT)	return;

	int mn_red = mn_r_lamp[1][idx];  // RED ���� ����

	switch(mn_red)
	{
	case 0 :  // OFF
		mp_btn_r_lamp[idx].SetWindowText("OFF");
		mp_btn_r_lamp[idx].ChangeColor(R_OFF, R_ON, R_OFF, R_OFF);
		break;
	case 1 :  // ON
		mp_btn_r_lamp[idx].SetWindowText("ON");
		mp_btn_r_lamp[idx].ChangeColor(R_ON, R_ON, R_ON, R_ON);
		break;
	case 2 :  // FLICKER
		mp_btn_r_lamp[idx].SetWindowText("FLICK");
		mp_btn_r_lamp[idx].ChangeColor(FLICK, R_ON, FLICK, R_ON);
		break;
	default :
		break;
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Yellow_State_Change(int idx)
{
	//Wanring�ϰ�, Self Check�� �����.
//	if (idx == dSELFCHECK || idx == dINIT)	return;
	if (idx == dINIT)	return;

	int mn_yellow = mn_y_lamp[1][idx];  // YELLOW ���� ����

	switch(mn_yellow)
	{
	case 0 :  // OFF
		mp_btn_y_lamp[idx].SetWindowText("OFF");
		mp_btn_y_lamp[idx].ChangeColor(Y_OFF, Y_ON, Y_OFF, Y_OFF);
		break;
	case 1 :  // ON
		mp_btn_y_lamp[idx].SetWindowText("ON");
		mp_btn_y_lamp[idx].ChangeColor(Y_ON, Y_ON, Y_ON, Y_ON);
		break;
	case 2 :  // FLICKER
		mp_btn_y_lamp[idx].SetWindowText("FLICK");
		mp_btn_y_lamp[idx].ChangeColor(FLICK, Y_ON, FLICK, Y_OFF);
		break;
	default :
		break;
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Green_State_Change(int idx)
{
	//Wanring�ϰ�, Self Check�� �����.
//	if (idx == dSELFCHECK || idx == dINIT)	return;
	if (idx == dINIT)	return;

	int mn_green = mn_g_lamp[1][idx];  // GREEN ���� ����

	switch(mn_green)
	{
	case 0 :  // OFF
		mp_btn_g_lamp[idx].SetWindowText("OFF");
		mp_btn_g_lamp[idx].ChangeColor(G_OFF, G_ON, G_OFF, G_OFF);
		break;
	case 1 :  // ON
		mp_btn_g_lamp[idx].SetWindowText("ON");
		mp_btn_g_lamp[idx].ChangeColor(G_ON, G_ON, G_ON, G_ON);
		break;
	case 2 :  // FLICKER
		mp_btn_g_lamp[idx].SetWindowText("FLICK");
		mp_btn_g_lamp[idx].ChangeColor(FLICK, G_ON, FLICK, G_OFF);
		break;
	default :
		break;
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Data_Set()
{
	int i;
	
	for (i = 0; i <= SELFCHECK; i++)
	{
		mn_r_lamp[1][i] = st_lamp.mn_lamp_r[i];
		mn_y_lamp[1][i] = st_lamp.mn_lamp_y[i];
		mn_g_lamp[1][i] = st_lamp.mn_lamp_g[i];
	}

	mn_buzzer_mode[1] = st_lamp.mn_buzzer_mode;

	mn_lamp_flicker_time[1] = st_lamp.mn_lamp_wait_time;
	mn_lamp_change_time[1] = st_lamp.mn_lamp_change_time;

	mstr_equip_no[1] = st_lamp.mstr_equip_no;
	mstr_equip_id[1] = st_lamp.mstr_equip_id;
	mstr_empty_id[1] = st_lamp.mstr_empty_id;

	////2015.0604
	mstr_ftp_path[1]		      = st_lamp.mstr_ftp_path;
	mstr_ftp_ip[1]				  = st_lamp.mstr_ftp_ip;
	mstr_ftp_user[1]			  = st_lamp.mstr_ftp_user;
	mstr_ftp_pw[1]				  = st_lamp.mstr_ftp_pw;
	mn_ftp_port[1]				  = st_lamp.mn_ftp_port;
	mstr_ftp_scrap_code_name[1]   = st_lamp.mstr_ftp_scrap_code_name;	
	mn_ftp_mode[1]				  = st_basic.mn_mode_ftp;
	////
	mstr_ftp_recipe_rule_name[1]   = st_lamp.mstr_ftp_recipe_rule_name;

//	mb_first = TRUE;
}

void CScreen_Set_Maintenance::OnMaintenance_Button_Set()
{
	short	shBtnColor = 30;

	// *************************************************************
	//  CHECK BUTTON		                                        
	// *************************************************************
	m_chk_buzzer.SetFont(Func.mp_maintenance_font);
	m_chk_buzzer.SetIcon(IDI_SOUND_ON, IDI_SOUND_OFF);
	m_chk_buzzer.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	m_chk_buzzer.SetButtonColor(1);

	// *************************************************************
	//  PAD BUTTON		                                            
	// *************************************************************
	m_pad_equip_id.SetIcon(IDI_KEYBOARD);
	m_pad_equip_id.DrawBorder(FALSE);

	m_pad_equip_no.SetIcon(IDI_KEYBOARD);
	m_pad_equip_no.DrawBorder(FALSE);

	m_pad_empty_id.SetIcon(IDI_KEYBOARD);
	m_pad_empty_id.DrawBorder(FALSE);

	////2015.0604
	m_pad_ftp_ip.SetIcon(IDI_KEYBOARD);
	m_pad_ftp_ip.DrawBorder(FALSE);
	
	m_pad_ftp_user.SetIcon(IDI_KEYBOARD);
	m_pad_ftp_user.DrawBorder(FALSE);

	m_pad_ftp_pw.SetIcon(IDI_KEYBOARD);
	m_pad_ftp_pw.DrawBorder(FALSE);
	
	m_pad_ftp_port.SetIcon(IDI_KEYBOARD);
	m_pad_ftp_port.DrawBorder(FALSE);

	////
}

void CScreen_Set_Maintenance::OnMaintenance_Buzzer_Change()
{
	if (mn_buzzer_mode[1] < 0 || mn_buzzer_mode[1] > 1)	mn_buzzer_mode[1] = TRUE;

	m_chk_buzzer.SetCheck(mn_buzzer_mode[1]);
	
	if (mn_buzzer_mode[1] == TRUE)	SetDlgItemText(IDC_CHK_BUZZER, "Buzzer On");
	else							SetDlgItemText(IDC_CHK_BUZZER, "Buzzer Off");
}

void CScreen_Set_Maintenance::OnMaintenance_EditBox_Set()
{	
	m_edit_equip_id.SubclassDlgItem(IDC_EDIT_EQUIP_ID, this);
	m_edit_equip_id.bkColor(RGB(50, 100, 150));
	m_edit_equip_id.textColor(RGB(255, 255,255));
	m_edit_equip_id.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_equip_no.SubclassDlgItem(IDC_EDIT_EQUIP_NO, this);
	m_edit_equip_no.bkColor(RGB(50, 100, 150));
	m_edit_equip_no.textColor(RGB(255, 255,255));
	m_edit_equip_no.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_empty_id.SubclassDlgItem(IDC_EDIT_EMPTY_ID, this);
	m_edit_empty_id.bkColor(RGB(50, 100, 150));
	m_edit_empty_id.textColor(RGB(255, 255,255));
	m_edit_empty_id.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	////2015.0604
	m_edit_ftp_scrap_code_name.SubclassDlgItem(IDC_EDIT_FTP_SCRAP_CODE_NAME, this);
	m_edit_ftp_scrap_code_name.bkColor(RGB(50, 100, 150));
	m_edit_ftp_scrap_code_name.textColor(RGB(255, 255,255));
	m_edit_ftp_scrap_code_name.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_ftp_recipe_rule_name.SubclassDlgItem(IDC_EDIT_FTP_RECIPE_RULE_NAME, this);
	m_edit_ftp_recipe_rule_name.bkColor(RGB(50, 100, 150));
	m_edit_ftp_recipe_rule_name.textColor(RGB(255, 255,255));
	m_edit_ftp_recipe_rule_name.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");


	m_edit_ftp_path.SubclassDlgItem(IDC_EDIT_FTP_PATH, this);
	m_edit_ftp_path.bkColor(RGB(50, 100, 150));
	m_edit_ftp_path.textColor(RGB(255, 255,255));
	m_edit_ftp_path.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_ftp_ip.SubclassDlgItem(IDC_EDIT_FTP_IP, this);
	m_edit_ftp_ip.bkColor(RGB(50, 100, 150));
	m_edit_ftp_ip.textColor(RGB(255, 255,255));
	m_edit_ftp_ip.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_ftp_user.SubclassDlgItem(IDC_EDIT_FTP_USER, this);
	m_edit_ftp_user.bkColor(RGB(50, 100, 150));
	m_edit_ftp_user.textColor(RGB(255, 255,255));
	m_edit_ftp_user.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
	
	m_edit_ftp_pw.SubclassDlgItem(IDC_EDIT_FTP_PW, this);
	m_edit_ftp_pw.bkColor(RGB(50, 100, 150));
	m_edit_ftp_pw.textColor(RGB(255, 255,255));
	m_edit_ftp_pw.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
	
	m_edit_ftp_port.SubclassDlgItem(IDC_EDIT_FTP_PORT, this);
	m_edit_ftp_port.bkColor(RGB(50, 100, 150));
	m_edit_ftp_port.textColor(RGB(255, 255,255));
	m_edit_ftp_port.setFont(-16, FW_BOLD, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	////
}

void CScreen_Set_Maintenance::OnMaintenance_Data_Apply()
{
	int i;
	CString strftp_port;//2015.0604

	for(i = 0; i <= SELFCHECK; i++)
	{
		// **********************************************************************
		// RED Ÿ�� ���� ���� ���� ������ �����Ѵ�                               
		// **********************************************************************
		if (mn_r_lamp[1][i]<0 || mn_r_lamp[1][i]>2) 
		{
			mn_r_lamp[1][i] = 0;
			OnMaintenance_Red_State_Change(i);     // RED Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		}
		st_lamp.mn_lamp_r[i] = mn_r_lamp[1][i];
		// **********************************************************************

		// **********************************************************************
		// YELLOW Ÿ�� ���� ���� ���� ������ �����Ѵ�                            
		// **********************************************************************
		if (mn_y_lamp[1][i]<0 || mn_y_lamp[1][i]>2) 
		{
			mn_y_lamp[1][i] = 0;
			OnMaintenance_Yellow_State_Change(i);  // YELLOW Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		}
		st_lamp.mn_lamp_y[i] = mn_y_lamp[1][i];
		// **********************************************************************

		// **********************************************************************
		// GREEN Ÿ�� ���� ���� ���� ������ �����Ѵ�                             
		// **********************************************************************
		if (mn_g_lamp[1][i]<0 || mn_g_lamp[1][i]>2) 
		{
			mn_g_lamp[1][i] = 0;
			OnMaintenance_Green_State_Change(i);   // GREEN Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		}
		st_lamp.mn_lamp_g[i] = mn_g_lamp[1][i];
		// **********************************************************************
	}


	st_lamp.mn_buzzer_mode = mn_buzzer_mode[1];

	st_lamp.mn_lamp_wait_time = mn_lamp_flicker_time[1];
	st_lamp.mn_lamp_change_time = mn_lamp_change_time[1];

	// **************************************************************************
	// ��/�� ���� ������ ��� ȣ�� ���� ������ �����Ѵ�                          
	// **************************************************************************
	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_NO))->GetWindowText(mstr_equip_no[1]);
	mstr_equip_no[1].MakeUpper();
	mstr_equip_no[1].TrimLeft(' ');
	mstr_equip_no[1].TrimRight(' ');

	if (mstr_equip_no[1].IsEmpty())  
	{
		mstr_equip_no[1] = "AMT";
	}
	st_lamp.mstr_equip_no = mstr_equip_no[1];
	// **************************************************************************

	////2015.0604
	st_basic.mn_mode_ftp = mn_ftp_mode[1];
	if(mn_ftp_mode[1] == 0)
	{
		GetDlgItem(IDC_RADIO_NOTUSE_FTP_MODE)->GetWindowText(mstr_ftp_mode[1]);
	}
	else if(mn_ftp_mode[1] == 1)
	{
		GetDlgItem(IDC_RADIO_USE_FTP_MODE)->GetWindowText(mstr_ftp_mode[1]);
	}
	else
	{
		GetDlgItem(IDC_RADIO_USE_FTP_MODE2)->GetWindowText(mstr_ftp_mode[1]);
	}
	st_basic.mstr_mode_ftp = mstr_ftp_mode[1];


	((CEdit*)GetDlgItem(IDC_EDIT_FTP_SCRAP_CODE_NAME))->GetWindowText(mstr_ftp_scrap_code_name[1]);
//	mstr_ftp_scrap_code_name[1].MakeUpper();
	mstr_ftp_scrap_code_name[1].TrimLeft(' ');               
	mstr_ftp_scrap_code_name[1].TrimRight(' ');
	
	if (mstr_ftp_scrap_code_name[1].IsEmpty())  
	{
		mstr_ftp_scrap_code_name[1] = "";
	}
	st_lamp.mstr_ftp_scrap_code_name = mstr_ftp_scrap_code_name[1];

	//2016.0129 
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_RECIPE_RULE_NAME))->GetWindowText(mstr_ftp_recipe_rule_name[1]);
	mstr_ftp_recipe_rule_name[1].TrimLeft(' ');               
	mstr_ftp_recipe_rule_name[1].TrimRight(' ');
	
	if (mstr_ftp_recipe_rule_name[1].IsEmpty())  
	{
		mstr_ftp_recipe_rule_name[1] = "";
	}
	st_lamp.mstr_ftp_recipe_rule_name = mstr_ftp_recipe_rule_name[1];

	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PATH))->GetWindowText(mstr_ftp_path[1]);
//	mstr_ftp_path[1].MakeUpper();
	mstr_ftp_path[1].TrimLeft(' ');               
	mstr_ftp_path[1].TrimRight(' ');
	
	if (mstr_ftp_path[1].IsEmpty())  
	{
		mstr_ftp_path[1] = "";
	}
	st_lamp.mstr_ftp_path = mstr_ftp_path[1];

	((CEdit*)GetDlgItem(IDC_EDIT_FTP_IP))->GetWindowText(mstr_ftp_ip[1]);
//	mstr_ftp_ip[1].MakeUpper();
	mstr_ftp_ip[1].TrimLeft(' ');               
	mstr_ftp_ip[1].TrimRight(' ');
	
	if (mstr_ftp_ip[1].IsEmpty())  
	{
		mstr_ftp_ip[1] = "";
	}
	st_lamp.mstr_ftp_ip = mstr_ftp_ip[1];

	((CEdit*)GetDlgItem(IDC_EDIT_FTP_USER))->GetWindowText(mstr_ftp_user[1]);
//	mstr_ftp_user[1].MakeUpper();
	mstr_ftp_user[1].TrimLeft(' ');               
	mstr_ftp_user[1].TrimRight(' ');
	
	if (mstr_ftp_user[1].IsEmpty())  
	{
		mstr_ftp_user[1] = "";
	}
	st_lamp.mstr_ftp_user = mstr_ftp_user[1];

	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PW))->GetWindowText(mstr_ftp_pw[1]);
//	mstr_ftp_pw[1].MakeUpper();
	mstr_ftp_pw[1].TrimLeft(' ');               
	mstr_ftp_pw[1].TrimRight(' ');
	
	if (mstr_ftp_pw[1].IsEmpty())  
	{
		mstr_ftp_pw[1] = "";
	}
	st_lamp.mstr_ftp_pw = mstr_ftp_pw[1];


	strftp_port.Format("%d",mn_ftp_port[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PORT))->GetWindowText(strftp_port);
//	strftp_port.MakeUpper();
	strftp_port.TrimLeft(' ');               
	strftp_port.TrimRight(' ');
	
	if (strftp_port.IsEmpty())  
	{
		mn_ftp_port[1] = 21;
	}
	st_lamp.mn_ftp_port = atoi(strftp_port);	
	////

	// **************************************************************************
	// ��/�� ���� ������ ��� �ڵ� ���� ������ �����Ѵ�                          
	// **************************************************************************
	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_ID))->GetWindowText(mstr_equip_id[1]);
	mstr_equip_id[1].MakeUpper();
	mstr_equip_id[1].TrimLeft(' ');               
	mstr_equip_id[1].TrimRight(' ');

	if (mstr_equip_id[1].IsEmpty())  
	{
		mstr_equip_id[1] = "AMT";
	}
	st_lamp.mstr_equip_id = mstr_equip_id[1];
	// **************************************************************************

	// **************************************************************************
	// ��/�� ���� ������ ��� �ڵ� ���� ������ �����Ѵ�                          
	// **************************************************************************
	((CEdit*)GetDlgItem(IDC_EDIT_EMPTY_ID))->GetWindowText(mstr_empty_id[1]);
	mstr_empty_id[1].MakeUpper();
	mstr_empty_id[1].TrimLeft(' ');               
	mstr_empty_id[1].TrimRight(' ');
	
	if (mstr_empty_id[1].IsEmpty())  
	{
		mstr_empty_id[1] = "AMT";
	}
	st_lamp.mstr_empty_id = mstr_empty_id[1];
	// **************************************************************************

	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Data_BackUp()
{
	int i;

	for(i=0; i<=SELFCHECK; i++)
	{
		// **********************************************************************
		// RED Ÿ�� ���� ���¸� ����Ѵ�                                         
		// **********************************************************************
		mn_r_lamp[0][i] = mn_r_lamp[1][i];
		// **********************************************************************

		// **********************************************************************
		// YELLOW Ÿ�� ���� ���¸� ����Ѵ�                                      
		// **********************************************************************
		mn_y_lamp[0][i] = mn_y_lamp[1][i];
		// **********************************************************************

		// **********************************************************************
		// GREEN Ÿ�� ���� ���¸� ����Ѵ�                                       
		// **********************************************************************
		mn_g_lamp[0][i] = mn_g_lamp[1][i];
		// **********************************************************************
	}

	mn_buzzer_mode[0] = mn_buzzer_mode[1];

	mn_lamp_flicker_time[0] = mn_lamp_flicker_time[1];
	mn_lamp_change_time[0] = mn_lamp_change_time[1];

	mstr_equip_no[0] = mstr_equip_no[1];
	mstr_equip_id[0] = mstr_equip_id[1];
	mstr_empty_id[0] = mstr_empty_id[1];

	////2015.0604
	mstr_ftp_path[0]			= mstr_ftp_path[1];
	mstr_ftp_ip[0]				= mstr_ftp_ip[1];
	mstr_ftp_user[0]			= mstr_ftp_user[1];
	mstr_ftp_pw[0]				= mstr_ftp_pw[1];
	mn_ftp_port[0]				= mn_ftp_port[1];
	mstr_ftp_scrap_code_name[0] = mstr_ftp_scrap_code_name[1];
	mstr_ftp_recipe_rule_name[0] = mstr_ftp_recipe_rule_name[1];

	mn_ftp_mode[0]				= mn_ftp_mode[1];
	////

}

void CScreen_Set_Maintenance::OnMaintenance_ListBar_Info_Send()
{
	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Data_Display()
{
	CString strFtp_port;

	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_ID))->SetWindowText(mstr_equip_id[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_NO))->SetWindowText(mstr_equip_no[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_EMPTY_ID))->SetWindowText(mstr_empty_id[1]);

	////2015.0604
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_SCRAP_CODE_NAME))->SetWindowText(mstr_ftp_scrap_code_name[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PATH))->SetWindowText(mstr_ftp_path[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_IP))->SetWindowText(mstr_ftp_ip[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_USER))->SetWindowText(mstr_ftp_user[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PW))->SetWindowText(mstr_ftp_pw[1]);

	((CEdit*)GetDlgItem(IDC_EDIT_FTP_RECIPE_RULE_NAME))->SetWindowText(mstr_ftp_recipe_rule_name[1]);

	strFtp_port.Format("%d", mn_ftp_port[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PORT))->SetWindowText(strFtp_port);
	////

	m_dgt_flicker_time.SetValue(mn_lamp_flicker_time[1]);
	m_dgt_change_time.SetValue(mn_lamp_change_time[1]);
}

int CScreen_Set_Maintenance::OnMaintenance_Data_Comparison()
{
	int i;
	int Ret = RET_GOOD;

	for(i=0; i<=SELFCHECK; i++)
	{
		// **********************************************************************
		// RED Ÿ�� ���� ���¸� Ȯ���Ѵ�                                         
		// **********************************************************************
		if (mn_r_lamp[0][i] != mn_r_lamp[1][i])					Ret = RET_ERROR;
		// **********************************************************************

		// **********************************************************************
		// YELLOW Ÿ�� ���� ���¸� Ȯ���Ѵ�                                      
		// **********************************************************************
		if (mn_y_lamp[0][i] != mn_y_lamp[1][i])					Ret = RET_ERROR;
		// **********************************************************************

		// **********************************************************************
		// GREEN Ÿ�� ���� ���¸� Ȯ���Ѵ�                                       
		// **********************************************************************
		if (mn_g_lamp[0][i] != mn_g_lamp[1][i])					Ret = RET_ERROR;
		// **********************************************************************
	}

	if (mn_buzzer_mode[0] != mn_buzzer_mode[1])					Ret = RET_ERROR;
	if (mn_lamp_flicker_time[0] != mn_lamp_flicker_time[1])		Ret = RET_ERROR;
	if (mn_lamp_change_time[0] != mn_lamp_change_time[1])		Ret = RET_ERROR;
	if (mstr_equip_no[0] != mstr_equip_no[1])					Ret = RET_ERROR;
	if (mstr_equip_id[0] != mstr_equip_id[1])					Ret = RET_ERROR;
	if (mstr_empty_id[0] != mstr_empty_id[1])					Ret = RET_ERROR;

	////2015.0604
	if (mstr_ftp_ip[0]   != mstr_ftp_ip[1])					    Ret = RET_ERROR;
	if (mstr_ftp_user[0] != mstr_ftp_user[1])				    Ret = RET_ERROR;
	if (mstr_ftp_pw[0]   != mstr_ftp_pw[1])				    	Ret = RET_ERROR;
	if (mn_ftp_port[0]   != mn_ftp_port[1])		    			Ret = RET_ERROR;
	if (mn_ftp_mode[0]	 != mn_ftp_mode[1])						Ret = RET_ERROR;
	////


	return Ret;
}

void CScreen_Set_Maintenance::OnMaintenance_Data_Recovery()
{
	int i;

	for (i = 0; i <= SELFCHECK; i++)
	{
		// **********************************************************************
		// RED Ÿ�� ���� ���¸� �����Ѵ�                                         
		// **********************************************************************
		mn_r_lamp[1][i] = mn_r_lamp[0][i];
		// **********************************************************************

		// **********************************************************************
		// YELLOW Ÿ�� ���� ���¸� �����Ѵ�                                      
		// **********************************************************************
		mn_y_lamp[1][i] = mn_y_lamp[0][i];
		// **********************************************************************

		// **********************************************************************
		// GREEN Ÿ�� ���� ���¸� �����Ѵ�                                       
		// **********************************************************************
		mn_g_lamp[1][i] = mn_g_lamp[0][i];
		// **********************************************************************
	}

	mn_buzzer_mode[1] = mn_buzzer_mode[0];

	mn_lamp_flicker_time[1] = mn_lamp_flicker_time[0];
	mn_lamp_change_time[1] = mn_lamp_change_time[0];

	mstr_equip_no[1] = mstr_equip_no[0];
	mstr_equip_id[1] = mstr_equip_id[0];
	mstr_empty_id[1] = mstr_empty_id[0];

	////2015.0604
	mstr_ftp_path[1]			= mstr_ftp_path[0];
	mstr_ftp_ip[1]				= mstr_ftp_ip[0];
	mstr_ftp_user[1]			= mstr_ftp_user[0];
	mstr_ftp_pw[1]				= mstr_ftp_pw[0];
	mn_ftp_port[1]				= mn_ftp_port[0];
	mstr_ftp_scrap_code_name[1] = mstr_ftp_scrap_code_name[0];
	mn_ftp_mode[1]				= mn_ftp_mode[0];
	////
	mstr_ftp_recipe_rule_name[1] = mstr_ftp_recipe_rule_name[0];

}

void CScreen_Set_Maintenance::OnMaintenance_Label_Set()
{
	m_msg_equip_id.SetTextFont(Func.mp_maintenance_font);
	m_msg_equip_id.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 3, 2);
	m_msg_equip_id.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_msg_equip_no.SetTextFont(Func.mp_maintenance_font);
	m_msg_equip_no.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 3, 2);
	m_msg_equip_no.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_msg_empty_id.SetTextFont(Func.mp_maintenance_font);
	m_msg_empty_id.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 3, 2);
	m_msg_empty_id.SetTextAlign(CFloatST::STA_CENTER, 10);

}

void CScreen_Set_Maintenance::OnMaintenance_Digital_Count_Set()
{
	m_dgt_flicker_time.SubclassDlgItem(IDC_DGT_FLICKER_TIME, this);
	m_dgt_flicker_time.SetStyle(IDB_BIG3, 5);

	m_dgt_change_time.SubclassDlgItem(IDC_DGT_CHANGE_TIME, this);
	m_dgt_change_time.SetStyle(IDB_BIG3, 5);

}

void CScreen_Set_Maintenance::OnMaintenance_Lamp_Change()
{
	int i;

	for(i = 0; i <= SELFCHECK; i++)
	{
		OnMaintenance_Red_State_Change(i);      // RED Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		OnMaintenance_Yellow_State_Change(i);   // YELLOW Ÿ�� ���� ��ư ���� ��ȯ �Լ�
		OnMaintenance_Green_State_Change(i);    // GREEN Ÿ�� ���� ��ư ���� ��ȯ �Լ�
	}
}

void CScreen_Set_Maintenance::OnMaintenance_File_Backup(CString strTargetFile, CString strCreateFile, CString strBackupFolder, CString strBackupFile, BOOL b_over_write)
{
	CString str_backup_file;	// ������ ��� ������ [���+���ϸ�+Ȯ����] ���� ����
	int n_existence;			// ���� ���� ���� �÷���
	int n_response;				// ��ȭ ���� ���� �÷���
	
	char chr_file[256];
	char szDrive[4] = {'\0',};
	char szDir[MAX_PATH] = {'\0',};
	char szName[MAX_PATH] = {'\0',};
	char szExt[MAX_PATH] = {'\0',};

	CDialog_Select  select_dlg;

	// **************************************************************************
	// ����� ������ �̹� �����ϴ��� �˻��Ѵ�                                    
	// -> �̹� �����ϴ� ��쿡�� ����ڿ��� ����� ���� ���� �ȴ�              
	// **************************************************************************
	sprintf(chr_file, "%s", strCreateFile);

	n_existence = access(chr_file,0) ;

	if (n_existence != -1)  // ���� ����
	{
		st_msg.mstr_confirm_msg = _T("Backup File Exist~! Overwrite??");

		n_response = select_dlg.DoModal();
		if (n_response == IDCANCEL)
			return ; 
	}
	// **************************************************************************

	// **************************************************************************
	// ��� ���� �����Ѵ�                                                        
	// **************************************************************************
	_tsplitpath(strTargetFile, szDrive, szDir, szName, szExt); 
	str_backup_file.Format("%s\\%s%s", strBackupFolder, strBackupFile, szExt);
	
	CopyFile(strTargetFile, str_backup_file, !b_over_write);
	// **************************************************************************
}

// ******************************************************************************
//  Tower Lamp ȭ�� ���� ���� ��û �޽���                                        
// ******************************************************************************
LRESULT CScreen_Set_Maintenance::OnMaintenance_Info_Apply(WPARAM wParam, LPARAM lParam)  
{
	OnMaintenance_Data_Apply();			// Tower Lamp ȭ�� ���� ���� ���� ������ ���� �Լ�

	OnMaintenance_LogFile_Create();

	OnMaintenance_Data_BackUp();

	OnMaintenance_ListBar_Info_Send();   // Ÿ��Ʋ �� ȭ�鿡 ���� ��� ��û�ϴ� �Լ�

	return 0;
}

void CScreen_Set_Maintenance::OnRLampClick(UINT nID)
{
	int idx = nID - ID_LAMP_RLAMP;		// �迭 ��ġ ����

	mn_r_lamp[1][idx]++;
	if (mn_r_lamp[1][idx]<0 || mn_r_lamp[1][idx]>2)
		mn_r_lamp[1][idx] = 0;

	OnMaintenance_Red_State_Change(idx);		// RED Ÿ�� ���� ��ư ���� ��ȯ �Լ�
}

void CScreen_Set_Maintenance::OnYLampClick(UINT nID)
{
	int idx = nID - ID_LAMP_YLAMP;		// �迭 ��ġ ����
	
	mn_y_lamp[1][idx]++;
	if (mn_y_lamp[1][idx]<0 || mn_y_lamp[1][idx]>2)
		mn_y_lamp[1][idx] = 0;

	OnMaintenance_Yellow_State_Change(idx);	// YELLOW Ÿ�� ���� ��ư ���� ��ȯ �Լ�
}

void CScreen_Set_Maintenance::OnGLampClick(UINT nID)
{
	int idx = nID - ID_LAMP_GLAMP;		// �迭 ��ġ ����
	
	mn_g_lamp[1][idx]++;
	if (mn_g_lamp[1][idx]<0 || mn_g_lamp[1][idx]>2)
		mn_g_lamp[1][idx] = 0;

	OnMaintenance_Green_State_Change(idx);		// GREEN Ÿ�� ���� ��ư ���� ��ȯ �Լ�
}

void CScreen_Set_Maintenance::OnDgtFlickerTime() 
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	char chr_buf[20] ;

	mn_lamp_flicker_time[1] = m_dgt_flicker_time.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_lamp_flicker_time[1], chr_buf, 10));

	st_msg.mstr_keypad_msg = _T("Lamp Flicker Time Setting");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(10, 10000, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);

	mn_lamp_flicker_time[1] = atoi(mstr_temp);
	m_dgt_flicker_time.SetValue(mn_lamp_flicker_time[1]);
}

void CScreen_Set_Maintenance::OnChkBuzzer() 
{
	if (m_chk_buzzer.GetCheck() == TRUE)	mn_buzzer_mode[1] = TRUE;
	else									mn_buzzer_mode[1] = FALSE;

	OnMaintenance_Buzzer_Change();
}

void CScreen_Set_Maintenance::OnBtnDataBackup() 
{
	CString str_year, str_month, str_day;	// ������ ��, ��, �� ���� ����
	CString str_back_day, str_back_time;
	CString str_back_file;					// ��� ���ϸ� ���� ����
	CString str_back_info;					// ����� ���� ���� ���� ���� [����+���ϸ�+Ȯ����]
	CString str_temp_file;					// ���ϸ� �ӽ� ���� ����
	CString str_chk_ext;					// ���� Ȯ���� ���� ����
	CString str_device;
	int n_response;							// ��ȭ ���� ���� �÷���
	int n_year, n_month, n_day;				// ��, ��, �� ���� ����
	CString str_cur_hour, str_cur_min, str_cur_sec;
	
	char chr_buf[20];

	CDialog_Select   select_dlg;
	CDialog_Message  msg_dlg;

	st_msg.mstr_confirm_msg = _T("Do you back up environment Setting information?");

	n_response = select_dlg.DoModal();

	if (n_response == IDOK)
	{
		// ******************************************************************************
		// ��� ���ϸ����� ����� ��¥ ���� �����Ѵ�                                     
		// ******************************************************************************
		COleDateTime cur_time = COleDateTime::GetCurrentTime();
		n_year = cur_time.GetYear();  
		n_month = cur_time.GetMonth();  
		n_day = cur_time.GetDay();  

		str_year = LPCTSTR(_itoa( n_year, chr_buf, 10 ));  
		str_month.Format("%02d", n_month);
		str_day.Format("%02d", n_day);

		str_back_day = str_year;
		str_back_day += str_month;
		str_back_day += str_day;
		// ******************************************************************************

		// **************************************************************************
		// ���� �ð� ���� ��´�                                                     
		// **************************************************************************
		str_cur_hour.Format("%02d",cur_time.GetHour());	// ���� �ð� ������ �����Ѵ�.
		str_cur_min.Format("%02d",cur_time.GetMinute());	// ���� �� ������ �����Ѵ�. 
		str_cur_sec.Format("%02d",cur_time.GetSecond());	// ���� �� ������ �����Ѵ�. 

		str_back_time = str_cur_hour;
		str_back_time += str_cur_min;
		str_back_time += str_cur_sec;
		// **************************************************************************

		// ******************************************************************************
		// ������ ���� ���� ���� ���� ����Ѵ�                                           
		// -> ����� ���� ������ �����ϴ��� �˻��ϰ� �ȴ�                                
		// ******************************************************************************
		str_back_file = "c:\\Backup\\" + str_back_day + "_" + str_back_time;  // ��� ���ϸ� ���� [��¥+���ϸ�]

		CopyFolder(st_path.mstr_path_dvc, str_back_file);
		CopyFolder(st_path.mstr_basic_folder, str_back_file);
	}
}

void CScreen_Set_Maintenance::OnPadEquipNo() 
{
	int n_response;  // ��ȭ ���� ���� �÷���

	CDialog_Keyboard  keyboard_dlg;

	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_NO))->GetWindowText(mstr_equip_no[1]);
	
	if (mstr_equip_no[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();			// Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
		mstr_equip_no[1].MakeUpper();
		mstr_equip_no[1].TrimLeft(' ');               
		mstr_equip_no[1].TrimRight(' ');

		st_msg.mstr_typing_msg = mstr_equip_no[1];  // Ű���� ���� ��� ������ ����
	}

	n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		mstr_equip_no[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����

		mstr_equip_no[1].MakeUpper();
		mstr_equip_no[1].TrimLeft(' ');               
		mstr_equip_no[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_NO))->SetWindowText(mstr_equip_no[1]);
	}
}

void CScreen_Set_Maintenance::OnPadEquipId() 
{
	int n_response;  // ��ȭ ���� ���� �÷���

	CDialog_Keyboard  keyboard_dlg ;

	((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_ID))->GetWindowText(mstr_equip_id[1]) ;
	if (mstr_equip_id[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();          // Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
		mstr_equip_id[1].MakeUpper();
		mstr_equip_id[1].TrimLeft(' ');               
		mstr_equip_id[1].TrimRight(' ');

		st_msg.mstr_typing_msg = mstr_equip_id[1];  // Ű���� ���� ��� ������ ����
	}

	n_response = keyboard_dlg.DoModal();
	if (n_response == IDOK)
	{
		mstr_equip_id[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����

		mstr_equip_id[1].MakeUpper();
		mstr_equip_id[1].TrimLeft(' ');               
		mstr_equip_id[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_EQUIP_ID))->SetWindowText(mstr_equip_id[1]);
	}
}

void CScreen_Set_Maintenance::OnBtnPasswordChangeLevle1() 
{
	int n_response;  // ��ȭ ���� ���� �÷���

	CDialog_Pass_Change  pass_cng_dlg;

	st_handler.mstr_pass_level=_T("Level1");  // ��ȣ ���� : ����Ʈ ����
	n_response = pass_cng_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (n_response == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CScreen_Set_Maintenance::OnBtnPasswordChangeLevle2() 
{
	int n_response;  // ��ȭ ���� ���� �÷���

	CDialog_Pass_Change  pass_cng_dlg;

	st_handler.mstr_pass_level=_T("Level2");  // ��ȣ ���� : ƼĪ ����
	n_response = pass_cng_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (n_response == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CScreen_Set_Maintenance::OnBtnMaintenanceApply() 
{
	int n_response;		// ��ȭ ���� ���� �÷���

	CDialog_Select  select_dlg;

	st_msg.mstr_confirm_msg = _T("Maintenance : Setting Data Apply?");

	n_response = select_dlg.DoModal();

	if (n_response == IDOK)
	{
		OnMaintenance_Data_Apply();			// Tower Lamp ȭ�� ���� ���� ���� ������ ���� �Լ�

		OnMaintenance_LogFile_Create();

		OnMaintenance_Data_Set();
		
		OnMaintenance_Data_BackUp();

		OnMaintenance_ListBar_Info_Send();	// ����Ʈ �� ȭ�鿡 ���� ��� ��û�ϴ� �Լ�
		

	}
	else if (n_response == IDCANCEL)
	{
	}
}

void CScreen_Set_Maintenance::OnSetfocusEditEquipId() 
{
	m_btn_maintenance_apply.SetFocus();
}

void CScreen_Set_Maintenance::OnSetfocusEditEquipNo() 
{
	m_btn_maintenance_apply.SetFocus();
}

void CScreen_Set_Maintenance::OnMaintenance_LogFile_Create()
{
	int i;
	CString str_msg;
	CString str_data;
	CString str_mode;
	int Ret = RET_GOOD;

	for(i=0; i<=SELFCHECK; i++)
	{
		if (i == 0)			str_mode = "STOP";
		else if (i == 1)	str_mode = "RUN";
		else if (i == 2)	str_mode = "JAM";
		else if (i == 3)	str_mode = "LOT END";
		else if (i == 4)	str_mode = "INIT";
		else if (i == 5)	str_mode = "WARNING";
		else if (i == 6)	str_mode = "LOCK";
//		else if (i == 7)	str_mode = "SELF CHECK";
		else if (i == 7)	str_mode = "EMPTY RUN";
		
		// **********************************************************************
		// RED Ÿ�� ���� ���¸� Ȯ���Ѵ�                                         
		// **********************************************************************
		if (mn_r_lamp[0][i] != mn_r_lamp[1][i])
		{
			if (mn_r_lamp[1][i] == 0)		str_data = "OFF";
			else if (mn_r_lamp[1][i] == 1)	str_data = "ON";
			else if (mn_r_lamp[1][i] == 2)	str_data = "FLICKER";

			str_msg.Format("[Tower Lamp] %s Status : RED LAMP was changed by %s", str_mode, str_data);
			Func.On_LogFile_Add(0, str_msg);
			Func.On_LogFile_Add(99, str_msg);
		}
		// **********************************************************************

		// **********************************************************************
		// YELLOW Ÿ�� ���� ���¸� Ȯ���Ѵ�                                      
		// **********************************************************************
		if (mn_y_lamp[0][i] != mn_y_lamp[1][i])
		{
			if (mn_y_lamp[1][i] == 0)		str_data = "OFF";
			else if (mn_y_lamp[1][i] == 1)	str_data = "ON";
			else if (mn_y_lamp[1][i] == 2)	str_data = "FLICKER";

			str_msg.Format("[Tower Lamp] %s Status : YELLOW LAMP was changed by %s", str_mode, str_data);
			Func.On_LogFile_Add(0, str_msg);
			Func.On_LogFile_Add(99, str_msg);
		}
		// **********************************************************************

		// **********************************************************************
		// GREEN Ÿ�� ���� ���¸� Ȯ���Ѵ�                                       
		// **********************************************************************
		if (mn_g_lamp[0][i] != mn_g_lamp[1][i])
		{
			if (mn_g_lamp[1][i] == 0)		str_data = "OFF";
			else if (mn_g_lamp[1][i] == 1)	str_data = "ON";
			else if (mn_g_lamp[1][i] == 2)	str_data = "FLICKER";

			str_msg.Format("[Tower Lamp] %s Status : GREEN LAMP was changed by %s", str_mode, str_data);
			Func.On_LogFile_Add(0, str_msg);
			Func.On_LogFile_Add(99, str_msg);
		}
		// **********************************************************************
	}

	if (mn_buzzer_mode[0] != mn_buzzer_mode[1])
	{
		if (mn_buzzer_mode[1] == 0)
		{
			str_data = _T("On");
		}
		else
		{
			str_data = _T("Off");
		}
		
		str_msg = "[TowerLamp] Buzzer Mode was changed by " + str_data;
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

	if (mn_lamp_flicker_time[0] != mn_lamp_flicker_time[1])
	{
		str_data.Format("%d", (int)mn_lamp_flicker_time[1]);
		str_msg = "[TowerLamp] Lamp Flicker Time was changed by " + str_data + "(msec)";
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

	if (mn_lamp_change_time[0] != mn_lamp_change_time[1])
	{
		str_data.Format("%d", (int)mn_lamp_change_time[1]);
		str_msg = "[TowerLamp] Lamp Change Time was changed by " + str_data + "(Sec)";
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

	if (mstr_equip_no[0] != mstr_equip_no[1])
	{
		str_msg = "[TowerLamp] EQUIP NO was changed by " + mstr_equip_no[1];
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

	if (mstr_equip_id[0] != mstr_equip_id[1])
	{
		str_msg = "[TowerLamp] EQUIP ID was changed by " + mstr_equip_id[1];
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

	if (mstr_empty_id[0] != mstr_empty_id[1])
	{
		str_msg = "[TowerLamp] EMPTY ID was changed by " + mstr_empty_id[1];
		Func.On_LogFile_Add(0, str_msg);
		Func.On_LogFile_Add(99, str_msg);
	}

}

void CScreen_Set_Maintenance::OnDgtChangeTime() 
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	char chr_buf[20] ;

	mn_lamp_change_time[1] = m_dgt_change_time.GetValue();
	mstr_temp = LPCTSTR(_itoa(mn_lamp_change_time[1], chr_buf, 10));

	st_msg.mstr_keypad_msg = _T("Lamp Change Time Setting");
    
	st_msg.mstr_keypad_val = mstr_temp;

	mstr_temp = KeyPad.GetNumEditString_I(0, 100, atoi(st_msg.mstr_keypad_val), st_msg.mstr_keypad_msg);
	mn_lamp_change_time[1] = atoi(mstr_temp);
	m_dgt_change_time.SetValue(mn_lamp_change_time[1]);
}

void CScreen_Set_Maintenance::OnBtnStepSave() 
{
	char path[MAX_PATH] = {0};
	//������ �۾���θ� ���� ���� �Ѵ�.
	GetCurrentDirectory(MAX_PATH, path);
	
	CFileDialogST	dlg(FALSE, NULL, "Step_back", OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Text File(*.txt)\0*.txt\0"), this);
	CString			sPathName, str_temp_file, str_ext, str_chk_ext;
	int				nRetValue, n_pos;
	
	BOOL			bResult; // = find.FindFile(str_file);
	
	CFileFind find;
	
	nRetValue = dlg.DoModal();
	
	if (nRetValue == IDOK)
	{
		sPathName = dlg.GetPathName();   // ȭ�鿡 �������� ������ ������ ��� �� ������ �̸��� �����!!
		
		n_pos = sPathName.Find(".txt");
		if(n_pos == -1)
			n_pos = sPathName.Find(".TXT");
		if(n_pos == -1)
			sPathName += ".txt";
		
		
		bResult = find.FindFile(sPathName);
		find.Close();
		
		if(bResult != 0)
		{
			DeleteFile(sPathName);
		}
		
		(st_msg.str_step_savefile_name).Format("%s", dlg.GetFileName());
		(st_msg.str_step_savefile_path).Format("%s", dlg.GetFileDir());

		OnMaintenance_Step_Save();
		
		SetCurrentDirectory(path);						// �۾� ��θ� ���� ��η� ���� �Ѵ�.
	}
	else if (nRetValue == IDCANCEL)
	{
		
	}
}

void CScreen_Set_Maintenance::OnMaintenance_Step_Save()
{
	CString mstr_file_name;    // ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;  // �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;  // �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int mn_existence, i;      // ���� ���� ���� ���� �÷��� 
	char fileName[256];    // �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;            // ���Ͽ� ���� ������ ���� ���� 
	
	CString str_title_time, str_file_time, str_new_file;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int n_cur_year, n_cur_month, n_cur_day;		// ��, ��, �� ���� ����
	int n_cur_hour, n_cur_minute, n_cur_second;		// ��, ��, �� ���� ����
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur;							// Ÿ��Ʋ �ð� ���� ����

	CString str_temp_data;
	int n_length;

	CString strData, str_time;
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	mtime_cur = COleDateTime::GetCurrentTime();
	
	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  

	n_cur_hour = mtime_cur.GetHour();
	n_cur_minute = mtime_cur.GetMinute();
	n_cur_second = mtime_cur.GetSecond();
	
	str_new_file.Format("DATA%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
	str_time.Format("%04d/%02d/%02d %02d:%02d:%02d", n_cur_year, n_cur_month, n_cur_day, n_cur_hour, n_cur_minute, n_cur_second);
		
	mstr_create_file = st_msg.str_step_savefile_path + st_msg.str_step_savefile_name;
	mstr_create_file += ".TXT";
	
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file = st_msg.str_step_savefile_path + st_msg.str_step_savefile_name;
		mstr_create_file += ".TXT";
		/* ********************************************************************** */
	}
	
	/* ************************************************************************** */
	/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
	/* ************************************************************************** */
	if ((fp=fopen(mstr_create_file,"a+"))==NULL)  
	{
		Func.MsgLog("���� ���� ����!..");
		return;
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ���Ͽ� Ÿ��Ʋ ���� ����Ѵ�                                                */
	/* ************************************************************************** */
	mstr_content = "********************************************************************************************************";
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "AMT8520 [SSD Interface Tester] : " + g_ver.GetDate() );
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "Thread Step Info File");
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "DIVISION  : AMT Coporation");
	fprintf(fp,"%s\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "File Name : " + st_msg.str_step_savefile_path + st_msg.str_step_savefile_name);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "Save Time : " + str_time);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content = "********************************************************************************************************";
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content = "+==========================+=============================================================================================================================+";
	fprintf(fp,"%s\r\n",mstr_content);
	
	mstr_content = "|  Step Trace              |                                                    History                                                                  |";
	fprintf(fp,"%s\n", mstr_content );
	
	mstr_content = "+==========================+=============================================================================================================================+";
	fprintf(fp,"%s\r\n",mstr_content) ;
	
	str_temp_data = "";
	
	for (i = 0; i < 87; i++)
	{
		str_temp_data = st_work.mstr_last_change_date[i];
		mstr_content.Format("|  Step %02d Change LastDate |", i);
		Func.TextAndLine(130, mstr_content, str_temp_data, fp);
		
		if (st_work.mstr_history_step_back_up[i] != "")
		{
			str_temp_data = st_work.mstr_history_step_back_up[i];
			n_length = str_temp_data.GetLength();
			mstr_content.Format("|  Step %02d before History  |", i);
			mstr_content += str_temp_data;
			fprintf(fp,"%s\r\n", mstr_content);
			mstr_content =      "+--------------------------+-----------------------------------------------------------------------------------------------------------------------------+";
			fprintf(fp,"%s\r\n",mstr_content);
		}
		str_temp_data = st_work.mstr_history_step[i];
		n_length = str_temp_data.GetLength();
		mstr_content.Format("|  Step %02d History         |", i);

		if (str_temp_data == "")
		{
			strData.Format("%-124s |", "");
			mstr_content += strData;
			fprintf(fp,"%s\r\n", mstr_content) ;
		}
		else
		{
			mstr_content += str_temp_data;
			fprintf(fp,"%s\r\n", mstr_content);
		}
		mstr_content =      "+--------------------------+-----------------------------------------------------------------------------------------------------------------------------+";
		fprintf(fp,"%s\r\n",mstr_content);
	}
	
	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	
	fclose(fp);  /* ���� ���� */
}

BOOL CScreen_Set_Maintenance::CopyFolder(CString &strFrom, CString &strTo)
{	
	CFileFind finder;
	
    CString sWildCard = strFrom + "\\*.*";
	
    BOOL bFound;
    BOOL bWorking = finder.FindFile(sWildCard);
	
    bFound = bWorking;
	
    ::CreateDirectory(strTo, NULL);
	
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
		
        // skip . and .. files; otherwise, we"d
        // recur infinitely!
        if(finder.IsDots()) continue;
		
        // ���丮�̸� ��� ȣ��
        if(finder.IsDirectory())
        {
            CString sFrom2 = finder.GetFilePath();
            CString sTo2 = strTo + "\\" + finder.GetFileName();
            ::CreateDirectory(sTo2, NULL);
            
            CopyFolder(sFrom2, sTo2);
            continue;
        }
        ExCopyFile(finder.GetFilePath(), strTo);
    }
    return bFound;

}

int CScreen_Set_Maintenance::ExCopyFile(CString strFrom, CString strTo)
{
	SHFILEOPSTRUCT shfo;
	
    char pszFrom[1024] = {0};
    char pszTo[1024] = {0};
	
    WORD wFlag = 0;
	
    strcpy(pszFrom, (LPCTSTR)strFrom);
    strcpy(pszTo, (LPCTSTR)strTo);
	
    pszFrom[lstrlen(pszFrom) + 1] = 0;
	
    shfo.hwnd = m_hWnd;
    shfo.wFunc = FO_COPY;
    shfo.pFrom = pszFrom;
    shfo.pTo = pszTo;
    shfo.fFlags = 0;
    shfo.lpszProgressTitle = "File Copy..";
	
    return SHFileOperation(&shfo);

}

void CScreen_Set_Maintenance::OnMaintenance_ComboBox_Set()
{

}

int CScreen_Set_Maintenance::OnMaintenance_Combine_Bin(int n_val)
{
	int Ret_val;
	
	switch(n_val)
	{
	case 0:
		Ret_val = BIN_GOOD_RAIL;
		break;
		
	case 1:
		Ret_val = BIN_STAKCER_1;
		break;
		
	case 2:
		Ret_val = BIN_STAKCER_2;
		break;
		
	case 3:
		Ret_val = BIN_STAKCER_3;
		break;

	case 4:
		Ret_val = BIN_STAKCER_4;
		break;
	}
	return Ret_val;
}

int CScreen_Set_Maintenance::OnMaintenance_Combine_Value(int n_num)
{
	int Ret_val;
	
	switch(n_num)
	{
	case BIN_GOOD_RAIL:
		Ret_val = 0;
		break;
		
	case BIN_STAKCER_1:
		Ret_val = 0;
		break;
		
	case BIN_STAKCER_2:
		Ret_val = 1;
		break;
		
	case BIN_STAKCER_3:
		Ret_val = 2;
		break;

	case BIN_STAKCER_4:
		Ret_val = 3;
		break;
	}
	return Ret_val;
}

void CScreen_Set_Maintenance::OnPadEmptyId() 
{
	int n_response;  // ��ȭ ���� ���� �÷���
	
	CDialog_Keyboard  keyboard_dlg ;
	
	((CEdit*)GetDlgItem(IDC_EDIT_EMPTY_ID))->GetWindowText(mstr_empty_id[1]) ;
	if (mstr_empty_id[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();          // Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
		mstr_empty_id[1].MakeUpper();
		mstr_empty_id[1].TrimLeft(' ');               
		mstr_empty_id[1].TrimRight(' ');
		
		st_msg.mstr_typing_msg = mstr_empty_id[1];  // Ű���� ���� ��� ������ ����
	}
	
	n_response = keyboard_dlg.DoModal();
	if (n_response == IDOK)
	{
		mstr_empty_id[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����
		
		mstr_empty_id[1].MakeUpper();
		mstr_empty_id[1].TrimLeft(' ');               
		mstr_empty_id[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_EMPTY_ID))->SetWindowText(mstr_empty_id[1]);
	}	
}

void CScreen_Set_Maintenance::OnSetfocusEditEmptyId() 
{
	// TODO: Add your control notification handler code here
		m_btn_maintenance_apply.SetFocus();
}


////2015.0604
void CScreen_Set_Maintenance::OnBtnFtpConnect() 
{
	
	// TODO: Add your control notification handler code here
	CString strSend;
	CString strFtp;

	if(mstr_ftp_ip[1] == "")
	{
		strSend.Format( "Please enter Ftp IP"  );
		Func.On_LogFile_Add( LOG_TOTAL,  strSend );
		return;
	}
	if(mstr_ftp_user[1] == "")
	{
		strSend.Format( "Please enter Ftp User"  );
		Func.On_LogFile_Add( LOG_TOTAL,  strSend );
		return;
	}
	if(mstr_ftp_pw[1] == "")
	{
		strSend.Format( "Please enter Ftp Password"  );
		Func.On_LogFile_Add( LOG_TOTAL,  strSend );
		return;
	}
	if(mn_ftp_port[1] <= 0)
	{
		strSend.Format( "Please enter Ftp Port"  );
		Func.On_LogFile_Add( LOG_TOTAL,  strSend );
		return;
	}
	
	//�����ϰ����ϴ� FTP ������ IP �ּҿ� ����ڸ�, ��ȣ �Է�
	if( m_pFtp.Open(mstr_ftp_ip[1], mstr_ftp_user[1], mstr_ftp_pw[1], mn_ftp_port[1]) )
	{
		m_strPathRemote = m_pFtp.RCD();
		OnUpdateRemote(m_strPathRemote);
		
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			strFtp = "Ftp Connection Complete";
			sprintf(st_msg.c_normal_msg, "%s", strFtp);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			strFtp = "Ftp Connection Fail";
			sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
}

void CScreen_Set_Maintenance::OnUpdateRemote(CString strPath)
{
	CString strTemp;
	CString strDirectory;
	
	BOOL bContinue = TRUE;
	
	unsigned long size;
	
	CTime time;
	
	int bWorking;
	int index;
	
	m_listRemote.DeleteAllItems();
	
	if(strPath != _T("/"))
	{
		strTemp = _T("[..]");
		// �߰����� �߰��� index �� �޾ƿ�
		index = m_listRemote.InsertItem(0, strTemp);
	}
	//ã�� ȭ�� ����
	CFTPFileFind	finder(m_pFtp.m_pConnection);
	bWorking = finder.FindFile(strPath);
	
	while(bWorking)
	{
		//FTP ������ ���ϸ��� ����
		bWorking = finder.FindNextFile();		// ȭ���ϳ� ã��
		
		// ȭ���̸� �߰�
		strTemp = finder.GetFileName();
		if(strTemp == _T(".")) continue;	// "." �̸� �������� �Ѿ
		
		if(finder.IsDirectory())
		{
			//���丮�� [ ] �� ����	
			strDirectory.Format(_T("[%s]"), strTemp);
		}
		else
		{
			strDirectory = strTemp;
		}
		// �߰����� �߰��� index �� �޾ƿ�
		index = m_listRemote.InsertItem(0, strDirectory);
		
		// ȭ��ũ�� �߰�
		size = finder.GetLength();
		strTemp.Format(_T("%d"),size);
		m_listRemote.SetItemText(index, 1, strTemp);
		
		// ȭ�ϳ�¥ �߰�
		finder.GetLastWriteTime(time);
		strTemp = time.Format("%y%m%d %H:%M");
		m_listRemote.SetItemText(index, 2, strTemp);
		
	}
	
	UpdateData(FALSE);	// ȭ�鿡 �ִ°�����.....
}

void CScreen_Set_Maintenance::OnInitListRemote()
{
	CRect rect;
	
	m_listRemote.GetClientRect(&rect);
	// �÷� �߰��ϴ°� 
	m_listRemote.InsertColumn(0, _T("File Name"), LVCFMT_LEFT, rect.Width() - 160);
	m_listRemote.InsertColumn(1, _T("Size"), LVCFMT_RIGHT, 60);
	m_listRemote.InsertColumn(2, _T("Date"), LVCFMT_LEFT, 80);
}

void CScreen_Set_Maintenance::OnPadFtpIp() 
{
	// TODO: Add your control notification handler code here
	int n_response;  // ��ȭ ���� ���� �÷���
	
	CDialog_Keyboard  keyboard_dlg;
	
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_IP))->GetWindowText(mstr_ftp_ip[1]);
	
	if (mstr_ftp_ip[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();			// Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
//		mstr_ftp_ip[1].MakeUpper();
		mstr_ftp_ip[1].TrimLeft(' ');               
		mstr_ftp_ip[1].TrimRight(' ');
		
		st_msg.mstr_typing_msg = mstr_ftp_ip[1];  // Ű���� ���� ��� ������ ����
	}
	
	n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		mstr_ftp_ip[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����
		
//		mstr_ftp_ip[1].MakeUpper();
		mstr_ftp_ip[1].TrimLeft(' ');               
		mstr_ftp_ip[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_FTP_IP))->SetWindowText(mstr_ftp_ip[1]);
	}	
}

void CScreen_Set_Maintenance::OnPadFtpUser() 
{
	// TODO: Add your control notification handler code here
	int n_response;  // ��ȭ ���� ���� �÷���
	
	CDialog_Keyboard  keyboard_dlg;
	
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_USER))->GetWindowText(mstr_ftp_user[1]);
	
	if (mstr_ftp_user[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();			// Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
//		mstr_ftp_user[1].MakeUpper();
		mstr_ftp_user[1].TrimLeft(' ');               
		mstr_ftp_user[1].TrimRight(' ');
		
		st_msg.mstr_typing_msg = mstr_ftp_user[1];  // Ű���� ���� ��� ������ ����
	}
	
	n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		mstr_ftp_user[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����
		
//		mstr_ftp_user[1].MakeUpper();
		mstr_ftp_user[1].TrimLeft(' ');               
		mstr_ftp_user[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_FTP_USER))->SetWindowText(mstr_ftp_user[1]);
	}		
}

void CScreen_Set_Maintenance::OnPadFtpPw() 
{
	// TODO: Add your control notification handler code here
	int n_response;  // ��ȭ ���� ���� �÷���
	
	CDialog_Keyboard  keyboard_dlg;
	
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PW))->GetWindowText(mstr_ftp_pw[1]);
	
	if (mstr_ftp_pw[1].IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();			// Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
//		mstr_ftp_pw[1].MakeUpper();
		mstr_ftp_pw[1].TrimLeft(' ');               
		mstr_ftp_pw[1].TrimRight(' ');
		
		st_msg.mstr_typing_msg = mstr_ftp_pw[1];  // Ű���� ���� ��� ������ ����
	}
	
	n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		mstr_ftp_pw[1] = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����
		
//		mstr_ftp_pw[1].MakeUpper();
		mstr_ftp_pw[1].TrimLeft(' ');               
		mstr_ftp_pw[1].TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_FTP_PW))->SetWindowText(mstr_ftp_pw[1]);
	}		
}

void CScreen_Set_Maintenance::OnPadFtpPort() 
{
	// TODO: Add your control notification handler code here
	int n_response;  // ��ȭ ���� ���� �÷���
	CString strFtp_port;

	CDialog_Keyboard  keyboard_dlg;
	
	strFtp_port.Format("%s", mn_ftp_port[1]);
	((CEdit*)GetDlgItem(IDC_EDIT_FTP_PORT))->GetWindowText(strFtp_port);
	
	if (strFtp_port.IsEmpty())
	{
		(st_msg.mstr_typing_msg).Empty();			// Ű���� ���� ��� ������ �ʱ�ȭ
	}
	else 
	{
//		strFtp_port.MakeUpper();
		strFtp_port.TrimLeft(' ');               
		strFtp_port.TrimRight(' ');
		
		st_msg.mstr_typing_msg = strFtp_port;  // Ű���� ���� ��� ������ ����
	}
	
	n_response = keyboard_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		strFtp_port = st_msg.mstr_typing_msg;  // Ű���� ���� �Է� ���� Ŭ���� ������ ����
		
//		strFtp_port.MakeUpper();
		strFtp_port.TrimLeft(' ');               
		strFtp_port.TrimRight(' ');
		((CEdit*)GetDlgItem(IDC_EDIT_FTP_PORT))->SetWindowText(strFtp_port);
		mn_ftp_port[1] = atoi(strFtp_port);
	}		
	
}

void CScreen_Set_Maintenance::OnDblclkListDirectory(NMHDR *pNMHDR, LRESULT *pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION pos;
	
	CString strTemp;
	CString strDirectory = m_strPathRemote;
	
	pos = m_listRemote.GetFirstSelectedItemPosition();	// ��ġ�� ����
	
	if(pos == NULL) return;	// �ƹ��͵� ���� �ȉ����� �׳� return
	
	pos = pos -1;		// 1 �������
	strTemp = m_listRemote.GetItemText((int)pos,0);				// ȭ�ϸ��� ����
	
	// ���丮�̸�
	if(strTemp.GetAt(0) == '[')
	{
		// ������ '[' �� ']' ����  �ֳ��ϸ� ���丮���� [ ] �� �ѷ��ο� �����Ƿ�
		strTemp = strTemp.Left(strTemp.GetLength() -1);
		strTemp = strTemp.Right(strTemp.GetLength() -1);
		
		// ��ü ���丮 �̸� ���ϱ�
		if(strTemp == _T(".."))
		{
			// ".."�� ���� �� ���� ���� �ö󰡴� ���
			strDirectory = m_strPathRemote.Left(m_strPathRemote.ReverseFind('/'));	// ������ '/' ������ ����
			
			if(strDirectory == _T(""))
			{
				strDirectory += _T("/");
			}
			m_strPathRemote = strDirectory;
		}
		else
		{
			strDirectory += _T("/");
			strDirectory += strTemp;
			
			strDirectory.Format(_T("%s%s%s"), m_strPathRemote, _T("/"), strTemp);
			m_strPathRemote = strDirectory;
		}
		
		//���丮 �̵�
		if(!m_pFtp.RCD(m_strPathRemote))
		{
			m_strPathRemote = m_pFtp.RCD();
			AfxMessageBox(_T("���丮 �̵��� �Ұ����Դϴ�."));
			
			return;
		}
		
		//���� ���丮�� ȭ�鿡 ǥ��
		m_strPathRemote = m_pFtp.RCD();
		
		OnUpdateRemote(m_strPathRemote);

		((CEdit*)GetDlgItem(IDC_EDIT_FTP_PATH))->SetWindowText(m_strPathRemote);
		mstr_ftp_path[1] = m_strPathRemote;

	}	// end of if	
	*pResult = 0;
}

void CScreen_Set_Maintenance::OnBtnScrapCodeRead() 
{
	// TODO: Add your control notification handler code here
	Func.Load_scrap_code_Data();

}

void CScreen_Set_Maintenance::OnBtnFtpFileDown() 
{
	// TODO: Add your control notification handler code here
	CString strData;
	CString strFtp;
	int nRet;
	
	char chr_file_name[256];
	CString strDelete, strDelete1;
	CString str_temp_data;
	int n_existence;
	
	sprintf(chr_file_name, "%s", st_path.mstr_scrap_code);
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)
	{
		DeleteFile(chr_file_name);//C:\AMT8520\Setting\RETEST_RULE.TXT ���� �ٿ� ���� ��ġ
	}


	if( m_pFtp.Open(mstr_ftp_ip[1], mstr_ftp_user[1], mstr_ftp_pw[1], mn_ftp_port[1]) )
	{ 
		strData.Format(_T("%s/%s"), mstr_ftp_path[1], mstr_ftp_scrap_code_name[1]);// /Users/MDL/MDL_HNDL/WP_SKEL_SSD/RETEST_RULE, RETEST_RULE.TXT -> FTP���� ���, FTP���� �ٿ� ���� ����
		
		nRet = m_pFtp.DownLoad(strData, st_path.mstr_scrap_code);		
		m_pFtp.Close();

		if(nRet == CTL_YES)
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				OnRadioUseFtpMode();
				strFtp = "Ftp File Down Complete";
				sprintf(st_msg.c_normal_msg, "%s", strFtp);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
			}	
		}
		else
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				OnRadioNotuseFtpMode();
				strFtp = "Ftp File Down Fail";
				sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}	
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			OnRadioNotuseFtpMode();
			strFtp = "Ftp File Down Fail";
			sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}	
	}
	
	m_pFtp.Close();	

}

void CScreen_Set_Maintenance::OnRadioNotuseFtpMode() 
{
	// TODO: Add your control notification handler code here
	if(mn_ftp_mode[1] == 0) return;
	mn_ftp_mode[1] = 0;
	g_dataMgr.SetDataTemp( &st_basic.mn_mode_ftp, (int)0 );
	OnMaintenance_FtpMode_Change();	
	
}

void CScreen_Set_Maintenance::OnRadioUseFtpMode() 
{
	
	
	int nResponse;								// ��ȭ ���ڿ� ���� ���� �� ���� ����
	
	CDialog_Pass_Check pass_chk_dlg ;			// ��ȣ �˻� ��ȭ ���� Ŭ���� ���� ���� 
	
	st_handler.mstr_pass_level = _T("MAX_COUNT");		// ��ȣ ���� ���� ���� 
	
	nResponse = pass_chk_dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		st_handler.mn_system_lock = FALSE;	// �ý��� ��� ���� ����
		
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
		if(mn_ftp_mode[1] == 1) return;
		mn_ftp_mode[1] = 2;
		g_dataMgr.SetDataTemp( &st_basic.mn_mode_ftp, (int)2 );
		OnMaintenance_FtpMode_Change();	
		return;
	}


	// TODO: Add your control notification handler code here
	if(mn_ftp_mode[1] == 1) return;
	mn_ftp_mode[1] = 1;
	g_dataMgr.SetDataTemp( &st_basic.mn_mode_ftp, (int)1 );
	OnMaintenance_FtpMode_Change();	
}

void CScreen_Set_Maintenance::OnRadioUseFtpMode2() 
{

	
	
	int nResponse;								// ��ȭ ���ڿ� ���� ���� �� ���� ����
	
	CDialog_Pass_Check pass_chk_dlg ;			// ��ȣ �˻� ��ȭ ���� Ŭ���� ���� ���� 
	
	st_handler.mstr_pass_level = _T("MAX_COUNT");		// ��ȣ ���� ���� ���� 
	
	nResponse = pass_chk_dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		st_handler.mn_system_lock = FALSE;	// �ý��� ��� ���� ����
		
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
		if(mn_ftp_mode[1] == 2) return;
		mn_ftp_mode[1] = 1;
		g_dataMgr.SetDataTemp( &st_basic.mn_mode_ftp, (int)1 );
		OnMaintenance_FtpMode_Change();		
		return;
	}

	// TODO: Add your control notification handler code here
	if(mn_ftp_mode[1] == 2) return;
	mn_ftp_mode[1] = 2;
	g_dataMgr.SetDataTemp( &st_basic.mn_mode_ftp, (int)2 );
	OnMaintenance_FtpMode_Change();		
}


void CScreen_Set_Maintenance::OnMaintenance_FtpMode_Change()
{
	if (mn_ftp_mode[1] < 0 || mn_ftp_mode[1] > 2)
	{
		mn_ftp_mode[1] = 0;
	}
		
	switch (mn_ftp_mode[1])
	{
	case 0:
		m_radio_use_ftp.SetCheck(FALSE);
		m_radio_use_local.SetCheck(FALSE);
		m_radio_notuse_ftp.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_use_ftp.SetCheck(TRUE);
		m_radio_use_local.SetCheck(FALSE);
		m_radio_notuse_ftp.SetCheck(FALSE);
		break;

	case 2:
		m_radio_use_ftp.SetCheck(FALSE);
		m_radio_use_local.SetCheck(TRUE);
		m_radio_notuse_ftp.SetCheck(FALSE);
		break;
	}
}
////


void CScreen_Set_Maintenance::OnBtnReciipeRule() 
{
	CString strData;
	CString strFtp;
	int nRet;
	
	char chr_file_name[256];
	CString strDelete, strDelete1;
	CString str_temp_data;
	int n_existence;
	
	sprintf(chr_file_name, "%s", st_path.mstr_recipe_rule);
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)
	{
		DeleteFile(chr_file_name);//C:\AMT8520\Setting\RECIPE_RULE.TXT ���� �ٿ� ���� ��ġ
	}
	
	
	if( m_pFtp.Open(mstr_ftp_ip[1], mstr_ftp_user[1], mstr_ftp_pw[1], mn_ftp_port[1]) )
	{ 
		strData.Format(_T("%s/%s"), mstr_ftp_path[1], mstr_ftp_recipe_rule_name[1]);// /Users/MDL/MDL_HNDL/WP_SKEL_SSD/RETEST_RULE, RETEST_RULE.TXT -> FTP���� ���, FTP���� �ٿ� ���� ����
		
		nRet = m_pFtp.DownLoad(strData, st_path.mstr_recipe_rule);		
		m_pFtp.Close();
		
		if(nRet == CTL_YES)
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				OnRadioUseFtpMode();
				strFtp = "Ftp recipe rule down complete";
				sprintf(st_msg.c_normal_msg, "%s", strFtp);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
			}	
		}
		else
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				OnRadioNotuseFtpMode();
				strFtp = "Ftp recipe rule down fail";
				sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}	
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			OnRadioNotuseFtpMode();
			strFtp = "Ftp recipe rule down fail";
			sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}	
	}
	
	m_pFtp.Close();		
}

void CScreen_Set_Maintenance::OnChangeEditFtpScrapCodeName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CScreen_Set_Maintenance::OnChangeEditFtpRecipeRuleName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
