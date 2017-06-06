// XGem.h: interface for the CXGem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XGEM_H__9575B0B1_9174_46B6_8089_D7DFB6CE1544__INCLUDED_)
#define AFX_XGEM_H__9575B0B1_9174_46B6_8089_D7DFB6CE1544__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "exgem.h"

class CXGem : public CeXGem  
{
public:
	long GetAscii(long nObjectID, CString* psValue, long *pnCount)
	{
		long nRet = 0;
		BSTR * pbstr = new BSTR;
		
		nRet = CeXGem::GetAscii(nObjectID, pbstr, pnCount);
		if (nRet == 0) { 
			*psValue = *pbstr;
		}
		
		// Memory Closing...
		delete pbstr; pbstr = NULL;

		return nRet;
	}


	long GEMGetParam(LPCTSTR sParamName, CString* psParamValue)
	{
		long nRet = 0;
		BSTR * pbstr = new BSTR;
		
		nRet = CeXGem::GEMGetParam(sParamName, pbstr);
		if (nRet == 0) { 
			*psParamValue = *pbstr;
		}
		
		// Memory Closing...
		delete pbstr; pbstr = NULL;

		return nRet;
	}

	long GEMSetECVChanged(long nCount, long* pnEcIds, CString* psEcVals)
	{
		long nRet = 0;
		int i = 0;
		BSTR * pbstr = NULL;

		if (nCount > 0) { pbstr = new BSTR[nCount]; }

		for(i = 0; i < nCount; i++) {
			pbstr[i] = psEcVals[i].AllocSysString();
		}
		nRet = CeXGem::GEMSetECVChanged(nCount, pnEcIds, pbstr);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr[i]); 
		}
		delete [] pbstr; pbstr = NULL;

		return nRet;
	}
	long GEMSetPPFmtChanged(long nMode, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, CString* psCCode, long* pnParamCount, CString* psParamNames)
	{
		long nRet = 0;
		int i = 0;
		int nParamTotalCount = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) { pbstr1 = new BSTR[nCount]; }
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psCCode[i].AllocSysString();
		}

		for(i = 0;  i < nCount; i++) {
			nParamTotalCount += pnParamCount[i];
		}

		if (nParamTotalCount > 0) { pbstr2 = new BSTR[nParamTotalCount]; }
		for(i = 0; i < nParamTotalCount; i++) {
			pbstr2[i] = psParamNames[i].AllocSysString();
		}

		nRet = CeXGem::GEMSetPPFmtChanged(nMode, sPpid, sMdln, sSoftRev, nCount, pbstr1, pnParamCount, pbstr2);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr1[i]); 
		}
		for(i = 0; i < nParamTotalCount; i++) { 
			::SysFreeString(pbstr2[i]); 
		}

		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}

	long GEMRspPPDelete(long nMsgId, long nCount, CString* psPpids, long nResult)
	{
		long nRet = 0;
		int i = 0;
		BSTR * pbstr = NULL;

		if (nCount > 0) { pbstr = new BSTR[nCount]; }
		for(i = 0; i < nCount; i++) {
			pbstr[i] = psPpids[i].AllocSysString();
		}
		nRet = CeXGem::GEMRspPPDelete(nMsgId, nCount, pbstr, nResult);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr[i]); 
		}
		delete [] pbstr; pbstr = NULL;

		return nRet;
	}
	long GEMRspPPList(long nMsgId, long nCount, CString* psPpids)
	{
		long nRet = 0;
		int i = 0;
		BSTR * pbstr = NULL;

		if (nCount > 0) { pbstr = new BSTR[nCount]; }
		for(i = 0; i < nCount; i++) {
			pbstr[i] = psPpids[i].AllocSysString();
		}
		nRet = CeXGem::GEMRspPPList(nMsgId, nCount, pbstr);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr[i]); 
		}
		delete [] pbstr; pbstr = NULL;

		return nRet;
	}

	long GEMReqPPFmtSend(LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, CString* psCCode, long* pnParamCount, CString* psParamNames)
	{
		long nRet = 0;
		int i = 0;
		int nParamTotalCount = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) { pbstr1 = new BSTR[nCount]; }
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psCCode[i].AllocSysString();
		}

		for(i = 0;  i < nCount; i++) {
			nParamTotalCount += pnParamCount[i];
		}
		if (nParamTotalCount > 0) { pbstr2 = new BSTR[nParamTotalCount]; }
		for(i = 0; i < nParamTotalCount; i++) {
			pbstr2[i] = psParamNames[i].AllocSysString();
		}

		nRet = CeXGem::GEMReqPPFmtSend(sPpid, sMdln, sSoftRev, nCount, pbstr1, pnParamCount, pbstr2);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr1[i]); 
		}
		for(i = 0; i < nParamTotalCount; i++) { 
			::SysFreeString(pbstr2[i]); 
		}

		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}

	long GEMRspPPFmt(long nMsgId, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, CString* psCCode, long* pnParamCount, CString* psParamNames)
	{
		long nRet = 0;
		int i = 0;
		int nParamTotalCount = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) { pbstr1 = new BSTR[nCount]; }
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psCCode[i].AllocSysString();
		}

		for(i = 0;  i < nCount; i++) {
			nParamTotalCount += pnParamCount[i];
		}
		if (nParamTotalCount > 0) { pbstr2 = new BSTR[nParamTotalCount]; }
		for(i = 0; i < nParamTotalCount; i++) {
			pbstr2[i] = psParamNames[i].AllocSysString();
		}

		nRet = CeXGem::GEMRspPPFmt(nMsgId, sPpid, sMdln, sSoftRev, nCount, pbstr1, pnParamCount, pbstr2);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr1[i]); 
		}
		for(i = 0; i < nParamTotalCount; i++) { 
			::SysFreeString(pbstr2[i]); 
		}

		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}
	
	long GEMReqPPFmtVerification(LPCTSTR sPpid, long nCount, long* pnAck, CString* psSeqNumber, CString* psError)
	{
		long nRet = 0;
		int i = 0;
		int nParamTotalCount = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) { 
			pbstr1 = new BSTR[nCount]; 
			pbstr2 = new BSTR[nCount]; 
		}
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psSeqNumber[i].AllocSysString();
			pbstr2[i] = psError[i].AllocSysString();
		}

		nRet = CeXGem::GEMReqPPFmtVerification(sPpid, nCount, pnAck, pbstr1, pbstr2);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr1[i]); 
			::SysFreeString(pbstr2[i]); 
		}

		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}


	long GEMSetVariable(long nCount, long* pnVid, CString* psValue)
	{
		long nRet = 0;
		int i = 0;
		BSTR * pbstr = NULL;

		if (nCount > 0) {
			pbstr = new BSTR[nCount];
		}

		for(i = 0; i < nCount; i++) {
			pbstr[i] = psValue[i].AllocSysString();
		}
		nRet = CeXGem::GEMSetVariable(nCount, pnVid, pbstr);

		// Memory Closing...
		for(i = 0; i < nCount; i++) { 
			::SysFreeString(pbstr[i]); 
		}
		delete [] pbstr; pbstr = NULL;

		return nRet;
	}


	long GEMGetVariable(long nCount, long *pnVid, CString *psValue)
	{
		long nRet = 0;
		BSTR * pbstr = NULL;

		if (nCount > 0) {
			pbstr = new BSTR[nCount];
		}
		
		nRet = CeXGem::GEMGetVariable(nCount, pnVid, pbstr);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				psValue[i] = pbstr[i];
				::SysFreeString(pbstr[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr; pbstr = NULL;

		return nRet;
	}

	long GEMGetVarName(long nCount, CString *psVid, CString *psValue)
	{
		long nRet = 0;
		long i = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) {
			pbstr1 = new BSTR[nCount];
			pbstr2 = new BSTR[nCount];
		}
		
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psVid[i].AllocSysString();
			pbstr2[i] = psValue[i].AllocSysString();
		}

		nRet = CeXGem::GEMGetVarName(nCount, pbstr1, pbstr2);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				psValue[i] = pbstr2[i];
				::SysFreeString(pbstr1[i]);
				::SysFreeString(pbstr2[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}

	long GEMSetVarName(long nCount, CString *psVid, CString *psValue)
	{
		long nRet = 0;
		long i = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) {
			pbstr1 = new BSTR[nCount];
			pbstr2 = new BSTR[nCount];
		}
		
		for(i = 0; i < nCount; i++) {
			pbstr1[i] = psVid[i].AllocSysString();
			pbstr2[i] = psValue[i].AllocSysString();
		}

		nRet = CeXGem::GEMSetVarName(nCount, pbstr1, pbstr2);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				::SysFreeString(pbstr1[i]);
				::SysFreeString(pbstr2[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}

	long GEMGetAlarmInfo(long nCount, long* pnALIDs, long* pnALCDs, CString* psALTXs)
	{
		long nRet = 0;
		long i = 0;
		BSTR * pbstr1 = NULL;

		if (nCount > 0) {
			pbstr1 = new BSTR[nCount];
		}

		nRet = CeXGem::GEMGetAlarmInfo(nCount, pnALIDs, pnALCDs, pbstr1);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				psALTXs[i] = pbstr1[i];
				::SysFreeString(pbstr1[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr1; pbstr1 = NULL;

		return nRet;
	}
	long GEMGetSVInfo(long nCount, long* pnSVIDs, CString* psMins, CString* psMaxs)
	{
		long nRet = 0;
		long i = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;

		if (nCount > 0) {
			pbstr1 = new BSTR[nCount];
			pbstr2 = new BSTR[nCount];
		}

		nRet = CeXGem::GEMGetSVInfo(nCount, pnSVIDs, pbstr1, pbstr2);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				psMins[i] = pbstr1[i];
				psMaxs[i] = pbstr2[i];
				::SysFreeString(pbstr1[i]);
				::SysFreeString(pbstr2[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}
	
	long GEMGetECVInfo(long nCount, long* pnEcIds, CString* psNames, CString* psDefs, CString* psMins, CString* psMaxs, CString* psUnits)
	{
		long nRet = 0;
		long i = 0;
		BSTR * pbstr1 = NULL;
		BSTR * pbstr2 = NULL;
		BSTR * pbstr3 = NULL;
		BSTR * pbstr4 = NULL;
		BSTR * pbstr5 = NULL;

		if (nCount > 0) {
			pbstr1 = new BSTR[nCount];
			pbstr2 = new BSTR[nCount];
			pbstr3 = new BSTR[nCount];
			pbstr4 = new BSTR[nCount];
			pbstr5 = new BSTR[nCount];
		}

		nRet = CeXGem::GEMGetECVInfo(nCount, pnEcIds, pbstr1, pbstr2, pbstr3, pbstr4, pbstr5);
		if (nRet == 0) {
			for(int i = 0; i < nCount; i++) { 
				psNames[i] = pbstr1[i];
				psDefs[i] = pbstr2[i];
				psMins[i] = pbstr3[i];
				psMaxs[i] = pbstr4[i];
				psUnits[i] = pbstr5[i];
				::SysFreeString(pbstr1[i]);
				::SysFreeString(pbstr2[i]);
				::SysFreeString(pbstr3[i]);
				::SysFreeString(pbstr4[i]);
				::SysFreeString(pbstr5[i]);
			}
		}
		
		// Memory Closing...
		delete [] pbstr1; pbstr1 = NULL;
		delete [] pbstr2; pbstr2 = NULL;
		delete [] pbstr2; pbstr2 = NULL;
		delete [] pbstr2; pbstr2 = NULL;
		delete [] pbstr2; pbstr2 = NULL;

		return nRet;
	}

	CXGem(){};
	virtual ~CXGem(){};

};

#endif // !defined(AFX_XGEM_H__9575B0B1_9174_46B6_8089_D7DFB6CE1544__INCLUDED_)
