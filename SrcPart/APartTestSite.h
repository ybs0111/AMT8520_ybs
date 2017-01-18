// APartTestSite.h: interface for the APartTestSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APARTTESTSITE_H__16AD9BE6_1112_4935_A65A_3F35452F2DCE__INCLUDED_)
#define AFX_APARTTESTSITE_H__16AD9BE6_1112_4935_A65A_3F35452F2DCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Run_Test_Site.h"
#include "PartFunction.h"
#include <vector>
// #include <algorithm>
// #include <functional>
//std::sort( vecRtn.begin(), vecRtn.end(), std::greater<int>() );

class APartTestSite  
{
public:
	APartTestSite();
	virtual ~APartTestSite();

	std::vector<int> GetVecEmptyInsert();
	std::vector<int> GetVecEmptyInsert_Single();
	std::vector<int> GetVecTestEndInsert( int nLotIdx );

	AModule& GetModule( int iIdx) { return m_mdl[iIdx]; }
	void SetModule( int iIdx, AModule mdl ) { m_mdl[iIdx] = mdl; }
	void EraseModule( int iIdx ) { m_mdl[iIdx].EraseData(); }

	void AddCount( int iIdx );
//	int GetTestCount( int iIdx ) { return m_nTestCnt[st_basic.n_cok_type][iIdx]; }
	int GetTestCount( int iIdx ) 
	{
		if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
		return m_nTestCnt[st_basic.mn_hotfix_pos-1][iIdx]; 
	}
	void ClearTestCount( int iIdx );

// 	void SetEnableSocket( int iIdx, bool bVal );
	//2016.0709
	void SetEnableSocket( int iIdx, int bVal );

// 	bool GetEnableSocket( int iIdx ) { return m_bEnable[st_basic.n_cok_type][iIdx]; }
	//2016.0703
//	bool GetEnableSocket( int iIdx ) { return m_bEnable[st_basic.mn_hotfix_pos][iIdx]; }

	//2016.0709
// 	int GetEnableSocket( int iIdx ) { return m_bEnable[st_basic.mn_hotfix_pos][iIdx]; }
	//2016.0703
	int GetEnableSocket( int iIdx ) 
	{ 
		if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
		return m_bEnable[st_basic.mn_hotfix_pos-1][iIdx]; 
	}


	void SetBoardOn( int iIdx, bool bVal ) { m_bBoardOn[iIdx] = bVal; }
	bool GetBoardOn( int iIdx ) { return m_bBoardOn[iIdx]; }

	int GetModuleCnt();

	void AddBinResult( int iIdx, CString strBin );
	int GetContinueFailCnt( int iIdx );
	CString GetBinHistory( int iIdx ) { return m_strBinHistory[iIdx]; }
	void WriteSocketCount();

protected:
	AModule	m_mdl[TSITE_SOCKET_CNT];
	int m_nTestCnt[COK_TYPE_CNT][TSITE_SOCKET_CNT];
	//bool m_bEnable[COK_TYPE_CNT][TSITE_SOCKET_CNT];
	//2016.0709
	int m_bEnable[COK_TYPE_CNT][TSITE_SOCKET_CNT];
	bool m_bBoardOn[TSITE_SOCKET_CNT];
	CString m_strBinHistory[TSITE_SOCKET_CNT];
	CString m_strSocketHistory[COK_TYPE_CNT][TSITE_SOCKET_CNT];

	//2016.0515
	CString m_strSocketSerial[COK_TYPE_CNT][TSITE_SOCKET_CNT];

};

extern APartTestSite g_site;

#endif // !defined(AFX_APARTTESTSITE_H__16AD9BE6_1112_4935_A65A_3F35452F2DCE__INCLUDED_)

