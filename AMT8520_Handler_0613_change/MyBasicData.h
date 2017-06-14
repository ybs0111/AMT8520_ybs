// MyBasicData.h: interface for the CMyBasicData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYBASICDATA_H__F10E96B3_4961_4A37_B43F_FDAD481A0494__INCLUDED_)
#define AFX_MYBASICDATA_H__F10E96B3_4961_4A37_B43F_FDAD481A0494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Variable.h"  // 전역 변수 정의 클래스 추가 
#include "Public_Function.h"

enum ENUM_DEGREE
{
	EDEG_0,
	EDEG_90,
	EDEG_180,
	EDEG_270,
	MAX_EDEG,
};

enum ENUM_PICK_TYPE
{
	EPT_1V1M_4USE,
	EPT_1V1M_2USE,
	EPT_2V1M_2USE,
	EPT_1V1M_4USE_2,
	MAX_EPT,
};

class CMyBasicData  
{
public:
	CMyBasicData();
	virtual ~CMyBasicData();

// Attributes
public:

// Operations
public:
	void OnAdmin_Data_Load();
	void On_Teach_Data_Save();
	//void On_Teach_Data_Load();
	int On_Teach_Data_Load();
	CString OnGet_Teach_File_Name(CString strName);
	CString OnGet_File_Name();
	void OnPogoAlignType_Save(CString strFile);//2013,0215
	void OnDaily_Count_Save();
	void OnBasic_Data_Save();
	void OnBasic_Data_Load();
	void OnMaintenance_Data_Save();
	void OnMaintenance_Data_Load();
	void OnWaitTime_Data_Save();
	void OnWaitTime_Data_Load();
	void OnMotorSpeed_Set_Data_Save();
	void OnMotorSpeed_Set_Data_Load();
	void OnStep_File_Name_Save(CString str_file);
	CString OnStep_File_Name_Load();
	void OnStep_File_Index_Save(CString str_index);
	CString OnStep_File_Index_Load();
	void OnPassWord_Save(CString str_save_level, CString str_pass_data);
	void OnPassWord_Load(CString str_load_level);

	
};

#endif // !defined(AFX_MYBASICDATA_H__F10E96B3_4961_4A37_B43F_FDAD481A0494__INCLUDED_)
