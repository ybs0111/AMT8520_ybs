// AlgMemory.cpp: implementation of the CAlgMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AlgMemory.h"
#include "Variable.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define SHAREMEM_NAME			"ALARM_SHAREMEM"

CAlgMemory clsMem;

CAlgMemory::CAlgMemory()
{
	m_hMemMap[0]	= m_hMemMap[1]	= NULL;
	m_pQueue[0]		= m_pQueue[1]	= NULL;

	OnMemoryOpen();
}

CAlgMemory::~CAlgMemory()
{
	OnMemoryClose();
}


bool CAlgMemory::OnMemoryOpen()
{
	int i;

	for (i=0; i<2; i++)
	{
		m_hMemMap[i] = CreateFileMapping(INVALID_HANDLE_VALUE,
										 NULL,
										 PAGE_READWRITE,
										 0,
										 sizeof(S_QUEUE),
										 SHAREMEM_NAME);
		if (m_hMemMap[i] == NULL)
		{
			return false;
		}
		
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return false;
		}
		
		
		m_pQueue[i] = (PS_QUEUE)MapViewOfFile(m_hMemMap[i], FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (m_pQueue[i] == NULL)
		{
			return false;
		}
	}

	m_pQueue[MSG_NORMAL]->m_nFront	= 0;
	m_pQueue[MSG_NORMAL]->m_nRear	= 0;
	m_pQueue[MSG_NORMAL]->m_nCount	= 0;

	return true;
}

void CAlgMemory::OnMemoryClose()
{
	int i;

	for (i=0; i<2; i++)
	{
		if (m_pQueue[i] != NULL)UnmapViewOfFile(m_pQueue[i]);
		
		if (m_hMemMap[i] != NULL) CloseHandle(m_hMemMap[i]);
	}

}

void CAlgMemory::OnNormalMessageWrite(CString strMsg)
{
	if (m_hMemMap[MSG_NORMAL] != NULL && m_pQueue[MSG_NORMAL] != NULL)
	{
		lstrcpyn(m_pQueue[MSG_NORMAL]->m_chMsg[m_pQueue[MSG_NORMAL]->m_nRear], strMsg, 5000);

		m_pQueue[MSG_NORMAL]->m_nRear++;

		if (m_pQueue[MSG_NORMAL]->m_nRear >= 100) m_pQueue[MSG_NORMAL]->m_nRear = 0;
	
		m_pQueue[MSG_NORMAL]->m_nCount++;
	}
}

void CAlgMemory::OnAbNormalMessagWrite(CString strMsg)
{
	if (m_hMemMap[MSG_ABNORMAL] != NULL && m_pQueue[MSG_ABNORMAL] != NULL)
	{
		lstrcpyn(m_pQueue[MSG_ABNORMAL]->m_chMsg[m_pQueue[MSG_ABNORMAL]->m_nRear], strMsg, 5000);
		
		m_pQueue[MSG_ABNORMAL]->m_nRear++;
		
		if (m_pQueue[MSG_ABNORMAL]->m_nRear >= 100) m_pQueue[MSG_ABNORMAL]->m_nRear = 0;
		
		m_pQueue[MSG_ABNORMAL]->m_nCount++;
	}
}

CString CAlgMemory::OnNormalMessageRead()
{
	CString strData;

	if (m_hMemMap[MSG_NORMAL] != NULL && m_pQueue[MSG_NORMAL] != NULL)
	{
		if (m_pQueue[MSG_NORMAL]->m_nCount > 0 )
		{
			strData = m_pQueue[MSG_NORMAL]->m_chMsg[m_pQueue[MSG_NORMAL]->m_nFront];
			
			m_pQueue[MSG_NORMAL]->m_nFront++;
			if (m_pQueue[MSG_NORMAL]->m_nFront >= 100) m_pQueue[MSG_NORMAL]->m_nFront = 0;
			
			m_pQueue[MSG_NORMAL]->m_nCount--;
		}
	}

	return strData;
}

CString	CAlgMemory::OnAbNormalMessageRead()
{
	CString strData;
	
	if (m_hMemMap[MSG_ABNORMAL] != NULL && m_pQueue[MSG_ABNORMAL] != NULL)
	{
		if (m_pQueue[MSG_ABNORMAL]->m_nCount > 0 )
		{
			strData = m_pQueue[MSG_ABNORMAL]->m_chMsg[m_pQueue[MSG_ABNORMAL]->m_nFront];
			
			m_pQueue[MSG_ABNORMAL]->m_nFront++;
			if (m_pQueue[MSG_ABNORMAL]->m_nFront >= 100) m_pQueue[MSG_ABNORMAL]->m_nFront = 0;
			
			m_pQueue[MSG_ABNORMAL]->m_nCount--;
		}
	}

	return strData;
}
