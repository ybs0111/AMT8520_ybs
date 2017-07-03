// APartTray.cpp: implementation of the APartTray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "handler.h"
#include "APartTray.h"
#include "APartHandler.h"
#include "../AMTLotManager.h"
#include "APartTestSite.h"
#include "APartNetworkManager.h"
#include "../IO_Manager.h"

#include <map>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
ATray::ATray()
{
	m_trayPos = TPOS_NONE;
	m_trayTargetPos = TPOS_NONE;

	m_nBufferNo = -1;
	m_bLastMdl = false;
	m_strLotID = "";
	m_bEmptyTray = true;
	m_bBypass = false;
	m_nProductCnt = 0;
}

ATray::~ATray()
{
	
}

void ATray::SetTrayInfo( CString strLotID, CString strPartID, int nBufferNo, bool bLastMdl, bool bEmptyTray, bool bBypass )
{
	m_strLotID = strLotID;
	m_strPartID = strPartID;
	m_nBufferNo = nBufferNo;
	m_bLastMdl = bLastMdl;
	m_bEmptyTray = bEmptyTray;

	m_bBypass = bBypass;
}

void ATray::Clear()
{
	m_trayPos = TPOS_NONE;
	m_trayTargetPos = TPOS_NONE;

	m_nBufferNo = -1;
	m_bLastMdl = false;
	m_strLotID = "";
	m_bEmptyTray = true;
	m_bBypass = false;

	for( int i=0; i< MAX_MODULE; i++ )
	{
		m_mdl[i].EraseData();
	}
}

bool ATray::IsAllGood( bool bLastModule /*= false */ )
{
	int nTotalCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i< nTotalCnt/*MAX_MODULE*/; i++ )
	{
		if( !bLastModule && m_mdl[i].IsExist() == false )
			return false;

		if( m_mdl[i].IsExist() && m_mdl[i].GetModuleState() != EMS_GOOD )
			return false;
	}	

	return true;
}

bool ATray::IsAllEmpty( bool bLastModule /*= false */ )
{
	int nTotalCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i< nTotalCnt/*MAX_MODULE*/; i++ )
	{
		if( !bLastModule && m_mdl[i].IsExist() == false )
			return false;

		if( m_mdl[i].IsExist() && m_mdl[i].GetModuleState() != EMS_NONE )
			return false;
	}	

	return true;
}

int ATray::GetModuleCnt()
{
	int nTotalCnt = st_basic.n_tray_x * st_basic.n_tray_y;

	int nRtn = 0;
	for( int i=0; i<nTotalCnt; i++ )
	{
		if( GetModule(i).IsExist() )
			nRtn++;
	}

	return nRtn;
}

bool ATray::IsAllState( EMODULE_STATE ems )
{
	int nTotalCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i< nTotalCnt/*MAX_MODULE*/; i++ )
	{
		if( m_mdl[i].IsExist() == false )
			continue;

		if( m_mdl[i].GetModuleState() != ems )
			return false;
	}	

	return true;
}

void ATray::SetPos( ETRAY_POS pos )
{
	 m_trayPos = pos;

	 if( g_handler.GetMachinePos() == EMPOS_REAR )
		 g_client_front.SendTrayInfo();
}

void ATray::SetTargetPos( ETRAY_POS pos )
{
	if( m_trayPos != m_trayTargetPos &&
		(m_trayTargetPos == TPOS_REJECT ||
		m_trayTargetPos == TPOS_NEXT ) )
		return;
	
	m_trayTargetPos = pos;

	if( g_handler.GetMachinePos() == EMPOS_REAR )
		 g_client_front.SendTrayInfo();
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
APartTray g_Tray;
APartTray::APartTray()
{
	m_vecRej.clear();
	m_nRejSelected = -1;
	m_nSelectedTray = -1;

	m_bSendNext_fromRej_Force = false;

	m_nRejTrayNo = 0;
}

APartTray::~APartTray()
{

}

void APartTray::AddTray( ATray tray )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == TPOS_NONE )
		{
			m_tray[i] = tray;
			break;
		}
	}

	if( g_handler.GetMachinePos() == EMPOS_REAR )
		 g_client_front.SendTrayInfo();
}

void APartTray::TrayMinus()
{
	int i,j;
	i=j=0;
	for( j=MAX_TRAY-1; j>=0; j-- )
	{
		if( m_tray[j].GetPos() == TPOS_REJECT && m_tray[j].GetTargetPos() == TPOS_REJECT)
		{
			std::vector<int>::iterator it = g_Tray.m_vecRej.begin();
			for( i=0; i<m_vecRej.size(); i++ )
			{
				if( m_vecRej.at(i) == j )
				{
					it += i;
					break;
				}
			}
			m_vecRej.erase(it);
			m_tray[j].Clear();
			break;
		}
	}
	
	g_Tray.m_nRejSelected = g_Tray.m_vecRej.size() - 1;
	
	if( st_handler.cwnd_main )
	{
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_REJ_FLOOR );
	}
}

////2015.0325
int APartTray::TrayPosCheck( ETRAY_POS pos )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == pos )
		{
			return CTL_YES;
		}
	}
	return CTL_NO;
}
////

void APartTray::TrayMoved( ETRAY_POS posFrom, ETRAY_POS posTo )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == posFrom )
		{
			if( posFrom == TPOS_REJECT && (m_vecRej.size() == 0 || m_vecRej.at( m_vecRej.size() - 1) != i) )
				continue;

			m_tray[i].SetPos( posTo );
			CheckAfterTrayMoved( i, posFrom, posTo );
			g_handler.CalcPriorityBin( posFrom, posTo );


			if( st_handler.cwnd_list )
			{
				sprintf(st_msg.c_normal_msg, "TRAY[%d] MOVE FROM: [%s] -----> TO:[%s]", i, (LPCSTR)GetStrPos(posFrom), (LPCSTR)GetStrPos(posTo) );
				//st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}
			int nCnt = st_basic.n_tray_x * st_basic.n_tray_y;
			CString strTrayDataIn;
			for( int j=1; j<=nCnt; j++ )
			{
				CString strTemp;
				AModule mdl = m_tray[i].GetModule(j - 1);
// 				strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s BIN=%s)", j, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
// 					mdl.GetWWN(), mdl.GetBin() );
				strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s PSID=%s BIN=%s)", j, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
					mdl.GetWWN(), mdl.GetCSN(), mdl.GetPSID(), mdl.GetBin() );
				strTrayDataIn += strTemp;
			}
 			Func.On_Log_Title_Msg(LOG_TOTAL, st_msg.c_normal_msg, strTrayDataIn);

			if( st_handler.cwnd_main )
			{
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_INIT, i );
			}

			if( g_handler.GetMachinePos() == EMPOS_REAR &&
				posFrom == TPOS_FRONT_TO_CONV1 && posTo == TPOS_CONV1 )
			{
				g_handler.SetReadySendNext( false );
			}
			else if( g_handler.GetMachinePos() == EMPOS_FRONT &&
				posTo == TPOS_CONV3 && m_tray[i].GetTargetPos() == TPOS_NEXT )
			{//2013,1222
// 				if( m_tray[i].GetEmptyTray() || m_tray[i].IsAllState( EMS_GOOD ) || m_tray[i].IsAllState( EMS_FAIL ) )
				if( m_tray[i].IsAllEmpty(true) ||  m_tray[i].GetEmptyTray() || m_tray[i].IsAllState( EMS_GOOD ) || m_tray[i].IsAllState( EMS_FAIL ) )
				{
					//ybs g_client_next.SetBufferReady( true );
					//2013,0807
// 					g_client_next.SetBufferReady( true, m_tray[i].GetEmptyTray(), m_tray[i].GetStrLotID());
					if(m_tray[i].GetEmptyTray())
					{
						if(st_basic.mn_empty_tray_out > 0)
						{
							if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
							{
								if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_ON)
								{
									g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());
								}
// 								else if(m_tray[i].IsAllState( EMS_GOOD ) || m_tray[i].IsAllState( EMS_FAIL ))
								else if(m_tray[i].GetModuleCnt() > 0)
								{
									if( st_handler.cwnd_list )
									{
										sprintf(st_msg.c_normal_msg, "emptyin_cnt:[%d],emptynext_cnt:[%d]", st_handler.mn_emptyin_cnt,st_handler.mn_emptynext_cnt);
										st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
									}
									g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());
								}
							}
							else
							{
								g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());
							}
						}
						else
						{
							g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());
						}
					}
					else
					{
						g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());
					}
				}
				else
				{//2013,1222
					if( m_tray[i].IsAllEmpty(true) )
					{
						//if( st_handler.cwnd_list )
						//{
						//	sprintf(st_msg.c_normal_msg, "[Buffer_Ready] AllEmpty [%d]", i);
						//	st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
						//}
						
						g_client_next.SetBufferReady( true, true, m_tray[i].GetStrLotID());//체크 필요 중국 버저노가 차이 있음
					}
				}
			}
			else if( posTo == TPOS_NEXT )
			{
				m_tray[i].Clear();
				g_handler.CheckLotEnd();

				if( g_handler.GetMachinePos() == EMPOS_FRONT )
				{
					g_client_next.SetBufferReady( false );
					//if (st_handler.cwnd_list != NULL)
					//{
					//	sprintf(st_msg.c_normal_msg, "[SMEMA]BufferReady = %d", false);
					//	st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
					//}

				}
			}
			break;
		}
	}
	g_handler.CalcTrayMoveNext_fromRejFull();
	g_handler.CalcTrayMoveNext_fromBin3();
	g_handler.CalcTrayMoveNext_fromReject();
	
	g_handler.CalcTrayMoveNext_fromBin();
}

CString APartTray::GetStrPos( ETRAY_POS epos )
{
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		switch( epos )
		{
		case TPOS_NONE:					return "";
		case TPOS_FRONT:				return "FRONT";
		case TPOS_CONV1:				return "CONV1";
		case TPOS_CONV2:				return "CONV2";
		case TPOS_CONV3:				return "CONV3";
		case TPOS_NEXT:					return "NEXT";
		case TPOS_FRONT_TO_CONV1:		return "FRONT_TO_CONV1";
		case TPOS_CONV1_TO_CONV2:		return "CONV1_TO_CONV2";
		case TPOS_CONV2_TO_CONV3:		return "CONV2_TO_CONV3";
		case TPOS_CONV3_TO_NEXT:		return "CONV3_TO_NEXT";
		case TPOS_CONV_TO_BIN1:			return "CONV_TO_WORK1";
		case TPOS_CONV_TO_BIN2:			return "CONV_TO_WORK2";
		case TPOS_CONV_TO_BIN3:			return "CONV_TO_WORK3";
		case TPOS_BIN_TO_CONV1:			return "WORK_TO_CONV1";
		case TPOS_BIN_TO_CONV2:			return "WORK_TO_CONV2";
		case TPOS_BIN_TO_CONV3:			return "WORK_TO_CONV3";
		case TPOS_BIN1:					return "WORK1";
		case TPOS_BIN2:					return "WORK2";
		case TPOS_BIN3:					return "WORK3";
		case TPOS_BIN_TO_REJECT:		return "WORK1_TO_REJECT";
		case TPOS_REJECT:				return "REJECT";
		}
	}
	else
	{
		switch( epos )
		{
		case TPOS_NONE:					return "";
		case TPOS_FRONT:				return "FRONT";
		case TPOS_CONV1:				return "CONV1";
		case TPOS_CONV2:				return "CONV2";
		case TPOS_CONV3:				return "CONV3";
		case TPOS_NEXT:					return "NEXT";
		case TPOS_FRONT_TO_CONV1:		return "FRONT_TO_CONV1";
		case TPOS_CONV1_TO_CONV2:		return "CONV1_TO_CONV2";
		case TPOS_CONV2_TO_CONV3:		return "CONV2_TO_CONV3";
		case TPOS_CONV3_TO_NEXT:		return "CONV3_TO_NEXT";
		case TPOS_CONV_TO_BIN1:			return "CONV_TO_WORK1";
		case TPOS_CONV_TO_BIN2:			return "CONV_TO_WORK2";
		case TPOS_CONV_TO_BIN3:			return "CONV_TO_WORK3";
		case TPOS_BIN_TO_CONV1:			return "WORK_TO_CONV1";
		case TPOS_BIN_TO_CONV2:			return "WORK_TO_CONV2";
		case TPOS_BIN_TO_CONV3:			return "WORK_TO_CONV3";
		case TPOS_BIN1:					return "WORK1";
		case TPOS_BIN2:					return "WORK2";
		case TPOS_BIN3:					return "WORK3";
		case TPOS_BIN_TO_REJECT:		return "WORK3_TO_REJECT";
		case TPOS_REJECT:				return "REJECT";
		}
	}

	return "";
}

int APartTray::GetRejCnt()
{
	int iCnt = 0;
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == TPOS_REJECT || m_tray[i].GetPos() == TPOS_REJECT_TO_BIN )
		{
			iCnt++;
		}
	}

	return iCnt;
}

ETRAY_POS APartTray::CalcTargetPos( ATray tray )
{
	int iMdlCnt = 0, iEmpCnt = 0, iPickCnt = 0;
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int iBin1_1 = g_Tray.GetIdxByTargetPos( TPOS_BIN1 );
	int iBin2_1 = g_Tray.GetIdxByTargetPos( TPOS_BIN2 );
	int iBin3_1 = g_Tray.GetIdxByTargetPos( TPOS_BIN3 );
	// 빈 트레이이면, BIN3 or REJ
	if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
	{
		iMdlCnt = g_site.GetModuleCnt();
		iPickCnt = g_robot.GetModuleCnt();
		if(iBin1 > -1)
		{
			if(g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin1).GetModuleCnt();
			}
		}
		else if(iBin1_1 > -1)
		{
			if(g_Tray.GetTray(iBin1_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin1_1).GetModuleCnt();
			}
		}
		if(iBin2 > -1)
		{
			if(g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin2).GetModuleCnt();
			}
		}
		else if(iBin2_1 > -1)
		{
			if(g_Tray.GetTray(iBin2_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin2_1).GetModuleCnt();
			}
		}
		if(iBin3 > -1)
		{
			if(g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin3).GetModuleCnt();
			}
		}
		else if(iBin3_1 > -1)
		{
			if(g_Tray.GetTray(iBin3_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin3_1).GetModuleCnt();
			}
		}
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_normal_msg, "EMPTY:%d,iMdlCnt:%d,iPickCnt:%d,iEmpCnt:%d", tray.GetEmptyTray(),iMdlCnt,iPickCnt,iEmpCnt);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}

		CString strTrayData;
		strTrayData.Format("iBin1:%d,iBin2:%d,iBin3:%d,iBin1_1:%d,iBin2_1:%d,iBin3_1:%d",iBin1,iBin2,iBin3,iBin1_1,iBin2_1,iBin3_1);

// 		Func.On_Log_Title_Msg(LOG_TOTAL, st_msg.c_normal_msg, strTrayData);
	}
	
	if( tray.GetEmptyTray() )
	{
//		if( g_Tray.GetIdxByPos(TPOS_REJECT) > -1 || g_Tray.GetIdxByTargetPos( TPOS_REJECT ) > -1 )
//		{
//			if( IsEnablePos( TPOS_BIN3 ) )
//				return TPOS_BIN3;
//			else
//			{
//				return TPOS_NEXT;
//			}
//		}
//
//		if( IsEnablePos( TPOS_REJECT ) )
//			return TPOS_REJECT;
//		else
//		{
//			return TPOS_NEXT;
//		}

		iMdlCnt = 0, iEmpCnt = 0, iPickCnt = 0;

		//ybs
		iMdlCnt = g_site.GetModuleCnt();

		if(iBin1 > -1)
		{
			if(g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin1).GetModuleCnt();
			}
		}
		else if(iBin1_1 > -1)
		{
			if(g_Tray.GetTray(iBin1_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin1_1).GetModuleCnt();
			}
		}
		if(iBin2 > -1)
		{
			if(g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin2).GetModuleCnt();
			}
		}
		else if(iBin2_1 > -1)
		{
			if(g_Tray.GetTray(iBin2_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin2_1).GetModuleCnt();
			}
		}
		if(iBin3 > -1)
		{
			if(g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin3).GetModuleCnt();
			}
		}
		else if(iBin3_1 > -1)
		{
			if(g_Tray.GetTray(iBin3_1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
			{
				iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y) - g_Tray.GetTray(iBin3_1).GetModuleCnt();
			}
		}
// 		if(g_Tray.GetIdxByTargetPos(TPOS_BIN1) > -1)
// 		{
// 			iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y);
// 		}
// 		if(g_Tray.GetIdxByTargetPos(TPOS_BIN2) > -1)
// 		{
// 			iEmpCnt = iEmpCnt + (st_basic.n_tray_x * st_basic.n_tray_y);
// 		}
		BOOL bRun = CTL_NO;
		iPickCnt = g_robot.GetModuleCnt();
		if(g_handler.GetMachinePos() == EMPOS_FRONT)
		{
			//2013,1217
//			if( st_handler.mn_emptyin_cnt == 0 && ((iMdlCnt + iPickCnt) > iEmpCnt))
			if( st_handler.mn_emptyin_cnt < st_basic.mn_empty_tray_out && ((iMdlCnt + iPickCnt) > iEmpCnt))
			{
				bRun = CTL_YES;
			}
		}
		else
		{
			if( (iMdlCnt + iPickCnt) > iEmpCnt)
			{
				bRun = CTL_YES;
			}
		}
		if(bRun == CTL_YES)
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "MdlCnt:%d,iPickCnt:%d,iEmpCnt:%d",iMdlCnt,iPickCnt,iEmpCnt);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			if( IsEnablePos( TPOS_BIN1 ) )
			{
				st_handler.mn_emptyin_cnt++;
				if (st_handler.cwnd_title != NULL)	
				{
					st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
				}
				return TPOS_BIN1;
			}
			
			if( IsEnablePos( TPOS_BIN2 ) )
			{
				st_handler.mn_emptyin_cnt++;
				if (st_handler.cwnd_title != NULL)	
				{
					st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
				}
				return TPOS_BIN2;
			}
			
			//if(g_handler.GetMachinePos() == EMPOS_REAR)
			//{
				if( IsEnablePos( TPOS_BIN3 ) )
				{
					st_handler.mn_emptyin_cnt++;
					if (st_handler.cwnd_title != NULL)	
					{
						st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
					}
					return TPOS_BIN3;
				}
			//}

			st_handler.mn_emptynext_cnt++;
			if( st_handler.cwnd_list )
			{
				sprintf(st_msg.c_normal_msg, "[CalcTargetPos1]emptynext_cnt:[%d]", st_handler.mn_emptynext_cnt);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}
			return TPOS_NEXT;

		}
		st_handler.mn_emptynext_cnt++;
		if( st_handler.cwnd_list )
		{
			sprintf(st_msg.c_normal_msg, "[CalcTargetPos2]emptynext_cnt:[%d]", st_handler.mn_emptynext_cnt);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		return TPOS_NEXT;
	}

	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		if( st_basic.mn_bypass_mode == BYPASS_FRONT ||
			st_basic.mn_bypass_mode == BYPASS_ALL )
		{
// 			CString strNextData;
// 			strNextData = _T("FRONT_BYPASS->TOPS_NEXT");	
//  			Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);
			
			return TPOS_NEXT;
		}
	}
	else if( g_handler.GetMachinePos() == EMPOS_REAR )
	{
		if( tray.GetBypass() )
		{
// 			CString strNextData;
// 			strNextData = _T("REAR_BYPASS->TOPS_NEXT");	
//  			Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);
			return TPOS_NEXT;
		}
	}

	// 굳 이면, 다음...
	if( tray.IsAllState( EMS_GOOD ) )
	{
// 		CString strNextData;
// 		strNextData = _T("AllState_GOOD->TOPS_NEXT");	
//  		Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);
		
		return TPOS_NEXT;
	}
	
	// 실패면 BIN3
	
	if( tray.IsAllState( EMS_FAIL ) )
	{
// 		CString strNextData;
// 		strNextData = _T("AllState_FAIL->TOPS_NEXT");	
//  		Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);
		return TPOS_NEXT;
	}

	// 앞에 하나라도 있고, 뒤에 하나도 없으면.
	// 넥스트로 가는 로드도 없어야한다.
//	if( g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode != BYPASS_REAR &&
//		(IsEnablePos( TPOS_BIN1 ) == false || IsEnablePos( TPOS_BIN2 ) == false) &&
//		( g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) < 0 && g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) < 0 &&
//		  g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) < 0 && g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) < 0) )
//ybs
//	if( g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode != BYPASS_REAR &&
//		(IsEnablePos( TPOS_BIN1 ) == false || IsEnablePos( TPOS_BIN2 ) == false || IsEnablePos( TPOS_BIN3 ) == false) &&
//		( g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) < 0 && g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) < 0 && g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) < 0 &&
//		  g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) < 0 && g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) < 0 && g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) < 0) )
//	{
//		bool bSendNext = false;
//		for( int j=0; j<MAX_TRAY; j++ )
//		{
//			if( g_Tray.GetTray(j).GetPos() == TPOS_NONE )
//				continue;
//			
//			if( g_Tray.GetTray(j).IsAllState( EMS_LOAD ) && g_Tray.GetTray(j).GetTargetPos() == TPOS_NEXT )
//			{
//				bSendNext = true;
//				break;
//			}
//		}
//
//		if( bSendNext == false )
//			return TPOS_NEXT;
//	}
	//2012,1109 ybs
	
	int front = 0, rear = 0;
	
	if(IsEnablePos( TPOS_BIN1 ) == false) front += 1;
	if(IsEnablePos( TPOS_BIN2 ) == false) front += 1;
	if(IsEnablePos( TPOS_BIN3 ) == false) front += 1;
	//2013,1027
	if(IsEnablePos( TPOS_REJECT_TO_BIN ) == false) front += 1;

// 	if(IsEnablePos( TPOS_NEXT ) == false) rear += 1;

	if(g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1) rear += 1;
	if(g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1) rear += 1;
	if(g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1) rear += 1;
	//2013,1027
	if(g_Tray.GetIdxByPos_Extra( TPOS_REJECT_TO_BIN ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_REJECT_TO_BIN ) > -1) rear += 1;
	
	if(g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode != BYPASS_REAR &&
		((front >= 1 && rear == 0) || (front >= 2 && rear < 2) || (front == 3 && rear < 3)))
	{
		bool bSendNext = false;
		for( int j=0; j<MAX_TRAY; j++ )
		{
			if( g_Tray.GetTray(j).GetPos() == TPOS_NONE )
				continue;
			
			if( g_Tray.GetTray(j).IsAllState( EMS_LOAD ) && g_Tray.GetTray(j).GetTargetPos() == TPOS_NEXT )
			{
				bSendNext = true;
				break;
			}
		}

		if( bSendNext == false )
		{
// 			CString strNextData;
// 			strNextData = _T("SendNext->TOPS_NEXT");	
//  			Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);
			
			return TPOS_NEXT;
		}
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( IsEnablePos( TPOS_BIN3 ) )
			return TPOS_BIN3;
		
		if( IsEnablePos( TPOS_BIN2 ) )
			return TPOS_BIN2;
		
		if(g_handler.GetMachinePos() == EMPOS_FRONT)
		{
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES")
			{
//				if( IsEnablePos( TPOS_BIN1 ) )
//					return TPOS_BIN1;
				//2013,1027
				if( IsEnablePos( TPOS_BIN1 ) && IsEnablePos(TPOS_REJECT_TO_BIN) )
					return TPOS_BIN1;
			}
		}
		else
		{
//			if( IsEnablePos( TPOS_BIN1 ) )
//				return TPOS_BIN1;
			//2013,1027
			if( IsEnablePos( TPOS_BIN1 ) && IsEnablePos(TPOS_REJECT_TO_BIN) )
				return TPOS_BIN1;
		}
	}
	else
	{
		if( IsEnablePos( TPOS_BIN1 ) )
			return TPOS_BIN1;

		if( IsEnablePos( TPOS_BIN2 ) )
			return TPOS_BIN2;

		if(g_handler.GetMachinePos() == EMPOS_FRONT)
		{
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES")
			{
//				if( IsEnablePos( TPOS_BIN3 ) )
//					return TPOS_BIN3;
				//2013,1027
				if( IsEnablePos( TPOS_BIN3 ) && IsEnablePos(TPOS_REJECT_TO_BIN) )
					return TPOS_BIN3;
			}
		}
		else
		{
//			if( IsEnablePos( TPOS_BIN3 ) )
//				return TPOS_BIN3;
			//2013,1027
			if( IsEnablePos( TPOS_BIN3 ) && IsEnablePos(TPOS_REJECT_TO_BIN) )
				return TPOS_BIN3;
		}
	}

// 	CString strNextData;
// 	strNextData = _T("TOPS_NEXT");	
//  	Func.On_Log_Title_Msg(LOG_TOTAL, "[CalcTargetPos] %s", strNextData);

	// 갈 곳이 없군..
	return TPOS_NEXT;

}

bool APartTray::IsEnablePos( ETRAY_POS epos)
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == epos || 
			m_tray[i].GetTargetPos() == epos )
		{
			return false;
		}
	}

	return true;
}

void APartTray::CheckAfterTrayMoved( int iIdx, ETRAY_POS posFrom, ETRAY_POS posTo )
{
	// Reject
	bool bChanged = false;
	if( posTo == TPOS_REJECT )
	{
		m_vecRej.push_back( iIdx );
		bChanged = true;
	}
	else if( posFrom == TPOS_REJECT )
	{
		std::vector<int>::iterator it = m_vecRej.begin();
		for( int i=0; i<m_vecRej.size(); i++ )
		{
			if( m_vecRej.at(i) == iIdx )
			{
				it += i;
				break;
			}
		}

		m_vecRej.erase(it);
		bChanged = true;
	}

	if( bChanged )
	{
		m_nRejSelected = m_vecRej.size() - 1;

		if( st_handler.cwnd_main )
		{
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_REJ_FLOOR );
		}
	}

	if( iIdx == g_Tray.GetSelectedTray() )
	{
		if( st_handler.cwnd_main )
		{
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_SEL_TRAY );
		}	
	}

	if( posFrom == TPOS_BIN1 || posTo == TPOS_BIN1 )
	{
		if( st_handler.cwnd_main )	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_0 );
	}

	if( posFrom == TPOS_BIN2 || posTo == TPOS_BIN2 )
	{
		if( st_handler.cwnd_main )	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_1 );
	}

	//ybs
	if( posFrom == TPOS_BIN3 || posTo == TPOS_BIN3 )
	{
		if( st_handler.cwnd_main )	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_2 );
	}
}

int APartTray::GetRejSelectedTray()
{
	if( m_nRejSelected < 0 || m_nRejSelected >= m_vecRej.size() )
		return -1;

	return m_vecRej[m_nRejSelected];
}

void APartTray::SetRejSelected( int iSel )
{
	if( iSel < 0 || iSel >= m_vecRej.size() )
		return;

	m_nRejSelected = iSel;
}

int APartTray::GetIdxByPos( ETRAY_POS epos )
{
	if( epos == TPOS_REJECT ) //ybs
	{
		int nRejCnt = m_vecRej.size();
		if( nRejCnt == 0 )
			return -1;

		return m_vecRej.at( nRejCnt - 1 );
	}
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == epos )
		{
			return i;
		}
	}

	return -1;
}

std::vector<int> APartTray::GetVecMustTestModule( int nLotIdx, int nExchange1, int nExchange2 )
{
	std::vector<int> vecRtn;
	vecRtn.clear();


	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub = TPOS_NONE;
	ETRAY_POS ePosThird = TPOS_NONE;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE) ePosThird = TPOS_BIN1;
					else                                                  ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
					{ 
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1) ePosThird = TPOS_BIN1;
					else           ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1)
					{
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}					

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;
	int iIndexBinThird = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN1 )	    iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )		iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN1 )		iIndexBinSub = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN2 )	    iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosThird == TPOS_BIN1 )	iIndexBinThird = MPOS_INDEX_BIN1;
	if( ePosThird == TPOS_BIN2 )	iIndexBinThird = MPOS_INDEX_BIN2;


	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )//2014,0522
		return vecRtn;

	ATray trayPri = g_Tray.GetTray( iIdx );

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

	if( trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))
	{
		std::multimap<int, int> mapCntByX;
		//2013,0306
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			for( ix=x-1; ix>=0; ix-- )
			{
				int nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					//if( trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
					if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() &&
						iIndexBinPri + nTarget != nExchange1 && //2014,1027
						iIndexBinPri + nTarget != nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int>( -nCnt, ix ) );
			}
		}
		else
		{
			for( ix=0; ix<x; ix++ )
			{
				int nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					//if( trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
					if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() &&
						iIndexBinPri + nTarget != nExchange1 && //2014,1027
						iIndexBinPri + nTarget != nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int>( -nCnt, ix ) );
			}
		}

		std::multimap<int,int>::iterator it = mapCntByX.begin();

		for( it; it != mapCntByX.end(); it++ )
		{
			for( iy=0; iy<y; iy++ )
			{
				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
				//if( trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
				if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() &&
					iIndexBinPri + nTarget != nExchange1 && //2014,1027
					iIndexBinPri + nTarget != nExchange2 )
	 				vecRtn.push_back( iIndexBinPri + nTarget );

			}
		}
	}

	bool bPosSub = true, bPosThird = true;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE )  bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}
	else
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE)   bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}
	

	if(bPosSub == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosSub );
		if( iIdx > -1 )
		{
			ATray traySub = g_Tray.GetTray( iIdx );
			if( traySub.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))
			{
				std::multimap<int,int> mapCntByX;
				int nCnt=0;
				//2013,0306
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							//if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
							if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() &&
								iIndexBinSub + nTarget != nExchange1 && //2014,1027
								iIndexBinSub + nTarget != nExchange2 )
								nCnt++;
						}
						mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							//if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
							if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() &&
								iIndexBinSub + nTarget != nExchange1 && //2014,1027
								iIndexBinSub + nTarget != nExchange2 )
								nCnt++;
						}
						mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
					}
				}

				std::multimap<int,int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						//if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
						if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() &&
							iIndexBinSub + nTarget != nExchange1 && //2014,1027
							iIndexBinSub + nTarget != nExchange2 )
							vecRtn.push_back( iIndexBinSub + nTarget );
					}
				}
			}
		}
	}

	if(bPosThird == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosThird );
		if( iIdx > -1 )
		{
			ATray trayThird = g_Tray.GetTray( iIdx );
			if( trayThird.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))
			{
				std::multimap<int,int> mapCntByX;
				int nCnt=0;
				//2013,0306
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							//if( trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() )
							if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() &&
								iIndexBinThird + nTarget != nExchange1 && //2014,1027
								iIndexBinThird + nTarget != nExchange2 )
								nCnt++;
						}
						mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							//if( trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() )
							if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() &&
								iIndexBinThird + nTarget != nExchange1 && //2014,1027
								iIndexBinThird + nTarget != nExchange2 )
								nCnt++;
						}
						mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
					}
				}
				
				std::multimap<int,int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						//if( trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() )
						if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() && trayThird.GetModule(nTarget).IsTestLoadState() &&
							iIndexBinThird + nTarget != nExchange1 && //2014,1027
							iIndexBinThird + nTarget != nExchange2 )
							vecRtn.push_back( iIndexBinThird + nTarget );
					}
				}
			}
		}
	}

	return vecRtn;
}

std::vector<int> APartTray::GetVecStacker3MustTestModule( int nLotIdx )
{
	std::vector<int> vecRtn;
	vecRtn.clear();


	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(iBin1 < 0 ) return vecRtn;
	}
	else
	{
		if(iBin3 < 0 ) return vecRtn;
	}

	//트레이가 비어 있을 수도 있으나, site가 비어있을 수 있더
	//트레이 3군데를 전부 봐야 한다.
	//Stacker3번째를 다른 곳(Site 또는 Tray)으로 옮긴뒤에 처리한다,
	int ix=0, iy=0;
	ETRAY_POS ePosPri;
	ETRAY_POS ePosSub1,ePosSub2;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		ePosPri = TPOS_BIN1;
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub1 = TPOS_BIN2;
				if(iBin3 > -1) ePosSub2 = TPOS_BIN3;
				else           ePosSub2 = TPOS_NONE;
			}
			else
			{
				if(iBin3 > -1) ePosSub1 = TPOS_BIN3;
				else
				{
					ePosSub1 = TPOS_NONE;
					ePosSub2 = TPOS_NONE;
				}
			}
		}
	}
	else
	{
		ePosPri = TPOS_BIN3;
		if(ePosPri == TPOS_BIN3)
		{
			if(iBin1 > -1)
			{
				ePosSub1 = TPOS_BIN1;
				if(iBin2 > -1) ePosSub2 = TPOS_BIN2;
				else           ePosSub2 = TPOS_NONE;
			}
			else
			{
				if(iBin2 > -1) ePosSub1 = TPOS_BIN2;
				else
				{
					ePosSub1 = TPOS_NONE;
					ePosSub2 = TPOS_NONE;
				}
			}
		}
	}

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub1 = MPOS_INDEX_BIN3;
	int iIndexBinSub2 = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN1 )	    iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )		iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub1 == TPOS_BIN1 )		iIndexBinSub1 = MPOS_INDEX_BIN1;
	if( ePosSub1 == TPOS_BIN2 )	    iIndexBinSub1 = MPOS_INDEX_BIN2;
	if( ePosSub2 == TPOS_BIN1 )		iIndexBinSub2 = MPOS_INDEX_BIN1;
	if( ePosSub2 == TPOS_BIN2 )	    iIndexBinSub2 = MPOS_INDEX_BIN2;


	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );

	//////////////////////////////////////////////////////////////////
	//2017.0102
	if( iIdx < 0 ) return vecRtn;
	//////////////////////////////////////////////////////////////////

	ATray trayPri = g_Tray.GetTray( iIdx );

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

	if( trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))//아니면 문제가 된다. 미리 다 막았지만
	{
		std::multimap<int, int> mapCntByX;
		int nCnt = 0;
		//2013,0306
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			for( ix=x-1; ix>=0; ix-- )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >=0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int>( -nCnt, ix ) );
			}
		}
		else
		{
			for( ix=0; ix<x; ix++ )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >=0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int>( -nCnt, ix ) );
			}
		}

		std::multimap<int,int>::iterator it = mapCntByX.begin();

		for( it; it != mapCntByX.end(); it++ )
		{
			for( iy=0; iy<y; iy++ )
			{
				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
				if( nTarget >=0 && trayPri.GetModule(nTarget).IsExist() && trayPri.GetModule(nTarget).IsTestLoadState() )
	 				vecRtn.push_back( iIndexBinPri + nTarget );

			}
		}
	}	
	
// 	iIdx = g_Tray.GetIdxByPos( ePosSub1 );
// 	if( iIdx > -1 )
// 	{
// 		ATray traySub = g_Tray.GetTray( iIdx );
// 		if( traySub.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))
// 		{
// 			std::multimap<int,int> mapCntByX;
// 			for( ix=0; ix<x; ix++ )
// 			{
// 				int nCnt = 0;
// 				for( iy=0; iy<y; iy++ )
// 				{
// 					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
// 					if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
// 						nCnt++;
// 				}
// 				mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
// 			}
// 
// 			std::multimap<int,int>::iterator it = mapCntByX.begin();
// 			for( it; it != mapCntByX.end(); it++ )
// 			{
// 				for( iy=0; iy<y; iy++ )
// 				{
// 					int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
// 					if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
// 						vecRtn.push_back( iIndexBinSub1 + nTarget );
// 				}
// 			}
// 		}		
// 	}
// 
// 	iIdx = g_Tray.GetIdxByPos( ePosSub2 );
// 	if( iIdx > -1 )
// 	{
// 		ATray traySub = g_Tray.GetTray( iIdx );
// 		if( traySub.GetStrLotID() == g_lotMgr.GetLotIDAt( nLotIdx ))
// 		{
// 			std::multimap<int,int> mapCntByX;
// 			for( ix=0; ix<x; ix++ )
// 			{
// 				int nCnt = 0;
// 				for( iy=0; iy<y; iy++ )
// 				{
// 					int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
// 					if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
// 						nCnt++;
// 				}
// 				mapCntByX.insert( std::pair<int,int>(-nCnt, ix ) );
// 			}
// 			
// 			std::multimap<int,int>::iterator it = mapCntByX.begin();
// 			for( it; it != mapCntByX.end(); it++ )
// 			{
// 				for( iy=0; iy<y; iy++ )
// 				{
// 					int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
// 					if( traySub.GetModule(nTarget).IsExist() && traySub.GetModule(nTarget).IsTestLoadState() )
// 						vecRtn.push_back( iIndexBinSub2 + nTarget );
// 				}
// 			}
// 		}		
// 	}

	return vecRtn;
}

std::vector<int> APartTray::GetVecEmptyCell( int nLotIdx, int nExchange1, int nExchange2 )
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;
/*
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_NONE;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}

	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_BIN2;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}
	}
//	int iIndexBinPri = MPOS_INDEX_BIN1;
//	int iIndexBinSub = MPOS_INDEX_BIN1;
//	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
//	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN1 )	iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )	iIndexBinSub = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;

	if(g_Tray.GetIdxByPos( ePosPri ) == -1)
	{
		int a = 100;
		ePosPri = TPOS_BIN3;
	}

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )
		return vecRtn;

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

	ATray trayPri = g_Tray.GetTray( iIdx );
	CString strLotno = g_lotMgr.GetLotIDAt(nLotIdx); //ybs

	if( trayPri.GetTargetPos() == ePosPri &&
		trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx))
	{
		std::multimap<int, int> mapCntByX;
		int nCnt=0,nTarget=0;
		//2013,0306
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			for( ix=x-1; ix>=0; ix-- )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( trayPri.GetModule(nTarget).IsExist() == false || 
						iIndexBinPri + nTarget == nExchange1 ||
						iIndexBinPri + nTarget == nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}
		else
		{
			for( ix=0; ix<x; ix++ )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( trayPri.GetModule(nTarget).IsExist() == false || 
						iIndexBinPri + nTarget == nExchange1 ||
						iIndexBinPri + nTarget == nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}

		std::multimap<int, int>::iterator it = mapCntByX.begin();
		for( it; it != mapCntByX.end(); it++ )
		{
			for( int iy=0; iy<y; iy++ )
			{
				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
				if( trayPri.GetModule(nTarget).IsExist() == false || 
				iIndexBinPri + nTarget == nExchange1 ||
				iIndexBinPri + nTarget == nExchange2 )
					vecRtn.push_back( iIndexBinPri + nTarget );
			}
		}
	}

	iIdx = g_Tray.GetIdxByPos( ePosSub );
	if( iIdx > -1 )
	{
		ATray traySub = g_Tray.GetTray( iIdx );

		if( traySub.GetTargetPos() == ePosSub &&
			traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
		{
			std::multimap<int, int> mapCntByX;
			//2013,0306
			int nCnt=0,nTarget=0;
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				for( ix=x-1; ix>=0; ix-- )
				{
					nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( traySub.GetModule(nTarget).IsExist() == false ||
							iIndexBinSub + nTarget == nExchange1 ||
							iIndexBinSub + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			else
			{
				for( ix=0; ix<x; ix++ )
				{
					int nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( traySub.GetModule(nTarget).IsExist() == false ||
							iIndexBinSub + nTarget == nExchange1 ||
							iIndexBinSub + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			
			std::multimap<int, int>::iterator it = mapCntByX.begin();
			for( it; it != mapCntByX.end(); it++ )
			{
				for( int iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
					if( traySub.GetModule(nTarget).IsExist() == false ||
					iIndexBinSub + nTarget == nExchange1 ||
					iIndexBinSub + nTarget == nExchange2 )
						vecRtn.push_back( iIndexBinSub + nTarget );
				}
			}
		}
		
	}
	*/
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub = TPOS_NONE;
	ETRAY_POS ePosThird = TPOS_NONE;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE) ePosThird = TPOS_BIN1;
					else                                                  ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
					{ 
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}

	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1) ePosThird = TPOS_BIN1;
					else           ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1)
					{
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}
//	int iIndexBinPri = MPOS_INDEX_BIN1;
//	int iIndexBinSub = MPOS_INDEX_BIN1;
//	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
//	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;
	int iIndexBinThird = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN1 )		iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )		iIndexBinSub = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )		iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )		iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosThird == TPOS_BIN1 )	iIndexBinThird = MPOS_INDEX_BIN1;
	if( ePosThird == TPOS_BIN2 )	iIndexBinThird = MPOS_INDEX_BIN2;

// 	if(g_Tray.GetIdxByPos( ePosPri ) == -1)
// 	{
// 		int a = 100;
// 		ePosPri = TPOS_BIN3;
// 	}

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )
		return vecRtn;

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

	ATray trayPri = g_Tray.GetTray( iIdx );
	CString strLotno = g_lotMgr.GetLotIDAt(nLotIdx); //ybs

	if( trayPri.GetTargetPos() == ePosPri &&
		trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx))
	{
		std::multimap<int, int> mapCntByX;
		int nCnt=0,nTarget=0;
		//2013,0306
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			for( ix=x-1; ix>=0; ix-- )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >= 0 && ( trayPri.GetModule(nTarget).IsExist() == false || 
						iIndexBinPri + nTarget == nExchange1 ||
						iIndexBinPri + nTarget == nExchange2 ) )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}
		else
		{
			for( ix=0; ix<x; ix++ )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >= 0 && ( trayPri.GetModule(nTarget).IsExist() == false || 
						iIndexBinPri + nTarget == nExchange1 ||
						iIndexBinPri + nTarget == nExchange2 ) )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}

		std::multimap<int, int>::iterator it = mapCntByX.begin();
		for( it; it != mapCntByX.end(); it++ )
		{
			for( int iy=0; iy<y; iy++ )
			{
				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
				if( nTarget >= 0 && ( trayPri.GetModule(nTarget).IsExist() == false || 
					iIndexBinPri + nTarget == nExchange1 ||
					iIndexBinPri + nTarget == nExchange2 ) )
					vecRtn.push_back( iIndexBinPri + nTarget );
			}
		}
	}

	bool bPosSub = true, bPosThird = true;
	
// 	if(st_handler.mn_mirror_type == CTL_YES)
// 	{
// 		if( ePosSub == TPOS_NONE || ((ePosSub == TPOS_BIN1 || ePosPri == TPOS_BIN1)&& st_handler.mb_rej_3stacker == TRUE) )			bPosSub = false;
// 		if( ePosThird == TPOS_NONE || ((ePosThird == TPOS_BIN1 || ePosThird == TPOS_BIN1)&& st_handler.mb_rej_3stacker == TRUE) )  	bPosThird = false;
// 	}
// 	else
// 	{
// 		if( ePosSub == TPOS_NONE || ((ePosSub == TPOS_BIN3 || ePosPri == TPOS_BIN3)&& st_handler.mb_rej_3stacker == TRUE) )			bPosSub = false;
// 		if( ePosThird == TPOS_NONE || ((ePosThird == TPOS_BIN3 || ePosThird == TPOS_BIN3)&& st_handler.mb_rej_3stacker == TRUE) )  	bPosThird = false;
// 	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE )  bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}
	else
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE)   bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}


	if(bPosSub == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosSub );
		if( iIdx > -1 )
		{
			ATray traySub = g_Tray.GetTray( iIdx );

			if( traySub.GetTargetPos() == ePosSub &&
				traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
			{
				std::multimap<int, int> mapCntByX;
				//2013,0306
				int nCnt=0,nTarget=0;
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && ( traySub.GetModule(nTarget).IsExist() == false ||
								iIndexBinSub + nTarget == nExchange1 ||
								iIndexBinSub + nTarget == nExchange2 ) )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						int nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && ( traySub.GetModule(nTarget).IsExist() == false ||
								iIndexBinSub + nTarget == nExchange1 ||
								iIndexBinSub + nTarget == nExchange2 ) )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				
				std::multimap<int, int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( int iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						if( nTarget >= 0 && ( traySub.GetModule(nTarget).IsExist() == false ||
						iIndexBinSub + nTarget == nExchange1 ||
						iIndexBinSub + nTarget == nExchange2 ) )
							vecRtn.push_back( iIndexBinSub + nTarget );
					}
				}
			}			
		}
	}

	if(bPosThird == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosThird );
		if( iIdx > -1 )
		{
			ATray trayThird = g_Tray.GetTray( iIdx );
			
			if( trayThird.GetTargetPos() == ePosThird &&
				trayThird.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
			{
				std::multimap<int, int> mapCntByX;
				//2013,0306
				int nCnt=0,nTarget=0;
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && ( trayThird.GetModule(nTarget).IsExist() == false ||
								iIndexBinThird + nTarget == nExchange1 ||
								iIndexBinThird + nTarget == nExchange2 ) )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						int nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && ( trayThird.GetModule(nTarget).IsExist() == false ||
								iIndexBinThird + nTarget == nExchange1 ||
								iIndexBinThird + nTarget == nExchange2 ) )
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				
				std::multimap<int, int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( int iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						if( nTarget >= 0 && ( trayThird.GetModule(nTarget).IsExist() == false ||
							iIndexBinThird + nTarget == nExchange1 ||
							iIndexBinThird + nTarget == nExchange2 ) )
							vecRtn.push_back( iIndexBinThird + nTarget );
					}
				}
			}			
		}
	}

	return vecRtn;
}

std::vector<int> APartTray::GetVecPutEmptyCell( int nLotIdx, int nExchange1, int nExchange2 )
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub = TPOS_NONE;
	ETRAY_POS ePosThird = TPOS_NONE;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE) ePosThird = TPOS_BIN1;
					else                                                  ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
					{ 
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}

	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1) ePosThird = TPOS_BIN1;
					else           ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1)
					{
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}
//	int iIndexBinPri = MPOS_INDEX_BIN1;
//	int iIndexBinSub = MPOS_INDEX_BIN1;
//	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
//	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;
	int iIndexBinThird = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN1 )		iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )		iIndexBinSub = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )		iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )		iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosThird == TPOS_BIN1 )	iIndexBinThird = MPOS_INDEX_BIN1;
	if( ePosThird == TPOS_BIN2 )	iIndexBinThird = MPOS_INDEX_BIN2;

// 	if(g_Tray.GetIdxByPos( ePosPri ) == -1)
// 	{
// 		int a = 100;
// 		ePosPri = TPOS_BIN3;
// 	}

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )
		return vecRtn;

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

	ATray trayPri = g_Tray.GetTray( iIdx );
	CString strLotno = g_lotMgr.GetLotIDAt(nLotIdx); //ybs

	if( trayPri.GetTargetPos() == ePosPri &&
		trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx))
	{
		std::multimap<int, int> mapCntByX;
		int nCnt=0,nTarget=0;
		//2013,0306
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			for( ix=x-1; ix>=0; ix-- )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() == false && 
						iIndexBinPri + nTarget != nExchange1 &&
						iIndexBinPri + nTarget != nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}
		else
		{
			for( ix=0; ix<x; ix++ )
			{
				nCnt = 0;
				for( iy=0; iy<y; iy++ )
				{
					nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
					if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() == false && 
						iIndexBinPri + nTarget != nExchange1 &&
						iIndexBinPri + nTarget != nExchange2 )
						nCnt++;
				}
				mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
			}
		}

		std::multimap<int, int>::iterator it = mapCntByX.begin();
		for( it; it != mapCntByX.end(); it++ )
		{
			for( int iy=0; iy<y; iy++ )
			{
				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
				if( nTarget >= 0 && trayPri.GetModule(nTarget).IsExist() == false && 
				iIndexBinPri + nTarget != nExchange1 &&
				iIndexBinPri + nTarget != nExchange2 )
					vecRtn.push_back( iIndexBinPri + nTarget );
			}
		}
	}

	bool bPosSub = true, bPosThird = true;
	

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE )  bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}
	else
	{
		if( ePosSub == TPOS_NONE )										  bPosSub = false;
		if( ePosSub == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE)   bPosSub = false;
		if( ePosThird == TPOS_NONE )  									  bPosThird = false;
		if( ePosThird == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE) bPosThird = false;
	}


	if(bPosSub == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosSub );
		if( iIdx > -1 )
		{
			ATray traySub = g_Tray.GetTray( iIdx );

			if( traySub.GetTargetPos() == ePosSub &&
				traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
			{
				std::multimap<int, int> mapCntByX;
				//2013,0306
				int nCnt=0,nTarget=0;
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() == false &&
								iIndexBinSub + nTarget != nExchange1 &&
								iIndexBinSub + nTarget != nExchange2 )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						int nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() == false &&
								iIndexBinSub + nTarget != nExchange1 &&
								iIndexBinSub + nTarget != nExchange2 )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				
				std::multimap<int, int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( int iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						if( nTarget >= 0 && traySub.GetModule(nTarget).IsExist() == false &&
						iIndexBinSub + nTarget != nExchange1 &&
						iIndexBinSub + nTarget != nExchange2 )
							vecRtn.push_back( iIndexBinSub + nTarget );
					}
				}
			}			
		}
	}

	if(bPosThird == true)
	{
		iIdx = g_Tray.GetIdxByPos( ePosThird );
		if( iIdx > -1 )
		{
			ATray trayThird = g_Tray.GetTray( iIdx );
			
			if( trayThird.GetTargetPos() == ePosThird &&
				trayThird.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
			{
				std::multimap<int, int> mapCntByX;
				//2013,0306
				int nCnt=0,nTarget=0;
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					for( ix=x-1; ix>=0; ix-- )
					{
						nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() == false &&
								iIndexBinThird + nTarget != nExchange1 &&
								iIndexBinThird + nTarget != nExchange2 )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				else
				{
					for( ix=0; ix<x; ix++ )
					{
						int nCnt = 0;
						for( iy=0; iy<y; iy++ )
						{
							int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
							if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() == false &&
								iIndexBinThird + nTarget != nExchange1 &&
								iIndexBinThird + nTarget != nExchange2 )	
								nCnt++;
						}
						mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
					}
				}
				
				std::multimap<int, int>::iterator it = mapCntByX.begin();
				for( it; it != mapCntByX.end(); it++ )
				{
					for( int iy=0; iy<y; iy++ )
					{
						int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
						if( nTarget >= 0 && trayThird.GetModule(nTarget).IsExist() == false &&
							iIndexBinThird + nTarget != nExchange1 &&
							iIndexBinThird + nTarget != nExchange2 )
							vecRtn.push_back( iIndexBinThird + nTarget );
					}
				}
			}			
		}
	}

	return vecRtn;
}

std::vector<int> APartTray::GetVecStacker3EmptyCell( int nLotIdx, int nExchange1, int nExchange2 )
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri;
	ETRAY_POS ePosSub1,ePosSub2;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		ePosPri = TPOS_BIN1;
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub1 = TPOS_BIN2;
				if(iBin3 > -1) ePosSub2 = TPOS_BIN3;
				else           ePosSub2 = TPOS_NONE;
			}
			else
			{
				if(iBin3 > -1) ePosSub1 = TPOS_BIN3;
				else
				{
					ePosSub1 = TPOS_NONE;
					ePosSub2 = TPOS_NONE;
				}
			}
		}
	}
	else
	{
		ePosPri = TPOS_BIN3;
		if(ePosPri == TPOS_BIN3)
		{
			if(iBin1 > -1)
			{
				ePosSub1 = TPOS_BIN1;
				if(iBin2 > -1) ePosSub2 = TPOS_BIN2;
				else           ePosSub2 = TPOS_NONE;
			}
			else
			{
				if(iBin2 > -1) ePosSub1 = TPOS_BIN2;
				else
				{
					ePosSub1 = TPOS_NONE;
					ePosSub2 = TPOS_NONE;
				}
			}
		}
	}

	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub1 = MPOS_INDEX_BIN3;
	int iIndexBinSub2 = MPOS_INDEX_BIN3;
	
	if( ePosPri == TPOS_BIN1 )	    iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosPri == TPOS_BIN2 )		iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub1 == TPOS_BIN1 )		iIndexBinSub1 = MPOS_INDEX_BIN1;
	if( ePosSub1 == TPOS_BIN2 )	    iIndexBinSub1 = MPOS_INDEX_BIN2;
	if( ePosSub2 == TPOS_BIN1 )		iIndexBinSub2 = MPOS_INDEX_BIN1;
	if( ePosSub2 == TPOS_BIN2 )	    iIndexBinSub2 = MPOS_INDEX_BIN2;

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )
		return vecRtn;

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
	{
		x = st_basic.n_tray_y;
		y = st_basic.n_tray_x;
	}

// 	ATray trayPri = g_Tray.GetTray( iIdx );
// 	CString strLotno = g_lotMgr.GetLotIDAt(nLotIdx); //ybs
// 
// 	if( trayPri.GetTargetPos() == ePosPri &&
// 		trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx))
// 	{
// 		std::multimap<int, int> mapCntByX;
// 		for( int ix=0; ix<x; ix++ )
// 		{
// 			int nCnt = 0;
// 			for( int iy=0; iy<y; iy++ )
// 			{
// 				int nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
// 				if( trayPri.GetModule(nTarget).IsExist() == false || 
// 				iIndexBinPri + nTarget == nExchange1 ||
// 				iIndexBinPri + nTarget == nExchange2 )
// 					nCnt++;
// 			}
// 			mapCntByX.insert( std::pair<int,int> (-nCnt, ix ) );
// 		}
// 
// 		std::multimap<int, int>::iterator it = mapCntByX.begin();
// 		for( it; it != mapCntByX.end(); it++ )
// 		{
// 			for( int iy=0; iy<y; iy++ )
// 			{
// 				int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
// 				if( trayPri.GetModule(nTarget).IsExist() == false || 
// 				iIndexBinPri + nTarget == nExchange1 ||
// 				iIndexBinPri + nTarget == nExchange2 )
// 					vecRtn.push_back( iIndexBinPri + nTarget );
// 			}
// 		}
// 	}

	iIdx = g_Tray.GetIdxByPos( ePosSub1 );
	if( iIdx > -1 )
	{
		ATray traySub = g_Tray.GetTray( iIdx );

		if( traySub.GetTargetPos() == ePosSub1 &&
			traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
		{
			std::multimap<int, int> mapCntByX;
			int nCnt=0,nTarget=0;
			//2013,0306
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				for( ix=x-1; ix>=0; ix-- )
				{
					nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
							iIndexBinSub1 + nTarget == nExchange1 ||
							iIndexBinSub1 + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			else
			{
				for( ix=0; ix<x; ix++ )
				{
					nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
							iIndexBinSub1 + nTarget == nExchange1 ||
							iIndexBinSub1 + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			
			std::multimap<int, int>::iterator it = mapCntByX.begin();
			for( it; it != mapCntByX.end(); it++ )
			{
				for( int iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
					if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
					iIndexBinSub1 + nTarget == nExchange1 ||
					iIndexBinSub1 + nTarget == nExchange2 )
						vecRtn.push_back( iIndexBinSub1 + nTarget );
				}
			}
		}
		
	}
	iIdx = g_Tray.GetIdxByPos( ePosSub2 );
	if( iIdx > -1 )
	{
		ATray traySub = g_Tray.GetTray( iIdx );
		
		if( traySub.GetTargetPos() == ePosSub2 &&
			traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(nLotIdx) )
		{
			std::multimap<int, int> mapCntByX;
			int nCnt=0,nTarget=0;
			//2013,0306
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				for( ix=x-1; ix>=0; ix-- )
				{
					nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
							iIndexBinSub2 + nTarget == nExchange1 ||
							iIndexBinSub2 + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			else
			{
				for( ix=0; ix<x; ix++ )
				{
					nCnt = 0;
					for( iy=0; iy<y; iy++ )
					{
						nTarget = GetMdlIdx( ix, iy, x, y, true ) - 1;
						if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
							iIndexBinSub2 + nTarget == nExchange1 ||
							iIndexBinSub2 + nTarget == nExchange2 )	
							nCnt++;
					}
					mapCntByX.insert( std::pair<int, int>(-nCnt, ix ) );
				}
			}
			
			std::multimap<int, int>::iterator it = mapCntByX.begin();
			for( it; it != mapCntByX.end(); it++ )
			{
				for( int iy=0; iy<y; iy++ )
				{
					int nTarget = GetMdlIdx( it->second, iy, x, y, true ) - 1;
					if( (nTarget >=0 && traySub.GetModule(nTarget).IsExist() == false) ||
						iIndexBinSub2 + nTarget == nExchange1 ||
						iIndexBinSub2 + nTarget == nExchange2 )
						vecRtn.push_back( iIndexBinSub2 + nTarget );
				}
			}
		}
		
	}
	return vecRtn;
}

std::vector<int> APartTray::GetVecRejEmptyCell()
{

	std::vector<int> vecRtn;

	int nRejCnt = m_vecRej.size();
	if( nRejCnt == 0 )
	{
// 		CString strErr;
// 		strErr.Format( "리젝트 트레이를 넣어주세요" );
// 		st_msg.mstr_event_msg[0] = strErr;
// 		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
// 		Func.OnSet_IO_Port_Sound(IO_ON);
		return vecRtn;
	}

	int iIdx = m_vecRej.at( nRejCnt - 1);
	ATray tray = g_Tray.GetTray( iIdx );

	if( tray.GetStrLotID() != "" &&
		tray.GetStrLotID() != g_lotMgr.GetLotIDAt( 0 ) )
		return vecRtn;
	
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i<iMdlCnt; i++ )
	{
		if( tray.GetModule(i).IsExist() == false )
			vecRtn.push_back( MPOS_INDEX_REJ + i );
	}

	return vecRtn;
}

int APartTray::GetIdxByTargetPos( ETRAY_POS epos )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetTargetPos() == epos )
		{
			return i;
		}
	}

	return -1;
}

std::vector<int> APartTray::GetVecFailinTray()
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

/*	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_BIN2;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}
	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_BIN2;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}
	}
	
	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosPri == TPOS_BIN1 )	iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )	iIndexBinSub = MPOS_INDEX_BIN1;

// 	if(g_Tray.GetIdxByPos( ePosPri ) == -1)
// 	{
// 		int a = 100;
// 		ePosPri = TPOS_BIN1;
// 		g_handler.SetPriorityBin(TPOS_BIN1);
// 	}

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	ATray trayPri = g_Tray.GetTray( iIdx );
	
	for( int i=0; i<iMdlCnt; i++ )
	{
		if( trayPri.GetModule(i).IsExist() &&
			trayPri.GetModule(i).GetModuleState() == EMS_FAIL &&
			trayPri.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0) )
			vecRtn.push_back( iIndexBinPri + i );
	}

	iIdx = g_Tray.GetIdxByPos( ePosSub );
	if( iIdx > -1 )
	{
		ATray traySub = g_Tray.GetTray( iIdx );
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( traySub.GetModule(i).IsExist() &&
				traySub.GetModule(i).GetModuleState() == EMS_FAIL &&
				traySub.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0) )
				vecRtn.push_back( iIndexBinSub + i );
		}
	}*/
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub = TPOS_NONE;
	ETRAY_POS ePosThird = TPOS_NONE;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_NONE;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE) ePosThird = TPOS_BIN1;
					else                                                  ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == FALSE)
					{ 
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)
			{
				ePosSub = TPOS_BIN2;
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false) ePosThird = TPOS_BIN3;
				else           ePosThird = TPOS_NONE;
			}
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
			{
				ePosSub = TPOS_BIN3;
				ePosThird = TPOS_NONE;
			}
			else
			{
				ePosSub = TPOS_BIN2;
				ePosThird = TPOS_NONE;
			}
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)
				{
					ePosSub = TPOS_BIN3;
					if(iBin1 > -1) ePosThird = TPOS_BIN1;
					else           ePosThird = TPOS_NONE;
				}
				else if(iBin1 > -1)
				{
					ePosSub = TPOS_BIN1;
					ePosThird = TPOS_NONE;
				}
				else
				{
					ePosSub = TPOS_NONE;
					ePosThird = TPOS_NONE;
				}
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1)
					{
						ePosSub = TPOS_BIN1;
						if(iBin2 > -1) ePosThird = TPOS_BIN2;
						else           ePosThird = TPOS_NONE;
					}
					else
					{
						if(iBin2 > -1)
						{
							ePosSub = TPOS_BIN2;
							ePosThird = TPOS_NONE;
						}
						else
						{
							ePosSub = TPOS_NONE;
							ePosThird = TPOS_NONE;
						}
					}
				}
			}
		}
	}
	
	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;
	int iIndexBinThird = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosPri == TPOS_BIN1 )	iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )	iIndexBinSub = MPOS_INDEX_BIN1;
	if( ePosThird == TPOS_BIN1 )	iIndexBinThird = MPOS_INDEX_BIN1;
	if( ePosThird == TPOS_BIN2 )	iIndexBinThird = MPOS_INDEX_BIN2;

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx > -1 )
	{
		ATray trayPri = g_Tray.GetTray( iIdx );
		
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( trayPri.GetModule(i).IsExist() &&
				trayPri.GetModule(i).GetModuleState() == EMS_FAIL &&
				trayPri.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0) )
				vecRtn.push_back( iIndexBinPri + i );
		}
	}
	
	iIdx = g_Tray.GetIdxByPos( ePosSub );
	if( iIdx > -1 )
	{
		ATray traySub = g_Tray.GetTray( iIdx );
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( traySub.GetModule(i).IsExist() &&
				traySub.GetModule(i).GetModuleState() == EMS_FAIL &&
				traySub.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0) )
				vecRtn.push_back( iIndexBinSub + i );
		}
	}

	iIdx = g_Tray.GetIdxByPos( ePosThird );
	if( iIdx > -1 )
	{
		ATray trayThird = g_Tray.GetTray( iIdx );
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( trayThird.GetModule(i).IsExist() &&
				trayThird.GetModule(i).GetModuleState() == EMS_FAIL &&
				trayThird.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0) )
				vecRtn.push_back( iIndexBinThird + i );
		}
	}
	return vecRtn;
}

int APartTray::GetLoadModuleCnt()
{
	int nRtn = 0;
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;

	for( int it=0; it<MAX_TRAY; it++ )
	{
		ATray tray = g_Tray.GetTray( it );
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( tray.GetModule(i).IsExist() && tray.GetModule(i).IsTestLoadState() &&
				tray.GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
				nRtn++;
		}
	}
	
	return nRtn;
}

bool APartTray::CalcLastTray( int iIdx, CString strLotID )
{
// 	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int i=0,it=0;
	int iMdlCnt = g_site.GetModuleCnt();
	int iPickCnt = g_robot.GetModuleCnt();
	for( it=0; it<MAX_TRAY; it++ )
	{
		if( it == iIdx )						continue;

		ATray tray = g_Tray.GetTray( it );
		if( tray.GetPos() == TPOS_NONE )		continue;
		if( tray.GetStrLotID() != strLotID )	continue;
		if( tray.GetEmptyTray() )				continue;

		return false;
	}
//	if( iMdlCnt > 0)						return false;
	if( iMdlCnt > 0)
	{
		for( i=0; i<TSITE_SOCKET_CNT; i++ )
		{
			if(g_site.GetModule(i).IsExist())
			{
				if(g_site.GetModule(i).GetLotNo() == strLotID)
				{
					return false;
				}
			}
		}
	}
	if( iPickCnt > 0)
	{
		for( i=0; i<PICKER_CNT; i++ )
		{
			if(g_robot.GetPicker(i).GetModule().GetLotNo() == strLotID)
			{
				return false;
			}
		}
	}
	
// 	if (g_handler.GetMachinePos() == EMPOS_REAR)
// 	{
		for( it=0; it<MAX_TRAY; it++ )
		{
			if( it == iIdx )						continue;
			ATray tray = g_Tray.GetTray( it );
			if( tray.GetPos() == TPOS_NONE )		continue;
			if( tray.GetStrLotID() != strLotID )	continue;
			return false;
		}
// 	}


	return true;
}

int APartTray::GetTrayCnt_byLotID( CString strLotID )
{
	int nRtn = 0;
	for( int it=0; it<MAX_TRAY; it++ )
	{
		ATray tray = g_Tray.GetTray( it );
		if( tray.GetPos() == TPOS_NONE )		continue;
		
		if( tray.GetStrLotID() != strLotID )	continue;

		nRtn++;
	}

	return nRtn;
}

ATray& APartTray::GetRejTopTray()
{
	if( m_vecRej.size() == 0 )
		return m_tray[MAX_TRAY - 1];
	
	int nCnt = m_vecRej.size();
	return GetTray( m_vecRej[nCnt - 1] );
}

bool APartTray::IsWorkingRejToNext()
{
	for( int it=0; it<MAX_TRAY; it++ )
	{
		ATray tray = g_Tray.GetTray( it );
		if( ( tray.GetPos() == TPOS_REJECT ||
			tray.GetPos() == TPOS_REJECT_TO_BIN ||
			tray.GetPos() == TPOS_BIN3 ||
			tray.GetPos() == TPOS_BIN_TO_CONV3 ||
			tray.GetPos() == TPOS_CONV3 ) &&
			tray.GetTargetPos() == TPOS_NEXT )
		{
			return true;
		}
	}

	return false;
}

ETRAY_POS APartTray::GetPosByStr( CString strPos )
{
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( strPos == "FRONT" )						return TPOS_FRONT;
		else if( strPos == "CONV1" )				return TPOS_CONV1;
		else if( strPos == "CONV2" )				return TPOS_CONV2;
		else if( strPos == "CONV3" )				return TPOS_CONV3;
		else if( strPos == "NEXT" )					return TPOS_NEXT;
		else if( strPos == "FRONT_TO_CONV1" )		return TPOS_FRONT_TO_CONV1;
		else if( strPos == "CONV1_TO_CONV2" )		return TPOS_CONV1_TO_CONV2;
		else if( strPos == "CONV2_TO_CONV3" )		return TPOS_CONV2_TO_CONV3;
		else if( strPos == "CONV3_TO_NEXT" )		return TPOS_CONV3_TO_NEXT;
		else if( strPos == "CONV_TO_WORK1" )			return TPOS_CONV_TO_BIN1;
		else if( strPos == "CONV_TO_WORK2" )			return TPOS_CONV_TO_BIN2;
		else if( strPos == "CONV_TO_WORK3" )			return TPOS_CONV_TO_BIN3;
		else if( strPos == "WORK_TO_CONV1" )			return TPOS_BIN_TO_CONV1;
		else if( strPos == "WORK_TO_CONV2" )			return TPOS_BIN_TO_CONV2;
		else if( strPos == "WORK_TO_CONV3" )			return TPOS_BIN_TO_CONV3;
		else if( strPos == "WORK1" )					return TPOS_BIN1;
		else if( strPos == "WORK2" )					return TPOS_BIN2;
		else if( strPos == "WORK3" )					return TPOS_BIN3;
		else if( strPos == "WORK1_TO_REJECT" )		return TPOS_BIN_TO_REJECT;
		else if( strPos == "REJECT" )				return TPOS_REJECT;
	}
	else
	{
		if( strPos == "FRONT" )						return TPOS_FRONT;
		else if( strPos == "CONV1" )				return TPOS_CONV1;
		else if( strPos == "CONV2" )				return TPOS_CONV2;
		else if( strPos == "CONV3" )				return TPOS_CONV3;
		else if( strPos == "NEXT" )					return TPOS_NEXT;
		else if( strPos == "FRONT_TO_CONV1" )		return TPOS_FRONT_TO_CONV1;
		else if( strPos == "CONV1_TO_CONV2" )		return TPOS_CONV1_TO_CONV2;
		else if( strPos == "CONV2_TO_CONV3" )		return TPOS_CONV2_TO_CONV3;
		else if( strPos == "CONV3_TO_NEXT" )		return TPOS_CONV3_TO_NEXT;
		else if( strPos == "CONV_TO_WORK1" )			return TPOS_CONV_TO_BIN1;
		else if( strPos == "CONV_TO_WORK2" )			return TPOS_CONV_TO_BIN2;
		else if( strPos == "CONV_TO_WORK3" )			return TPOS_CONV_TO_BIN3;
		else if( strPos == "WORK_TO_CONV1" )			return TPOS_BIN_TO_CONV1;
		else if( strPos == "WORK_TO_CONV2" )			return TPOS_BIN_TO_CONV2;
		else if( strPos == "WORK_TO_CONV3" )			return TPOS_BIN_TO_CONV3;
		else if( strPos == "WORK1" )					return TPOS_BIN1;
		else if( strPos == "WORK2" )					return TPOS_BIN2;
		else if( strPos == "WORK3" )					return TPOS_BIN3;
		else if( strPos == "WORK3_TO_REJECT" )		return TPOS_BIN_TO_REJECT;
		else if( strPos == "REJECT" )				return TPOS_REJECT;
	}

	return TPOS_NONE;
}

int APartTray::GetIdxByPos_Extra( ETRAY_POS epos )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray_extra[i].GetPos() == epos )
		{
			return i;
		}
	}

	return -1;
}

int APartTray::GetIdxByTargetPos_Extra( ETRAY_POS epos )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray_extra[i].GetTargetPos() == epos )
		{
			return i;
		}
	}

	return -1;
}

int APartTray::GetStopRejCnt()
{
	int iCnt = 0;
	for( int i=0; i<MAX_TRAY; i++ )
	{
		if( m_tray[i].GetPos() == TPOS_REJECT )
		{
			iCnt++;
		}
	}

	return iCnt;
}

std::vector<int> APartTray::GetVecGoodCell_3Stacker()
{
	std::vector<int> vecRtn;
	
	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;
	
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosSub;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		ePosSub = TPOS_BIN1;
	}
	else
	{
		ePosSub = TPOS_BIN3;	
	}
	
	int iIndexBinSub = MPOS_INDEX_BIN3;	
	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN1 )	iIndexBinSub = MPOS_INDEX_BIN1;
	
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosSub );
	if( iIdx < 0 )
		return vecRtn;
	
	ATray traySub = g_Tray.GetTray( iIdx );
	
	if( traySub.GetTargetPos() == ePosSub &&
		traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
	{
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( traySub.GetModule(i).IsExist() && traySub.GetModule(i).GetModuleState() == EMS_GOOD )
				vecRtn.push_back( iIndexBinSub + i );
		}
	}
	
	return vecRtn;
}


std::vector<int> APartTray::GetVecGoodCell_Sub()
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int ix=0, iy=0;
	ETRAY_POS ePosPri = g_handler.GetPriorityBin();
	ETRAY_POS ePosSub = TPOS_NONE;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_NONE;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3)
				{
					if(iBin1 > -1 && st_handler.mb_rej_3stacker == false) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}
	}
	else
	{
		if(ePosPri == TPOS_BIN1)
		{
			if(iBin2 > -1)	ePosSub = TPOS_BIN2;
			else if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	ePosSub = TPOS_BIN3;
			else			ePosSub = TPOS_NONE;
		}
		else
		{
			if(ePosPri == TPOS_BIN2) 
			{
				if(iBin3 > -1 && st_handler.mb_rej_3stacker == false)	    ePosSub = TPOS_BIN3;
				else if(iBin1 > -1) ePosSub = TPOS_BIN1;
				else                ePosSub = TPOS_NONE;
			}
			else
			{
				if(ePosPri == TPOS_BIN3 && st_handler.mb_rej_3stacker == false)
				{
					if(iBin1 > -1) ePosSub = TPOS_BIN1;
					else
					{
						if(iBin2 > -1) ePosSub = TPOS_BIN2;
						else           ePosSub = TPOS_NONE;
					}
				}
			}
		}
	}
	int iIndexBinPri = MPOS_INDEX_BIN3;
	int iIndexBinSub = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosSub == TPOS_BIN2 )	iIndexBinSub = MPOS_INDEX_BIN2;
	if( ePosPri == TPOS_BIN1 )	iIndexBinPri = MPOS_INDEX_BIN1;
	if( ePosSub == TPOS_BIN1 )	iIndexBinSub = MPOS_INDEX_BIN1;

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosSub );
	if( iIdx < 0 )
		return vecRtn;

	ATray traySub = g_Tray.GetTray( iIdx );

	if( traySub.GetTargetPos() == ePosSub &&
		traySub.GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
	{
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( traySub.GetModule(i).IsExist() && traySub.GetModule(i).GetModuleState() == EMS_GOOD )
				vecRtn.push_back( iIndexBinSub + i );
		}
	}

	return vecRtn;
}

std::vector<int> APartTray::GetVecEmptyCell_Pri()
{
	std::vector<int> vecRtn;

	if( g_handler.GetPriorityBin() == TPOS_NONE )
		return vecRtn;

	ETRAY_POS ePosPri = g_handler.GetPriorityBin();

	int iIndexBinPri = MPOS_INDEX_BIN3;

	if( ePosPri == TPOS_BIN2 )	iIndexBinPri = MPOS_INDEX_BIN2;
	if( ePosPri == TPOS_BIN1 )	iIndexBinPri = MPOS_INDEX_BIN1;

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	int iIdx = g_Tray.GetIdxByPos( ePosPri );
	if( iIdx < 0 )
		return vecRtn;

	ATray trayPri = g_Tray.GetTray( iIdx );

	if( trayPri.GetTargetPos() == ePosPri &&
		trayPri.GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
	{
		for( int i=0; i<iMdlCnt; i++ )
		{
			if( trayPri.GetModule(i).IsExist() == false  )
				vecRtn.push_back( iIndexBinPri + i );
		}
	}

	return vecRtn;
}

