// ServerItem.cpp : implementation file
//
// 이현철
// forcewin@popsmail.com

#include "stdafx.h"
#include "XgemInterface.h"
#include "ServerSocket.h" 
#include "Variable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerItem

CServerItem::CServerItem(CSocket* pServerSocket)
{
	m_ServerSocket = pServerSocket;
}

CServerItem::~CServerItem()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CServerItem, CSocket)
	//{{AFX_MSG_MAP(CServerItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CServerItem member functions

// 소켓 닫기
void CServerItem::OnClose(int nErrorCode) 
{
	CServerSocket* ServerSocket = (CServerSocket*)m_ServerSocket;

	// 소켓 해제
	ServerSocket->CloseClient(this);

	CSocket::OnClose(nErrorCode);

	// 통지 : SCM_CLOSE
	//::PostMessage(st_handler_info.cwnd_Main, WM_SERVER_MSG, SERVER_CLOSE, ServerSocket->m_n_address);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
	if(st_handler_info.cwnd_Main != NULL)
	{
		st_handler_info.cwnd_Main->PostMessage(WM_SERVER_MSG, SERVER_CLOSE, ServerSocket->m_n_address);
	}
}

// 수신 데이터 처리
void CServerItem::OnReceive(int nErrorCode) 
{
	CServerSocket* ServerSocket = (CServerSocket*)m_ServerSocket;

	// 데이터 읽기
	st_server_info[ServerSocket->m_n_address].n_rev_length = Receive(st_server_info[ServerSocket->m_n_address].ch_rev, 4096);

//	::PostMessage(st_handler_info.cwnd_Main, WM_SERVER_MSG, SERVER_REV, ServerSocket->m_n_address);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환
	if(st_handler_info.cwnd_Main != NULL)
	{
		st_handler_info.cwnd_Main->PostMessage(WM_SERVER_MSG, SERVER_REV, ServerSocket->m_n_address);
	}
	CSocket::OnReceive(nErrorCode);
}

