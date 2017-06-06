// MyJamData.cpp: implementation of the CMyJamData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Handler.h"
#include "MyJamData.h"

#include "io.h" 
#include "Public_Function.h"  // ���� ���� �� I/O ���� ���� Ŭ����
#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "SrcPart/APartHandler.h"
#include "AMTLotManager.h"
#include "AClient_Xgem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyJamData			MyJamData;
CMyJamData::CMyJamData()
{
	mn_noError = FALSE;
}

CMyJamData::~CMyJamData()
{

}

/* ****************************************************************************** */
/* �߻� �˶� ���� ���� �Լ�                                                       */
/* -> strPath     : �˶� ���� ���� ���� ���� ����                                 */
/* -> strDataFile : �˶� ���� ����� File Name(File Name+Ȯ����)                        */
/* -> strCode     : �߻��� �˶� Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Display_Info_Load(CString strPath, CString strDataFile, CString strActionFile, CString strCode)
{
	CString str_file_path;  // ���� ���� ���� */
	CString str_alarm_chk;  // �Լ� ���� ���� ���� ���� */
	int n_pos;

	str_file_path = strPath;
	if (str_file_path.Right(1) != "\\")  // ���� ��� ������ ���� �˻� */
	{
		str_file_path += "\\";
	}

	str_alarm_chk = On_Alarm_Bmp_Data_Load(str_file_path, strDataFile, strCode);  // �߻� �˶� ���� ǥ�ÿ� �׸� File Name ���� �Լ� */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // ���� �߻�
	{
		return str_alarm_chk;
	}

	str_alarm_chk = On_Alarm_Style_Data_Load(str_file_path, strDataFile, strCode);  // �˶� �߻� ���� ǥ�� ���� �� ��� ��ġ ���� �Լ� */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // ���� �߻�
	{
		return str_alarm_chk;
	}

	str_alarm_chk = On_Alarm_Repair_Data_Load(str_file_path, strActionFile, strCode);  // �߻��� �˶��� ���� ��ġ ���� ���� �Լ� */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // ���� �߻�
	{
		return str_alarm_chk;
	}

	return str_alarm_chk;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �߻� �˶� ���� ǥ�ÿ� �׸� File Name ���� �Լ�                                 */
/* -> strPath     : �˶� ǥ�� �׸� ���� ���� ����                                 */
/* -> strDataFile : �˶� ǥ�� �׸� File Name(File Name+Ȯ����)                    */
/* -> strCode     : �߻��� �˶� Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Bmp_Data_Load(CString strPath, CString strDataFile, CString strCode)
{
	CString str_return_data;  // �Լ� ���� �÷���
	CString str_file;  // �˶� ǥ�ÿ� �׸��� ���� [����]+[File Name] ���� ����
	CString str_temp;  // �˶� ǥ�ÿ� �׸� File Name �ӽ� ���� ����

	char chr_buf[1000];

	str_return_data = _T("OK");  // �Լ� ���� �÷��� �ʱ�ȭ

	str_file = strPath + strDataFile;  // [����]+[File Name] ����

	if (!GetPrivateProfileString(strCode, "DEF", "", chr_buf, 1000, str_file))
	{
		str_return_data = "File Name : "+ str_file;
		str_return_data += "\r\n";
		str_return_data += "[DEF] The information we read. Trouble -> Code : ";
		str_return_data += strCode;

		return str_return_data;
	}

	str_temp.Format("%s", chr_buf);
	st_alarm.mstr_bmp_file = str_temp;  // �˶� ǥ�ÿ� �׷� File Name ����

	return str_return_data;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �˶� �߻� ���� ǥ�� ���� �� ��� ��ġ ���� �Լ�                                */
/* -> strPath     : ���� ǥ�� ���� ���� ���� ����                                 */
/* -> strDataFile : ���� ǥ�� ���� ���� File Name(File Name+Ȯ����)               */
/* -> strCode     : �߻��� �˶� Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Style_Data_Load(CString strPath, CString strDataFile, CString strCode)
{
	CString str_chk_data;		// �Լ� ���� �÷���
	CString str_chk_file;		// �˶� ���� ǥ�� ������ ���� [����]+[File Name] ���� ����
	CString str_chk, str_chk2;  // �˶� ���� ǥ�� ���� �� ��ġ ���� ����
	int start=0, end, i;		// ���� ������ ��ġ ���� ����
	int max=4;					// ���� ������ �ִ� ���� (ȭ��ǥ ������� �ʱ�ȭ)

	char chr_buf[1000];
	char num[20];

	str_chk_data = _T("OK");  // �Լ� ���� �÷��� �ʱ�ȭ

	str_chk_file = strPath + strDataFile;  // [����]+[File Name] ����

	if (!GetPrivateProfileString(strCode, "STYLE", "", chr_buf, 1000, str_chk_file))
	{
		str_chk_data = "File Name : "+ str_chk_file;
		str_chk_data += "\r\n";
		str_chk_data += "[STYLE] The information we read. Trouble -> Code : ";
		str_chk_data += strCode;

		return str_chk_data;
	}

	str_chk.Format("%s", chr_buf);

	for(i=0; i<max; i++)
	{
		end = str_chk.Find(';', start);
		if (end == -1)  // ������ ������
		{
			break;
		}
		else            // ������ ����
		{
			str_chk2 = str_chk.Mid(start, end-start);  // ������ ������ �� �� ������ ����
			str_chk2.TrimLeft(' ');
			str_chk2.TrimRight(' ');

			if (i == 0)  // �˶� ���� ǥ�� ����
			{
				st_alarm.mstr_style = str_chk2;       // �˶� ���� ǥ�� ���� ����
				if (st_alarm.mstr_style == "CIRCLE")  // ���� ǥ�� ���� 8�� ����
				{
					max = 8;    
				}
				else if (st_alarm.mstr_style == "NONE")
				{
					max = 1;
				}
			}
			else
			{
				lstrcpy(num, str_chk2);
				if (i == 1)
				{
					st_alarm.mn_Cx = atoi(num);         // X�� �ȼ� ��ġ
				}
				else if (i == 2)
				{
					st_alarm.mn_Cy = atoi(num);         // Y�� �ȼ� ��ġ
				}
				else if (i == 3)
				{
					st_alarm.mn_Offset = atoi(num);     // �� ũ��
				}
				else if (i == 4)
				{
					st_alarm.mn_LineWidth = atoi(num);  // ���� �β�
				}
				else if (i == 5)
				{
					st_alarm.mn_R = atoi(num);          // RGB �� R ��
				}
				else if (i == 6)
				{
					st_alarm.mn_G = atoi(num);          // RGB �� G ��
				}
				else if (i == 7) 
				{
					st_alarm.mn_B = atoi(num);          // RGB �� B ��
				}
				/* ************************************************************** */
			}

			start = end + 1;  // ���� ������ ��ġ ����
		}
	}

	if (i != max)  // Data Format ����
	{
		str_chk_data = "File Name : "+ str_chk_file;
		str_chk_data += "\r\n";
		str_chk_data += "[STYLE] Data Format. Trouble -> Code : ";
		str_chk_data += strCode;

		return str_chk_data;
	}

	return str_chk_data;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �߻��� �˶��� ���� ��ġ ���� ���� �Լ�                                         */
/* -> strPath       : �˶� ��ġ ���� ����� ���� ����                             */
/* -> strActionFile : �˶� ��ġ ���� ����� File Name(File Name+Ȯ����)           */
/* -> strCode       : �߻��� �˶� Code                                            */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Repair_Data_Load(CString strPath, CString strActionFile, CString strCode)
{
	CString str_return_val;		// �Լ� ���� �÷���
	CString str_repair_file;	// ��ġ ���׿� ���� [����]+[File Name] ���� ����
	CString str_act_item;		// ��ġ ���� ���� ��ġ ���� ����
	CString str_act_temp;		// ��ġ ���� �ӽ� ���� ����
	int n_chk_cnt;				// ��ġ ���� ���� ���� ����
	int i; 
	
	char chr_buf[1000];

	str_return_val = _T("OK");  // �Լ� ���� �÷��� �ʱ�ȭ

	str_repair_file = strPath + strActionFile;  // [����]+[File Name] ����

	if (!GetPrivateProfileString(strCode, "COUNT", "0", chr_buf, 10, str_repair_file))
	{
		str_return_val = "File Name : "+ str_repair_file;
		str_return_val += "\r\n";
		str_return_val += "[COUNT] The information we read. Trouble -> Code : ";
		str_return_val += strCode;

		return str_return_val;
	}

	n_chk_cnt = atoi(chr_buf);  // ���� ��ġ ���� ���� ����
	if (n_chk_cnt<1)
	{
		str_return_val = "File Name : "+ str_repair_file;
		str_return_val += "\r\n";
		str_return_val += "[COUNT] Data Format Trouble -> Code : ";
		str_return_val += strCode;

		return str_return_val;
	}
	else
	{
		st_alarm.mn_action_cnt = n_chk_cnt;
	}

	(st_alarm.mstr_repair_msg).Empty();  //  ��ġ ���� ���� ���� �ʱ�ȭ

	for (i=0; i<(st_alarm.mn_action_cnt); i++)   
	{
		str_act_item.Format("%d", i+1);  // ��ġ ���� ���� ���� ��ġ ����
		if (!GetPrivateProfileString(strCode, (LPCTSTR)str_act_item, "", chr_buf, 1000, str_repair_file))
		{
			str_return_val = "File Name : "+ str_repair_file;
			str_return_val += "\r\n";
			str_return_val += "An action subject. The information we read. Trouble";

			str_return_val += "\r\n";
			str_return_val += "Code : ";
			str_return_val += strCode;

			str_return_val += "\r\n";
			str_return_val += "Position : ";
			str_return_val += str_act_item;

			return str_return_val;
		}
		/* ********************************************************************** */

		st_alarm.mstr_repair_msg += "\r\n";			//��ġ ���׿� �� ù���� ����.

		str_act_temp.Format(" %s", chr_buf);  // ��ġ ���� ����
		st_alarm.mstr_repair_msg += str_act_temp;

		if (i != ((st_alarm.mn_action_cnt)-1))  
			st_alarm.mstr_repair_msg += "\r\n";  // ��ġ ���� �ܶ� ������ ����
	}
	/* ************************************************************************** */
	
	return str_return_val;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �߻��� �˶� ȭ�� ��� �Լ�                                                     */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Display()
{
	CString str_cnt, str_temp;
	char temp_code[10]={0,}, temp_cnt[10]={0,}, chr_buf[20]={0,};
	
	CString str_display_time;	// ���� ��¿� �ð� ���� ����
	CString str_alarm_code;		// �߻��� �˶� �ڵ� ���� ����
	CString str_alarm_msg;		// �߻��� �˶� �޽��� ���� ����
	int n_cur_hour=0;				// ���� �ð� ���� ����
	int n_chk=0;					// ������ �˶� �ڵ� ���� ����
	long n_chk_time=0;			// �˶� �߻� �ð� ���� ���� ����
	int n_data_save = 0;		// Jam�� ������ ���̳�?

	COleDateTime cur_time;
	CTime alarm_occured_time;

 	/* ************************************************************************** */
 	/* ��� ���� �� �˶� �߻��ߴ��� �˻��Ѵ�                                      */
 	/* -> �˶� �߻��� ��� �α� ���� �� �ӽ� ���Ͽ� �����ϰ� ȭ�鿡 ����Ѵ�      */
 	/* ************************************************************************** */
 	if (COMI.mn_run_status == CTL_dJAM || COMI.mn_run_status == CTL_dWARNING || COMI.mn_run_status == CTL_dLOTEND)  // �˶� �߻�
 	{	
		//alarm.mstr_code = "990001" ;


 		if (st_handler.mn_alarm_screen == TRUE)	return; 
 		/* ********************************************************************** */
 		/* �˶� ȭ�� ��� �Ұ����� ��� �˻��Ѵ�.                                 */
 		/* -> st_handler.mn_system_lock : SYSTEM LOCK ���� �÷���                  */
 		/* ********************************************************************** */
 		if (st_handler.mn_system_lock != FALSE)  // SYSTEM LOCK
 		{
 			return;  // ���� ����
 		}
 		/* ********************************************************************** */
 

 		/* ************************************************************************** */
 		/* �˶� �߻��� �ð� �����Ѵ�                                                  */
 		/* ************************************************************************** */
 		cur_time = COleDateTime::GetCurrentTime();
 		n_cur_hour = cur_time.GetHour();
 
 		alarm_occured_time = CTime::GetCurrentTime();
 		str_display_time = alarm_occured_time.Format("%c");
 		/* ************************************************************************** */

 
 		/* ********************************************************************** */
 		/* �˶� ȭ�� ��� ���� �� �˶� ī��Ʈ ���� �˻��Ѵ�                       */
 		/* -> �̹� �˶� ȭ�� ��� ���� ��� �� ����� �ʿ� ������                 */
 		/*    �� ��쿡 �߻��ϴ� �˶��� �����Ѵ�                                  */
 		/*  : �˶� ȭ�� ��� �߿� �˶� Code ����Ǹ� ��� �޽��� �޶����� ����    */
 		/* -> st_handler.mn_alarm_screen : �˶� ȭ�� ��� ���� �÷���              */
 		/* -> st_alarm.mn_cur_state : �˶� ���� ���� �÷���                        */
 		/* ********************************************************************** */
 		if (st_handler.mn_alarm_screen != TRUE)  // �˶� ȭ�� �����
 		{
 			if (COMI.mn_run_status == CTL_dLOTEND)		
 			{
 				alarm.mn_reset_status = CTL_NO;
 			}
 			else	alarm.mn_reset_status = CTL_YES;
 
 			//�߻��� �˶��� List�� �ִ´�.
 			if (alarm.mstr_code != "")
 			{
 				/* ************************************************************************** */
 				/* ���� �߻��� �˶��� ���� �޽��� ���� �����Ѵ�                               */
 				/* ************************************************************************** */
 				On_Alarm_Info_Set_to_Variable(alarm.mstr_code);  // �߻��� �˶� �޽��� ���� ������ �����ϴ� �Լ�
 				/* ************************************************************************** */
 				sprintf(temp_code, "%s", alarm.mstr_code);
// 				str_cnt.Format("%d", st_alarm.mn_cur_state);
 				sprintf(temp_cnt, "%s", str_cnt);
 
 				/* ************************************************************************** */
 				/* �ٷ� ���� �߻��� �˶� �ڵ�� ���Ͽ� ������ ��� �˶� ī��Ʈ ������       */
 				/* -> �˶� ī��Ʈ �ð� ������ 30�ʷ� �����Ѵ�                                 */
 				/* ************************************************************************** */
 				alarm.stl_cur_alarm_time = GetCurrentTime();
 
 				n_chk = atoi(alarm.mstr_code);
				n_data_save = CTL_NO;				// �⺻�� NO

				//st_basic.mn_alarm_delay_time = 10;
 				
 				if (alarm.stl_prev_code == n_chk)	// �˶� �ڵ� ����
 				{
// 					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_prev_alarm_time; 

					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_alarm_start_time; 
 					
 					if (n_chk_time < st_basic.mn_alarm_delay_time * 1000 /*|| alarm.stl_alarm_start_time == 0*/)						// Basic���� �����Ҽ� �ְ� ������. 2K9/11/19/ViboX
 					{
 						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
 					}
 					else
 					{
 						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
 						n_data_save = CTL_YES;			// Jam�� ���� �� Count�� �Ѵ�.
 					}
 				}
 				else
 				{
 					/* ********************************************************************** */
 					/* �߻��� �˶� ������ ���� ������ ��ü�Ѵ�                                */
 					/* -> ������ �߻��ϴ� �˶��� ���ϴ� ������ ���ȴ�                     */
 					/* ********************************************************************** */
//  					alarm.stl_prev_code = n_chk;
//  					alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
//  					/* ********************************************************************** */
//  
//  					n_data_save = CTL_YES;				// Jam�� ���� �� Count�� �Ѵ�.
					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_alarm_start_time; 
					if (n_chk_time < 120 * 1000/*2��*/)
					{
						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
						mn_noError = TRUE;
						On_Occured_Alarm_Saving_to_File();  // �߻��� �˶� �α� ���Ͽ� �����ϴ� �Լ�
					}
					else
					{
						alarm.stl_prev_code = n_chk;
						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
						n_data_save = CTL_YES;			// Jam�� ���� �� Count�� �Ѵ�.
					}
				}
 				/* ************************************************************************** */
 			}

			if (st_handler.mn_status == CTL_NO || st_basic.mn_mode_run == CTL_NO || st_handler.mn_virtual_supply == 1 || (g_lotMgr.GetLotIDAt(0).GetLength() < 5))
			{
				n_data_save = CTL_NO;
			}

 			if (n_data_save == CTL_YES)
 			{
				if (st_alarm.mstr_cur_state == "1")			// Count�ϴ� Alarm�̸�... 2K9/08/28/ViboX
				{
					alarm.mn_alarm_occured++;
					alarm.mn_alarm_occured_day++;

					str_temp = LPCTSTR(_itoa(alarm.mn_alarm_occured_day, chr_buf, 10)) ;
					:: WritePrivateProfileString("BasicData", "Day_Alarm_Count", LPCTSTR(str_temp), st_path.mstr_basic);
				}

				if (st_handler.mn_status == CTL_YES)					// ��� �������̳�
				{
					g_handler.AddAlarmCnt();
					On_Occured_Alarm_Saving_to_File();  // �߻��� �˶� �α� ���Ͽ� �����ϴ� �Լ�
				}
 			}
 
 			st_handler.mn_alarm_screen = TRUE;	// �˶� ȭ�� ��� �÷��� ����
 
 			Func.OnSet_IO_Port_Alarm();			// �˶� ���� I/O ��� �Լ�

 			if( st_basic.mn_mode_xgem == CTL_YES )
			{
				g_client_xgem.SetGemAlarmState(alarm.mstr_code, 1);
			}
			
			::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 7, 1);  // ���� �����ӿ� �˶� ȭ�� ��� ��û 
 
 			if (st_handler.cwnd_title != NULL)  // Ÿ��Ʋ �� ��� �˻�
 				st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);  // Ÿ��Ʋ [�˶�] ��� ��û

 		}
// 		/* ********************************************************************** */
 	}
	/* ************************************************************************** */
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �߻��� �˶� �α� ���Ͽ� �����ϴ� �Լ�                                          */
/* ****************************************************************************** */
void CMyJamData::On_Occured_Alarm_Saving_to_File()
{
	CString str_display_time;	// ���� ��¿� �ð� ���� ����
	CString str_alarm_code;		// �߻��� �˶� Code ���� ����
	CString str_alarm_msg;		// �߻��� �˶� �޽��� ���� ����
	int n_cur_hour;				// ���� �ð� ���� ����
	int n_chk;					// ������ �˶� Code ���� ����
	long n_chk_time;			// �˶� �߻� �ð� ���� ���� ����

	COleDateTime cur_time;
	CTime alarm_occured_time;

	/* ************************************************************************** */
	/* �˶� �߻��� �ð� �����Ѵ�                                                  */
	/* -> SHIFT ���� �˻� �� �˶� �߻� �ð� �����ϱ� ���Ͽ� �ʿ�                  */
	/* ************************************************************************** */
	cur_time = COleDateTime::GetCurrentTime();
	n_cur_hour = cur_time.GetHour();

	alarm_occured_time = CTime::GetCurrentTime();
	str_display_time = alarm_occured_time.Format("%Y/%m/%d  %H:%M:%S");
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* �˶� Code �� �˶� �޽��� �Լ� ������ �����Ѵ�                              */
	/* -> ����� �� �Լ� ȣ�� ���� ���ο� �˶��� �߻��ϴ� ��� �˶� Code��        */
	/*    ��ġ ���� �� ��Ÿ ������ ��ġ���� ���� �� �ִ�                          */
	/* -> alarm.mstr_code : ���� �߻��� �˶� Code ���� ���� ����                   */
	/* -> st_alarm.mstr_cur_msg : �߻��� �˶� �޽��� ���� ����                     */
	/* ************************************************************************** */
	str_alarm_code = alarm.mstr_code;
	str_alarm_msg = st_alarm.mstr_cur_msg;

	st_alarmdb.mole_date = COleDateTime::GetCurrentTime();

// 	st_alarmdb.mstrlot_no = st_pcb[1].strLotNumber;
// 	st_alarmdb.mstrpart_no = st_pcb[1].strPartNumber;

	st_alarmdb.mstrAlarmCode = str_alarm_code;
	st_alarmdb.mstrAlarmMsg = str_alarm_msg;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* �Ϻ�, ���� �˶� �߻� Ƚ�� �ӽ� ���Ͽ� �߰��Ѵ�                             */
	/* ************************************************************************** */
	//2012,0306
	if(mn_noError == TRUE)
	{
		mn_noError = FALSE;
		On_UnderDaily_ListFile_Add(str_alarm_code, str_display_time, str_alarm_msg); // �Ϻ�, ���� �˶� ���� ����Ʈ ���Ͽ� �߰� �Լ� */
		return;
	}

	if (st_alarm.mstr_cur_state == "1")			// Count�ϴ� Alarm�̸�... 2K9/08/28/ViboX
	{
		On_Daily_ListFile_Add(str_alarm_code, str_display_time, str_alarm_msg); // �Ϻ�, ���� �˶� ���� ����Ʈ ���Ͽ� �߰� �Լ� */
		//Func.DB_Write_Alarm(st_alarmdb);
	}
	/* ************************************************************************** */
	
	/* ************************************************************************** */
	/* �ٷ� ���� �߻��� �˶� Code�� ���Ͽ� ������ ��� �˶� ī��Ʈ ������       */
	/* -> �˶� ī��Ʈ �ð� ������ 30�ʷ� �����Ѵ�                                 */
	/* ************************************************************************** */
	alarm.stl_cur_alarm_time = GetCurrentTime();

	n_chk = atoi(str_alarm_code);
	
	if (alarm.stl_prev_code == n_chk)  // �˶� Code ����
	{
		n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_prev_alarm_time; 
		
		if (n_chk_time < 30000) 
		{
			alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
			return;
		}
	}
	else
	{
		/* ********************************************************************** */
		/* �߻��� �˶� ������ ���� ������ ��ü�Ѵ�                                */
		/* -> ������ �߻��ϴ� �˶��� ���ϴ� ������ ���ȴ�                     */
		/* ********************************************************************** */
		alarm.stl_prev_code = n_chk;
		alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
		/* ********************************************************************** */
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* SHIFT�� �˶� �߻� Ƚ�� �ӽ� ���� ���Ͽ� �˶� �߰��Ѵ�                      */
	/* -> �˶� �߻� Ƚ�� �ӽ� ���� ���� ������ �� ������ �˶� ����Ʈ ���� �̸���  */
	/*    �Ϻ� ���� �̸����� ����� ������ ������ �����ϴ�                        */
	/* ************************************************************************** */
	if (n_cur_hour >= 6 && n_cur_hour < 14)  // SHIFT �� DAY */
	{
//		mcls_alarm_list.On_Shift_File_Add(1, str_alarm_code, str_display_time);  // SHIFT�� �˶� �߻� Ƚ�� ���� �ӽ� ���Ͽ� �߰� �Լ� */
	}
	else if (n_cur_hour >= 14 && n_cur_hour < 22)  // SHIFT �� SWING */
	{
//		mcls_alarm_list.On_Shift_File_Add(2, str_alarm_code, str_display_time);  // SHIFT�� �˶� �߻� Ƚ�� ���� �ӽ� ���Ͽ� �߰� �Լ� */
	}
	else  // SHIFT �� GY */
	{
//		mcls_alarm_list.On_Shift_File_Add(3, str_alarm_code, str_display_time);  // SHIFT�� �˶� �߻� Ƚ�� ���� �ӽ� ���Ͽ� �߰� �Լ� */
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* LOT�� �˶� �߻� Ƚ�� �ӽ� ���Ͽ� �߰��Ѵ�                                  */
	/* ************************************************************************** */
//	mcls_alarm_list.On_Lot_ListFile_Add(str_alarm_code, str_display_time);  // LOT�� �˶� �߻� Ƚ�� ���� �ӽ� ���Ͽ� �߰� �Լ� */
	/* ************************************************************************** */
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* ���Ͽ� ����� ��� �˶� ���� ���� ������ �����ϴ� �Լ�                         */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Info_Load()
{
	CString str_load_file;
	CString str_read_data;  // ���Ͽ� ����� �� ���� ���� ���� ����
	CString str_temp_data;
	CString str_comment_data;
	int n_count, n_len;
	int n_pos = 0 ;

	char chr_s[300];
	FILE  *fp ;  // ���� �ڵ� ���� ����

	str_load_file = _T("c:\\AMT8520\\Bmp\\Error.txt"); 

	/* ************************************************************************** */
    /* �˶� ���� ������ ���� ����ü ��� ���� �ʱ�ȭ                              */
    /* ************************************************************************** */
	for (n_count = 0; n_count < 2000; n_count++)  
	{
		(st_alarm.mstr_e_content[n_count]).Empty();  // ���Ͽ��� �ε��� �� ���� ���� ���� ���� �ʱ�ȭ
		(st_alarm.mstr_e_code[n_count]).Empty();     // �˶� Code ���� ���� �ʱ�ȭ
		(st_alarm.mstr_e_state[n_count]).Empty();    // �˶� ���� ���� ���� �ʱ�ȭ
		(st_alarm.mstr_e_part[n_count]).Empty();     // �˶� �߻� ���� ���� ���� �ʱ�ȭ
		(st_alarm.mstr_e_msg[n_count]).Empty();      // �˶� �޽��� ���� ���� �ʱ�ȭ
	}
	/* ************************************************************************** */
	/* ************************************************************************** */
    /* �˶� ���� ����� ���� ����                                                 */
    /* ************************************************************************** */
	if ((fp=fopen(str_load_file,"rt"))==NULL)  
	{
		Func.MsgLog("The failure because we open the file.");
		return ;
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
    /* ������ ���� �������� ��� �о� ���� ������ �����Ѵ�                        */
    /* ************************************************************************** */
	while(!feof(fp))  // ���� ������ �˻�
	{
		sprintf(chr_s, "");
		
        if (fgets(chr_s,300,fp)==NULL && ferror(fp))
		{   
			Func.MsgLog("The failure because we read the file.");
			return ;
		}
		
		str_read_data.Format("%s",chr_s);  // ���Ͽ��� ���� �� ���� ���� ����

		n_len = lstrlen(str_read_data);		//���� üũ

		if (!str_read_data.IsEmpty() && n_len > 2)
		{
			str_comment_data = str_read_data.Left(2);		//�ּ������� Ȯ���Ѵ�.

			if (str_comment_data != "//")					//�ּ����� �ƴ϶��...
			{
				st_alarm.mstr_e_content[n_pos] = str_read_data;         // ���Ͽ��� �ε��� �� ���� ���� ����
				st_alarm.mstr_e_code[n_pos] = str_read_data.Left(6) ;    // �˶� Code ����
				st_alarm.mstr_e_state[n_pos] = str_read_data.Mid(7,1) ;  // �˶� ���� ����
				st_alarm.mstr_e_part[n_pos] = str_read_data.Mid(9,1) ;   // �˶� �߻� ���� ����

				str_temp_data = str_read_data.Mid(12,270);
				
				if (st_alarm.mstr_e_code[n_pos] != "990000")
				{
					st_alarm.mstr_e_msg[n_pos] = str_temp_data.Left(str_temp_data.GetLength() - 3) ;  // ���� ���� �˶� �޽��� ����
				}
				else
				{
					st_alarm.mstr_e_msg[n_pos] = str_temp_data.Left(str_temp_data.GetLength() - 1) ;  // ���� ���� �˶� �޽��� ����
				}
				n_pos++;
			}
		}
	}
	/* ************************************************************************** */

	fclose(fp) ;  // ���� ����
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* �߻��� �˶� �޽��� ���� ������ �����ϴ� �Լ�                                   */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Info_Set_to_Variable(CString strErrCode)
{
	int n_cnt ;
	int n_cur_num = -1;    // �˶��� ���� �迭 ��ġ ���� ����
	CString str_err_code;  // �˶� Code ���� ����
	CString strPath;

	CString mstr_create_file;  // �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ����
	CString mstr_content;  // �� ���ο� ����� ���ڿ� ���� ���� ���� 
	int mn_existence;      // ���� ���� ���� ���� �÷��� 
	char fileName[256];    // �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;            // ���Ͽ� ���� ������ ���� ���� 
	
	CString str_title_time, str_file_time, str_new_file, str_display_time;		// ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ����
	int n_cur_year, n_cur_month, n_cur_day;		// ��, ��, �� ���� ����
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur, alarm_occured_time;							// Ÿ��Ʋ �ð� ���� ����

	/* ************************************************************************** */
    /* �Էµ� �˶� Code���� ��/�� ������ �����Ѵ�                                 */
    /* ************************************************************************** */
	str_err_code = strErrCode;
	str_err_code.TrimLeft(' ');	
	str_err_code.TrimRight(' ');
	/* ************************************************************************** */

	/* ************************************************************************** */
    /* �߻��� �˶� Code�� ���� �迭 ��ġ �˻��Ѵ�                                 */
    /* ************************************************************************** */
	for(n_cnt=0; n_cnt<2000; n_cnt++)
	{  
		if ( (st_alarm.mstr_e_code[n_cnt]).Compare(str_err_code) == 0 )
		{
			n_cur_num = n_cnt ; 
			break;
		}
	}
	/* ************************************************************************** */
	
	if (n_cur_num == -1)  // ������ �˶�
	{
		/* ************************************************************************** */
		/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
		/* ************************************************************************** */
		mtime_cur = COleDateTime::GetCurrentTime();
		
		n_cur_year = mtime_cur.GetYear();  
		n_cur_month = mtime_cur.GetMonth();  
		n_cur_day = mtime_cur.GetDay();  

		alarm_occured_time = CTime::GetCurrentTime();
		str_display_time = alarm_occured_time.Format("%Y/%m/%d  %H:%M:%S");
		
		str_new_file.Format("ACD%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
		
		mstr_create_file = "C:\\AMT8520\\Log\\AlarmCodeDebug\\" + str_new_file;
		mstr_create_file += ".TXT";
		
		sprintf(fileName, "%s", mstr_create_file);
		mn_existence = access(fileName,0);
		
		if (mn_existence == -1)  /* ���� ������ */
		{
			mstr_create_file = "C:\\AMT8520\\Log\\AlarmCodeDebug\\" + str_new_file;
			mstr_create_file += ".TXT";
			/* ********************************************************************** */
		}
		
		/* ************************************************************************** */
		/* �˶� �߻� Ƚ�� ���� ���� ���Ͽ� �߰� ������ ���� ���Ϸ� ����               */
		/* ************************************************************************** */
		if ((fp=fopen(mstr_create_file,"a+"))==NULL)  
		{
//			Func.MsgLog("���� ���� ����!..");
			return;
		}
		/* ************************************************************************** */
		
		mstr_content = "ã�� ���� �˶��ڵ� : " + str_err_code;
		fprintf(fp,"%s\n",mstr_content) ;
		mstr_content = "time : " + str_display_time;
		fprintf(fp,"%s\n",mstr_content) ;
		mstr_content = "------------------------------------------------------------------";
		fprintf(fp,"%s\n",mstr_content) ;
		
		if (ferror(fp))  
		{
			Func.MsgLog("���� ���� ����!..") ;
			clearerr(fp);
			fclose(fp);
			return ;
		}
		/* ************************************************************************** */
		
		fclose(fp);  /* ���� ���� */

		alarm.mstr_pcode = alarm.mstr_code;
		alarm.mstr_code = "990000";
		sprintf(alarm.mc_code, "990000");
		str_err_code = "990000";

		for (n_cnt=0; n_cnt<2000; n_cnt++)
		{  
			if ( (st_alarm.mstr_e_code[n_cnt]).Compare(str_err_code) == 0 )
			{
				n_cur_num = n_cnt ; 
				break;
			}
		}
		/* ************************************************************************** */
	}
	
	/* ************************************************************************** */
    /* �߻��� �˶��� ���� ���¿� �޽��� ���� ������ �����Ѵ�                      */
    /* ************************************************************************** */
//	st_alarm.mn_cur_state = atoi(st_alarm.mstr_e_state[n_cur_num]);
	st_alarm.mstr_cur_state = st_alarm.mstr_e_state[n_cur_num];
	st_alarm.mstr_cur_msg = st_alarm.mstr_e_msg[n_cur_num];
	/* ************************************************************************** */
}
/* ****************************************************************************** */
void CMyJamData::On_UnderDaily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg)
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
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur;							// Ÿ��Ʋ �ð� ���� ����
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	mtime_cur = COleDateTime::GetCurrentTime();
	
	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  
	
	str_new_file.Format("DY%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
	
	if (str_alarm_code.IsEmpty()==TRUE)  /* �˶� �ڵ� ������ */
	{
		//Func.MsgLog("Alarm �ڵ� ����!.."); 
		return;
	}
	
	mstr_create_file = "C:\\AMT8520\\Alarm\\UnderDaily\\" + str_new_file;
	mstr_create_file += ".TXT";
	
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file = "C:\\AMT8520\\Alarm\\UnderDaily\\" + str_new_file;
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
	/* �α� ���Ͽ� ���� �߻��� �˶� ���� �����Ѵ�                                 */
	/* ************************************************************************** */
	//mstr_content = "--------------------------------------------------------";
	//fprintf(fp,"%s\n",mstr_content) ;
	
	st_alarm.mn_e_list++;
	
	mstr_temp_data.Format("| %4d |", st_alarm.mn_e_list);
	mstr_content = mstr_temp_data; 
	mstr_temp_data.Format(" %6s |", str_alarm_code);
	mstr_content += mstr_temp_data; 
	mstr_temp_data.Format(" %-44s |", str_alarm_msg);
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format("          |     |");
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format(" %17s |", str_display_time);
	mstr_content += mstr_temp_data;
	fprintf(fp,"%s\n",mstr_content) ;
	
	mstr_content = "+------------------------------------------------------------------------------------------------------+";
	fprintf(fp,"%s\n",mstr_content) ;
	
	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	/* ************************************************************************** */
	
	fclose(fp);  /* ���� ���� */
}

void CMyJamData::On_Daily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg)
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
	
	COleDateTime mtime_cur;						// ���� �ð� ���� ����
	CTime mp_time_cur;							// Ÿ��Ʋ �ð� ���� ����
	
	/* ************************************************************************** */
	/* ���� �̸����� ����� ��¥ �����Ѵ�                                         */
	/* ************************************************************************** */
	mtime_cur = COleDateTime::GetCurrentTime();
	
	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  
	
	str_new_file.Format("DY%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);

	if (str_alarm_code.IsEmpty()==TRUE)  /* �˶� �ڵ� ������ */
	{
		//Func.MsgLog("Alarm �ڵ� ����!.."); 
		return;
	}

	mstr_create_file = "C:\\AMT8520\\Alarm\\Daily\\" + str_new_file;
	mstr_create_file += ".TXT";

	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file = "C:\\AMT8520\\Alarm\\Daily\\" + str_new_file;
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
	/* �α� ���Ͽ� ���� �߻��� �˶� ���� �����Ѵ�                                 */
	/* ************************************************************************** */
	//mstr_content = "--------------------------------------------------------";
	//fprintf(fp,"%s\n",mstr_content) ;

	st_alarm.mn_e_list++;

	mstr_temp_data.Format("| %4d |", st_alarm.mn_e_list);
	mstr_content = mstr_temp_data; 
	mstr_temp_data.Format(" %6s |", str_alarm_code);
	mstr_content += mstr_temp_data; 
	mstr_temp_data.Format(" %-44s |", str_alarm_msg);
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format("          |     |");
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format(" %17s |", str_display_time);
	mstr_content += mstr_temp_data;
	fprintf(fp,"%s\n",mstr_content) ;

	mstr_content = "+------------------------------------------------------------------------------------------------------+";
	fprintf(fp,"%s\n",mstr_content) ;

	if (ferror(fp))  
	{
		Func.MsgLog("���� ���� ����!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	/* ************************************************************************** */

	fclose(fp);  /* ���� ���� */
}

CString GetJamCode_GripperDn_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0GD0%d", iOn );
	strJamCode.Format( "51000%d", iOn );
	return strJamCode;
}

CString GetJamCode_VacuumDn_Time( int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0P%d0%d", iIdx+1, iOn );
	strJamCode.Format( "540%d0%d", iIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_GripperOn_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0GP%d", iOn );
	strJamCode.Format( "5110%02d", iOn );
	return strJamCode;
}

CString GetJamCode_GripperDn_Chk( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0GD0%d", iOn );
	strJamCode.Format( "25000%d", iOn );
	return strJamCode;
}

CString GetJamCode_VacuumDn_Chk( int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0P%d0%d", iIdx+1, iOn );
	strJamCode.Format( "280%d0%d", iIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Plate_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CSP%d0%d", nSite+1, iOn );
	strJamCode.Format( "360%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Insert_Chk( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0I%d0%d", nSite * 5 + iIdx, iOn );
	strJamCode.Format( "270%d0%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Lock_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CSL%d0%d", nSite+1, iOn );
	strJamCode.Format( "350%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_HF_Clamp_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0F%d0%d", nSite+1, iOn );
	strJamCode.Format( "230%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayGuide_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0G%d0%d", nTray+1, iOn );
	strJamCode.Format( "240%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayPusher_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CPS%d0%d", nTray+1, iOn );
	strJamCode.Format( "340%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayExist_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T%d0%d", nTray + 4, iOn );
	strJamCode.Format( "300%d0%d", nTray + 4, iOn );
	return strJamCode;
}

CString GetJamCode_AlignChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0A%d0%d", nIdx+1, iOn );
	strJamCode.Format( "200%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_StopperChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0S%d0%d", nIdx+1, iOn );
	strJamCode.Format( "292%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Align_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0A%d0%d", nIdx+1, iOn );
	strJamCode.Format( "480%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Stopper_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0S%d0%d", nIdx+1, iOn );
	strJamCode.Format( "570%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_ClampChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0C%d0%d", nIdx+1, iOn );
	strJamCode.Format( "210%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Clamp_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0C%d0%d", nIdx+1, iOn );
	strJamCode.Format( "490%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Elev_Z( int nIdx, int nPos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMZ%d0%d", nIdx+1, nPos );
	strJamCode.Format( "465%d0%d", nIdx+1, nPos );
	return strJamCode;
}

CString GetJamCode_Elev_R( int nIdx, int nPos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMR%d0%d", nIdx+1, nPos );
	strJamCode.Format( "461%d0%d", nIdx+1, nPos );
	return strJamCode;
}

CString GetJamCode_ConvTray_Chk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T%d0%d", nIdx + 1, iOn );
	strJamCode.Format( "300%d0%d", nIdx + 1, iOn );
	return strJamCode;
}

CString GetJamCode_Transfer_Y( int npos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMTY0%d", npos );
	strJamCode.Format( "46310%d", npos );
	return strJamCode;
}

CString GetJamCode_Transfer_Dn( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0TD0%d", iOn );
	strJamCode.Format( "31000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Variable( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0RL0%d", iOn );
	strJamCode.Format( "29100%d", iOn );
	return strJamCode;
}

CString GetJamCode_TransferTray( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T70%d", iOn );
	strJamCode.Format( "30070%d", iOn );
	return strJamCode;
}

CString GetJamCode_Transfer_C( int npos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMTC0%d", npos );
	strJamCode.Format( "46300%d", npos );
	return strJamCode;
}

CString GetJamCode_CokLock( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0CL0%d", iOn );
	strJamCode.Format( "22000%d", iOn );
	return strJamCode;
}

CString GetJamCode_TS_Insert_Time( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0I%d0%d", nSite * 5 + iIdx, iOn );
	strJamCode.Format( "520%d0%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}

//2013,0204
CString GetJamCode_TS_Pogo_Time( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
	if(nSite == 0)	strJamCode.Format( "520%d1%d", nSite * 5 + iIdx, iOn );
	else            strJamCode.Format( "521%d1%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}


CString GetJamCode_TS_Plate_Time( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TSP%d0%d", nSite+1, iOn );
	strJamCode.Format( "640%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Lock_Time( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TSL%d0%d", nSite+1, iOn );
	strJamCode.Format( "630%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayGuide_Time( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0G%d0%d", nTray+1, iOn );
	strJamCode.Format( "500%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayPusher_Time( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TPS%d0%d", nTray+1, iOn );
	strJamCode.Format( "530%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_ModuleExist_Chk( int nPicker, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CIT%d0%d", nPicker+1, iOn );
	strJamCode.Format( "330%d0%d", nPicker+1, iOn );
	return strJamCode;
}

CString GetJamCode_GripperOn_Chk( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0GP0%d", iOn );
	strJamCode.Format( "26000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Rej_Guide_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0RG0%d", iOn );
	strJamCode.Format( "55000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Rej_Seperator_Time( int iOn )
{//550010 1 A "����Ʈ ����Ŀ ���۷����� ���� ���ѽð� ����."
	CString strJamCode;
// 	strJamCode.Format( "T0RG0%d", iOn );
	strJamCode.Format( "55001%d", iOn );
	return strJamCode;
}


CString GetJamCode_Rail_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0RL0%d", iOn );
	strJamCode.Format( "56000%d", iOn );
	return strJamCode;
}

CString GetJamcode_Rej_Transfer_Down_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0TD0%d", iOn );
	strJamCode.Format( "59000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Pogo_Insert_Before_Chk( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
	strJamCode.Format( "270%d1%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}
