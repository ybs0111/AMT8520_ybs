// MyBasicData.cpp: implementation of the CMyBasicData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Handler.h"
#include "MyBasicData.h"
#include "io.h"
#include "math.h"
#include "ComizoaPublic.h"
#include "Run_Network.h"

#include "SrcPart/APartNetworkManager.h"
#include "SrcPart/APartHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyBasicData::CMyBasicData()
{

}

CMyBasicData::~CMyBasicData()
{

}

void CMyBasicData::OnPassWord_Load(CString str_load_level)
{
	CString str_pass;
	char chr_data[50];						// ��ȣ�� 25���� �̳��� ����

	(st_msg.mstr_password).Empty();		// ��ȣ ���� ���� �ʱ�ȭ

	if (str_load_level == "Lock")			// SYSTEM LOCK ��ȣ
		:: GetPrivateProfileString("Password", "SysLock", "M", chr_data, 50, st_path.mstr_basic);
	else if (str_load_level == "Level1")	// ����Ʈ ��ȣ
		:: GetPrivateProfileString("Password", "Level_1", "M", chr_data, 50, st_path.mstr_basic);
	else if (str_load_level == "Level2")	// ƼĪ ��ȣ
		:: GetPrivateProfileString("Password", "Level_2", "T", chr_data, 50, st_path.mstr_basic);
	else if (str_load_level == "MAXCOUNT")	// ƼĪ ��ȣ
		:: GetPrivateProfileString("Password", "MAX_COUNT", "AMT8520", chr_data, 50, st_path.mstr_basic);
	else  
		return;

	str_pass = chr_data;
	str_pass.MakeUpper();     // ���ڿ� �빮�ڷ� ����
	str_pass.TrimLeft(' ');   // ���� ���ڿ� ���� ����              
	str_pass.TrimRight(' ');  // ���� ���ڿ� ���� ����

	st_msg.mstr_password = str_pass;  // ��ȣ ���� ������ ����
}

void CMyBasicData::OnPassWord_Save(CString str_save_level, CString str_pass_data)
{
	if (str_save_level=="Lock")         // SYSTEM LOCK ��ȣ
		:: WritePrivateProfileString("Password", "SysLock", LPCTSTR(str_pass_data), st_path.mstr_basic);
	else if (str_save_level=="Level1")  // ����Ʈ ��ȣ
		:: WritePrivateProfileString("Password", "Level_1", LPCTSTR(str_pass_data), st_path.mstr_basic);
	else if (str_save_level=="Level2")  // ƼĪ ��ȣ
		:: WritePrivateProfileString("Password", "Level_2", LPCTSTR(str_pass_data), st_path.mstr_basic);
	else if (str_save_level=="MAXCOUNT")  // ƼĪ ��ȣ
		:: WritePrivateProfileString("Password", "MAXC_OUNT", LPCTSTR(str_pass_data), st_path.mstr_basic);
}

CString CMyBasicData::OnStep_File_Index_Load()
{
	CString str_file_index;  // ���� �ε��� ���� ����

	char chr_load[20];

	:: GetPrivateProfileString("Thread_Step_file", "File_Index", "00", chr_load, 20, st_path.mstr_basic);
	str_file_index = chr_load;
	str_file_index.TrimLeft(' ');               
	str_file_index.TrimRight(' ');

	return str_file_index;  // ���� �ε��� ����
}

void CMyBasicData::OnStep_File_Index_Save(CString str_index)
{
	:: WritePrivateProfileString("Thread_Step_file", "File_Index", LPCTSTR(str_index), st_path.mstr_basic);
}

CString CMyBasicData::OnStep_File_Name_Load()
{
	CString str_file_name;  // ���ϸ� ���� ����

	char chr_load[20];

	:: GetPrivateProfileString("Thread_Step_file", "File_Name", "DEFAULT", chr_load, 20, st_path.mstr_basic);
	str_file_name = chr_load;
	str_file_name.TrimLeft(' ');               
	str_file_name.TrimRight(' ');

	return str_file_name;  // ���ϸ� ����
}

void CMyBasicData::OnStep_File_Name_Save(CString str_file)
{
	:: WritePrivateProfileString("Thread_Step_file", "File_Name", LPCTSTR(str_file), st_path.mstr_basic);
}

void CMyBasicData::OnMotorSpeed_Set_Data_Load()
{
	int  n_chk, i = 0, j = 0;
	double d_chk;
	CString str_temp, str_part;       // �ӽ� ���� ����
	CString str_motor_name;

	char chr_data[20];

	CString str_msg, str_LoadFile;       // �ӽ� ���� ����
	
	FILE    *fp ;
	int     existence;

	//2016.0327
	if( st_basic.n_file_save == CTL_NO)
	{
	
		str_LoadFile = OnGet_Teach_File_Name("Speed");  // ������ ���� ���ϸ� �ε� �Լ� 
		
		existence = access(str_LoadFile, 0);
		
		if (!existence)
		{
			if ((fp=fopen(str_LoadFile,"rb")) == NULL)
			{
				if (st_handler.cwnd_list != NULL)
				{
	//				st_msg.mstr_abnormal_msg.Format("[%s] file open error.", str_LoadFile);
					sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				}
				return ;
			}
		}
		else
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("[%s] file is not exist.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return;
		}
		
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			fread(&COMI.md_spd_vel[i][0],		sizeof(COMI.md_spd_vel[i][0]), 1, fp);
			fread(&COMI.md_spd_vel[i][1],		sizeof(COMI.md_spd_vel[i][1]), 1, fp);
			fread(&COMI.md_spd_vel[i][2],		sizeof(COMI.md_spd_vel[i][2]), 1, fp);
			fread(&COMI.md_spd_home[i],			sizeof(COMI.md_spd_home[i]), 1, fp);
			fread(&COMI.md_spd_jog[i],			sizeof(COMI.md_spd_jog[i]), 1, fp);
			fread(&COMI.md_allow_value[i],		sizeof(COMI.md_allow_value[i]), 1, fp);
		}
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("[%s] FILE Read Error.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Read Error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				clearerr(fp);
			}
		}

		fclose(fp) ;
	}


	//2016.0327
	if( st_basic.n_file_save == CTL_YES)
	{
		for( i = 0; i < MOTOR_COUNT; i++ )
		{//double md_spd_vel[BD_MAX_MOTOR][3]; 		//[0]:VEL, [1]:accel,[2]:deceel
			str_motor_name.Format("%02d_AXIS_SPEED", i+1);
			:: GetPrivateProfileString(str_motor_name, "VEL", "1000", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_spd_vel[i][0] = d_chk;
			
			:: GetPrivateProfileString(str_motor_name, "ACC", "100", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_spd_vel[i][1] = d_chk;
			
			:: GetPrivateProfileString(str_motor_name, "DEC", "100", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_spd_vel[i][2] = d_chk;
			
			:: GetPrivateProfileString(str_motor_name, "HOME", "10", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_spd_home[i] = d_chk;
			:: GetPrivateProfileString(str_motor_name, "JOG", "10", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_spd_jog[i] = d_chk;
			:: GetPrivateProfileString(str_motor_name, "ALLOW", "10", chr_data, 10, st_path.mstr_basic);
			d_chk = atof(chr_data);
			COMI.md_allow_value[i] = d_chk;			
		}
	}

	//Speed Rate
	:: GetPrivateProfileString("Speed_Rate", "Run", "100", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	COMI.mn_runspeed_rate = (double)n_chk;

	:: GetPrivateProfileString("Speed_Rate", "Manual", "80", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	COMI.mn_manualspeed_rate = (double)n_chk;


	//2016.0426
	if(COMI.md_spd_vel[MOTOR_ROBOT_X][1] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_X][1] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_X][2] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_X][2] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_Y][1] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_Y][1] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_Y][2] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_Y][2] = 500;
	}
}

void CMyBasicData::OnMotorSpeed_Set_Data_Save()
{
	int i = 0, j = 0;
	CString str_temp, str_part;       // �ӽ� ���� ����
	CString str_motor_name;
	char chr_buf[20];
	
	CString str_SaveFile;			// �ӽ� ���� ����
	
	//2016.0327
	if(st_basic.n_file_save == CTL_NO)
	{
		str_SaveFile = OnGet_Teach_File_Name("Speed");		// ������ ���� ���ϸ� �ε� �Լ� 
		
		FILE  *fp;
		int   err = 0;
		
		if ((fp = fopen(str_SaveFile, "wb")) == NULL) 
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("Fail in Creating [%s] FILE.", str_SaveFile);
				sprintf(st_msg.c_abnormal_msg, "Fail in Creating [%s] FILE.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return;
		}

		for (i = 0; i < MOTOR_COUNT; i++)
		{
	// 		fwrite(&st_motor[i].md_spd_acc,		sizeof(st_motor[i].md_spd_acc), 1, fp);
	// 		fwrite(&st_motor[i].md_spd_vel,		sizeof(st_motor[i].md_spd_vel), 1, fp);
	// 		fwrite(&st_motor[i].md_spd_dec,		sizeof(st_motor[i].md_spd_dec), 1, fp);
	// 		fwrite(&st_motor[i].md_spd_home,	sizeof(st_motor[i].md_spd_home), 1, fp);
	// 		fwrite(&st_motor[i].md_spd_jog,		sizeof(st_motor[i].md_spd_jog), 1, fp);
	// 		fwrite(&st_motor[i].mn_allow,		sizeof(st_motor[i].mn_allow), 1, fp);

			fwrite(&COMI.md_spd_vel[i][0],		sizeof(COMI.md_spd_vel[i][0]), 1, fp);
			fwrite(&COMI.md_spd_vel[i][1],		sizeof(COMI.md_spd_vel[i][1]), 1, fp);
			fwrite(&COMI.md_spd_vel[i][2],		sizeof(COMI.md_spd_vel[i][2]), 1, fp);
			fwrite(&COMI.md_spd_home[i],		sizeof(COMI.md_spd_home[i]), 1, fp);
			fwrite(&COMI.md_spd_jog[i],			sizeof(COMI.md_spd_jog[i]), 1, fp);
			fwrite(&COMI.md_allow_value[i],		sizeof(COMI.md_allow_value[i]), 1, fp);
		}

		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("[%s] FILE Write Error.", str_SaveFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Write Error.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			clearerr(fp);
		}
		
		fclose(fp);
	}



	//2016.0327
	if(st_basic.n_file_save == CTL_YES)
	{
		for( i = 0; i < MOTOR_COUNT; i++ )
		{//double md_spd_vel[BD_MAX_MOTOR][3]; 		//[0]:VEL, [1]:accel,[2]:deceel
			str_motor_name.Format("%02d_AXIS_SPEED", i+1);
			str_temp = LPCTSTR( _itoa( (int)(COMI.md_spd_vel[i][0]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "VEL", LPCTSTR(str_temp), st_path.mstr_basic );

			str_temp = LPCTSTR( _itoa( (int)(COMI.md_spd_vel[i][1]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "ACC", LPCTSTR(str_temp), st_path.mstr_basic );

			str_temp = LPCTSTR( _itoa( (int)(COMI.md_spd_vel[i][2]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "DEC", LPCTSTR(str_temp), st_path.mstr_basic );

			str_temp = LPCTSTR( _itoa( (int)(COMI.md_spd_home[i]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "HOME", LPCTSTR(str_temp), st_path.mstr_basic );

			str_temp = LPCTSTR( _itoa( (int)(COMI.md_spd_jog[i]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "JOG", LPCTSTR(str_temp), st_path.mstr_basic );
			
			str_temp = LPCTSTR( _itoa( (int)(COMI.md_allow_value[i]) , chr_buf, 10 ) );
			:: WritePrivateProfileString( str_motor_name, "ALLOW", LPCTSTR(str_temp), st_path.mstr_basic );
		}
		
	}
	
	// Speed Rate
	str_temp = LPCTSTR(_itoa((int)(COMI.mn_runspeed_rate), chr_buf, 10));
	:: WritePrivateProfileString("Speed_Rate", "Run", LPCTSTR(str_temp), st_path.mstr_basic);
	
	str_temp = LPCTSTR(_itoa((int)(COMI.mn_manualspeed_rate), chr_buf, 10));
	:: WritePrivateProfileString("Speed_Rate", "Manual", LPCTSTR(str_temp), st_path.mstr_basic);

	//2016.0426
	if(COMI.md_spd_vel[MOTOR_ROBOT_X][1] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_X][1] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_X][2] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_X][2] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_Y][1] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_Y][1] = 500;
	}
	if(COMI.md_spd_vel[MOTOR_ROBOT_Y][2] < 500 )
	{
		COMI.md_spd_vel[MOTOR_ROBOT_Y][2] = 500;
	}
}

void CMyBasicData::OnWaitTime_Data_Load()
{
////2015.0915
// 	CString str_temp;       // �ӽ� ���� ����
// 	CString str_motor_name;
// 	
// 	CString str_msg, str_LoadFile;       // �ӽ� ���� ����
// 	
// 	FILE    *fp ;
// 	int     existence;
// 	
// 	str_LoadFile = OnGet_Teach_File_Name("Time");  // ������ ���� ���ϸ� �ε� �Լ� 
// 	
// 	existence = access(str_LoadFile, 0);
// 	
// 	if (!existence)
// 	{
// 		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
// 		{
// 			if (st_handler.cwnd_list != NULL)
// 			{
// //				st_msg.mstr_abnormal_msg.Format("[%s] file open error.", str_LoadFile);
// 				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
// 				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
// 			}
// 			return ;
// 		}
// 	}
// 	else
// 	{
// 		if (st_handler.cwnd_list != NULL)
// 		{
// //			st_msg.mstr_abnormal_msg.Format("[%s] file is not exist.", str_LoadFile);
// 			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
// 		}
// 		return;
// 	}
// 	
// 	fread(&st_time, sizeof(st_time), 1, fp);
// 	
// 	if (ferror(fp)) 
// 	{
// 		if (st_handler.cwnd_list != NULL)
// 		{
// //			st_msg.mstr_abnormal_msg.Format("[%s] FILE Read Error.", str_LoadFile);
// 			sprintf(st_msg.c_abnormal_msg, "[%s] FILE Read Error.", str_LoadFile);
// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
// 			clearerr(fp);
// 		}
// 	}
// 
// 	fclose(fp) ;

	CString str_temp;       // �ӽ� ���� ����
	CString str_motor_name;
	char chr_data[20];
	
	CString str_msg, str_LoadFile;       // �ӽ� ���� ����
	
	FILE    *fp ;
	int     existence, k, n_chk = 0;
	
	if( st_basic.n_file_save == CTL_NO)
	{
		str_LoadFile = OnGet_Teach_File_Name("Time");  // ������ ���� ���ϸ� �ε� �Լ� 
		
		existence = access(str_LoadFile, 0);
		
		if (!existence)
		{
			if ((fp=fopen(str_LoadFile,"rb")) == NULL)
			{
				if (st_handler.cwnd_list != NULL)
				{
					//    st_msg.mstr_abnormal_msg.Format("[%s] file open error.", str_LoadFile);
					sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				}
				for (k = 0; k < E_WAIT_MAX; k++)
				{
					st_time.n_wait_time[k][IO_ON] = 100;
					st_time.n_wait_time[k][IO_OFF] = 100;
					st_time.n_limit_time[k] = 10000;
				} 
				return ;
			}
		}
		else
		{
			if (st_handler.cwnd_list != NULL)
			{
				//   st_msg.mstr_abnormal_msg.Format("[%s] file is not exist.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			for (k = 0; k < E_WAIT_MAX; k++)
			{
				st_time.n_wait_time[k][IO_ON] = 100;
				st_time.n_wait_time[k][IO_OFF] = 100;
				st_time.n_limit_time[k] = 10000;
			} 
			return;
		}
		
		fread(&st_time, sizeof(st_time), 1, fp);
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				//   st_msg.mstr_abnormal_msg.Format("[%s] FILE Read Error.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Read Error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				clearerr(fp);
			}
		}
		fclose(fp) ;
	}



	//2016.0327
	if( st_basic.n_file_save == CTL_YES)
	{
		for ( k = 0 ; k < E_WAIT_MAX; k++)
		{
			str_temp.Format( "TIME_[%02d]", k+1);
			str_motor_name.Format( "%s_ON", str_temp );
			:: GetPrivateProfileString( "ON_WAIT_TIME", str_motor_name, "100", chr_data, 10, st_path.mstr_basic );
			n_chk = atoi( chr_data );
			st_time.n_wait_time[k][IO_ON] = n_chk;

			str_motor_name.Format( "%s_OFF", str_temp );
			:: GetPrivateProfileString( "OFF_WAIT_TIME", str_motor_name, "100", chr_data, 10, st_path.mstr_basic );
			n_chk = atoi( chr_data );
			st_time.n_wait_time[k][IO_OFF] = n_chk;

			str_motor_name.Format( "%s_LIMIT", str_temp );
			:: GetPrivateProfileString( "LIMIT_TIME", str_motor_name, "100", chr_data, 10, st_path.mstr_basic );
			n_chk = atoi( chr_data );
			st_time.n_limit_time[k] = n_chk;
		}
	}



	for ( k = 0; k < E_WAIT_MAX; k++)
	{
		if( st_time.n_wait_time[k][IO_ON] <= 0 )
			st_time.n_wait_time[k][IO_ON] = 100;
		if( st_time.n_wait_time[k][IO_OFF] <= 0 )
			st_time.n_wait_time[k][IO_OFF] = 100;
		if( st_time.n_limit_time[k] <= 0 )
			st_time.n_limit_time[k] = 10000;
	}
	
////
}

void CMyBasicData::OnWaitTime_Data_Save()
{
	CString str_temp;       // �ӽ� ���� ����
	CString str_motor_name, str_data;
	int k;
	
	CString str_SaveFile;			// �ӽ� ���� ����
	
	//2016.0327
	if(st_basic.n_file_save == CTL_NO)
	{
		str_SaveFile = OnGet_Teach_File_Name("Time");		// ������ ���� ���ϸ� �ε� �Լ� 
		
		FILE  *fp;
		int   err = 0;
		
		if ((fp = fopen(str_SaveFile, "wb")) == NULL) 
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("Fail in Creating [%s] FILE.", str_SaveFile);
				sprintf(st_msg.c_abnormal_msg, "Fail in Creating [%s] FILE.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return;
		}
		
		fwrite(&st_time, sizeof(st_time), 1, fp);
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
	//			st_msg.mstr_abnormal_msg.Format("[%s] FILE Write Error.", str_SaveFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Write Error.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			clearerr(fp);
		}
		
		fclose(fp);
	}


	//2016.0327
	if(st_basic.n_file_save == CTL_YES)
	{
		for ( k = 0 ; k < E_WAIT_MAX; k++)
		{
			str_temp.Format( "TIME_[%02d]", k+1);
			str_motor_name.Format( "%s_ON", str_temp );
			if( st_time.n_wait_time[k][IO_ON] <= 0 )
				st_time.n_wait_time[k][IO_ON] = 100;
			str_data.Format("%d", st_time.n_wait_time[k][IO_ON]);
			:: WritePrivateProfileString( "ON_WAIT_TIME", str_motor_name, (LPCTSTR)str_data, st_path.mstr_basic );
			
			str_motor_name.Format( "%s_OFF", str_temp );
			if( st_time.n_wait_time[k][IO_OFF] <= 0 )
				st_time.n_wait_time[k][IO_OFF] = 100;
			str_data.Format("%d", st_time.n_wait_time[k][IO_OFF]);
			:: WritePrivateProfileString( "OFF_WAIT_TIME", str_motor_name, (LPCTSTR)str_data, st_path.mstr_basic );
			
			str_motor_name.Format( "%s_LIMIT", str_temp );
			if( st_time.n_limit_time[k] <= 0 )
				st_time.n_limit_time[k] = 10000;
			str_data.Format("%d", st_time.n_limit_time[k] );
			:: WritePrivateProfileString( "LIMIT_TIME", str_motor_name, (LPCTSTR)str_data, st_path.mstr_basic );
		}

	}
}

void CMyBasicData::OnMaintenance_Data_Load()
{
	CString str_temp, str_part;  // �ε� ���� �ӽ� ���� ����
	int n_chk;
	CString strLoadFile;

	char chr_data[100], chr_buf[20] ;//2015.0604 20 -> 100

	/* ************************************************************************** */
	/* Ÿ�� ���� RED ���� �ε��Ͽ� ���� ������ �����Ѵ�                           */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData_Red", "Stop_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[0] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[0], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[0] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "Run_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[1] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[1], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[1] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "Alarm_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[2] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[2], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[2] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "LotEnd_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[3] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[3], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[3] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "Initial_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[4] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[4], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[4] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "Warring_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[5] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[5], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[5] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "Lock_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[6] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[6], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[6] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Red", "SelfCheck_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_r[7] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[7], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Red", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_r[7] = n_chk;
	/* ************************************************************************** */


	/* ************************************************************************** */
	/* Ÿ�� ���� YELLOW ���� �ε��Ͽ� ���� ������ �����Ѵ�                        */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData_Yellow", "Stop_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[0] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[0], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[0] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "Run_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[1] = 1;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[1], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[1] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "Alarm_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[2] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[2], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[2] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "LotEnd_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[3] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[3], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[3] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "Initial_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[4] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[4], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[4] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "Warring_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[5] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[5], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[5] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "Lock_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[6] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[6], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[6] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Yellow", "SelfCheck_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_y[7] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[7], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Yellow", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_y[7] = n_chk;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* Ÿ�� ���� GREEN ���� �ε��Ͽ� ���� ������ �����Ѵ�                         */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData_Green", "Stop_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[0] = 1;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[0], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[0] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "Run_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[1] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[1], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[1] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "Alarm_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[2] = 0;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[2], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[2] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "LotEnd_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[3] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[3], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[3] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "Initial_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[4] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[4], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[4] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "Warring_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[5] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[5], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[5] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "Lock_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[6] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[6], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[6] = n_chk;

	:: GetPrivateProfileString("TowerLampData_Green", "SelfCheck_State", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>2)
	{
		st_lamp.mn_lamp_g[7] = 2;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[7], chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData_Green", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_g[7] = n_chk;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ���� ��� ��� �ε��Ͽ� ���� ������ ����                                   */
	/* -> 0:���    1:�̻��                                                      */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData", "n_buzzer_mode", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk<0 || n_chk>1)
	{
		st_lamp.mn_buzzer_mode = 1;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_buzzer_mode, chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData", "n_buzzer_mode", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_buzzer_mode = n_chk;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ��� ȣ�� �� ��� �ڵ� �ε��Ͽ� ���� ������ ����                           */
	/* -> �ε� �������� ��/�� ������ �����Ѵ�                                     */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData", "str_equip_no", "AMT", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_equip_no = chr_data;
	(st_lamp.mstr_equip_no).TrimLeft(' ');               
	(st_lamp.mstr_equip_no).TrimRight(' ');

	:: GetPrivateProfileString("TowerLampData", "str_equip_code", "AMT", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_equip_id = chr_data;
	(st_lamp.mstr_equip_id).TrimLeft(' ');               
	(st_lamp.mstr_equip_id).TrimRight(' ');

	:: GetPrivateProfileString("TowerLampData", "str_empty_code", "AMT", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_empty_id = chr_data;
	(st_lamp.mstr_empty_id).TrimLeft(' ');               
	(st_lamp.mstr_empty_id).TrimRight(' ');


	////2015.0604
	:: GetPrivateProfileString("FTP", "ftp_path", "", chr_data, 80, st_path.mstr_basic);
	st_lamp.mstr_ftp_path = chr_data;
	(st_lamp.mstr_ftp_path).TrimLeft(' ');               
	(st_lamp.mstr_ftp_path).TrimRight(' ');

	:: GetPrivateProfileString("FTP", "ftp_ip", "", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_ftp_ip = chr_data;
	(st_lamp.mstr_ftp_ip).TrimLeft(' ');               
	(st_lamp.mstr_ftp_ip).TrimRight(' ');

	:: GetPrivateProfileString("FTP", "ftp_user", "", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_ftp_user = chr_data;
	(st_lamp.mstr_ftp_user).TrimLeft(' ');               
	(st_lamp.mstr_ftp_user).TrimRight(' ');

	:: GetPrivateProfileString("FTP", "ftp_pw", "", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_ftp_pw = chr_data;
	(st_lamp.mstr_ftp_pw).TrimLeft(' ');               
	(st_lamp.mstr_ftp_pw).TrimRight(' ');

	:: GetPrivateProfileString("FTP", "ftp_port", "", chr_data, 20, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk < 0)
	{
		st_lamp.mn_ftp_port = 21;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_ftp_port, chr_buf, 10)) ;
		:: WritePrivateProfileString("FTP", "ftp_port", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_ftp_port = n_chk;

	:: GetPrivateProfileString("FTP", "ftp_scrap_code_name", "", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_ftp_scrap_code_name = "RETEST_RULE.TXT";
	(st_lamp.mstr_ftp_scrap_code_name).TrimLeft(' ');               
	(st_lamp.mstr_ftp_scrap_code_name).TrimRight(' ');

	:: GetPrivateProfileString("FTP", "ftp_recipe_rule_name", "", chr_data, 20, st_path.mstr_basic);
	st_lamp.mstr_ftp_recipe_rule_name = "RECIPE_RULE.TXT";
	(st_lamp.mstr_ftp_recipe_rule_name).TrimLeft(' ');               
	(st_lamp.mstr_ftp_recipe_rule_name).TrimRight(' ');


	:: GetPrivateProfileString("FTP", "ftp_mode", "", chr_data, 20, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk < 0 || n_chk > 2)
	{
		st_basic.mn_mode_ftp = 0;
		str_temp = LPCTSTR(_itoa(st_basic.mn_mode_ftp, chr_buf, 10)) ;
		:: WritePrivateProfileString("FTP", "ftp_mode", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_basic.mn_mode_ftp = n_chk;

	:: GetPrivateProfileString("FTP", "ftp_mode_name", "", chr_data, 20, st_path.mstr_basic);
	st_basic.mstr_mode_ftp = chr_data;
	(st_basic.mstr_mode_ftp).TrimLeft(' ');               
	(st_basic.mstr_mode_ftp).TrimRight(' ');

	////

	/* ************************************************************************** */

	/* ************************************************************************** */
	/* Ÿ�� ���� ON/OFF ��� �ð� �ε��Ͽ� ���� ������ ����                       */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData", "n_lamp_on_time_w", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk < 1)
	{
		st_lamp.mn_lamp_wait_time = 500;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_wait_time, chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData", "n_lamp_on_time_w", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_wait_time = n_chk;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* Ÿ�� ���� ���� �ð� �ε��Ͽ� ���� ������ ����                              */
	/* ************************************************************************** */
	:: GetPrivateProfileString("TowerLampData", "n_lamp_change_time", "0", chr_data, 10, st_path.mstr_basic);
	n_chk = atoi(chr_data);
	if (n_chk < 0)
	{
		st_lamp.mn_lamp_change_time = 5;
		str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_change_time, chr_buf, 10)) ;
		:: WritePrivateProfileString("TowerLampData", "n_lamp_change_time", LPCTSTR(str_temp), st_path.mstr_basic);
	}
	else
		st_lamp.mn_lamp_change_time = n_chk;
	/* ************************************************************************** */
	st_lamp.mn_lamp_change_time = 5;
}

void CMyBasicData::OnMaintenance_Data_Save()
{
	CString str_temp, str_part;  // �ε� ���� �ӽ� ���� ����
	char chr_buf[20] ;
	CString strSaveFile;

	/* ************************************************************************** */
	/* Ÿ�� ���� RED ���� ������ ���Ͽ� �����Ѵ�                                  */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[0], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[1], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[2], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[3], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[4], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[5], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[6], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_r[7], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Red", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);
	/* ************************************************************************** */


	/* ************************************************************************** */
	/* Ÿ�� ���� YELLOW ���� ������ ���Ͽ� �����Ѵ�                               */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[0], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[1], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[2], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[3], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[4], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[5], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[6], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_y[7], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Yellow", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);

	/* ************************************************************************** */

	/* ************************************************************************** */
	/* Ÿ�� ���� GREEN ���� ������ ���Ͽ� �����Ѵ�                                */
	/* ->  STOP			= 0,
		   RUN			= 1,
		   ALARM		= 2,
		   LOTEND		= 3,
		   INIT			= 4,
		   WARRING		= 5,
		   LOCK			= 6,
		   SELFCHECK	= 7														  */
	/* -> �ε� �� [0:OFF    1:ON    2:FLICKER]                                    */
	/* ************************************************************************** */
	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[0], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Stop_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[1], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Run_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[2], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Alarm_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[3], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "LotEnd_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[4], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Initial_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[5], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Warring_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[6], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "Lock_State", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_g[7], chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData_Green", "SelfCheck_State", LPCTSTR(str_temp), st_path.mstr_basic);

	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ���� ��� ��� �ε��Ͽ� ���� ������ ����                                   */
	/* -> 0:���    1:�̻��                                                      */
	/* ************************************************************************** */
	str_temp = LPCTSTR(_itoa(st_lamp.mn_buzzer_mode, chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData", "n_buzzer_mode", LPCTSTR(str_temp), st_path.mstr_basic);
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* ��� ȣ�� �� ��� �ڵ� �ε��Ͽ� ���� ������ ����                           */
	/* -> �ε� �������� ��/�� ������ �����Ѵ�                                     */
	/* ************************************************************************** */
	:: WritePrivateProfileString("TowerLampData", "str_equip_no", LPCTSTR(st_lamp.mstr_equip_no), st_path.mstr_basic);
	:: WritePrivateProfileString("TowerLampData", "str_equip_code", LPCTSTR(st_lamp.mstr_equip_id), st_path.mstr_basic);
	:: WritePrivateProfileString("TowerLampData", "str_empty_code", LPCTSTR(st_lamp.mstr_empty_id), st_path.mstr_basic);
	/* ************************************************************************** */

	////2015.0604
	:: WritePrivateProfileString("FTP", "ftp_path", LPCTSTR(st_lamp.mstr_ftp_path), st_path.mstr_basic);
	:: WritePrivateProfileString("FTP", "ftp_ip", LPCTSTR(st_lamp.mstr_ftp_ip), st_path.mstr_basic);
	:: WritePrivateProfileString("FTP", "ftp_user", LPCTSTR(st_lamp.mstr_ftp_user), st_path.mstr_basic);
	:: WritePrivateProfileString("FTP", "ftp_pw", LPCTSTR(st_lamp.mstr_ftp_pw), st_path.mstr_basic);
	:: WritePrivateProfileString("FTP", "ftp_scrap_code_name", LPCTSTR(st_lamp.mstr_ftp_scrap_code_name), st_path.mstr_basic);
	:: WritePrivateProfileString("FTP", "ftp_recipe_rule_name", LPCTSTR(st_lamp.mstr_ftp_recipe_rule_name), st_path.mstr_basic);

	
	str_temp = LPCTSTR(_itoa(st_lamp.mn_ftp_port, chr_buf, 10));
	:: WritePrivateProfileString("FTP", "ftp_port", LPCTSTR(str_temp), st_path.mstr_basic);

	str_temp = LPCTSTR(_itoa(st_basic.mn_mode_ftp, chr_buf, 10)) ;
	:: WritePrivateProfileString("FTP", "ftp_mode", LPCTSTR(str_temp), st_path.mstr_basic);

	:: WritePrivateProfileString("FTP", "ftp_mode_name", LPCTSTR(st_basic.mstr_mode_ftp), st_path.mstr_basic);

	////

	/* ************************************************************************** */
	/* Ÿ�� ���� ON/OFF ��� �ð� �ε��Ͽ� ���� ������ ����                       */
	/* ************************************************************************** */
	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_wait_time, chr_buf, 10));
	:: WritePrivateProfileString("TowerLampData", "n_lamp_on_time_w", LPCTSTR(str_temp), st_path.mstr_basic);
	/* ************************************************************************** */

	str_temp = LPCTSTR(_itoa(st_lamp.mn_lamp_change_time, chr_buf, 10)) ;
	:: WritePrivateProfileString("TowerLampData", "n_lamp_change_time", LPCTSTR(str_temp), st_path.mstr_basic);

}

void CMyBasicData::OnBasic_Data_Load()
{
	CString str_load_device;	// �ε� ����̽��� ���� ����
	CString str_load_file;
	CString str_chk_ext;		// ���� Ȯ���� ���� ����
	CString mstr_temp, str_part, str_station, str_board;			// ������ ���� �ӽ� ���� ���� 
	char chr_data[50], chr_buf[20];
	int mn_chk = 0, mn_pos = 0, n_pos = 0, i = 0, j = 0;
	double md_chk = 0;
	int nInterval = 0; //kwlee 2015.1005

	/* ************************************************************************** */
    /*  ������ �ε��� ���� �����Ѵ� [���� Ȯ���� �˻�]                            */
    /* ************************************************************************** */
	str_load_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // ƼĪ ������ �ε� ���� ����
	
	n_pos = str_load_file.Find(".");  // Ȯ���� ��ġ �˻�

	if (n_pos == -1)
		str_load_file += _T(".TXT");  // Ȯ���� �߰�
	else 
	{
		str_chk_ext = str_load_file.Mid(n_pos);  // ���� Ȯ���� ����
		if (str_chk_ext != _T(".TXT"))  
		{
			str_load_file = st_path.mstr_path_dvc + _T("DEFAULT.TXT");  // �ε� �κ� X�� ƼĪ ������ �ε� ���ο� ���� ����

			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE FILE] The error happened at a file extension.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
		}
	}
	/* ************************************************************************** */

	:: GetPrivateProfileString("BasicData", "Run_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.mn_mode_run = 1;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_run, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Run_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_mode_run = mn_chk;

	:: GetPrivateProfileString("BasicData", "Device_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_device = 1;
		mstr_temp.Format("%d", st_basic.n_mode_device);
		:: WritePrivateProfileString("BasicData", "Device_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.n_mode_device = mn_chk;

	ReadBasicData_Int( st_basic.mn_bypass_mode, "Bypass_Mode", 0, 0, 3, st_path.mstr_basic );

	:: GetPrivateProfileString("BasicData", "Electric_Check", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 1 || mn_chk > 1440)
	{
		st_basic.mn_electrostatic_check_gap = 120;
		mstr_temp.Format("%d", st_basic.mn_electrostatic_check_gap);
		:: WritePrivateProfileString("BasicData", "Electric_Check", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_electrostatic_check_gap = mn_chk;

	:: GetPrivateProfileString("BasicData", "Electrostatic_Value", "0", chr_data, 10, st_path.mstr_basic);
	md_chk = atof(chr_data);
	if (md_chk < 1 || md_chk > 1440)
	{
		st_basic.md_electrostatic_value = 120;
		mstr_temp.Format("%d", st_basic.md_electrostatic_value);
		:: WritePrivateProfileString("BasicData", "Electrostatic_Value", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.md_electrostatic_value = md_chk;

	:: GetPrivateProfileString("BasicData", "Run_Speed", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 1 || mn_chk > 100)
	{
		st_basic.nRunSpeed = 100;
		mstr_temp.Format("%d", st_basic.nRunSpeed);
		:: WritePrivateProfileString("BasicData", "Run_Speed", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.nRunSpeed = mn_chk;

	:: GetPrivateProfileString("BasicData", "Manual_Speed", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 1 || mn_chk > 100)
	{
		st_basic.nManualSpeed = 60;
		mstr_temp.Format("%d", st_basic.nManualSpeed);
		:: WritePrivateProfileString("BasicData", "Manual_Speed", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.nManualSpeed = mn_chk;

	// Data Logger�� �ִ� �Ϸ��� ���� �ð��� �����´�. ���� ����ϴϱ�.
	:: GetPrivateProfileString("DAY START TIME", "Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 24)
	{
		st_basic.n_day_start_time = 22;
	}
	else  st_basic.n_day_start_time = mn_chk;
	st_basic.n_day_start_time = 22;

	// Data Logger�� �ִ� Shift �ð��� �����´�. ���� ����ϴϱ�.
	:: GetPrivateProfileString("DAY SHIFT TIME", "Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 24)
	{
		st_basic.n_day_shift_time = 8;
	}
	else  st_basic.n_day_shift_time = mn_chk;
	st_basic.n_day_shift_time = 8;

	:: GetPrivateProfileString("BasicData", "Day_Alarm_Count", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		alarm.mn_alarm_occured_day = 0;
		mstr_temp = LPCTSTR(_itoa(alarm.mn_alarm_occured_day, chr_buf, 10)) ;
		:: WritePrivateProfileString("BasicData", "Day_Alarm_Count", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  alarm.mn_alarm_occured_day = mn_chk;

	:: GetPrivateProfileString("BasicData", "Daily_Run_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		st_handler.m_tDR = 0;
	}
	else  st_handler.m_tDR = mn_chk;

	:: GetPrivateProfileString("BasicData", "Daily_Stop_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		st_handler.m_tDS = 0;
	}
	else  st_handler.m_tDS = mn_chk;

	:: GetPrivateProfileString("BasicData", "Daily_Alarm_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		st_handler.m_tDJ = 0;
	}
	else  st_handler.m_tDJ = mn_chk;

	:: GetPrivateProfileString("BasicData", "Daily_Maint_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		st_handler.m_tDM = 0;
	}
	else  st_handler.m_tDM = mn_chk;


	//2015.0914
	:: GetPrivateProfileString("BasicData", "Daily_Ref_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0)
	{
		st_handler.m_tDRef = 0;
	}
	else  st_handler.m_tDRef = mn_chk;
	
	CString str;

	for (j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format("Daily_RunDown_Time[%d]", j);
		:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
		mn_chk = atoi(chr_data);
		if (mn_chk < 0)
		{
			st_handler.m_tDRdown[j] = 0;
		}
		else  st_handler.m_tDRdown[j] = mn_chk;

		str.Format("Daily_Stop[%d]", j);
		:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
		mn_chk = atoi(chr_data);
		if (mn_chk < 0)
		{
			st_handler.m_tDStop[j] = 0;
		}
		else  st_handler.m_tDStop[j] = mn_chk;
		
		str.Format("Daily_UserStop[%d]", j);
		:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
		mn_chk = atoi(chr_data);
		if (mn_chk < 0)
		{
			st_handler.m_tDUserStop[j] = 0;
		}
		else  st_handler.m_tDUserStop[j] = mn_chk;
		
		str.Format("InStandBy_Time[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDInStandby[j]);
		:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
		mn_chk = atoi(chr_data);
		if (mn_chk < 0)
		{
		 	st_handler.m_tDInStandby[j] = 0;
		}
		else  st_handler.m_tDInStandby[j] = mn_chk;

	}


	for ( int k = 0; k < TSITE_INSERT_CNT; k++ )
	{
		for ( int j = 0; j < 2; j++)
		{
			str.Format("Daily_Test_Time[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				st_handler.m_tDtest[j][k] = 0;
			}
			else  st_handler.m_tDtest[j][k] = mn_chk;
			
			str.Format("Daily_SokOff_Time[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				st_handler.m_tDSokOff[j][k] = 0;
			}
			else  st_handler.m_tDSokOff[j][k] = mn_chk;
			
			str.Format("OutStandBy_Time[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				st_handler.m_tDOutStandby[j][k] = 0;
			}
			else  st_handler.m_tDOutStandby[j][k] = mn_chk;
			
			str.Format("Fail_Device[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				Run_Test_Site[j].mn_Fail[k] = 0;
			}
			else  Run_Test_Site[j].mn_Fail[k] = mn_chk;
			
			str.Format("Pass_Device[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				Run_Test_Site[j].mn_Pass[k] = 0;
			}
			else  Run_Test_Site[j].mn_Pass[k] = mn_chk;
			
			str.Format("Input_Device[%d]", (j*10) + k);
			:: GetPrivateProfileString("BasicData", str, "0", chr_data, 10, st_path.mstr_basic);
			mn_chk = atoi(chr_data);
			if (mn_chk < 0)
			{
				Run_Test_Site[j].mn_Input[k] = 0;
			}
			else  Run_Test_Site[j].mn_Input[k] = mn_chk;
		}
	}
	///////////////////////////
	
	

	:: GetPrivateProfileString("Interface", "ECSERVER_IP", "109.117.11.29", chr_data, 16, st_path.mstr_basic);
	mstr_temp = chr_data;
	mstr_temp.MakeUpper();     // ���ڿ� �빮�ڷ� ����
	mstr_temp.TrimLeft(' ');   // ���� ���ڿ� ���� ����              
	mstr_temp.TrimRight(' ');  // ���� ���ڿ� ���� ����
	
//	Run_Network.m_Net_ECServer.SetIP( mstr_temp );
	
	:: GetPrivateProfileString("Interface", "ECSERVER_Port", "12108", chr_data, 10, st_path.mstr_basic);
	mstr_temp = chr_data;
	mstr_temp.MakeUpper();     // ���ڿ� �빮�ڷ� ����
	mstr_temp.TrimLeft(' ');   // ���� ���ڿ� ���� ����              
	mstr_temp.TrimRight(' ');  // ���� ���ڿ� ���� ����
	
//	Run_Network.m_Net_ECServer.SetPort( atoi( (LPCSTR)mstr_temp ) );

	:: GetPrivateProfileString("BasicData", "Network_Wait_Time", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 60)
	{
		st_basic.mn_network_wait_time = 20;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_network_wait_time, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Network_Wait_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_network_wait_time = mn_chk;

	:: GetPrivateProfileString("BasicData", "Network_Retry_Count", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 50)
	{
		st_basic.mn_network_retry_cnt = 3;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_network_retry_cnt, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Network_Retry_Count", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_network_retry_cnt = mn_chk;

	:: GetPrivateProfileString("BasicData", "Ararm_Delay_Time", "-1", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1000)
	{
		st_basic.mn_alarm_delay_time = 10;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_alarm_delay_time, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Ararm_Delay_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_alarm_delay_time = mn_chk;

	:: GetPrivateProfileString("BasicData", "DeviceType", "-1", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.mn_device_type = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_device_type, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "DeviceType", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_device_type = mn_chk;

	:: GetPrivateProfileString("BasicData", "Tray_X", "-1", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 1 || mn_chk > 5)
	{
		st_basic.n_tray_x = 1;
		mstr_temp = LPCTSTR(_itoa(st_basic.n_tray_x, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Tray_X", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_tray_x = mn_chk;

	:: GetPrivateProfileString("BasicData", "TIME_OUT", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 9999)
	{
		st_basic.mn_time_out = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_time_out, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "TIME_OUT", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_time_out = mn_chk;
	
	:: GetPrivateProfileString("BasicData", "TIME_OUT_PER", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 100)
	{
		st_basic.mn_time_out_per = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_time_out_per, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "TIME_OUT_PER", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_time_out_per = mn_chk;

	
	:: GetPrivateProfileString("BasicData", "EMPTY_TRAY_OUT", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 9999)
	{
		st_basic.mn_empty_tray_out = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_empty_tray_out, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "EMPTY_TRAY_OUT", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_empty_tray_out = mn_chk;

	:: GetPrivateProfileString("BasicData", "ROBOT_INIT_TIME", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 9999)
	{
		st_basic.mn_robot_init_time = 10;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_robot_init_time, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "ROBOT_INIT_TIME", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_robot_init_time = mn_chk;

	:: GetPrivateProfileString("BasicData", "Tray_Y", "-1", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 1 || mn_chk > 5)
	{
		st_basic.n_tray_y = 1;
		mstr_temp = LPCTSTR(_itoa(st_basic.n_tray_y, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Tray_Y", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_tray_y = mn_chk;

	:: GetPrivateProfileString("BasicData", "COK_TYPE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk >= 6)
	{
		st_basic.n_cok_type = 0;
		mstr_temp.Format("%d", st_basic.n_cok_type);
		:: WritePrivateProfileString("BasicData", "COK_TYPE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_cok_type = mn_chk;

	:: GetPrivateProfileString("BasicData", "DEGREE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk >= 4)
	{
		st_basic.n_degree = 0;
		mstr_temp.Format("%d", st_basic.n_degree);
		:: WritePrivateProfileString("BasicData", "DEGREE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_degree = mn_chk;

	:: GetPrivateProfileString("BasicData", "PICK_TYPE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk >= 4)// 2013,1031 if (mn_chk < 0 || mn_chk >= 3)
	{
		st_basic.n_pick_type = 0;
		mstr_temp.Format("%d", st_basic.n_pick_type);
		:: WritePrivateProfileString("BasicData", "PICK_TYPE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_pick_type = mn_chk;


	//2013,0215
	//////////////////////////////////////////////////////////////////////////
	:: GetPrivateProfileString("BasicData", "Mirror_Type", "-1", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_mirror_type = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_mirror_type, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Mirror_Type", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_mirror_type = mn_chk;

	//2013,1001
// 	if(CHINA_VER == 1)
// 	{
// 		st_handler.mn_mirror_type = 0;
// 	}

	
	:: GetPrivateProfileString("BasicData", "Pogo_Type", "-1", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_pogo_type = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_pogo_type, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Pogo_Type", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_pogo_type = mn_chk;

	//2013,1001
// 	if(CHINA_VER == 1)
// 	{
// 		st_handler.mn_pogo_type = 1;
// 	}
// 
	:: GetPrivateProfileString("BasicData", "AlignBuffer_Type", "-1", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);

	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.mn_alignBuffer_type = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_alignBuffer_type, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_alignBuffer_type = mn_chk;

	//2013,1001
// 	if(CHINA_VER == 1)
// 	{
// 		st_basic.mn_alignBuffer_type = 1;
// 	}


	//////////////////////////////////////////////////////////////////////////
	//2012,1225
	:: GetPrivateProfileString("BasicData", "Pogo_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_pogo = 1;
		mstr_temp.Format("%d", st_basic.n_mode_pogo);
		:: WritePrivateProfileString("BasicData", "Pogo_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_pogo = mn_chk;
	
	:: GetPrivateProfileString("BasicData", "Jig_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_jig = 1;
		mstr_temp.Format("%d", st_basic.n_mode_jig);
		:: WritePrivateProfileString("BasicData", "Jig_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_jig = mn_chk;
	
	////2015.0914
	:: GetPrivateProfileString("BasicData", "Retest_Jig_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_retest_jig = 1;
		mstr_temp.Format("%d", st_basic.n_retest_jig);
		:: WritePrivateProfileString("BasicData", "Retest_Jig_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_retest_jig = mn_chk;
	////
	:: GetPrivateProfileString("BasicData", "RECIPE_MODE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_recipe_mode = 1;
		mstr_temp.Format("%d", st_basic.n_recipe_mode);
		:: WritePrivateProfileString("BasicData", "RECIPE_MODE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_recipe_mode = mn_chk;
	
	:: GetPrivateProfileString("BasicData", "Recovery_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_recovery = 1;
		mstr_temp.Format("%d", st_basic.n_mode_recovery);
		:: WritePrivateProfileString("BasicData", "Recovery_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.n_mode_recovery = mn_chk;
	//////////////////////////////////////////////////////////////////////////
	//2013,0503
	:: GetPrivateProfileString("BasicData", "HOTFIX_POS", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 9999)
	{
		st_basic.mn_hotfix_pos = 0;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_hotfix_pos, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "HOTFIX_POS", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_hotfix_pos = mn_chk;

	//////////////////////////////////////////////////////////////////////////
	//2013,0906
	:: GetPrivateProfileString("BasicData", "MAX_SOCKET_CNT", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 999999)
	{
		st_basic.mn_socket_max_count = 100;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_socket_max_count, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "MAX_SOCKET_CNT", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.mn_socket_max_count = mn_chk;

	:: GetPrivateProfileString("BasicData", "XGEM_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.mn_mode_xgem = 1;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_xgem, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "XGEM_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_mode_xgem = mn_chk;

	st_basic.m_bFDC_Data = st_basic.mn_mode_xgem;

	:: GetPrivateProfileString("BasicData", "RCMD_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.mn_mode_rcmd = 1;
		mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_rcmd, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "RCMD_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_basic.mn_mode_rcmd = mn_chk;


// 	:: GetPrivateProfileString("BasicData", "RECIPE_NAME", "", chr_data, 20, st_path.mstr_basic);
// 	CString str_recipe;
// 	str_recipe.Format("%s", chr_data);
// 	if (mn_chk < 0 || mn_chk > 1)
// 	{
// 		st_basic.mstr_revipe_name = str_recipe;
// 		:: WritePrivateProfileString("BasicData", "RECIPE_NAME", LPCTSTR(str_recipe), st_path.mstr_basic);
// 	}
// 	else  st_basic.mstr_revipe_name = LPCTSTR(str_recipe);
	//2013,1216
	:: GetPrivateProfileString("BasicData", "RECIPE_NAME", "", chr_data, 20, str_load_file);
	CString str_recipe;
	str_recipe.Format("%s", chr_data);//2012,1216
	if (str_recipe == "")
	{
		st_basic.mstr_revipe_name = str_recipe = "AMT8520";
		:: WritePrivateProfileString("BasicData", "RECIPE_NAME", LPCTSTR(str_recipe), str_load_file);
	}
	else  st_basic.mstr_revipe_name = LPCTSTR(str_recipe);


	:: GetPrivateProfileString("BasicData", "PRODUCT_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_product = 1;
		mstr_temp.Format("%d", st_basic.n_mode_product);
		:: WritePrivateProfileString("BasicData", "PRODUCT_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_product = mn_chk;

	:: GetPrivateProfileString("BasicData", "PRODUCTSITE1_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_product_site1 = 1;
		mstr_temp.Format("%d", st_basic.n_mode_product_site1);
		:: WritePrivateProfileString("BasicData", "PRODUCTSITE1_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_product_site1 = mn_chk;
	
	:: GetPrivateProfileString("BasicData", "PRODUCTSITE2_Mode", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_mode_product_site2 = 1;
		mstr_temp.Format("%d", st_basic.n_mode_product_site2);
		:: WritePrivateProfileString("BasicData", "PRODUCTSITE2_Mode", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_product_site2 = mn_chk;

	:: GetPrivateProfileString("BasicData", "PRODUCTSITE1_SENSE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 3)
	{
		st_basic.n_mode_product_site1_mode = 3;
		mstr_temp.Format("%d", st_basic.n_mode_product_site1_mode);
		:: WritePrivateProfileString("BasicData", "PRODUCTSITE1_SENSE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_product_site1_mode = mn_chk;

	:: GetPrivateProfileString("BasicData", "PRODUCTSITE2_SENSE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 3)
	{
		st_basic.n_mode_product_site2_mode = 3;
		mstr_temp.Format("%d", st_basic.n_mode_product_site1_mode);
		:: WritePrivateProfileString("BasicData", "PRODUCTSITE2_SENSE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_mode_product_site2_mode = mn_chk;

	for( i=0; i<MAX_TRAYCELL; i++ )
	{
		CString strKey;
		strKey.Format("TRAY_OFFSET_1_X_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_1_x[i], strKey, 0, -9999, 9999, str_load_file );

		strKey.Format("TRAY_OFFSET_2_X_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_2_x[i], strKey, 0, -9999, 9999, str_load_file );

		//2012,1225
		strKey.Format("TRAY_OFFSET_3_X_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_3_x[i], strKey, 0, -9999, 9999, str_load_file );

		strKey.Format("TRAY_OFFSET_REJ_X_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_rej_x[i], strKey, 0, -9999, 9999, str_load_file );

		strKey.Format("TRAY_OFFSET_1_Y_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_1_y[i], strKey, 0, -9999, 9999, str_load_file );

		strKey.Format("TRAY_OFFSET_2_Y_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_2_y[i], strKey, 0, -9999, 9999, str_load_file );

		//2012,1225
		strKey.Format("TRAY_OFFSET_3_Y_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_3_y[i], strKey, 0, -9999, 9999, str_load_file );

		strKey.Format("TRAY_OFFSET_REJ_Y_%02d", i );
		ReadBasicData_Double( st_basic.md_tray_offset_rej_y[i], strKey, 0, -9999, 9999, str_load_file );
	}

	////2015.0604
	:: GetPrivateProfileString("BasicData", "Hifix_mode", "", chr_data, 20, st_path.mstr_basic);
	CString str_hifix;
	str_hifix.Format("%s", chr_data);//2012,1216
	str_hifix.TrimLeft(' ');
	str_hifix.TrimRight(' ');
	if (str_hifix == "")
	{
		st_basic.n_str_hifix_mode = str_hifix = "SATA";
		:: WritePrivateProfileString("BasicData", "Hifix_mode", LPCTSTR(str_hifix), st_path.mstr_basic);
	}
	else  st_basic.n_str_hifix_mode = LPCTSTR(str_hifix);
	
	:: GetPrivateProfileString("BasicData", "HIFIX", "SATA", chr_data, 20, str_load_file);
	mstr_temp = chr_data;
	st_basic.n_str_hifix_mode = mstr_temp;
	mstr_temp.TrimLeft();
	mstr_temp.TrimRight();
	
	
	:: GetPrivateProfileString("BasicData", "RECIPE", "AMT8520", chr_data, 20, str_load_file);
	mstr_temp = chr_data;
	st_basic.mstr_revipe_name = mstr_temp;
	mstr_temp.TrimLeft();
	mstr_temp.TrimRight();
	
	:: GetPrivateProfileString("BasicData", "Version_Mode", "-1", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_version_mode = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_version_mode, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "Version_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_version_mode = mn_chk;
	////
	
	//2017.0602
	:: GetPrivateProfileString("BasicData", "XGEM_Mode", "-1", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_xgem_mode = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_xgem_mode, chr_buf, 10));
		:: WritePrivateProfileString("BasicData", "XGEM_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_xgem_mode = mn_chk;


	//2015.1123
	:: GetPrivateProfileString("BasicData", "CSERIAL2_MODE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_cserial2_mode = 0;
		mstr_temp.Format("%d", st_basic.n_cserial2_mode);
		:: WritePrivateProfileString("BasicData", "CSERIAL2_MODE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_cserial2_mode = mn_chk;

	//2016.0110
	:: GetPrivateProfileString("BasicData", "AUTO_TIMEOUT_MODE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_auto_timeout_mode = 0;
		mstr_temp.Format("%d", st_basic.n_auto_timeout_mode);
		:: WritePrivateProfileString("BasicData", "AUTO_TIMEOUT_MODE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_auto_timeout_mode = mn_chk;

	//2016.0328
	:: GetPrivateProfileString("BasicData", "AUTO_FILESAVE_MODE", "0", chr_data, 10, str_load_file);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_basic.n_file_save = CTL_NO;
		mstr_temp.Format("%d", st_basic.n_file_save);
		:: WritePrivateProfileString("BasicData", "AUTO_FILESAVE_MODE", LPCTSTR(mstr_temp), str_load_file);
	}
	else  st_basic.n_file_save = mn_chk;

}

void CMyBasicData::OnBasic_Data_Save()
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	CString str_save_file;
	CString str_part, str_chk_ext;
	char chr_buf[20];
	COleDateTime time_cur;
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour, n_pos; // ���� �ð����� ���� ���� 

	/* ************************************************************************** */
    /*  ������ ������ ���� �����Ѵ� [���� Ȯ���� �˻�]                            */
    /* ************************************************************************** */
	str_save_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // ƼĪ ������ ���� ���� ����
	
	n_pos = str_save_file.Find(".");  // Ȯ���� ��ġ �˻�
	
	if (n_pos == -1)
		str_save_file += _T(".TXT");  // Ȯ���� �߰�
	else 
	{
		str_chk_ext = str_save_file.Mid(n_pos);  // ���� Ȯ���� ����
		if (str_chk_ext != _T(".TXT"))  
		{
			str_save_file = st_path.mstr_path_dvc + _T("DEFAULT.TXT");  // ƼĪ ������ ���� ���ο� ���� ����

			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE FILE] The error happened at a file extension.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
		}
	}
	/* ************************************************************************** */

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_run, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Run_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d", st_basic.mn_electrostatic_check_gap);
	:: WritePrivateProfileString("BasicData", "Electric_Check", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d", st_basic.n_mode_device);
	:: WritePrivateProfileString("BasicData", "Device_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%0.2f", st_basic.md_electrostatic_value);
	:: WritePrivateProfileString("BasicData", "ELECT_VALUE", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d", st_basic.nRunSpeed);
	:: WritePrivateProfileString("BasicData", "Run_Speed", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d", st_basic.nManualSpeed);
	:: WritePrivateProfileString("BasicData", "Manual_Speed", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	:: WritePrivateProfileString("BasicData", "Device_Type", LPCTSTR(st_basic.mstr_device_name), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(alarm.mn_alarm_occured_day, chr_buf, 10)) ;
	:: WritePrivateProfileString("BasicData", "Day_Alarm_Count", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDR);
	:: WritePrivateProfileString("BasicData", "Daily_Run_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDS);
	:: WritePrivateProfileString("BasicData", "Daily_Stop_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDJ);
	:: WritePrivateProfileString("BasicData", "Daily_Alarm_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDM);
	:: WritePrivateProfileString("BasicData", "Daily_Maint_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	////2015.0914
	mstr_temp.Format("%d",st_handler.m_tDRef);
	:: WritePrivateProfileString("BasicData", "Daily_Ref_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	CString str;
	for (int j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format("Daily_RunDown_Time[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDRdown[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("m_tDStop[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDStop[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("Daily_UserStop[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDUserStop[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
		str.Format("InStandBy_Time[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDInStandby[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
		
	}
	
	for ( int k = 0; k < TSITE_INSERT_CNT; k++ )
	{
		
		for ( int j = 0; j < 2; j++)
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
	////////////////

	time_cur = COleDateTime::GetCurrentTime();

	mn_cur_year = time_cur.GetYear();  
	mn_cur_month = time_cur.GetMonth();  
    mn_cur_day = time_cur.GetDay();  
	mn_cur_hour = time_cur.GetHour();
	/* ************************************************************************** */

	mstr_temp.Format("%04d%02d%02d%02d", mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour);
	:: WritePrivateProfileString("BasicData", "Data_Save_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_bypass_mode, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Bypass_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_time_out, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "TIME_OUT", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_time_out_per, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "TIME_OUT_PER", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_empty_tray_out, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "EMPTY_TRAY_OUT", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_robot_init_time, chr_buf, 10));//2012,1225
	:: WritePrivateProfileString("BasicData", "ROBOT_INIT_TIME", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_network_wait_time, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Network_Wait_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_network_retry_cnt, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Network_Retry_Count", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_alarm_delay_time, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Ararm_Delay_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
	//st_basic.mn_device_type
	mstr_temp = LPCTSTR(_itoa(st_basic.mn_device_type, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "DeviceType", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.n_tray_x, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Tray_X", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.n_tray_y, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Tray_Y", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_cok_type);
	:: WritePrivateProfileString("BasicData", "COK_TYPE", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_degree);
	:: WritePrivateProfileString("BasicData", "DEGREE", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_pick_type);
	:: WritePrivateProfileString("BasicData", "PICK_TYPE", LPCTSTR(mstr_temp), str_save_file);

	//2013,0215
	//////////////////////////////////////////////////////////////////////////
	mstr_temp = LPCTSTR(_itoa(st_handler.mn_mirror_type, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Mirror_Type", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	mstr_temp = LPCTSTR(_itoa(st_handler.mn_pogo_type, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Pogo_Type", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	mstr_temp.Format("%d", st_basic.mn_alignBuffer_type);
	:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", LPCTSTR(mstr_temp), str_save_file);

	//2013,0215
	//////////////////////////////////////////////////////////////////////////	
	mstr_temp.Format("%d", st_basic.n_mode_pogo);
	:: WritePrivateProfileString("BasicData", "Pogo_Mode", LPCTSTR(mstr_temp), str_save_file);
	
	mstr_temp.Format("%d", st_basic.n_mode_jig);
	:: WritePrivateProfileString("BasicData", "Jig_Mode", LPCTSTR(mstr_temp), str_save_file);
	
	////2015.0914
	mstr_temp.Format("%d", st_basic.n_retest_jig);
	:: WritePrivateProfileString("BasicData", "Retest_Jig_Mode", LPCTSTR(mstr_temp), str_save_file);
	////
	mstr_temp.Format("%d", st_basic.n_recipe_mode);
	:: WritePrivateProfileString("BasicData", "RECIPE_MODE", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_recovery);
	:: WritePrivateProfileString("BasicData", "Recovery_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	//////////////////////////////////////////////////////////////////////////
	//2013,0503
	mstr_temp = LPCTSTR(_itoa(st_basic.mn_hotfix_pos, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "HOTFIX_POS", LPCTSTR(mstr_temp), str_save_file);

	//////////////////////////////////////////////////////////////////////////
	//2013,0906
	mstr_temp = LPCTSTR(_itoa(st_basic.mn_socket_max_count, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "MAX_SOCKET_CNT", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_xgem, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "XGEM_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(_itoa(st_basic.mn_mode_rcmd, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "RCMD_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	
	mstr_temp = LPCTSTR(st_basic.mstr_revipe_name);
	:: WritePrivateProfileString("BasicData", "RECIPE_NAME", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_product);
	:: WritePrivateProfileString("BasicData", "PRODUCT_Mode", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_product_site1);
	:: WritePrivateProfileString("BasicData", "PRODUCTSITE1_Mode", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_product_site2);
	:: WritePrivateProfileString("BasicData", "PRODUCTSITE2_Mode", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_product_site1_mode);
	:: WritePrivateProfileString("BasicData", "PRODUCTSITE1_SENSE", LPCTSTR(mstr_temp), str_save_file);

	mstr_temp.Format("%d", st_basic.n_mode_product_site2_mode);
	:: WritePrivateProfileString("BasicData", "PRODUCTSITE2_SENSE", LPCTSTR(mstr_temp), str_save_file);

	for( int i=0; i<MAX_TRAYCELL; i++ )
	{
		CString strKey;
		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_1_x[i] );
		strKey.Format("TRAY_OFFSET_1_X_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_2_x[i] );
		strKey.Format("TRAY_OFFSET_2_X_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		//2012,1225
		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_3_x[i] );
		strKey.Format("TRAY_OFFSET_3_X_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_rej_x[i] );
		strKey.Format("TRAY_OFFSET_REJ_X_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_1_y[i] );
		strKey.Format("TRAY_OFFSET_1_Y_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_2_y[i] );
		strKey.Format("TRAY_OFFSET_2_Y_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		//2012,1225
		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_3_y[i] );
		strKey.Format("TRAY_OFFSET_3_Y_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);

		ReadBasicData_Double( st_basic.md_tray_offset_3_y[i], strKey, 0, -9999, 9999, str_save_file );
		mstr_temp.Format( "%.03f", st_basic.md_tray_offset_rej_y[i] );

		strKey.Format("TRAY_OFFSET_REJ_Y_%02d", i );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(mstr_temp), str_save_file);
	}

	////2015.0604
	mstr_temp = LPCTSTR(st_basic.n_str_hifix_mode);
	:: WritePrivateProfileString("BasicData", "HIFIX", LPCTSTR(mstr_temp), str_save_file);
	
	mstr_temp = LPCTSTR(_itoa(st_handler.mn_version_mode, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "Version_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp = LPCTSTR(st_basic.mstr_revipe_name);
	:: WritePrivateProfileString("BasicData", "RECIPE", LPCTSTR(mstr_temp), str_save_file);
	////

	//2017.0602
	mstr_temp = LPCTSTR(_itoa(st_handler.mn_xgem_mode, chr_buf, 10));
	:: WritePrivateProfileString("BasicData", "XGEM_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	
	//2015.1123
	mstr_temp.Format("%d", st_basic.n_cserial2_mode);
	:: WritePrivateProfileString("BasicData", "CSERIAL2_MODE", LPCTSTR(mstr_temp), str_save_file);

	//2016.0112
	mstr_temp.Format("%d", st_basic.n_auto_timeout_mode);
	:: WritePrivateProfileString("BasicData", "AUTO_TIMEOUT_MODE", LPCTSTR(mstr_temp), str_save_file);

	//2016.0328
	mstr_temp.Format("%d", st_basic.n_file_save);
	:: WritePrivateProfileString("BasicData", "AUTO_FILESAVE_MODE", LPCTSTR(mstr_temp), str_save_file);
}

//2013,0215
void CMyBasicData::OnPogoAlignType_Save(CString strFile)
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	
	//////////////////////////////////////////////////////////////////////////
	mstr_temp.Format("%d", st_handler.mn_temp_pogo_type);
	:: WritePrivateProfileString("BasicData", "Pogo_Type", LPCTSTR(mstr_temp), strFile);
	
	mstr_temp.Format("%d", st_handler.mn_temp_alignBuffer_type);
	:: WritePrivateProfileString("BasicData", "AlignBuffer_Type", LPCTSTR(mstr_temp), strFile);
	//////////////////////////////////////////////////////////////////////////
}

void CMyBasicData::OnDaily_Count_Save()
{
	CString mstr_temp;  // ������ ���� �ӽ� ���� ���� 
	char chr_buf[20];

	mstr_temp = LPCTSTR(_itoa(alarm.mn_alarm_occured_day, chr_buf, 10)) ;
	:: WritePrivateProfileString("BasicData", "Day_Alarm_Count", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDR);
	:: WritePrivateProfileString("BasicData", "Daily_Run_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDS);
	:: WritePrivateProfileString("BasicData", "Daily_Stop_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDJ);
	:: WritePrivateProfileString("BasicData", "Daily_Alarm_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	mstr_temp.Format("%d",st_handler.m_tDM);
	:: WritePrivateProfileString("BasicData", "Daily_Maint_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);

	//2015.0914
	mstr_temp.Format("%d",st_handler.m_tDRef);
	:: WritePrivateProfileString("BasicData", "Daily_Ref_Time", LPCTSTR(mstr_temp), st_path.mstr_basic);
		

	CString str;
	for (int j =0; j<TSITE_SOCKET_CNT; j++)
	{
		str.Format("InStandBy_Time[%d]", j);
		mstr_temp.Format("%d",st_handler.m_tDInStandby[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);

		str.Format("Daily_RunDown_Time[%d]", j );
		mstr_temp.Format("%d",st_handler.m_tDRdown[j]);
		:: WritePrivateProfileString("BasicData", str, LPCTSTR(mstr_temp), st_path.mstr_basic);
	}

	for ( int k = 0; k < TSITE_INSERT_CNT; k++ )
	{
		
		for ( int j = 0; j < 2; j++)
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
}

CString CMyBasicData::OnGet_File_Name()
{
	CString str_temp;       // �ӽ� ���� ����
	CString str_save_file;  // ���� ���� �ӽ� ���� ����
	CString str_chk_ext;    // ���� Ȯ���� ���� ����
	CString str_part, str_part2;
	CString str_new_save_file;
	int n_pos;
	
	// **************************************************************************
    // Socket Contact Count ������ ������ ���� �����Ѵ� [���� Ȯ���� �˻�]        
    // **************************************************************************
	str_save_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // ƼĪ ������ ���� ���� ����
	
	n_pos = str_save_file.Find(".");  // Ȯ���� ��ġ �˻�
	
	if (n_pos == -1)
		str_save_file += _T(".TXT");  // Ȯ���� �߰�
	else 
	{
		str_chk_ext = str_save_file.Mid(n_pos);  // ���� Ȯ���� ����
		if (str_chk_ext != _T(".TXT"))  
		{
			str_save_file = st_path.mstr_path_dvc + _T("DEFAULT.TXT");  // ƼĪ ������ ���� ���ο� ���� ����
			
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE FILE] The error happened at a file extension.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
		}
	}
	// **************************************************************************

	return str_save_file;  // ���ϸ� ���� 
}

CString CMyBasicData::OnGet_Teach_File_Name(CString strName)
{
	CString str_temp;       // �ӽ� ���� ����
	CString str_save_file;  // ���� ���� �ӽ� ���� ����
	CString str_chk_ext;    // ���� Ȯ���� ���� ����
	CString str_part, str_part2;
	CString str_new_save_file;
	CString str_device_name;
	int n_pos;
	
	n_pos = st_basic.mstr_device_name.Find(".");  // Ȯ���� ��ġ �˻�
	
	if (n_pos == -1)
	{
		st_basic.mstr_device_name += _T(".DAT");  // Ȯ���� �߰�
		n_pos = st_basic.mstr_device_name.Find(".");  // Ȯ���� ��ġ �˻�
	}
	
	str_device_name = st_basic.mstr_device_name.Mid(0, n_pos);
	
	
	if (strName.Find("Back") != -1)
	{
		str_save_file = st_path.mstr_path_back + str_device_name;  // ƼĪ ������ ���� ���� ����
	}
	else
	{
		str_save_file = st_path.mstr_path_dvc + str_device_name;  // ƼĪ ������ ���� ���� ����
	}
	
	if (strName != "")
	{
		str_save_file += "_";
		str_save_file += strName;
	}
	
	n_pos = str_save_file.Find(".");  // Ȯ���� ��ġ �˻�
	
	if (n_pos == -1)
	{
		str_save_file += _T(".DAT");  // Ȯ���� �߰�
	}
	else 
	{
		str_chk_ext = str_save_file.Mid(n_pos);  // ���� Ȯ���� ����
		if (str_chk_ext != _T(".DAT"))  
		{
			str_save_file = st_path.mstr_path_dvc + _T("DEFAULT.DAT");  // ƼĪ ������ ���� ���ο� ���� ����
			
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
//				st_msg.mstr_abnormal_msg = _T("[DEVICE FILE] The error happened at a file extension.");
				sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ��� ��û
			}
		}
	}
	// **************************************************************************
	
	return str_save_file;  // ���ϸ� ���� 
}

/*void CMyBasicData::On_Teach_Data_Load()
{
	CString str_temp, str_msg, str_LoadFile;       // �ӽ� ���� ����
	int i;
	
	FILE    *fp ;
	int     existence;
	
	str_LoadFile = OnGet_Teach_File_Name("Teach");  // ������ ���� ���ϸ� �ε� �Լ� 
	
	existence = access(str_LoadFile, 0);
	
	if (!existence)
	{
		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
//				st_msg.mstr_abnormal_msg.Format("[%s] file open error.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
//			st_msg.mstr_abnormal_msg.Format("[%s] file is not exist.", str_LoadFile);
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return;
	}
	
	for (i = 0; i < MOTOR_COUNT; i++)
	{
		fread(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
	}
	
	if (ferror(fp)) 
	{
		if (st_handler.cwnd_list != NULL)
		{
//			st_msg.mstr_abnormal_msg.Format("[%s] FILE Read Error.", str_LoadFile);
			sprintf(st_msg.c_abnormal_msg, "[%s] FILE Read Error.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			clearerr(fp);
		}
	}

	fclose(fp) ;
}*/

int CMyBasicData::On_Teach_Data_Load()
{
	CString str_temp, str_msg, str_LoadFile, str_load_file, str_chk_ext, str_motor_name, str_pos;       // �ӽ� ���� ����
	int i,k,n_pos;
	double d_chk;
	char chr_data[20];
	
	FILE    *fp ;
	int     existence;
	
	//2016.0328
	if( st_basic.n_file_save == CTL_NO )
	{
		str_LoadFile = OnGet_Teach_File_Name("Teach");  // ������ ���� ���ϸ� �ε� �Լ� 
		
		existence = access(str_LoadFile, 0);
		
		if (!existence)
		{
			if ((fp=fopen(str_LoadFile,"rb")) == NULL)
			{
				if (st_handler.cwnd_list != NULL)
				{
					//				st_msg.mstr_abnormal_msg.Format("[%s] file open error.", str_LoadFile);
					sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				}
				return -1;
			}
		}
		else
		{
			if (st_handler.cwnd_list != NULL)
			{
				//			st_msg.mstr_abnormal_msg.Format("[%s] file is not exist.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return -1;
		}
		
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			fread(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
		}
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				//			st_msg.mstr_abnormal_msg.Format("[%s] FILE Read Error.", str_LoadFile);
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Read Error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				clearerr(fp);
			}
		}
		
		fclose(fp) ;

	}


	//2016.0328
	if( st_basic.n_file_save == CTL_YES )
	{
		str_load_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // ƼĪ ������ �ε� ���� ����
		n_pos = str_load_file.Find(".");  // Ȯ���� ��ġ �˻�
		
		if (n_pos == -1)
			str_load_file += _T(".TXT");  // Ȯ���� �߰�
		else 
		{
			str_chk_ext = str_load_file.Mid(n_pos);  // ���� Ȯ���� ����
			if (str_chk_ext != _T(".TXT"))  
			{
				str_load_file = st_path.mstr_path_dvc + _T("DEFAULT.TXT");  // �ε� �κ� X�� ƼĪ ������ �ε� ���ο� ���� ����
				
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
				{
					sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				}
			}
		}
		
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			//fwrite(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
			str_motor_name.Format("%02d_AXIS_TEACH", i+1);
			for ( k = 0; k  < MAX_POS; k++ )
			{
				str_pos.Format("%02dth_POS", k+1); 
				:: GetPrivateProfileString(str_motor_name, str_pos, "0", chr_data, 10, str_load_file);
				d_chk = atof(chr_data);
				st_motor[i].d_pos[k] = d_chk;
			}
		}
	}

	return RET_GOOD;
}

void CMyBasicData::On_Teach_Data_Save()
{
	CString str_temp, str_SaveFile, str_pos, str_chk_ext;			// �ӽ� ���� ����
	int i,k;
	CString str_load_file, str_motor_name;
	int n_pos;
	
	//2016.0327
	if(st_basic.n_file_save == CTL_NO)
	{

		str_SaveFile = OnGet_Teach_File_Name("Teach");		// ������ ���� ���ϸ� �ε� �Լ� 
		
		FILE  *fp;
		int   err = 0;
		
		if ((fp = fopen(str_SaveFile, "wb")) == NULL) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "Fail in Creating [%s] FILE.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return;
		}
		
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			fwrite(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
		}
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Write Error.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			clearerr(fp);
		}
		
		fclose(fp);

		str_SaveFile = OnGet_Teach_File_Name("Teach_Back");		// ������ ���� ���ϸ� �ε� �Լ� 
			
		if ((fp = fopen(str_SaveFile, "wb")) == NULL) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "Fail in Creating [%s] FILE.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return;
		}
		
		for (i = 0; i < MOTOR_COUNT; i++)
		{
			fwrite(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
		}
		
		if (ferror(fp)) 
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] FILE Write Error.", str_SaveFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			clearerr(fp);
		}
		
		fclose(fp);	
	}



	//2016.0328
	if( st_basic.n_file_save == CTL_YES )
	{
		str_load_file = st_path.mstr_path_dvc + st_basic.mstr_device_name;  // ƼĪ ������ �ε� ���� ����
		n_pos = str_load_file.Find(".");  // Ȯ���� ��ġ �˻�
		
		if (n_pos == -1)
			str_load_file += _T(".TXT");  // Ȯ���� �߰�
		else 
		{
			str_chk_ext = str_load_file.Mid(n_pos);  // ���� Ȯ���� ����
			if (str_chk_ext != _T(".TXT"))  
			{
				str_load_file = st_path.mstr_path_dvc + _T("DEFAULT.TXT");  // �ε� �κ� X�� ƼĪ ������ �ε� ���ο� ���� ����
				
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
				{
					sprintf(st_msg.c_abnormal_msg, "[DEVICE FILE] The error happened at a file extension.");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
				}
			}
		}

		for (i = 0; i < MOTOR_COUNT; i++)
		{
			//fwrite(&st_motor[i].d_pos, sizeof(st_motor[i].d_pos), 1, fp);
			str_motor_name.Format("%02d_AXIS_TEACH", i+1);
			for ( k = 0; k  < MAX_POS; k++ )
			{
				str_pos.Format("%02dth_POS", k+1); 
				str_temp.Format("%.3f", st_motor[i].d_pos[k]);
				:: WritePrivateProfileString( str_motor_name, str_pos, LPCTSTR(str_temp), str_load_file );
			}
		}
	}

}

void CMyBasicData::OnAdmin_Data_Load()
{
	g_client_router.SetIP( Func.GetFileString("ADMIN", "CIP_ROUTER", "9999", 20, st_path.mstr_basic ) );
	g_client_router.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_ROUTER", "9999", 20, st_path.mstr_basic )) );

	g_client_bpc.SetIP( Func.GetFileString("ADMIN", "CIP_BPC", "9999", 20, st_path.mstr_basic ) );
	g_client_bpc.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_BPC", "9999", 20, st_path.mstr_basic )) );

	g_client_ec.SetIP( Func.GetFileString("ADMIN", "CIP_EC", "9999", 20, st_path.mstr_basic ) );
	g_client_ec.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_EC", "9999", 20, st_path.mstr_basic )) );

	g_client_front.SetIP( Func.GetFileString("ADMIN", "CIP_FRONT", "9999", 20, st_path.mstr_basic ) );
	g_client_front.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_FRONT", "9999", 20, st_path.mstr_basic )) );

	g_client_next.SetIP( Func.GetFileString("ADMIN", "CIP_NEXT", "9999", 20, st_path.mstr_basic ) );
	g_client_next.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_NEXT", "9999", 20, st_path.mstr_basic )) );
	
	g_client_xgem.SetIP( Func.GetFileString("ADMIN", "CIP_XGEM", "9999", 20, st_path.mstr_basic ) );
	g_client_xgem.SetPort( atoi(Func.GetFileString("ADMIN", "CPORT_XGEM", "9999", 20, st_path.mstr_basic )) );

	g_server_bpc.SetPort( atoi(Func.GetFileString("ADMIN", "SPORT_BPC", "9999", 20, st_path.mstr_basic )) );
	g_server_front.SetPort( atoi(Func.GetFileString("ADMIN", "SPORT_FRONT", "9999", 20, st_path.mstr_basic )) );
	g_server_next.SetPort( atoi(Func.GetFileString("ADMIN", "SPORT_NEXT", "9999", 20, st_path.mstr_basic )) );
	
//	Run_Network.m_Net_FMachine.SetPort( atoi((LPCSTR)mstr_temp) );
	g_handler.SetMachinePos( (ENUM_MACHINE_POSITION)atoi(Func.GetFileString("ADMIN", "EMPOS", "0", 20, st_path.mstr_basic )) );
}