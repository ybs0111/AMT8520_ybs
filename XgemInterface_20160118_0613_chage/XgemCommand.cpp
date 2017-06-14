// XgemCommand.cpp: implementation of the CXgemCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xgeminterface.h"
#include "XgemCommand.h"
#include "Variable.h"
#include "Global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXgemCommand clsXgem;

CXgemCommand::CXgemCommand()
{
	m_nXgemOpenFlag		= CONNECT_NO;
	m_nXgemStartFlag	= CONNECT_NO;

}

CXgemCommand::~CXgemCommand()
{

}

int CXgemCommand::OnEqCommunication(CXGem *hXgem, CString strRecv)
{
	int		nPos[2];
	int		nLength;
	long	nRet;
	CString strConfig;

	nPos[0] = strRecv.Find("CONFIG=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);

	nLength = (nPos[1] - nPos[0]) - 7;

	strConfig	= "";
	strConfig = strRecv.Mid(nPos[0] + 7, nLength);

	if (strConfig == "") return CTLBD_RET_ERROR;

	m_hXgem = hXgem;
	
	nRet = m_hXgem->Initialize(strConfig);

	if (nRet < 0)
	{
		sprintf(st_msg_info.c_normal_msg, "XGem Initialize Failed");
		if (st_handler_info.cwnd_Main != NULL)
		{
			st_handler_info.cwnd_Main->PostMessage(WM_LIST_DATA, NORMAL_MSG, 0);
		}
		return CTLBD_RET_ERROR;
	}
	m_nXgemOpenFlag	= CONNECT_YES;
	
	nRet = m_hXgem->Start();
	
	if (nRet < 0)
	{
		sprintf(st_msg_info.c_normal_msg, "XGem Start Failed");
		if (st_handler_info.cwnd_Main != NULL)
		{
			st_handler_info.cwnd_Main->PostMessage(WM_LIST_DATA, NORMAL_MSG, 0);
		}
		return CTLBD_RET_ERROR;
	}


	m_nXgemStartFlag	= CONNECT_YES;

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqMachineOperate(CString strRecv)
{
	int		nPos[2];
	int		nLength;
	CString strConnect;
	
	nPos[0] = strRecv.Find("CONNECT=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 8;
	
	strConnect	= "";
	strConnect = strRecv.Mid(nPos[0] + 8, nLength);
	
	if (strConnect == "") return CTLBD_RET_ERROR;

	if (strConnect == "0")
	{
		// off - line 
		if (m_hXgem->GEMReqOffline() < 0)
		{
			return CTLBD_RET_ERROR;
		}
	}
	else if (strConnect == "1")
	{
		// on - line local
		if (m_hXgem->GEMReqLocal() < 0)
		{
			return CTLBD_RET_ERROR;
		}
	}
	else if (strConnect == "2")
	{
		// on - line remote
		if (m_hXgem->GEMReqRemote() < 0)
		{
			return CTLBD_RET_ERROR;
		}
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqAlarm(CString strRecv)
{
	int		nPos[2];
	int		nLength;
//	int		nCeid;

	long	nCode;
	long	nStatus;
//	long	lVid[10];
	
	CString strValue[10];

	nPos[0] = strRecv.Find("CODE=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 5;
	
	nCode = atoi(strRecv.Mid(nPos[0] + 5, nLength));

	nPos[0] = strRecv.Find("STATUS=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 7;
	
	nStatus = atoi(strRecv.Mid(nPos[0] + 7, nLength));
	
	if (m_hXgem->GEMSetAlarm(nCode, nStatus) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	if(nStatus == 1)
	{
		OnEqMachineStatus(MC_ALARM);

//		nCeid		= 1002;
//		lVid[0]		= m_nVid[nCeid][0][0];
	}
	else 
	{
		OnEqMachineStatus(MC_STOP);

//		nCeid		= 1003;
//		lVid[0]		= m_nVid[nCeid][0][0];
	}
//	strValue[0].Format("%d", 1);

//	m_hXgem->GEMSetVariable(1, lVid, strValue);
//	m_hXgem->GEMSetEvent(nCeid);

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqCEID(CString strRecv)
{
	int nCeid;
	int nReport;
	int nReportCnt[10];
	int nVidCnt[10][10];
	int nPos[2];
	int nLength;
	int i, j, k;
	int nCnt;
	long lVid[10];
	long lObjid;

	CString strReport[10][10];
	CString strTmp;
	CString strRpt;
	CString strVid;
	CString strValue[10];

	nPos[0] = strRecv.Find("CEID_NUM=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	nLength = (nPos[1] - nPos[0]) - 9;
	nCeid = atoi(strRecv.Mid(nPos[0] + 9, nLength));

	nPos[0] = strRecv.Find("REPORT=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	nLength = (nPos[1] - nPos[0]) - 7;
	nReport = atoi(strRecv.Mid(nPos[0] + 7, nLength));

	for(i=0; i<nReport; i++)
	{
		strTmp.Format("REP_%02d={", i+1);
		nPos[0]			= strRecv.Find(strTmp, 0);
		nPos[1]			= strRecv.Find("}", nPos[0]);
		nLength			= (nPos[1] - nPos[0]) - 8;
		strRpt			= strRecv.Mid(nPos[0] + 8, nLength);

		nPos[0]			= strRpt.Find("CNT=", 0);
		nPos[1]			= strRpt.Find(",", nPos[0]);
		nLength			= (nPos[1] - nPos[0]) - 4;
		nReportCnt[i]	= atoi(strRpt.Mid(nPos[0] + 4, nLength));

		for(j=0; j<nReportCnt[i]; j++)
		{
			strTmp.Format("[DA_%02d=", j+1);
			nPos[0]			= strRpt.Find(strTmp, 0);
			nPos[1]			= strRpt.Find("]", nPos[0]);
			nLength			= (nPos[1] - nPos[0]) - 7;
			strVid			= strRpt.Mid(nPos[0] + 7, nLength);

			nVidCnt[i][j]	= atoi(strVid.Mid(0, 2));
			strReport[i][j].Format("[%s]", strVid.Mid(3, strVid.GetLength()-3));
		}
	}

	for (i=0; i<nReport; i++)
	{
		nCnt = 0;
		for (j=0; j<nReportCnt[i]; j++)
		{
			if (nVidCnt[i][j] == 1)
			{
				lVid[0]		= m_nVid[nCeid][i][j];
				nPos[0]		= strReport[i][j].Find("[", 0);
				nPos[1]		= strReport[i][j].Find("]", nPos[0]);
				nLength		= (nPos[1] - nPos[0]) - 1;
				strValue[0] = strReport[i][j].Mid(nPos[0] + 1, nLength);

				if (m_hXgem->GEMSetVariable(1, lVid, strValue) < 0)
				{
					return CTLBD_RET_ERROR;
				}
			}
			else
			{
				if (nVidCnt[i][j] <= 0) break;

				lVid[0]		= m_nVid[nCeid][i][j];
				nCnt		= nVidCnt[i][j];
				// LINK 만들기 
				if (m_hXgem->MakeObject(&lObjid) < 0)
				{
					return CTLBD_RET_ERROR;
				}
				// LINK LIST 수량 설정 
				if (m_hXgem->SetList(lObjid, nCnt) < 0)
				{
					return CTLBD_RET_ERROR;
				}

				nPos[0] = 1;
				for (k=0; k<nCnt; k++)
				{
					nPos[1]		= -1;
					nPos[1]		= strReport[i][j].Find(",", nPos[0]);
					if (nPos[1] < 0)
					{
						nLength		= ((strReport[i][j].GetLength() - 1) - nPos[0]);
					}
					else
					{
						nLength		= (nPos[1] - nPos[0]);
					}
					
					strTmp		= strReport[i][j].Mid(nPos[0], nLength);
					// LINK LIST 데이터 넣기
					if (m_hXgem->SetAscii(lObjid, strTmp, strTmp.GetLength()) < 0)
					{
						return CTLBD_RET_ERROR;
					}
					nPos[0]		= nPos[1] + 1;
				}
				// CEID값 넣기
				if (m_hXgem->GEMSetVariables(lObjid, lVid[0]) < 0)
				{
					return CTLBD_RET_ERROR;
				}
			}
		}
	}

	if (m_hXgem->GEMSetEvent(nCeid) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnHostGetCurrentTime()
{
	if (m_hXgem->GEMReqGetDateTime() < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnHostSetCurrentTime(long lMsgID, int nRes)
{
	if (m_hXgem->GEMRspDateTime(lMsgID, nRes) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqGetCurrentTime(long lMsgID)
{
	CString strTime;
	SYSTEMTIME Time; 
	
	::GetLocalTime(&Time);	

	strTime.Format("%04d%02d%02d%02d%02d%02d", Time.wYear, 
											   Time.wMonth,
											   Time.wDay,
											   Time.wHour,
											   Time.wMinute, 
											   Time.wSecond);

	if (m_hXgem->GEMRspGetDateTime(lMsgID, strTime) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnHostRCMD(long lMsgID, long lReport, CString strRcmd, CString strRecv)
{
	long nResult = 0;
	CString strTmp,strHead;

	if (m_hXgem->GEMRspRemoteCommand(lMsgID, strRcmd, 0, 1, &nResult) < 0)
	{
		return CTLBD_RET_ERROR;
	}
	
// 	strTmp.Format("HD=%06d,FUNC=RCMD,RCMD_NUM=%s,REPORT=%02d,%s", strRecv.GetLength(), strRcmd, lReport, strRecv);
//  	strTmp.Format("FUNC=RCMD,RCMD_NUM=%s,REPORT=%02d,%s", strRcmd, lReport, strRecv);//2013,1104
// 	strHead.Format("HD=%06d,", strTmp.GetLength());
// 	sprintf(st_server_info[0].ch_send, strHead+strTmp);
	strTmp.Format("FUNC=RCMD,RCMD_NUM=%s,REPORT=%02d,%s", strRcmd, lReport, strRecv);//2013,1104
	strHead.Format("HD=%06d,%s", strTmp.GetLength(), strTmp);
	sprintf(st_server_info[0].ch_send, strHead);

// 	sprintf(st_server_info[0].ch_send, strTmp);
	::Sleep(500);
	if(st_handler_info.cwnd_Main != NULL)
	{
		st_handler_info.cwnd_Main->PostMessage(WM_SERVER_MSG_1, SERVER_SEND, 0);
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqPpidChange(CString strID)
{
//	m_hXgem->GEMSetPPChanged();
	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqMachineOffLine()
{
	long Ret;
	
	Ret = m_hXgem->GEMReqOffline();
	
	if(Ret < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqMachineLocal()
{
	long Ret;
	
	Ret = m_hXgem->GEMReqLocal();
	
	if(Ret < 0)
	{
		return CTLBD_RET_ERROR;
	}
	
	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqMachineRemote()
{
	long Ret;
	
	Ret = m_hXgem->GEMReqRemote();
	
	if(Ret < 0)
	{
		return CTLBD_RET_ERROR;
	}
	
	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqMachineStatus(int nStatus)
{
	int nCeid = 1001;
	long lVid[10];
	
	CString strValue[10];

	lVid[0]		= m_nVid[nCeid][0][0];
	strValue[0] = m_strEqID;
	
	lVid[1]		= m_nVid[nCeid][0][1];
	strValue[1].Format("%d", 3);
	
	if (m_hXgem->GEMSetVariable(2, lVid, strValue) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	if (m_hXgem->GEMSetEvent(1001) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnXgemMessage(long nObjectID, long nStream, long nFunction, long nSysbyte)
{
//	short   nI1, nI2, nU1, nBinary, nBool;
    long    lCnt;
//    float   rF4;
//    double  rU4, rU8, rF8;
	short   nBinary;
    CString strAscii[100];
	CString strMsg;
	
//	long	lSize;
	
	switch(nStream)
	{
		case 7:
				switch(nFunction)
				{
					case 2:
						m_hXgem->GetBinary(nObjectID, &nBinary, &lCnt, 1 );
						m_hXgem->CloseObject(nObjectID);

						if(nBinary == 0)
						{
							strMsg += "[XGEM<->HOST] S7F1/S7F2 MESSAGE SUCCESS";
							sprintf(st_msg_info.c_normal_msg, strMsg);
							if (st_handler_info.cwnd_Main != NULL)
							{
								st_handler_info.cwnd_Main->PostMessage(WM_LIST_DATA, NORMAL_MSG, 0);
							}

							if (OnEqPPIDSend(m_strPPID, m_strPPBody) == CTLBD_RET_ERROR)
							{
								return CTLBD_RET_ERROR;
							}
						}
						else
						{
							strMsg += "[XGEM<->HOST] S7F1/S7F2 MESSAGE FAIL";
							sprintf(st_msg_info.c_normal_msg, strMsg);
							if (st_handler_info.cwnd_Main != NULL)
							{
								st_handler_info.cwnd_Main->PostMessage(WM_LIST_DATA, NORMAL_MSG, 0);
							}
						}
						break;

					case 4:
						m_hXgem->GetBinary(nObjectID, &nBinary, &lCnt, 1 );
						m_hXgem->CloseObject(nObjectID);

						if(nBinary == 0)
						{
							strMsg += "[XGEM<->HOST] S7F3/S7F4 MESSAGE SUCCESS";
						}
						else
						{
							strMsg += "[XGEM->HOST] S7F3/S7F4 MESSAGE FAIL";
						}

						sprintf(st_msg_info.c_normal_msg, strMsg);
						if (st_handler_info.cwnd_Main != NULL)
						{
							st_handler_info.cwnd_Main->PostMessage(WM_LIST_DATA, NORMAL_MSG, 0);
						}
						break;
				}
				break;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnXgemEvent(int nState)
{
	switch(nState)
	{
		case XGemState_Execute: 
			//bEnable : 0(Disabled), 1(Enabled)
			if (m_hXgem->GEMSetEstablish(1) < 0)
			{
				return CTLBD_RET_ERROR;
			}

			
			if (m_hXgem->GEMEQInitialized(1) < 0)
			{
				return CTLBD_RET_ERROR;
			}
			break;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqPPIDLoad(CString strPPID)
{
	long	nObjectID;
	long    nSysbyte = 0;
	double  rU4 = 10000;
	CString strAscii;

	if (m_hXgem->MakeObject(&nObjectID) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	if (m_hXgem->SetList( nObjectID, 2 ) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	strAscii = strPPID;
	if (m_hXgem->SetAscii( nObjectID, strAscii, strAscii.GetLength()) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	if (m_hXgem->SetU4(nObjectID, &rU4, 1) < 0)
	{
		return CTLBD_RET_ERROR;
	}
	if (m_hXgem->SendSECSMessage(nObjectID, 7, 1, nSysbyte) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}

int CXgemCommand::OnEqPPIDSend(CString strPPID, CString strBody)
{
	long	nObjectID;
	long    nSysbyte = 0;

	char	chBody[10000];
	short   nBinary[10000];

	int		nLength;
	int		i;
	
	CString strAscii;

	nLength = strBody.GetLength();

	memset(nBinary, 0, sizeof(nBinary));
	sprintf(chBody, strBody);
	for(i=0; i<nLength; i++)
	{
		nBinary[i] = chBody[i];
	}

	if (m_hXgem->MakeObject(&nObjectID) < 0)
	{
		return CTLBD_RET_ERROR;
	}
	if (m_hXgem->SetList( nObjectID, 2 ) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	strAscii = strPPID;
	if (m_hXgem->SetAscii(nObjectID, strAscii, strAscii.GetLength()) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	if (m_hXgem->SetBinary(nObjectID, nBinary, strBody.GetLength()) < 0)
	{
		return CTLBD_RET_ERROR;
	}
	if (m_hXgem->SendSECSMessage(nObjectID, 7, 3, nSysbyte) < 0)
	{
		return CTLBD_RET_ERROR;
	}

	return CTLBD_RET_GOOD;
}
