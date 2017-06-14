#if !defined(AFX_MGRSOCKET_H__2DDDCFD1_0099_4F04_A5AE_6560BAB820FB__INCLUDED_)
#define AFX_MGRSOCKET_H__2DDDCFD1_0099_4F04_A5AE_6560BAB820FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MgrSocket.h : header file
//

enum { DB_READE, DB_WRITE };

struct stSocketInfo
{
	bool bDBAccess;

	char mchr_Hifix[6];
	char mchr_Site[6];
	char mchr_Serial[50];
	char mchr_Count[6];
	char mchr_mdlName[6];//Model name 2.5 msate etc.

	stSocketInfo()
	{
		bDBAccess = DB_READE;
		memset( mchr_Hifix, sizeof(mchr_Hifix), 0x00);
		memset( mchr_Site, sizeof(mchr_Site), 0x00);
		memset( mchr_Serial, sizeof(mchr_Serial), 0x00);
		memset( mchr_Count, sizeof(mchr_Count), 0x00);
		memset( mchr_mdlName, sizeof(mchr_mdlName), 0x00);
	}

	void SocketInfoWrite( bool _bDBAccess, char* _mchr_Hifix, char* _mchr_Site, char* _mchr_Serial, char* _mchr_Count, char* _mchr_mdlName)
	{
		bDBAccess = _bDBAccess;
		memcpy(mchr_Hifix, _mchr_Hifix, sizeof(mchr_Hifix));
		memcpy(mchr_Site, _mchr_Site, sizeof(mchr_Site));
		memcpy(mchr_Serial, _mchr_Serial, sizeof(mchr_Serial));
		memcpy(mchr_Count, _mchr_Count, sizeof(mchr_Count));
		memcpy(mchr_mdlName, _mchr_mdlName, sizeof(mchr_mdlName));
	}
};


/////////////////////////////////////////////////////////////////////////////
// CMgrSocket command target

class CMgrSocket : public CObject
{
	DECLARE_SERIAL(CMgrSocket)
public:
	CMgrSocket();           // protected constructor used by dynamic creation
	virtual ~CMgrSocket();

// Attributes
protected:
	CString mstr_Hifix;
	CString mstr_Site;
	CString mstr_Serial;
	CString mstr_Count;
	CString mstr_mdlName;//Model name 2.5 msate etc.

	//Socket was failed or Socket couter overed count
	//remain rest  //delete??

	CString mstr_remain_Hifix;
	CString mstr_remain_Serial;
	CString mstr_remain_Count;
	CString mstr_remain_name;

	//ADD Apply
	CString mstr_add_Hifix;
	CString mstr_add_Site;
	CString mstr_add_Serial;
	CString mstr_add_Count;
	CString mstr_add_name;

public:
// 	int GetModuleHix()
// 	{
// 		return 
// 	}
	stSocketInfo& GetSocketInfo( bool _bDBAccess, char* _mchr_hifix, char* _mchr_site, char* _mchr_serial, char* _mchr_count, char* _mchr_mdlname )
	{
		stSocketInfo socket;
		socket.bDBAccess = _bDBAccess;
		memcpy(socket.mchr_Hifix, _mchr_hifix, sizeof(socket.mchr_Hifix));
		memcpy(socket.mchr_Site, _mchr_site, sizeof(socket.mchr_Site));
		memcpy( socket.mchr_Serial, _mchr_serial, sizeof(socket.mchr_Serial));
		memcpy( socket.mchr_Count, _mchr_count, sizeof(socket.mchr_Count));
		memcpy( socket.mchr_mdlName, _mchr_mdlname, sizeof(socket.mchr_mdlName));
	}

// Operations
public:


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGRSOCKET_H__2DDDCFD1_0099_4F04_A5AE_6560BAB820FB__INCLUDED_)
