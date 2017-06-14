// APartTestSite.cpp: implementation of the APartTestSite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "APartTestSite.h"
#include "../AMTRegistry.h"
#include "../AMTLotManager.h"
#include "../ManagerSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <map>
#include <utility>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APartTestSite g_site;

APartTestSite::APartTestSite()
{
	for(int i=0; i<TSITE_SOCKET_CNT; i++ )
	{
		m_bBoardOn[i] = false;

		CString strVal;

		for( int j=0; j<COK_TYPE_CNT; j++ )
		{
			strVal.Format( "%s_%d_%02d", REG_VAL_COUNT_TS, j, i );
			m_nTestCnt[j][i] = AMTRegistry::RegReadInt( REG_KEY_COUNT, strVal );

			//2016.0709
			strVal.Format( "%d_%02d", j, i );
			m_bEnable[j][i] = AMTRegistry::RegReadInt( REG_KEY_USE_SOCKET, strVal, 1 ) == 1 ? true : false;

			strVal.Format( "%s_%d_%2d", REG_VAL_SOCKET_TS, j, i );
			m_strSocketHistory[j][i] = AMTRegistry::RegReadString(REG_KEY_SOCKET_CAUSE, strVal, "").c_str();			
			
		}


		strVal.Format( "%s_%d", REG_VAL_TS_BIN_HISTORY, i );
		m_strBinHistory[i] = AMTRegistry::RegReadString( REG_KEY_BIN, strVal, "" ).c_str();
	}
	//2016.0515
	for(int l=0; l<TSITE_SOCKET_CNT; l++ )
	{
		for( int k=0; k<COK_TYPE_CNT; k++ )
		{
			ManagerReadData( l, k, &m_strSocketSerial[k][l], &m_nTestCnt[k][l]);
		}
	}
}

APartTestSite::~APartTestSite()
{

}

std::vector<int> APartTestSite::GetVecEmptyInsert()
{
	std::vector<int> vecRtn;
	std::multimap<int, int> mapCntSite;

	//2016.0204//2016.0330
// 	for( int i=0; i<TSITE_INSERT_CNT; i++ )
// 	{
// 		int nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 
// 		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
// 			continue;
// 
// 		if( g_site.GetEnableSocket( i * 2 ) == false ||
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
// 			continue;
// 
// //		mapCntSite.insert( std::pair<int,int>( nTestCnt, i ) );
// 		//2016.0329
// 		mapCntSite.insert( std::pair<int,int>( i, i ) );
// 	}

// 	std::multimap<int,int>::iterator it = mapCntSite.begin();
// 	for( it; it != mapCntSite.end(); it++ )
// 	{
// 		int a = it->second;
// 		int b = it->first;
// 		vecRtn.push_back( it->second );
// 	}
	//2016.0329//2016.0330
// 	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
// 	for( it; it != mapCntSite.rend(); it++ )
// 	{
// 		int a = it->second;
// 		int b = it->first;
// 		vecRtn.push_back( it->second );
// 	}

// 	int nTestCnt = 0, i=0, a=0;
// 	std::multimap<int, int> mapCntSite2;
// 	for( i=0; i<5; i++ )
// 	{
// 		nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 		
// 		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
// 			continue;
// 		
// 		if( g_site.GetEnableSocket( i * 2 ) == false ||
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
// 			continue;
// 		
// 		mapCntSite.insert( std::pair<int,int>( i, i ) );
// 	}
// 	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
// 	for( it; it != mapCntSite.rend(); it++ )
// 	{
// 		a = it->second;
// 		vecRtn.push_back( it->second );
// 	}
// 	for( i=5; i<TSITE_INSERT_CNT; i++ )
// 	{
// 		nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 		
// 		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
// 			continue;
// 		
// 		if( g_site.GetEnableSocket( i * 2 ) == false ||
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
// 			continue;
// 		
// 		mapCntSite2.insert( std::pair<int,int>( i, i ) );
// 	}
// 	std::multimap<int,int>::reverse_iterator it2 = mapCntSite2.rbegin();
// 	for( it2; it2 != mapCntSite2.rend(); it2++ )
// 	{
// 		a = it2->second;
// 		vecRtn.push_back( it2->second );
// 	}

	//2016.0703
	if( st_basic.mn_hotfix_pos < 1) st_basic.mn_hotfix_pos = 1;
	int nTestCnt = 0, i=0, a=0;
	std::multimap<int, int> mapCntSite2;
	for( i=0; i<5; i++ )
	{
		nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2];
		if( nTestCnt < m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1] )
			nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1];
		
		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
			continue;
		
// 		if( g_site.GetEnableSocket( i * 2 ) == false ||
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
		if( g_site.GetEnableSocket( i * 2 ) <= 0 ||
			g_site.GetEnableSocket( i * 2 + 1 ) <= 0 )
			continue;
		
		mapCntSite.insert( std::pair<int,int>( i, i ) );
	}
	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
	for( it; it != mapCntSite.rend(); it++ )
	{
		a = it->second;
		vecRtn.push_back( it->second );
	}
	for( i=5; i<TSITE_INSERT_CNT; i++ )
	{
		nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2];
		if( nTestCnt < m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1] )
			nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1];
		
		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
			continue;
		
// 		if( g_site.GetEnableSocket( i * 2 ) == false ||
// 			g_site.GetEnableSocket( i * 2 + 1 ) == false )
		if( g_site.GetEnableSocket( i * 2 ) <= 0 ||
			g_site.GetEnableSocket( i * 2 + 1 ) <= 0 )
			continue;
		
		mapCntSite2.insert( std::pair<int,int>( i, i ) );
	}
	std::multimap<int,int>::reverse_iterator it2 = mapCntSite2.rbegin();
	for( it2; it2 != mapCntSite2.rend(); it2++ )
	{
		a = it2->second;
		vecRtn.push_back( it2->second );
	}
	return vecRtn;
}

std::vector<int> APartTestSite::GetVecTestEndInsert( int nLotIdx )
{
	std::vector<int> vecRtn;
	std::multimap<int, int> mapCntSite;
	//2016.0330
// 	for( int i=0; i<TSITE_INSERT_CNT; i++ )
// 	{
// 		int nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 
// 		if(m_mdl[i * 2].IsExist() == false && m_mdl[i * 2 + 1].IsExist() == false )
// 			continue;
// 
// 		if( (m_mdl[i * 2].IsExist() == false || (m_mdl[i * 2].IsTestEndState() && m_mdl[i * 2].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx) ) ) &&
// 			(m_mdl[i * 2 + 1].IsExist() == false || (m_mdl[i * 2 + 1].IsTestEndState() && m_mdl[i * 2 + 1].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx ) ) ) )
// 		{
// // 			mapCntSite.insert( std::pair<int,int>( nTestCnt, i ) );
// 			//vecRtn.push_back( i );
// 			//2016.0329
// 			mapCntSite.insert( std::pair<int,int>( i, i ) );
// 		}
//  	}
// 	std::multimap<int,int>::iterator it = mapCntSite.begin();
// 	for( it; it != mapCntSite.end(); it++ )
// 	{
// 		int a = it->second;
// 		vecRtn.push_back( it->second );
// 	}
	//2016.0329//2016.0330
// 	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
// 	for( it; it != mapCntSite.rend(); it++ )
// 	{
// 		int a = it->second;
// 		vecRtn.push_back( it->second );
// 	}
	//2016.0330
// 	int nTestCnt=0, i=0, a=0;
// 	std::multimap<int, int> mapCntSite2;
// 	for( i=0; i<5; i++ )
// 	{
// 		nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 		
// 		if(m_mdl[i * 2].IsExist() == false && m_mdl[i * 2 + 1].IsExist() == false )
// 			continue;
// 		
// 		if( (m_mdl[i * 2].IsExist() == false || (m_mdl[i * 2].IsTestEndState() && m_mdl[i * 2].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx) ) ) &&
// 			(m_mdl[i * 2 + 1].IsExist() == false || (m_mdl[i * 2 + 1].IsTestEndState() && m_mdl[i * 2 + 1].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx ) ) ) )
// 		{
// 			mapCntSite.insert( std::pair<int,int>( i, i ) );
// 		}
// 	}
// 	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
// 	for( it; it != mapCntSite.rend(); it++ )
// 	{
// 		a = it->second;
// 		vecRtn.push_back( it->second );
// 	}
// 
// 	for( i=5; i<TSITE_INSERT_CNT; i++ )
// 	{
// 		nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2];
// 		if( nTestCnt < m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] )
// 			nTestCnt = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1];
// 		
// 		if(m_mdl[i * 2].IsExist() == false && m_mdl[i * 2 + 1].IsExist() == false )
// 			continue;
// 		
// 		if( (m_mdl[i * 2].IsExist() == false || (m_mdl[i * 2].IsTestEndState() && m_mdl[i * 2].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx) ) ) &&
// 			(m_mdl[i * 2 + 1].IsExist() == false || (m_mdl[i * 2 + 1].IsTestEndState() && m_mdl[i * 2 + 1].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx ) ) ) )
// 		{
// 			mapCntSite2.insert( std::pair<int,int>( i, i ) );
// 		}
// 	}
// 	std::multimap<int,int>::reverse_iterator it2 = mapCntSite2.rbegin();
// 	for( it2; it2 != mapCntSite2.rend(); it2++ )
// 	{
// 		a = it2->second;
// 		vecRtn.push_back( it2->second );
// 	}

	//2016.0703
	if( st_basic.mn_hotfix_pos < 1) st_basic.mn_hotfix_pos = 1;
	int nTestCnt=0, i=0, a=0;
	std::multimap<int, int> mapCntSite2;
	for( i=0; i<5; i++ )
	{
		nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2];
		if( nTestCnt < m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1] )
			nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1];
		
		if(m_mdl[i * 2].IsExist() == false && m_mdl[i * 2 + 1].IsExist() == false )
			continue;
		
		if( (m_mdl[i * 2].IsExist() == false || (m_mdl[i * 2].IsTestEndState() && m_mdl[i * 2].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx) ) ) &&
			(m_mdl[i * 2 + 1].IsExist() == false || (m_mdl[i * 2 + 1].IsTestEndState() && m_mdl[i * 2 + 1].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx ) ) ) )
		{
			mapCntSite.insert( std::pair<int,int>( i, i ) );
		}
	}
	std::multimap<int,int>::reverse_iterator it = mapCntSite.rbegin();
	for( it; it != mapCntSite.rend(); it++ )
	{
		a = it->second;
		vecRtn.push_back( it->second );
	}
	
	for( i=5; i<TSITE_INSERT_CNT; i++ )
	{
		nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2];
		if( nTestCnt < m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1] )
			nTestCnt = m_nTestCnt[st_basic.mn_hotfix_pos-1][i * 2 + 1];
		
		if(m_mdl[i * 2].IsExist() == false && m_mdl[i * 2 + 1].IsExist() == false )
			continue;
		
		if( (m_mdl[i * 2].IsExist() == false || (m_mdl[i * 2].IsTestEndState() && m_mdl[i * 2].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx) ) ) &&
			(m_mdl[i * 2 + 1].IsExist() == false || (m_mdl[i * 2 + 1].IsTestEndState() && m_mdl[i * 2 + 1].GetLotNo() == g_lotMgr.GetLotIDAt( nLotIdx ) ) ) )
		{
			mapCntSite2.insert( std::pair<int,int>( i, i ) );
		}
	}
	std::multimap<int,int>::reverse_iterator it2 = mapCntSite2.rbegin();
	for( it2; it2 != mapCntSite2.rend(); it2++ )
	{
		a = it2->second;
		vecRtn.push_back( it2->second );
	}
	return vecRtn;
}

void APartTestSite::AddCount( int iIdx )
{
//	m_nTestCnt[st_basic.n_cok_type][iIdx]++;
// 	CString strVal;
// 	strVal.Format( "%s_%d_%02d", REG_VAL_COUNT_TS, st_basic.n_cok_type, iIdx );
// 	AMTRegistry::RegWriteInt( REG_KEY_COUNT, strVal, m_nTestCnt[st_basic.n_cok_type][iIdx] );

	if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
	m_nTestCnt[st_basic.mn_hotfix_pos-1][iIdx]++;
	CString strVal;
	strVal.Format( "%s_%d_%02d", REG_VAL_COUNT_TS, st_basic.mn_hotfix_pos-1, iIdx );
	AMTRegistry::RegWriteInt( REG_KEY_COUNT, strVal, m_nTestCnt[st_basic.mn_hotfix_pos-1][iIdx] );

	//2016.0515
// 	m_nTestCnt[st_basic.mn_hotfix_pos][iIdx]++;
// 	ManagerWriteData(iIdx, st_basic.mn_hotfix_pos, m_nTestCnt[st_basic.mn_hotfix_pos][iIdx]);
}

//2016.0703
void APartTestSite::WriteSocketCount()
{
	if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
	for(int i=0; i<TSITE_SOCKET_CNT; i++ )
	{
		ManagerWriteData(i, st_basic.mn_hotfix_pos-1, m_nTestCnt[st_basic.mn_hotfix_pos-1][i]);
	}
}

int APartTestSite::GetModuleCnt()
{
	int nRtn = 0;

	for( int i=0; i<TSITE_SOCKET_CNT; i++ )
	{
		if( m_mdl[i].IsExist() &&
			m_mdl[i].GetLotNo() == g_lotMgr.GetLotIDAt(0) )
			nRtn++;
	}

	return nRtn;
}

void APartTestSite::AddBinResult( int iIdx, CString strBin )
{
	if( (strBin.GetAt(0) >= '0' && strBin.GetAt(0) <= '9') || (strBin.GetAt(0) == '/') )
	{
		m_strBinHistory[iIdx] += strBin;
		if( m_strBinHistory[iIdx].GetLength() > 20 )
			m_strBinHistory[iIdx] = m_strBinHistory[iIdx].Right(20);

		CString strVal;
		strVal.Format( "%s_%d", REG_VAL_TS_BIN_HISTORY, iIdx );
		AMTRegistry::RegWriteString( REG_KEY_BIN, (LPCSTR)strVal, (LPCSTR)m_strBinHistory[iIdx] );
	}
}

int APartTestSite::GetContinueFailCnt( int iIdx )
{
	int nCnt = 0;

	CString strTemp = m_strBinHistory[iIdx];
	while( strTemp.GetLength() > 0 )
	{
		CString strLast = strTemp.Right(1);
		int nBin = atoi(strLast);
		if( nBin > 3 )
			nCnt++;
		else
			break;

		strTemp = strTemp.Left( strTemp.GetLength() - 1 );
	}

	return nCnt;
}

void APartTestSite::SetEnableSocket( int iIdx, int bVal )
{
// 	if( st_basic.n_cok_type >= 0 )
// 	{
// 		m_bEnable[st_basic.n_cok_type][iIdx] = bVal;
// 		
// 		CString strVal;
// 		strVal.Format( "%d_%02d", st_basic.n_cok_type, iIdx );
// 		AMTRegistry::RegWriteInt( REG_KEY_USE_SOCKET, strVal, m_bEnable[st_basic.n_cok_type][iIdx] );
// 		
// 		if( bVal == true )
// 		{
// 			AddBinResult( iIdx, "0" );
// 		}
// 
// 		if( st_handler.cwnd_main )
// 		{
// 			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
// 		}
// 	}
	//2016.0703
	if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;

	if( st_basic.n_cok_type >= 0 )
	{
		m_bEnable[st_basic.mn_hotfix_pos-1][iIdx] = bVal;
		
		CString strVal;
		strVal.Format( "%d_%02d", st_basic.mn_hotfix_pos-1, iIdx );
		AMTRegistry::RegWriteInt( REG_KEY_USE_SOCKET, strVal, m_bEnable[st_basic.mn_hotfix_pos-1][iIdx] );
		
		//if( bVal == true )//2016.0709
		if( bVal > 0 )
		{
			AddBinResult( iIdx, "0" );
		}
		
		if( st_handler.cwnd_main )
		{
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
		}
	}
}

void APartTestSite::ClearTestCount( int iIdx )
{
// 	m_nTestCnt[st_basic.n_cok_type][iIdx] = 0;
// 	CString strVal;
// 	strVal.Format( "%s_%d_%02d", REG_VAL_COUNT_TS, st_basic.n_cok_type, iIdx );
// 	AMTRegistry::RegWriteInt( REG_KEY_COUNT, strVal, m_nTestCnt[st_basic.n_cok_type][iIdx] );
	//2016.0703
	if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
	m_nTestCnt[st_basic.mn_hotfix_pos-1][iIdx] = 0;
	CString strVal;
	strVal.Format( "%s_%d_%02d", REG_VAL_COUNT_TS, st_basic.mn_hotfix_pos-1, iIdx );
	AMTRegistry::RegWriteInt( REG_KEY_COUNT, strVal, m_nTestCnt[st_basic.mn_hotfix_pos-1][iIdx] );
}

std::vector<int> APartTestSite::GetVecEmptyInsert_Single()
{
	std::vector<int> vecRtn;
	std::multimap<int, int> mapCntSite;
	
	for( int i=0; i<TSITE_INSERT_CNT; i++ )
	{
		if( m_mdl[i * 2].IsExist() || m_mdl[i * 2 + 1].IsExist() )
			continue;

		int nEnable = 0;
		bool bUp = false;
// 		if( g_site.GetEnableSocket( i * 2 ) )		{ nEnable++; bUp = true; }
// 		if( g_site.GetEnableSocket( i * 2 + 1) )	{ nEnable++;			 }
		//2016.0709
		if( g_site.GetEnableSocket( i * 2 ) > 0 )		{ nEnable++; bUp = true; }
		if( g_site.GetEnableSocket( i * 2 + 1) > 0 )	{ nEnable++;			 }
		
		if( nEnable != 1 )
			continue;

// 		int nTestGap = m_nTestCnt[st_basic.n_cok_type][i * 2 + 1] - m_nTestCnt[st_basic.n_cok_type][i * 2];
		int nTestGap = m_nTestCnt[st_basic.mn_hotfix_pos][i * 2 + 1] - m_nTestCnt[st_basic.mn_hotfix_pos][i * 2];
		if( bUp == true )
			nTestGap *= -1;

		mapCntSite.insert( std::pair<int,int>( nTestGap, i ) );
	}

	std::multimap<int,int>::iterator it = mapCntSite.begin();
	for( it; it != mapCntSite.end(); it++ )
	{
		vecRtn.push_back( it->second );
	}

	return vecRtn;
}
