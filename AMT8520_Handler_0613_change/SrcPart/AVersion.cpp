// AVersion.cpp: implementation of the AVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AVersion.h"

#include "Run_Network.h"
#include "AMTRegistry.h"
#include "SrcPart/APartNetworkManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <string>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AVersion g_ver;

AVersion::AVersion()
{
	m_strUpdate = "Update Desc...";

	m_strHandlerName = "";
	m_strVersion = "";
	m_strDate = "";
}

AVersion::~AVersion()
{

}

void AVersion::CheckVersion()
{
	std::string strVer_old = AMTRegistry::RegReadString( REG_KEY_VER, REG_VALUE_CREATE_VER, 0 );
	std::string strDate_old = AMTRegistry::RegReadString( REG_KEY_VER, REG_VALUE_CREATE_DATE, 0 );
	
 	if( m_strVersion != strVer_old.c_str() ||
 		m_strDate != strDate_old.c_str() )
 	{
		CString strMsg;
// 		strMsg.Format( "FUNCTION=VERSION_UPDATE EQP_ID=%s MODEL=\"SSD AUTO SORTER\" VERSION=\"%s_%s\" DESC=\"%s\"", st_lamp.mstr_equip_id, m_strVersion,
// 			m_strDate, m_strUpdate );
//2013,0715
// 		strMsg.Format( "FUNCTION=VERSION_UPDATE EQP_ID=%s MODEL=\"SSD INTERFACE\" VERSION=\"%s %s\" DESC=\"%s\"", st_lamp.mstr_equip_id, m_strVersion,
// 			m_strDate, m_strUpdate );

		////2015.0604
		strMsg.Format( "FUNCTION=VERSION_UPDATE EQPID=%s EQPMODEL=%s DESC=%s-%s", st_lamp.mstr_equip_id, st_lamp.mstr_equip_no, m_strVersion, m_strDate );
		////
		
		g_client_ec.PushSendMsg( (LPCSTR)strMsg );
 	}
}

void AVersion::UpdateVersion()
{
	AMTRegistry::RegWriteString( REG_KEY_VER, REG_VALUE_CREATE_VER, (LPCSTR)m_strVersion );
	AMTRegistry::RegWriteString( REG_KEY_VER, REG_VALUE_CREATE_DATE, (LPCSTR)m_strDate );
	if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
	{
		sprintf(st_msg.c_normal_msg, "%s %s",m_strVersion,m_strDate);
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
	}
}

void AVersion::SetVersion()
{
	CString str_temp;

	//2013,0715
	if(CHINA_VER == 1)
	{
//		m_strHandlerName = "AMT8520(CHINA)";
		m_strHandlerName = "AMT8520";
	}
	else
	{
		m_strHandlerName = "AMT8520";
	}
	m_strVersion = "Ver2.0.00";
// 	m_strVersion = "2.00";

////2015.0604
//	str_temp.Format("%04d%02d%02d", 2015, 10, 05);
	str_temp.Format("TP - %04d%02d%02d", 2015, 11, 19);
	m_strDate = str_temp;
//	str_temp.Format("%04d/%02d/%02d", 2015, 6, 12);
//	m_strDate = str_temp + "_TP";
////

	m_strUpdate = "Full Inline Test";

	////2015.0604
	//CheckVersion();
	if(st_handler.mn_version_mode == CTL_YES)
	{
		CheckVersion();
	}
	////
	UpdateVersion();
}

CString AVersion::GetStrVersion()
{
	CString strRet;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		strRet.Format("%s [SSD Interface Tester] : %s_%s(MIRROR)", m_strHandlerName, m_strVersion, m_strDate );
	}
	else
	{
		strRet.Format("%s [SSD Interface Tester] : %s_%s", m_strHandlerName, m_strVersion, m_strDate );
	}
	//2015.0914
	:: WritePrivateProfileString("FILLATIO", "Ver", LPCTSTR(strRet), st_path.mstr_basic);
	return strRet;
}