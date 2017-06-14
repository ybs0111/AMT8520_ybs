// Run_Test_Site.cpp: implementation of the CRun_Test_Site class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Test_Site.h"
#include "ComizoaPublic.h"
#include "IO_Manager.h"
#include "SrcPart/APartNetworkManager.h"

#include "SrcPart/APartTestSite.h"
#include "AMTLotManager.h"
#include "SrcBase/ALocalization.h"


// #include "stdafx.h"
// #include "handler.h"
// #include "Run_Test_Site_1.h"
// 
// #include "FastechPublic_IO.h"
 
// #include "Public_Function.h"
// #include "CtlBd_Function.h"
// #include "CtlBd_Library.h"
// 
// #include "Cmmsdk.h"
// #include "CmmsdkDef.h"
// 
// #include "math.h"
// #include "io.h" 
// 
// #include "COMI_Manager.h"
// #include "IO_Manager.h"
// #include "AMTLotManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CRun_Test_Site, CObject, 1);	// 복구 동작을 위함...

CRun_Test_Site Run_Test_Site[TEST_SITE_CNT];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRun_Test_Site::CRun_Test_Site()
{
	Thread_Variable_Initial();
	m_bRejAlarm = false;
}

CRun_Test_Site::~CRun_Test_Site()
{

}

void CRun_Test_Site::Thread_Variable_Initial()
{
	mn_InitStep = 0;
	mn_RunStep = 0;

	for( int i=0; i<INSERT_CNT; i++ )
	{
		m_fbInsert[i] = FB_NONE;
		m_step_insert[i] = ESTEP_CYL_START;
		//2012,1225
		m_fbPogoOnOff[i] = FB_NONE;
		m_step_pogo[i] = ESTEP_CYL_START;

		m_step_test[i] = ETEST_NONE;

		st_work.mn_site_insert[0][i] = IO_OFF;
		st_work.mn_site_insert[1][i] = IO_OFF;

		st_work.mn_site_pogo[0][i] = IO_OFF;
		st_work.mn_site_pogo[1][i] = IO_OFF;
	}

	m_fbLock = FB_NONE;
	m_step_lock = ESTEP_CYL_START;

	m_fbPlate = FB_NONE;
	m_step_plate = ESTEP_CYL_START;

	m_step_move = EMOVE_Z_NONE;

	m_step_move_old = EMOVE_Z_NONE;
	for( i=0; i<MAX_MODULE; i++ )
	{
		m_strOldLotID[i] = _T("");
	}

	////2015.0604
	for( i=0; i<100; i++ )
	{
		st_handler.mn_scrap_code_retry[i] = 0;
	}
	////

}

void CRun_Test_Site::Thread_Run()
{
	switch (COMI.mn_run_status)
	{
	case dINIT:
		Run_Initial();
		break;
		
	case dRUN:
		Run_Move();
		break;
		
	case dREINSTATE:
		Run_Reinstate();
		break;

	default:
		OnRobot_FinalPos();
		CTL_Lib.mn_single_motmove_step[ m_nMotID ] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.

		m_step_move_old = EMOVE_Z_NONE;
		break;
	}

	for( int i=0; i<INSERT_CNT; i++ )
	{
		Run_Move_Pogo( i );
		Run_Move_Insert( i );
	}

	Run_Move_Lock();
	Run_Move_Plate();
}

void CRun_Test_Site::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[m_nSiteID] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case ETSI_START:					mn_InitStep = ETSI_CHECK_ROBOT;	break;

	case ETSI_CHECK_ROBOT:				ETSI_Check_Robot();						break;
	case ETSI_POGO_BACKWARD:			ETSI_Check_Pogo_Backward();				break; //2012,1225
	case ETSI_POGO_BACKWARD_CHK:		ETSI_Check_Pogo_Backward_Chk();			break;
	case ETSI_PLATE_BACKWARD:			ETSI_Plate_Backward();					break;
	case ETSI_PLATE_BACKWARD_CHK:		ETSI_Plate_Backward_Chk();				break;
	case ETSI_COK_INSERT_FORWARD:		ETSI_COK_Insert_Forward();				break;
	case ETSI_COK_INSERT_FORWARD_CHK:	ETSI_COK_Insert_Forward_Chk();			break;
	case ETSI_COK_UNLOCK:				ETSI_COK_UnLock();						break;
	case ETSI_COK_UNLOCK_CHK:			ETSI_COK_UnLock_Chk();					break;
		
	case ETSI_HOMECHK_Z:				ETSI_HomeChk_Z();						break;
	case ETSI_FINISH:					ETSI_Finish();							break;
	}
}

void CRun_Test_Site::Run_Move()
{
	for( int i=0; i<INSERT_CNT; i++ )
	{
		Run_Move_Test( i );
	}

	Run_Move_ToSocket();
}

void CRun_Test_Site::Serialize(CArchive &ar)
{
	int i = 0, j = 0;
	
	CObject::Serialize(ar);
	
	if(ar.IsStoring())				// 저장하기
	{
		ar << mn_RunStep;
	}
	else							// 불러오기
	{	
		ar >> mn_RunStep;
		
	}
}

void CRun_Test_Site::ETSI_Check_Robot()
{
	if( Run_Robot.m_bInitSafety == false )	return;
	
	// 	mn_InitStep = ETSI_PLATE_BACKWARD; //2012,1225
	mn_InitStep = ETSI_POGO_BACKWARD;
}

void CRun_Test_Site::ETSI_COK_UnLock()
{
	ReqLock( BACKWARD );

	mn_InitStep = ETSI_COK_UNLOCK_CHK;
}
	
void CRun_Test_Site::ETSI_COK_UnLock_Chk()
{
	if( GetLock() != FB_NONE )
		return;

	mn_InitStep = ETSI_HOMECHK_Z;
	Func.OnInitBtn( RBTN_SITE1_LOCK + (m_nSiteID - INIT_TEST_SITE_1 ) * 5 );
}
	
void CRun_Test_Site::ETSI_COK_Insert_Forward()
{
	//2012,1220
	if( GetCurrentTime() - m_dwTimeInit < 0 )
	{
		m_dwTimeInit = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTimeInit < 300 )
		return;

	ReqInsert( 0, FORWARD );
	ReqInsert( 1, FORWARD );
	ReqInsert( 2, FORWARD );
	ReqInsert( 3, FORWARD );
	ReqInsert( 4, FORWARD );

	mn_InitStep = ETSI_COK_INSERT_FORWARD_CHK;
}
	
void CRun_Test_Site::ETSI_COK_Insert_Forward_Chk()
{
	if( GetInsert(0) != FB_NONE )		return;
	if( GetInsert(1) != FB_NONE )		return;
	if( GetInsert(2) != FB_NONE )		return;
	if( GetInsert(3) != FB_NONE )		return;
	if( GetInsert(4) != FB_NONE )		return;

	mn_InitStep = ETSI_COK_UNLOCK;
	Func.OnInitBtn( RBTN_SITE1_COK_INSERT + (m_nSiteID - INIT_TEST_SITE_1 ) * 5 );

	m_dwTimeInit = GetCurrentTime();
}

void CRun_Test_Site::ETSI_Check_Pogo_Backward()
{
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		ReqPogo( 0, BACKWARD );
		ReqPogo( 1, BACKWARD );
		ReqPogo( 2, BACKWARD );
		ReqPogo( 3, BACKWARD );
		ReqPogo( 4, BACKWARD );		
	}
	mn_InitStep = ETSI_POGO_BACKWARD_CHK;
	m_dwTimeInit = GetCurrentTime();
}

void CRun_Test_Site::ETSI_Check_Pogo_Backward_Chk()
{
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		if( GetPogo(0) != FB_NONE )		return;
		if( GetPogo(1) != FB_NONE )		return;
		if( GetPogo(2) != FB_NONE )		return;
		if( GetPogo(3) != FB_NONE )		return;
		if( GetPogo(4) != FB_NONE )		return;
	}
	
	mn_InitStep = ETSI_PLATE_BACKWARD;
	m_dwTimeInit = GetCurrentTime();
}

void CRun_Test_Site::ETSI_Plate_Backward()
{
	ReqPlate( BACKWARD );

	mn_InitStep = ETSI_PLATE_BACKWARD_CHK;
	m_dwTimeInit = GetCurrentTime();
}
	
void CRun_Test_Site::ETSI_Plate_Backward_Chk()
{
	if( GetPlate() != FB_NONE )
		return;

	mn_InitStep = ETSI_COK_INSERT_FORWARD;
	Func.OnInitBtn( RBTN_SITE1_PLATE_FORWARD + (m_nSiteID - INIT_TEST_SITE_1 ) * 5 );
}

void CRun_Test_Site::ETSI_HomeChk_Z()
{
	// ROBOT Z CHK

	int nRet = COMI.HomeCheck_Mot( m_nMotID, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ETSI_FINISH;
		Func.OnInitBtn( RBTN_SITE1_Z + (m_nSiteID - INIT_TEST_SITE_1 ) * 5 );
	}
	else if( nRet == BD_ERROR )
	{
// 		CString strJamCode = "HMS100";
		CString strJamCode = "444100";
// 		if( m_nSiteID - INIT_TEST_SITE_1 == 1 )
// 			strJamCode = "HMS200";
		if( m_nSiteID - INIT_TEST_SITE_1 == 1 )
			strJamCode = "444200";

		CTL_Lib.Alarm_Error_Occurrence(660, CTL_dWARNING, strJamCode );
	}
}
	
void CRun_Test_Site::ETSI_Finish()
{
	st_handler.mn_init_state[m_nSiteID] = CTL_YES;
	mn_InitStep = ETSI_START;
	Func.OnInitBtn( RBTN_SITE1 + (m_nSiteID - INIT_TEST_SITE_1 ) * 5 );
}

void CRun_Test_Site::Run_Move_Insert(int iIdx)
{
	if( m_fbInsert[iIdx] == FB_NONE )	return;

	int iOnOff = IO_ON;
	if( m_fbInsert[iIdx] == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_insert[iIdx] )
	{
	case ESTEP_CYL_START:	m_step_insert[iIdx] = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			SetInsert(iIdx, iOnOff);
			//2013,0203
			if(COMI.mn_run_status == dRUN) st_work.mn_site_insert[m_nSiteID - INIT_TEST_SITE_1][iIdx] = iOnOff;
			m_step_insert[iIdx] = ESTEP_CYL_CHK;
			m_dwTimeInsertStart[iIdx] = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			if( ChkInsert( iIdx, iOnOff) )
			{
				m_step_insert[iIdx] = ESTEP_CYL_FINISH;
				m_dwTimeInsert[iIdx] = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeInsertStart[iIdx] > st_time.n_limit_time[E_WAIT_INSERT] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TS_Insert_Time(m_nSiteID - INIT_TEST_SITE_1, iIdx, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 661, dWARNING, strJamCode );
					//2015.0826
					SetInsert(iIdx, IO_OFF);
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimeInsertStart[iIdx] < 0 )
			{
				m_dwTimeInsertStart[iIdx] = GetCurrentTime();
			}

		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimeInsert[iIdx] < 0 )
			{
				m_dwTimeInsert[iIdx] = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeInsert[iIdx] < st_time.n_wait_time[E_WAIT_INSERT][iOnOff] )
				return;

			m_fbInsert[iIdx] = FB_NONE;
			m_step_insert[iIdx] = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Test_Site::Run_Move_Plate()
{
	if( m_fbPlate == FB_NONE )	return;
	
	int iOnOff = IO_ON;
	if( m_fbPlate == BACKWARD )
		iOnOff = IO_OFF;

	int iOnOff_Rev = (iOnOff == IO_ON) ? IO_OFF : IO_ON;

	switch( m_step_plate )
	{
	case ESTEP_CYL_START:	m_step_plate = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:
		{
			int iForward = st_io.o_testsite1_cok_plate_fwd;
			int iBackward = st_io.o_testsite1_cok_plate_bwd;
			
			if( m_nSiteID == INIT_TEST_SITE_2 )
			{
				iForward = st_io.o_testsite2_cok_plate_fwd;
				iBackward = st_io.o_testsite2_cok_plate_bwd;
			}
			
			g_ioMgr.set_out_bit( iForward, iOnOff );
			g_ioMgr.set_out_bit( iBackward, iOnOff_Rev );

			m_step_plate = ESTEP_CYL_CHK;
			m_dwTimePlateStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iForward = st_io.i_testsite1_cok_plate_fwd_chk;
			int iBackward = st_io.i_testsite1_cok_plate_bwd_chk;
			
			if( m_nSiteID == INIT_TEST_SITE_2 )
			{
				iForward = st_io.i_testsite2_cok_plate_fwd_chk;
				iBackward = st_io.i_testsite2_cok_plate_bwd_chk;
			}
			
			if( g_ioMgr.get_in_bit( iBackward ) != iOnOff &&
				g_ioMgr.get_in_bit( iForward ) == iOnOff )
			{
				m_step_plate = ESTEP_CYL_FINISH;
				m_dwTimePlate = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimePlateStart > st_time.n_limit_time[E_WAIT_SITE_PLATE] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TS_Plate_Time( m_nSiteID - INIT_TEST_SITE_1, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 662, dWARNING, strJamCode );
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimePlateStart < 0 )
			{
				m_dwTimePlateStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimePlate < 0 )
			{
				m_dwTimePlate = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimePlate < st_time.n_wait_time[E_WAIT_SITE_PLATE][iOnOff] )
				return;

			m_fbPlate = FB_NONE;
			m_step_plate = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Test_Site::SetInsert( int iIdx, int iOnOff )
{
	int iInsert = st_io.o_testsite1_cok_insert_fwdbwd[iIdx];
	if( m_nSiteID == INIT_TEST_SITE_2 )
	{
		iInsert = st_io.o_testsite2_cok_insert_fwdbwd[iIdx];
	}

	g_ioMgr.set_out_bit( iInsert, iOnOff );
}

bool CRun_Test_Site::ChkInsert( int iIdx, int iOnOff )
{
	int iInsert_Fwd = st_io.i_testsite1_cok_insert_fwd_chk[iIdx];
	int iInsert_Bwd = st_io.i_testsite1_cok_insert_bwd_chk[iIdx];

	if( m_nSiteID == INIT_TEST_SITE_2 )
	{
		iInsert_Fwd = st_io.i_testsite2_cok_insert_fwd_chk[iIdx];
		iInsert_Bwd = st_io.i_testsite2_cok_insert_bwd_chk[iIdx];
	}

	if( g_ioMgr.get_in_bit( iInsert_Fwd ) != iOnOff )	return false;
	if( g_ioMgr.get_in_bit( iInsert_Bwd ) == iOnOff )	return false;

	return true;
}

void CRun_Test_Site::ReqInsert( int iIdx, EFB efb )
{
	ASSERT( iIdx > -1);

	m_fbInsert[iIdx] = efb;
	m_step_insert[iIdx] = ESTEP_CYL_START;
}

void CRun_Test_Site::Run_Move_Lock()
{
	if( m_fbLock == FB_NONE )	return;

	int iOnOff = IO_ON;
	if( m_fbLock == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_lock )
	{
	case ESTEP_CYL_START:	m_step_lock = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			int iLock = st_io.o_testsite1_cok_lockunlock;
			if( m_nSiteID == INIT_TEST_SITE_2 )
				iLock = st_io.o_testsite2_cok_lockunlock;

			g_ioMgr.set_out_bit( iLock, iOnOff );
			m_step_lock = ESTEP_CYL_CHK;
			m_dwTimeLockStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iLock = st_io.i_testsite1_cok_lock_chk;
			int iUnlock = st_io.i_testsite1_cok_unlock_chk;

			if( m_nSiteID == INIT_TEST_SITE_2 )
			{
				iLock = st_io.i_testsite2_cok_lock_chk;
				iUnlock = st_io.i_testsite2_cok_unlock_chk;
			}

			if( g_ioMgr.get_in_bit( iLock ) == iOnOff &&
				g_ioMgr.get_in_bit( iUnlock ) != iOnOff )
			{
				m_step_lock = ESTEP_CYL_FINISH;
				m_dwTimeLock = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeLockStart > st_time.n_limit_time[E_WAIT_COK_LOCK] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TS_Lock_Time( m_nSiteID - INIT_TEST_SITE_1, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 663, dWARNING, strJamCode );
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimeLockStart < 0 )
			{
				m_dwTimeLockStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimeLock < 0 )
			{
				m_dwTimeLock = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeLock < st_time.n_wait_time[E_WAIT_COK_LOCK][iOnOff] )
				return;

			m_fbLock = FB_NONE;
			m_step_lock = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Test_Site::ReqLock( EFB efb )
{
	m_fbLock = efb;
	m_step_lock = ESTEP_CYL_START;
}

void CRun_Test_Site::ReqPlate( EFB efb )
{
	m_fbPlate = efb;
	m_step_plate = ESTEP_CYL_START;
}

void CRun_Test_Site::Run_Move_Test( int iIdx )
{
	if( Run_Move_Test_JamChk( iIdx ) == false )
		return;

	bool bSaveTest = false;
    CString strSokSerial="";

	int nIdxAdd = (m_nSiteID - INIT_TEST_SITE_1) * 5;
	Func.ThreadFunctionStepTrace(45 + m_nSiteID + iIdx, m_step_test[iIdx]); //2013,0608
	switch( m_step_test[iIdx] )
	{
	case ETEST_NONE:		m_step_test[iIdx] = ETEST_MUSTTEST_CHK;		break;

	case ETEST_MUSTTEST_CHK:
		{
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() == false && 
				g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() == false )
				return;

			if( (!g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() || g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestLoadState() ) &&
				( !g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() || g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsTestLoadState() ) )
			{

				m_step_test[iIdx] = ETEST_READY;
			}
		}
		break;

	case ETEST_READY:		
		{
			m_step_test[iIdx] = ETEST_READY_CHK;

			//2017.0602
			if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES )
			{
				if( (!g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() || g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestLoadState() ) )
				{
					g_client_xgem.SetDVCTestStart( (iIdx  + nIdxAdd)* 2, g_site.GetModule((iIdx  + nIdxAdd)* 2).GetSerial() );
				}
				if( ( !g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() || g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsTestLoadState() ) )
				{
					g_client_xgem.SetDVCTestStart( (iIdx  + nIdxAdd)* 2 + 1, g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetSerial() );
				}
			}
		}
		break;

	case ETEST_READY_CHK:	
		if( st_basic.mn_mode_run )
			m_step_test[iIdx] = ETEST_SEND_READY;
		else
			m_step_test[iIdx] = ETEST_INSERT_IN;
		break;

	case ETEST_SEND_READY:
		if( g_site.GetModule((iIdx + nIdxAdd)* 2).IsExist() )
		{
			g_site.GetModule((iIdx + nIdxAdd)* 2).SetModuleState( EMS_READY );
		}
		if( g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).IsExist() )
		{
			g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).SetModuleState( EMS_READY );
		}

		if( st_handler.cwnd_main )
		{
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
		}

		m_step_test[iIdx] = ETEST_RECV_READY;
		break;

	case ETEST_RECV_READY:
		if( g_site.GetModule((iIdx + nIdxAdd)* 2).GetModuleState() == EMS_READY )
			break;
		if( g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).GetModuleState() == EMS_READY )
			break;

		m_step_test[iIdx] = ETEST_INSERT_IN;
		break;

	case ETEST_INSERT_IN:
		if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			break;

		if(st_handler.mn_pogo_type == CTL_YES && st_basic.n_mode_pogo == CTL_YES)
		{
			ReqPogo(iIdx, BACKWARD);
			m_step_test[iIdx] = ETEST_INSERT_IN_POGO;
		}
		else
		{
			ReqInsert(iIdx, FORWARD);
			m_step_test[iIdx] = ETEST_INSERT_IN_CHK;
		}
		break;

	case ETEST_INSERT_IN_POGO:
		if( GetPogo(iIdx) != FB_NONE )	return;
		m_step_test[iIdx] = ETEST_INSERT_IN_POGO_CHK;
		break;

	case ETEST_INSERT_IN_POGO_CHK:
		if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			break;
		ReqInsert(iIdx, FORWARD);
		m_step_test[iIdx] = ETEST_INSERT_IN_CHK;
		break;

	case ETEST_INSERT_IN_CHK:
		if( GetInsert(iIdx) != FB_NONE )	return;

		if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
			g_site.AddCount((iIdx  + nIdxAdd)* 2);
		if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
			g_site.AddCount((iIdx  + nIdxAdd)* 2 + 1);
		
		if( st_basic.mn_mode_run )
		{
			if(st_handler.mn_pogo_type == CTL_YES)
			{
				if(st_basic.n_mode_pogo == CTL_YES)
				{
					m_step_test[iIdx] = ETEST_INSERT_POGO_CHK;
				}
				else
				{
					m_step_test[iIdx] = ETEST_TEST_READY_CHK;
				}
			}
			else
			{
				m_step_test[iIdx] = ETEST_TEST_READY_CHK;
			}
		}
		else
		{
// 			m_step_test[iIdx] = ETEST_TEST_END_CHK;
			if(st_handler.mn_pogo_type == CTL_YES)
			{
				if(st_basic.n_mode_pogo == CTL_YES)
				{
					m_step_test[iIdx] = ETEST_INSERT_POGO_CHK;
				}
				else
				{
					m_step_test[iIdx] = ETEST_TEST_READY_CHK;
				}
			}
			else
			{
				m_step_test[iIdx] = ETEST_TEST_READY_CHK;
			}
// 			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
// 			{
// 				g_site.GetModule((iIdx  + nIdxAdd)* 2).SetModuleState( EMS_TEST );
// 				m_dwTimeStartTest[iIdx * 2] = GetCurrentTime();
// 			}
// 			if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
// 			{
// 				g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).SetModuleState( EMS_TEST );
// 				m_dwTimeStartTest[iIdx * 2 + 1] = GetCurrentTime();
// 			}
		}

		{
			int num1 = g_site.GetTestCount((iIdx  + nIdxAdd)* 2);
			int num2 = g_site.GetTestCount((iIdx  + nIdxAdd)* 2 + 1);
			if(num1 > st_basic.mn_socket_max_count || num2 > st_basic.mn_socket_max_count)
			{
				if(GetRejAlarm() == false)
				{
					CString strErr;
					strErr.Format( " 사이트 : %d, 또는 사이트 : %d  에서 최대 소켓 사용 회수를 초과 했습니다.(MAX SOCKET COUNT : %d)", num1, num2, st_basic.mn_socket_max_count);
					if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( " SITE : %d or SITE : %d  exceed Max Socket Count.(MAX SOCKET COUNT : %d)", num1, num2, st_basic.mn_socket_max_count);
					st_msg.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
					if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
					{
						strErr.Format( " 사이트 : %d, 또는 사이트 : %d  에서 최대 소켓 사용 회수를 초과 했습니다.(MAX SOCKET COUNT : %d)", num1, num2, st_basic.mn_socket_max_count);
						if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( " SITE : %d or SITE : %d  exceed Max Socket Count.(MAX SOCKET COUNT : %d)", num1, num2, st_basic.mn_socket_max_count);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
					}
					SetRejAlarm(true);
				}
			}

		}
		break;

	case  ETEST_INSERT_POGO_CHK:
		if(st_handler.mn_pogo_type == CTL_YES)
		{
			if(st_basic.n_mode_pogo == CTL_YES)
			{
				m_step_test[iIdx] = ETEST_POGO_ON;
			}
			else
			{
				m_step_test[iIdx] = ETEST_TEST_READY_CHK;
			}
		}
		else
		{
			m_step_test[iIdx] = ETEST_TEST_READY_CHK;
		}
		break;

	case ETEST_POGO_ON:
		if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			break;
		
		if(st_handler.mn_pogo_type == CTL_YES && st_basic.n_mode_pogo == CTL_YES)
		{
			ReqPogo(iIdx, FORWARD);
			m_step_test[iIdx] = ETEST_POGO_ON_CHK;
		}
		else
		{
			m_step_test[iIdx] = ETEST_TEST_READY_CHK;
		}
		break;
		
	case ETEST_POGO_ON_CHK:
		if( GetPogo(iIdx) != FB_NONE)	return; //2012,1229
		
		m_step_test[iIdx] = ETEST_TEST_READY_CHK;
		break;

	case ETEST_TEST_READY_CHK:
		if( st_basic.mn_mode_run )
		{
			m_step_test[iIdx] = ETEST_SEND_TEST_START;
		}
		else
		{
			m_step_test[iIdx] = ETEST_TEST_END_CHK;
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
			{
				g_site.GetModule((iIdx  + nIdxAdd)* 2).SetModuleState( EMS_TEST );
				m_dwTimeStartTest[iIdx * 2] = GetCurrentTime();
			}
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
			{
				g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).SetModuleState( EMS_TEST );
				m_dwTimeStartTest[iIdx * 2 + 1] = GetCurrentTime();
			}
		}		
		break;

	case ETEST_SEND_TEST_START:
		if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
		{
			g_site.GetModule((iIdx + nIdxAdd)* 2).SetModuleState( EMS_START );
// 			m_dwTimeStartModule[(iIdx  + nIdxAdd)* 2] = GetCurrentTime();
			m_dwTimeStartModule[iIdx * 2] = GetCurrentTime();
		}
		if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
		{
			g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).SetModuleState( EMS_START );
// 			m_dwTimeStartModule[(iIdx  + nIdxAdd)* 2 + 1] = GetCurrentTime();
			m_dwTimeStartModule[iIdx * 2 + 1] = GetCurrentTime();
		}

		m_step_test[iIdx] = ETEST_RECV_TEST_START;


		m_nFlag[(iIdx  + nIdxAdd)* 2] = CTL_NO;
		m_nFlag[(iIdx  + nIdxAdd)* 2 + 1] = CTL_NO;
		break;


	case ETEST_RECV_TEST_START:
		if( m_nFlag[(iIdx  + nIdxAdd)* 2] == CTL_NO)
		{
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist())
			{
				if( g_site.GetModule((iIdx + nIdxAdd)* 2).GetModuleState() == EMS_TEST)
				{					
					m_nFlag[(iIdx  + nIdxAdd)* 2] = CTL_YES;
					//if( st_handler.mn_Abort = CTL_YES)
					//{
					//	m_nFlag[(iIdx  + nIdxAdd)* 2] = CTL_NO;
					//}
				}
			}
			else
			{
				m_nFlag[(iIdx  + nIdxAdd)* 2] = CTL_YES;
			}
		}
		if( m_nFlag[(iIdx  + nIdxAdd)* 2 + 1] == CTL_NO)
		{
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist())
			{
				if( g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).GetModuleState() == EMS_TEST)
				{
					m_nFlag[(iIdx  + nIdxAdd)* 2 + 1] = CTL_YES;
					//if( st_handler.mn_Abort = CTL_YES)
					//{
					//	m_nFlag[(iIdx  + nIdxAdd)* 2] = CTL_NO;
					//}
				}
			}
			else
			{
				m_nFlag[(iIdx  + nIdxAdd)* 2 + 1] = CTL_YES;
			}
		}


		if( m_nFlag[(iIdx  + nIdxAdd)* 2] == CTL_YES && m_nFlag[(iIdx  + nIdxAdd)* 2 + 1] == CTL_YES )
		{
			if( g_site.GetModule((iIdx + nIdxAdd)* 2).GetModuleState() == EMS_START )
				break;

			if( g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).GetModuleState() == EMS_START )
				break;

			m_step_test[iIdx] = ETEST_TEST_END_CHK;
		}
		else if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestEnd() &&
			g_site.GetModule((iIdx + nIdxAdd) * 2 + 1).IsTestEnd() )
		{
			CString str_msg;
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() && g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
			{
				str_msg.Format("[RunTest_Abort] Slot:%d , Slot%d", (iIdx  + nIdxAdd)* 2, (iIdx + nIdxAdd) * 2 + 1 );
			}
			else if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist())
			{
				str_msg.Format("[RunTest_Abort] Slot:%d ", (iIdx  + nIdxAdd)* 2 );
			}
			else // g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() 
			{
				str_msg.Format("[RunTest_Abort] Slot:%d ", (iIdx + nIdxAdd) * 2 + 1 );
			}

			st_handler.mn_Abort = CTL_NO;

			Func.On_LogFile_Add(99, str_msg);
			m_step_test[iIdx] = ETEST_TEST_TIMEOUT;

		}
		else
		{
			DWORD dwCurTime = GetCurrentTime();
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
			{
				if( dwCurTime - m_dwTimeStartModule[iIdx * 2] < 0 )	 m_dwTimeStartModule[iIdx * 2] = GetCurrentTime();
			}
			if( g_site.GetModule((iIdx + nIdxAdd)*2 +1).IsExist() )
			{
				if( dwCurTime - m_dwTimeStartModule[iIdx * 2 + 1] < 0 ) m_dwTimeStartModule[iIdx * 2 + 1] = GetCurrentTime();
			}
			if( st_basic.n_auto_timeout_mode == CTL_YES)
			{
				CString strLotID;
				strLotID = g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo();
				if( g_lotMgr.GetLotByLotID(strLotID).GetAutoTimeOut() == CTL_YES )
				{
					if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2] > (g_lotMgr.GetLotByLotID(strLotID).GetTimeOut() * 1000)) ||
						(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2 + 1] > (g_lotMgr.GetLotByLotID(strLotID).GetTimeOut() * 1000)) )
					{
						m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
					}
				}
				else
				{
					if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2] > (st_basic.mn_time_out * 1000)) ||
						(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
					{
						m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
					}
				}
			}
			else
			{
				if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
				{
					if( dwCurTime - m_dwTimeStartModule[iIdx * 2] < 0 )	 m_dwTimeStartModule[iIdx * 2] = GetCurrentTime();
				}
				if( g_site.GetModule((iIdx + nIdxAdd)*2 +1).IsExist() )
				{
					if( dwCurTime - m_dwTimeStartModule[iIdx * 2 + 1] < 0 ) m_dwTimeStartModule[iIdx * 2 + 1] = GetCurrentTime();
				}
				if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2] > (st_basic.mn_time_out * 1000)) ||
					(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartModule[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
				{
					m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
				}
			}
		}
		break;

	case ETEST_TEST_END_CHK:

		if( st_basic.mn_mode_run )
		{
			DWORD dwCurTime = GetCurrentTime();

			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestEnd() &&
				g_site.GetModule((iIdx + nIdxAdd) * 2 + 1).IsTestEnd() )
			{
				m_step_test[iIdx] = ETEST_TEST_END;

			}
			
			// TIME OUT
			{
				//2016.0201
// 				if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2] > (st_basic.mn_time_out * 1000)) ||
// 					(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
// 				{
// 					m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
// 				}
				if( st_basic.n_auto_timeout_mode == CTL_YES)
				{
					CString strLotID;
					strLotID = g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo();
					if( g_lotMgr.GetLotByLotID(strLotID).GetAutoTimeOut() == CTL_YES )
					{
						if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2] > (g_lotMgr.GetLotByLotID(strLotID).GetTimeOut() * 1000)) ||
							(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1] > (g_lotMgr.GetLotByLotID(strLotID).GetTimeOut() * 1000)) )
						{
							m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
						}
					}
					else
					{
						if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2] > (st_basic.mn_time_out * 1000)) ||
							(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
						{
							m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
						}
					}
				}
				else
				{
					if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2] > (st_basic.mn_time_out * 1000)) ||
						(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsTestEnd() == false && dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
					{
						m_step_test[iIdx] = ETEST_TEST_TIMEOUT;
					}
				}
			}
		}
		else
		{
			DWORD dwCurTime = GetCurrentTime();
			if( (g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsExist() == false || dwCurTime - m_dwTimeStartTest[iIdx * 2] > (st_basic.mn_time_out * 1000)) &&
				(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsExist() == false || dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1] > (st_basic.mn_time_out * 1000)) )
			{
				m_step_test[iIdx] = ETEST_TEST_END;
				if(g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsExist())
				{
					g_site.GetModule( (iIdx + nIdxAdd ) * 2).SetBin("1");
					g_site.GetModule((iIdx + nIdxAdd ) * 2).SetTestedPos((iIdx + nIdxAdd ) * 2);
				}
				if(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsExist())
				{
					g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).SetBin("1");
					g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetTestedPos((iIdx + nIdxAdd ) * 2 + 1);
				}

			}
		}
		
		break;

	case ETEST_TEST_TIMEOUT:
		if( g_site.GetModule((iIdx + nIdxAdd) * 2).IsExist() && g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestEnd() == false )
			g_site.GetModule((iIdx + nIdxAdd)* 2).SetModuleState( EMS_ABORT );
		if( g_site.GetModule((iIdx + nIdxAdd) * 2 + 1).IsExist() && g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsTestEnd() == false )
			g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).SetModuleState( EMS_ABORT );

		m_step_test[iIdx] = ETEST_TEST_TIMEOUT_CHK;
		break;

	case ETEST_TEST_TIMEOUT_CHK:
		if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsTestEnd() &&
			g_site.GetModule((iIdx + nIdxAdd) * 2 + 1).IsTestEnd() )
		{
			m_dwTimeTestEnd[iIdx] = GetCurrentTime();
			m_step_test[iIdx] = ETEST_TEST_END;
		}
		break;

	case ETEST_TEST_END:
		if( GetCurrentTime() - m_dwTimeTestEnd[iIdx] < 0 )
		{
			m_dwTimeTestEnd[iIdx] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTimeTestEnd[iIdx] < 1000 )
			break;

		if(g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsExist())
		{
			//2013,0322
 			if(g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() != m_strOldLotID[(iIdx + nIdxAdd ) * 2])
 			{
 				g_site.AddBinResult((iIdx + nIdxAdd ) * 2, "/" );//LOT 구분
 				m_strOldLotID[(iIdx + nIdxAdd ) * 2] = g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo();
 			}
			g_site.GetModule( (iIdx + nIdxAdd ) * 2).AddTestCnt();
			g_site.AddBinResult((iIdx + nIdxAdd ) * 2, g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetBin() );
			int nBin = atoi(g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetBin());
			if( nBin > 3 )
			{
				//2015.0914
				mn_Input[(iIdx)* 2]++;
				mn_Fail[(iIdx)* 2]++;
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() == 1 )
				{
					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( PRIME );
					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( PRIME );
				}

////2015.0604
// 				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_retest_cnt )
// 				{
// 					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
// 					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
// 					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
// 				}
// 
				if(st_basic.mn_mode_ftp == 0)//not use
				{
					if( g_lotMgr.GetLotCount() > 1 )//Multi lot
					{
						if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_multi_retest_cnt )
						{
							g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
						}
					}
					else
					{
						if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_retest_cnt )
						{
							g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
						}
					}
				}
				else if(st_basic.mn_mode_ftp == 1 || st_basic.mn_mode_ftp == 2)//FTP || //Local
				{
					if( g_lotMgr.GetLotCount() > 1 )//Multi lot
					{
						BOOL bRet1 = FALSE;

						for( int j=0; j<st_handler.m_nScrap_code_cnt; j++ )
						{
							if(g_site.GetModule((iIdx + nIdxAdd ) * 2).GetScrapCode() == st_handler.mstr_scrap_code[j])
							{
								bRet1 = TRUE;
								//2016.0711
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_MultiLot_cnt[j];
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_MultiLot_cnt[j]);
								
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_MultiLot_cnt[j] )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
						if(bRet1 == FALSE)
						{
							if( g_lotMgr.GetLotCount() > 1 )//Multi lot
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_multi_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_multi_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_multi_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
							else
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}

					}
					else if( g_lotMgr.GetLotCount() > 0 )//1Lot
					{
						BOOL bRet = FALSE;
						
						for( int i=0; i<st_handler.m_nScrap_code_cnt; i++ )
						{
							if(g_site.GetModule((iIdx + nIdxAdd ) * 2).GetScrapCode() == st_handler.mstr_scrap_code[i])
							{
								bRet = TRUE;
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_1Lot_cnt[i];
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_1Lot_cnt[i]);
								
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_1Lot_cnt[i] )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
						
						if(bRet == FALSE)
						{
							if( g_lotMgr.GetLotCount() > 1 )//Multi lot
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_multi_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_multi_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_multi_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
							else
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2).SetScrapeCnt(st_handler.mn_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > st_handler.mn_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
					}

				}
////
				if( st_handler.mn_AutoSocketOff_Cnt > 0 &&
					( g_site.GetContinueFailCnt( (iIdx + nIdxAdd ) * 2 ) >= st_handler.mn_AutoSocketOff_Cnt ) )
				{
					CString strErr="", str_log="";
				
					strErr.Format( "오토 소켓 오프 (사이트 : %d, 번호 : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 );					
					if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "AUTO SOCKET OFF (SITE : %d, NUM : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 );
					st_msg.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				
					g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2, -2 );
					//2015.1005
					g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 );

					//2017.0602
					if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
					{
						strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2, st_basic.n_cok_type );
						if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
					}

					//kwlee 2015.1008 log 추가
					str_log.Format("Socket Use : %d : OFF",(iIdx + nIdxAdd) * 2);
					Func.On_LogFile_Add(99, str_log);
				
					//2013,0321
					bool bAllFail = true;
					for( int i=0; i<TSITE_INSERT_CNT; i++ )
					{//2016.0709
// 						if( g_site.GetEnableSocket( i * 2 ) == false &&
// 							g_site.GetEnableSocket( i * 2 + 1 ) == false )
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
						strErr.Format( "모든 Testet socket을 사용할 수 없습니다. 테스터를 체크해 주세요");
						if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "It can't use Tester socket. Check tester plaese.");
						st_msg.mstr_event_msg[0] = strErr;
						::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						COMI.mn_run_status = dSTOP;
					}

					bSaveTest = true;

				}
				//2016.0711
				if(st_handler.m_bBackupEnable2[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2] )
				{
					if(st_handler.m_bBackupEnable[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2] <= 0 && g_site.GetEnableSocket((iIdx + nIdxAdd ) * 2) > 0 )
					{
						g_site.SetEnableSocket( (iIdx + nIdxAdd ) * 2, -4);//repair
						g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 );
						//2017.0602
						if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
						{
							strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2, st_basic.n_cok_type );
							if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
						}
						bSaveTest = true;
					}
				}
			}
			else
			{
				//2015.0914
				mn_Input[(iIdx)* 2]++;
				mn_Pass[(iIdx)* 2]++;
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() == 1 )
				{
					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( PRIME );
					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddPassCnt( PRIME );
				}
				g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetLotNo() ).AddInputCnt( CUM );
				g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddPassCnt( CUM );
				g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo() ).AddBinCnt( nBin );

				//2016.0821
				if( st_basic.mn_hotfix_pos >= 1)
				{
					st_handler.m_bBackupEnable[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2] = 1;
				}

				//2016.0201
				if( st_basic.n_auto_timeout_mode == CTL_YES)
				{
					DWORD dwCurTime = GetCurrentTime();
					CString strLotID;
					DWORD dwCurTime1;
					int i = 0, nTotal = 0, nTestTime;
					if(g_site.GetModule( (iIdx + nIdxAdd ) * 2).IsExist())
					{
						strLotID = g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetLotNo();
						if( g_lotMgr.GetLotByLotID(strLotID).GetAutoTimeOut() != "YES" )
						{
							dwCurTime1 = dwCurTime - m_dwTimeStartTest[iIdx * 2];
							g_lotMgr.GetLotByLotID(strLotID).SetEachTestTimeOut(g_lotMgr.GetLotByLotID(strLotID).GetTimeOutCnt(), dwCurTime1);
							
							if( g_lotMgr.GetLotByLotID(strLotID).GetTimeOutCnt() >= (TEST_MAX_CNT-1) )
							{
								for(i = 0; i <TEST_MAX_CNT; i++ )
								{
									nTotal += g_lotMgr.GetLotByLotID(strLotID).GetEachTestTimeOut(i);
								}
								nTestTime = nTotal/TEST_MAX_CNT;
								//if( nTestTime < 60 || nTestTime > 600 )
								//{
								//	g_lotMgr.GetLotByLotID(strLotID).SetTimeOutCnt(-1);
								//}
								//else
								//{
									nTestTime = nTestTime + ( (nTestTime*st_basic.mn_time_out_per) / 100 ) ;
									nTestTime = nTestTime/1000;
									g_lotMgr.GetLotByLotID(strLotID).SetTimeOut( nTestTime );
									g_lotMgr.GetLotByLotID(strLotID).SetAutoTimeOut("YES");

									if(st_handler.cwnd_title != NULL)
									{
										st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, AOTO_TIMEOUT_MODE, nTestTime);
									}

								//}
							}
							g_lotMgr.GetLotByLotID(strLotID).AddTimeOutCnt();
						}						
					}					
				}
			}

			//2016.0703
			int num1 = g_site.GetTestCount((iIdx  + nIdxAdd)* 2);
			//2016.0709
			if( num1 > st_basic.mn_socket_max_count )
			{
				CString strErr,str_log;
				
				strErr.Format( "최대 소켓 사용 회수 초과 (사이트 : %d, 번호 : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 );					
				if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "MAX SOCKET USE COUNT OFF (SITE : %d, NUM : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 );
				st_msg.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				
				//g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2, false );
				g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2, -1 );//2016.0712
				//2015.1005
				g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 );

				//2017.0602
				if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
				{
					strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2, st_basic.n_cok_type );
					if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
				}

				
				//kwlee 2015.1008 log 추가
				str_log.Format("Socket Use : %d : OFF",(iIdx + nIdxAdd) * 2);
				Func.On_LogFile_Add(99, str_log);
				
				//2013,0321
				bool bAllFail = true;
				for( int i=0; i<TSITE_INSERT_CNT; i++ )
				{//2016.0709
// 					if( g_site.GetEnableSocket( i * 2 ) == false &&
// 						g_site.GetEnableSocket( i * 2 + 1 ) == false )
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
					strErr.Format( "모든 Testet socket을 사용할 수 없습니다. 테스터를 체크해 주세요");
					if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "It can't use Tester socket. Check tester plaese.");
					st_msg.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
					COMI.mn_run_status = dSTOP;
				}
				bSaveTest = true;

			}

		}
		if(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsExist())
		{
			//2013,0322
			if(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() != m_strOldLotID[(iIdx + nIdxAdd ) * 2 + 1])
			{
				g_site.AddBinResult((iIdx + nIdxAdd ) * 2 + 1, "/" );//LOT 구분
				m_strOldLotID[(iIdx + nIdxAdd ) * 2 + 1] = g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo();
			}
			g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).AddTestCnt();
			g_site.AddBinResult((iIdx + nIdxAdd ) * 2 + 1, g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetBin() );
			int nBin = atoi(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetBin());
			if( nBin > 3 )
			{
				//2015.0914
				mn_Input[(iIdx)* 2 + 1]++;
				mn_Fail[(iIdx)* 2+ 1]++;
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() == 1 )
				{
					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( PRIME );
					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( PRIME );
				}
////2015.0604
// 				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_retest_cnt )
// 				{
// 					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
// 					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
// 					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
// 				}
//
				if(st_basic.mn_mode_ftp == 0)//not use
				{
					if( g_lotMgr.GetLotCount() > 1 )//Multi lot
					{
						if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_multi_retest_cnt )
						{
							g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
						}
					}
					else
					{
						if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_retest_cnt )
						{
							g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
							g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
						}
					}
				}
				else if(st_basic.mn_mode_ftp == 1 || st_basic.mn_mode_ftp == 2)//FTP || //Local
				{
					if( g_lotMgr.GetLotCount() > 1 )//Multi lot
					{
						BOOL bRet3 = FALSE;

						for( int j=0; j<st_handler.m_nScrap_code_cnt; j++ )
						{
							if(g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).GetScrapCode() == st_handler.mstr_scrap_code[j])
							{
								bRet3 = TRUE;
								//2016.0711
								//st_handler.mn_scrap_code_retry[0] = st_handler.mn_MultiLot_cnt[j];
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_MultiLot_cnt[j]);
								
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_MultiLot_cnt[j] )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
						if(bRet3 == FALSE)
						{
							if( g_lotMgr.GetLotCount() > 1 )//Multi lot
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_multi_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_multi_retest_cnt);

								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_multi_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
							else
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
					}
					else if( g_lotMgr.GetLotCount() > 0 )//1Lot
					{
						BOOL bRet2 = FALSE;
						
						for( int i=0; i<st_handler.m_nScrap_code_cnt; i++ )
						{
							if(g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).GetScrapCode() == st_handler.mstr_scrap_code[i])
							{
								bRet2 = TRUE;
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_1Lot_cnt[i];
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_1Lot_cnt[i]);
								
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_1Lot_cnt[i] )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
						}
						if(bRet2 == FALSE)
						{
							if( g_lotMgr.GetLotCount() > 1 )//Multi lot
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_multi_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_multi_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_multi_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
							else
							{
								st_handler.mn_scrap_code_retry[0] = st_handler.mn_retest_cnt;
								g_site.GetModule((iIdx + nIdxAdd ) * 2 + 1).SetScrapeCnt(st_handler.mn_retest_cnt);
								if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > st_handler.mn_retest_cnt )
								{
									g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddFailCnt( CUM );
									g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );
								}
							}
							
						}
					}

				}
////
				if( st_handler.mn_AutoSocketOff_Cnt > 0 &&
					( g_site.GetContinueFailCnt( (iIdx + nIdxAdd ) * 2 + 1) >= st_handler.mn_AutoSocketOff_Cnt ) )
				{
					CString strErr,str_log;
					strErr.Format( "오토 소켓 오프 (사이트 : %d, 번호 : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 + 1);
					if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "AUTO SOCKET OFF (SITE : %d, NUM : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 + 1);
					st_msg.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

					g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2 + 1, -2 );
					//2015.1005
					g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 + 1 );

					//2017.0602
					if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
					{
						strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2 + 1, st_basic.n_cok_type );
						if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
					}

					//kwlee 2015.1008 log 추가
					str_log.Format("Socket Use : %d : OFF",(iIdx + nIdxAdd) * 2 + 1);
					Func.On_LogFile_Add(99, str_log);

					//2013,0321
					bool bAllFail = true;
					for( int i=0; i<TSITE_INSERT_CNT; i++ )
					{//2016.0709
// 						if( g_site.GetEnableSocket( i * 2 ) == false &&
// 							g_site.GetEnableSocket( i * 2 + 1 ) == false )
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
						strErr.Format( "모든 Testet socket을 사용할 수 없습니다. 테스터를 체크해 주세요");
						if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "It can't use Tester socket. Check tester plaese.");
						st_msg.mstr_event_msg[0] = strErr;
						::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						COMI.mn_run_status = dSTOP;
					}
					
					bSaveTest = true;
			
				}

				//2016.0711
				if(st_handler.m_bBackupEnable2[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2 + 1] )
				{
					if(st_handler.m_bBackupEnable[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2 + 1] <= 0 && g_site.GetEnableSocket((iIdx + nIdxAdd ) * 2 + 1) > 0 )
					{
						g_site.SetEnableSocket( (iIdx + nIdxAdd ) * 2 + 1, -4);//repair
						g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 + 1 );

						//2017.0602
						if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
						{
							strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2 + 1, st_basic.n_cok_type );
							if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
						}

						bSaveTest = true;
					}
				}

			}
			else
			{
				//2015.0914
				mn_Input[(iIdx)* 2 + 1]++;
				mn_Pass[(iIdx)* 2+ 1]++;
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() == 1 )
				{
					g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( PRIME );
					g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddPassCnt( PRIME );
				}
				g_lotMgr.GetLotByLotID( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetLotNo() ).AddInputCnt( CUM );
				g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddPassCnt( CUM );
				g_lotMgr.GetLotByLotID( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo() ).AddBinCnt( nBin );


				//2016.0821
				if( st_basic.mn_hotfix_pos >= 1)
				{
					st_handler.m_bBackupEnable[st_basic.mn_hotfix_pos-1][(iIdx + nIdxAdd ) * 2 + 1] = 1;
				}

				//2016.0201
				if( st_basic.n_auto_timeout_mode == CTL_YES)
				{
					DWORD dwCurTime = GetCurrentTime();
					CString strLotID;
					DWORD dwCurTime2;
					int i = 0, nTotal = 0, nTestTime;
				
					if(g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).IsExist())
					{
						strLotID = g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetLotNo();
						if( g_lotMgr.GetLotByLotID(strLotID).GetAutoTimeOut() != "YES" )
						{
							dwCurTime2 = dwCurTime - m_dwTimeStartTest[iIdx * 2 + 1];
							g_lotMgr.GetLotByLotID(strLotID).SetEachTestTimeOut(g_lotMgr.GetLotByLotID(strLotID).GetTimeOutCnt(), dwCurTime2);
							
							if( g_lotMgr.GetLotByLotID(strLotID).GetTimeOutCnt() >= (TEST_MAX_CNT-1) )
							{
								for(i = 0; i <TEST_MAX_CNT; i++ )
								{
									nTotal += g_lotMgr.GetLotByLotID(strLotID).GetEachTestTimeOut(i);
								}
								nTestTime = nTotal/TEST_MAX_CNT;
								//if( nTestTime < 60 || nTestTime > 600 )
								//{
								//	g_lotMgr.GetLotByLotID(strLotID).SetTimeOutCnt(-1);
								//}
								//else
								//{
									nTestTime = nTestTime + ( (nTestTime*st_basic.mn_time_out_per) / 100 ) ;
									nTestTime = nTestTime/1000;
									g_lotMgr.GetLotByLotID(strLotID).SetTimeOut( nTestTime );
									g_lotMgr.GetLotByLotID(strLotID).SetAutoTimeOut("YES");

									if(st_handler.cwnd_title != NULL)
									{
										st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, AOTO_TIMEOUT_MODE, nTestTime);
									}

								//}
							}
							g_lotMgr.GetLotByLotID(strLotID).AddTimeOutCnt();
						}
					}
					
				}
		
			}


			//2016.0703
			int num2 = g_site.GetTestCount((iIdx  + nIdxAdd)* 2 + 1);
			//2016.0709
			if( num2 > st_basic.mn_socket_max_count )
			{
				CString strErr,str_log;
				
				strErr.Format( "최대 소켓 사용 회수 초과 (사이트 : %d, 번호 : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 + 1);					
				if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "MAX SOCKET USE COUNT OFF (SITE : %d, NUM : %d", (iIdx + nIdxAdd ) / 5, (iIdx + nIdxAdd) * 2 + 1);
				st_msg.mstr_event_msg[0] = strErr;
				::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				
				//g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2, false );
				g_site.SetEnableSocket( (iIdx + nIdxAdd) * 2 + 1, -1 );//2016.0712
				//2015.1005
				g_client_bpc.SetSocketOnOff( (iIdx + nIdxAdd) * 2 + 1 );
				
				//2017.0602
				if(st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
				{
					strSokSerial = g_site.GetSocketSerial( (iIdx + nIdxAdd ) * 2 + 1, st_basic.n_cok_type );
					if( strSokSerial != "") g_client_xgem.SetSocketOff( strSokSerial );
				}

				//kwlee 2015.1008 log 추가
				str_log.Format("Socket Use : %d : OFF",(iIdx + nIdxAdd) * 2 + 1);
				Func.On_LogFile_Add(99, str_log);
				
				//2013,0321
				bool bAllFail = true;
				for( int i=0; i<TSITE_INSERT_CNT; i++ )
				{//2016.0709
					// 					if( g_site.GetEnableSocket( i * 2 ) == false &&
					// 						g_site.GetEnableSocket( i * 2 + 1 ) == false )
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
					strErr.Format( "모든 Testet socket을 사용할 수 없습니다. 테스터를 체크해 주세요");
					if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "It can't use Tester socket. Check tester plaese.");
					st_msg.mstr_event_msg[0] = strErr;
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
					COMI.mn_run_status = dSTOP;
				}
				bSaveTest = true;
			}
		}

		//2016.0711
		if( bSaveTest == true )
		{
			Func.SaveSocketSkipRule();
		}

		// display yield info
		if (st_handler.cwnd_main)
		{
			st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_COUNTINFO, 0);
		}
		//2013,0110
		if(st_handler.mn_pogo_type == CTL_YES && st_basic.n_mode_pogo == CTL_YES)
		{
			m_step_test[iIdx] = ETEST_POGO_OUT;
		}
		else
		{
			m_step_test[iIdx] = ETEST_INSERT_OUT;
		}
		break;

	case ETEST_POGO_OUT:
		if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			break;
		
		ReqPogo(iIdx, BACKWARD );
		m_step_test[iIdx] = ETEST_POGO_OUT_CHK;
		break;

	case ETEST_POGO_OUT_CHK:
		if( GetPogo(iIdx) != FB_NONE )		return;
		
		m_step_test[iIdx] = ETEST_INSERT_OUT;
		break;

	case ETEST_INSERT_OUT:
		if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			break;

		ReqInsert(iIdx, BACKWARD );
		m_step_test[iIdx] = ETEST_INSERT_OUT_CHK;
		break;

	case ETEST_INSERT_OUT_CHK:
		if( GetInsert(iIdx) != FB_NONE )		return;

		m_step_test[iIdx] = ETEST_FINISH;
		break;

	case ETEST_FINISH:
		g_site.GetModule((iIdx  + nIdxAdd)* 2).CalcModuleState_TestEnd();		
		g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).CalcModuleState_TestEnd();

		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
		//2015.0914
		isOutStandBy[(iIdx)* 2] = CTL_YES;
		isOutStandBy[(iIdx)* 2 + 1] = CTL_YES;
		/////
		

		//2017.0602
		if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES )
		{
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2).IsExist() )
			{
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2).GetTestCnt() > 1 )//retest
				{
					g_client_xgem.SetDVCRetestResult( g_site.GetModule((iIdx  + nIdxAdd)* 2).GetTrayNum(),g_site.GetModule((iIdx  + nIdxAdd)* 2).GetSerial(), (iIdx  + nIdxAdd)* 2 );
				}
				else
				{
					g_client_xgem.SetDVCTestEnd( (iIdx  + nIdxAdd)* 2, g_site.GetModule((iIdx  + nIdxAdd)* 2).GetSerial() );
				}
			}
			if( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).IsExist() )
			{
				if( g_site.GetModule( (iIdx + nIdxAdd ) * 2 + 1).GetTestCnt() > 1 )//retest
				{
					g_client_xgem.SetDVCRetestResult( g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetTrayNum(), g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetSerial(), (iIdx  + nIdxAdd)* 2 + 1);
				}
				else
				{
					g_client_xgem.SetDVCTestEnd( (iIdx  + nIdxAdd)* 2 + 1, g_site.GetModule((iIdx  + nIdxAdd)* 2 + 1).GetSerial() );
				}
			}
		}

		m_step_test[iIdx] = ETEST_NONE;
		break;
	}
}

void CRun_Test_Site::ReqMoveToSocket()
{
	m_step_move = EMOVE_Z_CHK;	
}

void CRun_Test_Site::Run_Move_ToSocket()
{
	if( m_step_move_old != m_step_move )
	{
		m_step_move_old = m_step_move;
		m_dwTime_Move = GetCurrentTime();

		if( Run_Move_ToSocket_JamChk() == false )
			return;
	}

	if( Run_Move_ToSocket_TimeChk() == false )
		return;

	Func.ThreadFunctionStepTrace(30, m_step_move);
	switch( m_step_move )
	{
	case EMOVE_Z_NONE:			break;

	case EMOVE_Z_CHK:
		{
			int nMotor = MOTOR_TESTSITE_1_Z + m_nSiteID - INIT_TEST_SITE_1;
			if( abs(g_comiMgr.Get_MotCurrentPos( nMotor ) - st_motor[nMotor].d_pos[P_SITE_SOCKET]) < 0.01f )
				m_step_move = EMOVE_FINISH;
			else
				m_step_move = EMOVE_Z_BACK_PLATE;
		}
		break;

	case EMOVE_Z_BACK_PLATE:
		ReqPlate( BACKWARD );
		m_step_move = EMOVE_Z_BACK_PLATE_CHK;

	case EMOVE_Z_BACK_PLATE_CHK:
		if( GetPlate() != FB_NONE )		break;

		m_step_move = EMOVE_Z_MOVE;
		break;

	case EMOVE_Z_MOVE:
		{
			int nMotor = MOTOR_TESTSITE_1_Z + m_nSiteID - INIT_TEST_SITE_1;
			int nRet = CTL_Lib.Single_Move( nMotor, st_motor[nMotor].d_pos[P_SITE_SOCKET], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_move = EMOVE_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMS%d10", m_nSiteID+1 );
				strJamCode.Format( "452%d10", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 664, CTL_dWARNING, strJamCode );
			}
		}
		break;
	
	case EMOVE_FINISH:
		m_step_move = EMOVE_Z_NONE;
		break;
	}
}

bool CRun_Test_Site::IsPlateForward()
{
	int iForward = st_io.i_testsite1_cok_plate_fwd_chk;
	int iBackward = st_io.i_testsite1_cok_plate_bwd_chk;
		
	if( m_nSiteID == INIT_TEST_SITE_2 )
	{
		iForward = st_io.i_testsite2_cok_plate_fwd_chk;
		iBackward = st_io.i_testsite2_cok_plate_bwd_chk;
	}
	
	if( g_ioMgr.get_in_bit( iForward ) == IO_OFF )
		return false;

	if( g_ioMgr.get_in_bit( iBackward ) == IO_ON )
		return false;
	
	return true;
}

bool CRun_Test_Site::Run_Move_Test_JamChk( int iIdx )
{
	// 로봇 x,y
	int nIdxAdd = (m_nSiteID - INIT_TEST_SITE_1) * 5;
	switch( m_step_test[iIdx] )
	{
	case ETEST_NONE:
	case ETEST_MUSTTEST_CHK:
		break;

	case ETEST_READY:
	case ETEST_READY_CHK:	
	case ETEST_SEND_READY:
	case ETEST_RECV_READY:
		if( Chk_Insert( m_nSiteID - INIT_TEST_SITE_1, iIdx, IO_OFF ) == false )		return false;
		if( Chk_Plate( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )				return false;
		if( Chk_Lock( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )					return false;
		if( Chk_HifixClamp( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )			return false;
		break;

	case ETEST_INSERT_IN:
	case ETEST_INSERT_IN_CHK:
		if( Chk_Plate( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )				return false;
		if( Chk_Lock( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )					return false;
		if( Chk_HifixClamp( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )			return false;
		break;

	case ETEST_SEND_TEST_START:
	case ETEST_RECV_TEST_START:
	case ETEST_TEST_END_CHK:
	case ETEST_TEST_TIMEOUT:
	case ETEST_TEST_TIMEOUT_CHK:
	case ETEST_TEST_END:
		if( Chk_Insert( m_nSiteID - INIT_TEST_SITE_1, iIdx, IO_ON ) == false )			return false;
		if( Chk_Plate( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )				return false;
		if( Chk_Lock( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )					return false;
		if( Chk_HifixClamp( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )			return false;
		break;

	case ETEST_INSERT_OUT:
	case ETEST_INSERT_OUT_CHK:
		if( Chk_Plate( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )				return false;
		if( Chk_Lock( m_nSiteID - INIT_TEST_SITE_1, IO_ON ) == false )					return false;
		if( Chk_HifixClamp( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )			return false;
		break;

	case ETEST_FINISH:
		break;
	}

	return true;
}

bool CRun_Test_Site::Chk_Insert( int nSite, int iIdx, int iOn )
{
	int nO_InsertFwd = st_io.o_testsite1_cok_insert_fwdbwd[iIdx];
	int nI_InsertFwdChk = st_io.i_testsite1_cok_insert_fwd_chk[iIdx];
	int nI_InsertBwdChk = st_io.i_testsite1_cok_insert_bwd_chk[iIdx];
	if( nSite == 1 )
	{
		nO_InsertFwd = st_io.o_testsite2_cok_insert_fwdbwd[iIdx];
		nI_InsertFwdChk = st_io.i_testsite2_cok_insert_fwd_chk[iIdx];
		nI_InsertBwdChk = st_io.i_testsite2_cok_insert_bwd_chk[iIdx];
	}
	
	if( g_ioMgr.get_out_bit( nO_InsertFwd, iOn ) == iOn &&
		g_ioMgr.get_in_bit( nI_InsertFwdChk, iOn ) == iOn &&
		g_ioMgr.get_in_bit( nI_InsertBwdChk, !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TS_Insert_Chk( nSite, iIdx, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 665, dWARNING, strJamCode );
	return false;
}

bool CRun_Test_Site::Chk_Plate( int nSite, int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_testsite_cok_plate_fwd[nSite], iOn ) == iOn &&
		g_ioMgr.get_out_bit( st_io.o_testsite_cok_plate_bwd[nSite], !iOn ) != iOn &&
		g_ioMgr.get_in_bit( st_io.i_testsite_cok_plate_fwd_chk[nSite], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_testsite_cok_plate_bwd_chk[nSite], !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TS_Plate_Chk( nSite, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 666, dWARNING, strJamCode );
	return false;
}

bool CRun_Test_Site::Chk_Lock( int nSite, int iOn )
{
	int nO_TS_Lock = st_io.o_testsite1_cok_lockunlock;
	int nI_TS_LockChk = st_io.i_testsite1_cok_lock_chk;
	int nI_TS_UnlockChk = st_io.i_testsite1_cok_unlock_chk;
	if( nSite == 1 )
	{
		nO_TS_Lock = st_io.o_testsite2_cok_lockunlock;
		nI_TS_LockChk = st_io.i_testsite2_cok_lock_chk;
		nI_TS_UnlockChk = st_io.i_testsite2_cok_unlock_chk;
	}
	
	if( g_ioMgr.get_out_bit( nO_TS_Lock, iOn ) == iOn &&
		g_ioMgr.get_in_bit( nI_TS_LockChk, iOn ) == iOn &&
		g_ioMgr.get_in_bit( nI_TS_UnlockChk, !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TS_Lock_Chk( nSite, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 667, dWARNING, strJamCode );
	return false;
}

bool CRun_Test_Site::Chk_HifixClamp( int nSite, int iOn )
{
	int nO_TS_Clamp = st_io.o_testsite1_hifix_clamp;
	int nI_TS_ClampChk = st_io.i_testsite1_hifix_clamp_chk;
	int nI_TS_HiFixChk = st_io.i_testsite1_hifix_chk; 
	if( nSite == 1 )
	{
		nO_TS_Clamp = st_io.o_testsite2_hifix_clamp;
		nI_TS_ClampChk = st_io.i_testsite2_hifix_clamp_chk;
		nI_TS_HiFixChk = st_io.i_testsite2_hifix_chk;
	}
	
	if( g_ioMgr.get_out_bit( nO_TS_Clamp, iOn ) == iOn &&
		g_ioMgr.get_in_bit( nI_TS_ClampChk, !iOn ) != iOn &&
		g_ioMgr.get_in_bit( nI_TS_HiFixChk, !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_HF_Clamp_Chk( nSite, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 668, dWARNING, strJamCode );
	return false;	
}

bool CRun_Test_Site::Run_Move_ToSocket_JamChk()
{
	switch( m_step_move )
	{
	case EMOVE_Z_NONE:
	case EMOVE_Z_CHK:
	case EMOVE_Z_BACK_PLATE:
	case EMOVE_Z_BACK_PLATE_CHK:
		break;

	case EMOVE_Z_MOVE:
		if( Chk_Plate( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )		return false;
		if( Chk_HifixClamp( m_nSiteID - INIT_TEST_SITE_1, IO_OFF ) == false )	return false;
		break;
	
	case EMOVE_FINISH:
		break;
	}
	return true;
}

bool CRun_Test_Site::Run_Move_ToSocket_TimeChk()
{
	switch( m_step_move )
	{
	case EMOVE_Z_MOVE:
		if( InTime_ToSocket( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 669, dWARNING, "625000" );
			return false;
		}
	}

	return true;
}

bool CRun_Test_Site::InTime_ToSocket( int iMilliSec )
{
	//2012,1220
	if( GetCurrentTime() - m_dwTime_Move < 0 )
	{
		m_dwTime_Move = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_Move < iMilliSec )
		return true;

	return false;
}

bool CRun_Test_Site::Chk_Robot_Safety()
{
	if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "SM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 670, dWARNING, "470000" );
		return false;
	}

	return true;
}

//2012,1225
bool CRun_Test_Site::ChkPogoOnOff( int iIdx, int iOnOff )
{
	int nO_Pogo_1 = st_io.o_testsite1_pogo_clamp[iIdx*2];
	int nO_Pogo_2 = st_io.o_testsite1_pogo_clamp[iIdx*2 + 1];
	int nI_PogoChk_1 = st_io.i_testsite1_pogo_clamp_chk[iIdx*2];
	int nI_PogoChk_2 = st_io.i_testsite1_pogo_clamp_chk[iIdx*2 + 1];
	
	if( m_nSiteID == INIT_TEST_SITE_2 )
	{
		nO_Pogo_1 = st_io.o_testsite2_pogo_clamp[iIdx*2];
		nO_Pogo_2 = st_io.o_testsite2_pogo_clamp[iIdx*2 + 1];
		nI_PogoChk_1 = st_io.i_testsite2_pogo_clamp_chk[iIdx*2];
		nI_PogoChk_2 = st_io.i_testsite2_pogo_clamp_chk[iIdx*2 + 1];
	}

// 	if(g_ioMgr.get_out_bit(nO_Pogo, iOnOff) == iOnOff &&
// 		g_ioMgr.get_in_bit(nI_PogoChk_1, !iOnOff) == !iOnOff &&
// 		g_ioMgr.get_in_bit(nI_PogoChk_2, !iOnOff) == !iOnOff)

	int nIdxAdd = (m_nSiteID - INIT_TEST_SITE_1) * 5;
	if(iOnOff == IO_ON)
	{
		if( g_site.GetModule((iIdx + nIdxAdd)* 2).IsExist() == true && g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).IsExist() == true )
		{
			
			if(g_ioMgr.get_out_bit( nO_Pogo_1, iOnOff ) == iOnOff &&
				g_ioMgr.get_out_bit( nO_Pogo_2, iOnOff ) == iOnOff &&
				g_ioMgr.get_in_bit( nI_PogoChk_1, !iOnOff ) == !iOnOff &&
				g_ioMgr.get_in_bit( nI_PogoChk_2, !iOnOff ) == !iOnOff)
			{
				return true;
			}
		}
		else if(g_site.GetModule((iIdx + nIdxAdd)* 2).IsExist() == true )
		{
			if(g_ioMgr.get_out_bit( nO_Pogo_1, iOnOff ) == iOnOff &&
				g_ioMgr.get_in_bit( nI_PogoChk_1, !iOnOff ) == !iOnOff)
			{
				return true;
			}
		}
		else if(g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).IsExist() == true )
		{
			if(g_ioMgr.get_out_bit( nO_Pogo_2, iOnOff ) == iOnOff &&
				g_ioMgr.get_in_bit( nI_PogoChk_2, !iOnOff ) == !iOnOff)
			{
				return true;
			}
		}
	}
	else
	{
// 		int iForward, iBackward;
// 		if(g_site.GetEnableSocket( iIdx * 2 ) == false)
// 		{
// 			if(g_ioMgr.get_out_bit( nO_Pogo_2, iOnOff ) == iOnOff &&
// 				g_ioMgr.get_in_bit( nI_PogoChk_2, !iOnOff ) == !iOnOff)
// 			{
// 				if(iIdx < 5)
// 				{
// 					iForward = st_io.i_testsite1_cok_plate_fwd_chk;
// 					iBackward = st_io.i_testsite1_cok_plate_bwd_chk;
// 				}				
// 				else //if( m_nSiteID == INIT_TEST_SITE_2 )
// 				{
// 					iForward = st_io.i_testsite2_cok_plate_fwd_chk;
// 					iBackward = st_io.i_testsite2_cok_plate_bwd_chk;
// 				}
// 				
// 				if( g_ioMgr.get_in_bit( iBackward ) != iOnOff &&
// 					g_ioMgr.get_in_bit( iForward ) == iOnOff )
// 				{
// 					return false;
// 				}
// 
// 				return true;
// 			}
// 			
// 		}
// 		else if(g_site.GetEnableSocket( iIdx * 2 + 1 ) == false)
// 		{
// 			if(g_ioMgr.get_out_bit( nO_Pogo_1, iOnOff ) == iOnOff &&
// 				g_ioMgr.get_in_bit( nI_PogoChk_1, !iOnOff ) == !iOnOff)
// 			{
// 				if(iIdx < 5)
// 				{
// 					iForward = st_io.i_testsite1_cok_plate_fwd_chk;
// 					iBackward = st_io.i_testsite1_cok_plate_bwd_chk;
// 				}				
// 				else //if( m_nSiteID == INIT_TEST_SITE_2 )
// 				{
// 					iForward = st_io.i_testsite2_cok_plate_fwd_chk;
// 					iBackward = st_io.i_testsite2_cok_plate_bwd_chk;
// 				}
// 				
// 				if( g_ioMgr.get_in_bit( iBackward ) != iOnOff &&
// 					g_ioMgr.get_in_bit( iForward ) == iOnOff )
// 				{
// 					return false;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if(g_ioMgr.get_out_bit( nO_Pogo_1, iOnOff ) == iOnOff &&
// 				g_ioMgr.get_out_bit( nO_Pogo_2, iOnOff ) == iOnOff &&
// 				g_ioMgr.get_in_bit( nI_PogoChk_1, !iOnOff ) == !iOnOff &&
// 				g_ioMgr.get_in_bit( nI_PogoChk_2, !iOnOff ) == !iOnOff)
// 			{
// 				return true;
// 			}
// 		}

		if(g_ioMgr.get_out_bit( nO_Pogo_1, iOnOff ) == iOnOff &&
			g_ioMgr.get_out_bit( nO_Pogo_2, iOnOff ) == iOnOff &&
			g_ioMgr.get_in_bit( nI_PogoChk_1, !iOnOff ) == !iOnOff &&
			g_ioMgr.get_in_bit( nI_PogoChk_2, !iOnOff ) == !iOnOff)
		{
			return true;
		}

	}
	//2012,1229


	return false;
	
}

void CRun_Test_Site::SetPogoOnOff( int iIdx, int iOnOff )
{
	int iInsert_1 = st_io.o_testsite1_pogo_clamp[iIdx*2];
	int iInsert_2 = st_io.o_testsite1_pogo_clamp[iIdx*2 + 1];
	if( m_nSiteID == INIT_TEST_SITE_2 )
	{
		iInsert_1 = st_io.o_testsite2_pogo_clamp[iIdx*2];
		iInsert_2 = st_io.o_testsite2_pogo_clamp[iIdx*2 + 1];
	}

	int nIdxAdd = (m_nSiteID - INIT_TEST_SITE_1) * 5;
	
	if(iOnOff == IO_ON)
	{
	 	if( g_site.GetModule((iIdx + nIdxAdd)* 2).IsExist() == true )
			g_ioMgr.set_out_bit( iInsert_1, iOnOff );
	 	if( g_site.GetModule((iIdx + nIdxAdd)* 2 + 1).IsExist() == true) 
			g_ioMgr.set_out_bit( iInsert_2, iOnOff );
	}
	else
	{
			g_ioMgr.set_out_bit( iInsert_1, iOnOff );
			g_ioMgr.set_out_bit( iInsert_2, iOnOff );
	}
}

void CRun_Test_Site::ReqPogo( int iIdx, EFB efb )
{
	ASSERT( iIdx > -1);
	
	m_fbPogoOnOff[iIdx] = efb;
	m_step_pogo[iIdx] = ESTEP_CYL_START;
}

void CRun_Test_Site::Run_Move_Pogo(int iIdx)
{
	if( m_fbPogoOnOff[iIdx] == FB_NONE )	return;
	
	int iOnOff = IO_ON;
	if( m_fbPogoOnOff[iIdx] == BACKWARD )
		iOnOff = IO_OFF;
	
	switch( m_step_pogo[iIdx] )
	{
	case ESTEP_CYL_START:	m_step_pogo[iIdx] = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			SetPogoOnOff(iIdx, iOnOff);
			if(COMI.mn_run_status == dRUN) st_work.mn_site_pogo[m_nSiteID - INIT_TEST_SITE_1][iIdx] = iOnOff;
			m_step_pogo[iIdx] = ESTEP_CYL_CHK;
			m_dwTimePogoStart[iIdx] = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			if( ChkPogoOnOff( iIdx, iOnOff)) //2012,1229
			{
				m_step_pogo[iIdx] = ESTEP_CYL_FINISH;
				m_dwTimeInsert[iIdx] = GetCurrentTime();
				break;
			}
			double dCurTime = GetCurrentTime() - m_dwTimePogoStart[iIdx];
			if( GetCurrentTime() - m_dwTimePogoStart[iIdx] > st_time.n_limit_time[E_WAIT_POGO] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TS_Pogo_Time(m_nSiteID - INIT_TEST_SITE_1, iIdx, iOnOff );
 					CTL_Lib.Alarm_Error_Occurrence( 671, dWARNING, strJamCode );
//  					m_step_pogo[iIdx] = ESTEP_CYL_MOVE;
				}
			}
			else if( GetCurrentTime() - m_dwTimePogoStart[iIdx] < 0 )
			{
				m_dwTimePogoStart[iIdx] = GetCurrentTime();
			}
			
		}
		break;
	case ESTEP_CYL_FINISH:
		{
			if( GetCurrentTime() - m_dwTimePogo[iIdx] < 0 )
			{
				m_dwTimePogo[iIdx] = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimePogo[iIdx] < st_time.n_wait_time[E_WAIT_POGO][iOnOff] )
				return;
			
			m_fbPogoOnOff[iIdx] = FB_NONE;
			m_step_pogo[iIdx] = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Test_Site::OnRobot_FinalPos()
{
	mn_reinstate_step[0] = mn_reinstate_step[1] = 0;
}

void CRun_Test_Site::Run_Reinstate()
{
	if(st_work.mn_reinstate_tester_robot[m_nSiteID - INIT_TEST_SITE_1] == CTL_YES) return;
	switch(mn_reinstate_step[m_nSiteID - INIT_TEST_SITE_1])
	{
	case 0:
		if(st_work.mn_reinstate_xyz_robot == CTL_YES)
		{
			mn_reinstate_step[m_nSiteID - INIT_TEST_SITE_1] = 1000;
		}
		break;

	case 1000:
		{
			for (int i = 0; i < 5; i++)
			{
				if(st_work.mn_site_insert[m_nSiteID - INIT_TEST_SITE_1][i] == IO_ON)
				{
					ReqInsert( i, FORWARD );
				}
			}
		}
		mn_reinstate_step[m_nSiteID - INIT_TEST_SITE_1] = 1100;
		break;

	case 1100:
		if( GetInsert(0) != FB_NONE )		return;
		if( GetInsert(1) != FB_NONE )		return;
		if( GetInsert(2) != FB_NONE )		return;
		if( GetInsert(3) != FB_NONE )		return;
		if( GetInsert(4) != FB_NONE )		return;
		mn_reinstate_step[m_nSiteID - INIT_TEST_SITE_1] = 2000;
		break;

	case 2000:
		st_work.mn_reinstate_tester_robot[m_nSiteID - INIT_TEST_SITE_1] = CTL_YES;
		break;

	}
}
