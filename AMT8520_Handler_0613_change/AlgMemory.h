// AlgMemory.h: interface for the CAlgMemory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALGMEMORY_H__8D8B0D00_16EE_45D3_B880_AF8ED0888ACC__INCLUDED_)
#define AFX_ALGMEMORY_H__8D8B0D00_16EE_45D3_B880_AF8ED0888ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MSG_NORMAL			0
#define MSG_ABNORMAL			1

typedef struct _S_QUEUE_
{
	int			m_nRear;
	int			m_nFront;
	int			m_nCount;

	char		m_chMsg[100][5000];
} S_QUEUE, *PS_QUEUE;


class CAlgMemory  
{
public:
	HANDLE		m_hMemMap[2];
	PS_QUEUE	m_pQueue[2];

	bool		OnMemoryOpen();
	void		OnMemoryClose();
	void		OnNormalMessageWrite(CString strMsg);
	void		OnAbNormalMessagWrite(CString strMsg);
	CString		OnNormalMessageRead();
	CString		OnAbNormalMessageRead();
public:
	CAlgMemory();
	virtual ~CAlgMemory();

};

extern CAlgMemory clsMem;
#endif // !defined(AFX_ALGMEMORY_H__8D8B0D00_16EE_45D3_B880_AF8ED0888ACC__INCLUDED_)
