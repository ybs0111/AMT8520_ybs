// ClientSocket.cpp : implementation file
//
// 이현철
// forcewin@popsmail.com

#include "stdafx.h"
#include "ClientSocket.h"
#include "Variable.h"			// 전역 변수 정의 클래스 추가 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

//DEL BOOL CClientSocket::Create(CString szAddress, UINT nPortNo, int nReceiveSize)
//DEL {
//DEL /*	if (!CSocket::Create())
//DEL 	{
//DEL #ifndef _DEBUG  // debug version in CClientSocket.cpp
//DEL 		AfxMessageBox("Error : CSocket::Create() in socket");
//DEL #endif
//DEL 		return FALSE;
//DEL 	}
//DEL 
//DEL 	if (!CSocket::Connect(szAddress, nPortNo))
//DEL 	{
//DEL 		CSocket::Close();
//DEL #ifndef _DEBUG  // debug version in CClientSocket.cpp
//DEL 		AfxMessageBox("Error : CSocket::Connect() in socket");
//DEL #endif
//DEL 		return FALSE;
//DEL 	}
//DEL 	m_nReceiveLen  = 0;
//DEL 
//DEL 	m_nReceiveSize = nReceiveSize;
//DEL 	m_byteReceive = new BYTE[m_nReceiveSize];
//DEL 
//DEL 	return TRUE;*/
//DEL 	double dblTimeout = 1;
//DEL 	
//DEL 	if(dblTimeout <= 0.0) 
//DEL 	{
//DEL 		TRACE("CSocketWithTimeout::Connect() : Original CSocket::Connect() Executed. [this=0x%08X]\n", (unsigned long)this);
//DEL 		return CSocket::Connect(szAddress.GetBuffer(szAddress.GetLength()), nPortNo);
//DEL 	}
//DEL 	
//DEL 	SOCKET	fdSocket	= m_hSocket;
//DEL 	
//DEL 	DWORD	dwSocketNonBlocking	= SOCKET_NONBLOCKING;
//DEL 	if(ioctlsocket(fdSocket, FIONBIO, &dwSocketNonBlocking)) 
//DEL 	{
//DEL 		TRACE("CSocketWithTimeout::Connect() : Failed to be NonBlocking mode. [this=0x%08X]\n", (unsigned long)this);
//DEL 		return FALSE;
//DEL 	}
//DEL 	
//DEL 	AsyncSelect(NULL);
//DEL 	
//DEL 	sockaddr_in	SocketAddr;
//DEL 	ZeroMemory(&SocketAddr, sizeof(SocketAddr));
//DEL 	
//DEL 	SocketAddr.sin_family			= AF_INET;
//DEL 	SocketAddr.sin_addr.S_un.S_addr	= inet_addr(szAddress.GetBuffer(szAddress.GetLength()));
//DEL 	SocketAddr.sin_port				= htons(nPortNo);
//DEL 	
//DEL 	connect(fdSocket, (sockaddr*)&SocketAddr, sizeof(SocketAddr));
//DEL 	
//DEL 	fd_set	SocketWriteSet;
//DEL 	ZeroMemory(&SocketWriteSet, sizeof(SocketWriteSet));
//DEL 	
//DEL 	FD_ZERO(&SocketWriteSet);
//DEL 	FD_SET(fdSocket, &SocketWriteSet);
//DEL 	
//DEL 	struct	timeval	tvTimeout;
//DEL 	tvTimeout.tv_sec	= (long)(dblTimeout);
//DEL 	tvTimeout.tv_usec	= (long)((dblTimeout - tvTimeout.tv_sec) * 1000);
//DEL 	
//DEL 	BOOL	bReturn	= select(NULL, NULL, &SocketWriteSet, NULL, &tvTimeout);
//DEL 	
//DEL 	DWORD	dwSocketBlocking	= SOCKET_BLOCKING;
//DEL 	if(ioctlsocket(fdSocket, FIONBIO, &dwSocketBlocking)) 
//DEL 	{
//DEL 		TRACE("CSocketWithTimeout::Connect() : Failed to return Blocking mode. [this=0x%08X]\n", (unsigned long)this);
//DEL 		return FALSE;
//DEL 	}
//DEL 	
//DEL 	if(bReturn == 0) 
//DEL 	{
//DEL 		TRACE("CSocketWithTimeout::Connect() : select() Timeout. [this=0x%08X]\n", (unsigned long)this);
//DEL 		return FALSE;
//DEL 	}
//DEL 
//DEL 	return TRUE;
//DEL }


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode) 
{
	// 데이터 읽기
	st_client_info[m_n_address].n_rev_length = Receive(st_client_info[m_n_address].ch_rev, 4096);

 //  ::PostMessage(st_handler.hWnd, WM_CLIENT_MSG + m_n_address, CLIENT_REV, m_n_address);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
	if(st_handler_info.cwnd_Main != NULL)
	{
		st_handler_info.cwnd_Main->PostMessage(WM_CLIENT_MSG + m_n_address, CLIENT_REV, m_n_address);
	}
	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode) 
{
	st_client_info[m_n_address].n_rev_length = Receive(st_client_info[m_n_address].ch_rev, 4096);
	
//	::PostMessage(st_handler.hWnd, WM_CLIENT_MSG + m_n_address, CLIENT_CLOSE, m_n_address);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
	if(st_handler_info.cwnd_Main != NULL)
	{
		st_handler_info.cwnd_Main->PostMessage(WM_CLIENT_MSG + m_n_address, CLIENT_CLOSE, m_n_address);
	}
	CSocket::OnClose(nErrorCode);
}

void CClientSocket::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CSocket::OnAccept(nErrorCode);
}

void CClientSocket::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	CSocket::OnConnect(nErrorCode);
}

BOOL CClientSocket::IsConnect()
{
	return 0;
}

BOOL CClientSocket::Create(int n_address, CString str_ip, int n_port, int n_timeout)
{
	if(!CSocket::Create())
	{
//		AfxMessageBox("Error : CSocket::Create() in socket");
		return FALSE;
	}
	
	CSocket::m_nTimeOut = n_timeout;
	if(!CSocket::Connect(str_ip, n_port))
	{
		CSocket::Close();
//		AfxMessageBox("Error : CSocket::Connect() in socket");
		return FALSE;
	}

	m_n_address = n_address;
	return TRUE;
}

int CClientSocket::Receive(void* lpBuf, int nBufLen, int nFlags) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}

BOOL CClientSocket::ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	CTime  curt, st;
	CTimeSpan span(0, 0, 0, m_nTimeOut);

	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}
	
	m_nConnectError = -1;
	
	if(!CAsyncSocket::ConnectHelper(lpSockAddr, nSockAddrLen))
	{
		if(GetLastError() == WSAEWOULDBLOCK)
		{
			st = CTime().GetCurrentTime();
			while(PumpMessages(FD_CONNECT))
			{
				if(m_nConnectError != -1)
				{
					WSASetLastError(m_nConnectError);
					return (m_nConnectError == 0);
				}
				curt = CTime().GetCurrentTime();
				if(curt > (st + span))
				{
					return FALSE;
				}
			}
		}
		return FALSE;
	}
	return TRUE;
}