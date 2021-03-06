#ifndef _VARIABLE_h__
#define _VARIABLE_h__

#include "Alg_Mysql.h"
#include "CtlBd_Variable.h"

/////////////////////////////////////////////////////////
//DLL 구조로 변경예정 //2011.1007 추가 
/////////////////////////////////////////////////////////
#define BD_NO		0  //어떤 조건이 클리어된 상태 
#define BD_YES		1  //어떤 조건을 요청한 상태

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//motor, i/O 보드등의 리턴값은 이 것만 사용한다  
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//2013,1001
#define CHINA_VER   1

//2016.0329
#define P_XY_SAFETY_TRAY_LIMIT 0
#define	P_XY_SAFETY_REJ_LIMIT  1

#define RECIPE_CNT					20

#define BD_ERROR    -1
#define BD_PROCEED  0
#define BD_GOOD		1
#define BD_RETRY	2
#define BD_SAFETY	3

#define TRAY_X                  0
#define TRAY_Y                  1

#define MAX_TRAYCELL			20

#define M_ROBOT_XY_INDEX		0

#define BINCHECK_SKIP           0    // BIN 데이터 검사하지 않음 

#define PICKER_NUM			4

#define TSITE_SOCKET_CNT	20
#define TSITE_INSERT_CNT	10
#define COK_TYPE_CNT	6

#define CONNECT_NO				0
#define CONNECT_YES				1

#define MC_OFF_LINE				0
#define MC_ON_LINE_LOCAL		1
#define MC_ON_LINE_REMOTE		2


enum PICKERJOBTYPE
{
	PLACE,
	PICK,
};

enum ENUM_INIT_SITE
{
	INIT_ROBOT,
	INIT_BIN_STACKER_1,
	INIT_BIN_STACKER_2,
	INIT_BIN_STACKER_3,
	INIT_BIN_STACKER_REJECT,
	INIT_COK_ROBOT,
	INIT_REJECT_STACKER,
	INIT_REJECT_TRAY_TRANSFER,
	INIT_TEST_SITE_1,
	INIT_TEST_SITE_2,
	MAXSITE,
};

enum ENUM_SAFETY_MODE
{
	ESM_IN,
	ESM_OUT,
};


enum ENUM_SAFETY_SITE
{
	ESS_REJECT,
	ESS_TRAY,
	ESS_TESTSITE,
};

///////////////////////////////////////

#define MAX_SITE_INFO		13
#define BACK_SET_POS		800		//해당 위치를 티칭한 데이터다..

///////////////////////////////////////

enum pStakcerBin
{
	BIN_GOOD_RAIL =				1,
	BIN_STAKCER_1,
	BIN_STAKCER_2,
	BIN_STAKCER_3,
	BIN_STAKCER_4
};

enum pLoaderShifter_X
{
	P_LDS_X_PICK =				0,
	P_LDS_X_BUFFER_1,
	P_LDS_X_BUFFER_2,
	P_LDS_X_ID_CHECK,
	P_LDS_X_WARPAGE_1,
	P_LDS_X_WARPAGE_2,
	P_LDS_X_WARPAGE_3,
	P_LDS_X_WARPAGE_4,
	P_LDS_X_WARPAGE_5,
	P_LDS_X_WARPAGE_6,
	P_LDS_X_WARPAGE_7,
	P_LDS_X_WARPAGE_8
};

enum pLoaderShifter_Z
{
	P_LDS_Z_SAFETY =			0,
	P_LDS_Z_PICK,
	P_LDS_Z_BUFFER_1_PLACE,
	P_LDS_Z_BUFFER_2_PLACE,
	P_LDS_Z_WARPAGE_BOAT,
	P_LDS_Z_WARPAGE_PCB
};

enum pUnloaderShifter_X
{
	P_ULDS_X_PLACE =			0,
	P_ULDS_X_BUFFER_1,
	P_ULDS_X_BUFFER_2
};

enum pBuffer_X
{
	P_BUF_X_IN =				0,
	P_BUF_X_FLIPPER,
	P_BUF_X_OUT,
	P_BUF_X_WARPAGE_1,
	P_BUF_X_WARPAGE_2,
	P_BUF_X_WARPAGE_3,
	P_BUF_X_WARPAGE_4,
	P_BUF_X_WARPAGE_5,
	P_BUF_X_WARPAGE_6,
	P_BUF_X_WARPAGE_7,
	P_BUF_X_WARPAGE_8
};

enum pBuffer_R
{
	P_BUF_R_0 =				0,
	P_BUF_R_90,
	P_BUF_R_180,
	P_BUF_R_270,
};

enum pJigStacker
{
	P_JIG_STACKER_0 =		0,
	P_JIG_STACKER_90
};

enum pFlipper_Z
{
	P_FLIPPER_Z_SAFETY =		0,
	P_FLIPPER_Z_MSTACKER_TOP,
	P_FLIPPER_Z_MSTACKER_BOTTOM,
	P_FLIPPER_Z_MSTACKER_SAFETY_GAP,
	P_FLIPPER_Z_STATION
};

enum pFlipper_Y
{
	P_FLIPPER_Y_SAFETY =		0,
	P_FLIPPER_Y_MSTACKER,
	P_FLIPPER_Y_FLIP
};

enum pTrayTreansfer_Y
{
	P_TRAYTRANSFER_Y_LOADER =		0,
	P_TRAYTRANSFER_Y_STACKER_1,
	P_TRAYTRANSFER_Y_STACKER_2,
	P_TRAYTRANSFER_Y_STACKER_3,
	P_TRAYTRANSFER_Y_STACKER_4,
	P_TRAYTRANSFER_Y_GOOD
};

enum pTrayTreansfer_Z
{
	P_TRAYTRANSFER_Z_SAFETY =		0,
	P_TRAYTRANSFER_Z_PICK,
	P_TRAYTRANSFER_Z_PLACE
};

enum pBinStacker
{
	P_BIN_STACKER_SAFETY =		0,
	P_BIN_STACKER_BASE,
	P_BIN_STACKER_DOWN_GAP,
	P_BIN_STACKER_DOWN_LIMIT
};

enum pConveyor_Z
{
	P_CONVEYOR_Z_SAFETY =		0,
	P_CONVEYOR_Z_UP,
	P_CONVEYOR_Z_DOWN
};

enum pConveyor_X
{
	P_CONVEYOR_X_SAFETY =		0,
	P_CONVEYOR_X_IN,
	P_CONVEYOR_X_OUT
};

enum pRobot_XY
{
	P_ROBOT_XY_SAFETY =		0,
	P_ROBOT_XY_BUF_1_START,
	P_ROBOT_XY_BUF_1_END,
	P_ROBOT_XY_BUF_2_START,
	P_ROBOT_XY_BUF_2_END,
	P_ROBOT_XY_FLIPPER,
	P_ROBOT_XY_GOOD_START,
	P_ROBOT_XY_GOOD_END,
	P_ROBOT_XY_STAKCER_1_START,
	P_ROBOT_XY_STAKCER_1_END,
	P_ROBOT_XY_STAKCER_2_START,
	P_ROBOT_XY_STAKCER_2_END,
	P_ROBOT_XY_STAKCER_3_START,
	P_ROBOT_XY_STAKCER_3_END,
	P_ROBOT_XY_STAKCER_4_START,
	P_ROBOT_XY_STAKCER_4_END
};

enum pRobot_Z
{
	P_ROBOT_Z_SAFETY =		0,
	P_ROBOT_Z_BUF_1_PICK,
	P_ROBOT_Z_BUF_2_PICK,
	P_ROBOT_Z_BUF_PLACE,
	P_ROBOT_Z_FLIPPER_PICK,
	P_ROBOT_Z_FLIPPER_PLACE,
	P_ROBOT_Z_STAKCER_PLACE
};

enum pRobot_Vacuum_Pitch
{
	P_ROBOT_VP_CLOSE =		0,
	P_ROBOT_VP_OPEN
};

enum pRobot_Gripper_Pitch
{
	P_ROBOT_GP_CLOSE =		0,
	P_ROBOT_GP_OPEN
};

enum EFB
{
	FB_NONE,
	FORWARD,
	BACKWARD,
};

enum ENUM_STEP_CYL
{
	ESTEP_CYL_START = 0,
	ESTEP_CYL_MOVE = 1000,
	ESTEP_CYL_CHK = 2000,
	ESTEP_CYL_FINISH = 3000,
};


#define BIN_CLEARBIN			-1
#define BIN_PASS				1
#define BIN_REJECT				2
#define BIN_HTO					3
#define BIN_NTO					4
#define BIN_BTO					5
#define BIN_RETEST				6
#define BIN_BP					7
#define BIN_ALL					10

#define MAIN_BUFFER_DSP			34
#define MAIN_RETEST_DSP			35
#define MAIN_REJECT_DSP			36
#define MAIN_LOB_TRAY_DSP		37
#define MAIN_TRAY_DSP			38
#define MAIN_PICKER_INFO_DSP	39
#define MAIN_CUSTOMER_DSP		40
#define MAIN_AIR_BLOW			67		

#define MC_STOP					0
#define MC_RUN					1
#define MC_ALARM				2
#define MC_WARNING				3
#define MD_IDLE					4

enum LOG_TYPE
{ 
	LOG_OPERATION = 0,
	LOG_TESTINTERFACE,
	LOG_TCPIP,
	LOG_MACHINE,
	LOG_DEBUG,
	LOG_BARCODE,
	LOG_TIME,
	LOG_AUTOSOCKETOFF,
	LOG_WORK,
	LOG_ADJUSTMENT,
	LOG_HISTORY,
	LOG_PICK_FAIL,
	LOG_RUN_STOP,
	LOG_MOTOR,
	LOG_DISABLE,
	LOG_TEST,
	LOG_TOTAL = 99
};

// *****************************************************************************
// 사용자 정의 메시지 ID 선언                                                   
// *****************************************************************************
//2015.0914
#define WM_REQ						WM_USER+100	  
#define WM_APPLY					WM_USER+110
////

#define WM_FORM_CHANGE				WM_USER+200   // 화면이 전환된 때 해당 전환 상태 정보를 전송하기 위한 사용자 정의 메시지 
#define WM_STATUS_CHANGE			WM_USER+201   // 타이틀 바 화면으로 전송된 메시지 출력시키기 위한 사용자 정의 메시지 추가 
#define WM_LIST_DATA				WM_USER + 6   // 통신 메시지 출력 리스트 대화 상자에 

#define WM_DATA_SEND				WM_USER + 7   // Data를 전송하기 위한 화면에서 정보를 전송하기 위해 정의한 사용자 정의 함수
#define WM_WORK_END					WM_USER + 8   // 테스트 작업 완료 시 정보를 메인 화면에 출력시키기 위한 사용자 정의 메시지 추가 
#define WM_SCRAP_DSP				WM_USER + 9

#define WM_BASIC_APPLY				WM_USER + 10  // BASIC DATA를 APPLY 시키기 위한 사용자 정의 메시지 추가 
#define WM_MAINTENANCE_APPLY		WM_USER + 11  // Maintenance DATA를 APPLY 시키기 위한 사용자 정의 메시지 추가
#define WM_TESTREFERENCE_APPLY		WM_USER + 12
#define WM_INTERFACE_APPLY			WM_USER + 13
#define	WM_WAITTIME_APPLY			WM_USER + 14  // Wait Time DATA를 APPLY 시키기 위한 사용자 정의 메시지 추가 

#define	WM_MOTORTEACH_APPLY			WM_USER + 15
#define	WM_SHIFTER_APPLY			WM_USER + 16
#define	WM_MOTORSPEED_APPLY			WM_USER + 17

#define WM_SERIAL_PORT				WM_USER + 18
#define WM_NETWORK_MSG				WM_USER + 19

#define WM_MAINFRAME_WORK			WM_USER + 20
#define WM_WORK_MSG					WM_USER + 21

#define DIAGNOSE_START				WM_USER + 22
#define DIAGNOSE_END				WM_USER + 23

#define SELF_DIAGNOSE_END			WM_USER + 25
#define	WM_MAIN_EVENT				WM_USER + 26
#define WM_TRACKOUT_DSP				WM_USER + 27

#define WM_RECEIVED_NEW_LOT			WM_USER + 36
#define WM_DRAW_LOT_TIME			WM_USER + 37
#define WM_DRAW_WORK_SITE			WM_USER + 38

//2016.0129
#define WM_COMM_FTP					WM_USER + 40

enum ENUM_WM_MESSAGE
{
	WM_DRAW_UI_MAIN					= WM_USER + 39,
	WM_DRAW_DATA_MAIN,
	WM_DRAW_DATA_LOT,
	WM_DRAW_DATA_TESTSITE,
	WM_MOTOR_COMPLETION,
	WM_SOCKET_ACCEPT,

	WM_INIT_BTN,

	WM_CHANGE_VIEW_TYPE,
	WM_DRAW_DATA_XGEM,
};

enum ENUM_DRAW_UI_MAIN
{
	EDDC_SITE,
	EDDC_COKSTACKER,
	
	EDDT_INIT,
	EDDT_REJ_FLOOR,
	EDDT_SEL_TRAY,
	EDDT_BINTRAY_0,
	EDDT_BINTRAY_1,
	EDDT_BINTRAY_2, //ybs

	EDDR_PICKER,
	EDDS_TESTSITE,
	EDDS_VIDEO,

	EDDN_NETUSABLE,
};

enum ENUM_CHANGE_VIEW_TYPE
{
	ECVT_TESTSITE,
	ECVT_MODULE,
};

enum ENUM_NET_STATE
{
	ENS_NONE,
	ENS_RECEIVED,
	ENS_SENDED,
	ENS_RECV_RPY,
};

// *****************************************************************************
//  Main Screen 출력용 메세지 정의                                              
// *****************************************************************************
#define MAIN_TIMEINFO		9
#define MAIN_COUNTINFO		10
#define MAIN_LOTINFO		21
#define MAIN_CYCLETIME		22
#define MAIN_FTPDOWN        23//2015.0604

// *****************************************************************************
#define VARIABLE_INIT		0

#define GMS_PORT			1
#define MAX_PORT			1

#define COM_ERROR			-1		// 에러 메시지

//2015.0914
#define  WM_DAY1_DATA          1
#define  WM_DAY3_DATA           2
#define  WM_DAY7_DATA           3
#define  WM_MONTH1_DATA         4
#define  WM_MONTH2_DATA         5
#define  WM_MONTH3_DATA         6
#define  WM_MONTHCAL_DATA       7
#define  WM_FILLRATIO_DATA       8
#define  WM_NOW_DATA			9
/////////

#define IO_OFF				0
#define IO_ON				1

#define TMR_IO_CHECK				9

// 메뉴얼 화면에서 사용한다.

#define HOMING				1
#define MOVING				2
#define POS_MOVING			4

// ******************************************************************************
// 장비 동작 제어 시 필요한 메시지 정의                                          
// ******************************************************************************
#define dSTOP			0
#define dRUN			1
#define dJAM			2
#define dLOTEND			3
#define dINIT			4
#define dWARNING		5
#define dLOCK			6
#define dSELFCHECK		7
#define dMAINT			8
#define dWAIT			9	// 운전중인데 자재가없어서 놀 때
#define dREINSTATE		10

#define eWARNING		0
#define eRUNNING		1
#define eSTOP			2
#define eERROR			3

// *****************************************************************************
//  Title Bar 출력용 메세지 정의                                                
// *****************************************************************************
#define FORM_NAME			1
#define MACHINE_STATUS		2
#define RETEST_MODE			3
#define DEVICE_MODE			4
#define LEVEL_MODE			5
#define FRONT_KEY_MODE		6
#define REAR_KEY_MODE		7
#define ONLINE_MODE			8
#define EMPTYTRAY_MODE		9
#define AOTO_TIMEOUT_MODE	10


// *****************************************************************************
//  List Box 출력용 메세지 정의                                                 
// *****************************************************************************
#define RECEIVE_MSG			1
#define SEND_MSG			2
#define NORMAL_MSG			3
#define ABNORMAL_MSG		4

#define MACHINE_INFO		5
#define DEVICE_INFO			6

#define TCPIP_MSG			7

#define SOCKETCLOSE			14

// ******************************************************************************
// I/O 모듈 갯수 및 포트 최대 갯수 선언한다                                      
// ******************************************************************************
#define MAX_IN			716
#define MAX_IO			716

// ******************************************************************************
//  MOTOR 보드 관련 변수 선언한다                                                
// ******************************************************************************
#define PLUS			1
#define MINUS			0

// ******************************************************************************
// 자재 테스트 및 소팅 관련 갯수 선언한다                                        
// ******************************************************************************
#define TESTSIZE		10

// ******************************************************************************
// SOCKET DATA관련.
#define LOADTRAY_ROW	30
#define LOADTRAY_COL	3
#define UNLOADTRAY_ROW	30
#define UNLOADTRAY_COL	3
#define REJECTTRAY_ROW	30
#define REJECTTRAY_COL	3

#define BUFFERMAXSIZE	20		// 버퍼 각각의 등급 구분 정보[0][x] 및 모듈 유/무 정보[1][x]를 가진다 

#define BIN_GOOD		1
#define BIN_REJECT		2

//MOT Speed Setting  
#define	MOT_SPD_VEL			0
#define	MOT_SPD_ACC			1
#define MOT_SPD_DEC			2
#define MOT_SPD_JOG			3
#define MOT_SPD_ORG			4
#define MOT_SPD_FEEDRATE	5
#define MOT_SPD_INTERACC	6

//motor sensor 
#define MOT_SENS_HOME		0
#define MOT_SENS_ELM		1
#define MOT_SENS_ELP		2
#define MOT_SENS_SD			3 //slow down sensor 

#define MOT_RTY_CNT			4	//파스텍 제어기 관련 문제시 재 시도하는 횟수

#define MOT_TIMEOUT			90000   //1분의 time out를 가지고 동작한다  //홈 체크시 사용 

//에러등 메세지 지정시사용 
#define MOT_ERR_MSG			1
#define MOT_ERR_CODE		2

///////////////////////////////////
// IO Board Define
//////////////////////////////////

#define RET_PROCEED		0
#define RET_GOOD		1
#define RET_ERROR		2
#define RET_RETRY		3
#define RET_DOOROPEN	4		// Door Open이 되어 있음. 2K6/06/20/ViboX
#define RET_MOVEGOOD	5

///////////////////////////////////////////
// Control Board(IO, Motor) Board Define
///////////////////////////////////////////

#define HANDLER_BACKUP_DATA_RW	29
#define DATA_READ    0
#define DATA_WRITE   1

// 칼라 정의
#define SKY_C					RGB(187, 250, 255)
#define SKY_D					RGB(0, 255, 255)
#define SKY_L					RGB(128, 255, 255)
#define NOR_C					RGB(164, 164, 164)
#define NOR_L					RGB(190, 190, 190)
#define RED_C					RGB(255, 0, 0)
#define RED_L					RGB(255, 150, 150)
#define RED_D					RGB(80, 20, 0)
#define RED_A					RGB(200, 0, 0)
#define BLUE_C					RGB(0, 0, 255)
#define BLUE_A					RGB(0, 0, 200)
#define WHITE_C					RGB(255, 255, 255)
#define BLACK_C					RGB(25, 1, 1)
#define BLACK_L					RGB(62, 55, 55)
#define YELLOW_C				RGB(255,255,0)
#define YELLOW_D				RGB(128, 128, 0)
#define YELLOW_DD				RGB(64, 64, 0)
#define YELLOW_L				RGB(255, 255, 128)
#define GREEN_C					RGB(0,255,0)
#define GREEN_B					RGB(0,255,64)
#define GREEN_L					RGB(150,255,150)
#define GREEN_D					RGB(0,100,20)
#define DSERBLE_BC				RGB(210,210,230)
#define TEXT_BC					RGB(220,230,210)
#define OK_BC					RGB(0, 0, 255)
#define NG_BC					RGB(255, 0, 0)
#define RESULT_BC				RGB(40, 1, 1)
#define ORANGE_C				RGB(0xFF, 0x68, 0x20)
#define ORANGE_L				RGB(255, 200, 128)
#define BLACK					RGB(0, 0, 0)
#define BLUE_D					RGB(0, 0, 128)
#define BLUE				    RGB(0, 0, 255)
#define GRAY_C					RGB(192, 192, 192)
//#define BLUE_L					RGB(150, 150, 255)
#define BLUE_L					RGB(124, 139, 252)
#define ON_C					RGB(0, 255, 0)
#define OFF_C					RGB(0, 150, 0)
#define NG_C					0x0000FF
#define OK_C					0xFF00
#define BLACK_GC				0x00
#define YELLOW_GC				0xFFFF
#define TEST_BC					RGB(194, 231, 150)
#define IN_BC					RGB(204, 153, 255)
#define PINK_D					RGB(252, 233, 218)
#define COL_PUPLE				RGB(204,102,204)

#define COL_COK_0				RED_L
#define COL_COK_1				ORANGE_L
#define COL_COK_2				YELLOW_L
#define COL_COK_3				GREEN_L
#define COL_COK_4				BLUE_L
#define COL_COK_5				COL_PUPLE


//Grid 정의 
#define DATE					0
#define EDIT					1
#define COMBO					2
#define BUTTON					3
#define STATIC					4
#define TIME					5
#define PERCENT					6
#define CHECK1					7
#define NUMBER					8
#define NUMBER1					9

#define IO_IN					1
#define IO_OUT					0
#define IO_NONE					2

// Client 소켓 Event
#define CLIENT_REV_MSG			WM_USER+1103

// *****************************************************************************
//  네트웍 메세지 정의 장비쪽에서 BPC로 보내는 메세지.
// *****************************************************************************
#define NW_TRACK_IN						95
#define NW_PCBOX_TIMEOUT				96
#define NW_PCBOX_OFF					12
#define NW_TRACK_OUT					97
#define NW_BPC_BUFFER_READY				151
#define NW_BPC_BUFFER_IN				152
// *****************************************************************************

////////////////////////////////////////////////////////////////////////////////
// AMT8480 전용
////////////////////////////////////////////////////////////////////////////////
#define NW_LOT_DISPLAY					25		// 서버에 해당 LOT의 정보를 요청하여, 이를 바탕으로 Test 기분 적용 및 수량 정보 활용
#define NW_LOT_DISPLAY_RPY				46		
#define NW_DELETE_SCRAP					109		// 서버에 저장된 불량 정보를 삭제 함.
#define NW_DELETE_SCRAP_RPY				110
#define NW_SORTER_LOT_END				111		// Sorter에서 서버에 Lot End를 보내 전산 처리를 함.
#define NW_SORTER_LOT_END_RPY			112
// *****************************************************************************
//  네트웍 메세지 정의 BPC쪽에서 장비로 보내는 메세지.
// *****************************************************************************
#define NW_TEMP_CONTROL					93
#define NW_PCBOX_START					31
#define NW_PGM_START					60
#define NW_PCBOX_END					32
// *****************************************************************************
// *****************************************************************************
//  네트웍 메세지 정의 BPC<->EPC 서로 주고받는 메세지.
// *****************************************************************************
#define NW_PCBOX_STATUS					98
#define NW_PCBOX_DOWN					99
#define NW_LOT_CANCEL					30

#define NW_PCBOX_STATUS_SEND			102
#define NW_PCBOX_DOWN_SEND				103
#define NW_LOT_CANCEL_SEND				104
// *****************************************************************************

// *****************************************************************************
//  네트웍 메세지 정의. 장비간 통신.
// *****************************************************************************
#define NW_BUFFER_READY					201
#define NW_BUFFER_IN					202
#define NW_RETEST_REQ					219

#define NW_SOCKET_CHK_SEND				214
#define NW_BUFFER_READY_SEND			215
#define NW_BUFFER_IN_SEND				216

#define NW_BUFFER_IN_RPY				217
#define NW_BUFFER_READY_RPY				218
#define NW_RETEST_REQ_RPY				220
// *****************************************************************************

#define NW_TRACK_IN_RPY					203
#define NW_PCBOX_TIMEOUT_RPY			204
#define NW_PCBOX_OFF_RPY				206
#define NW_LAST_RETEST_RPY				207
#define NW_TRACK_OUT_RPY				208		// 여기까진 EPC의 답변

#define NW_PCBOX_STATUS_RPY				209
#define NW_PCBOX_DOWN_RPY				210
#define NW_LOT_CANCEL_RPY				211
#define NW_VIDEO_SELECT_RPY				213		// 여기까진 서로 보내는 부분.

#define NW_BOOT_OK						221
#define NW_NEW_LOT_IN					222
#define NW_NEW_LOT_IN_SEND				223
#define NW_NEW_LOT_IN_RPY				224

#define NW_LOT_NAME						225
#define NW_LOT_NAME_RPY					226

#define NW_BLOT_COUNT					231
#define NW_BLOT_COUNT_RPY				232

#define NW_BPC_BUFFER_READY_RPY			233
#define NW_BPC_BUFFER_IN_RPY			234

#define NW_VERSION_UPDATE_RPY			990
#define NW_INVALID_FUNCTION				999

// 이하는 VLCA/320/420에 썼던 프로토콜들.
// *****************************************************************************
//  네트웍 메세지 정의 장비쪽에서 BPC로 보내는 메세지.
// *****************************************************************************
#define NW_LOT_START_RPY				1
//#define NW_LOT_CANCEL_RPY				2
#define NW_PCBOX_START_RPY				3
#define NW_PCBOX_END_RPY				4
#define NW_PCBOX_CHANGE_RPY				5
#define NW_ENABLE_RPY					6
#define NW_DISABLE_RPY					7
#define NW_SLOT_MASK_RPY				8
#define NW_LOT_START_ENABLE_RPY			9
#define NW_PCBOX_REMOVE_RPY				10
#define NW_PCBOX_ERROR_RPY				11
#define NW_UNLOAD_COMPT					13
#define NW_CANCEL_END					14
#define NW_LOT_END						15
#define NW_PCBOX_DISABLE				16
#define NW_VIDEO_TIMEOUT				17
#define NW_HOLD_TIMEOUT					18
#define NW_PCBOX_LOT_INFO				19
#define NW_FAIL_INFO_SEND				20
#define NW_FAIL_REGNUMBER_REQUEST		21
#define NW_FAIL_INFO_SERACH				22
#define NW_FAIL_INFO_SAVE				23
#define NW_FAIL_INFO_DELETE				24
#define NW_EQP_LOT_START				26
#define NW_EQP_LOT_CANCEL				27
#define NW_EQP_LOT_END					28
#define NW_PCBOX_OFF_CHECK				100		// 새로 추가됨. 2K11/03/02/ViboX
#define NW_POWER_OFF_CHECK_RPY			101
#define NW_EQP_ENABLE_RPY				200
#define NW_EQP_DISABLE_RPY				201
#define NW_EQP_ENABLE					200
#define NW_EQP_DISABLE					201
#define NW_LOT_START					29
#define NW_PCBOX_CHANGE					33
#define NW_ENABLE						34
#define NW_DISABLE						35
#define NW_SLOT_MASK					36
#define NW_LOT_START_ENABLE				37
#define NW_PCBOX_REMOVE					38
#define NW_PCBOX_ERROR					39
#define NW_PCBOX_LOT_INFO_RPY			40
#define NW_FAIL_INFO_SEND_RPY			41
#define NW_FAIL_REGNUMBER_REQUEST_RPY	42
#define NW_FAIL_INFO_SERACH_RPY			43
#define NW_FAIL_INFO_SAVE_RPY			44
#define NW_FAIL_INFO_DELETE_RPY			45
#define NW_EQP_LOT_START_RPY			47
#define NW_EQP_LOT_CANCEL_RPY			48
#define NW_EQP_LOT_END_RPY				49
#define NW_PCBOX_ENABLE					50
#define NW_POWER_CONTROL_ON				71
#define NW_POWER_CONTROL_OFF			72
#define NW_POWER_OFF					82
#define NW_VIDEO_TIMEOUT_RPY			91
#define NW_POWER_OFF_RPY				92

enum ENUM_WAIT_TIME
{
	E_WAIT_RAIL,
	E_WAIT_GUIDE,
	E_WAIT_PUSHER,
	E_WAIT_ALIGN,
	E_WAIT_STOPPER,
	E_WAIT_CLAMP,
	E_WAIT_GRIPPER_DN,
	E_WAIT_GRIPPER_ON,
	E_WAIT_PICKER_DN,
	E_WAIT_SITE_PLATE,
	E_WAIT_COK_LOCK,
	E_WAIT_POGO,//2012,1225
	E_WAIT_INSERT,
	E_WAIT_HIFIX_CLAMP,
	E_WAIT_COK_STACKER_LOCK,
	E_WAIT_TRANSFER_DN,
	E_WAIT_REJ_STACKER_GUIDE,
	E_WAIT_PICKER_EJECT,//2015.0819
	E_WAIT_MAX=30,
};
//2015.0914
enum ENUM_FILLRATIO
{
	
	REF_TIME,
		RUN_TIME,
		TEST_TIME,
		RATIO_RUN,
		RATIO_TEST,
		RUN_DOWN,
		SOCKETOFF,
		INSTANBY,
		OUTREADY,
		STOP,
		USERSTOP,
		OUTPUT,
		YIELD,		
};
///////////////////
enum ENUM_COKINDEX
{
	COKINDEX_GS1,
	COKINDEX_GS2,
	COKINDEX_MSATA,
	COKINDEX_25_REVERSE,
	COKINDEX_25_FRONT,
	MAX_COKINDEX,
};
// *****************************************************************************
// 화면 동작 관련 기본 데이터 저장 구조체 변수 선언                             
// *****************************************************************************
struct st_handler_param
{
	int mn_menu_num;				// 각 화면에 설정된 화면 번호 저장 변수 
	int mn_alarm_screen;			// 현재 알람 화면이 출력되어져 있는지에 대한 정보 저장 변수 

	BOOL b_program_exit;			// 프로그램을 빠져나가는건지 확인한다.
	
	// *************************************************************************
	// 화면에 대한 핸들 정보 저장 변수 선언                                     
	// *************************************************************************
	HWND hWnd;						// 메인 프레임 핸들 저장 변수
	CWnd *cwnd_title;				// 타이틀 출력 화면 핸들 저장 변수
	CWnd *cwnd_list;				// 리스트 출력 화면 핸들 저장 변수

	CWnd *cwnd_main;				// 메인 화면 핸들 저장 변수
	CWnd *cwnd_basic;				// BASIC 화면 핸들 저장 변수
	CWnd *cwnd_maintenance;			// Maintenance 화면 핸들 저장 변수
	CWnd *cwnd_testreference;		// Test Reference 화면 핸들 저장 변수
	CWnd *cwnd_interface;			// Interface 화면 핸들 저장 변수
	CWnd *cwnd_waittime;			// Wait Time 화면 핸들 저장 변수
	CWnd *cwnd_init;

	CWnd *cwnd_rack_info;

	CWnd *cwnd_motorteach;			// motor teaching 화면 핸들 저장 변수
	CWnd *cwnd_motorspeed;			// Motor Speed 화면 핸들 저장 변수
	CWnd *cwnd_diagnose;

	CWnd *cwnd_motor;

	CWnd *cwnd_motorAxis;
	CWnd *cwnd_motor_with_io;

	CWnd* cwnd_data_lot;
	CWnd* cwnd_data_testsite;
	CWnd* cwnd_data_xgem;// 2013,1028
	// *************************************************************************		

	// *************************************************************************
	// 장비 상태 정보 설정 변수 선언                                            
	// *************************************************************************
	int mn_initial;					// 초기화 작업 완료 플래그
	int mn_manual;
	
	int mn_menu_lock;				// 메뉴 사용 가능 상태 플래그 
	int mn_system_lock;				// SYSTEM LOCK 상태 플래그
	int mn_switch_lock;				// Dialog가 떠 있는 상태 플래그 (Start Switch Lock)

	CString mstr_pass_level;
	int mn_level_maint;				// 메인트 레벨(Level 1) 설정 플래그
	int mn_level_teach;				// 티칭 레벨(Level 2) 설정 플래그
	int mn_level_speed;				// 스피드 레벨(Level 3) 설정 플래그
	int mn_level_admin;				// Admin 레벨(Level 4) 설정 플래그

	int mn_init_state[MAXSITE];		// 초기화 완료 플래그
	int mn_initial_error;

	int mn_status;					// 장비가 가동중이냐
	int mn_reinstatement;

	int mn_safety;

	CTimeSpan m_tR;		//Run Time
	CTimeSpan m_tJ;		//Jam Time
	CTimeSpan m_tS;		//Stop Time
	CTimeSpan m_tM;		//Maint Time
	CTimeSpan m_tT;		//Test Time
	CTimeSpan m_tPT;	//Prev Test Time
	CTimeSpan m_tIT;	//Index Time
	CTimeSpan m_tPIT;	//Prev Index Time
	CTimeSpan m_tCT;	//Cycle Time
	CTimeSpan m_tPCT;	//Prev Cycle Time
	CTime m_tRef;
	
	CTimeSpan m_tDR;	//Run Time
	CTimeSpan m_tDRB;	//Run Time
	CTimeSpan m_tDRW;	//Run Time
	CTimeSpan m_tDJ;	//Jam Time
	CTimeSpan m_tDS;	//Stop Time
	CTimeSpan m_tDM;	//Maint Time
	CTimeSpan m_tLotR[MAX_PLOT];	//Lot Run Time //2013,1105


	
	//2015.0914
	CTimeSpan m_tDRef;
	//CTimeSpan m_tDRdown[2][10];	
	CTimeSpan m_tDRdown[20];
	//2015.0914
	CTimeSpan m_tDUserStop[20];	//Stop Time
	CTimeSpan m_tDStop[20];	//Stop Time
	CTimeSpan m_tDInStandby[20];
	///////
	CTimeSpan m_tDtest[2][10];
	CTimeSpan m_tDSokOff[2][10];
	//CTimeSpan m_tDInStandby[2][10];
	CTimeSpan m_tDOutStandby[2][10];
	
	int				mn_Pass[20];
	int				mn_Fail[20];
	int				mn_Input[20];
	int             mn_Output[20];
	int             mn_Yield[20];

	int mn_motor_init_check;
	int n_initiailze;
	
	int mn_interval; //kwlee 2015.1005
	CString mstr_DirPath;

	int mnSerial_Port_Creating[MAX_PORT];

	CString mstrSelectMessage;
	int mnSelectMessage;

	CString mstr_lot_name[3];		// 현재 장비 안에 있는 LOT NAME과 BUFFER_READY에서 날라오는 LOT NAME과 비교한다.
	int mn_comm_lot_name_chk;

	int mn_virtual_mode;
	int mn_virtual_supply;
	double md_picker_position[4];
	double md_tray_place_offset[2];
	double md_common_teaching[COMMON_TEACHING_MAX];
	int mn_cokIndex[MAX_COKINDEX];

	//2016.0329
	double md_safty_rbtx;
	double md_safty_rbty[2];//0:tray 1:reject

	int mn_retest_cnt;
	int mn_beat_cnt;
	int mn_AutoSocketOff_Cnt;

	CString str_main_lot;
	CString str_main_part;
	CString str_main_last;
	CString str_main_bypass;
	
	int mn_emptyout_cnt;	
	int mn_emptyin_cnt;
	int mn_emptynext_cnt;
	int mn_buffer_in;
	BOOL mb_rej_3stacker;
	int mn_cok_change;

	int mn_temp_pogo_type;
	int mn_temp_alignBuffer_type;
	int mn_pogo_type;

	//2013,0213
	int mn_cok_chgstate[3];
	int mn_mirror_type;

	int m_nfront_smema;
	int m_nfront_request;

	int mn_robot_motorX_safety;
	int mn_robot_motorRej_safety;

	//2013,0715
	int mn_fullinline_mode;
	CString m_strProcessID;
	CString m_strRPropEnd;
	int m_iPLotInfo;		// 2013,0910

	int m_bRestore;

	int m_nRemote;
	CString m_xgemstrErrMsg;
	CString m_strRecipe[100];
	int m_nTotal;
	CString m_strToolInform;

	////2015.0518
	int mn_multi_retest_cnt;
	int m_nhifix_mode_cnt;
	CString mstr_hifix_mode[100];
	////

	////2015.0604
	int m_nScrap_code_cnt;
	CString mstr_scrap_code[100];
	int mn_MultiLot_cnt[100];
	int mn_1Lot_cnt[100];
	int mn_version_mode;
	int mn_scrap_code_retry[100];

	int m_nRecipe_rule_cnt;

	//2015.0914
	int mn_allnewlot;
	int mn_InstandyBy[20];
	int mn_InstandyBy_1[20];
	int mn_OutstandyBy[20];

	int mn_userstop;
	bool isTestRun[20];

	bool mn_xgem_connect;
	CString mstr_device_name;

	bool mn_Abort;//
	//2016.0709
	int m_bEnable[COK_TYPE_CNT][TSITE_SOCKET_CNT];
	int m_bBackupEnable[COK_TYPE_CNT][TSITE_SOCKET_CNT];
	int m_bBackupEnable2[COK_TYPE_CNT][TSITE_SOCKET_CNT];

	int mn_xgem_mode;//2017.0602 NEW_OLD

} ;
extern  st_handler_param  st_handler;

// *****************************************************************************
//  셋팅 관련 변수
// *****************************************************************************
struct st_setting_param
{
	int mn_text_r[2][2];
	int mn_text_g[2][2];
	int mn_text_b[2][2];
	
	int mn_grid_r[2][2];
	int mn_grid_g[2][2];
	int mn_grid_b[2][2];
};
extern  st_setting_param  st_set;
// *****************************************************************************

// *****************************************************************************
//  장비 부위간 Sync 관련 변수
// *****************************************************************************
struct st_sync_param
{
	CString mstr_buffer_in[2];
	CString mstr_new_lot_in;
	CString mstr_scrap_info;
	CString mstr_online_end;
	CString mstr_track_out;
	CString mstr_track_out_msg;
	CString mstr_lot_display;
	CString mstr_delete_scrap;

	int mn_rbuffer_work[2];			// Receive Buffer에 할일이 생겼다.
	int mn_rbuffer_work_id[2];		// Receive Buffer에 할일은?
	
	int mn_add_fail;
	
	int mn_rbuffer_station[2];		// APCT의 소켓 정보를 물어본다.

	int n_scrap_serial;
};
extern  st_sync_param  st_sync;
// *****************************************************************************

// *****************************************************************************
//  메세지 관련
// *****************************************************************************
struct st_message_param
{
	int mn_pad_dialog;
	int mn_dot_use;
	CString mstr_pad_high_limit;
	CString mstr_pad_low_limit;
	CString mstr_keypad_val;

	CString mstr_keypad_msg;
	CString mstr_keyboard_msg;

	CString str_fallacy_msg;		// 오류 메세지.
	CString mstr_confirm_msg;		// 확인 메세지
	CString mstr_typing_msg;		// 키보드 메세지

	CString mstr_parent_menu;

	CString mstr_password;

	CString mstr_barcode;

	// List에 출력 하다가 죽는 현상을 막기 위해서... 2K11/09/01/ViboX
	char c_msg[5000];
	char c_abmsg[5000];
	char c_normal_msg[5000];
	char c_abnormal_msg[5000];

	CString mstr_send;
	CString mstr_recive;

	CString mstr_rpy_msg[10];

	CString mstr_event_msg[3];

	CString str_step_savefile_name;
	CString str_step_savefile_path;

	char n_select_buffer;
} ;
extern  st_message_param  st_msg;

////2015.0604
struct st_message_param_ftp
{
	CString mstr_event_ftp[3];
} ;
extern  st_message_param_ftp  st_ftp;
////

// *****************************************************************************

// ******************************************************************************
// 파일 경로명 저장 구조체 변수 선언                                             
// ******************************************************************************
struct st_path_param 
{
	CString mstr_basic;			// 기본 셋팅 정보 저장 폴더+파일명 설정 변수
	CString mstr_basic_folder;	// 기본 셋팅 정보 저장 폴더+파일명 설정 변수

	////2015.0604
	CString mstr_scrap_code;			// Scrap Code 파일 폴더 설정
	CString mstr_recipe_rule;
	////

	CString mstr_path_dvc;		// 디바이스별 티칭 정보 저장 파일 생성 폴더 설정 변수
	CString mstr_path_alarm;	// 출력 알람 정보 존재 폴더 설정 변수
	
	CString mstr_path_file;		// 파일 생성 폴더 설정 변수
	CString mstr_path_back;		// BACKUP 파일 생성 폴더 설정 변수
	
	CString mstr_file_basic;	// 기본 셋팅 정보 저장 파일명 설정 변수
	CString mstr_file_wait;		// 대기 시간 정보 저장 파일명 설정 변수
	
	CString mstr_operation;		// Operation Log 파일 폴더 설정.
	CString mstr_interface;		// Interface Log 파일 폴더 설정.
	CString mstr_tcpip;
	CString mstr_machine;		// Machine Log 파일 폴더 설정.
	CString mstr_barcode;		// Barcode Log 파일 폴더 설정.
	CString mstr_total;			// Total Log 파일 폴더 설정.
	CString str_log_path;
	CString str_lot_data_path;
	
	CString mstr_io_map;		// IO MAP 파일 위치 폴더.
	CString mstr_motor;
	
	CString mstr_handler;

	CString mstr_io_part_map;
	CString mstr_motor_part_map;
	CString mstr_motor_axis_map;
	CString mstr_wait_time_map;

	CString mstr_recipe_path;//2013,1105 XGEM
	CString mstr_socket_skip;//2016.0709
} ;
extern  st_path_param  st_path;
// ******************************************************************************

// ******************************************************************************
// I/O 동작 대기 시간 및 리미트 시간 저장 구조체 선언                            
// ******************************************************************************
struct st_time_param 
{
	int n_wait_time[E_WAIT_MAX][2];
	int n_limit_time[E_WAIT_MAX];
} ;
extern  st_time_param  st_time;
// ******************************************************************************
//2015.0914
struct st_DB_time_param 
{
	CTimeSpan n_Db_time[20][20];
	int				mn_Pass[20];
	int				mn_Fail[20];
	int				mn_Input[20];
	int             mn_Output[20];
	float             mn_Yield[20];
};
extern  st_DB_time_param  st_DB_time;
// *****************************************************************************
//  Tower Lamp 화면에 대한 정보 저장 구조체 변수 선언                           
// *****************************************************************************
struct st_lamp_param
{
	int mn_lamp_r[8];			// RED 램프 상태 저장 변수
	int mn_lamp_y[8];			// YELLOW 램프 상태 저장 변수
	int mn_lamp_g[8];			// GREEN 램프 상태 저장 변수
	
	CString mstr_equip_no;		// 장비 호기 저장 변수
	CString mstr_equip_id;	// 장비 코드 저장 변수
	CString mstr_empty_id;
	CString mstr_tool_id;

	int mn_buzzer_mode;			// 부저 사용 모드 저장 변수
	
	int mn_lamp_wait_time;		// 램프 FLACKER 시간 저장 변수
	int mn_lamp_change_time;	// 램프 변경 시간 저장 변수

	////2015.0604
	CString mstr_ftp_path;// /Users/MDL/MDL_HNDL/WP_SKEL_SSD/RETEST_RULE -> FTP서버 경로
	CString mstr_ftp_ip;
	CString mstr_ftp_user;
	CString mstr_ftp_pw;
 	int		mn_ftp_port;
	CString mstr_ftp_scrap_code_name;// RETEST_RULE.TXT -> FTP서버 다운 받을 파일
	CString mstr_ftp_recipe_rule_name;//RECIPE_RULE.TXT
	////

} ;
extern  st_lamp_param  st_lamp;
// *****************************************************************************

// ******************************************************************************
// 베이직 화면 환경 설정 정보 저장 구조체 선언                                   
// ******************************************************************************
struct st_basic_param 
{
	int nManualSpeed;
	int nRunSpeed;
	
	int n_tray_x;
	int n_tray_y;

	int n_mode_device;
	int n_mode_pogo;//2012,1225

	int mn_alignBuffer_type;//2013,0215
	int n_mode_run;
	int n_retry_cnt;

	int n_mode_jig;
	int n_mode_recovery;

	int n_mode_product;
	int n_mode_product_site1;
	int n_mode_product_site2;
	int n_mode_product_site1_mode;
	int n_mode_product_site2_mode;

	int n_cok_type;
	int n_degree;
	int n_pick_type;

	int mn_mode_run;			// 장비 동작 모드 저장 변수		(1:ON LINE	0:OFF LINE)
	int mn_mode_xgem;
	int mn_mode_rcmd;
	
	int mn_bypass_mode;

	int mn_device_type;
	
	int n_day_start_time;		// Data 기록용.. 하루의 시작 시간...
	int n_day_shift_time;		// Data 기록용.. Shift 시간...
			
	CString mstr_device_name;	// 선택된 디바이스 종류 저장 변수
	int mn_alarm_delay_time;
	CString mstr_revipe_name;	// 선택된 디바이스 종류 저장 변수
	
	CString mstr_Interval_Cnt; //kwlee 2015.1005;
	int mn_electrostatic_check_gap;
	double md_electrostatic_value;

	int mn_network_wait_time;		// Network 대기 시간
	int mn_network_retry_cnt;		// Network Retry 횟수

	int mn_time_out;
	int mn_time_out_per;
	int mn_empty_tray_out;
	int mn_robot_init_time;
		
	long l_handler_recovery_flag[4];
	//[0]=>복구모드 저장 모드(0:저장되지 않은 상태, 1:복구모드가 저장 상태(이때만 초기화시 복구여부를 물어볼 수  있다) 
	//[1]=>초기화시 복구모드 선택 유/무 확인 (0:복구모드 선택안함, 1:복구모드 선택)
	//[2]=>복구 모드 저장 시간 저장 
	//[3]=>초기화시 복구모드 선택시간  

	double md_tray_offset_1_x[MAX_TRAYCELL];
	double md_tray_offset_2_x[MAX_TRAYCELL];
	double md_tray_offset_3_x[MAX_TRAYCELL];//ybs
	double md_tray_offset_rej_x[MAX_TRAYCELL];
	double md_tray_offset_1_y[MAX_TRAYCELL];
	double md_tray_offset_2_y[MAX_TRAYCELL];
	double md_tray_offset_3_y[MAX_TRAYCELL];//ybs
	double md_tray_offset_rej_y[MAX_TRAYCELL];

	int mn_hotfix_pos;//2013,0504
	int mn_socket_max_count;

	int m_bFDC_Data;//2013,1105 FDC Data

	
	CString n_str_hifix_mode;//2015.0518
	
	////2015.0604
	CString mstr_FtpIP;
	CString mstr_FtpName;
	CString mstr_FtpPort;
	CString mstr_mode_ftp;
	int mn_mode_ftp;
	////

	int n_retest_jig;//2015.0914
	int n_recipe_mode;

	CString m_strScrapHead;

	int n_cserial2_mode;

	int n_auto_timeout_mode;//2016.0112

	int n_file_save;

} ;
extern  st_basic_param  st_basic;
// ******************************************************************************

// ******************************************************************************
// 발생한 알람 관련 데이터 저장 구조체 선언                                      
// ******************************************************************************
struct st_alarm_param 
{
	CStringList mstr_list_jam;	// 발생한 Jam Code의 리스트
	CStringList mstr_list_jam_time;	//alarm 발생시간 저장 리스트
	CString mstr_code;			// 발생한 알람 코드 저장 변수
	char mc_code[10];
	CString mstr_pcode;			// 발생한 알람 코드 저장 변수
	int mn_count_mode;			// 알람 카운트 플래그 (0:알람 카운트 미진행    1:알람 카운트 진행)
	int mn_type_mode;			// 알람 종류 저장 변수 (0:경고성    1:진행성    2:장비 정지)
	
	long stl_cur_alarm_time;	// 알람 발생 시간 저장 변수
	int stl_prev_code;			// 바로 전 발생한 알람 코드 저장 변수
	long stl_prev_alarm_time;	// 바로 전 발생한 알람 발생 시간 저장 변수
	long stl_alarm_start_time;	// 시작 버튼이 눌렸을때의 발생 시간 저장 변수
	
	int mn_alarm_occured;		// 알람 발생 횟수 저장 변수
	int mn_alarm_occured_day;	// 알람 발생 횟수 저장 변수

	int mn_reset_status;		// Alarm Reset Status
	int mn_emo_alarm;            // EMO관련 알람은 한번만 발생하도록 하기 위함!!
} ;
extern  st_alarm_param  alarm;
// ******************************************************************************

// ******************************************************************************
// 알람 데이터 저장 구조체 선언                                                  
// ******************************************************************************
struct st_dsp_alarm_param 
{
	// **************************************************************************
	// 알람 부위 표시 마크 [출력 위치], [크기], [칼라] 저장 변수                 
	// **************************************************************************
	int mn_Offset;
	int mn_Cy;
	int mn_Cx;
	int mn_LineWidth;
	int mn_R;
	int mn_G;
	int mn_B;
	// **************************************************************************
	
	// **************************************************************************
	// 화면에 출력할 알람 정보 저장 변수                                         
	// **************************************************************************
	int mn_action_cnt;            // 출력할 알람 조치 사항 갯수 저장 변수
	CString mstr_style;           // 알람 출력 형태 저장 변수
	CString mstr_bmp_file;        // 알람 표시 그림 파일 저장 변수
	CString mstr_repair_msg;      // 알람 조치 사항 저장 변수
	// **************************************************************************
	
	// **************************************************************************
	// 파일에 저장된 알람 정보 저장 변수                                         
	// **************************************************************************
	CString mstr_e_content[2000];  // 파일에서 로딩한 한 라인 정보 저장 변수
	CString mstr_e_code[2000];     // 알람 코드 저장 변수
	CString mstr_e_state[2000];    // 알람 상태 저장 변수
	CString mstr_e_part[2000];     // 알람 발생 부위 저장 변수
	CString mstr_e_msg[2000];      // 공백 제거 알람 메시지 저장 변수
	// **************************************************************************
	
	int mn_e_list;
	
	// **************************************************************************
	// 현재 발생한 알람 저장 변수                                                
	// **************************************************************************
	int mn_cur_state;				// 알람 저장 여부 플래그
	CString mstr_cur_state;			// 알람 등급
	CString mstr_cur_msg;			// 발생한 알람 메시지 저장 변수
} ;
extern  st_dsp_alarm_param  st_alarm;
// ******************************************************************************

// ******************************************************************************
// 장비 동작 제어 플래그 정보 저장 구조체 선언                                   
// ******************************************************************************
struct st_work_param 
{
	int nMdlInputCount[2][2];	// [Prime, Cum][Lot, Daily]
	int nMdlPassCount[2][2];
	int nMdlFailCount[2][2];
	int nMdlHtoCount[2][2];
	int nMdlBtoCount[2][2];
	int nMdlNtoCount[2][2];
	int nMdlRejectCount[2][2];
	float fMdlyield[2][2];

	// **************************************************************************
	// 장비 동작 중 발생하는 시간 정보 저장 관련 멤버 변수                       
	// **************************************************************************

	int mn_elec_serial_work;
	// **************************************************************************

	////복귀동작 관리  // 0:Front Shifter, 1:Rear Shifter, 2:Front Buffer, 3:Read Buffer
	int n_sync_reinstate;
	int mn_reinstatement_mode;					// 0:복귀클리&완료 , 1:stop전환후//백업후 , 2:스타트 키 정보 저장, 3:복귀중 ...., 복귀완료후 0 그리고 런 하자 
	double md_reinstatement_Pos[2][100];				// 0: Ld Elev., 1: Tray Transfer, 2: Robot X, 3: Robot Y, 4: Robot Z, 5: Robot Pitch, 6: Uld Elev.
// 	double md_reinstatement_pos[100];				// 0: Ld Elev., 1: Tray Transfer, 2: Robot X, 3: Robot Y, 4: Robot Z, 5: Robot Pitch, 6: Uld Elev.
	int md_reinstatement_cyl[100];
	int	mn_picker_updn[4];
	int	mn_glipperr_updn;
	int	mn_tray_guide[3];
	int	mn_tray_pusher[3];//1부터 시작
	int	mn_site_insert[2][5];
	int	mn_site_pogo[2][5];
	int mn_transfer_cyl;

	int mn_xyz_safety;

	int mn_reinstate_xyz_robot;
	int mn_reinstate_reject_robot;
	int mn_reinstate_tester_robot[2];
	int mn_reinstate_stacker_robot[3];

	int n_back_type;		//이동방식이 계산을 통한 위치인지, teach데이터로 이동인지

	double mdReadElectData[5];

	int mn_tl_status;					// 타워램프 상태 정보 저장 변수 (O:STOP 1:RUN(IN PCB) 2:ALARM 3:MAINT 4:INIT 5:RUN(NO PCB))

	CString mstr_client_rev_msg[3];

	int		mn_prev_step[100];			// 이전 step (변경 되었는지 보기 위함)
	CString	mstr_history_step[100];		// step_history 약 50개에 해당하는 step을 저장
	CString	mstr_history_step_back_up[100];		// step_history 약 50개에 해당하는 step을 저장
	CString mstr_last_change_date[100];	// 최종 step으로 변경되는 시점의 시간

	double d_reinstatement_x_pos;		//복귀될 계산된 x 위치값
	double d_reinstatement_y_pos;		//복귀될 계산된 y 위치값
	double d_reinstatement_z_pos;		//복귀될 계산된 z 위치값

	char cPLotName[MAX_PLOT][50];	// P_LOT 2013,0910
	int nPLotInputCount[MAX_PLOT][3];	// P_LOT 2013,0910
	int nPLotPassCount[MAX_PLOT][3];	// P_LOT 2013,0910
	int nPLotFailCount[MAX_PLOT][3];	// P_LOT 2013,0910
	double fPLotYield[MAX_PLOT][3];	// P_LOT 2013,0910
	bool m_bPLotDel;
	int nPLotCnt;

} ;
extern  st_work_param  st_work;

#define NETWORK_CNT						2

#define NETWORK_ECSERVER				0
#define NETWORK_NMACHINE				1

// ******************************************************************************
//  TEST REFERENCE화면 환경 설정 정보 저장 구조체 선언                           
// ******************************************************************************
// ******************************************************************************

struct st_modulemap_size
{
	int nDeviceRobotPicker[4][PICKER_NUM];			// 0:Pickerinfo, 1:bin정보, 2:retest count, 3:Array
} ;
extern  st_modulemap_size  st_modulemap;

// ******************************************************************************
// RS-232C 시리얼 통신 관련 저장 구조체 선언                                     
// ******************************************************************************
struct st_serial_param 
{
	int mn_snd_chk[MAX_PORT];         // 데이터 송신 완료 플래그
	int mn_rec_chk[MAX_PORT];         // 데이터 수신 완료 플래그
	
	CString mstr_port_chk[MAX_PORT];  // 시리얼 포트 생성 정보 저장 변수
	
	CString mstr_snd[MAX_PORT];       // 송신 메시지 저장 변수
	CString mstr_rec[MAX_PORT];       // 수신 메시지 저장 변수
	
	CString mcomm_snd[MAX_PORT];      // 리스트 화면 출력 송신 메시지 저장 변수
	CString mcomm_rec[MAX_PORT];      // 리스트 화면 출력 수신 메시지 저장 변수
	CString mcomm_err[MAX_PORT];      // 리스트 화면 출력 에러 메시지 저장 변수

} ;
extern  st_serial_param  st_serial;
// ******************************************************************************

struct st_nw_param
{
	int			mnID;

	CString		mstr_full_msg[10];
	CString		mstr_header_msg[2];
	CString		mstr_data_msg[2];
	CString		mstr_function[2];
	CString		mstr_lot_id[2];
	CString		mstr_lot_no[2];
	CString		mstr_eqp_id[2];
	CString		mstr_oper_cnt[2];
	CString		mstr_oper_id[2];
	CString		mstr_oper_info[2];
	CString		mstr_pcbox_id[2];
	CString		mstr_serial[2];

	CString		mstr_pcbox_cnt[2];
	CString		mstr_pcbox_info[2];
	CString		mstr_lot_qty[2];
	CString		mstr_slot[2];
	CString		mstr_step_cnt[2];
	CString		mstr_part_id[2];
	CString		mstr_cycle_id[2];
	CString		mstr_temp[2];
	CString		mstr_result[2];
	CString		mstr_qty[2];
	CString		mstr_test_result[2];
	CString		mstr_novideo_cnt[2];
	CString		mstr_fail_no[2];
	CString		mstr_fail_cnt[2];
	CString		mstr_fail_log[2];
	CString		mstr_fail_slot[2];
	CString		mstr_fail_type[2];
	CString		mstr_fail_desc[2];
	CString		mstr_fail_code[2];
	CString		mstr_retest_no[2];
	CString		mstr_retest_id[2];
	CString		mstr_retest_flag[2];
	CString		mstr_slot_no[2];
	CString		mstr_test_idx[2];
	CString		mstr_fail_regnumber[2];
	CString		mstr_error_log[2];
	CString		mstr_scrap_cnt[2];
	CString		mstr_scrap_info[2];

	CString		mstr_pcbox[2][150];
	
	CString		mstr_msg[2];
	
	CString		mstr_buffer_rcv_data[10][100];
	int			mn_rcv_data[10];
	int			mn_find_data[10];
	
	CString		mstr_client_rev;
	CString     mstr_client_send;
	
	// 장비간 통신하는 프로토콜용 변수.
	CString		mstr_m_eqp_id[2];
	CString		mstr_m_buffer_no[2];
	CString		mstr_m_station[2];
	CString		mstr_m_lot_no[2];
	CString		mstr_m_part_no[2];
	CString		mstr_m_last_module[2];
	CString		mstr_m_bypass[2];
	CString		mstr_m_mode[2];
	CString		mstr_m_module_cnt[2];
	CString		mstr_m_module_data[20][2];
	CString		mstr_m_retestcheck[2];

	CString		mstr_pct_bypass[2];
	
	CString		mstr_m_socket_off[2];

	CString		mstr_new_lot_no[2];

	CString		mstr_bpc_buffer_ready;
	CString		mstr_bpc_buffer_in;

	bool		mb_buffer_in_received;

	CString		mstr_buffer_in_recv;
	CString		mstr_buffer_in_temp;

	int mn_scrap_info_flag;
};
extern st_nw_param st_NW;
// ******************************************************************************

struct st_time_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	
	CString			mstrlot_no;
	CString			mstrpart_no;
	
	int				mnTime_Section;
	int				mnTime_Time;
	int				mnMode;
	
	int				mnSectionBuffer;
};
extern struct st_time_database st_timedb;

struct st_alarm_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	
	CString			mstrlot_no;
	CString			mstrpart_no;
	
	CString			mstrAlarmCode;
	CString			mstrAlarmMsg;
};
extern struct st_alarm_database st_alarmdb;

struct st_ground_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	
	CString			mstrChannel[5];
};
extern struct st_ground_database st_grounddb;

//2015.0914
struct st_dbtime_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	char            str_slot[100];

	CTimeSpan		mn_Ref_time[20];
	CTimeSpan		mn_run_time[20];
	CTimeSpan		mn_test_time[20];
	float			mf_Run[20];
	float           mf_test[20];
	CTimeSpan       mn_lt_Rundown[20];
	CTimeSpan       mn_lt_Socketoff[20];
	CTimeSpan       mn_lt_instandby[20];
	CTimeSpan       mn_lt_outReady[20];
	CTimeSpan       mn_lt_stop[20];
	CTimeSpan       mn_lt_User_stop[20];
	
	float           mf_lr_Rd[20];
	float           mf_lr_SocketOff[20];
	float           mf_lr_instandby[20];
	float           mf_lr_outReady[20];
	float           mf_lr_stop[20];
	float           mf_lr_userstop[20];
	
	int             mn_Output[20];
	double          md_Yield[20];	
	int				mn_Pass[20];
	int				mn_Fail[20];
	int				mn_Input[20];
	
};
extern struct st_dbtime_database	st_dbtime;


struct st_Buffer_DB_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	char            str_slot[100];
	
	CTimeSpan		mn_Ref_time[20];
	CTimeSpan		mn_run_time[20];
	CTimeSpan		mn_test_time[20];
	float			mf_Run[20];
	float           mf_test[20];
	CTimeSpan       mn_lt_Rundown[20];
	CTimeSpan       mn_lt_Socketoff[20];
	CTimeSpan       mn_lt_instandby[20];
	CTimeSpan       mn_lt_outReady[20];
	CTimeSpan       mn_lt_stop[20];
	CTimeSpan       mn_lt_User_stop[20];
	
	float           mf_lr_Rd[20];
	float           mf_lr_SocketOff[20];
	float           mf_lr_instandby[20];
	float           mf_lr_outReady[20];
	float           mf_lr_stop[20];
	float           mf_lr_userstop[20];
	
	int             mn_Output[20];
	double          md_Yield[20];
	int				mn_Pass[20];
	int				mn_Fail[20];
	int				mn_Input[20];
	
};
extern struct st_Buffer_DB_database	st_BufferTemp;


struct st_dbTimeTmp_database
{
	char            str_startDate[100];
	char            str_EndDate[100];
	char            str_slot[100];
	
	CTimeSpan		mn_Ref_time;
	CTimeSpan		mn_run_time;
	CTimeSpan		mn_test_time;
	float			mf_Run;
	float           mf_test;
	CTimeSpan       mn_lt_Rundown;
	CTimeSpan       mn_lt_Socketoff;
	CTimeSpan       mn_lt_instandby;
	CTimeSpan       mn_lt_outReady;
	CTimeSpan       mn_lt_stop;
	CTimeSpan       mn_lt_User_stop;
	
	float           mf_lr_Rd;
	float           mf_lr_SocketOff;
	float           mf_lr_instandby;
	float           mf_lr_outReady;
	float           mf_lr_stop;
	float           mf_lr_userstop;
	
	int             mn_Output;
	int             mn_Pass;
	double          md_Yield;
	int             mn_Cnt;
	int             mn_Interval;
	
};
extern struct st_dbTimeTmp_database	*st_dbTimeTmp;
//////////////

struct st_buffer_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	CString			mstrlot_no;
	CString			mstrpart_no;
	int				mnbuffer_pos;
	int				mnlast_module;
	int				mnbypass;
	int				mnmodule_cnt;
	int				mnretest;
};
extern struct st_buffer_database		st_bufferdb;

struct st_boat_database
{
	int				mndb;
	
	COleDateTime	mole_date;
	CString			mstrlot_no;
	CString			mstrpart_no;

	int				mn_boat_id;
	int				mn_product_id;
	int				mn_pcb_cnt;
	int				mn_bin[8];		// 4 * 2 또는 2 * 4가 수량이 제일 많은 Boat임. Bin은 1(0), 2(1), 4(2), 8(3), 조립불량(4), 치수불량(5), UnKnown(6).
};
extern struct st_boat_database		st_boatdb;

struct st_buffer_tray_param
{
	int nDeviceExistence[4][3][100];
	char cModuleData[3][90][50];
	int nArrayInfo[3][90];
	char cSerialNumber[3][90][20];
	char cLotNumber[3][90][20];
	char cTestResult[3][90][20];
	char cArrayInfo[3][90][20];

	char cTotalLotNumber[40];
	char cTotalPartNumber[40];
	char cModuleCnt[5];
	int nRetestMdlCnt[2];			// 0 : HTO, BTO, 1: NTO
	int nLastModule;
	int nByPass;

	int nTrayExistence;
};
extern st_buffer_tray_param	st_buffer_tray[MAX_SITE_INFO];

#define BT_REAR_BUFFER			0
#define BT_FRONT_BUFFER			1
#define BT_PICKER				2
#define BT_RETEST_BUFFER		3
#define BT_REJECT_BUFFER		4
#define BT_LOB_BUFFER			5
#define BT_TRAY_1				6
#define BT_TRAY_2				7
#define BT_CUSTOMER_BUFFER		8
#define BT_AIRBLOW_BOX			9
#define BT_LENGTH_VARIABLE		10
#define BT_SAFETY_ZONE			11

struct st_part_motor_info
{
	int				n_part_cnt;
	int				n_part_axis_cnt[20];
	int				n_part_axis[20][10];
	
	int				n_axis_pos_num[20][10];
	int				n_axis_minus_el[20][10];
	int				n_axis_plus_el[20][10];
	int				n_axis_home[20][10];
	int				n_axis_sd[20][10];
	int				n_axis_direction[20][10];
	int				n_axis_account[20][10];
	
	CString			str_part_info[20];
	CString			str_part_name[20];
	
	CString			str_part_axis_name[20][10];
	CString			str_axis_pos_name[20][10][32];
	CString			str_axis_pos_info[20][10][32];
};
extern struct st_part_motor_info st_motor_info;

struct st_part_info
{
	CString		str_part_info[20];
};
extern struct st_part_info st_part;

struct st_xgemdata
{
	CString m_str_xgmInit;
	CString m_str_connect;
	CString m_str_bufferin;
	CString m_str_loadtray;
	CString m_str_unloadtray;

	CString m_str_onSocket;
	CString m_str_offSocket;
	CString m_str_machine_status;
	CString m_str_lotid;
	CString m_str_part_id;
	CString m_str_alarm;
	CString m_str_ppid;

	long	m_lTactTime;
	long	m_lTotCnt;
	long	m_lPassCnt;
	long	m_lFailCnt;
	int		m_nSokXpos;
	int		m_nSokYpos;

};
extern st_xgemdata st_xgem;

#endif
