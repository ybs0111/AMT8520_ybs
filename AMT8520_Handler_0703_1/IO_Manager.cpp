// IO_Manager.cpp: implementation of the IO_Manager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "IO_Manager.h"
#include "FAS_HSSI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "..\common\ShareMem.h"
#include "FastechPublic_IO.h"
#include "Srcbase\ALocalization.h"

st_io_param st_io;
IO_Manager g_ioMgr;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IO_Manager::IO_Manager()
{

}

IO_Manager::~IO_Manager()
{

}

BOOL IO_Manager::Get_In_Bit( int iIoNum )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
	{
		std::string strGet = g_sm.GetData( (SM_TYPE)iIoNum );
		if( strGet == "1" )
			bRtn = TRUE;
	}
	else
	{
		bRtn = FAS_IO.Get_In_Bit( iIoNum );
	}

	return bRtn;
}

int IO_Manager::Get_In_Bit( int iIoNum, int iDummy )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
	{
		std::string strGet = g_sm.GetData( (SM_TYPE)iIoNum );
		if( strGet == "1" )
			bRtn = TRUE;
	}
	else
	{
		bRtn = FAS_IO.Get_In_Bit( iIoNum );
	}

// 	return Get_In_Bit( iIoNum ); //2012,1229
	return bRtn;
}

int IO_Manager::Set_Out_Bit( int nIONo, BOOL bOnOff ) 
{
	int iRtn = CTLBD_RET_GOOD;

	if( st_handler.mn_virtual_mode == 1 )
	{
		std::string strOnOff = "0";
		if( bOnOff == TRUE )
			strOnOff = "1";

		g_sm.SetData( (SM_TYPE)nIONo, strOnOff );
	}
	else
	{
		iRtn = FAS_IO.Set_Out_Bit( nIONo, bOnOff );
	}
	return iRtn;
}

int IO_Manager::Set_IO_PortAutoEnable(int nMasterID, int nPortNo)
{
	int iRtn = CTL_YES;
	if( st_handler.mn_virtual_mode == 1 )
	{

	}
	else
	{
		iRtn = FAS_IO.Set_IO_PortAutoEnable( nMasterID, nPortNo );
	}

	return iRtn;
}

int IO_Manager::Set_IO_SlaveEnable(int nMasterID, int nPortNo, int nSlaveNo, BOOL bEnable)
{
	int iRtn = CTL_YES;
	if( st_handler.mn_virtual_mode == 1 )
	{

	}
	else
	{
		iRtn = FAS_IO.Set_IO_SlaveEnable( nMasterID, nPortNo, nSlaveNo, bEnable );
	}

	return iRtn;
}

BOOL IO_Manager::Get_Out_Bit( int iIoNum )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
	{
		std::string strGet = g_sm.GetData( (SM_TYPE)iIoNum );
		if( strGet == "1" )
			bRtn = TRUE;
	}
	else
	{
		bRtn = FAS_IO.Get_Out_Bit( iIoNum );
	}
	
	return bRtn;	
}

BOOL IO_Manager::Get_Out_Bit( int iIoNum, int iDummy )
{
	return Get_Out_Bit( iIoNum );
}

WORD IO_Manager::Get_In_Word(int nIONo)
{
	WORD wRtn = 0x00;
	if( st_handler.mn_virtual_mode == 1 )
	{
		for( int i=0; i<16; i++ )
		{
			std::string strGet = g_sm.GetData( (SM_TYPE) (nIONo * 100 + i) );
			if( strGet == "1" )
				wRtn |= (1 << i);
		}
	}
	else
	{
		wRtn = FAS_IO.Get_In_Word( nIONo );
	}
	return wRtn;
}

WORD IO_Manager::Get_Out_Word(int nIONo)
{
	WORD wRtn = 0x00;
	if( st_handler.mn_virtual_mode ==  1 )
	{
		for( int i=0; i<16; i++ )
		{
			std::string strGet = g_sm.GetData( (SM_TYPE) (nIONo * 100 + i) );
			if( strGet == "1" )
				wRtn |= (1 << i);
		}
	}
	else
	{
		wRtn = FAS_IO.Get_Out_Word( nIONo );
	}
	return wRtn;
}


int IO_Manager::set_out_bit( int nIONo, BOOL bOnOff )
{
	int iRtn = 0;
	if( st_handler.mn_virtual_mode == 1 )
		iRtn = Set_Out_Bit( nIONo, bOnOff );
	else
		iRtn = FAS_IO.set_out_bit( nIONo, bOnOff );

	return iRtn;
}

BOOL IO_Manager::get_in_bit( int iIoNum, int iDummy )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
		bRtn = Get_In_Bit( iIoNum, iDummy );
	else
		bRtn = FAS_IO.get_in_bit( iIoNum, iDummy );

	return bRtn;
}

BOOL IO_Manager::get_in_bit( int iIoNum, int opt, int iDummy )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
		bRtn = Get_In_Bit( iIoNum, iDummy );
	else
		bRtn = FAS_IO.Get_In_Bit( iIoNum, iDummy );
	
	return bRtn;
}

BOOL IO_Manager::get_in_bit( int iIoNum )
{
	BOOL bRtn = FALSE;
	int iDummy = IO_OFF;
	if( st_handler.mn_virtual_mode == 1 )
		bRtn = Get_In_Bit( iIoNum, iDummy );
	else
		bRtn = FAS_IO.get_in_bit( iIoNum, iDummy );

	return bRtn;
}

BOOL IO_Manager::get_out_bit( int iIoNum, int iDummy )
{
	BOOL bRtn = FALSE;
	if( st_handler.mn_virtual_mode == 1 )
		bRtn = Get_Out_Bit( iIoNum, iDummy );
	else
		bRtn = FAS_IO.get_out_bit( iIoNum, iDummy );

	return bRtn;
}

st_io_param::st_io_param()
{
	int i;

	//ASS'Y  : BIN STACKER1
	st_io.o_bin_stacker_rail_variable_fwd					= 0;			//S0000
	st_io.o_bin_stacker_rail_variable_bwd					= 1;			//S0001
	st_io.o_bin_stacker1_rail_guide_fwdbwd					= 2;			//S0002
	st_io.o_bin_stacker1_rail_pusher_fwdbwd					= 3;			//S0003
	st_io.o_bin_stacker2_rail_guide_fwdbwd					= 4;			//S0004
	st_io.o_bin_stacker2_rail_pusher_fwdbwd					= 5;			//S0005
	st_io.o_bin_stacker3_rail_guide_fwdbwd					= 6;			//S0006
	st_io.o_bin_stacker3_rail_pusher_fwdbwd					= 7;

	//2017.0102
	st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd				= 9;
	st_io.o_bin_rear_stacker2_rail_pusher_fwdbwd				= 10;
	st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd				= 11;


	st_io.o_bin_stacker_rail_guide_fwdbwd[0]				= st_io.o_bin_stacker1_rail_guide_fwdbwd;
	st_io.o_bin_stacker_rail_guide_fwdbwd[1]				= st_io.o_bin_stacker2_rail_guide_fwdbwd;
	st_io.o_bin_stacker_rail_guide_fwdbwd[2]				= st_io.o_bin_stacker3_rail_guide_fwdbwd;

	st_io.o_bin_stacker_rail_pusher_fwdbwd[0]				= st_io.o_bin_stacker1_rail_pusher_fwdbwd;
	st_io.o_bin_stacker_rail_pusher_fwdbwd[1]				= st_io.o_bin_stacker2_rail_pusher_fwdbwd;
	st_io.o_bin_stacker_rail_pusher_fwdbwd[2]				= st_io.o_bin_stacker3_rail_pusher_fwdbwd;

	st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[0]			= st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd;
	st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[1]			= st_io.o_bin_rear_stacker2_rail_pusher_fwdbwd;
	st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[2]			= st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd;


	st_io.i_bin_stacker3_rail_pusher_fwd_chk				= 12;
	st_io.i_bin_stacker3_rail_pusher_bwd_chk				= 13;

	//ASS'Y  : BIN STACKER2
	st_io.i_bin_stacker_rail_variable_fwd_chk				= 100;			//PS0100
	st_io.i_bin_stacker_rail_variable_bwd_chk				= 101;			//PS0101
	st_io.i_bin_stacker1_rail_guide_fwd_chk					= 102;			//PS0102
	st_io.i_bin_stacker1_rail_guide_bwd_chk					= 103;			//PS0103
	st_io.i_bin_stacker1_rail_pusher_fwd_chk				= 104;			//PS0104
	st_io.i_bin_stacker1_rail_pusher_bwd_chk				= 105;			//PS0105
	st_io.i_bin_stacker1_tray_chk							= 106;			//PS0106
	st_io.i_bin_stacker2_rail_guide_fwd_chk					= 107;			//PS0107
	st_io.i_bin_stacker2_rail_guide_bwd_chk					= 108;			//PS0108
	st_io.i_bin_stacker2_rail_pusher_fwd_chk				= 109;			//PS0109
	st_io.i_bin_stacker2_rail_pusher_bwd_chk				= 110;			//PS0110
	st_io.i_bin_stacker2_tray_chk							= 111;			//PS0111
	st_io.i_bin_stacker3_rail_guide_fwd_chk					= 112;			//PS0112
	st_io.i_bin_stacker3_rail_guide_bwd_chk					= 113;			//PS0113
	st_io.i_bin_stacker3_tray_chk							= 114;			//PS0114

	st_io.i_bin_stacker_rail_guide_fwd_chk[0]				= st_io.i_bin_stacker1_rail_guide_fwd_chk;
	st_io.i_bin_stacker_rail_guide_fwd_chk[1]				= st_io.i_bin_stacker2_rail_guide_fwd_chk;
	st_io.i_bin_stacker_rail_guide_fwd_chk[2]				= st_io.i_bin_stacker3_rail_guide_fwd_chk;

	st_io.i_bin_stacker_rail_guide_bwd_chk[0]				= st_io.i_bin_stacker1_rail_guide_bwd_chk;
	st_io.i_bin_stacker_rail_guide_bwd_chk[1]				= st_io.i_bin_stacker2_rail_guide_bwd_chk;
	st_io.i_bin_stacker_rail_guide_bwd_chk[2]				= st_io.i_bin_stacker3_rail_guide_bwd_chk;

	st_io.i_bin_stacker_tray_chk[0]							= st_io.i_bin_stacker1_tray_chk;
	st_io.i_bin_stacker_tray_chk[1]							= st_io.i_bin_stacker2_tray_chk;
	st_io.i_bin_stacker_tray_chk[2]							= st_io.i_bin_stacker3_tray_chk;

	st_io.i_bin_stacker_rail_pusher_fwd_chk[0]				= st_io.i_bin_stacker1_rail_pusher_fwd_chk;
	st_io.i_bin_stacker_rail_pusher_fwd_chk[1]				= st_io.i_bin_stacker2_rail_pusher_fwd_chk;
	st_io.i_bin_stacker_rail_pusher_fwd_chk[2]				= st_io.i_bin_stacker3_rail_pusher_fwd_chk;

	st_io.i_bin_stacker_rail_pusher_bwd_chk[0]				= st_io.i_bin_stacker1_rail_pusher_bwd_chk;
	st_io.i_bin_stacker_rail_pusher_bwd_chk[1]				= st_io.i_bin_stacker2_rail_pusher_bwd_chk;
	st_io.i_bin_stacker_rail_pusher_bwd_chk[2]				= st_io.i_bin_stacker3_rail_pusher_bwd_chk;
	
	//ASS'Y  : BIN1
	st_io.o_bin_tray1_align_fwdbwd							= 200;				//S0200
	st_io.o_bin_stopper1_updn								= 201;				//S0201
	st_io.o_bin_elevator1_tray_clamp						= 202;				//S0202
	st_io.i_bin_tray1_in_chk								= 204;				//PS0200
	st_io.i_bin_tray1_direction_chk							= 205;				//PS0201
	st_io.i_bin_tray1_stop_position_chk						= 206;				//PS0202
	st_io.i_bin_tray1_align_fwd_chk							= 207;				//PS0203
	st_io.i_bin_tray1_align_bwd_chk							= 208;				//PS0204
	st_io.i_bin_stopper1_up_chk								= 209;				//PS0205
	st_io.i_bin_stopper1_dn_chk								= 210;				//PS0206
	st_io.i_bin_elevator1_tray_clamp_chk					= 211;				//PS0207
	st_io.i_reject_elevator_tray_chk						= 215;				//PS0211

	//ASS'Y  : BIN2
	st_io.o_bin_tray2_align_fwdbwd							= 300;				//S0300
	st_io.o_bin_stopper2_updn								= 301;				//S0301
	st_io.o_bin_elevator2_tray_clamp						= 302;				//S0302
	st_io.i_bin_tray2_stop_position_chk						= 304;				//PS0301
	st_io.i_bin_tray2_align_fwd_chk							= 306;				//PS0302
	st_io.i_bin_tray2_align_bwd_chk							= 307;				//PS0303
	st_io.i_bin_stopper2_up_chk								= 308;				//PS0304
	st_io.i_bin_stopper2_dn_chk								= 309;				//PS0305
	st_io.i_bin_elevator2_tray_clamp_chk					= 310;				//PS0306

	st_io.i_bottom_safety_door1_chk							= 311;
	st_io.i_bottom_safety_door2_chk							= 312;
	st_io.i_bottom_safety_door3_chk							= 313;
	st_io.i_safety_motorx_chk								= 314;
	st_io.i_bottom_safety_door4_chk							= 315;

	//ASS'Y  : BIN3
	st_io.o_bin_tray3_align_fwdbwd							= 400;				//S0400
	st_io.o_bin_stopper3_updn								= 401;				//S0401
	st_io.o_bin_elevator3_tray_clamp						= 402;				//S0402
	st_io.o_reject_transfer_updn							= 403;				//S0403
	st_io.i_bin_tray3_stop_position_chk						= 405;				//PS0401
	st_io.i_bin_tray3_out_chk								= 406;				//PS0402
	st_io.i_bin_tray3_align_fwd_chk							= 407;				//PS0403
	st_io.i_bin_tray3_align_bwd_chk							= 408;				//PS0404
	st_io.i_bin_stopper3_up_chk								= 409;				//PS0405
	st_io.i_bin_stopper3_dn_chk								= 410;				//PS0406
	st_io.i_bin_elevator3_tray_clamp_chk					= 411;				//PS0407
	st_io.i_reject_transfer_tray_chk						= 413;				//PS0409
	st_io.i_reject_transfer_up_chk							= 414;				//PS0410
	st_io.i_reject_transfer_dn_chk							= 415;				//PS0411

	st_io.o_bin_tray_align_fwdbwd[0]						= st_io.o_bin_tray1_align_fwdbwd;
	st_io.o_bin_tray_align_fwdbwd[1]						= st_io.o_bin_tray2_align_fwdbwd;
	st_io.o_bin_tray_align_fwdbwd[2]						= st_io.o_bin_tray3_align_fwdbwd;
	st_io.o_bin_stopper_updn[0]								= st_io.o_bin_stopper1_updn;
	st_io.o_bin_stopper_updn[1]								= st_io.o_bin_stopper2_updn;
	st_io.o_bin_stopper_updn[2]								= st_io.o_bin_stopper3_updn;
	st_io.o_bin_elevator_tray_clamp[0]						= st_io.o_bin_elevator1_tray_clamp;
	st_io.o_bin_elevator_tray_clamp[1]						= st_io.o_bin_elevator2_tray_clamp;
	st_io.o_bin_elevator_tray_clamp[2]						= st_io.o_bin_elevator3_tray_clamp;

	st_io.i_bin_tray_stop_position_chk[0]					= st_io.i_bin_tray1_stop_position_chk;
	st_io.i_bin_tray_stop_position_chk[1]					= st_io.i_bin_tray2_stop_position_chk;
	st_io.i_bin_tray_stop_position_chk[2]					= st_io.i_bin_tray3_stop_position_chk;
	st_io.i_bin_tray_send_arrive_chk[0]						= st_io.i_bin_tray2_stop_position_chk;
	st_io.i_bin_tray_send_arrive_chk[1]						= st_io.i_bin_tray3_stop_position_chk;
	st_io.i_bin_tray_send_arrive_chk[2]						= st_io.i_bin_tray3_out_chk;
	st_io.i_bin_tray_align_fwd_chk[0]						= st_io.i_bin_tray1_align_fwd_chk;
	st_io.i_bin_tray_align_fwd_chk[1]						= st_io.i_bin_tray2_align_fwd_chk;
	st_io.i_bin_tray_align_fwd_chk[2]						= st_io.i_bin_tray3_align_fwd_chk;
	st_io.i_bin_tray_align_bwd_chk[0]						= st_io.i_bin_tray1_align_bwd_chk;
	st_io.i_bin_tray_align_bwd_chk[1]						= st_io.i_bin_tray2_align_bwd_chk;
	st_io.i_bin_tray_align_bwd_chk[2]						= st_io.i_bin_tray3_align_bwd_chk;
	st_io.i_bin_stopper_up_chk[0]							= st_io.i_bin_stopper1_up_chk;
	st_io.i_bin_stopper_up_chk[1]							= st_io.i_bin_stopper2_up_chk;
	st_io.i_bin_stopper_up_chk[2]							= st_io.i_bin_stopper3_up_chk;
	st_io.i_bin_stopper_dn_chk[0]							= st_io.i_bin_stopper1_dn_chk;
	st_io.i_bin_stopper_dn_chk[1]							= st_io.i_bin_stopper2_dn_chk;
	st_io.i_bin_stopper_dn_chk[2]							= st_io.i_bin_stopper3_dn_chk;
	st_io.i_bin_elevator_tray_clamp_chk[0]					= st_io.i_bin_elevator1_tray_clamp_chk;
	st_io.i_bin_elevator_tray_clamp_chk[1]					= st_io.i_bin_elevator2_tray_clamp_chk;
	st_io.i_bin_elevator_tray_clamp_chk[2]					= st_io.i_bin_elevator3_tray_clamp_chk;

	//ASS'Y  : ROBOT1
	st_io.o_gripper_updn									= 500;				//S0500
	st_io.o_gripper											= 501;				//S0501
	st_io.o_picker1_updn									= 502;				//S0502
	st_io.o_picker2_updn									= 503;				//S0503
	st_io.o_picker3_updn									= 504;				//S0504
	st_io.o_picker4_updn									= 505;				//S0505
	st_io.o_picker1_vacuum									= 506;				//S0506
	st_io.o_picker2_vacuum									= 507;				//S0507
	st_io.o_picker3_vacuum									= 508;				//S0508
	st_io.o_picker4_vacuum									= 509;				//S0509
	st_io.o_picker1_eject									= 510;				//S0510
	st_io.o_picker2_eject									= 511;				//S0511
	st_io.o_picker3_eject									= 512;				//S0512
	st_io.o_picker4_eject									= 513;				//S0513
	st_io.i_jog_up_switch_chk								= 514;				//PS0500
	st_io.i_jog_dn_switch_chk								= 515;				//PS0501

	for (i = 0; i < 4; i++)
	{
		st_io.o_picker_updn[3 - i]							= st_io.o_picker1_updn + i;
		st_io.o_picker_vacuum[3 - i]						= st_io.o_picker1_vacuum + i;
		st_io.o_picker_eject[3 - i]							= st_io.o_picker1_eject + i;
	}

	//ASS'Y  : ROBOT2
	st_io.i_gripper_up_chk									= 600;				//PS0600
	st_io.i_gripper_dn_chk									= 601;				//PS0601
	st_io.i_gripper_on_chk									= 602;				//PS0602
	st_io.i_gripper_off_chk									= 603;				//PS0603
	st_io.i_picker1_up_chk									= 604;				//PS0604
	st_io.i_picker1_dn_chk									= 605;				//PS0605
	st_io.i_picker2_up_chk									= 606;				//PS0606
	st_io.i_picker2_dn_chk									= 607;				//PS0607
	st_io.i_picker3_up_chk									= 608;				//PS0608
	st_io.i_picker3_dn_chk									= 609;				//PS0609
	st_io.i_picker4_up_chk									= 610;				//PS0610
	st_io.i_picker4_dn_chk									= 611;				//PS0611
	st_io.i_picker1_vacuum_chk								= 612;				//PS0612
	st_io.i_picker2_vacuum_chk								= 613;				//PS0613
	st_io.i_picker3_vacuum_chk								= 614;				//PS0614
	st_io.i_picker4_vacuum_chk								= 615;				//PS0615

	for (i = 0; i < 4; i++)
	{
		st_io.i_picker_up_chk[3 - i]							= st_io.i_picker1_up_chk + i * 2;
		st_io.i_picker_dn_chk[3 - i]							= st_io.i_picker1_dn_chk + i * 2;
		st_io.i_picker_vacuum_chk[3 - i]						= st_io.i_picker1_vacuum_chk + i;
	}

	//ASS'Y  : TESTSITE1-1
	st_io.o_testsite1_cok_plate_fwd							= 700;				//S0700
	st_io.o_testsite1_cok_plate_bwd							= 701;				//S0701
	st_io.o_testsite1_cok_lockunlock						= 702;				//S0702
	st_io.o_testsite1_cok_insert1_fwdbwd					= 703;				//S0703
	st_io.o_testsite1_cok_insert2_fwdbwd					= 704;				//S0704
	st_io.o_testsite1_cok_insert3_fwdbwd					= 705;				//S0705
	st_io.o_testsite1_cok_insert4_fwdbwd					= 706;				//S0706
	st_io.o_testsite1_cok_insert5_fwdbwd					= 707;				//S0707
	st_io.o_testsite1_hifix_clamp							= 708;				//S0708
	st_io.o_reject_stacker_guide							= 709;				//S0709
	st_io.o_reject_stacker_seperator						= 710;

	st_io.i_reject_stacker_guide_forward_chk				= 713;
	st_io.i_reject_stacker_guide_backward_chk				= 714;
	st_io.i_reject_stacker_seperator_backward_chk			= 715;

	for (i = 0; i < 5; i++)
	{
		st_io.o_testsite1_cok_insert_fwdbwd[4 - i]				= st_io.o_testsite1_cok_insert1_fwdbwd + i;
	}

	//ASS'Y  : TESTSITE1-2
	st_io.i_testsite1_cok_plate_fwd_chk						= 1000;				//PS1000
	st_io.i_testsite1_cok_plate_bwd_chk						= 1001;				//PS1001
	st_io.i_testsite1_cok_lock_chk							= 1002;				//PS1002
	st_io.i_testsite1_cok_unlock_chk						= 1003;				//PS1003
	st_io.i_testsite1_cok_insert1_fwd_chk					= 1004;				//PS1004
	st_io.i_testsite1_cok_insert1_bwd_chk					= 1005;				//PS1005
	st_io.i_testsite1_cok_insert2_fwd_chk					= 1006;				//PS1006
	st_io.i_testsite1_cok_insert2_bwd_chk					= 1007;				//PS1007
	st_io.i_testsite1_cok_insert3_fwd_chk					= 1008;				//PS1008
	st_io.i_testsite1_cok_insert3_bwd_chk					= 1009;				//PS1009
	st_io.i_testsite1_cok_insert4_fwd_chk					= 1010;				//PS1010
	st_io.i_testsite1_cok_insert4_bwd_chk					= 1011;				//PS1011
	st_io.i_testsite1_cok_insert5_fwd_chk					= 1012;				//PS1012
	st_io.i_testsite1_cok_insert5_bwd_chk					= 1013;				//PS1013
	st_io.i_testsite1_hifix_clamp_chk						= 1014;				//PS1014

	for (i = 0; i < 5; i++)
	{
		st_io.i_testsite1_cok_insert_fwd_chk[4 - i]				= st_io.i_testsite1_cok_insert1_fwd_chk + i * 2;
		st_io.i_testsite1_cok_insert_bwd_chk[4 - i]				= st_io.i_testsite1_cok_insert1_bwd_chk + i * 2;
	}

	//ASS'Y  : TESTSITE1-3
	st_io.i_testsite1_cok1_chk								= 1100;				//PS1100
	st_io.i_testsite1_cok2_chk								= 1101;				//PS1101
	st_io.i_testsite1_cok3_chk								= 1102;				//PS1102
	st_io.i_testsite1_cok4_chk								= 1103;				//PS1103
	st_io.i_testsite1_cok5_chk								= 1104;				//PS1104
	st_io.i_testsite1_cok6_chk								= 1105;				//PS1105
	st_io.i_testsite1_cok7_chk								= 1106;				//PS1106
	st_io.i_testsite1_cok8_chk								= 1107;				//PS1107
	st_io.i_testsite1_cok9_chk								= 1108;				//PS1108
	st_io.i_testsite1_cok10_chk								= 1109;				//PS1109
	st_io.i_testsite1_hifix_chk								= 1110;				//PS1110
	st_io.i_testsite1_docking_chk							= 1111;				//PS1111

	for (i = 0; i < 10; i++)
	{
		st_io.i_testsite1_cok_chk[9 - i]							= st_io.i_testsite1_cok1_chk + i;
	}

	//ASS'Y  : TESTSITE2-1
	st_io.o_testsite2_cok_plate_fwd							= 1200;				//S1200
	st_io.o_testsite2_cok_plate_bwd							= 1201;				//S1201
	st_io.o_testsite2_cok_lockunlock						= 1202;				//S1202
	st_io.o_testsite2_cok_insert1_fwdbwd					= 1203;				//S1203
	st_io.o_testsite2_cok_insert2_fwdbwd					= 1204;				//S1204
	st_io.o_testsite2_cok_insert3_fwdbwd					= 1205;				//S1205
	st_io.o_testsite2_cok_insert4_fwdbwd					= 1206;				//S1206
	st_io.o_testsite2_cok_insert5_fwdbwd					= 1207;				//S1207
	st_io.o_testsite2_hifix_clamp							= 1208;				//S1208

	for (i = 0; i < 5; i++)
	{
		st_io.o_testsite2_cok_insert_fwdbwd[4 - i]				= st_io.o_testsite2_cok_insert1_fwdbwd + i;
	}

	//ASS'Y  : TESTSITE2-2
	st_io.i_testsite2_cok_plate_fwd_chk						= 1300;				//PS1300
	st_io.i_testsite2_cok_plate_bwd_chk						= 1301;				//PS1301
	st_io.i_testsite2_cok_lock_chk							= 1302;				//PS1302
	st_io.i_testsite2_cok_unlock_chk						= 1303;				//PS1303
	st_io.i_testsite2_cok_insert1_fwd_chk					= 1304;				//PS1304
	st_io.i_testsite2_cok_insert1_bwd_chk					= 1305;				//PS1305
	st_io.i_testsite2_cok_insert2_fwd_chk					= 1306;				//PS1306
	st_io.i_testsite2_cok_insert2_bwd_chk					= 1307;				//PS1307
	st_io.i_testsite2_cok_insert3_fwd_chk					= 1308;				//PS1308
	st_io.i_testsite2_cok_insert3_bwd_chk					= 1309;				//PS1309
	st_io.i_testsite2_cok_insert4_fwd_chk					= 1310;				//PS1310
	st_io.i_testsite2_cok_insert4_bwd_chk					= 1311;				//PS1311
	st_io.i_testsite2_cok_insert5_fwd_chk					= 1312;				//PS1312
	st_io.i_testsite2_cok_insert5_bwd_chk					= 1313;				//PS1313
	st_io.i_testsite2_hifix_clamp_chk						= 1314;				//PS1314

	for (i = 0; i < 5; i++)
	{
		st_io.i_testsite2_cok_insert_fwd_chk[4 - i]				= st_io.i_testsite2_cok_insert1_fwd_chk + i * 2;
		st_io.i_testsite2_cok_insert_bwd_chk[4 - i]				= st_io.i_testsite2_cok_insert1_bwd_chk + i * 2;
	}

	//ASS'Y  : TESTSITE2-3
	st_io.i_testsite2_cok1_chk								= 1400;				//PS1400
	st_io.i_testsite2_cok2_chk								= 1401;				//PS1401
	st_io.i_testsite2_cok3_chk								= 1402;				//PS1402
	st_io.i_testsite2_cok4_chk								= 1403;				//PS1403
	st_io.i_testsite2_cok5_chk								= 1404;				//PS1404
	st_io.i_testsite2_cok6_chk								= 1405;				//PS1405
	st_io.i_testsite2_cok7_chk								= 1406;				//PS1406
	st_io.i_testsite2_cok8_chk								= 1407;				//PS1407
	st_io.i_testsite2_cok9_chk								= 1408;				//PS1408
	st_io.i_testsite2_cok10_chk								= 1409;				//PS1409
	st_io.i_testsite2_hifix_chk								= 1410;				//PS1410
	//2014,0717
	st_io.i_testsite2_docking_chk							= 1411;				//PS1411
	st_io.i_testsite1_dvc1_check							= 1412;				//PS1411
	st_io.i_testsite1_dvc2_check							= 1413;				//PS1411
	st_io.i_testsite2_dvc1_check							= 1414;				//PS1411
	st_io.i_testsite2_dvc2_check							= 1415;				//PS1411

	for (i = 0; i < 10; i++)
	{
		st_io.i_testsite2_cok_chk[9 - i]						= st_io.i_testsite2_cok1_chk + i;
	}

	st_io.i_testsite_cok_plate_fwd_chk[0]						= st_io.i_testsite1_cok_plate_fwd_chk;
	st_io.i_testsite_cok_plate_fwd_chk[1]						= st_io.i_testsite2_cok_plate_fwd_chk;
	st_io.i_testsite_cok_plate_bwd_chk[0]						= st_io.i_testsite1_cok_plate_bwd_chk;
	st_io.i_testsite_cok_plate_bwd_chk[1]						= st_io.i_testsite2_cok_plate_bwd_chk;

	st_io.o_testsite_cok_plate_fwd[0]						= st_io.o_testsite1_cok_plate_fwd;
	st_io.o_testsite_cok_plate_fwd[1]						= st_io.o_testsite2_cok_plate_fwd;
	st_io.o_testsite_cok_plate_bwd[0]						= st_io.o_testsite1_cok_plate_bwd;
	st_io.o_testsite_cok_plate_bwd[1]						= st_io.o_testsite2_cok_plate_bwd;

	//ASS'Y  : COK STACKER
	st_io.o_cok_stacker_lockunlock							= 1500;				//S1500
	st_io.i_cok_stacker_lock_chk							= 1501;				//PS1500
	st_io.i_cok_stacker_unlock_chk							= 1502;				//PS1501
	st_io.i_cok_stacker_position_chk						= 1503;				//PS1502
	st_io.i_cok_stacker1_top_chk							= 1504;				//PS1503
	st_io.i_cok_stacker1_bottom_chk							= 1505;				//PS1504
	st_io.i_cok_stacker2_top_chk							= 1506;				//PS1505
	st_io.i_cok_stacker2_bottom_chk							= 1507;				//PS1506
	st_io.i_cok_stacker3_top_chk							= 1508;				//PS1507
	st_io.i_cok_stacker3_bottom_chk							= 1509;				//PS1508
	st_io.i_cok_stacker4_top_chk							= 1510;				//PS1509
	st_io.i_cok_stacker4_bottom_chk							= 1511;				//PS1510
	st_io.i_cok_stacker5_top_chk							= 1512;				//PS1511
	st_io.i_cok_stacker5_bottom_chk							= 1513;				//PS1512
	st_io.i_cok_stacker6_top_chk							= 1514;				//PS1513
	st_io.i_cok_stacker6_bottom_chk							= 1515;				//PS1514

	for (i = 0; i < 6; i++)
	{
		st_io.i_cok_stacker_top_chk[i]						= st_io.i_cok_stacker6_top_chk - (i * 2);
		st_io.i_cok_stacker_bottom_chk[i]					= st_io.i_cok_stacker6_bottom_chk - (i * 2);
	}

	//ASS'Y  : SWITCH
	st_io.o_start_switch_lamp								= 1600;				//S1600
	st_io.o_stop_switch_lamp								= 1601;				//S1601
	st_io.o_alarm_clear_lamp								= 1602;				//S1602
	st_io.o_buzzer_off_lamp									= 1603;				//S1603
	st_io.i_start_switch_chk								= 1608;				//PS1600
	st_io.i_stop_switch_chk									= 1609;				//PS1601
	st_io.i_alarm_clear_chk									= 1610;				//PS1602
	st_io.i_buzzer_off_chk									= 1611;				//PS1603
	st_io.i_auto_mode_switch_chk							= 1612;				//PS1604
	st_io.i_manual_mode_switch_chk							= 1613;				//PS1605
	st_io.i_front_select_switch_chk							= 1614;				//PS1606
	st_io.i_rear_select_switch_chk							= 1615;				//PS1607

	//ASS'Y  : TL & DOOR
	st_io.o_main_air										= 1700;				//S1700
	st_io.o_towerlamp_green									= 1701;				//S1701
	st_io.o_towerlamp_yellow								= 1702;				//S1702
	st_io.o_towerlamp_red									= 1703;				//S1703
	st_io.o_buzzer_1										= 1704;				//S1704
	st_io.o_buzzer_2										= 1705;				//S1705
	st_io.o_door_lock										= 1706;				//S1706
	st_io.i_main_air_chk									= 1708;				//PS1700
	st_io.i_door1_chk										= 1709;				//PS1701
	st_io.i_door2_chk										= 1710;				//PS1702
	st_io.i_door3_chk										= 1711;				//PS1703
	st_io.i_door4_chk										= 1712;				//PS1704
	st_io.i_door5_chk										= 1713;				//PS1703
	st_io.i_door6_chk										= 1714;				//PS1704

	for (i = 0; i < 6; i++)
	{
		st_io.i_door_chk[i]									= st_io.i_door1_chk + i;
	}

	//ASS'Y  : EMO & CONVEYOR
	st_io.o_bin_conveyor1									= 2000;				//S2000
	st_io.o_bin_conveyor2									= 2001;				//S2001
	st_io.o_bin_conveyor3									= 2002;				//S2002
	st_io.i_mc1_chk											= 2008;				//PS2000
	st_io.i_mc2_chk											= 2009;				//PS2001
	st_io.i_emo_switch1_chk									= 2012;				//PS2004
	st_io.i_emo_switch2_chk									= 2013;				//PS2005


	for (i = 0; i < 3; i++)
	{
		if( i < 2 )
			st_io.i_emo_switch_chk[i]							= st_io.i_emo_switch1_chk + i;

		st_io.o_bin_conveyor[i]								= st_io.o_bin_conveyor1 + i;
	}

	//ASS'Y  : SMEMA
	st_io.o_front_shuttle_request_signal					= 2100;				//S2100
	st_io.o_rear_shuttle_transfer_signal					= 2101;				//S2101
	st_io.i_front_shuttle_transfer_signal_chk				= 2108;				//PS2100
	st_io.i_rear_shuttle_request_signal_chk					= 2109;				//PS2101

	//POGO
	
	st_io.i_testsite1_pogo1_unclamp_chk						= 2206;
	st_io.i_testsite1_pogo2_unclamp_chk						= 2207;
	st_io.i_testsite1_pogo3_unclamp_chk						= 2208;
	st_io.i_testsite1_pogo4_unclamp_chk						= 2209;
	st_io.i_testsite1_pogo5_unclamp_chk						= 2210;
	st_io.i_testsite1_pogo6_unclamp_chk						= 2211;
	st_io.i_testsite1_pogo7_unclamp_chk						= 2212;
	st_io.i_testsite1_pogo8_unclamp_chk						= 2213;
	st_io.i_testsite1_pogo9_unclamp_chk						= 2214;
	st_io.i_testsite1_pogo10_unclamp_chk					= 2215;
	
	for (i = 0; i < 10; i++)
	{
		st_io.i_testsite1_pogo_clamp_chk[9-i]					= st_io.i_testsite1_pogo1_unclamp_chk + i;
	}
	

	st_io.o_testsite1_pogo1_clamp							= 2300;
	st_io.o_testsite1_pogo2_clamp							= 2301;
	st_io.o_testsite1_pogo3_clamp							= 2302;
	st_io.o_testsite1_pogo4_clamp							= 2303;
	st_io.o_testsite1_pogo5_clamp							= 2304;
	st_io.o_testsite1_pogo6_clamp							= 2305;
	st_io.o_testsite1_pogo7_clamp							= 2306;
	st_io.o_testsite1_pogo8_clamp							= 2307;
	st_io.o_testsite1_pogo9_clamp							= 2308;
	st_io.o_testsite1_pogo10_clamp							= 2309;
	
	
	for (i = 0; i < 10; i++)
	{
		st_io.o_testsite1_pogo_clamp[9-i]						= st_io.o_testsite1_pogo1_clamp + i;
	}

	st_io.i_testsite2_pogo1_unclamp_chk						= 2406;
	st_io.i_testsite2_pogo2_unclamp_chk						= 2407;
	st_io.i_testsite2_pogo3_unclamp_chk						= 2408;
	st_io.i_testsite2_pogo4_unclamp_chk						= 2409;
	st_io.i_testsite2_pogo5_unclamp_chk						= 2410;
	st_io.i_testsite2_pogo6_unclamp_chk						= 2411;
	st_io.i_testsite2_pogo7_unclamp_chk						= 2412;
	st_io.i_testsite2_pogo8_unclamp_chk						= 2413;
	st_io.i_testsite2_pogo9_unclamp_chk						= 2414;
	st_io.i_testsite2_pogo10_unclamp_chk					= 2415;

	for (i = 0; i < 10; i++)
	{
		st_io.i_testsite2_pogo_clamp_chk[9-i]					= st_io.i_testsite2_pogo1_unclamp_chk + i;
	}

	st_io.o_testsite2_pogo1_clamp							= 2500;
	st_io.o_testsite2_pogo2_clamp							= 2501;
	st_io.o_testsite2_pogo3_clamp							= 2502;
	st_io.o_testsite2_pogo4_clamp							= 2503;
	st_io.o_testsite2_pogo5_clamp							= 2504;
	st_io.o_testsite2_pogo6_clamp							= 2505;
	st_io.o_testsite2_pogo7_clamp							= 2506;
	st_io.o_testsite2_pogo8_clamp							= 2507;
	st_io.o_testsite2_pogo9_clamp							= 2508;
	st_io.o_testsite2_pogo10_clamp							= 2509;
	
	
	for (i = 0; i < 10; i++)
	{
		st_io.o_testsite2_pogo_clamp[9-i]						= st_io.o_testsite2_pogo1_clamp + i;
	}
}

int IO_Manager::IO_Board_Initialize()
{
	INT HSSI_speed = 0;
	int Ret = 0;
	INT pMasterNo;

	if( !st_handler.mn_virtual_mode &&
		FAS_IO.Set_IO_BoardOpen(&pMasterNo, START_NOTHING) != STATUS_SUCCESS )
		return FALSE;
	
	if( IO_Port_AutoEnable( 0, 0 ) == FALSE )		return FALSE;
	if( IO_Port_AutoEnable( 0, 1 ) == FALSE )		return FALSE;
	if( IO_Port_AutoEnable( 0, 2 ) == FALSE )		return FALSE;


	if( IO_SlaveEnable( 0, 0, 8, TRUE ) == FALSE )	return FALSE;
	if( IO_SlaveEnable( 0, 1, 8, TRUE ) == FALSE )	return FALSE;
// 	//2013,0205
// 	st_handler.mn_pogo_type = CTL_NO;
// 	st_basic.n_mode_pogo = CTL_NO;
	if( IO_SlaveEnable( 0, 2, 7, TRUE ) == FALSE || st_handler.mn_pogo_type == CTL_NO)	//2014.0717 ysj 6->7
	{
		if( IO_SlaveEnable( 0, 2, 2, TRUE ) == FALSE )	return FALSE;
		else
		{
			st_handler.mn_pogo_type = CTL_NO;
		}
	}
	else
	{
		st_handler.mn_pogo_type = CTL_YES;
	}

	for (int i = 0; i < 3; i++)
	{
		Ret = FAS_IO.Set_IO_HSSISpeed(0, i, PORT_SPEED_10);
		Ret = FAS_IO.Get_IO_HSSISpeed(0, i, &HSSI_speed);
	}

//	FAS_IO.Set_IO_DefineWord(0, 0, 0, 0x00ff); // 0번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅
	//2017.0102
	FAS_IO.Set_IO_DefineWord(0, 0, 0, 0x0fff); // 0번 Master, 0번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 1, 0x0000); // 0번 Master, 0번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 2, 0x0007); // 0번 Master, 0번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 3, 0x0007); // 0번 Master, 0번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 4, 0x000f); // 0번 Master, 0번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 5, 0x3fff); // 0번 Master, 0번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 6, 0x0000); // 0번 Master, 0번 포트, 6번 슬레이브 , 16bit 셋팅
//	FAS_IO.Set_IO_DefineWord(0, 0, 7, 0x07ff); // 0번 Master, 0번 포트, 7번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 0, 7, 0x1fff); // 0번 Master, 0번 포트, 7번 슬레이브 , 16bit 셋팅
	
	FAS_IO.Set_IO_DefineWord(0, 1, 0, 0x0000); // 0번 Master, 1번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 1, 0x0000); // 0번 Master, 1번 포트, 1번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 2, 0x01ff); // 0번 Master, 1번 포트, 2번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 3, 0x0000); // 0번 Master, 1번 포트, 3번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 4, 0x0000); // 0번 Master, 1번 포트, 4번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 5, 0x0001); // 0번 Master, 1번 포트, 5번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 6, 0x000f); // 0번 Master, 1번 포트, 6번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 1, 7, 0x007f); // 0번 Master, 1번 포트, 7번 슬레이브 , 16bit 셋팅

	FAS_IO.Set_IO_DefineWord(0, 2, 0, 0x0007); // 0번 Master, 2번 포트, 0번 슬레이브 , 16bit 셋팅
	FAS_IO.Set_IO_DefineWord(0, 2, 1, 0x0003); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		FAS_IO.Set_IO_DefineWord(0, 2, 2, 0x003f); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
		FAS_IO.Set_IO_DefineWord(0, 2, 3, 0xffff); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
		FAS_IO.Set_IO_DefineWord(0, 2, 4, 0x003f); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
		FAS_IO.Set_IO_DefineWord(0, 2, 5, 0xffff); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅
		FAS_IO.Set_IO_DefineWord(0, 2, 6, 0x00ff); // 0번 Master, 2번 포트, 1번 슬레이브 , 16bit 셋팅 //2014.0717 ysj 
	}
	else
	{
		st_basic.n_mode_pogo = 0;
		st_handler.mn_pogo_type = CTL_NO;
	}
	SetIOBoardInit( true );
	
	return TRUE;
}

void IO_Manager::OnSet_IO_Out_Port_Clear()
{
	g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_buzzer_2, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_start_switch_lamp, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_stop_switch_lamp, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_alarm_clear_lamp, IO_OFF);
	g_ioMgr.set_out_bit(st_io.o_buzzer_off_lamp, IO_OFF);
	
	g_ioMgr.set_out_bit(st_io.o_main_air, IO_ON);
	
	if (g_ioMgr.Get_In_Bit(st_io.i_main_air_chk) != IO_ON)
	{
		g_ioMgr.set_out_bit(st_io.o_main_air, IO_OFF);
		
		Sleep(300);
		
		g_ioMgr.set_out_bit(st_io.o_main_air, IO_ON);
	}
}


int IO_Manager::IO_Port_AutoEnable( int iMasterID, int iPortNo )
{
	int nRtn = Set_IO_PortAutoEnable(iMasterID, iPortNo);
	if ( nRtn != CTL_YES)
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "%d Port, not enable", iPortNo);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}

		SetIOBoardInit( false );
		return FALSE;
	}

	return TRUE;
}

int IO_Manager::IO_SlaveEnable(int nMasterID, int nPortNo, int nSlaveCnt, BOOL bEnable)
{
	int nFailCnt = 0;
	for (int i = 0; i < nSlaveCnt; i++)
	{
		int nRet = Set_IO_SlaveEnable( nMasterID, nPortNo, i, bEnable);
		
		if (nRet != CTL_YES)
		{
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_abnormal_msg, "Slave #%d, not enable", i);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
			}
			i = -1;
			
			if (nFailCnt < 3)
			{
				nFailCnt++;
			}
			else
			{
				SetIOBoardInit( false );
				return FALSE;
			}
			continue;
		}
		
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Slave #%d, enable success", i);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 성공 출력 요청
		}
		Sleep(100);
	}

	return TRUE;
}

void IO_Manager::set_out_reverse( int nIONo )
{
	BOOL bOn = get_out_bit( nIONo, IO_ON );

	if( bOn == IO_ON )
	{
		if( nIONo == st_io.o_bin_stacker1_rail_guide_fwdbwd &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker1_tray_chk ) == IO_ON )
		{
			if ( g_local.GetLocalType() == LOCAL_ENG )//20131029
			{
				DoModal_Msg( "Tray Check." );
			}
			else
			{
				DoModal_Msg( "트레이가 있습니다." );
			}
			return;
		}
		else if( nIONo == st_io.o_bin_stacker2_rail_guide_fwdbwd &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker2_tray_chk ) == IO_ON )
		{
			if ( g_local.GetLocalType() == LOCAL_ENG )//20131029
			{
				DoModal_Msg( "Tray Check." );
			}
			else
			{
				DoModal_Msg( "트레이가 있습니다." );
			}
			return;
		}
		else if( nIONo == st_io.o_bin_stacker3_rail_guide_fwdbwd &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker3_tray_chk ) == IO_ON )
		{
			if ( g_local.GetLocalType() == LOCAL_ENG )//20131029
			{
				DoModal_Msg( "Tray Check." );
			}
			else
			{
				DoModal_Msg( "트레이가 있습니다." );
			}
			return;
		}
	}
	
	set_out_bit( nIONo, !bOn );
}

void IO_Manager::set_out_reverse( int nFwd, int nBwd )
{
	BOOL bFwd = get_out_bit( nFwd, IO_ON );
	BOOL bBwd = get_out_bit( nBwd, IO_ON );

	if( bFwd && !bBwd )
	{
		set_out_bit( nFwd, IO_OFF );
		set_out_bit( nBwd, IO_ON );
	}
	else
	{
		set_out_bit( nFwd, IO_ON );
		set_out_bit( nBwd, IO_OFF );
	}
}
