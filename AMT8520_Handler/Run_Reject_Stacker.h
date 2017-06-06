// Run_Reject_Stacker.h: interface for the CRun_Reject_Stacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_REJECT_STACKER_H__6F78C115_19BF_4BFC_9515_FA9A5DA0F660__INCLUDED_)
#define AFX_RUN_REJECT_STACKER_H__6F78C115_19BF_4BFC_9515_FA9A5DA0F660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ENUM_REJECTSTACKER_INIT
{
	ERSI_START,
	ERSI_GUIDE_BACK,
	ERSI_GUIDE_BACK_CHK,
	ERSI_HOMECHK_Z,
	
	ERSI_CNT_MOVETO_TRAY_MAX,
	ERSI_CNT_CHK_TRAY_MAX,

	ERSI_ZERO_DOWN,

	ERSI_FINISH,
};


class CRun_Reject_Stacker	: public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_Reject_Stacker);				// 복구 동작을 위함...

	CRun_Reject_Stacker();
	virtual ~CRun_Reject_Stacker();

// Attributes
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;

	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void Run_Initial();
	void Thread_Run();
	void Thread_Variable_Initial();

protected:
	void ERSI_HomeChk_Z();
	void ERSI_Finish();


public:
	void ReqGuide( EFB efb );
	EFB GetGuide() { return m_fbGuide; }
	void ReqSeperator( EFB efb );
	EFB GetSeperator() { return m_fbSeperator; }

	bool Chk_Guide( int iIO );
	bool Chk_Seperator( int iIO );

protected:
	void Run_Move_Guide();
	void Run_Move_Seperator();

	ENUM_STEP_CYL		m_step_guide;
	EFB					m_fbGuide;
	DWORD				m_dwTimeGuide;
	DWORD				m_dwTimeGuideStart;

	ENUM_STEP_CYL		m_step_seperator;
	EFB					m_fbSeperator;
	DWORD				m_dwTimeSeperator;
	DWORD				m_dwTimeSeperatorStart;

	int					m_nTrayMax;
};
extern CRun_Reject_Stacker Run_Reject_Stacker;

#endif // !defined(AFX_RUN_REJECT_STACKER_H__6F78C115_19BF_4BFC_9515_FA9A5DA0F660__INCLUDED_)
