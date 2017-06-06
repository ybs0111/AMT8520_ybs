// ManagerSocket.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ManagerSocket.h"
#include ".\\Ctrl\\Variable.h"
#include "DialogManager.h"
#include ".\\Ctrl\\Alg_Mysql.h"
#include "io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CManagerSocketApp

BEGIN_MESSAGE_MAP(CManagerSocketApp, CWinApp)
	//{{AFX_MSG_MAP(CManagerSocketApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManagerSocketApp construction

CManagerSocketApp::CManagerSocketApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CManagerSocketApp object

CManagerSocketApp theApp;

BOOL CManagerSocketApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
// 	CWinApp::InitInstance();
// 	
// 	if (!AfxSocketInit())
// 	{
// 		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
// 		return FALSE;
// 	}
// 	
// 	return TRUE

	//CWinApp::InitInstance();

	return CWinApp::InitInstance();
}

class CDialogManager;
CDialogManager		*m_pMangerDlg;

tagMANAGER_INFO		st_manager_info;
tagSOCKET_INFO      st_socket_info[10][100];

CString	OnCharToString(char chData[], int nLength)
{
	CString strTmp;
	BSTR	buf;
	int		nCnt;

	nCnt	= MultiByteToWideChar(CP_ACP, 0, chData, nLength, NULL, NULL);
	buf		= SysAllocStringLen(NULL, nCnt);
	MultiByteToWideChar(CP_ACP, 0, chData, nLength, buf, nCnt);
	strTmp.Format(_T("%s"), buf);

	SysFreeString(buf);

	return strTmp;
}


void OnStringToChar(CString strData, char chData[])
{
/*	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);*/
}


bool CreateFolder(CString strPath)
{
	int nRet = 0;
	char chfilename[100];

    do
	{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            CString strTemp = strPath.Left(nRet+1);
			
			OnStringToChar(strTemp, chfilename);

            //if (_access(chfilename.operator LPCTSTR(), 00) == -1)
			if (_access(strTemp, 0) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return true;
}

void OpenConfig()
{
	CString strPath = _T("C:\\Manager\\");
	CString strLoadFile = _T("Socket.TXT");
	CString strTemp;
	CString strHead;

	CString strRecipe;

	TCHAR chData[50];

	int i;

	strLoadFile = strPath;
	strLoadFile += _T("Socket.TXT");

	CreateFolder(strPath);
	
	GetPrivateProfileString(_T("HIFIX"), _T("Path"), _T(""), chData, sizeof(chData), strLoadFile);
	strTemp.Format("%s", chData);
	strRecipe.Format("%s", strTemp);

	if (strRecipe == _T(""))
	{
		WritePrivateProfileString(_T("HIFIX"), _T("Path"), LPCTSTR(_T("C:\\Manager\\Recipe.INI")), strLoadFile);

		strRecipe = strPath + _T("Recipe.INI");

		strTemp.Format(_T("%d"), 4);
		WritePrivateProfileString(_T("Recipe_Data"), _T("Total"), LPCTSTR(strTemp), strRecipe);

		strHead.Format(_T("%d"), 1);
		WritePrivateProfileString(_T("Recipe_Data"), strHead, LPCTSTR(strHead), strRecipe);

		strHead.Format(_T("%d"), 2);
		WritePrivateProfileString(_T("Recipe_Data"), strHead, LPCTSTR(strHead), strRecipe);

		strHead.Format(_T("%d"), 3);
		WritePrivateProfileString(_T("Recipe_Data"), strHead, LPCTSTR(strHead), strRecipe);

		strHead.Format(_T("%d"), 4);
		WritePrivateProfileString(_T("Recipe_Data"), strHead, LPCTSTR(strHead), strRecipe);
	}
	else
	{
		GetPrivateProfileString(_T("Recipe_Data"), _T("Total"), _T("0"), chData, sizeof(chData), strRecipe);
		strTemp.Format(_T("%s"), chData);
// 		st_manager_info.nHifixNum = _wtoi(strTemp);
		st_manager_info.nHifixNum = atoi(strTemp);

		for (i=0; i<st_manager_info.nHifixNum; i++)
		{
			strHead.Format(_T("%d"), i + 1);
			GetPrivateProfileString(_T("Recipe_Data"), strHead, _T(""), chData, sizeof(chData), strRecipe);
			st_manager_info.strHifix[i] = (LPCTSTR)chData;
		}
	}

	if (m_pMangerDlg != NULL)
	{
		delete m_pMangerDlg;
	}
	m_pMangerDlg = NULL;
}


void OpenSlotData()
{
	int nNum;
	int nDataCount;
	int nPos[3];
	int i, j, k;
	int nVal[5];
	int nHifix;
	int nSlot;

	CString strTable[8];
	CString strData;
	CString strTemp;

	for (i=0; i<st_manager_info.nHifixNum; i++)
	{
		nVal[0] = i;/* + 1;*/

		nNum = Mysql.MySql_Open();
		if (nNum < 0) break;

		if(Mysql.Database_Select(nNum, _T("AMT")))
		{
			if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
			{
				nDataCount = Mysql.Record_Int_Search(nNum, _T("SOCKET"), _T("HIFIX"), _T("="), nVal);

				if (nDataCount > 0)
				{
					for (j=0; j<nDataCount; j++)
					{
						strData = Mysql.Record_Read(nNum);

						nPos[0] =0;
						for (k=0; k<Mysql.m_nManagerTableCnt; k++)
						{
							nPos[1]		= strData.Find(_T(","), nPos[0] + 1);
							strTable[k]	= strData.Mid(nPos[0], (nPos[1] - nPos[0]));
							nPos[0]		= nPos[1] + 1;
						}

// 						nSlot	= _wtoi(strTable[5]) - 1;
// 						nHifix	= _wtoi(strTable[3]) - 1;
// 						nSlot	= atoi(strTable[5]) - 1;
// 						nHifix	= atoi(strTable[3]) - 1;
						nSlot	= atoi(strTable[5]);
						nHifix	= atoi(strTable[3]);
						
						
						//if (nHifix < 0 || nHifix > 9) break;
						//if (nSlot < 0 || nSlot > 99) break;
						if (nHifix >= 0 && nSlot >= 0)
						{
							//st_socket_info[nHifix][nSlot].nCount	= _wtoi(strTable[4]);
							st_socket_info[nHifix][nSlot].nCount	= atoi(strTable[4]);
							st_socket_info[nHifix][nSlot].strSerial	= strTable[2];
							st_socket_info[nHifix][nSlot].strType	= strTable[6];
						}
					}
				}
			}
		}

		Mysql.MySql_Close(nNum);
	}
}


// DLL Open
extern "C" __declspec(dllexport)void ManagerOpen(int nSlot, int nRow, int nCol)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	st_manager_info.nMaxSlot	= nSlot;
	st_manager_info.nMaxCol		= nCol;
	st_manager_info.nMaxRow		= nRow;

	OpenConfig();
	OpenSlotData();
}

extern "C" __declspec(dllexport)void ManagerClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pMangerDlg != NULL)
	{
		m_pMangerDlg->DestroyWindow();

		delete m_pMangerDlg;
		m_pMangerDlg = NULL;
	}
}

extern "C" __declspec(dllexport)void ManagerDisplay(CPoint cpXY, int nHifix)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pMangerDlg == NULL)
	{
		m_pMangerDlg					= new CDialogManager;
		m_pMangerDlg->m_cpXY			= cpXY;
		m_pMangerDlg->m_nHifixNum		= nHifix;
		m_pMangerDlg->Create();
		m_pMangerDlg->ShowWindow(SW_SHOW);
	}
}

extern "C" __declspec(dllexport)void ManagerWriteData(int nSlot, int nHifix, int nCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nNum;
	int nDataCount;
	int nPos[3];
	int i;

	CString strTable[8];
	CString strData;
	CString strTemp;

	nNum = Mysql.MySql_Open();
	if (nNum < 0) return;

	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			nDataCount = Mysql.Record_Slot_Search(nNum, _T("SOCKET"), nSlot, nHifix);

			if (nDataCount > 0)
			{
				strData = Mysql.Record_Read(nNum);

				nPos[0] =0;
				for (i=0; i<Mysql.m_nManagerTableCnt; i++)
				{
					nPos[1]		= strData.Find(_T(","), nPos[0] + 1);
					strTable[i]	= strData.Mid(nPos[0], (nPos[1] - nPos[0]));
					nPos[0]		= nPos[1] + 1;
				}

				strTemp.Format(_T("%d"), nCount);

				Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SLOT"), strTable[5], _T("HIFIX"), strTable[3], _T("COUNT"), strTemp);
			}
			else
			{
				Mysql.MySql_Close(nNum);
				
				return;
			}
		}
	}

	Mysql.MySql_Close(nNum);
}

extern "C" __declspec(dllexport)void ManagerReadData(int nSlot, int nHifix, CString *strSerial, int *nCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nSlotNum;
	int nHifixNum;

	nSlotNum = nSlot;// nSlot - 1;
	if (nSlotNum < 0 || nSlotNum > 19) 
	{
		*strSerial	= _T("");
		*nCount		= -1;

		return;
	}

	nHifixNum = nHifix;//nHifix - 1;
	if (nHifixNum < 0 || nHifixNum > 9) 
	{
		*strSerial	= _T("");
		*nCount		= -1;

		return;
	}

	*strSerial	= st_socket_info[nHifixNum][nSlotNum].strSerial;
	*nCount		= st_socket_info[nHifixNum][nSlotNum].nCount;
}