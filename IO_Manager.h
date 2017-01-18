// IO_Manager.h: interface for the IO_Manager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IO_MANAGER_H__EA6CB312_2364_436A_97C1_903DD2657C23__INCLUDED_)
#define AFX_IO_MANAGER_H__EA6CB312_2364_436A_97C1_903DD2657C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct st_io_param
{
	//ASS'Y  : BIN STACKER1
	int o_bin_stacker_rail_variable_fwd;						//S0000
	int o_bin_stacker_rail_variable_bwd;						//S0001
	int o_bin_stacker1_rail_guide_fwdbwd;						//S0002
	int o_bin_stacker1_rail_pusher_fwdbwd;						//S0003
	int o_bin_stacker2_rail_guide_fwdbwd;						//S0004
	int o_bin_stacker2_rail_pusher_fwdbwd;						//S0005
	int o_bin_stacker3_rail_guide_fwdbwd;						//S0006
	int o_bin_stacker3_rail_pusher_fwdbwd;

	//2017.0102
	int o_bin_rear_stacker1_rail_pusher_fwdbwd;
	int o_bin_rear_stacker2_rail_pusher_fwdbwd;
	int o_bin_rear_stacker3_rail_pusher_fwdbwd;

	int o_bin_stacker_rail_guide_fwdbwd[3];
	int o_bin_stacker_rail_pusher_fwdbwd[3];

	int o_bin_rear_stacker_rail_pusher_fwdbwd[3];

	int i_bin_stacker3_rail_pusher_fwd_chk;
	int i_bin_stacker3_rail_pusher_bwd_chk;


	//ASS'Y  : BIN STACKER2
	int i_bin_stacker_rail_variable_fwd_chk;					//PS0100
	int i_bin_stacker_rail_variable_bwd_chk;					//PS0101
	int i_bin_stacker1_rail_guide_fwd_chk;						//PS0102
	int i_bin_stacker1_rail_guide_bwd_chk;						//PS0103
	int i_bin_stacker1_rail_pusher_fwd_chk;						//PS0104
	int i_bin_stacker1_rail_pusher_bwd_chk;						//PS0105
	int i_bin_stacker1_tray_chk;								//PS0106
	int i_bin_stacker2_rail_guide_fwd_chk;						//PS0107
	int i_bin_stacker2_rail_guide_bwd_chk;						//PS0108
	int i_bin_stacker2_rail_pusher_fwd_chk;						//PS0109
	int i_bin_stacker2_rail_pusher_bwd_chk;						//PS0110
	int i_bin_stacker2_tray_chk;								//PS0111
	int i_bin_stacker3_rail_guide_fwd_chk;						//PS0112
	int i_bin_stacker3_rail_guide_bwd_chk;						//PS0113
	int i_bin_stacker3_tray_chk;								//PS0114

	int i_bin_stacker_rail_guide_fwd_chk[3];
	int i_bin_stacker_rail_guide_bwd_chk[3];
	int i_bin_stacker_rail_pusher_fwd_chk[3];
	int i_bin_stacker_rail_pusher_bwd_chk[3];
	int i_bin_stacker_tray_chk[3];

	//ASS'Y  : BIN1
	int o_bin_tray1_align_fwdbwd;								//S0200
	int o_bin_stopper1_updn;									//S0201
	int o_bin_elevator1_tray_clamp;								//S0202
	int i_bin_tray1_in_chk;										//PS0200
	int i_bin_tray1_direction_chk;								//PS0201
	int i_bin_tray1_stop_position_chk;							//PS0202
	int i_bin_tray1_align_fwd_chk;								//PS0203
	int i_bin_tray1_align_bwd_chk;								//PS0204
	int i_bin_stopper1_up_chk;									//PS0205
	int i_bin_stopper1_dn_chk;									//PS0206
	int i_bin_elevator1_tray_clamp_chk;							//PS0207
	int i_reject_elevator_tray_chk;								//PS0211

	//ASS'Y  : BIN2
	int o_bin_tray2_align_fwdbwd;								//S0300
	int o_bin_stopper2_updn;									//S0301
	int o_bin_elevator2_tray_clamp;								//S0302
	int i_bin_tray2_stop_position_chk;							//PS0301
	int i_bin_tray2_align_fwd_chk;								//PS0302
	int i_bin_tray2_align_bwd_chk;								//PS0303
	int i_bin_stopper2_up_chk;									//PS0304
	int i_bin_stopper2_dn_chk;									//PS0305
	int i_bin_elevator2_tray_clamp_chk;							//PS0306

	int i_bottom_safety_door1_chk;								//PS0310
	int i_bottom_safety_door2_chk;								//PS0311
	int i_bottom_safety_door3_chk;								//PS0312

	int i_safety_motorx_chk;									//PS0313
	int i_bottom_safety_door4_chk;								//PS0314

	//ASS'Y  : BIN3
	int o_bin_tray3_align_fwdbwd;								//S0400
	int o_bin_stopper3_updn;									//S0401
	int o_bin_elevator3_tray_clamp;								//S0402
	int o_reject_transfer_updn;									//S0403
	int i_bin_tray3_stop_position_chk;							//PS0401
	int i_bin_tray3_out_chk;									//PS0402
	int i_bin_tray3_align_fwd_chk;								//PS0403
	int i_bin_tray3_align_bwd_chk;								//PS0404
	int i_bin_stopper3_up_chk;									//PS0405
	int i_bin_stopper3_dn_chk;									//PS0406
	int i_bin_elevator3_tray_clamp_chk;							//PS0407
	int i_reject_transfer_tray_chk;								//PS0409
	int i_reject_transfer_up_chk;								//PS0410
	int i_reject_transfer_dn_chk;								//PS0411

	int o_bin_tray_align_fwdbwd[3];
	int o_bin_stopper_updn[3];
	int o_bin_elevator_tray_clamp[3];

	
	int i_bin_tray_stop_position_chk[3];
	int i_bin_tray_send_arrive_chk[3];
	int i_bin_tray_align_fwd_chk[3];
	int i_bin_tray_align_bwd_chk[3];
	int i_bin_stopper_up_chk[3];
	int i_bin_stopper_dn_chk[3];
	int i_bin_elevator_tray_clamp_chk[3];

	//ASS'Y  : ROBOT1
	int o_gripper_updn;											//S0500
	int o_gripper;												//S0501
	int o_picker1_updn;											//S0502
	int o_picker2_updn;											//S0503
	int o_picker3_updn;											//S0504
	int o_picker4_updn;											//S0505
	int o_picker1_vacuum;										//S0506
	int o_picker2_vacuum;										//S0507
	int o_picker3_vacuum;										//S0508
	int o_picker4_vacuum;										//S0509
	int o_picker1_eject;										//S0510
	int o_picker2_eject;										//S0511
	int o_picker3_eject;										//S0512
	int o_picker4_eject;										//S0513
	int i_jog_up_switch_chk;									//PS0500
	int i_jog_dn_switch_chk;									//PS0501

	int o_picker_updn[4];
	int o_picker_vacuum[4];
	int o_picker_eject[4];

	//ASS'Y  : ROBOT2
	int i_gripper_up_chk;										//PS0600
	int i_gripper_dn_chk;										//PS0601
	int i_gripper_on_chk;										//PS0602
	int i_gripper_off_chk;										//PS0603
	int i_picker1_up_chk;										//PS0604
	int i_picker1_dn_chk;										//PS0605
	int i_picker2_up_chk;										//PS0606
	int i_picker2_dn_chk;										//PS0607
	int i_picker3_up_chk;										//PS0608
	int i_picker3_dn_chk;										//PS0609
	int i_picker4_up_chk;										//PS0610
	int i_picker4_dn_chk;										//PS0611
	int i_picker1_vacuum_chk;									//PS0612
	int i_picker2_vacuum_chk;									//PS0613
	int i_picker3_vacuum_chk;									//PS0614
	int i_picker4_vacuum_chk;									//PS0615

	int i_picker_up_chk[4];
	int i_picker_dn_chk[4];
	int i_picker_vacuum_chk[4];

	//ASS'Y  : TESTSITE1-1
	int o_testsite1_cok_plate_fwd;								//S0700
	int o_testsite1_cok_plate_bwd;								//S0701
	int o_testsite1_cok_lockunlock;								//S0702
	int o_testsite1_cok_insert1_fwdbwd;							//S0703
	int o_testsite1_cok_insert2_fwdbwd;							//S0704
	int o_testsite1_cok_insert3_fwdbwd;							//S0705
	int o_testsite1_cok_insert4_fwdbwd;							//S0706
	int o_testsite1_cok_insert5_fwdbwd;							//S0707
	int o_testsite1_hifix_clamp;								//S0708
	int o_reject_stacker_guide;									//S0709
	int o_reject_stacker_seperator;								//S0710

	int i_reject_stacker_guide_forward_chk;
	int i_reject_stacker_guide_backward_chk;
	int i_reject_stacker_seperator_backward_chk;

	int o_testsite_cok_plate_fwd[2];
	int o_testsite_cok_plate_bwd[2];
	int o_testsite1_cok_insert_fwdbwd[5];

	//ASS'Y  : TESTSITE1-2
	int i_testsite1_cok_plate_fwd_chk;							//PS1000
	int i_testsite1_cok_plate_bwd_chk;							//PS1001
	int i_testsite1_cok_lock_chk;								//PS1002
	int i_testsite1_cok_unlock_chk;								//PS1003
	int i_testsite1_cok_insert1_fwd_chk;						//PS1004
	int i_testsite1_cok_insert1_bwd_chk;						//PS1005
	int i_testsite1_cok_insert2_fwd_chk;						//PS1006
	int i_testsite1_cok_insert2_bwd_chk;						//PS1007
	int i_testsite1_cok_insert3_fwd_chk;						//PS1008
	int i_testsite1_cok_insert3_bwd_chk;						//PS1009
	int i_testsite1_cok_insert4_fwd_chk;						//PS1010
	int i_testsite1_cok_insert4_bwd_chk;						//PS1011
	int i_testsite1_cok_insert5_fwd_chk;						//PS1012
	int i_testsite1_cok_insert5_bwd_chk;						//PS1013
	int i_testsite1_hifix_clamp_chk;							//PS1014

	int i_testsite1_cok_insert_fwd_chk[5];
	int i_testsite1_cok_insert_bwd_chk[5];

	//ASS'Y  : TESTSITE1-3
	int i_testsite1_cok1_chk;									//PS1100
	int i_testsite1_cok2_chk;									//PS1101
	int i_testsite1_cok3_chk;									//PS1102
	int i_testsite1_cok4_chk;									//PS1103
	int i_testsite1_cok5_chk;									//PS1104
	int i_testsite1_cok6_chk;									//PS1105
	int i_testsite1_cok7_chk;									//PS1106
	int i_testsite1_cok8_chk;									//PS1107
	int i_testsite1_cok9_chk;									//PS1108
	int i_testsite1_cok10_chk;									//PS1109
	int i_testsite1_hifix_chk;									//PS1110
	int i_testsite1_docking_chk;								//PS1111

	int i_testsite1_cok_chk[10];

	//ASS'Y  : TESTSITE2-1
	int o_testsite2_cok_plate_fwd;								//S1200
	int o_testsite2_cok_plate_bwd;								//S1201
	int o_testsite2_cok_lockunlock;								//S1202
	int o_testsite2_cok_insert1_fwdbwd;							//S1203
	int o_testsite2_cok_insert2_fwdbwd;							//S1204
	int o_testsite2_cok_insert3_fwdbwd;							//S1205
	int o_testsite2_cok_insert4_fwdbwd;							//S1206
	int o_testsite2_cok_insert5_fwdbwd;							//S1207
	int o_testsite2_hifix_clamp;								//S1208

	int o_testsite2_cok_insert_fwdbwd[5];

	//ASS'Y  : TESTSITE2-2
	int i_testsite2_cok_plate_fwd_chk;							//PS1300
	int i_testsite2_cok_plate_bwd_chk;							//PS1301
	int i_testsite2_cok_lock_chk;								//PS1302
	int i_testsite2_cok_unlock_chk;								//PS1303
	int i_testsite2_cok_insert1_fwd_chk;						//PS1304
	int i_testsite2_cok_insert1_bwd_chk;						//PS1305
	int i_testsite2_cok_insert2_fwd_chk;						//PS1306
	int i_testsite2_cok_insert2_bwd_chk;						//PS1307
	int i_testsite2_cok_insert3_fwd_chk;						//PS1308
	int i_testsite2_cok_insert3_bwd_chk;						//PS1309
	int i_testsite2_cok_insert4_fwd_chk;						//PS1310
	int i_testsite2_cok_insert4_bwd_chk;						//PS1311
	int i_testsite2_cok_insert5_fwd_chk;						//PS1312
	int i_testsite2_cok_insert5_bwd_chk;						//PS1313
	int i_testsite2_hifix_clamp_chk;							//PS1314

	int i_testsite2_cok_insert_fwd_chk[5];
	int i_testsite2_cok_insert_bwd_chk[5];

	//ASS'Y  : TESTSITE2-3
	int i_testsite2_cok1_chk;									//PS1400
	int i_testsite2_cok2_chk;									//PS1401
	int i_testsite2_cok3_chk;									//PS1402
	int i_testsite2_cok4_chk;									//PS1403
	int i_testsite2_cok5_chk;									//PS1404
	int i_testsite2_cok6_chk;									//PS1405
	int i_testsite2_cok7_chk;									//PS1406
	int i_testsite2_cok8_chk;									//PS1407
	int i_testsite2_cok9_chk;									//PS1408
	int i_testsite2_cok10_chk;									//PS1409
	int i_testsite2_hifix_chk;									//PS1410
	int i_testsite2_docking_chk;								//PS1411
	//2014,0717
	int i_testsite1_dvc1_check;									//PS1412
	int i_testsite1_dvc2_check;									//PS1413
	int i_testsite2_dvc1_check;									//PS1414
	int i_testsite2_dvc2_check;									//PS1415

	int i_testsite2_cok_chk[10];

	int i_testsite_cok_plate_fwd_chk[2];
	int i_testsite_cok_plate_bwd_chk[2];

	//ASS'Y  : COK STACKER
	int o_cok_stacker_lockunlock;								//S1500
	int i_cok_stacker_lock_chk;									//PS1500
	int i_cok_stacker_unlock_chk;								//PS1501
	int i_cok_stacker_position_chk;								//PS1502
	int i_cok_stacker1_top_chk;									//PS1503
	int i_cok_stacker1_bottom_chk;								//PS1504
	int i_cok_stacker2_top_chk;									//PS1505
	int i_cok_stacker2_bottom_chk;								//PS1506
	int i_cok_stacker3_top_chk;									//PS1507
	int i_cok_stacker3_bottom_chk;								//PS1508
	int i_cok_stacker4_top_chk;									//PS1509
	int i_cok_stacker4_bottom_chk;								//PS1510
	int i_cok_stacker5_top_chk;									//PS1511
	int i_cok_stacker5_bottom_chk;								//PS1512
	int i_cok_stacker6_top_chk;									//PS1513
	int i_cok_stacker6_bottom_chk;								//PS1514

	int i_cok_stacker_top_chk[6];
	int i_cok_stacker_bottom_chk[6];

	//ASS'Y  : SWITCH
	int o_start_switch_lamp;									//S1600
	int o_stop_switch_lamp;										//S1601
	int o_alarm_clear_lamp;										//S1602
	int o_buzzer_off_lamp;										//S1603
	int i_start_switch_chk;										//PS1600
	int i_stop_switch_chk;										//PS1601
	int i_alarm_clear_chk;										//PS1602
	int i_buzzer_off_chk;										//PS1603
	int i_auto_mode_switch_chk;									//PS1604
	int i_manual_mode_switch_chk;								//PS1605
	int i_front_select_switch_chk;								//PS1606
	int i_rear_select_switch_chk;								//PS1607

	//ASS'Y  : TL & DOOR
	int o_main_air;												//S1700
	int o_towerlamp_green;										//S1701
	int o_towerlamp_yellow;										//S1702
	int o_towerlamp_red;										//S1703
	int o_buzzer_1;												//S1704
	int o_buzzer_2;												//S1705
	int o_door_lock;											//S1706
	int i_main_air_chk;											//PS1700
	int i_door1_chk;											//PS1701
	int i_door2_chk;											//PS1702
	int i_door3_chk;											//PS1703
	int i_door4_chk;											//PS1704
	int i_door5_chk;
	int i_door6_chk;

	int i_door_chk[6];

	//ASS'Y  : EMO & CONVEYOR
	int o_bin_conveyor1;										//S2000
	int o_bin_conveyor2;										//S2001
	int o_bin_conveyor3;										//S2002
	int i_mc1_chk;												//PS2000
	int i_mc2_chk;												//PS2001
	int i_emo_switch1_chk;										//PS2004
	int i_emo_switch2_chk;										//PS2005

	int o_bin_conveyor[3];
	int i_emo_switch_chk[2];

	//ASS'Y  : SMEMA
	int o_front_shuttle_request_signal;							//S2100
	int o_rear_shuttle_transfer_signal;							//S2101
	int i_front_shuttle_transfer_signal_chk;					//PS2100
	int i_rear_shuttle_request_signal_chk;						//PS2101
	
	int i_testsite1_pogo1_unclamp_chk;
	int i_testsite1_pogo2_unclamp_chk;
	int i_testsite1_pogo3_unclamp_chk;
	int i_testsite1_pogo4_unclamp_chk;
	int i_testsite1_pogo5_unclamp_chk;
	int i_testsite1_pogo6_unclamp_chk;
	int i_testsite1_pogo7_unclamp_chk;
	int i_testsite1_pogo8_unclamp_chk;
	int i_testsite1_pogo9_unclamp_chk;
	int i_testsite1_pogo10_unclamp_chk;	

	int i_testsite1_pogo_clamp_chk[10];

	
	int o_testsite1_pogo1_clamp;
	int o_testsite1_pogo2_clamp;
	int o_testsite1_pogo3_clamp;
	int o_testsite1_pogo4_clamp;
	int o_testsite1_pogo5_clamp;
	int o_testsite1_pogo6_clamp;
	int o_testsite1_pogo7_clamp;
	int o_testsite1_pogo8_clamp;
	int o_testsite1_pogo9_clamp;
	int o_testsite1_pogo10_clamp;

	int o_testsite1_pogo_clamp[10];	
	
	int i_testsite2_pogo1_unclamp_chk;
	int i_testsite2_pogo2_unclamp_chk;
	int i_testsite2_pogo3_unclamp_chk;
	int i_testsite2_pogo4_unclamp_chk;
	int i_testsite2_pogo5_unclamp_chk;
	int i_testsite2_pogo6_unclamp_chk;
	int i_testsite2_pogo7_unclamp_chk;
	int i_testsite2_pogo8_unclamp_chk;
	int i_testsite2_pogo9_unclamp_chk;
	int i_testsite2_pogo10_unclamp_chk;
	
	int i_testsite2_pogo_clamp_chk[10];

	int o_testsite2_pogo1_clamp;
	int o_testsite2_pogo2_clamp;
	int o_testsite2_pogo3_clamp;
	int o_testsite2_pogo4_clamp;
	int o_testsite2_pogo5_clamp;
	int o_testsite2_pogo6_clamp;
	int o_testsite2_pogo7_clamp;
	int o_testsite2_pogo8_clamp;
	int o_testsite2_pogo9_clamp;
	int o_testsite2_pogo10_clamp;

	int o_testsite2_pogo_clamp[10];

	st_io_param();
};
extern st_io_param st_io;

class IO_Manager  
{
public:
	IO_Manager();
	virtual ~IO_Manager();

	BOOL Get_In_Bit( int iIoNum );
	BOOL Get_In_Bit( int iIoNum, int iDummy );
	BOOL Get_Out_Bit( int iIoNum );
	BOOL Get_Out_Bit( int iIoNum, int iDummy );
	int Set_Out_Bit( int nIONo, BOOL bOnOff ) ;
	int Set_IO_PortAutoEnable(int nMasterID, int nPortNo);
	int Set_IO_SlaveEnable(int nMasterID, int nPortNo, int nSlaveNo, BOOL bEnable);

	WORD Get_In_Word(int nIONo);
	WORD Get_Out_Word(int nIONo);

	int set_out_bit( int nIONo, BOOL bOnOff );
	BOOL get_in_bit( int iIoNum, int opt, int iDummy );
	BOOL get_in_bit( int iIoNum, int iDummy );
	BOOL get_in_bit( int iIoNum);
	BOOL get_out_bit( int iIoNum, int iDummy );

	void set_out_reverse( int nIONo );
	void set_out_reverse( int nFwd, int nBwd );

public:
	int IO_Board_Initialize();
	void OnSet_IO_Out_Port_Clear();


	void SetIOBoardInit( bool bVal )	{ m_bIOBoardInit = bVal; }
	bool GetIOBoardInit()	{ return m_bIOBoardInit; }

protected:
	int IO_Port_AutoEnable( int iMasterID, int iPortNo );
	int IO_SlaveEnable(int nMasterID, int nPortNo, int nSlaveCnt, BOOL bEnable);

protected:
	bool	m_bIOBoardInit;
};

extern IO_Manager g_ioMgr;

#endif // !defined(AFX_IO_MANAGER_H__EA6CB312_2364_436A_97C1_903DD2657C23__INCLUDED_)
