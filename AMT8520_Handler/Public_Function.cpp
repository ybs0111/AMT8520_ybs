// Public_Function.cpp: implementation of the CPublic_Function class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Handler.h"
#include "Variable.h"			// ���� ���� ���� Ŭ����
#include "Public_Function.h"	// ���� ���� �� I/O ���� ���� Ŭ����
#include <nb30.h>
#include <wininet.h>
#include "Alg_Mysql.h"
#include "FastechPublic_IO.h"
#include "FAS_HSSI.h"
#include "ComizoaPublic.h"
#include "Cmmsdkdef.h"
#include "AMTLotManager.h"
#include "CtlBd_Function.h"
#include "CtlBd_Library.h"
#include "SrcPart/APartHandler.h"
#include "SrcPart/APartTestSite.h"

// #include "stdafx.h"
// #include "iostream.h"
// #include "afxmt.h"

#include "SrcBase\ACriticalSection.h"
#include "SrcPart/APartDatabase.h"
#include "SrcPart/APartHandler_COK.h"
#include "ABpcManager.h"
#include "AMTRegistry.h"
#include "Run_Network.h"

#include "AMTRegistry.h"
#include "AVersion.h"
#include "IO_Manager.h"

#include "SrcPart/APartNetworkManager.h"
// ******************************************************************************
// ��ȭ ���� Ŭ���� �߰�                                                         
// ******************************************************************************
#include "Dialog_Infor.h"
#include "Dialog_Select.h"
#include "Dialog_Message.h"

#include "Dialog_KeyPad.h"
// ******************************************************************************
#include "Srcbase\ALocalization.h"

typedef struct _ASTAT_ 
{ 

ADAPTER_STATUS adapt; 
NAME_BUFFER NameBuff [30]; 

}ASTAT, * PASTAT; 

ASTAT Adapter; 

#include "math.h"
#include "io.h"			// ���� ���� ���� �˻� �Լ� ȣ���ϱ� ���ؼ��� �ݵ�� �ʿ�

//CMyBasicData     mcls_p_basic;		// �⺻ ���� ���� �ε� �� ���� Ŭ����
CMutex pmutex;
CMutex plogmutex;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CPublic_Function,CObject,1);	// ���� ������ ����...2K12/02/23/ViboX

CPublic_Function  Func;				// ���� Ŭ���� ������ ����

CPublic_Function::CPublic_Function()
{
	Mot_RetryCount = 3;	
	mp_main_font = NULL;
	mp_main_font = new CFont;
	mp_main_font->CreateFont(15,0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "����ü");

	mp_main_big_font = NULL;
	mp_main_big_font = new CFont;
	mp_main_big_font->CreateFont(21,0,0,0,0,0,0,0,0,0,0,DEFAULT_QUALITY,0,"2002L");

	mp_motor_font = NULL;
	mp_motor_font = new CFont;
	mp_motor_font->CreateFont(15,0,0,0,FW_BOLD,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");

	mp_motorspeed_font = NULL;
	mp_motorspeed_font = new CFont;
	mp_motorspeed_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_motoraxis1_font = NULL;
	mp_motoraxis1_font = new CFont;
	mp_motoraxis1_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_motorpart_font = NULL;
	mp_motorpart_font = new CFont;
	mp_motorpart_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_iopart_font = NULL;
	mp_iopart_font = new CFont;
	mp_iopart_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_partmanual_font = NULL;
	mp_partmanual_font = new CFont;
	mp_partmanual_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_fileEditer_font = NULL;
	mp_fileEditer_font = new CFont;
	mp_fileEditer_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_pgminfo_font = NULL;
	mp_pgminfo_font = new CFont;
	mp_pgminfo_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	
	mp_alarm_font = NULL;
	mp_alarm_font = new CFont;
	mp_alarm_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_listdataview_font = NULL;
	mp_listdataview_font = new CFont;
	mp_listdataview_font->CreateFont(24,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_buffer_font = NULL;
	mp_buffer_font = new CFont;
	mp_buffer_font->CreateFont(15,0,0,0,0,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");

	mp_exit_font = NULL;
	mp_exit_font = new CFont;
	mp_exit_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	
	mp_info_font = NULL;
	mp_info_font = new CFont;
	mp_info_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_board_font = NULL;
	mp_board_font = new CFont;
	mp_board_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_pad_font = NULL;
	mp_pad_font = new CFont;
	mp_pad_font->CreateFont(12,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_list_font = NULL;
	mp_list_font = new CFont;
	mp_list_font->CreateFont(14,6,0,0,900,0,0,0,0,0,0,PROOF_QUALITY,0,"MS Sans Serif");

	mp_msg_font = NULL;
	mp_msg_font = new CFont;
	mp_msg_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_change_font = NULL;
	mp_change_font = new CFont;
	mp_change_font->CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_pass_font = NULL;
	mp_pass_font = new CFont;
	mp_pass_font->CreateFont(17,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_select_font = NULL;
	mp_select_font = new CFont;
	mp_select_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_title_font = NULL;
	mp_title_font = new CFont;
	mp_title_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_menu_font = NULL;
	mp_menu_font = new CFont;
	mp_menu_font->CreateFont(18,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_m_msg_font = NULL;
	mp_m_msg_font = new CFont;
	mp_m_msg_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_io_font = NULL;
	mp_io_font = new CFont;
	mp_io_font->CreateFont(14,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_init_font = NULL;
	mp_init_font = new CFont;
	//	mp_init_font->CreateFont(35,20,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"MS Sans Serif");
	mp_init_font->CreateFont(35,20,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial Black Italic");

	mp_lock_font = NULL;
	mp_lock_font = new CFont;
	mp_lock_font->CreateFont(42,0,0,0,0,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial Black Italic");

	mp_wait_time_font = NULL;
	mp_wait_time_font = new CFont;
	mp_wait_time_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_maintenance_font = NULL;
	mp_maintenance_font = new CFont;
	mp_maintenance_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"MS Sans Serif");
	
	mp_combo_font = NULL;
	mp_combo_font = new CFont;
	mp_combo_font->CreateFont(23, 0, 0, 0, 300, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");

	mp_basic_font = NULL;
	mp_basic_font = new CFont;
	mp_basic_font->CreateFont(15,8,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");

	mp_operation_list_font = NULL;
	mp_operation_list_font = new CFont;
	mp_operation_list_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");

	mp_admin_font = NULL;
	mp_admin_font = new CFont;
	mp_admin_font->CreateFont(15,8,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");
}

CPublic_Function::~CPublic_Function()
{
	delete mp_main_font;
	mp_main_font = NULL;

	delete mp_main_big_font;
	mp_main_big_font = NULL;

	delete mp_motor_font;
	mp_motor_font = NULL;

	delete mp_motorspeed_font;
	mp_motorspeed_font = NULL;

	delete mp_motoraxis1_font;
	mp_motoraxis1_font = NULL;

	delete mp_motorpart_font;
	mp_motorpart_font = NULL;

	delete mp_iopart_font;
	mp_iopart_font = NULL;

	delete mp_partmanual_font;
	mp_partmanual_font = NULL;

	delete mp_fileEditer_font;
	mp_fileEditer_font = NULL;
	
	delete mp_pgminfo_font;
	mp_pgminfo_font = NULL;
	
	delete mp_alarm_font;
	mp_alarm_font = NULL;

	delete mp_listdataview_font;
	mp_listdataview_font = NULL;
	
	delete mp_buffer_font;
	mp_buffer_font = NULL;

	delete mp_exit_font;
	mp_exit_font = NULL;
	
	delete mp_info_font;
	mp_info_font = NULL;
	
	delete mp_board_font;
	mp_board_font = NULL;
	
	delete mp_pad_font;
	mp_pad_font = NULL;

	delete mp_list_font;
	mp_list_font = NULL;

	delete mp_msg_font;
	mp_msg_font = NULL;

	delete mp_change_font;
	mp_change_font = NULL;

	delete mp_pass_font;
	mp_pass_font = NULL;

	delete mp_select_font;
	mp_select_font = NULL;

	delete mp_title_font;
	mp_title_font = NULL;

	delete mp_menu_font;
	mp_menu_font = NULL;

	delete mp_m_msg_font;
	mp_m_msg_font = NULL;

	delete mp_io_font;
	mp_io_font = NULL;

	delete mp_init_font;
	mp_init_font = NULL;

	delete mp_lock_font;
	mp_lock_font = NULL;

	delete mp_wait_time_font;
	mp_wait_time_font = NULL;

	delete mp_maintenance_font;
	mp_maintenance_font = NULL;
	
	delete mp_combo_font;
	mp_combo_font = NULL;

	delete mp_basic_font;
	mp_basic_font = NULL;

	delete mp_operation_list_font;
	mp_operation_list_font = NULL;

	delete mp_admin_font;
	mp_admin_font = NULL;	
}

void CPublic_Function::On_LogFile_Add(int n_msg_select, CString str_msg)
{
	CSingleLock sing(&pmutex);

	sing.Lock();

	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // ���� ��, ��, �� ���� ���������� ��ȯ�Ͽ� ������ ���� 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour, mn_cur_min, mn_cur_sec; // ���� ��, ��, �� ���� ���� ���� 
	CString mstr_file_name;		// ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;	// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;		// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	COleDateTime time_cur;		// �˻��� �ð� ���� ���� ���� 
	CTime m_time_current;		// ������ ����� ���� �ð� ���� ���� ����
	int mn_existence;			// ���� ���� ���� ���� �÷��� 
	char fileName[256];			// �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;					// ���Ͽ� ���� ������ ���� ���� 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	CString mstr_folder, mstr_file;
	int Ret;

	CString sTime;
	
	SYSTEMTIME csTime;
	
	GetLocalTime(&csTime);
	
	csTime.wYear;
	csTime.wMonth;
	csTime.wDay;
	csTime.wHour;
	csTime.wMinute;
	csTime.wSecond;
	csTime.wMilliseconds;
	
	sTime.Format("[%04d/%02d/%02d %02d:%02d:%02d.%03d] ", csTime.wYear, csTime.wMonth, csTime.wDay, csTime.wHour, csTime.wMinute, csTime.wSecond, csTime.wMilliseconds);

	if (str_msg.IsEmpty() == TRUE)  
	{
		sing.Unlock();
		return;
	}

	// **************************************************************************
	// ���� �̸����� ����� ��¥ ������ ��´�                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // ���� �ð� ������ ��´�. 

	m_time_current = CTime::GetCurrentTime();  // ������ ������ ���� �ð� ���� ��´�. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();

	mn_cur_min = time_cur.GetMinute();
	mn_cur_sec = time_cur.GetSecond();
	// **************************************************************************

	// **************************************************************************
	// ��¥ ������ ���������� ��ȯ�Ͽ� ������ �����Ѵ�                           
	// **************************************************************************
	mstr_cur_year.Format("%04d", mn_cur_year);
	mstr_cur_month.Format("%02d", mn_cur_month);
	mstr_cur_day.Format("%02d", mn_cur_day);

	// **************************************************************************
	// ���� �ð� ���� ��´�                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();	// ���� �ð� ������ �����Ѵ�. 
	mstr_cur_hour.Format("%d",time_cur.GetHour());	// ���� �ð� ������ �����Ѵ�.
	mstr_cur_min.Format("%d",time_cur.GetMinute());	// ���� �� ������ �����Ѵ�. 
	mstr_cur_sec.Format("%d",time_cur.GetSecond());	// ���� �� ������ �����Ѵ�. 
	str_display_time = m_time_current.Format("%c");  // ����Ʈ ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ���� 
	// **************************************************************************

	mstr_file.Format("%d.txt",time_cur.GetHour());

	switch (n_msg_select)			//Operation Setting
	{		
	case LOG_OPERATION:
		mstr_file_name = "OP" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_operation + mstr_file_name;
		break;
	case LOG_TESTINTERFACE:							//GPIB
		mstr_file_name = "TI" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_interface + mstr_file_name;
		break;
	case LOG_TCPIP:								//��� ���� ����.
		mstr_file_name = "TCP" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_tcpip + mstr_file_name;
		break;
	case LOG_MACHINE:								//��� ���� ����.
		mstr_file_name = "MA" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_DEBUG:									// ��� ����� ����.
		mstr_file_name = "DEBUG" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_BARCODE:								// ���ڵ� ����.
		mstr_file_name = "BARCODE" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_barcode + mstr_file_name;
		break;
	case LOG_TIME:									// TIME����
		mstr_file_name = "TIME" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_AUTOSOCKETOFF:							//��� ���� ����.
		mstr_file_name = "AUTOSOCKETOFF" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_WORK:									//��� ���� ����.
		mstr_file_name = "WORK" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_ADJUSTMENT:							//��� ���� ����.
		mstr_file_name = "ADJUSTMENT" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_HISTORY:								//��� ���� ����.
		mstr_file_name = "HISTORY" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_machine + mstr_file_name;
		break;
	case LOG_TOTAL:									//��ü �޼���.
		mstr_file_name = "TOT" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = st_path.mstr_total + mstr_file_name;

		break;

	default:
		return;
	}

	mstr_create_file += ".TXT";

	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName, 0);
	
	if (mn_existence == -1)
	{
		switch (n_msg_select)			//Operation Setting
		{		
		case LOG_OPERATION:
			mstr_file_name = "OP" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_operation + mstr_file_name;
			break;
		case LOG_TESTINTERFACE:							//GPIB
			mstr_file_name = "TI" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_interface + mstr_file_name;
			break;
		case LOG_TCPIP:								//��� ���� ����.
			mstr_file_name = "TCP" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_tcpip + mstr_file_name;
			break;
		case LOG_MACHINE:								//��� ���� ����.
			mstr_file_name = "MA" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_DEBUG:									// ��� ����� ����.
			mstr_file_name = "DEBUG" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_BARCODE:								// ���ڵ� ����.
			mstr_file_name = "BARCODE" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_barcode + mstr_file_name;
			break;
		case LOG_TIME:									// TIME����
			mstr_file_name = "TIME" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_AUTOSOCKETOFF:							//��� ���� ����.
			mstr_file_name = "AUTOSOCKETOFF" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_WORK:									//��� ���� ����.
			mstr_file_name = "WORK" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_ADJUSTMENT:							//��� ���� ����.
			mstr_file_name = "ADJUSTMENT" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_HISTORY:								//��� ���� ����.
			mstr_file_name = "HISTORY" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_machine + mstr_file_name;
			break;
		case LOG_TOTAL:									//��ü �޼���.
			mstr_file_name = "TOT" + mstr_cur_year;
			mstr_file_name += mstr_cur_month; 
			mstr_file_name += mstr_cur_day; 
			mstr_create_file = st_path.mstr_total + mstr_file_name;
			
			break;
			
		default:
			return;
		}
		mstr_create_file += ".TXT";
		// **********************************************************************
	}

	Ret = FileSizeCheck(mstr_create_file, 1000000, CTL_YES); //size and rename
	
	if (Ret == 1) //file size over
	{
		CString strTmp = mstr_create_file.Left( mstr_create_file.GetLength() - 4 );
		BackupName.Format("%s.%02d%02d%02d.bak",strTmp, mn_cur_hour, mn_cur_min, mn_cur_sec );
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + ".bak" ;
		rename(mstr_create_file, BackupName);
	}

	// **************************************************************************
	// �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����              
	// **************************************************************************
	if ((fp = fopen(mstr_create_file,"a+")) == NULL)  
	{
		CString strLog;
		strLog.Format("File Open Failue .. [%s]", mstr_create_file);
		Func.MsgLog(strLog);
		sing.Unlock();
		return;
	}
	// **************************************************************************

	// **************************************************************************
	// �α� ���Ͽ� ���� �߻��� �˶� ���� �����Ѵ�                                
	// **************************************************************************
//	mstr_content += str_display_time;
// 	mstr_content += sTime;
// 
// 	mstr_content += " : " + str_msg;

	mstr_content.Format("%s : %s",sTime,str_msg);

	fprintf(fp,"%s\n",mstr_content);

	if (ferror(fp))  
	{
		if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
//			st_msg.mstr_abnormal_msg = _T("File save failure.");
			sprintf(st_msg.c_abnormal_msg, "File save failure.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		fclose(fp);
		sing.Unlock();
		return ;
	}
	// **************************************************************************
	fclose(fp);  // ������ �����Ѵ�.

	sing.Unlock();
}

BOOL CPublic_Function::CreateFolder(LPCTSTR szPath)
{
	int nRet = 0;
	
	CString strPath = szPath;
	
    do{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            CString strTemp = strPath.Left(nRet+1);
			
            if (_access(strTemp.operator LPCTSTR(), 00) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return nRet;
}

CString CPublic_Function::Get_MotorName(int n_motor_no)
{
	CString strRet;

	switch (n_motor_no)
	{
	case MOTOR_BIN_1_Z:			strRet = "BIN 1 Z";				break;
	case MOTOR_BIN_1_R:			strRet = "BIN 1 R";				break;
	case MOTOR_BIN_2_Z:			strRet = "BIN 2 Z";				break;
	case MOTOR_BIN_2_R:			strRet = "BIN 2 R";				break;
	case MOTOR_REJECT_Z:		strRet = "BIN 3 Z";				break;
	case MOTOR_REJECT_R:		strRet = "BIN 3 R";				break;
	case MOTOR_TRAY_TR_Y:		strRet = "TRANSFER Y";			break;
	case MOTOR_TRAY_TR_CLAMP:	strRet = "TRANSFER CLAMP";		break;
	case MOTOR_TRAY_STACKER:	strRet = "STACKER";				break;
	case MOTOR_ROBOT_Z:			strRet = "ROBOT Z";				break;
	case MOTOR_ROBOT_X:			strRet = "ROBOT X";				break;
	case MOTOR_ROBOT_Y:			strRet = "ROBOT Y";				break;
	case MOTOR_TESTSITE_1_Z:	strRet = "TEST SITE 1 Z";		break;
	case MOTOR_TESTSITE_2_Z:	strRet = "TEST SITE 2 Z";		break;
	case MOTOR_COK_ROBOT_Y:		strRet = "COK ROBOT Y";			break;
	case MOTOR_COK_ROBOT_Z:		strRet = "COK ROBOT Z";			break;
	
	default:
		strRet = "Unknown_Motor";
		break;
	}
	
	return strRet;
}

int CPublic_Function::OnElect_CheckData(CString str_data)
{
	int nFuncRet = CTLBD_RET_GOOD;
	
	char	cp_cmd[1024];
	int     nLength;  // ������ �� ���� ���� ���� 
	CString strBody;  // BODY ������ ���� ���� 
	CString strTemp;
	
	int nChannel;
	double dValue;
	
	memset(&cp_cmd, 0, sizeof(cp_cmd));  // ������ ���� ���� �ʱ�ȭ 
	
	// **************************************************************************
	// ���� ������ �ּ� ���� �˻��Ѵ� [24�ڸ��� �Ǿ� �Ѵ�.]
	// **************************************************************************
	nLength = str_data.GetLength();
	if (nLength != 25)			
		return CTLBD_RET_ERROR;
	// **************************************************************************

	//2016.0426
	CString strRecvLog;
	strRecvLog.Format("%s",str_data);
	sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
	Func.On_LogFile_Add(99, st_msg.c_msg);

	strBody = str_data.Mid(2, nLength - 3);  // [���+����] ���� 
	strBody.TrimLeft(' ');
	strBody.TrimRight(' ');
	
	strTemp = strBody.Mid(11, 1);			// ä��.
	nChannel = atoi(strTemp);

	//2015.0904
	if(nChannel < 1 || nChannel > 5)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "%dPort is  error!!!", nChannel);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
		}
		return nFuncRet;
	}
	
	
	strTemp = strBody.Mid(12, 5);
	dValue = atof(strTemp);
	
	st_work.mdReadElectData[nChannel-1] = dValue;	// ���� �ִ´�.
	
	if (strBody.Mid(12, 2) == "OP")
	{
//		st_msg.mstr_normal_msg.Format("%dPort : OPEN", nChannel);
		sprintf(st_msg.c_normal_msg, "%dPort : OPEN", nChannel);
		st_grounddb.mstrChannel[nChannel-1].Format("OPEN");
		st_work.mdReadElectData[nChannel-1] = -99999;	// ���� �ִ´�.
	}
	else if (strBody.Mid(12, 2) == "OV")
	{
//		st_msg.mstr_normal_msg.Format("%dPort : OVER", nChannel);
		sprintf(st_msg.c_normal_msg, "%dPort : OVER", nChannel);
		st_grounddb.mstrChannel[nChannel-1].Format("OVER");
		st_work.mdReadElectData[nChannel-1] = -99998;	// ���� �ִ´�.
	}
	else
	{
//		st_msg.mstr_normal_msg.Format("%dPort : %0.2f", nChannel, dValue);
		sprintf(st_msg.c_normal_msg, "%dPort : %0.2f", nChannel, dValue);
		st_grounddb.mstrChannel[nChannel-1].Format("%0.2f", dValue);
	}
	
	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
	}
	
	if (st_handler.cwnd_basic != NULL)											// BASIC ȭ�� ����
	{
		st_handler.cwnd_basic->SendMessage(WM_BASIC_APPLY, 1, nChannel);		// DATA APPLY ��û
	}
	
	return nFuncRet;
}

void CPublic_Function::OnMot_Speed_Setting()
{
	int i;

	for (i = 0; i < MOTOR_COUNT; i++)
	{
		if (COMI.md_spd_vel[i][1] < 1)
		{
			COMI.md_spd_vel[i][1] = 10000;

			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
			{
//				st_msg.mstr_abnormal_msg.Format("%s Motor Acc is under 1 -> 10000 Set up", Get_MotorName(i));
				sprintf(st_msg.c_abnormal_msg, "%s Motor Acc is under 1 -> 10000 Set up", Get_MotorName(i));
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
		}
		
		if (COMI.md_spd_vel[i][2] < 1)
		{
			COMI.md_spd_vel[i][2] = 10000;

			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
			{
				sprintf(st_msg.c_abnormal_msg, "%s Motor Dec is under 1 -> 10000 Set up", Get_MotorName(i));
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
		}

		COMI.Set_MotSpeed(MOT_SPD_VEL, i, cmSMODE_T, COMI.md_spd_vel[i][0], COMI.md_spd_vel[i][1], COMI.md_spd_vel[i][2]);
		COMI.Set_MotSpeed(MOT_SPD_ORG, i, cmSMODE_T, COMI.md_spd_home[i], COMI.md_spd_vel[i][1]/10, COMI.md_spd_vel[i][2]/10); //st_motor[i].d_spd_home[0] * 10, st_motor[i].d_spd_home[0] * 10);

		COMI.Set_HomeSetConfig(i, COMI.mn_homecheck_method[i], 0, 2, 1);
		Sleep(100);
	}
}

int CPublic_Function::DoorOpenCheckSpot()
{
	int Ret = CTLBD_RET_GOOD;
	
	// ����� ���ȴ��� �޼���â �����ش�.
// 	370201 0 A "���� 1�� ����."
// 	370202 0 A "���� 2�� ����."
// 	370203 0 A "���� 3�� ����."
// 	370204 0 A "���� 4�� ����."
// 	370205 0 A "���� 5�� ����."
// 	370206 0 A "���� 6�� ����."	
	for (int i = 0; i <6 ; i++)
	{
		if ( g_ioMgr.get_in_bit( st_io.i_door_chk[i], IO_ON) == IO_OFF )
		{
			CString strJamcode;
			strJamcode.Format( "%d", 370201 + i );
			CTL_Lib.Alarm_Error_Occurrence( 9233, dWARNING, strJamcode );
			Ret = CTLBD_RET_ERROR;
			break;
		}
	}
	return Ret;
}

int CPublic_Function::OnMotor_Go_Check(int n_motor, double d_pos)
{
	CString str_msg, str_motor;
	
	int n_response, nRet = CTL_NO;
	
	CDialog_Select	select_dlg;
	
	str_motor = Get_MotorName(n_motor);
	
	str_msg.Format("%s Motor, Move value is %3.3f, Go?",str_motor, (float)d_pos);
	
	st_msg.mstr_confirm_msg = _T(str_msg);
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		nRet = CTL_YES;
	}
	else
	{
		nRet = CTL_NO;
	}
	
	return nRet;
}

int CPublic_Function::On_Encoder_Check(double d_pos)
{
	CString str_msg;
	
	int n_response, nRet = CTL_NO;
	
	CDialog_Select	select_dlg;
	
	str_msg.Format("Now Reading Value is %.03f, Setting?", (double)d_pos);
	st_msg.mstr_confirm_msg = _T(str_msg);
	n_response = select_dlg.DoModal();
	if (n_response == IDOK) 
	{
		nRet = CTL_YES;
	}
	else
	{
		nRet = CTL_NO;
	}
	
	return nRet;
}

void CPublic_Function::OnSet_IO_Port_Stop()
{
	if ( st_timedb.mnTime_Section != dSTOP )
	{
		st_timedb.mnTime_Section	= st_timedb.mnSectionBuffer;
		st_timedb.mnSectionBuffer = dSTOP;
		
		st_timedb.mole_date		= COleDateTime::GetCurrentTime();
		
		//g_db.DB_Write_Time(st_timedb);
	}
	COMI.mn_run_status = dSTOP;
	
	st_handler.mn_status =  CTL_NO;					// ��� �������̳�.. 2K9/12/29/ViboX
	
//	g_ioMgr.set_out_bit(st_io.o_front_machine_request_sginal, CTL_NO);	// ��Ʋ���� �о� ������� �ϴ� OFF��Ų��. 2K10/08/10/ViboX

	g_ioMgr.set_out_bit(st_io.o_start_switch_lamp, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_stop_switch_lamp, IO_ON);


	// ����� ������Ų��.
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor1, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor2, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor3, IO_OFF );
		
	st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);

	if( st_basic.mn_mode_xgem == CTL_YES )
	{
//jtkim 20150216		g_client_xgem.SetGemState(0);//Offline
		g_client_xgem.SetMachineStatus(MC_STOP);
	}
	////2015.0321
	sprintf(st_msg.c_msg, "[Stop Button] Handler Stop");
	On_LogFile_Add(99, st_msg.c_msg);
	////

}

void CPublic_Function::OnSet_IO_Port_Run()
{
	//2013,0321
	bool bAllFail = true;
	CString strErr;
	for( int i=0; i<TSITE_INSERT_CNT; i++ )
	{
// 		if( g_site.GetEnableSocket( i * 2 ) == false &&
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
		if( g_site.GetEnableSocket( i * 2 ) <= 0 &&
			g_site.GetEnableSocket( i * 2 + 1 ) <= 0 )
		{
		}
		else
		{
			bAllFail = false;
		}
	}

	if(bAllFail == true)
	{
		strErr.Format( "��� Tester socket�� ����� �� �����ϴ�. �׽��͸� üũ�� �ּ���");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		COMI.mn_run_status = dSTOP;
		return;
	}
	
	COMI.mn_run_status = dRUN;
	if ( st_timedb.mnTime_Section != st_timedb.mnSectionBuffer )
	{
		st_timedb.mnTime_Section	= st_timedb.mnSectionBuffer;
		
		st_timedb.mnSectionBuffer = dRUN;
		
		st_timedb.mole_date		= COleDateTime::GetCurrentTime();
		
		//g_db.DB_Write_Time(st_timedb);
	}
	
	if( st_basic.mn_mode_xgem == CTL_YES)
	{
		if(st_basic.mn_mode_rcmd == CTL_YES)
		{
			if( st_handler.m_nRemote == CTL_YES )
			{
// jtkim 20150216 				g_client_xgem.SetGemState(2);//Online
				g_client_xgem.SetMachineStatus(MC_RUN);
			}
			else
			{
				CString strErr = "Remote Stop �����Դϴ�. Recipe�� ���޵��� �ʾҰų� �������� �۾��� �㰡���� �ʽ��ϴ�.";
				st_msg.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

				return;
			}
		}
		else
		{
// jtkim 20150216			g_client_xgem.SetGemState(2);//Online
			g_client_xgem.SetMachineStatus(MC_RUN);
		}
		
	}
	
	//���� �ʱ�ȭ.
	st_handler.mn_level_teach = FALSE;
	st_handler.mn_level_maint = FALSE;
	st_handler.mn_safety = CTL_YES;						// ���� ��ġ �۵� 2K6/06/09/ViboX
	
	st_handler.mn_status =  CTL_YES;					// ��� �������̳�.. 2K9/12/29/ViboX
	
	g_ioMgr.set_out_bit(st_io.o_start_switch_lamp,	IO_ON);
	g_ioMgr.set_out_bit(st_io.o_stop_switch_lamp,	IO_OFF);
	
	// ��� ��ٴ�.
	g_ioMgr.set_out_bit(st_io.o_door_lock,				IO_ON);

	// COK �˻�.
	g_handler_cok.CheckCokTypeChange();

	st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, LEVEL_MODE, 0);
	st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);

	alarm.stl_alarm_start_time = GetCurrentTime();
	////2015.0321
	sprintf(st_msg.c_msg, "[Start Button] Handler Start");
	On_LogFile_Add(99, st_msg.c_msg);
	////

	st_handler.mn_alarm_screen = FALSE;
}

void CPublic_Function::OnSet_IO_Port_Init()
{
	COMI.mn_run_status = dINIT;

	// ��� ��ٴ�.
	g_ioMgr.set_out_bit(st_io.o_door_lock,				IO_ON);
}

int CPublic_Function::Handler_Recovery_Data(int n_Mode)
{
	//////////////////////////////////////////////////////////////////////////
	// �� Run Class�� ���� �б� �� ����
	//////////////////////////////////////////////////////////////////////////
	CFile file;
	int nFileMode,nArchiveMode;
	CString sFileName;
	int nFuncRet = CTL_GOOD;
	
	sFileName = "c:\\AMT8520\\Setting\\RecoveryRunData.dat";
	
	TRY
	{
		if(n_Mode == DATA_WRITE) //file write
		{
			nFileMode = CFile::modeWrite | CFile::modeCreate;
			nArchiveMode = CArchive::store;
		}
		else
		{
			nFileMode = CFile::modeRead;
			nArchiveMode = CArchive::load;
		}
		
		file.Open(LPCTSTR(sFileName),nFileMode);
		CArchive ar(&file,nArchiveMode);
		
		GlobalDataSerialize(ar);
		Run_Bin_Stacker[0].Serialize(ar);
		Run_Bin_Stacker[1].Serialize(ar);
		Run_Bin_Stacker[2].Serialize(ar);
		Run_COK_Robot.Serialize(ar);
		Run_Reject_Stacker.Serialize(ar);
		Run_Reject_Tray_Transfer.Serialize(ar);
		Run_Robot.Serialize(ar);
		Run_Test_Site[0].Serialize(ar);
		Run_Test_Site[1].Serialize(ar);
	}
	CATCH(CFileException, e) 
	{
		e->ReportError();
		sprintf(st_msg.c_abnormal_msg, "Fail in accessing [%s] FILE.", sFileName);
		nFuncRet = CTL_ERROR;
	}
	END_CATCH
		
		if(nFuncRet == CTL_ERROR && st_handler.cwnd_list != NULL) 
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG); 
		
	return nFuncRet;
}

void CPublic_Function::OnSet_IO_Port_Alarm()
{
	if ( st_timedb.mnTime_Section != CTL_dWARNING )
	{
		st_timedb.mnTime_Section	= st_timedb.mnSectionBuffer;
		st_timedb.mnSectionBuffer = CTL_dWARNING;
		
		st_timedb.mole_date			= COleDateTime::GetCurrentTime();		
		//g_db.DB_Write_Time(st_timedb);
	}
	OnSet_IO_Port_Sound(IO_ON);							// Buzzer�� ON ��Ų��.
	
	if (COMI.mn_run_status != dLOTEND)		
	{
		g_ioMgr.set_out_bit(st_io.o_alarm_clear_lamp, IO_ON);	// Reset Switch�� Lamp�� On ��Ų��.
	}
	
	st_handler.mn_reinstatement = CTL_NO;					// ���� �����߿� �˶��� �߻� ���� ��쿡 ���� �������� ���� ��Ų��.
	
	// ��� ����.
//	g_ioMgr.set_out_bit(st_io.o_door_lock_cyl,	IO_OFF);
}

void CPublic_Function::OnSet_IO_Port_Sound(int n_status, int n_opt)
{
	switch (n_status)
	{
	case 0:
		g_ioMgr.set_out_bit(st_io.o_buzzer_1,	IO_OFF);	// Buzzer ��� ������� ����.
		g_ioMgr.set_out_bit(st_io.o_buzzer_2,	IO_OFF);	// Buzzer ��� ������� ����.
		g_ioMgr.set_out_bit(st_io.o_buzzer_off_lamp, IO_OFF);
		break;
		
	case 1:
		if (st_lamp.mn_buzzer_mode == CTL_YES)			// Buzzer ��� ��忡���� �Ҵ�.
		{
			if( n_opt == 1)	g_ioMgr.set_out_bit(st_io.o_buzzer_1,	IO_ON);
			else			g_ioMgr.set_out_bit(st_io.o_buzzer_2,	IO_ON);
			g_ioMgr.set_out_bit(st_io.o_buzzer_off_lamp, IO_ON);
		}
		break;
	}
}

CString CPublic_Function::OnNetworkBodyAnalysis(CString strBody, CString strFind, int nOpt)
{
	CString strFuncRet = _T("");//2016.0302
	int n_title, n_equal, n_end, n_length, n_qw = 0;    
	
	n_title =	strBody.Find(strFind, 0);							// TITLE�� ��ġ�� ã�´�.
	n_equal =	strBody.Find("=", n_title + 1);						// �ش� item�� �� ��ġ�� ã�´�.
	
	if (n_title == -1 || n_equal == -1)		return strFuncRet;
	
	if (nOpt == 0)
	{
		n_end =		strBody.Find(" ", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_length =	n_end - n_equal - 1;							// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_equal + 1, n_length);
	}
	else if (nOpt == 7)
	{
		strFuncRet = strBody.Mid(n_title + 5);
	}
	else if (nOpt == 2)
	{
		n_end =		strBody.Find("=", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.						// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_end + 1);
	}
	else if (nOpt == 3)
	{
		n_end =		strBody.Find("=", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.						// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_end + 1, 1);
	}
	else if (nOpt == 4)
	{
		n_qw =		strBody.Find("((", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_end =		strBody.Find("))", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_length =	n_end - n_qw - 1;								// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_qw + 2, n_length - 1);
	}
	else if (nOpt == 5)
	{
		n_qw =		strBody.Find("FAIL_REGNUMBER", n_title + 22);					// �ش� item�� �� ��ġ�� ã�´�.
		n_equal =	strBody.Find("=", n_qw + 1);	
		n_end =		strBody.Find(" ", n_qw + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_length =	n_end - n_equal - 1;							// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_equal + 1, n_length);
	}
	else if (nOpt == 6)
	{
		n_qw =		strBody.Find("ID", n_title + 6);					// �ش� item�� �� ��ġ�� ã�´�.
		n_equal =	strBody.Find("=", n_qw + 1);	
		n_end =		strBody.Find(" ", n_qw + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_length =	n_end - n_equal - 1;							// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_equal + 1, n_length);
	}
	else if ( nOpt == 8)
	{
		n_qw =		strBody.Find("\"", n_equal + 1 );
		n_end =		strBody.Find("\"", n_equal + 2 );
		n_length =	n_end - n_qw - 1;
		strFuncRet = strBody.Mid(n_qw + 1, n_length);
	}
	else
	{
		n_qw =		strBody.Find("(", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_end =		strBody.Find(")", n_title + 1);					// �ش� item�� �� ��ġ�� ã�´�.
		n_length =	n_end - n_qw - 1;								// DATA�� ���� ���.
		strFuncRet = strBody.Mid(n_qw + 1, n_length);
	}
	
	strFuncRet.MakeUpper();
	strFuncRet.TrimLeft(' ');
	strFuncRet.TrimRight(' ');

	return strFuncRet;
}

CString CPublic_Function::OnNetworkDataMake(int nID)
{
	CString strHeader;
	CString strBody;
	int nBody = 0, i;

	CString strTemp;

	strBody = "";
	switch (nID)
	{
// ���⼭���ʹ� ��񿡼� �亯�� �ִ� �κ��̴�.

	case NW_DELETE_SCRAP:
		break;

	case NW_SORTER_LOT_END:
		break;

	case NW_LOT_START:
		break;
	case NW_LOT_CANCEL:
		break;
	
	case NW_NEW_LOT_IN_SEND:			// 0000000000000050 FUNCTION=NEW_LOT_IN LOT_NO=TEST_LOT EQP_ID=IR8130
		strBody		= "FUNCTION=NEW_LOT_IN";
		strBody		+= " LOT_NO=" + st_NW.mstr_new_lot_no[1];
		strBody		+= " EQP_ID=" + st_lamp.mstr_equip_id;
		break;

	case NW_LOT_NAME:					// 0000000000000050 FUNCTION=LOT_NAME LOT_NO=TEST_LOT
		strBody		= "FUNCTION=LOT_NAME";
		strBody		+= " LOT_NO=" + st_NW.mstr_m_lot_no[1];
		break;

	case NW_LOT_START_ENABLE:
		st_NW.mstr_function[0]		= "LOT_START_ENABLE";
		st_NW.mstr_lot_id[0]		= st_NW.mstr_lot_id[1];
		st_NW.mstr_eqp_id[0]		= st_NW.mstr_eqp_id[1];
		st_NW.mstr_step_cnt[0]		= st_NW.mstr_step_cnt[1];
		st_NW.mstr_result[0]		= st_NW.mstr_result[1];
		st_NW.mstr_fail_log[0]		= st_NW.mstr_fail_log[1];

		strBody		= "FUNCTION_RPY=" + st_NW.mstr_function[0] + " ";
		strBody		+= "EQP_ID=" + st_NW.mstr_eqp_id[0] + " ";
		strBody		+= "STEP_COUNT" + st_NW.mstr_step_cnt[0] + " ";
		strBody		+= "RESULT=PASS ";
		strBody		+= "FAIL_LOG" + st_NW.mstr_fail_log[0];
		break;
// ��������� ��񿡼� �亯�� �ִ� �κ��̴�.


// ���⼭���ʹ� ��� ������ �ϰų� �뺸�� �ϴ� �κ��̴�.
	case NW_TRACK_IN:		
		break;

	case NW_BPC_BUFFER_READY:
		strBody		= "FUNCTION=BUFFER_READY ";
		strBody	   += st_NW.mstr_bpc_buffer_ready;
		break;

	case NW_BPC_BUFFER_IN:
		strBody		= "FUNCTION=BUFFER_IN ";
		strBody	   += st_NW.mstr_bpc_buffer_in;
		break;

	case NW_TRACK_OUT:
		break;

	case NW_LOT_CANCEL_SEND:
		break;
	// ���� �߰���. �Ŀ��� ���� ���Ĵ� ������ �̰ɷ� �Ѵ�. 2K11/03/02/ViboX

	case NW_UNLOAD_COMPT:
		break;
	case NW_CANCEL_END:
		break;

	case NW_LOT_END:
		st_NW.mstr_function[0]		= "LOT_END";
		st_NW.mstr_lot_id[0]		= st_NW.mstr_lot_id[1];
		st_NW.mstr_step_cnt[0]		= st_NW.mstr_step_cnt[1];
		st_NW.mstr_eqp_id[0]		= st_lamp.mstr_equip_id;
		st_NW.mstr_fail_cnt[0]		= st_NW.mstr_fail_cnt[1];
		st_NW.mstr_novideo_cnt[0]	= st_NW.mstr_novideo_cnt[1];


		strBody		= "FUNCTION=" + st_NW.mstr_function[0] + " ";
		strBody		+= "STEP_CNT=" + st_NW.mstr_step_cnt[0] + " ";
		strBody		+= "EQP_ID=" + st_NW.mstr_eqp_id[0] + " ";
		strBody		+= "FAIL_CNT=" + st_NW.mstr_fail_cnt[0] + " ";
		strBody		+= "NOVIDEO_CNT=" + st_NW.mstr_novideo_cnt[0];
		break;

	case NW_FAIL_INFO_SEND:
		break;
	case NW_FAIL_REGNUMBER_REQUEST:
		st_NW.mstr_function[0]		= "FAIL_REGNUMBER_REQUEST";
		st_NW.mstr_pcbox_id[0]		= st_NW.mstr_pcbox_id[1];
		st_NW.mstr_lot_id[0]		= st_NW.mstr_lot_id[1];
		st_NW.mstr_step_cnt[0]		= st_NW.mstr_step_cnt[1];
		st_NW.mstr_slot_no[0]		= st_NW.mstr_slot_no[1];
		st_NW.mstr_test_idx[0]		= st_NW.mstr_test_idx[1];

		strBody		= "FUNCTION=" + st_NW.mstr_function[0] + " ";
		strBody		+= "PCBOX_ID=" + st_NW.mstr_pcbox_id[0] + " ";
		strBody		+= "LOT_ID=" + st_NW.mstr_lot_id[0] + " ";
		strBody		+= "STEP_CNT=" + st_NW.mstr_step_cnt[0] + " ";
		strBody		+= "SLOT_NO=" + st_NW.mstr_slot_no[0] + " ";
		strBody		+= "TEST_IDX=" + st_NW.mstr_test_idx[0];
		break;
	case NW_FAIL_INFO_SERACH:
		break;
	case NW_FAIL_INFO_SAVE:
		st_NW.mstr_function[0]			= "FAIL_INFO_SAVE";
		st_NW.mstr_eqp_id[0]			= st_lamp.mstr_equip_id;
		st_NW.mstr_lot_id[0]			= st_NW.mstr_lot_id[1];
		st_NW.mstr_step_cnt[0]			= st_NW.mstr_step_cnt[1];
		st_NW.mstr_fail_regnumber[0]	= st_NW.mstr_fail_regnumber[1];
		st_NW.mstr_pcbox_id[0]			= st_NW.mstr_pcbox_id[1];
		st_NW.mstr_slot_no[0]			= st_NW.mstr_slot_no[1];
		st_NW.mstr_test_idx[0]			= st_NW.mstr_test_idx[1];
		st_NW.mstr_fail_desc[0]			= st_NW.mstr_fail_desc[1];
		st_NW.mstr_fail_code[0]			= st_NW.mstr_fail_code[1];
		st_NW.mstr_fail_cnt[0]			= st_NW.mstr_fail_cnt[1];
		st_NW.mstr_retest_flag[0]		= st_NW.mstr_retest_flag[1];

		strBody		= "FUNCTION=" + st_NW.mstr_function[0] + " ";
		strBody		+= "EQP_ID=" + st_NW.mstr_eqp_id[0] + " ";
		strBody		+= "LOT_ID=" + st_NW.mstr_lot_id[0] + " ";
		strBody		+= "STEP_CNT=" + st_NW.mstr_step_cnt[0] + " ";
		strBody		+= "FAIL_REGNUMBER=" + st_NW.mstr_fail_regnumber[0] + " ";
		strBody		+= "PCBOX_ID=" + st_NW.mstr_pcbox_id[0] + " ";
		strBody		+= "SLOT_NO=" + st_NW.mstr_slot_no[0] + " ";
		strBody		+= "TEST_IDX=" + st_NW.mstr_test_idx[0] + " ";
		strBody		+= "FAIL_DESC=" + st_NW.mstr_fail_desc[0] + " ";
		strBody		+= "FAIL_CODE=" + st_NW.mstr_fail_code[0] + " ";
		strBody		+= "FAIL_CNT=" + st_NW.mstr_fail_cnt[0] + " ";
		strBody		+= "RETEST_FLAG=" + st_NW.mstr_retest_flag[0];
		break;
	case NW_FAIL_INFO_DELETE:
		st_NW.mstr_function[0]			= "FAIL_INFO_DELETE";
		st_NW.mstr_lot_id[0]			= st_NW.mstr_lot_id[1];
		st_NW.mstr_step_cnt[0]			= st_NW.mstr_step_cnt[1];
		st_NW.mstr_eqp_id[0]			= st_lamp.mstr_equip_id;
		st_NW.mstr_fail_regnumber[0]	= st_NW.mstr_fail_regnumber[1];

		strBody		= "FUNCTION=" + st_NW.mstr_function[0] + " ";
		strBody		+= "LOT_ID=" + st_NW.mstr_lot_id[0] + " ";
		strBody		+= "STEP_COUNT=" + st_NW.mstr_step_cnt[0] + " ";
		strBody		+= "EQP_ID=" + st_NW.mstr_eqp_id[0] + " ";
		strBody		+= "FAIL_REGNUMBER=" + st_NW.mstr_fail_regnumber[0];
		break;

	case NW_LOT_DISPLAY:
		break;

	case NW_EQP_LOT_START:
		break;

	case NW_EQP_LOT_CANCEL:
		break;

	case NW_EQP_LOT_END:
		break;
	case NW_PGM_START:
		break;
// ��������� ��� ������ �ϰų� �뺸�� �ϴ� �κ��̴�.

	case NW_BUFFER_IN_SEND:
		strBody		= "FUNCTION=BUFFER_IN";
		strBody		+= " LOT_NO=" + st_NW.mstr_m_lot_no[1];
		strBody		+= " EQP_ID=" + st_lamp.mstr_equip_id;
		strBody		+= " BUFFER_NO=" + st_NW.mstr_m_buffer_no[1];
		strBody		+= " PCT_BYPASS=" + st_NW.mstr_pct_bypass[1];
		strBody		+= " MDL_CNT=" + st_NW.mstr_m_module_cnt[1];

		for (i = 0; i < 20; i++)
		{
			strTemp.Format(" MDL_%02d=%s", i + 1, st_NW.mstr_m_module_data[i][1]);
			strBody		+= strTemp;
		}
		break;

	case NW_BUFFER_READY_SEND:
		strBody		= "FUNCTION=BUFFER_READY";
		strBody		+= " LOT_NO=" + st_NW.mstr_m_lot_no[1];
		strBody		+= " PART_NO=" + st_NW.mstr_m_part_no[1];
		strBody		+= " EQP_ID=" + st_lamp.mstr_equip_id;
		strBody		+= " BUFFER_NO=" + st_NW.mstr_m_buffer_no[1];
		strBody		+= " LAST_MODULE=" + st_NW.mstr_m_last_module[1];
		strBody		+= " BYPASS=" + st_NW.mstr_m_bypass[1];
		strBody		+= " RETEST=" + st_NW.mstr_m_retestcheck[1];				// ���� �߰� ����.

		{
			CString strTemp;
			strTemp.Format("[LAST MODULE] NW_BUFFER_READY_SEND	LOT:[%s]	LASTMODULE=[%s]", st_NW.mstr_m_lot_no[1], st_NW.mstr_m_last_module[1] );
			On_LogFile_Add(3, strTemp);
		}

		break;

	case NW_RETEST_REQ:
		strBody		= "FUNCTION=RETEST_REQ";
		strBody		+= " LOT_NO=" + st_NW.mstr_m_lot_no[1];
		strBody		+= " BUFFER_NO=" + st_NW.mstr_m_buffer_no[1];
		strBody		+= " SOCKET_OFF=" + st_NW.mstr_m_socket_off[1];
		break;

	case NW_BLOT_COUNT:
		strBody		= "FUNCTION=BLOT_COUNT";
		break;
	}

	nBody = strBody.GetLength();

	if (nBody > 0)
	{
		if (nID == NW_BUFFER_READY_SEND || nID == NW_BUFFER_IN_SEND || nID == NW_RETEST_REQ
			 || nID == NW_NEW_LOT_IN_SEND || nID == NW_LOT_NAME || nID == NW_LOT_DISPLAY || nID == NW_TRACK_OUT
			 || nID == NW_BPC_BUFFER_READY ||  nID == NW_BPC_BUFFER_IN || nID == NW_DELETE_SCRAP)
		{
			strHeader.Format("%016d", nBody);
		}
		else
		{
			strHeader.Format("%010d", nBody);
		}

		st_NW.mstr_header_msg[0] = strHeader;
		st_NW.mstr_data_msg[0] = strBody;
		st_NW.mstr_full_msg[0] = strHeader + strBody;
	}
	else
	{
		st_NW.mstr_header_msg[0] = "";
		st_NW.mstr_data_msg[0] = "";
		st_NW.mstr_full_msg[0] = "";
	}

	return st_NW.mstr_full_msg[0];
}

CString CPublic_Function::OnDeviceInfoAnalysis(CString strBody, int nInfo)
{
	CString strFuncRet = "NONE";
	int nLength;

	int n_start[5], n_check[5], n_length[5];    

	nLength = strBody.GetLength();
	if (nLength < 23)		return "FAIL";		// ���̰� 22���� ª���� FAIL�̴�.

	n_start[0] = 0;
	n_check[0] = strBody.Find("-", n_start[0]);

	if (n_check[0] != -1)
	{
		n_length[0] = n_check[0];
	}
	else
	{
		n_length[0] = 10;
		n_check[0] = 9;
	}

	n_start[1] = n_check[0] + 1;
	n_check[1] = strBody.Find("_", n_start[1]);
	n_length[1] = n_check[1] - n_start[1];

	n_start[2] = n_check[1] + 1;
	n_check[2] = strBody.Find("_", n_start[2]);
	n_length[2] = n_check[2] - n_start[2];

	n_start[3] = n_check[2] + 1;
	n_check[3] = strBody.Find("_", n_start[3]);
	n_length[3] = n_check[3] - n_start[3];

	n_start[4] = n_check[3] + 1;
	n_check[4] = strBody.Find("_", n_start[4]);
	n_length[4] = n_check[4] - n_start[4];

	if (nInfo < 4)
	{
		strFuncRet = strBody.Mid(n_start[nInfo], n_length[nInfo]);
	}
	else
	{
		strFuncRet = strBody.Mid(n_start[nInfo]);
	}
	return strFuncRet;
}

int CPublic_Function::FileSizeCheck(CString FileName, long size, int n_check)
{
	CFileFind finder;
	long Len=0, flag=0;
	BOOL bContinue;
	
	if (bContinue = finder.FindFile(FileName))
	{	
		if (n_check == CTL_YES)			// Size�� üũ�ϴ°��̸�...
		{
			finder.FindFile(FileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = finder.GetLength();
			if (Len > size)
			{
				flag = 1;
			}
			finder.Close();	
		}
		else						// Size�� Ȯ���ϴ°��̸�...
		{
			finder.FindFile(FileName);
			finder.FindNextFile();
			finder.GetFileName();
			Len = finder.GetLength();
			
			flag = Len;
			
			finder.Close();	
		}
	}
	
	return flag;
}

CString CPublic_Function::GetLocalMacAddress()
{
	NCB Ncb; 
	UCHAR uRetCode; 
	LANA_ENUM lenum; 
	int i; 
	CString str_value; 
	
	memset( &Ncb, 0, sizeof(Ncb) ); 
	Ncb.ncb_command = NCBENUM; 
	Ncb.ncb_buffer = (UCHAR *)&lenum; 
	Ncb.ncb_length = sizeof(lenum); 
	uRetCode = Netbios( &Ncb ); 
	
	for (i=0; i < lenum.length ;i++) 
	{ 
		memset( &Ncb, 0, sizeof(Ncb) ); 
		Ncb.ncb_command = NCBRESET; 
		Ncb.ncb_lana_num = lenum.lana[i]; 
		
		uRetCode = Netbios( &Ncb ); 
		memset( &Ncb, 0, sizeof (Ncb) ); 
		Ncb.ncb_command = NCBASTAT; 
		Ncb.ncb_lana_num = lenum.lana[i]; 
		
		strcpy( (char*)Ncb.ncb_callname, "* " ); 
		Ncb.ncb_buffer = (unsigned char *) &Adapter; 
		Ncb.ncb_length = sizeof(Adapter); 
		
		uRetCode = Netbios( &Ncb ); 
		
		if ( uRetCode == 0 ) 
		{ 
			str_value.Format("%02x-%02x-%02x-%02x-%02x-%02x", 
				Adapter.adapt.adapter_address[0], 
				Adapter.adapt.adapter_address[1], 
				Adapter.adapt.adapter_address[2], 
				Adapter.adapt.adapter_address[3], 
				Adapter.adapt.adapter_address[4], 
				Adapter.adapt.adapter_address[5] ); 
		} 
	} 	
	return str_value;
}

int CPublic_Function::Robot_Reinstatement_Position(int mode)
{
	int Ret = 0, i = 0, n_response = 0;
	int FuncRet = RET_PROCEED;
	int nRet = 0, nRet2 = 0;

	return RET_GOOD;

	CDialog_Select	select_dlg;
 
	if (mode == 0 && st_work.mn_reinstatement_mode == 0) // ���� �ʱ�ȭ �ĸ� ������ 
	{	// ���� ��ġ ��� 
		for (i = 0; i < MOTOR_COUNT; i++)
		{
// 			st_work.md_reinstatement_Pos[0][i] = st_motor[i].mcommandposbackup;
			st_work.md_reinstatement_Pos[0][i] = COMI.md_cmdpos_backup[i];
		}
						
		st_work.mn_reinstatement_mode = 1; //�ѹ� �����͸� �����Ѵ� 
		reinstatement_Step = 0;

		return RET_GOOD;
	}	 
	else if (mode == 0)
	{
		return RET_GOOD;
	}

	switch (reinstatement_Step)
	{
	case 0:
		if (st_work.mn_reinstatement_mode == 1) // ��ŸƮ Ű�� ������ 2�̴� 
		{
			reinstatement_Step = 1000;
		}
		break;

	case 1000:
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			st_work.md_reinstatement_Pos[0][i] = COMI.Get_MotCurrentPos(i);
		}

		mn_front_change = 0;
		mn_rear_change = 0;
		
		//����� ��ġ�� ���
		if (st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_X] > st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_X] + COMI.md_allow_value[MOTOR_ROBOT_X]
			|| st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_X] < st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_X] - COMI.md_allow_value[MOTOR_ROBOT_X])
		{
			mn_front_change++;
			sprintf(st_msg.c_abnormal_msg, "Robot X POS [OLD] : %ld -> [NOW] : %ld [GAP] : %ld", (long)st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_X], (long)st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_X], (long)COMI.md_allow_value[MOTOR_ROBOT_X]);
		}
		else if (st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Y] > st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y] + COMI.md_allow_value[MOTOR_ROBOT_Y]
			|| st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Y] < st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y] - COMI.md_allow_value[MOTOR_ROBOT_Y])
		{
			sprintf(st_msg.c_abnormal_msg, "Robot Y POS [OLD] : %ld -> [NOW] : %ld [GAP] : %ld", (long)st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y], (long)st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Y], (long)COMI.md_allow_value[MOTOR_ROBOT_Y]);
		}
		else if (st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Z] > st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] + COMI.md_allow_value[MOTOR_ROBOT_Z]
			|| st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Z] < st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] - COMI.md_allow_value[MOTOR_ROBOT_Z])
		{
			mn_rear_change++;
			sprintf(st_msg.c_abnormal_msg, "Robot Z POS [OLD] : %ld -> [NOW] : %ld [GAP] : %ld", (long)st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z], (long)st_work.md_reinstatement_Pos[1][MOTOR_ROBOT_Z], (long)COMI.md_allow_value[MOTOR_ROBOT_Z]);
		}
		
		if (st_handler.cwnd_list != NULL)
		{
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
		}

		st_msg.mstr_event_msg[0] = "Recovery Mode.......";
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

		st_work.mn_reinstatement_mode = 3; //�ʱ�ȭ ������ �Ϸ��ߴ�, Ŭ���� 

		if (mn_front_change > 0 && mn_rear_change > 0)
		{
			reinstatement_Step = 2000;
		}
		else if (mn_rear_change > 0)
		{
			reinstatement_Step = 2000;
		}
		else
		{
			reinstatement_Step = 2000;
		}
		break;

	case 1100:
		backuptime[0] = GetCurrentTime();
		reinstatement_Step = 1200;
		break;

	case 1200:
		backuptime[1] = GetCurrentTime();
		backuptime[2] = backuptime[1] - backuptime[0];
		if (backuptime[2] >= 500)
		{
			reinstatement_Step = 50000;
		}
		else if(backuptime[2] < 0)
		{
			backuptime[0] = GetCurrentTime();
		}
		break;

	case 1300:		
		backuptime[0] = GetCurrentTime();
		reinstatement_Step = 1400;
		break;
		
	case 1400:
		backuptime[1] = GetCurrentTime();
		backuptime[2] = backuptime[1] - backuptime[0];
		if (backuptime[2] >= 500)
		{
			reinstatement_Step = 50000;
		}
		else if(backuptime[2] < 0)
		{
			backuptime[0] = GetCurrentTime();
		}
		break;

	case 2000:
		//���⼭ ������ �����ߴ� ��ġ�� �ٽ� ����ؾ��Ѵ�..
		Robot_BackMovePosValCheck();

		for (i = 0; i < MOTOR_COUNT; i++)
		{
			COMI.Set_MotPower(i, IO_ON);
		}
		
		mn_check[0] = VARIABLE_INIT;
		mn_check[1] = VARIABLE_INIT;

		backuptime[0] = GetCurrentTime();
		reinstatement_Step = 2010;
		break;

	case 2010:
		backuptime[1] = GetCurrentTime();
		backuptime[2] = backuptime[1] - backuptime[0];
		if (backuptime[2] >= 1000)
		{
			reinstatement_Step = 2020;
		}
		else if(backuptime[2] < 0)
		{
			backuptime[0] = GetCurrentTime();
		}
		break;

	case 2020:
		nRet = COMI.HomeCheck_Mot(MOTOR_ROBOT_Z, MOT_TIMEOUT);
		
		if (nRet == CTLBD_RET_GOOD)
		{
			reinstatement_Step = 2100;
		}		
		else if (nRet != CTLBD_RET_PROCEED)
		{
			// 000001 0 A "Motor Home Check Error - (Front Shifter Z)."
			alarm.mstr_code = "000001";
			sprintf(alarm.mc_code, "000001");
			alarm.mn_count_mode = 0; alarm.mn_type_mode = CTL_dWARNING; COMI.mn_run_status = CTL_dWARNING;
			CtlBdFunc.Alarm_Error_Occurrence(10, alarm.mn_type_mode, COMI.mn_run_status, alarm.mstr_code);
		}
		break;

	case 2100:
		nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_ROBOT_Z_SAFETY], 10);
		
		if (nRet == CTLBD_RET_GOOD)
		{
			reinstatement_Step = 2200;
		}
		else if (nRet != CTLBD_RET_PROCEED)
		{
			// 000100 1 A "ROBOT Z Safety Pos Move Error."
			alarm.mstr_code = "090100";
			sprintf(alarm.mc_code, "090100");
			alarm.mn_count_mode = 0; alarm.mn_type_mode = CTL_dWARNING; COMI.mn_run_status = CTL_dWARNING;
			CtlBdFunc.Alarm_Error_Occurrence(11, alarm.mn_type_mode, COMI.mn_run_status, alarm.mstr_code);
		}
		break;
		
	case 2200:
		backuptime[0] = GetCurrentTime();
		reinstatement_Step = 2300;
		break;
		
	case 2300:
		backuptime[1] = GetCurrentTime();
		backuptime[2] = backuptime[1] - backuptime[0];
		if (backuptime[2] >= 500)
		{
			reinstatement_Step = 50000;
		}
		else if(backuptime[2] < 0)
		{
			backuptime[0] = GetCurrentTime();
		}
		break;

	case 50000:
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_NO, 0);

		reinstatement_Step = 0;
		st_handler.mn_reinstatement = CTL_NO;			// ���� ������ �ƴϴ�.
		st_work.mn_reinstatement_mode = 0;	// �ʱ�ȭ ������ �Ϸ��ߴ�, Ŭ���� 		
		FuncRet = RET_GOOD;
		break;
	}
 	return FuncRet;
}

void CPublic_Function::Robot_BackMovePosValCheck()
{
	int i = 0;
	double d_gap_pos = 0;

// 	st_work.d_reinstatement_x_pos = 0;
// 	st_work.d_reinstatement_y_pos = 0;
// 	st_work.d_reinstatement_z_pos = st_motor[MOTOR_ROBOT_Z].d_pos[P_ROBOT_Z_SAFETY];
// 
// 	if (st_work.n_back_type == BACK_SET_POS)
// 	{
// 		switch (st_work.n_back_type)
// 		{
// 		case BT_LENGTH_VARIABLE:
// 			st_work.d_reinstatement_x_pos = st_motor[MOTOR_ROBOT_X].d_pos[P_ROBOT_Z_SAFETY];
// 			st_work.d_reinstatement_y_pos = st_motor[MOTOR_ROBOT_Y].d_pos[P_ROBOT_Z_SAFETY];
// 			break;
// 			
// 		case BT_SAFETY_ZONE:
// 			st_work.d_reinstatement_x_pos = st_motor[MOTOR_ROBOT_X].d_pos[P_ROBOT_Z_SAFETY];
// 			st_work.d_reinstatement_y_pos = st_motor[MOTOR_ROBOT_Y].d_pos[P_ROBOT_Z_SAFETY];
// 			break;
// 		}
// 	}
// 	else
// 	{
// // 		st_work.d_reinstatement_x_pos = Run_Sort_Robot_Manual.XYRobot_MovePosValCheck(st_work.n_back_type, M_ROBOT_X, st_work.n_back_x_pos);
// // 		st_work.d_reinstatement_y_pos = Run_Sort_Robot_Manual.XYRobot_MovePosValCheck(st_work.n_back_type, M_ROBOT_Y, st_work.n_back_y_pos);
// 		
// 		switch (st_work.n_back_type)
// 		{
// 		case BT_REAR_BUFFER:
// 			break;
// 		case BT_FRONT_BUFFER:
// 			break;
// 		case BT_RETEST_BUFFER:
// 			break;
// 		case BT_REJECT_BUFFER:
// 			break;
// 		case BT_LOB_BUFFER:
// 			break;
// 		case BT_TRAY_1:
// 			break;
// 		case BT_TRAY_2:
// 			break;
// 		case BT_CUSTOMER_BUFFER:
// 			break;
// 		case BT_LENGTH_VARIABLE:
// 			break;
// 		case BT_SAFETY_ZONE:
// 			break;
// 		}
// 	}
// 	
// 	// z axis�� ���� �ٽ��ѹ��Ѵ�...
// 	st_work.d_reinstatement_z_pos = st_motor[M_ROBOT_Z].d_pos[st_work.n_back_z_movepos];
}

void CPublic_Function::ThreadFunctionStepTrace(int nPos, int nStep)
{
	if (nStep == st_work.mn_prev_step[nPos]) // step�� �����ϱ�~
	{
		return;
	}

// 	CString str_ybs;
// 	str_ybs.Format("[Pos:%02d] Prev : %d -> now %d",nPos,st_work.mn_prev_step[nPos],nStep);
// 	Func.On_LogFile_Add(LOG_DEBUG, str_ybs);


	st_work.mn_prev_step[nPos] = nStep;
	StepChangeTime[nPos] = CTime::GetCurrentTime();	 
	st_work.mstr_last_change_date[nPos] = StepChangeTime[nPos].Format("%y/%m/%d %H:%M:%S");	
	
	if (st_work.mstr_history_step[nPos].GetLength() == 120 || st_work.mstr_history_step[nPos].GetLength() == 276)
	{
		str_step_temp[nPos].Format("%05d|", nStep);
		st_work.mstr_history_step[nPos] += str_step_temp[nPos];
		st_work.mstr_history_step[nPos] += "\r\n|                          |";
	}
	else if (st_work.mstr_history_step[nPos].GetLength() == 432)
	{
		str_step_temp[nPos].Format("%05d|", nStep);
		st_work.mstr_history_step[nPos] += str_step_temp[nPos];

		st_work.mstr_history_step_back_up[nPos] = st_work.mstr_history_step[nPos];
		st_work.mstr_history_step[nPos] = "";
	}
	else
	{
		str_step_temp[nPos].Format("%05d,", nStep);
		st_work.mstr_history_step[nPos] += str_step_temp[nPos];
	}
}

void CPublic_Function::TextAndLine(int dir, CString str_sdata, CString str_data, FILE *fp)
{
	CString line, str_temp_data_temp, str_content;
	
	line = "";
	
	fprintf(fp,"%s", str_sdata);
	
	switch (dir)
	{
	case 6 :
		str_temp_data_temp.Format("%6s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+-------+";
		break;
		
	case 14:
		str_temp_data_temp.Format("%14s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+---------------+";   // +----------------------------------------+----------------------------------+
		break;
		
	case 15:
		str_temp_data_temp.Format("%33s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+----------------------------------+";
		break;
		
	case 24:
		str_temp_data_temp.Format("%24s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+-------------------------+";
		break;
		
	case 140:
		str_temp_data_temp.Format("%14s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s", line);
		break;
		
	case 30:
		str_temp_data_temp.Format("%30s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+-------------------------------+";
		break;
		
	case 131:
		str_temp_data_temp.Format("%30s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+-----------------------------------------------------------------------------------------------------------------------+";
		break;
		
	case 51:
		str_temp_data_temp.Format("%50s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s", line);
		break;
		
	case 49:
		str_temp_data_temp.Format("%30s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+---------------------------------------------------+";
		break;
		
	case 50:
		str_temp_data_temp.Format("%50s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+----------------------------------------+---------------------------------------------------+";
		break;

	case 130:
		str_temp_data_temp.Format("%124s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s\r\n", line);
		str_content = "+--------------------------+-----------------------------------------------------------------------------------------------------------------------------+";
		break;
		
	case 300:
		str_temp_data_temp.Format("%30s", str_data);
		line += str_temp_data_temp;
		line += " |";
		fprintf(fp,"%s", line);
		break;
	}
	fprintf(fp,"%s\r\n",str_content);
}

void CPublic_Function::DB_Write_Alarm(st_alarm_database st_alarmdb)
{	
	if (st_basic.n_mode_device == CTL_NO)			return;

	g_cs.EnterCriticalSection();

	CString strTmp[6];
	
	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", st_alarmdb.mole_date.GetYear(), 
		st_alarmdb.mole_date.GetMonth(),
		st_alarmdb.mole_date.GetDay(),
		st_alarmdb.mole_date.GetHour(),
		st_alarmdb.mole_date.GetMinute(),
		st_alarmdb.mole_date.GetSecond());
	
	
	strTmp[2].Format("%-1s", st_alarmdb.mstrlot_no);
	strTmp[3].Format("%-1s", st_alarmdb.mstrpart_no);
	
	strTmp[4].Format("%-1s", st_alarmdb.mstrAlarmCode);
	strTmp[5].Format("%-1s", st_alarmdb.mstrAlarmMsg);
	
	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "ALARM_DATA"))
			{
				Mysql.Record_Add("ALARM_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "ALARM_DATA", 6, Mysql.mstr_alarm_table_name, Mysql.mn_alarm_table_type, Mysql.mn_alarm_table_num))
				{
					Mysql.Record_Add("ALARM_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "ALARM_DATA", 6, Mysql.mstr_alarm_table_name, Mysql.mn_alarm_table_type, Mysql.mn_alarm_table_num))
				{
					Mysql.Record_Add("ALARM_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();

	g_cs.LeaveCriticalSection();
}

void CPublic_Function::DB_Write_Ground(st_ground_database st_grounddb)
{
	CString strTmp[7];
	
	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", st_grounddb.mole_date.GetYear(), 
		st_grounddb.mole_date.GetMonth(),
		st_grounddb.mole_date.GetDay(),
		st_grounddb.mole_date.GetHour(),
		st_grounddb.mole_date.GetMinute(),
		st_grounddb.mole_date.GetSecond());
		
	strTmp[2].Format("%-1s", st_grounddb.mstrChannel[0]);
	strTmp[3].Format("%-1s", st_grounddb.mstrChannel[1]);
	strTmp[4].Format("%-1s", st_grounddb.mstrChannel[2]);
	strTmp[5].Format("%-1s", st_grounddb.mstrChannel[3]);
	strTmp[6].Format("%-1s", st_grounddb.mstrChannel[4]);

	if ((strTmp[2] == "" || strTmp[2] == " ") && (strTmp[3] == "" || strTmp[3] == " ") && (strTmp[4] == "" || strTmp[4] == " ")
		&& (strTmp[5] == "" || strTmp[5] == " ") && (strTmp[6] == "" || strTmp[6] == " "))
	{
		return;
	}

	g_cs.EnterCriticalSection();

	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "GROUND_DATA"))
			{
				Mysql.Record_Add("GROUND_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "GROUND_DATA", 7, Mysql.mstr_ground_table_name, Mysql.mn_ground_table_type, Mysql.mn_ground_table_num))
				{
					Mysql.Record_Add("GROUND_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "GROUND_DATA", 7, Mysql.mstr_ground_table_name, Mysql.mn_ground_table_type, Mysql.mn_ground_table_num))
				{
					Mysql.Record_Add("GROUND_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();

	g_cs.LeaveCriticalSection();
}

void CPublic_Function::DB_Write_Buffer(st_buffer_database st_bufferdb)
{
	CString strTmp[9];
	
	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", st_bufferdb.mole_date.GetYear(), 
		st_bufferdb.mole_date.GetMonth(),
		st_bufferdb.mole_date.GetDay(),
		st_bufferdb.mole_date.GetHour(),
		st_bufferdb.mole_date.GetMinute(),
		st_bufferdb.mole_date.GetSecond());
	
	strTmp[2].Format("%-1s", st_bufferdb.mstrlot_no);
	strTmp[3].Format("%-1s", st_bufferdb.mstrpart_no);
	strTmp[4].Format("%d", st_bufferdb.mnbuffer_pos);
	strTmp[5].Format("%d", st_bufferdb.mnlast_module);
	strTmp[6].Format("%d", st_bufferdb.mnbypass);
	strTmp[7].Format("%d", st_bufferdb.mnmodule_cnt);
	strTmp[8].Format("%d", st_bufferdb.mnretest);
	
	g_cs.EnterCriticalSection();
	
	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "BUFFER_DATA"))
			{
				Mysql.Record_Add("BUFFER_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "BUFFER_DATA", 9, Mysql.mstr_buffer_table_name, Mysql.mn_buffer_table_type, Mysql.mn_buffer_table_num))
				{
					Mysql.Record_Add("BUFFER_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "BUFFER_DATA", 9, Mysql.mstr_buffer_table_name, Mysql.mn_buffer_table_type, Mysql.mn_buffer_table_num))
				{
					Mysql.Record_Add("BUFFER_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();
	
	g_cs.LeaveCriticalSection();
}

CString CPublic_Function::GetStrCurrentTime()
{
	COleDateTime timeCur;
	int iYear, iMonth, iDay, iHour, iMin, iSec;
	CString strTime;
	
	timeCur = COleDateTime::GetCurrentTime();

	iYear = timeCur.GetYear();
	iMonth = timeCur.GetMonth();
	iDay = timeCur.GetDay();

	iHour = timeCur.GetHour();
	iMin = timeCur.GetMinute();
	iSec = timeCur.GetSecond();

	strTime.Format("%04d/%02d/%02d_%02d:%02d:%02d", iYear, iMonth, iDay, iHour, iMin, iSec );
	return strTime;
}

void CPublic_Function::OnLogGalmg_PushLot( CString strlot, CString strtmp )
{
	CString strTemp;
	strTemp.Format( "[PUSH LOT] (%s) 0: [%s] / 1: [%s] / 2: [%s] : %s", strlot, st_handler.mstr_lot_name[0], st_handler.mstr_lot_name[1], st_handler.mstr_lot_name[2], strtmp );
	On_LogFile_Add(3, strTemp);
}

void CPublic_Function::OnLogWorkStep( CString strClass, CString strFunc, int iOld, int iCur )
{
	CString strLog;
	strLog.Format( "%s	%s			%d -------> %d", strClass, strFunc, iOld, iCur );
	On_LogFile_Add( LOG_WORK, strLog );
}

void CPublic_Function::MsgLog( LPCTSTR strMsg )
{
	if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
	{
		sprintf(st_msg.c_abnormal_msg, strMsg);
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
	}
}

void CPublic_Function::DeleteOldFiles( LPCTSTR lpDirPath, int iDays )
{
	if ( lpDirPath == NULL )
		return;

	CString strNextDirPath = "";
	CFileFind find;
	BOOL bFind = find.FindFile( lpDirPath );

	if ( bFind == FALSE )
		return;

	while( bFind )
	{
		bFind = find.FindNextFile();

		// . or .. pass
		if ( find.IsDots() == TRUE )
			continue;

		if ( find.IsDirectory() )
		{
			CString strDir;
			strDir.Format("%s\\*.*", find.GetFilePath() );
			DeleteOldFiles( strDir, iDays );
		}
		else
		{
			HANDLE hF = CreateFile(find.GetFileName(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

			SYSTEMTIME st;
			GetLocalTime(&st);
			
			CTime tNow(st.wYear, st.wMonth, st.wDay, 0, 0, 0);
			CTime tTime;
			find.GetLastWriteTime(tTime);
			
			CTimeSpan span = tNow - tTime;
			int iDays = span.GetDays();

			if ( span.GetDays() > iDays )
			{
				DeleteFile( find.GetFilePath() );

				CString strLog;
				strLog.Format("Delete File - Lost %d Days : %s", iDays, find.GetFilePath() );
				Func.On_LogFile_Add(3, strLog);
			}
		}
	}

	find.Close();
}

void CPublic_Function::SetRBufferStation( int iPos, int iVal, bool bWrite )
{
	if ( st_sync.mn_rbuffer_station[iPos] != iVal )
	{
		st_sync.mn_rbuffer_station[iPos] = iVal;

		if ( st_handler.cwnd_main != NULL )
		{
			st_handler.cwnd_main->SendMessage( WM_DRAW_WORK_SITE, 0, 0 );
		}

		if ( bWrite  )
		{
			std::string strPos = "Front";
			if ( iPos == 1 )
				strPos = "Rear";
			
//			char strKey[256];
//			sprintf( strKey, "%s%s_RBuffer", REG_KEY_SITE, strPos.c_str() );
//			AMTRegistry::RegWriteInt( strKey, REG_VALUE_RBUFFER_STATION, st_sync.mn_rbuffer_station[iPos] );
		}
	}
}

bool CPublic_Function::CheckWorkingLot( CString strLotNo )
{
	for( int i=0; i<3; i++ )
	{
		if ( st_handler.mstr_lot_name[i] == strLotNo )
			return true;
	}
	return false;
}

void CPublic_Function::On_Log_Title_Msg( int iLogType, CString strTitle, CString strMsg )
{
	CSingleLock singlog(&plogmutex);
	
	singlog.Lock();
	
	CString strTemp;
	strTemp.Format("|%s| [%s]", strTitle, strMsg);
	On_LogFile_Add( iLogType, strTemp );

	singlog.Unlock();
}

void CPublic_Function::SetActionParam()
{
	pMainFrm = (CMainFrame*)::AfxGetMainWnd();   ////////���α׷��� ���� �ɶ�...MainFrm�� �ʱ�ȭ ���ش�..
	ASSERT(pMainFrm);
}

//==================================================================//
// ���μ����� ���� ����
//==================================================================//
char CPublic_Function::GetNtPrivilege()
{
	HANDLE h_token;
	TOKEN_PRIVILEGES privilege_info;
	
	// ���� ���μ����� ���Ѱ� ���õ� ������ �����ϱ� ���� ��ū������ ����
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |
		TOKEN_QUERY, &h_token))
	{
		// ���Ѱ� ���õ� ���� ���ٿ� ������
		return 0;
	}
	
	// ���� ���μ����� SE_SHUTDOWN_NAME ������ ����Ҽ� �ֵ��� �����Ѵ�
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &privilege_info.Privileges[0].Luid);
	privilege_info.PrivilegeCount = 1;
	privilege_info.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	// ������ ������ ������ �����Ѵ�
	AdjustTokenPrivileges(h_token, FALSE, &privilege_info, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
	{
		// ���� ������ ������ ���
		return 0;
	}
	return 1;
}

void CPublic_Function::DateTimeChange(CString sDateTime)
{
	// �ð� ����
	SYSTEMTIME csTime;
	csTime.wYear	= atoi(sDateTime.Mid(0,4));
	csTime.wMonth	= atoi(sDateTime.Mid(4,2));
	csTime.wDay		= atoi(sDateTime.Mid(6,2));
	csTime.wHour	= atoi(sDateTime.Mid(8,2));
	csTime.wMinute	= atoi(sDateTime.Mid(10,2));
	csTime.wSecond	= atoi(sDateTime.Mid(12,2));
	csTime.wMilliseconds = 0;
	
	GetNtPrivilege();  // ���μ����� ���� ����
	
	if (SetLocalTime(&csTime))
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_normal_msg, "�ý��� ��¥�� ����Ǿ����ϴ�. : %s", sDateTime);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "�ý��� ��¥�� ������ �� �����ϴ�. : %s", sDateTime);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
}

void CPublic_Function::OnLotDataSave()
{
	CString mstr_file_name;    // ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;  // �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;  // �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int mn_existence;      // ���� ���� ���� ���� �÷��� 
	char fileName[256];    // �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;            // ���Ͽ� ���� ������ ���� ���� 
	
	CString str_title_time, str_file_time, str_new_file;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int n_cur_year, n_cur_month, n_cur_day;		// ��, ��, �� ���� ����
	int n_cur_hour, n_cur_minute, n_cur_second;		// ��, ��, �� ���� ����
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur;							// Ÿ��Ʋ �ð� ���� ����

	CString str_temp_data, strTemp;

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
	
	str_new_file.Format("LOTDATA%04d%02d%02d[%s]", n_cur_year, n_cur_month, n_cur_day, st_handler.mstr_lot_name[0]);
	str_time.Format("%04d/%02d/%02d %02d:%02d:%02d", n_cur_year, n_cur_month, n_cur_day, n_cur_hour, n_cur_minute, n_cur_second);
		
	mstr_create_file = st_path.str_lot_data_path + str_new_file;
	mstr_create_file += ".TXT";
	
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file = st_path.str_lot_data_path + str_new_file;
		mstr_create_file += ".TXT";
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
	
	mstr_content.Format("| %-100s |", "Lot Data Info File");
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "DIVISION  : AMT Coporation");
	fprintf(fp,"%s\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "File Name : " + st_path.str_lot_data_path + str_new_file);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "Save Time : " + str_time);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "Lot Name  : " + st_handler.mstr_lot_name[0]);
	fprintf(fp,"%s\r\n", mstr_content) ;

	if (st_handler.str_main_part.GetLength() >= 16)
	{
		mstr_content.Format("| %-100s |", "Part No.  : " + st_handler.str_main_part.Mid(0, 16));
		fprintf(fp,"%s\r\n", mstr_content) ;
	}

	strTemp.Format("%06d", st_work.nMdlInputCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Input    : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlPassCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Pass     : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", st_work.nMdlRejectCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Reject   : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", st_work.nMdlBtoCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Bto      : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", st_work.nMdlNtoCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Nto      : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", st_work.nMdlHtoCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Hto      : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", st_work.nMdlFailCount[0][0]);
	mstr_content.Format("| %-100s |", "Prime Fail     : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	if (st_work.nMdlPassCount[0][0] == 0)
	{
		st_work.fMdlyield[0][0] = 0;
	}
	else
	{
		st_work.fMdlyield[0][0] = (double)((double)st_work.nMdlPassCount[0][0] / (double)((double)st_work.nMdlPassCount[0][0] + (double)st_work.nMdlRejectCount[0][0])) * (double)100;
	}
	
	strTemp.Format("%3.2f%%", st_work.fMdlyield[0][0]);
	
	mstr_content.Format("| %-100s |", "Prime Yield    : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content);

	mstr_content.Format("| %-100s |", "" + strTemp);
	fprintf(fp,"%s\r\n", mstr_content);

	strTemp.Format("%06d", st_work.nMdlInputCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Input      : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlPassCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Pass       : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlRejectCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Reject     : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlBtoCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Bto        : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlNtoCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Nto        : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlHtoCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Hto        : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", st_work.nMdlFailCount[1][0]);
	mstr_content.Format("| %-100s |", "Cum Fail       : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	if (st_work.nMdlPassCount[1][0] == 0)
	{
		st_work.fMdlyield[1][0] = 0;
	}
	else
	{
		st_work.fMdlyield[1][0] = (double)((double)st_work.nMdlPassCount[1][0] / (double)((double)st_work.nMdlPassCount[1][0] + (double)st_work.nMdlRejectCount[1][0])) * (double)100;
	}
	
	strTemp.Format("%3.2f%%", st_work.fMdlyield[1][0]);

	mstr_content.Format("| %-100s |", "Cum Yield      : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content = "********************************************************************************************************";
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	str_temp_data = "";
	
	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	
	fclose(fp);  /* ���� ���� */
}

void CPublic_Function::OnDailyDataSave()
{
	CString mstr_file_name;    // ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;  // �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;  // �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int mn_existence;      // ���� ���� ���� ���� �÷��� 
	char fileName[256];    // �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;            // ���Ͽ� ���� ������ ���� ���� 
	
	CString str_title_time, str_file_time, str_new_file;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int n_cur_year, n_cur_month, n_cur_day;		// ��, ��, �� ���� ����
	int n_cur_hour, n_cur_minute, n_cur_second;		// ��, ��, �� ���� ����
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur;							// Ÿ��Ʋ �ð� ���� ����

	CString str_temp_data, strTemp;

	CString strData, str_time;

	float fMdlyield;
	
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
	
	str_new_file.Format("DAY_DATA%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
	str_time.Format("%04d/%02d/%02d %02d:%02d:%02d", n_cur_year, n_cur_month, n_cur_day, n_cur_hour, n_cur_minute, n_cur_second);
		
	mstr_create_file = st_path.str_lot_data_path + str_new_file;
	mstr_create_file += ".TXT";
	
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file = st_path.str_lot_data_path + str_new_file;
		mstr_create_file += ".TXT";
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
	
	mstr_content.Format("| %-100s |", "Day Data Info File");
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	mstr_content.Format("| %-100s |", "DIVISION  : AMT Coporation");
	fprintf(fp,"%s\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "File Name : " + st_path.str_lot_data_path + str_new_file);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content.Format("| %-100s |", "Save Time : " + str_time);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetInputCnt(PRIME));
	mstr_content.Format("| %-100s |", "Input Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetPassCnt(PRIME));
	mstr_content.Format("| %-100s |", "Pass  Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format("%06d", g_lotMgr.GetFailCnt(PRIME));
	mstr_content.Format("| %-100s |", "Fail Prime : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content);

	fMdlyield = 0;
	
	if (g_lotMgr.GetPassCnt(PRIME) == 0)
	{
		fMdlyield = 0;
	}
	else
	{
		fMdlyield = (double)g_lotMgr.GetPassCnt(PRIME) / (double)g_lotMgr.GetInputCnt(PRIME) * 100.0f;
	}
	
	strTemp.Format("%3.2f%%", fMdlyield);
	
	mstr_content.Format("| %-100s |", "Prime Yield  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;


	// CUM
	strTemp.Format("%06d", g_lotMgr.GetInputCnt(CUM));
	mstr_content.Format("| %-100s |", "Input Cum   : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", g_lotMgr.GetPassCnt(CUM));
	mstr_content.Format("| %-100s |", "Pass  Cum   : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	strTemp.Format("%06d", g_lotMgr.GetFailCnt(CUM));
	mstr_content.Format("| %-100s |", "Fail Cum  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	fMdlyield = 0;

	if (g_lotMgr.GetPassCnt(CUM) == 0)
	{
		fMdlyield = 0;
	}
	else
	{
		fMdlyield = (double)g_lotMgr.GetPassCnt(CUM) / (double)g_lotMgr.GetInputCnt(CUM) * 100.0f;
	}
	
	strTemp.Format("%3.2f%%", fMdlyield);
	
	mstr_content.Format("| %-100s |", "Cum   Yield  : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;
	mstr_content.Format("| %-100s |", "");
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDR.GetTotalHours(), st_handler.m_tDR.GetMinutes(), st_handler.m_tDR.GetSeconds() );
	mstr_content.Format("| %-100s |", "Run Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDS.GetTotalHours(), st_handler.m_tDS.GetMinutes(), st_handler.m_tDS.GetSeconds() );
	mstr_content.Format("| %-100s |", "Stop Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%02d:%02d:%02d", st_handler.m_tDJ.GetTotalHours(), st_handler.m_tDJ.GetMinutes(), st_handler.m_tDJ.GetSeconds() );
	mstr_content.Format("| %-100s |", "Alarm Time : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	strTemp.Format( "%d", g_handler.GetAlarmCnt() );
	mstr_content.Format("| %-100s |", "Alarm Count : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	int nSec_MTBI = st_handler.m_tDR.GetTotalHours() * 3600 + st_handler.m_tDR.GetMinutes() * 60 + st_handler.m_tDR.GetSeconds();
	if( g_handler.GetAlarmCnt() == 0 )
		nSec_MTBI = 0;
	else
	{
		nSec_MTBI /= g_handler.GetAlarmCnt();
	}

	strTemp.Format( "%02d:%02d:%02d", nSec_MTBI / 3600, (nSec_MTBI % 3600) / 60, nSec_MTBI % 60 );
	mstr_content.Format("| %-100s |", "MTBI : " + strTemp);
	fprintf(fp,"%s\r\n", mstr_content) ;

	mstr_content = "********************************************************************************************************";
	fprintf(fp,"%s\r\n", mstr_content) ;
	
	str_temp_data = "";
	
	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	
	fclose(fp);  /* ���� ���� */
}

void CPublic_Function::OnDayDataReset()
{
	CString mstr_temp;
	int k,j;
	
	st_handler.m_tDR = 0;
	st_handler.m_tDJ = 0;
	st_handler.m_tDM = 0;
	st_handler.m_tDS = 0;
	st_handler.m_tDRef = 0;
	
	for (k = 0; k<20; k++)
	{
		st_handler.m_tDRdown[k] = 0; 
		st_handler.m_tDStop[k] = 0;
		st_handler.m_tDUserStop[k] = 0;
		st_handler.m_tDInStandby[k] = 0;  
		////////
	}
	
	for ( k = 0; k < 10; k++)
	{
		for ( j = 0; j < 2; j++)
		{
			st_handler.m_tDtest[j][k] = 0;
			st_handler.m_tDSokOff[j][k] = 0;
			st_handler.m_tDOutStandby[j][k] = 0;
		}

		Run_Test_Site[0].mn_Fail[k] = 0;
		Run_Test_Site[1].mn_Fail[k] = 0;
		Run_Test_Site[0].mn_Pass[k] = 0;
		Run_Test_Site[1].mn_Pass[k] = 0;
		Run_Test_Site[0].mn_Input[k] = 0;
		Run_Test_Site[1].mn_Input[k] = 0;
	}

	mstr_temp.Format("%d",st_handler.m_tDR);
	:: WritePrivateProfileString("BasicData", "Daily_Run_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	mstr_temp.Format("%d",st_handler.m_tDS);
	:: WritePrivateProfileString("BasicData", "Daily_Stop_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	mstr_temp.Format("%d",st_handler.m_tDJ);
	:: WritePrivateProfileString("BasicData", "Daily_Alarm_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	mstr_temp.Format("%d",st_handler.m_tDM);
	:: WritePrivateProfileString("BasicData", "Daily_Maint_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	////////////2015.0914
	mstr_temp.Format("%d",st_handler.m_tDRef);
	:: WritePrivateProfileString("BasicData", "Daily_Ref_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	CString str;
	
	for (k =0; k<TSITE_SOCKET_CNT; k++)
	{	
		str.Format("Daily_RunDown_Time[%d]", k);
		mstr_temp.Format("%d",st_handler.m_tDRdown[k]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("Daily_Stop[%d]", k);
		mstr_temp.Format("%d",st_handler.m_tDStop[k]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("Daily_UserStop[%d]", k);
		mstr_temp.Format("%d",st_handler.m_tDUserStop[k]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("InStandBy_Time[%d]",  k);
		mstr_temp.Format("%d",st_handler.m_tDInStandby[k]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
	}

	for ( k = 0; k < TSITE_INSERT_CNT; k++ )
	{
		for ( j = 0; j < 2; j++)
		{
			str.Format("Daily_Test_Time[%d]", (j*10) + k);
			mstr_temp.Format("%d",st_handler.m_tDtest[j][k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
			
			str.Format("Daily_SokOff_Time[%d]", (j*10) + k);
			mstr_temp.Format("%d",st_handler.m_tDSokOff[j][k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
			
			str.Format("OutStandBy_Time[%d]", (j*10) + k);
			mstr_temp.Format("%d",st_handler.m_tDOutStandby[j][k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
			
			str.Format("Fail_Device[%d]", (j*10) + k);
			mstr_temp.Format("%d",Run_Test_Site[j].mn_Fail[k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);

			str.Format("Pass_Device[%d]", (j*10) + k);
			mstr_temp.Format("%d",Run_Test_Site[j].mn_Pass[k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
			
			str.Format("Input_Device[%d]", (j*10) + k);
			mstr_temp.Format("%d",Run_Test_Site[j].mn_Input[k]);
			:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		}
	}

	g_lotMgr.ClearCountData();
	g_handler.ClearAlarmCnt();

	if (st_handler.cwnd_main != NULL)
	{
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_COUNTINFO, 0);
	}
	
}

void CPublic_Function::GlobalDataSerialize(CArchive &ar)
{
	int i=0; int j=0; int k=0; int l=0; int m=0;

	CObject::Serialize(ar);
	
	if (ar.IsStoring())
	{
		ar << st_handler.mn_menu_num;					// �� ȭ�鿡 ������ ȭ�� ��ȣ ���� ���� 
		ar << st_handler.b_program_exit;				// ���α׷��� ���������°��� Ȯ���Ѵ�.
		
		// *************************************************************************
		// ��� ���� ���� ���� ���� ����                                            
		// *************************************************************************
		ar << st_handler.mn_initial;					// �ʱ�ȭ �۾� �Ϸ� �÷���
		ar << st_handler.mn_manual;

		ar << st_handler.mn_menu_lock;					// �޴� ��� ���� ���� �÷��� 
		ar << st_handler.mn_system_lock;				// SYSTEM LOCK ���� �÷���
		ar << st_handler.mn_switch_lock;				// Dialog�� �� �ִ� ���� �÷��� (Start Switch Lock)

		ar << st_handler.mstr_pass_level;
		ar << st_handler.mn_level_maint;				// ����Ʈ ����(Level 1) ���� �÷���
		ar << st_handler.mn_level_teach;				// ƼĪ ����(Level 2) ���� �÷���
		ar << st_handler.mn_level_speed;				// ���ǵ� ����(Level 3) ���� �÷���
		ar << st_handler.mn_level_admin;				// Admin ����(Level 4) ���� �÷���

		for (i = 0; i < MAXSITE; i++)
		{
			ar << st_handler.mn_init_state[i];			// �ʱ�ȭ �Ϸ� �÷���
		}

		ar << st_handler.mn_initial_error;

	}
	else
	{
		ar >> st_handler.mn_menu_num;					// �� ȭ�鿡 ������ ȭ�� ��ȣ ���� ���� 
		ar >> st_handler.b_program_exit;				// ���α׷��� ���������°��� Ȯ���Ѵ�.
		
		// *************************************************************************
		// ��� ���� ���� ���� ���� ����                                            
		// *************************************************************************
		ar >> st_handler.mn_initial;					// �ʱ�ȭ �۾� �Ϸ� �÷���
		ar >> st_handler.mn_manual;

		ar >> st_handler.mn_menu_lock;					// �޴� ��� ���� ���� �÷��� 
		ar >> st_handler.mn_system_lock;				// SYSTEM LOCK ���� �÷���
		ar >> st_handler.mn_switch_lock;				// Dialog�� �� �ִ� ���� �÷��� (Start Switch Lock)

		ar >> st_handler.mstr_pass_level;
		ar >> st_handler.mn_level_maint;				// ����Ʈ ����(Level 1) ���� �÷���
		ar >> st_handler.mn_level_teach;				// ƼĪ ����(Level 2) ���� �÷���
		ar >> st_handler.mn_level_speed;				// ���ǵ� ����(Level 3) ���� �÷���
		ar >> st_handler.mn_level_admin;				// Admin ����(Level 4) ���� �÷���

		for (i = 0; i < MAXSITE; i++)
		{
			ar >> st_handler.mn_init_state[i];			// �ʱ�ȭ �Ϸ� �÷���
		}

		ar >> st_handler.mn_initial_error;

	}
}

void CPublic_Function::Handler_Recovery_Data_Write()
{	
	int nFuncRet, nRet, nCancleFlag=CTL_NO;
	
	:: WritePrivateProfileString("SaveMode", "RecoveryMode", LPCTSTR("1"), st_path.mstr_file_basic);
	
	do{
		nFuncRet = Handler_Recovery_Data(DATA_WRITE);
		
		if (nFuncRet == CTL_ERROR)
		{
			nRet = AfxMessageBox("Recovery Data Save���߿� ������ �߻��Ͽ����ϴ�. ����ϸ� ���� ������ ������ ��� �˴ϴ�. ��õ��Ͻðڽ��ϱ� ?", MB_OKCANCEL);
			if(nRet != IDOK)
			{
				nCancleFlag = CTL_YES;
			}
		}
		else
			
			if(nCancleFlag == CTL_YES)
			{
				:: WritePrivateProfileString("SaveMode", "RecoveryMode", LPCTSTR("0"), st_path.mstr_file_basic);
			}
			
	}
	while(nFuncRet == CTL_ERROR && nCancleFlag == CTL_NO);
}

void CPublic_Function::DB_Write_Boat(st_boat_database st_boatdb)
{
	CString strTmp[15];
	
	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", st_bufferdb.mole_date.GetYear(), 
		st_bufferdb.mole_date.GetMonth(),
		st_bufferdb.mole_date.GetDay(),
		st_bufferdb.mole_date.GetHour(),
		st_bufferdb.mole_date.GetMinute(),
		st_bufferdb.mole_date.GetSecond());
	
	strTmp[2].Format("%-1s", st_boatdb.mstrlot_no);
	strTmp[3].Format("%-1s", st_boatdb.mstrpart_no);
	strTmp[4].Format("%d", st_boatdb.mn_boat_id);
	strTmp[5].Format("%d", st_boatdb.mn_product_id);
	strTmp[6].Format("%d", st_boatdb.mn_pcb_cnt);

	for (int i = 0; i < 8; i++)
	{
		strTmp[7 + i].Format("%d", st_boatdb.mn_bin[i]);
	}
	
	g_cs.EnterCriticalSection();
	
	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "BOAT_DATA"))
			{
				Mysql.Record_Add("BOAT_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "BOAT_DATA", 14, Mysql.mstr_boat_table_name, Mysql.mn_boat_table_type, Mysql.mn_boat_table_num))
				{
					Mysql.Record_Add("BOAT_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "BOAT_DATA", 14, Mysql.mstr_boat_table_name, Mysql.mn_boat_table_type, Mysql.mn_boat_table_num))
				{
					Mysql.Record_Add("BOAT_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();
	
	g_cs.LeaveCriticalSection();
}

BOOL CPublic_Function::Handler_Recovery_Data_Read()
{
	char chr_data[20];
	::GetPrivateProfileString( "SaveMode", "RecoveryMode", "1", chr_data, 20, st_path.mstr_file_basic );
	BOOL bRecoveryFlag = BOOL( atoi(chr_data) );

	int nFuncRet = CTL_ERROR;
	if( bRecoveryFlag == CTL_YES )
	{
		CDialog_Select dlgSelect;
		st_msg.mstr_confirm_msg = _T("���� ���·� ���� �Ͻðڽ��ϱ�? \r\n(����: ���� �ʱ�ȭ ��ŵ)");
		if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Do you recovery previous status? \r\n(Caution: Skip Mootr Init)");
		
		if( dlgSelect.DoModal() == IDOK )
		{
			bool bRetry = false;
			do
			{
				nFuncRet = Handler_Recovery_Data( DATA_READ );
				if( nFuncRet == CTL_ERROR )
				{
					int nRet = AfxMessageBox("Recovery Data Read ���߿� ������ �߻��Ͽ����ϴ�. ����ϸ� ���� ������ �б�� ��� �˴ϴ�. ��õ��Ͻðڽ��ϱ� ?", MB_OKCANCEL);
					if( nRet == IDOK )
						bRetry = true;
				}
			}while( bRetry );
		}

	}

	if( bRecoveryFlag && nFuncRet == CTL_YES )
		return TRUE;

	return FALSE;
}

void CPublic_Function::OnInitBtn(int iVal)
{
	if( st_handler.cwnd_init )
		st_handler.cwnd_init->SendMessage( WM_INIT_BTN, iVal );
}

CString CPublic_Function::GetFileString( LPCSTR lpApp, LPCSTR lpKey, LPCSTR lpDef, DWORD dwSize, LPCSTR lpFilePath )
{
	char chr_data[MAX_PATH];
	:: GetPrivateProfileString(lpApp, lpKey, lpDef, chr_data, dwSize, st_path.mstr_basic);
	CString mstr_temp = chr_data;
	mstr_temp.MakeUpper();     // ���ڿ� �빮�ڷ� ����
	mstr_temp.TrimLeft(' ');   // ���� ���ڿ� ���� ����              
	mstr_temp.TrimRight(' ');  // ���� ���ڿ� ���� ����

	return mstr_temp;
}

int CPublic_Function::GetCurrentRbtPos()
{
	double dCurrPos[2];
	int nFuncRet = CRBT_NONE;

	dCurrPos[0] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_X);
	dCurrPos[1] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y);

	if(dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 5.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + st_handler.md_picker_position[3] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_TRAY1;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 5.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + st_handler.md_picker_position[3] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_TRAY2;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + st_handler.md_picker_position[3] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_TRAY3;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 5.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + st_handler.md_picker_position[3] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_REJECT;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT1;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT1;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT2;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT2;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT3;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT3;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_4] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_4] + st_handler.md_picker_position[1] +  COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT4;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT4;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT5;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT5;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT1;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT1;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_2] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_2] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT2;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT2;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_3] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 10.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_3] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT3;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] - COMI.md_allow_value[MOTOR_ROBOT_X] - 10.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_X] + 10.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT3;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 5.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT4;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT4;
		}
	}
	if(nFuncRet == -1 && dCurrPos[1] > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] - 5.0f &&
		dCurrPos[1] < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] + st_handler.md_picker_position[1] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
	{
		if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE2_INSERT5;
		}
		else if(dCurrPos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
			dCurrPos[0] < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			nFuncRet = CRBT_SITE1_INSERT5;
		}
	}
	return nFuncRet;
}

void CPublic_Function::OnSavePPID( bool bSkipTool )
{
	CString str_content;    // �� ���ο� ����� ���ڿ� ���� ����
	CString str_temp_data;  // ������ ���� �ӽ� ���� ����
	CString str_folder_data;					// ������ ���� ���� ���� ������ ���� ���� ����
	CString str_new_file;						// ������ ���ϸ� ���� ����
	CString str_title_time;						// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����

	CString str_temp_onoff;

	CString str_file_folder,mstr_temp; 

	str_folder_data = (LPCTSTR)st_path.mstr_recipe_path;
// 	str_temp_data.Format( "%s%s", str_folder_data, st_basic.mstr_device_name );
	//2013,1216
	str_temp_data.Format( "%s%s", str_folder_data, st_basic.mstr_revipe_name  + ".txt");

	DeleteFile(str_temp_data);


//	str_file_folder.Format( "%s%s",str_folder_data,st_basic.mstr_device_name );
// 	str_file_folder.Format( "%s%s",str_folder_data,st_basic.mstr_revipe_name );
	str_file_folder.Format( "%s%s",str_folder_data,st_basic.mstr_revipe_name + ".txt");

	// ************************************************************************** //
	// ���Ͽ� Ÿ��Ʋ ���� ����Ѵ�                                                //
	// ************************************************************************** //
	char chr_buf[20];

	mstr_temp = (LPCTSTR)st_lamp.mstr_equip_no;
	:: WritePrivateProfileString("Machine Serial", "Name", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp = (LPCTSTR)st_basic.mstr_device_name;
	:: WritePrivateProfileString("Recipe", "Name", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp = LPCTSTR(_itoa(st_handler.mn_retest_cnt, chr_buf, 10));
	:: WritePrivateProfileString("Mode", "Retest", LPCTSTR(mstr_temp), str_file_folder);

	////2015.0518
	mstr_temp = LPCTSTR(_itoa(st_handler.mn_multi_retest_cnt, chr_buf, 10));
	:: WritePrivateProfileString("Mode", "Multi_Retest", LPCTSTR(mstr_temp), str_file_folder);
	////

	mstr_temp.Format("%d", st_basic.n_cok_type);
	:: WritePrivateProfileString("Mode", "COK", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_time_out, chr_buf, 10));
	:: WritePrivateProfileString("Mode", "TEST TIMEOUT", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_run, chr_buf, 10));
	:: WritePrivateProfileString("Mode", "Mode", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp.Format("%d", st_basic.n_mode_jig);
	:: WritePrivateProfileString("Mode", "Align Buffer", LPCTSTR(mstr_temp), str_file_folder);

	////2015.0914
	mstr_temp.Format("%d", st_basic.n_retest_jig);
	:: WritePrivateProfileString("Mode", "Retest Jig Buffer", LPCTSTR(mstr_temp), str_file_folder);
	////

	mstr_temp.Format("%d", st_handler.mn_AutoSocketOff_Cnt);
	:: WritePrivateProfileString("Mode", "Auto Socket Off", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_bypass_mode, chr_buf, 10));
	:: WritePrivateProfileString("Mode", "BYPASS MODE", LPCTSTR(mstr_temp), str_file_folder);

	mstr_temp.Format("%d", st_basic.n_mode_device);
	:: WritePrivateProfileString("Mode", "Device mode", LPCTSTR(mstr_temp), str_file_folder);
	// ************************************************************************** //
	mstr_temp.Format("%s", st_basic.mstr_revipe_name);
	:: WritePrivateProfileString("Mode", "RECIPE_NAME", LPCTSTR(mstr_temp), str_file_folder);

	if( g_lotMgr.GetLotCount() <= 0)
	{
		mstr_temp="";
		:: WritePrivateProfileString("Mode", "LOT_NAME", LPCTSTR(mstr_temp), str_file_folder);
	}
	else
	{
		mstr_temp=g_lotMgr.GetLotAt(0).GetLotID();
		:: WritePrivateProfileString("Mode", "LOT_NAME", LPCTSTR(mstr_temp), str_file_folder);
	}
	//2014,1205
	for ( int i = 0; i < TSITE_SOCKET_CNT; i++)
	{
		mstr_temp.Format("SOCKET_%d", i+1);
		str_temp_onoff = "OFF";
		if( g_site.GetEnableSocket(i) > 0 /*g_site.GetEnableSocket(i)*/) str_temp_onoff = "ON";
		:: WritePrivateProfileString("Mode", LPCTSTR(mstr_temp), LPCTSTR(str_temp_onoff), str_file_folder);
	}


	//2013,1216
	if( st_basic.mn_mode_xgem == CTL_YES)
	{
		//2015.1001
		if(st_basic.mn_mode_rcmd == CTL_YES)
		{
			if( st_handler.m_nRemote == CTL_YES )
				st_handler.m_nRemote = CTL_NO;
		}
		g_client_xgem.SetPPID();
	}

	//2014,0623
	if(g_client_ec.GetToolInform() == false && bSkipTool == false )
	{
		g_client_ec.OnSendToolInform(true);
	}

}

////2015.0604
void CPublic_Function::Load_hifix_mode_Data()
{
	int nTotal = 0;
	FILE    *fp ;
	int     existence;
	char chr_data[100];
	
	CString str_LoadFile = "C:\\AMT8520\\Setting\\Hifix_mode.ini";
	
	existence = access(str_LoadFile, 0);
	
	if (!existence)
	{
		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			st_handler.m_nhifix_mode_cnt = 0;
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		st_handler.m_nhifix_mode_cnt = 0;
		return;
	}
	:: GetPrivateProfileString("Hifix_Mode", "Total", "0", chr_data, 100, str_LoadFile);
	nTotal = atoi(chr_data);
	
	st_handler.m_nhifix_mode_cnt = nTotal;
	
	CString str,strLabel;
	for (int i = 0; i < nTotal; i++)
	{
		str.Format("%d",i+1);
		:: GetPrivateProfileString("Hifix_Mode", str, "0", chr_data, 100, str_LoadFile);
		strLabel = chr_data;
		strLabel.TrimLeft(' ');               
		strLabel.TrimRight(' ');
		if(strLabel == "") st_handler.mstr_hifix_mode[i] = "MSATA";
		else                st_handler.mstr_hifix_mode[i] = strLabel;

	}
	fclose(fp);
}

void CPublic_Function::Load_scrap_code_Data()
{
	int nTotal = 0;
	FILE    *fp ;
	int     existence;
	char chr_data[100];
	
	CString str;
	CString str_LoadFile;
	CString str_Machine_no;
	CString str_ScrapCode;
	
	str_LoadFile = st_path.mstr_scrap_code;
	str_Machine_no = st_lamp.mstr_equip_id;
	
	existence = access(str_LoadFile, 0);
	
	if (!existence)
	{
		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			st_handler.m_nScrap_code_cnt = 0;
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		st_handler.m_nScrap_code_cnt = 0;
		return;
	}
	
	:: GetPrivateProfileString(str_Machine_no, "Total", "0", chr_data, 100, str_LoadFile);
	nTotal = atoi(chr_data);
	
	st_handler.m_nScrap_code_cnt = nTotal;
	
	
	
	for (int i = 0; i < nTotal; i++)
	{
		str.Format("%d",i+1);
		:: GetPrivateProfileString(str_Machine_no, str, "0", chr_data, 100, str_LoadFile);
		str_ScrapCode = chr_data;
		
		st_handler.mstr_scrap_code[i] = str_ScrapCode.Mid(0,4);//Scrap Code
		st_handler.mn_1Lot_cnt[i] = atoi(str_ScrapCode.Mid(5,1));//���� Lot
		st_handler.mn_MultiLot_cnt[i]= atoi(str_ScrapCode.Mid(7,1));//���� Lot
	}

	fclose(fp);//2015.0721
}
/////////////


//2015.0914
void CPublic_Function::DB_Time_Write(st_dbtime_database st_dbtime,int i)
{
	CString strTmp[15];


	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", 
		st_dbtime.mole_date.GetYear(), 
		st_dbtime.mole_date.GetMonth(),
		st_dbtime.mole_date.GetDay(),
		st_dbtime.mole_date.GetHour(),
		st_dbtime.mole_date.GetMinute(),
		st_dbtime.mole_date.GetSecond());
	
	strTmp[2].Format("slot_%d",i);
	strTmp[3].Format("%d", st_dbtime.mn_Ref_time[i]);
	strTmp[4].Format("%d",st_dbtime.mn_run_time[i]);
	strTmp[5].Format("%d", st_dbtime.mn_test_time[i]);
	strTmp[6].Format("%d", st_dbtime.mn_lt_Rundown[i]);	
	
	strTmp[7].Format("%d",st_dbtime.mn_lt_Socketoff[i]);
	strTmp[8].Format("%d", st_dbtime.mn_lt_instandby[i]);
	strTmp[9].Format("%d", st_dbtime.mn_lt_outReady[i]);	
	strTmp[10].Format("%d", st_dbtime.mn_lt_stop[i]);
	strTmp[11].Format("%d", st_dbtime.mn_lt_User_stop[i]);

	strTmp[12].Format("%d"   , st_dbtime.mn_Output[i]);
	strTmp[13].Format("%0.2f", st_dbtime.md_Yield[i]);
	strTmp[14].Format("%d", st_dbtime.mn_Pass[i]);
////
	
	
	g_cs.EnterCriticalSection();
	
	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "FILLRATIO_DATA"))
			{
				Mysql.Record_Add("FILLRATIO_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "FILLRATIO_DATA",15, Mysql.mstr_dbtime_table_name, Mysql.mn_dbtime_table_type, Mysql.mn_dbtime_table_num))
				{
					Mysql.Record_Add("FILLRATIO_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "FILLRATIO_DATA",15, Mysql.mstr_dbtime_table_name, Mysql.mn_dbtime_table_type, Mysql.mn_dbtime_table_num))
				{
					Mysql.Record_Add("FILLRATIO_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();
	
	g_cs.LeaveCriticalSection();
}
//////////////////////////////////////////////
//2015.1005
void CPublic_Function::DB_perTime_Data()
{

}

void CPublic_Function::DB_perTime_Write(st_dbtime_database st_dbtime,int i)
{
	CString strTmp[15];
	strTmp[1].Format("%04d/%02d/%02d %02d:%02d:%02d", 
		st_dbtime.mole_date.GetYear(), 
		st_dbtime.mole_date.GetMonth(),
		st_dbtime.mole_date.GetDay(),
		st_dbtime.mole_date.GetHour(),
		st_dbtime.mole_date.GetMinute(),
		st_dbtime.mole_date.GetSecond());
	
	strTmp[2].Format("slot_%d",i);
	strTmp[3].Format("%d", st_dbtime.mn_Ref_time[i]);
	strTmp[4].Format("%d",st_dbtime.mn_run_time[i]);
	strTmp[5].Format("%d", st_dbtime.mn_test_time[i]);
	strTmp[6].Format("%d", st_dbtime.mn_lt_Rundown[i]);	
	
	strTmp[7].Format("%d",st_dbtime.mn_lt_Socketoff[i]);
	strTmp[8].Format("%d", st_dbtime.mn_lt_instandby[i]);
	strTmp[9].Format("%d", st_dbtime.mn_lt_outReady[i]);	
	strTmp[10].Format("%d", st_dbtime.mn_lt_stop[i]);
	strTmp[11].Format("%d", st_dbtime.mn_lt_User_stop[i]);
	
	strTmp[12].Format("%d"   , st_dbtime.mn_Output[i]);
	strTmp[13].Format("%0.2f", st_dbtime.md_Yield[i]);
	strTmp[14].Format("%d", st_dbtime.mn_Pass[i]);
	////

	g_cs.EnterCriticalSection();
	
	if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
	{
		if (Mysql.Database_Select("AMT8520"))
		{	
			if (Mysql.Table_Select("AMT8520", "PER_HOUR_DATA"))
			{
				Mysql.Record_Add("PER_HOUR_DATA", strTmp);
			}
			else
			{
				if (Mysql.Table_Create("AMT8520", "PER_HOUR_DATA",15, Mysql.mstr_dbtime_table_name, Mysql.mn_dbtime_table_type, Mysql.mn_dbtime_table_num))
				{
					Mysql.Record_Add("PER_HOUR_DATA", strTmp);
				}
			}
		}
		else
		{
			if (Mysql.Database_Create("AMT8520"))
			{
				if (Mysql.Table_Create("AMT8520", "PER_HOUR_DATA",15, Mysql.mstr_dbtime_table_name, Mysql.mn_dbtime_table_type, Mysql.mn_dbtime_table_num))
				{
					Mysql.Record_Add("PER_HOUR_DATA", strTmp);
				}
			}
		}
	}
	
	Mysql.MySql_Close();
	
	g_cs.LeaveCriticalSection();
}




int CPublic_Function::SemeaWork()
{

	int nFuncRet = CTLBD_RET_GOOD;

	int iBin1_1 = g_Tray.GetIdxByPos( TPOS_BIN1 ); 
	int iBin1_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN1 );
	int iBin2_1 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin2_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN2 );
	int iBin3_1 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int iBin3_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN3 );

	//2013,1027
	int iBin4_1 = g_Tray.GetIdxByPos( TPOS_REJECT_TO_BIN );
	int iBin4_2 = g_Tray.GetIdxByTargetPos( TPOS_REJECT_TO_BIN );
	int num = 0;


	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		int ix=0, iy=0;
		int x = st_basic.n_tray_x;
		int y = st_basic.n_tray_y;

		if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
		{
			x = st_basic.n_tray_y;
			y = st_basic.n_tray_x;
		}

		for( int it=0; it<MAX_TRAY; it++ )
		{
			ATray tray = g_Tray.GetTray( it );
			if( tray.GetTargetPos() == TPOS_NEXT )
			{

				int nCnt = 0;
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >=0 && tray.GetModule(nTarget).IsTestLoadState() )
							{
								nCnt++;
								break;
							}
						}

					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >=0 && tray.GetModule(nTarget).IsTestLoadState() )
							{
								nCnt++;
								break;
							}
						}
					}
				}

				if( nCnt > 0)
					num++;
			}
		}
	}


	int iNBin1_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ); 
	int iNBin1_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 );
	int iNBin2_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN2 );
	int iNBin2_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 );
	int iNBin3_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN3 );
	int iNBin3_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 );

	//2013,1027
	int iNBin4_1 = g_Tray.GetIdxByPos( TPOS_REJECT_TO_BIN );
	int iNBin4_2 = g_Tray.GetIdxByTargetPos( TPOS_REJECT_TO_BIN ); 

	if(iNBin1_1 > -1 || iNBin1_2 > -1) num += 1;
	if(iNBin2_1 > -1 || iNBin2_2 > -1) num += 1;
	if(iNBin3_1 > -1 || iNBin3_2 > -1) num += 1;

	//2013,1027
	if(iNBin4_1 > -1 || iNBin4_2 > -1) num += 1; 


	
	if(g_lotMgr.GetLotCount() > 1 || g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
	{
		 if(st_handler.mn_mirror_type == CTL_YES)
		 {
			 if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
			 {				
				 if(num >= 2)
				 {
					 nFuncRet = CTLBD_RET_ERROR;
				 }
			 }
		 }
		 else
		 {
			 if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
			 {				
				 if(num >= 2)
				 {
					 nFuncRet = CTLBD_RET_ERROR;
				 }
			 }
		}
	}

	return nFuncRet;
}

int CPublic_Function::OnScrapCodeFind(int nMode, CString strLoadFile)
{
	TCHAR chData[100];

	int nCount;
	int nCode;
	int nCurr, nNext;
	int i;

	CString strTemp;
	CString strHead;
	CString strData;

	GetPrivateProfileString(st_basic.m_strScrapHead, _T("Total"), _T("0"), chData, sizeof(chData), strLoadFile);
	strTemp.Format(_T("%s"), chData);
	nCount = atoi(strTemp);

	if (nCount > 0)
	{
		for (i=0; i<nCount; i++)
		{
			strHead.Format(_T("%d"), i+1);
			GetPrivateProfileString(st_basic.m_strScrapHead, strHead, _T("0"), chData, sizeof(chData), strLoadFile);
			strData.Format(_T("%s"), chData);
			
			nCode = atoi(strData.Mid(0, 4));
			nCurr = atoi(strData.Mid(5, 1));
			nNext = atoi(strData.Mid(7, 1));

//			st_code_info[nMode].m_nScrapCode[nCode][0] = nCurr;
//			st_code_info[nMode].m_nScrapCode[nCode][1] = nNext;
		}
	}
	else
	{
		return RET_ERROR;
	}

	return RET_GOOD;
}

//2016.0129
CString	CPublic_Function::OnRecipeCheck(CString strPartNo)
{
	CString strRecipe = _T("");						// �Լ� ���� �÷���
	CString strTemp;
	
	HANDLE hFind;
    WIN32_FIND_DATA fd;
	
	CString strFileName = st_path.mstr_path_dvc ;
	
	if (strFileName.Right (1) != "\\")
	{
        strFileName += "\\";
	}
	
	strFileName += "*.TXT";
	

// 	strFileName += st_basic.mstr_device_name;
    
    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileName, &fd)) != INVALID_HANDLE_VALUE) 
	{
//      while (::FindNextFile (hFind, &fd)) 
// 		{
//          if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
// 			{
// 				strTemp = (fd.cFileName);
// 				if (OnPartNoMatch(strTemp, strPartNo) == RET_GOOD)
// 				{
// 					strRecipe = strTemp;
// 					break;
// 				}
// 			}
//      }
		do 
		{
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strTemp = (fd.cFileName);
				if (OnPartNoMatch(strTemp, strPartNo) == RET_GOOD)
				{
					strRecipe = strTemp;
					break;
				}
			}
		} while (::FindNextFile (hFind, &fd));
		::FindClose(hFind);
    }
	
	return strRecipe;
}

int CPublic_Function::OnPartNoMatch(CString strFileName, CString strPartNo)
{
	TCHAR chData[300];

	int nPos;
	int nLength;
	int i, j;
	int nCount;

	CString strTemp;
	CString strRecipe;
	CString strData;
	CString strFind[2];

	BOOL bFind;

// 	nPos = st_basic.mstr_device_name.Find(_T("."), 0);
// 	strRecipe = st_basic.mstr_device_name.Mid(0, nPos);
	nPos = strFileName.Find(_T("."), 0);
	strRecipe = strFileName.Mid(0, nPos);
	strTemp.Format(_T("%s_TOTAL"), strRecipe);

	st_basic.m_strScrapHead = st_lamp.mstr_equip_id;

	GetPrivateProfileString(st_basic.m_strScrapHead, strTemp, _T("0"), chData, sizeof(chData), _T("C:\\AMT8520\\Setting\\RECIPE_RULE.TXT"));
	strTemp.Format(_T("%s"), chData);
	nCount = atoi(strTemp);

	if (nCount == 0) return RET_ERROR;

	for (i=0; i<nCount; i++)
	{
		strTemp.Format(_T("%s_%02d"), strRecipe, i+1);
		GetPrivateProfileString(st_basic.m_strScrapHead, strTemp, _T("0"), chData, sizeof(chData), _T("C:\\AMT8520\\Setting\\RECIPE_RULE.TXT"));
		strData.Format(_T("%s"), chData);
		
		bFind = TRUE;
		nLength = strData.GetLength();

		for(j=0; j<nLength; j++)
		{
			strFind[0] = strPartNo.GetAt(j);
			strFind[1] = strData.GetAt(j);

			if (strFind[1] != _T("_"))
			{
				if (strFind[0] != strFind[1])
				{
					bFind = FALSE;
					break;
				}
			}
		}
		if (bFind) return RET_GOOD;
	}

	return RET_ERROR;
}

//2016.0709
void CPublic_Function::LoadSocketSkipRule()
{
	Func.Load_hifix_mode_Data();
	char chr_file_name[256];
	sprintf(chr_file_name, "%s", st_path.mstr_socket_skip);
	int i = 0, j = 0, k = 0;
	int n_existence = access(chr_file_name, 0) ;
	

	FILE  *fp ;	
	
	int existence = access(st_path.mstr_socket_skip, 0);
	
	if (!existence)
	{
		if ((fp=fopen(st_path.mstr_socket_skip,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", st_path.mstr_socket_skip);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", st_path.mstr_socket_skip);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return;
	}
	
	CString strSocket, strNum, str_temp;
	char chr_data[100];
	int nTotal = 0, nUse = 0;
	for(i = 0; i <= 5; i++)//Hifix num
	{
		for( j=0; j< st_handler.m_nhifix_mode_cnt; j++ ) //Hifix mode
		{			
			strSocket.Format("%d_%s", i+1, st_handler.mstr_hifix_mode[j]);
			:: GetPrivateProfileString(strSocket, "Total", "20", chr_data, 10, st_path.mstr_socket_skip );
			nTotal = atoi(chr_data);
			if ( nTotal != 20 )
			{
				str_temp = "20";
				:: WritePrivateProfileString(strSocket, "Total", LPCTSTR(str_temp), st_path.mstr_socket_skip);
				for(k = 0; k < 20; k++)
				{
					strNum.Format("%d", k+1);
					str_temp = "1";
					:: WritePrivateProfileString(strSocket, strNum, LPCTSTR(str_temp), st_path.mstr_socket_skip);
					st_handler.m_bEnable[i][k] = 1;
				}				
			}
			else
			{
				for(k = 0; k < TSITE_SOCKET_CNT; k++)
				{
					if( st_handler.mstr_hifix_mode[j] == st_basic.n_str_hifix_mode)
					{
						strNum.Format("%d", k+1);
						:: GetPrivateProfileString(strSocket, strNum, "0", chr_data, 10, st_path.mstr_socket_skip);
						nUse = atoi(chr_data);
						if( nUse > 0)
						{
							st_handler.m_bEnable[i][k] = 1;
						}
						else if( nUse < 0)
						{
							st_handler.m_bEnable[i][k] = nUse;
						}
						else
						{
							st_handler.m_bEnable[i][k] = 0;
						}
						st_handler.m_bBackupEnable[i][k] = st_handler.m_bBackupEnable2[i][k] = st_handler.m_bEnable[i][k];
						if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
						if( st_basic.mn_hotfix_pos -1 == i && st_handler.mstr_hifix_mode[j] == st_basic.n_str_hifix_mode)
							g_site.SetEnableSocket(k, st_handler.m_bEnable[i][k]);
					}

				}
			}
		}
	}

	fclose(fp);
// 	SaveSocketSkipRule();

}

//2016.0709
void CPublic_Function::SaveSocketSkipRule()
{
	char chr_file_name[256];
	sprintf(chr_file_name, "%s", st_path.mstr_socket_skip);
	int i = 0, j = 0, k = 0;
	int n_existence = access(chr_file_name, 0) ;
	

	
	CString strSocket, strNum, str_temp;
	int nTotal = 0, nUse = 0, num1 = 0;

	if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;

	strSocket.Format("%d_%s", st_basic.mn_hotfix_pos -1 + 1, st_basic.n_str_hifix_mode); 
	str_temp.Format("20");
	:: WritePrivateProfileString(strSocket, "Total", LPCTSTR(str_temp), st_path.mstr_socket_skip);
	for(i = 0; i < TSITE_SOCKET_CNT; i++)
	{
		strNum.Format("%d", k+1);
		num1 = g_site.GetTestCount( i );		
		st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i] = g_site.GetEnableSocket( i );

// 		if	   ( num1 > st_basic.mn_socket_max_count )
// 		{
// 			if( st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i] != -1 )
// 			{
// 				st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i] = -1;
// 				g_site.SetEnableSocket( i, -1 );
// 			}
// 		}
// 		g_site.SetEnableSocket(i, st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i]);

		strNum.Format("%d", i+1);
		str_temp.Format( "%d", st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i] );
		:: WritePrivateProfileString(strSocket, strNum, LPCTSTR(str_temp), st_path.mstr_socket_skip);
// 		st_handler.m_bEnable[st_basic.mn_hotfix_pos-1][i] = 1;
	}
}

