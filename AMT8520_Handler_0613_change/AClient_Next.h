// AClient_Next.h: interface for the AClient_Next class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACLIENT_NEXT_H__D6FDEF51_F3DB_4460_84B6_7503485FFC20__INCLUDED_)
#define AFX_ACLIENT_NEXT_H__D6FDEF51_F3DB_4460_84B6_7503485FFC20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AClient_Next : public ANetwork_Client 
{
public:
	AClient_Next();
	virtual ~AClient_Next();

	virtual void Run_Move();
	void Run_Move_NewLotIn();
	void Run_Move_BufferIn();
	void Run_Move_BufferReady();
	void Run_Move_RecipeInfo();//2016.0129

	virtual void OnParseComplete(std::string strRecv );

	void OnReceived_NewLotIn( CString strRecv );
	void OnReceived_BufferIn( CString strRecv );
	void OnReceived_BufferReady( CString strRecv );
	void OnReceived_RecipeInfo( CString strRecv );//2016.0129



	void SetNewLotInID( CString strLotID ){ m_strLotID_NewLotIn = strLotID; }
	void SetBufferIn() { m_bBufferIn = true; }
	void SetProcessID( CString strProcessID ){ m_strProcessID = strProcessID;}
	bool GetBufferIn() { return m_bBufferIn; }
	void ResetBufferIn() { m_bBufferIn = false; }//2013,0503
	void SetRecipeInfoID( CString strRecipeInfoID ){ m_strRecipeInfoID = strRecipeInfoID; }

	//ybs
	void SetBufferReady( bool bVal, bool bEmotyTray=0, CString strLotID="" ) { m_bBufferReady = bVal; m_bBufferEmpty = bEmotyTray; m_strLotid = strLotID;}
	bool getBufferReady() { return m_bBufferReady; }

	void SendTrayInfoReq();
	void SendRecvStateReq();

protected:
	DWORD	m_dwTime_NewLotIn_Send;
	CString m_strLotID_NewLotIn;
	CString m_strProcessID;
	int		m_nCnt_NewLotIn;

	DWORD	m_dwTime_Buffer_In_Send;
	bool	m_bBufferIn;

	DWORD	m_dwTime_Buffer_Ready_Send;
	bool	m_bBufferReady;
	//ybs
	bool	m_bBufferEmpty;
	CString m_strLotid;
	int		m_nCnt_BufferIn;

	CString m_strRecipeInfoID;
	DWORD   m_dwTime_RecipeInfo_Send;
	int		m_nCnt_RecipeIfo;
};

extern AClient_Next g_client_next;
#endif // !defined(AFX_ACLIENT_NEXT_H__D6FDEF51_F3DB_4460_84B6_7503485FFC20__INCLUDED_)
