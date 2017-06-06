// APartRobot.cpp: implementation of the APartRobot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "APartRobot.h"
#include "../AMTLotManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APicker::APicker()
{
	m_nTargetPos = -1;
	m_nPickPos = -1;
}

APicker::~APicker()
{
	
}

//////////////////////////////////////////////////////////////////////////
APartRobot g_robot;

APartRobot::APartRobot()
{

}

APartRobot::~APartRobot()
{

}

bool APartRobot::IsPicking()
{
	for( int i=0; i<PICKER_CNT; i++ )
	{
		if( m_Picker[i].GetModule().IsExist() )
			return true;
	}

	return false;
}

void APartRobot::AddJob( bool bPick, int iRobotFirst, int iRobotSecond, int iRobotThird, int iRobotFourth, int iPosFirst, int iPosSecond, int iPosThird, int iPosFourth)
{
	if( iPosFirst < 0 )
	{
		int a = 1;
		return;
	}

	stJobPicker st;
	st.Set( bPick, iRobotFirst, iRobotSecond, iRobotThird, iRobotFourth, iPosFirst, iPosSecond, iPosThird, iPosFourth );

	if(iPosFirst == 200)
	{
		int c= 1;
	}

	m_vecJob.push_back( st );
}

void APartRobot::AddJob( bool bPick, int iRobotFirst, int iRobotSecond, int iPosFirst)
{
	if( iPosFirst < 0 )
	{
		int a = 1;
		return;
	}

	stJobPicker st;
	st.Set( bPick, iRobotFirst, iRobotSecond, iPosFirst );

	if(iPosFirst == 200)
	{
		int c= 1;
	}

	m_vecJob.push_back( st );
}


stJobPicker& APartRobot::GetFirstJob()
{
	if( m_vecJob.size() > 0 )
		return m_vecJob.at(0);

	return m_dummyJob;
}

void APartRobot::EraseFirstJob()
{
	if( m_vecJob.size() > 0 )
		m_vecJob.erase( m_vecJob.begin() );
}

stJobPicker& APartRobot::GetJob( int iIdx )
{
	if( m_vecJob.size() > iIdx )
		return m_vecJob.at(iIdx);

	return m_dummyJob;
}

int APartRobot::GetModuleCnt()
{
	int nRtn = 0;
	for( int i=0; i<PICKER_CNT; i++ )
	{
		if( m_Picker[i].GetModule().IsExist() &&
			m_Picker[i].GetModule().GetLotNo() == g_lotMgr.GetLotIDAt(0) )
			nRtn++;
	}

	return nRtn;
}



//////////////////////////////////////////////////////////////////////////
ENUM_MODULE_POSITION GetPosByTarget( int nTarget )
{
	if( nTarget < MPOS_INDEX_BIN1 )		return MPOS_INDEX_TESTSITE;
	else if( nTarget < MPOS_INDEX_BIN2 )	return MPOS_INDEX_BIN1;
	else if( nTarget < MPOS_INDEX_BIN3 )	return MPOS_INDEX_BIN2;//ybs
//	else if( nTarget < MPOS_INDEX_REJ )	return MPOS_INDEX_BIN2;
	else if( nTarget < MPOS_INDEX_REJ )	return MPOS_INDEX_BIN3;
	else if( nTarget < MPOS_INDEX_COK_STACKER )	return MPOS_INDEX_REJ;
	else									return MPOS_INDEX_COK_STACKER;
};

int GetPosPartByTarget( int nTarget )
{
	if( GetPosByTarget( nTarget ) == MPOS_INDEX_TESTSITE )
	{
		int nRtn = nTarget / 5;
		if( nRtn >= 4 )
			nRtn -= 2;
		return nRtn;
	}
	else
	{
		return GetPosByTarget( nTarget );
	}
};