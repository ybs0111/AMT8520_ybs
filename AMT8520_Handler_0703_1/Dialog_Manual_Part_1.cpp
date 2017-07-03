// Dialog_Manual_Part_1.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Manual_Part_1.h"

#include "COMI_Manager.h"
#include "IO_Manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_1 dialog


CDialog_Manual_Part_1::CDialog_Manual_Part_1(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Manual_Part_1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Manual_Part_1)
	//}}AFX_DATA_INIT
	Func.mp_manual_1 = NULL;
}


void CDialog_Manual_Part_1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Manual_Part_1)
	DDX_Control(pDX, IDC_BTN_FM_U_SMEMA_TR_SIGNAL_OFF, m_btn_fm_u_smema_tr_signal_off);
	DDX_Control(pDX, IDC_BTN_FM_L_SMEMA_REQ_SIGNAL_OFF, m_btn_fm_l_smema_req_signal_off);
	DDX_Control(pDX, IDC_BTN_BUF2_BOAT_CLAMP_OFF, m_btn_buf2_boat_clmap_off);
	DDX_Control(pDX, IDC_BTN_BUF1_BOAT_CLAMP_OFF, m_btn_buf1_boat_clamp_off);
	DDX_Control(pDX, IDC_RADIO_WARPAGE_CHK, m_radio_warpage_chk);
	DDX_Control(pDX, IDC_RADIO_ULD_CLAMP_UP, m_radio_uld_clamp_up);
	DDX_Control(pDX, IDC_RADIO_ULD_CLAMP_ON, m_radio_uld_clamp_on);
	DDX_Control(pDX, IDC_RADIO_ULD_CLAMP_OFF, m_radio_uld_clamp_off);
	DDX_Control(pDX, IDC_RADIO_ULD_CLAMP_DOWN, m_radio_uld_clamp_down);
	DDX_Control(pDX, IDC_RADIO_ULD_BOAT_CHK, m_radio_uld_boat_chk);
	DDX_Control(pDX, IDC_RADIO_LD_CLAMP_ON, m_radio_ld_clamp_on);
	DDX_Control(pDX, IDC_RADIO_LD_CLAMP_OFF, m_radio_ld_clamp_off);
	DDX_Control(pDX, IDC_RADIO_LD_BOAT_POS_2, m_radio_ld_boat_pos_2);
	DDX_Control(pDX, IDC_RADIO_LD_BOAT_POS_1, m_radio_ld_boat_pos_1);
	DDX_Control(pDX, IDC_RADIO_FM_U_SMEMA_TR_SIGNAL_CHK, m_radio_fm_u_smema_tr_signal_chk);
	DDX_Control(pDX, IDC_RADIO_FM_U_SMEMA_REQ_SIGNAL_CHK, m_radio_fm_u_smema_req_signal_chk);
	DDX_Control(pDX, IDC_RADIO_FM_L_SMEMA_TR_SIGNAL_CHK, m_radio_fm_l_smema_tr_signal_chk);
	DDX_Control(pDX, IDC_RADIO_FM_L_SMEMA_REQ_SIGNAL_CHK, m_radio_fm_l_smema_req_signal_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_2_UNCLAMP_CHECK, m_radio_buf_2_unclamp_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_2_ROTATOR_CHECK, m_radio_buf_2_rotator_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_2_CLAMP_CHECK, m_radio_buf_2_clamp_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_2_BOAT_CHECK, m_radio_buf_2_boat_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_1_UNCLAMP_CHECK, m_radio_buf_1_unclamp_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_1_ROTATOR_CHECK, m_radio_buf_1_rotator_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_1_CLAMP_CHECK, m_radio_buf_1_clamp_chk);
	DDX_Control(pDX, IDC_RADIO_BUF_1_BOAT_CHECK, m_radio_buf_1_boat_check);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_8, m_radio_boat_id_8);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_7, m_radio_boat_id_7);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_6, m_radio_boat_id_6);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_5, m_radio_boat_id_5);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_4, m_radio_boat_id_4);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_3, m_radio_boat_id_3);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_2, m_radio_boat_id_2);
	DDX_Control(pDX, IDC_RADIO_BOAT_ID_1, m_radio_boat_id_1);
	DDX_Control(pDX, IDC_RADIO_BOAT_CHK_2, m_radio_boat_chk_2);
	DDX_Control(pDX, IDC_RADIO_BOAT_CHK_1, m_radio_boat_chk_1);
	DDX_Control(pDX, IDC_BTN_ULD_M_BOAT_PLACE, m_btn_uld_m_boat_place);
	DDX_Control(pDX, IDC_BTN_ULD_M_BOAT_PICK_2, m_btn_uld_m_boat_pick_2);
	DDX_Control(pDX, IDC_BTN_ULD_M_BOAT_PICK_1, m_btn_uld_m_boat_pick_1);
	DDX_Control(pDX, IDC_BTN_LD_M_BOAT_PLACE_2, m_btn_ld_m_boat_place_2);
	DDX_Control(pDX, IDC_BTN_LD_M_BOAT_PLACE_1, m_btn_ld_m_boat_place_1);
	DDX_Control(pDX, IDC_BTN_LD_M_BOAT_PICK, m_btn_ld_m_boat_pick);
	DDX_Control(pDX, IDC_BTN_FM_U_SMEMA_TR_SIGNAL, m_btn_fm_u_smema_tr_signal);
	DDX_Control(pDX, IDC_BTN_FM_L_SMEMA_REQ_SIGNAL, m_btn_fm_l_smema_req_signal);
	DDX_Control(pDX, IDC_BTN_BUF2_BOAT_CLAMP, m_btn_buf2_boat_clamp);
	DDX_Control(pDX, IDC_BTN_BUF1_BOAT_CLAMP, m_btn_buf1_boat_clamp);
	DDX_Control(pDX, IDC_BTN_1_EXIT, m_btn_1_exit);
	DDX_Control(pDX, IDC_BTN_BUF2_ULD, m_btn_buf2_uld);
	DDX_Control(pDX, IDC_BTN_BUF2_LD, m_btn_buf2_ld);
	DDX_Control(pDX, IDC_BTN_BUF2_FLIPPER, m_btn_buf2_flipper);
	DDX_Control(pDX, IDC_BTN_BUF2_DEG_180, m_btn_buf2_deg_180);
	DDX_Control(pDX, IDC_BTN_BUF2_DEG_0, m_btn_buf2_deg_0);
	DDX_Control(pDX, IDC_BTN_BUF1_ULD, m_btn_buf1_uld);
	DDX_Control(pDX, IDC_BTN_BUF1_LD, m_btn_buf1_ld);
	DDX_Control(pDX, IDC_BTN_BUF1_FLIPPER, m_btn_buf1_flipper);
	DDX_Control(pDX, IDC_BTN_BUF1_DEG_180, m_btn_buf1_deg_180);
	DDX_Control(pDX, IDC_BTN_BUF1_DEG_0, m_btn_buf1_deg_0);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_8, m_btn_ld_x_wp_8);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_7, m_btn_ld_x_wp_7);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_6, m_btn_ld_x_wp_6);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_5, m_btn_ld_x_wp_5);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_4, m_btn_ld_x_wp_4);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_3, m_btn_ld_x_wp_3);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_2, m_btn_ld_x_wp_2);
	DDX_Control(pDX, IDC_BTN_LD_X_WP_1, m_btn_ld_x_wp_1);
	DDX_Control(pDX, IDC_LABEL_WARPAGE_CHECK, m_label_warpage_check);
	DDX_Control(pDX, IDC_LABEL_ID_CHECK, m_label_id_check);
	DDX_Control(pDX, IDC_LABEL_BOAT_CHECK, m_label_boat_check);
	DDX_Control(pDX, IDC_BTN_ULD_X_PLACE, m_btn_uld_x_place);
	DDX_Control(pDX, IDC_BTN_ULD_X_BUF_2, m_btn_uld_x_buf_2);
	DDX_Control(pDX, IDC_BTN_ULD_X_BUF_1, m_btn_uld_x_buf_1);
	DDX_Control(pDX, IDC_BTN_ULD_UPDN_PLACE_DN, m_btn_uld_updn_place_dn);
	DDX_Control(pDX, IDC_BTN_ULD_UPDN_BUF_2_PICK_DN, m_btn_uld_updn_buf_2_pick_dn);
	DDX_Control(pDX, IDC_BTN_ULD_UPDN_BUF_1_PICK_DN, m_btn_uld_updn_buf_1_pick_dn);
	DDX_Control(pDX, IDC_BTN_ULD_BOAT_CLAMP_ON, m_btn_uld_boat_clamp_on);
	DDX_Control(pDX, IDC_BTN_ULD_BOAT_CLAMP_OFF, m_btn_uld_boat_clamp_off);
	DDX_Control(pDX, IDC_BTN_LD_Z_PLACE_BUF_2, m_btn_ld_z_place_buf_2);
	DDX_Control(pDX, IDC_BTN_LD_Z_PLACE_BUF_1, m_btn_ld_z_place_buf_1);
	DDX_Control(pDX, IDC_BTN_LD_Z_PICK, m_btn_ld_z_pick);
	DDX_Control(pDX, IDC_BTN_LD_X_PICK, m_btn_ld_x_pick);
	DDX_Control(pDX, IDC_BTN_LD_X_ID_CHECK, m_btn_ld_x_id_check);
	DDX_Control(pDX, IDC_BTN_LD_X_BUF_2, m_btn_ld_x_buf_2);
	DDX_Control(pDX, IDC_BTN_LD_X_BUF_1, m_btn_ld_x_buf_1);
	DDX_Control(pDX, IDC_BTN_LD_BOAT_CLAMP_ON, m_btn_ld_boat_clamp_on);
	DDX_Control(pDX, IDC_BTN_LD_BOAT_CLAMP_OFF, m_btn_ld_boat_clamp_off);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Manual_Part_1, CDialog)
	//{{AFX_MSG_MAP(CDialog_Manual_Part_1)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_1_EXIT, OnBtn1Exit)
	ON_BN_CLICKED(IDC_BTN_LD_BOAT_CLAMP_ON, OnBtnLdBoatClampOn)
	ON_BN_CLICKED(IDC_BTN_LD_BOAT_CLAMP_OFF, OnBtnLdBoatClampOff)
	ON_BN_CLICKED(IDC_BTN_ULD_BOAT_CLAMP_ON, OnBtnUldBoatClampOn)
	ON_BN_CLICKED(IDC_BTN_ULD_BOAT_CLAMP_OFF, OnBtnUldBoatClampOff)
	ON_BN_CLICKED(IDC_BTN_BUF1_BOAT_CLAMP, OnBtnBuf1BoatClamp)
	ON_BN_CLICKED(IDC_BTN_BUF2_BOAT_CLAMP, OnBtnBuf2BoatClamp)
	ON_BN_CLICKED(IDC_BTN_FM_L_SMEMA_REQ_SIGNAL, OnBtnFmLSmemaReqSignal)
	ON_BN_CLICKED(IDC_BTN_FM_U_SMEMA_TR_SIGNAL, OnBtnFmUSmemaTrSignal)
	ON_BN_CLICKED(IDC_BTN_BUF1_BOAT_CLAMP_OFF, OnBtnBuf1BoatClampOff)
	ON_BN_CLICKED(IDC_BTN_BUF2_BOAT_CLAMP_OFF, OnBtnBuf2BoatClampOff)
	ON_BN_CLICKED(IDC_BTN_FM_L_SMEMA_REQ_SIGNAL_OFF, OnBtnFmLSmemaReqSignalOff)
	ON_BN_CLICKED(IDC_BTN_FM_U_SMEMA_TR_SIGNAL_OFF, OnBtnFmUSmemaTrSignalOff)
	ON_BN_CLICKED(IDC_BTN_LD_Z_PICK, OnBtnLdZPick)
	ON_BN_CLICKED(IDC_BTN_LD_Z_PLACE_BUF_1, OnBtnLdZPlaceBuf1)
	ON_BN_CLICKED(IDC_BTN_LD_Z_PLACE_BUF_2, OnBtnLdZPlaceBuf2)
	ON_BN_CLICKED(IDC_BTN_LD_X_PICK, OnBtnLdXPick)
	ON_BN_CLICKED(IDC_BTN_LD_X_BUF_1, OnBtnLdXBuf1)
	ON_BN_CLICKED(IDC_BTN_LD_X_BUF_2, OnBtnLdXBuf2)
	ON_BN_CLICKED(IDC_BTN_LD_X_ID_CHECK, OnBtnLdXIdCheck)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_1, OnBtnLdXWp1)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_2, OnBtnLdXWp2)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_3, OnBtnLdXWp3)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_4, OnBtnLdXWp4)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_5, OnBtnLdXWp5)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_6, OnBtnLdXWp6)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_7, OnBtnLdXWp7)
	ON_BN_CLICKED(IDC_BTN_LD_X_WP_8, OnBtnLdXWp8)
	ON_BN_CLICKED(IDC_BTN_LD_M_BOAT_PICK, OnBtnLdMBoatPick)
	ON_BN_CLICKED(IDC_BTN_LD_M_BOAT_PLACE_1, OnBtnLdMBoatPlace1)
	ON_BN_CLICKED(IDC_BTN_LD_M_BOAT_PLACE_2, OnBtnLdMBoatPlace2)
	ON_BN_CLICKED(IDC_BTN_ULD_UPDN_PLACE_DN, OnBtnUldUpdnPlaceDn)
	ON_BN_CLICKED(IDC_BTN_ULD_UPDN_BUF_1_PICK_DN, OnBtnUldUpdnBuf1PickDn)
	ON_BN_CLICKED(IDC_BTN_ULD_UPDN_BUF_2_PICK_DN, OnBtnUldUpdnBuf2PickDn)
	ON_BN_CLICKED(IDC_BTN_ULD_X_PLACE, OnBtnUldXPlace)
	ON_BN_CLICKED(IDC_BTN_ULD_X_BUF_1, OnBtnUldXBuf1)
	ON_BN_CLICKED(IDC_BTN_ULD_X_BUF_2, OnBtnUldXBuf2)
	ON_BN_CLICKED(IDC_BTN_ULD_M_BOAT_PLACE, OnBtnUldMBoatPlace)
	ON_BN_CLICKED(IDC_BTN_ULD_M_BOAT_PICK_1, OnBtnUldMBoatPick1)
	ON_BN_CLICKED(IDC_BTN_ULD_M_BOAT_PICK_2, OnBtnUldMBoatPick2)
	ON_BN_CLICKED(IDC_BTN_BUF1_LD, OnBtnBuf1Ld)
	ON_BN_CLICKED(IDC_BTN_BUF1_FLIPPER, OnBtnBuf1Flipper)
	ON_BN_CLICKED(IDC_BTN_BUF1_ULD, OnBtnBuf1Uld)
	ON_BN_CLICKED(IDC_BTN_BUF1_DEG_0, OnBtnBuf1Deg0)
	ON_BN_CLICKED(IDC_BTN_BUF1_DEG_180, OnBtnBuf1Deg180)
	ON_BN_CLICKED(IDC_BTN_BUF2_LD, OnBtnBuf2Ld)
	ON_BN_CLICKED(IDC_BTN_BUF2_FLIPPER, OnBtnBuf2Flipper)
	ON_BN_CLICKED(IDC_BTN_BUF2_ULD, OnBtnBuf2Uld)
	ON_BN_CLICKED(IDC_BTN_BUF2_DEG_0, OnBtnBuf2Deg0)
	ON_BN_CLICKED(IDC_BTN_BUF2_DEG_180, OnBtnBuf2Deg180)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_1 message handlers

BOOL CDialog_Manual_Part_1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rectArea;
	
	mp_manual_font = NULL;
	mp_manual_font = new CFont;
	mp_manual_font->CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	
	GetClientRect(rectArea);
	MoveWindow(mcp_coord.x, mcp_coord.y, rectArea.Width(), rectArea.Height(), TRUE);

	OnManualPart1_Label_Set();

	SetTimer(TMR_IO_CHECK, 100, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Manual_Part_1::OnDestroy() 
{
	if(mp_manual_font != NULL)
	{
		delete mp_manual_font;
		mp_manual_font = NULL;
	}

	CDialog::OnDestroy();
}

void CDialog_Manual_Part_1::OnManualPart1_Label_Set()
{
	m_label_warpage_check.SetFont(mp_manual_font);
	m_label_warpage_check.SetWindowText(_T("Warpage"));
	m_label_warpage_check.SetCenterText();
	m_label_warpage_check.SetColor(RGB(0, 0, 0));
	m_label_warpage_check.SetGradientColor(RGB(0, 0, 255));
	m_label_warpage_check.SetTextColor(RGB(255, 255, 255));

	m_label_id_check.SetFont(mp_manual_font);
	m_label_id_check.SetWindowText(_T("ID Check"));
	m_label_id_check.SetCenterText();
	m_label_id_check.SetColor(RGB(0, 0, 0));
	m_label_id_check.SetGradientColor(RGB(0, 0, 255));
	m_label_id_check.SetTextColor(RGB(255, 255, 255));

	m_label_boat_check.SetFont(mp_manual_font);
	m_label_boat_check.SetWindowText(_T("Boat Æ²¾îÁü"));
	m_label_boat_check.SetCenterText();
	m_label_boat_check.SetColor(RGB(0, 0, 0));
	m_label_boat_check.SetGradientColor(RGB(0, 0, 255));
	m_label_boat_check.SetTextColor(RGB(255, 255, 255));
}

void CDialog_Manual_Part_1::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case TMR_IO_CHECK:
		OnManualPart1_IO_Check();
		break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDialog_Manual_Part_1::PostNcDestroy() 
{
	delete this;
	Func.mp_manual_1 = NULL;
	
	CDialog::PostNcDestroy();
}

BOOL CDialog_Manual_Part_1::Create() 
{
	return CDialog::Create( CDialog_Manual_Part_1::IDD );
}

void CDialog_Manual_Part_1::OnBtn1Exit() 
{
	DestroyWindow();
}

void CDialog_Manual_Part_1::OnManualPart1_IO_Check()
{
// 	m_radio_warpage_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_chk, CTL_YES));
// 	m_radio_uld_clamp_up.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_up_chk, CTL_YES));
// 	m_radio_uld_clamp_on.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_clamp_chk, CTL_YES));
// 	m_radio_uld_clamp_off.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_unclamp_chk, CTL_YES));
// 	m_radio_uld_clamp_down.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_dn_chk, CTL_YES));
// 	m_radio_uld_boat_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_uld_shifter_boat_chk, CTL_YES));
// 	m_radio_ld_clamp_on.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_clamp_chk, CTL_YES));
// 	m_radio_ld_clamp_off.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_unclamp_chk, CTL_YES));
// 	m_radio_ld_boat_pos_2.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_pos2_chk, CTL_YES));
// 	m_radio_ld_boat_pos_1.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_pos1_chk, CTL_YES));
// 	m_radio_fm_u_smema_tr_signal_chk.SetCheck(g_ioMgr.get_out_bit(st_io.o_front_machine_transfer_sginal, CTL_YES));
// 	m_radio_fm_u_smema_req_signal_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_front_machine_request_signal_chk, CTL_YES));
// 	m_radio_fm_l_smema_tr_signal_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_front_machine_transfer_signal_chk, CTL_YES));
// 	m_radio_fm_l_smema_req_signal_chk.SetCheck(g_ioMgr.get_out_bit(st_io.o_front_machine_transfer_sginal, CTL_YES));
// 	m_radio_buf_2_unclamp_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer2_boat_unclamp_chk, CTL_YES));
// 	m_radio_buf_2_rotator_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer2_rotator_pos_chk, CTL_YES));
// 	m_radio_buf_2_clamp_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer2_boat_clamp_chk, CTL_YES));
// 	m_radio_buf_2_boat_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer2_boat_chk, CTL_YES));
// 	m_radio_buf_1_unclamp_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer1_boat_unclamp_chk, CTL_YES));
// 	m_radio_buf_1_rotator_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer1_rotator_pos_chk, CTL_YES));
// 	m_radio_buf_1_clamp_chk.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer1_boat_clamp_chk, CTL_YES));
// 	m_radio_buf_1_boat_check.SetCheck(g_ioMgr.get_in_bit(st_io.i_shuttle_buffer1_boat_chk, CTL_YES));
// 	m_radio_boat_id_8.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_product_id4_chk, CTL_YES));
// 	m_radio_boat_id_7.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_product_id3_chk, CTL_YES));
// 	m_radio_boat_id_6.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_product_id2_chk, CTL_YES));
// 	m_radio_boat_id_5.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_product_id1_chk, CTL_YES));
// 	m_radio_boat_id_4.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_id4_chk, CTL_YES));
// 	m_radio_boat_id_3.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_id3_chk, CTL_YES));
// 	m_radio_boat_id_2.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_id2_chk, CTL_YES));
// 	m_radio_boat_id_1.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_id1_chk, CTL_YES));
// 	m_radio_boat_chk_2.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_pos2_chk, CTL_YES));
// 	m_radio_boat_chk_1.SetCheck(g_ioMgr.get_in_bit(st_io.i_ld_shifter_boat_pos1_chk, CTL_YES));
}

void CDialog_Manual_Part_1::OnBtnLdBoatClampOn() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader Boat Clamp On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_ld_shifter_boat_clamp, CTL_YES);
// 		g_ioMgr.set_out_bit(st_io.o_ld_shifter_boat_unclamp, CTL_NO);
// 
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader Boat Clamp On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdBoatClampOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader Boat Clamp Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_ld_shifter_boat_clamp, CTL_NO);
// 		g_ioMgr.set_out_bit(st_io.o_ld_shifter_boat_unclamp, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader Boat Clamp Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldBoatClampOn() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Unloader Boat Clamp On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_uld_shifter_boat_clamp, CTL_YES);
// 		g_ioMgr.set_out_bit(st_io.o_uld_shifter_boat_unclamp, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Unloader Boat Clamp On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldBoatClampOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Unloader Boat Clamp Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_uld_shifter_boat_clamp, CTL_NO);
// 		g_ioMgr.set_out_bit(st_io.o_uld_shifter_boat_unclamp, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Unloader Boat Clamp Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1BoatClamp() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Buffer 1 Boat Clamp On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_shuttle_buffer1_boat_clamp, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Buffer 1 Boat Clamp On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1BoatClampOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Buffer 1 Boat Clamp Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_shuttle_buffer1_boat_clamp, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Buffer 1 Boat Clamp Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2BoatClamp() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Buffer 2 Boat Clamp On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_shuttle_buffer2_boat_clamp, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Buffer 2 Boat Clamp On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2BoatClampOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Buffer 2 Boat Clamp Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_shuttle_buffer2_boat_clamp, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Buffer 2 Boat Clamp Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnFmLSmemaReqSignal() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Request Signal On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_request_sginal, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Request Signal On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnFmLSmemaReqSignalOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Request Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_request_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Request Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnFmUSmemaTrSignal() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal On?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_YES);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal On");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnFmUSmemaTrSignalOff() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}


void CDialog_Manual_Part_1::OnBtnLdZPick() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader Z Axis Pick Pos Move?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader Z Axis Pick Pos Move");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdZPlaceBuf1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader Z Axis Buffer 1 Place Pos Move?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader Z Axis Buffer 1 Place Pos Move");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdZPlaceBuf2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader Z Axis Buffer 2 Place Pos Move?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader Z Axis Buffer 2 Place Pos Move");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXPick() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Loader X Axis Pick Pos Move?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Loader X Axis Pick Pos Move");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDialog_Manual_Part_1::OnBtnLdXBuf1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXBuf2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXIdCheck() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp3() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp4() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp5() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp6() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp7() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdXWp8() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdMBoatPick() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdMBoatPlace1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnLdMBoatPlace2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldUpdnPlaceDn() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldUpdnBuf1PickDn() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldUpdnBuf2PickDn() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldXPlace() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldXBuf1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldXBuf2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldMBoatPlace() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldMBoatPick1() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnUldMBoatPick2() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1Ld() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1Flipper() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1Uld() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1Deg0() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf1Deg180() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2Ld() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2Flipper() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2Uld() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2Deg0() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}

void CDialog_Manual_Part_1::OnBtnBuf2Deg180() 
{
// 	st_msg.mstr_confirm_msg = _T("Manual : Front Machine Transfer Signal Off?");
// 	
// 	n_response = select_dlg.DoModal();
// 	
// 	if (n_response == IDOK)
// 	{
// 		g_ioMgr.set_out_bit(st_io.o_front_machine_transfer_sginal, CTL_NO);
// 		
// 		sprintf(st_msg.c_normal_msg, "[Manual] Front Machine Transfer Signal Off");
// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
// 	}
}
