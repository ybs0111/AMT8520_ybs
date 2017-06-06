#if !defined(AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_)
#define AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// Client 소켓 Event
//#define	SCM_CLOSE	WM_USER+2	// Client 소켓 해제
//#define	SCM_RECEIVE	WM_USER+3	// Client로 부터 수신되는 데이터
/////////////////////////////////////////////////////////////////////////////
//	ON_MESSAGE(SCM_CLOSE,   OnClientClose)		// Client 소켓 해제
//	ON_MESSAGE(SCM_RECEIVE, OnClientReceive)	// Client로 부터 수신되는 데이터
//	afx_msg LRESULT OnClientClose(WPARAM wParam, LPARAM lClient);	// Client 소켓 해제
//	afx_msg LRESULT OnClientReceive(WPARAM wParam, LPARAM lClient);	// Client로 부터 수신되는 데이터

/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target
#define	SOCKET_BLOCKING		0
#define	SOCKET_NONBLOCKING	1

#define MAX_RECEIVE_LEN	4096

class CClientSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();


// Overrides
public:
	BOOL Create(int n_address, CString str_ip, int n_port, int n_timeout);
	BOOL IsConnect();
	BYTE	m_by_rev[1024];
	int		m_n_rev_length;
	int     m_n_address;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CWnd*	m_wndParent;	// Parent 윈도우
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__2A49E7C6_67E3_11D3_8401_006097663D30__INCLUDED_)
