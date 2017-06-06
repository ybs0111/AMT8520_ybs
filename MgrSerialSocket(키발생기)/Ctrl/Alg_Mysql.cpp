// MySqlAlg.cpp: implementation of the MySqlAlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Alg_Mysql.h"
// #define __LCC__
// #include "mysql.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAlg_Mysql Mysql;

MYSQL_RES *pRes;
MYSQL_ROW pRow;
MYSQL_FIELD *pFields;

CAlg_Mysql::CAlg_Mysql()
{
	// Mysql DB 정의
	CString str_tmp;

	int i;
	int nNum;

	for (i=0; i<MAX_NUM; i++)
	{
		m_nMySqlFlag[i]	= 0;
		m_pRes[i]		= NULL;
	}
	// Mysql DB 정의
	// Mysql DB 정의
	m_nManagerTableCnt			= 3;
	m_strManagerTableName[0]	= _T("NO");
	m_strManagerTableName[1]	= _T("DATE");
	m_strManagerTableName[2]	= _T("SERIAL");

	m_nManagerTableNum[0]		= 5;
	m_nManagerTableNum[1]		= 30;
	m_nManagerTableNum[2]		= 20;

	m_nManagerTableType[0]		= ID_TYPE;
	m_nManagerTableType[1]		= DATEAUTO_TYPE;
	m_nManagerTableType[2]		= TEXT_TYPE;


	nNum = MySql_Open(_T("localhost"), _T("root"), _T("1111"), 3306);

	if (nNum < 0) return;

	WaitTime(nNum, 0,1000);
	WaitTime(nNum, 1,300);	
	
	MySql_Close(nNum);
	
	m_pRes[0] = NULL;
}

CAlg_Mysql::~CAlg_Mysql()
{
	int i;

	if(m_strquery != NULL)
	{
		for (i=0; i<MAX_NUM; i++)
		{
			if (m_strquery[i] != NULL)
			{
				delete [] m_strquery[i];
				m_strquery[i] = NULL;
			}
		}
	}
}

CString	CAlg_Mysql::OnCharToString(char chData[], int nLength)
{
	CString strTmp;
/*	BSTR	buf;
	int		nCnt;

	nCnt	= MultiByteToWideChar(CP_ACP, 0, chData, nLength, NULL, NULL);
	buf		= SysAllocStringLen(NULL, nCnt);
	MultiByteToWideChar(CP_ACP, 0, chData, nLength, buf, nCnt);
	strTmp.Format(_T("%s"), buf);

	SysFreeString(buf);*/

	return strTmp;
}


void CAlg_Mysql::OnStringToChar(CString strData, char chData[])
{
/*	int nCnt;
	wchar_t *chBuf;

	chBuf = strData.GetBuffer(strData.GetLength());
	nCnt  = WideCharToMultiByte(CP_ACP, 0, chBuf, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, chBuf, -1, chData, nCnt, 0, 0);*/
}


int CAlg_Mysql::MySql_Open(CString hostname, CString username,  CString password, int port)
{
	// 데이터 베이스 열기
	int nNum = -1;
	int i;
	
	//char chHost[100];
	//char chName[100];
	//char chPass[100];
	//char chQurey[8192];

	CString strMsg;

	//OnStringToChar(hostname, chHost);
	//OnStringToChar(username, chName);
	//OnStringToChar(password, chPass);

	for (i=0; i<MAX_NUM; i++)
	{
		if (m_nMySqlFlag[i] == 0)
		{
			nNum = i;
			break;
		}
	}

	mysql_init(&m_pMySql[nNum]);

	if(!(mysql_real_connect(&m_pMySql[nNum], hostname, username, password, NULL, port, NULL, NULL)))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}

	// 한글 지원하기 위해
	//OnStringToChar(_T("SET NAMES euckr"), chQurey);
	if(mysql_query(&m_pMySql[nNum], _T("SET NAMES euckr")))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);

		return FALSE;
	}
	
	m_nMySqlFlag[nNum] = 1;

	return nNum;
}

BOOL CAlg_Mysql::Database_Create(int nNum, CString databasename)
{
	// 데이터 베이스 새로 추가하기 
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	strQuery = _T("CREATE DATABASE ") + databasename;

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}

	return TRUE;
}

BOOL CAlg_Mysql::Database_Delete(int nNum, CString databasename)
{
	// 데이터 베이스 삭제하기
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	strQuery = _T("DROP DATABASE ") + databasename;
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	
	return TRUE;
}

/*
TYNYINT_TYPE			0
SMALLINT_TYPE			1
MEDIUMINT_TYPE			2
INT_TYPE				3
BIGINT_TYPE				4
FLOAT_TYPE				5
DOUBLE_TYPE				6
DATE_TYPE				7
DATETIME_TYPE			8
TIMESTAMP_TYPE			9 
TIME_TYPE				10
YEAR_TYPE				11
CHAR_TYPE				12
VARCHAR_TYPE			13
TINYBLOB_TYPE			14
TINYTEXT_TYPE			15
BLOB_TYPE				16
TEXT_TYPE				17
MEDIUMBLOB_TYPE			18
MEDIUMTEXT_TYPE			19
LONGBLOB_TYPE			20
LONGTEXT_TYPE			21
NUMBER_TYEP				22
*/

BOOL CAlg_Mysql::Table_Create(int nNum, CString database, CString tablename, int itemnum, CString *itemname, int *itemtype, int *itemhundred)
{
	CString strQuery;
	CString strItem;
	CString strTmp;
	CString strMsg;

	//char chQurey[8192];

	int i;

 	strQuery = _T("USE ") + database;

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}


	for(i=0; i<itemnum; i++)
	{
		switch(itemtype[i])
		{
			case ID_TYPE:
				strTmp.Format(_T("%s INT PRIMARY KEY NOT NULL AUTO_INCREMENT"), itemname[i]);
				strItem += strTmp;
				break;
			case INT_TYPE:
				strTmp.Format(_T("%s INT"), itemname[i]);
				strItem += strTmp;
				break;
			case FLOAT_TYPE:
				strTmp.Format(_T("%s FLOAT"), itemname[i]);
				strItem += strTmp;
				break;
			case DOUBLE_TYPE:
				strTmp.Format(_T("%s DOUBLE"), itemname[i]);
				strItem += strTmp;
				break;
			case DATE_TYPE:
				strTmp.Format(_T("%s DATE"), itemname[i]);
				strItem += strTmp;
				break;
			case DATEAUTO_TYPE:
				strTmp.Format(_T("%s TIMESTAMP"), itemname[i]);
				strItem += strTmp;
				break;
			case TEXT_TYPE:
				strTmp.Format(_T("%s CHAR(%d)"), itemname[i], itemhundred[i]);
				strItem += strTmp;
				break;	
		}

		if((itemnum - 1) > i)
		{
			strItem += ",";
		}
	}
	strQuery.Format(_T("CREATE TABLE %s(%s);"), tablename, strItem);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		strTmp = mysql_error(&m_pMySql[nNum]);

		if(strTmp == _T("Table 'automation_machine' already exists"))
		{
			return TRUE;
		}
		else
		{
			m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
			
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAlg_Mysql::Table_Delete(int nNum, CString tablename)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	strQuery = _T("DROP TABLE ") + tablename;
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}

	return TRUE;
}

BOOL CAlg_Mysql::Record_Add(int nNum, CString tablename, CString *itemdata, BOOL bFlag)
{
	// 레코드 추가 
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strValue;
	CString strItem;
	CString strTmp;
	CString strQuery;
	CString strType;
	CString strMsg;

	//char chQurey[8192];

	int i;
	int pos;
	int num;

	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	for(i=0; i<num; i++)
	{
		if (bFlag == FALSE)
		{
			Row = mysql_fetch_row(Res);

			if(itemdata[i] != _T(""))
			{
				strItem += Row[0];
				strTmp = Row[1];
				pos = strTmp.Find(_T("("), 0);
				if(pos < 0)
				{
					pos = strTmp.GetLength();
				}
				strType = strTmp.Mid(0, pos);
				strType.MakeUpper();

				strTmp = _T("");
				if(strType == _T("INT"))
				{
					strTmp += itemdata[i];
				}
				else if(strType == _T("FLOAT"))
				{
					strTmp += itemdata[i];
				}
				else if(strType == _T("DOUBLE"))
				{
					strTmp += itemdata[i];
				}
				else if(strType == _T("DATETIME"))
				{
					strTmp.Format(_T("'%s'"), itemdata[i]);
				}
				else if(strType == _T("DATE"))
				{
					strTmp.Format(_T("'%s'"), itemdata[i]);
				}
				else if(strType == _T("TIMESTAMP"))
				{

				}
				else if(strType == _T("VARCHAR"))
				{
					strTmp.Format(_T("'%s'"), itemdata[i]);
				}
				else if(strType == _T("CHAR"))
				{
					strTmp.Format(_T("'%s'"), itemdata[i]);
				}
				else
				{
					strTmp += itemdata[i];
				}

				strValue += strTmp;

				if((num - 1) > i)
				{
					strItem += _T(",");
					strValue += _T(",");
				}
			}
		}
		else if (bFlag == TRUE)
		{
			Row = mysql_fetch_row(Res);

			if (i>0)
			{
				if(itemdata[i-1] != _T(""))
				{
					strItem += Row[0];
					strTmp = Row[1];
					pos = strTmp.Find(_T("("), 0);
					if(pos < 0)
					{
						pos = strTmp.GetLength();
					}
					strType = strTmp.Mid(0, pos);
					strType.MakeUpper();

					strTmp = _T("");
					if(strType == _T("INT"))
					{
						strTmp += itemdata[i-1];
					}
					else if(strType == _T("FLOAT"))
					{
						strTmp += itemdata[i-1];
					}
					else if(strType == _T("DOUBLE"))
					{
						strTmp += itemdata[i-1];
					}
					else if(strType == _T("DATETIME"))
					{
						strTmp.Format(_T("'%s'"), itemdata[i-1]);
					}
					else if(strType == _T("DATE"))
					{
						strTmp.Format(_T("'%s'"), itemdata[i-1]);
					}
					else if(strType == _T("TIMESTAMP"))
					{

					}
					else if(strType == _T("VARCHAR"))
					{
						strTmp.Format(_T("'%s'"), itemdata[i-1]);
					}
					else if(strType == _T("CHAR"))
					{
						strTmp.Format(_T("'%s'"), itemdata[i-1]);
					}
					else
					{
						strTmp += itemdata[i-1];
					}

					strValue += strTmp;

					if((num - 1) > i)
					{
						strItem += _T(",");
						strValue += _T(",");
					}
				}
			}
		}
	}

	strQuery.Format(_T("INSERT INTO %s(%s) VALUES(%s)"), tablename, strItem, strValue);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	
	mysql_free_result(Res);
	return TRUE;
}

void CAlg_Mysql::MySql_Close(int nNum)
{
	if (m_strquery[nNum] != NULL)
	{
		delete [] m_strquery[nNum];
		m_strquery[nNum] = NULL;
	}

	mysql_close(&m_pMySql[nNum]);

	m_nMySqlFlag[nNum] = 0;
}

BOOL CAlg_Mysql::Database_Select(int nNum, CString databasename)
{
	// 데이터 베이스 선택하기 
	CString strMsg;

	//char chDb[1024];

	//OnStringToChar(databasename, chDb);
	if(mysql_select_db(&m_pMySql[nNum], databasename))
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CAlg_Mysql::Record_Updata(int nNum, CString tablename, CString search_item_1, CString search_data_1, CString search_item_2, CString search_data_2, CString change_item, CString data)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	if(search_item_2 == _T(""))
	{
		strQuery.Format(_T("UPDATE %s SET %s='%s' where %s='%s'"), tablename, change_item, data, search_item_1, search_data_1);
	}
	else
	{
		strQuery.Format(_T("UPDATE %s SET %s='%s' where %s='%s' and %s='%s'"), tablename, change_item, data, search_item_1, search_data_1, search_item_2, search_data_2);
	}

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAlg_Mysql::Record_Int_Delete(int nNum, CString tablename, CString itemname, CString range, int value[])
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	if(range == _T("<"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %d"), tablename, itemname, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %d"), tablename, itemname, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %d AND %s < %d"), tablename, itemname, value[0], itemname, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %d OR %s > %d"), tablename, itemname, value[0], itemname, value[1]);
	}
	else
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s = %d"), tablename, itemname, value[0]);
	}
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	return TRUE;
}

BOOL CAlg_Mysql::Record_Float_Delete(int nNum, CString tablename, CString itemname, CString range, float value[])
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	if(range == _T("<"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %f"), tablename, itemname, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %f"), tablename, itemname, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %f AND %s < %f"), tablename, itemname, value[0], itemname, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %f OR %s > %f"), tablename, itemname, value[0], itemname, value[1]);
	}
	else
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s = %f"), tablename, itemname, value[0]);
	}
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	return TRUE;
}

BOOL CAlg_Mysql::Record_Double_Delete(int nNum, CString tablename, CString itemname, CString range, double value[])
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	if(range == _T("<"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %f"), tablename, itemname, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %f"), tablename, itemname, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s > %f AND %s < %f"), tablename, itemname, value[0], itemname, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s < %f OR %s > %f"), tablename, itemname, value[0], itemname, value[1]);
	}
	else
	{
		strQuery.Format(_T("DELETE FROM %s WHERE %s = %f"), tablename, itemname, value[0]);
	}
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	return TRUE;
}

BOOL CAlg_Mysql::Record_Text_Delete(int nNum, CString tablename, CString itemname, CString value)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	strQuery.Format(_T("DELETE FROM %s WHERE %s = '%s'"), tablename, itemname, value);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	return TRUE;
}

BOOL CAlg_Mysql::Record_Date_Delete(int nNum, CString tablename, CString itemname, CString range, COleDateTime time[])
{
	CString strQuery;
	CString strTmp[2];
	CString strMsg;

	//char chQurey[8192];

	if(range == _T("<"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strQuery.Format(_T("DELETE FROM %s WHERE %s < '%s'"), tablename, itemname, strTmp[0]);
	}
	else if(range == _T(">"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strQuery.Format(_T("DELETE FROM %s WHERE %s > '%s'"), tablename, itemname, strTmp[0]);
	}
	else if(range == _T("<>"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());

		strTmp[1].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[1].GetYear(), 
														  time[1].GetMonth(),
														  time[1].GetDay(),
														  time[1].GetHour(),
														  time[1].GetMinute(),
														  time[1].GetSecond());
		strQuery.Format(_T("DELETE FROM %s WHERE %s > '%s' AND %s < '%s'"), tablename, itemname, strTmp[0], itemname, strTmp[1]);
	}
	else if(range == _T("><"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());

		strTmp[1].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[1].GetYear(), 
														  time[1].GetMonth(),
														  time[1].GetDay(),
														  time[1].GetHour(),
														  time[1].GetMinute(),
														  time[1].GetSecond());
		strQuery.Format(_T("DELETE FROM %s WHERE %s < '%s' OR %s > '%s'"), tablename, itemname, strTmp[0], itemname, strTmp[1]);
	}
	else
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strQuery.Format(_T("DELETE FROM %s WHERE %s = '%s'"), tablename, itemname, strTmp[0]);
	}

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	return TRUE;
}

long CAlg_Mysql::Record_Int_Search(int nNum, CString tablename, CString itemsearch, CString range, int value[])
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strQuery;
	CString stritem;
	CString strTmp[2], strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;

	stritem = _T("");

	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += ",";
		}
	}

	if(range == _T("<"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %d"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < %d"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %d AND %s < %d"), stritem, tablename, itemsearch, value[0], itemsearch, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < %d OR %s > %d"), stritem, tablename, itemsearch, value[0], itemsearch, value[1]);
	}
	else
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s = %d"), stritem, tablename, itemsearch, value[0]);
	}

	mysql_free_result(Res);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];

	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);

		if(Row == NULL)
		{
			mysql_free_result(Res);
		}

		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}

		m_strquery[nNum][i] = strData;
	}
	
	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;

	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Float_Search(int nNum, CString tablename, CString itemsearch, CString range, float value[])
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strQuery;
	CString stritem;
	CString strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;

	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += ",";
		}
	}
	
	if(range == _T("<"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %f"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < %f"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %f AND %s < %f"), stritem, tablename, itemsearch, value[0], itemsearch, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < %f OR %s > %f"), stritem, tablename, itemsearch, value[0], itemsearch, value[1]);
	}
	else
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s = %f"), stritem, tablename, itemsearch, value[0]);
	}

	mysql_free_result(Res);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];

	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);

		if(Row == NULL)
		{
			mysql_free_result(Res);
		}

		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}

		m_strquery[nNum][i] = strData;
	}

	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;

	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Double_Search(int nNum, CString tablename, CString itemsearch, CString range, double value[])
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strQuery;
	CString stritem;
	CString strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;

	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}

	//	SELECT name,age from guest where age <= 25 ;
	if(range == _T("<"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %f"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T(">"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < %f"), stritem, tablename, itemsearch, value[0]);
	}
	else if(range == _T("<>"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %f <= %s AND %s <= %f"), stritem, tablename, value[0], itemsearch, itemsearch, value[1]);
	}
	else if(range == _T("><"))
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > %f OR %s < %f"), stritem, tablename, itemsearch, value[0], itemsearch, value[1]);
	}
	else
	{
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s = %f"), stritem, tablename, itemsearch, value[0]);
	}

	mysql_free_result(Res);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];

	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);

		if(Row == NULL)
		{
			mysql_free_result(Res);
		}

		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}

		m_strquery[nNum][i] = strData;
	}

	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;

	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Double_Search(int nNum, CString tablename, CString itemsearch1, CString itemsearch2, double x[], double y[])
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;
	
	CString strQuery;
	CString stritem;
	CString strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;
	
	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}
	
	//	SELECT name,age from guest where age <= 25 ;
	
	strQuery.Format(_T("SELECT %s FROM %s WHERE %f <= %s AND %s <= %f AND %f <= %s AND %s <= %f"), stritem, tablename, x[0], itemsearch1, itemsearch1, x[1], y[0], itemsearch2, itemsearch2, y[1]);
	
	
	mysql_free_result(Res);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];
	
	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);
		
		if(Row == NULL)
		{
			mysql_free_result(Res);
		}
		
		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}
		
		m_strquery[nNum][i] = strData;
	}
	
	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;
	
	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Text_Search(int nNum, CString tablename, CString itemsearch, CString value)
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strQuery;
	CString stritem;
	CString strTmp[2], strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;

	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	//strQuery.Format(_T("SELECT *FROM %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}

	strQuery.Format(_T("SELECT *FROM %s WHERE %s = '%s'"), tablename, itemsearch, value);

	mysql_free_result(Res);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		mysql_free_result(m_pRes[nNum]);
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];

	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);

		if(Row == NULL)
		{
			mysql_free_result(Res);
		}

		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}

		m_strquery[nNum][i] = strData;
	}

	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;

	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Text_Search(int nNum, CString tablename, CString itemsearch1, CString itemsearch2, CString value1, CString value2)
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;
	
	CString strQuery;
	CString stritem;
	CString strTmp[2], strData;
	CString strMsg;

	//char chQurey[8192];
	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;
	
	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}
	
	strQuery.Format(_T("SELECT %s FROM %s WHERE %s = '%s' AND %s = '%s'"), stritem, tablename, itemsearch1, value1, itemsearch2, value2);
	
	mysql_free_result(Res);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		mysql_free_result(m_pRes[nNum]);
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];
	
	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);
		
		if(Row == NULL)
		{
			mysql_free_result(Res);
		}
		
		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}
		
		m_strquery[nNum][i] = strData;
	}
	
	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;
	
	return m_nfind_cnt[nNum];
}

long CAlg_Mysql::Record_Date_Search(int nNum, CString tablename, CString itemsearch, CString range, COleDateTime time[])
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	CString strQuery;
	CString stritem;
	CString strTmp[2], strData;
	CString strMsg;

	//char chQurey[8192];

	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;
	
	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), tablename);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}

	if(range == _T("<"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());

		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > '%s'"), stritem, tablename, itemsearch, strTmp[0]);
	}
	else if(range == _T(">"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());

		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < '%s'"), stritem, tablename, itemsearch, strTmp[0]);
	}
	else if(range == _T("<>"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strTmp[1].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[1].GetYear(), 
														  time[1].GetMonth(),
														  time[1].GetDay(),
														  time[1].GetHour(),
													      time[1].GetMinute(),
														  time[1].GetSecond());
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s > '%s' AND %s < '%s'"), stritem, tablename, itemsearch, strTmp[0], itemsearch, strTmp[1]);
	}
	else if(range == _T("><"))
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strTmp[1].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[1].GetYear(), 
														  time[1].GetMonth(),
														  time[1].GetDay(),
														  time[1].GetHour(),
														  time[1].GetMinute(),
														  time[1].GetSecond());
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s < '%s' OR %s > '%s'"), stritem, tablename, itemsearch, strTmp[0], itemsearch, strTmp[1]);
	}
	else
	{
		strTmp[0].Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), time[0].GetYear(), 
														  time[0].GetMonth(),
														  time[0].GetDay(),
														  time[0].GetHour(),
														  time[0].GetMinute(),
														  time[0].GetSecond());
		strQuery.Format(_T("SELECT %s FROM %s WHERE %s = '%s'"), stritem, tablename, itemsearch, strTmp[0]);
	}

	mysql_free_result(Res);

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		mysql_free_result(m_pRes[nNum]);
		return m_nfind_cnt[nNum];
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];

	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);

		if(Row == NULL)
		{
			mysql_free_result(Res);
		}

		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}

		m_strquery[nNum][i] = strData;
	}

	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;

	return m_nfind_cnt[nNum];
}

BOOL CAlg_Mysql::Table_Select(int nNum, CString databasename, CString tablename)
{
	CString strQuery;
	int num, i;
	CString strTmp;
	CString strMsg;

	//char chQurey[8192];

	BOOL find;

	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;

	//OnStringToChar(_T("SHOW TABLES"), chQurey);
	if(mysql_query(&m_pMySql[nNum], _T("SHOW TABLES")))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	find = FALSE;
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		strTmp = Row[0];

		strTmp.MakeUpper();
		tablename.MakeUpper();
		
		if(strTmp == tablename)
		{
			find = TRUE;
			break;
		}
	}

	mysql_free_result(Res);

	if(find)
	{
		//OnStringToChar(tablename, chQurey);
		if(mysql_select_db(&m_pMySql[nNum], tablename))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
	return FALSE;
}

CString CAlg_Mysql::Record_Read(int nNum)
{
	CString strTmp;

	strTmp = _T("");
	m_nread_cnt[nNum]++;

	strTmp = m_strquery[nNum][m_nread_cnt[nNum]-1];
	if(m_nread_cnt[nNum] == m_nfind_cnt[nNum])
	{
		if (m_strquery[nNum] != NULL)
		{
			delete [] m_strquery[nNum];
			m_strquery[nNum] = NULL;
		}
	}
	return strTmp;
}

BOOL CAlg_Mysql::WaitTime(int nNum, int mode, int time)
{
	CString strQuery;

	//char chQurey[8192];

	if(mode == 0)	
	{	
		strQuery.Format(_T("set global max_connections=%d"), time);
	}
	else	
	{	
		strQuery.Format(_T("set wait_timeout=%d"), time);	
	}
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		return FALSE;	
	}
	
	return TRUE;    
}

BOOL CAlg_Mysql::Record_Sort(int nNum, int mode, CString tablename, CString itemsearch)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	if(mode == 0)
	{
		strQuery.Format(_T("SELECT * FROM %s ORDER BY %s ASC"), tablename, itemsearch);
	}
	else
	{
		strQuery.Format(_T("SELECT * FROM %s ORDER BY %s DESC"), tablename, itemsearch);
	}

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		return FALSE;	
	}
	
	return TRUE;
}

/*
SELECT * INTO OUTFILE 'YOUR_FILE.CSV'
 FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '"'
 LINES TERMINATED BY '\n'
 FROM YOUR_TABLE;
*/
BOOL CAlg_Mysql::WriteFile(int nNum, CString strFileName)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];
	//DATA
	strQuery.Format(_T("SELECT * INTO OUTFILE '%s' FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '%c' LINES TERMINATED BY '\r\n' FROM DATA"), strFileName, _T('"'));	

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		return FALSE;
	}

	return TRUE;
}

/*
Load Data Local InFile 'FILE_PATH' Into Table androids FIELDS TERMINATED BY ',' ENCLOSED BY '"' LINES TERMINATED BY '\n'
*/

BOOL CAlg_Mysql::ReadFile(int nNum, CString strFileName)
{
	CString strQuery;
	CString strMsg;

	//char chQurey[8192];

	//DATA
	strQuery.Format(_T("LOAD DATA LOCAL INFILE '%s' INTO TABLE DATA FIELDS TERMINATED BY ',' ENCLOSED BY '%c' LINES TERMINATED BY '\n'"), strFileName, _T('"'));	
//	strQuery.Format("SELECT * INTO INFILE '%s' FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '%c' LINES TERMINATED BY '\r\n' FROM DATA", strFileName, '"');	

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return FALSE;
	}

	return TRUE;
}

// SELECT MAX(article) AS article FROM shop
float CAlg_Mysql::Record_Max(int nNum, CString item)
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;
	int num;

	CString strQuery;
	CString stritem;
	CString strMsg;

	//char chQurey[8192];

	int i;

	float dVal = 0.0;

	//DATA
	strQuery.Format(_T("SELECT MAX(%s) AS %s FROM DATA"), item, item);	

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return dVal;
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}

	dVal = (float)atof(stritem);

	return dVal;
}

float CAlg_Mysql::Record_Min(int nNum, CString item)
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;
	int num;

	CString strQuery;
	CString stritem;
	CString strMsg;

	//char chQurey[8192];

	int i;

	float dVal = 0.0;

	//DATA
	strQuery.Format(_T("SELECT MIN(%s) AS %s FROM DATA"), item, item);	

	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))		
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return dVal;
	}

	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);

	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}

	dVal = (float)atof(stritem);

	return dVal;
}

void CAlg_Mysql::WriteWork(CString *strData)
{
// 	int nNum;
// 
// 	CTime			time;
// 	COleDateTime	tDB[2];
// 
// 	time	= CTime::GetCurrentTime();				
// 
// 	tDB[0].SetDateTime(time.GetYear()-1, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
// 	tDB[1].SetDateTime(0, 0, 0, 0, 0, 0);
// 
// 	nNum = MySql_Open();
// 	if (nNum < 0) return;
// 
// 	if(Mysql.Database_Select(nNum, _T("AMT")))
// 	{
// 		if(Table_Select(nNum, _T("AMT"), _T("PRODUCT_DATA")))
// 		{
// 			Record_Date_Delete(nNum, _T("PRODUCT_DATA"), _T("DATE"), _T("<"), tDB);
// 			Record_Add(nNum, _T("PRODUCT_DATA"), strData);
// 		}
// 		else
// 		{
// 			// table이 없으면 새로 table 추가
// 			if(Table_Create(nNum, _T("AMT"), _T("PRODUCT_DATA"), m_nWorkTableCnt, m_strWorkTableName, m_nWorkTableType, m_nWorkTableNum))
// 			{
// 				Record_Date_Delete(nNum, _T("PRODUCT_DATA"), _T("DATE"), _T("<"), tDB);
// 				Record_Add(nNum, _T("PRODUCT_DATA"), strData);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// amt database가 없으면 database 추가
// 		if(Database_Create(nNum, _T("AMT")))
// 		{
// 			// table이 없으면 새로 table 추가
// 			if(Table_Create(nNum, _T("AMT"), _T("PRODUCT_DATA"), m_nWorkTableCnt, m_strWorkTableName, m_nWorkTableType, m_nWorkTableNum))
// 			{
// 				Record_Date_Delete(nNum, _T("PRODUCT_DATA"), _T("DATE"), _T("<"), tDB);
// 				Record_Add(nNum, _T("PRODUCT_DATA"), strData);
// 			}
// 		}
// 	}
// 
// 	MySql_Close(nNum);
}

void CAlg_Mysql::WriteCycle(CString *strData)
{
// 	int nNum;
// 
// 	CTime			time;
// 	COleDateTime	tDB[2];
// 
// 	time	= CTime::GetCurrentTime();				
// 
// 	tDB[0].SetDateTime(time.GetYear()-1, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
// 	tDB[1].SetDateTime(0, 0, 0, 0, 0, 0);
// 
// 	nNum = MySql_Open();
// 	if (nNum < 0) return;
// 
// 	if(Mysql.Database_Select(nNum, _T("AMT")))
// 	{
// 		if(Table_Select(nNum, _T("AMT"), _T("CYCLE_DATA")))
// 		{
// 			Record_Date_Delete(nNum, _T("CYCLE_DATA"), _T("DATE"), _T("<"), tDB);
// 			Record_Add(nNum, _T("CYCLE_DATA"), strData);
// 		}
// 		else
// 		{
// 			// table이 없으면 새로 table 추가
// 			if(Table_Create(nNum, _T("AMT"), _T("CYCLE_DATA"), m_nCycleTableCnt, m_strCycleTableName, m_nCycleTableType, m_nCycleTableNum))
// 			{
// 				Record_Date_Delete(nNum, _T("CYCLE_DATA"), _T("DATE"), _T("<"), tDB);
// 				Record_Add(nNum, _T("CYCLE_DATA"), strData);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// amt database가 없으면 database 추가
// 		if(Database_Create(nNum, _T("AMT")))
// 		{
// 			// table이 없으면 새로 table 추가
// 			if(Table_Create(nNum, _T("AMT"), _T("CYCLE_DATA"), m_nCycleTableCnt, m_strCycleTableName, m_nCycleTableType, m_nCycleTableNum))
// 			{
// 				Record_Date_Delete(nNum, _T("CYCLE_DATA"), _T("DATE"), _T("<"), tDB);
// 				Record_Add(nNum, _T("CYCLE_DATA"), strData);
// 			}
// 		}
// 	}
// 
// 	MySql_Close(nNum);
}

long CAlg_Mysql::Record_Slot_Search(int nNum, CString strTableName, int nSlot, int nHifix)
{
	MYSQL_RES *Res;
	MYSQL_ROW Row;
	MYSQL_FIELD *Fields;
	
	CString strQuery;
	CString stritem;
	CString strTmp[2], strData;
	CString strMsg;

	//char chQurey[8192];
	int i, j;
	int num;
	m_nfind_cnt[nNum] = 0;
	
	stritem = _T("");
	strQuery.Format(_T("DESCRIBE %s"), strTableName);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	num = (int)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	for(i=0; i<num; i++)
	{
		Row = mysql_fetch_row(Res);
		
		stritem += Row[0];
		if((num-1) != i)
		{
			stritem += _T(",");
		}
	}
	
	strQuery.Format(_T("SELECT %s FROM %s WHERE %s = '%d' AND %s = '%d'"), stritem, strTableName, _T("SLOT"), nSlot, _T("HIFIX"), nHifix);
	
	mysql_free_result(Res);
	
	//OnStringToChar(strQuery, chQurey);
	if(mysql_query(&m_pMySql[nNum], strQuery))
	{
		m_strErrorMsg	= mysql_error(&m_pMySql[nNum]);
		
		mysql_free_result(m_pRes[nNum]);
		return m_nfind_cnt[nNum];
	}
	
	Res = mysql_store_result(&m_pMySql[nNum]);
	m_nfind_cnt[nNum] = (long)mysql_num_rows(Res);
	Fields = mysql_fetch_fields(Res);
	
	m_strquery[nNum] = new CString[m_nfind_cnt[nNum]];
	
	for(i=0; i<m_nfind_cnt[nNum]; i++)
	{
		num = mysql_num_fields(Res);
		Row = mysql_fetch_row(Res);
		
		if(Row == NULL)
		{
			mysql_free_result(Res);
		}
		
		strData = _T("");
		for(j=0; j<num; j++)
		{
			strData += Row[j];
			strData += _T(",");
		}
		
		m_strquery[nNum][i] = strData;
	}
	
	mysql_free_result(Res);
	m_nread_cnt[nNum] = 0;
	
	return m_nfind_cnt[nNum];
}