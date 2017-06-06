// AVersion.h: interface for the AVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVERSION_H__AA379D4C_FA4A_4E18_86C5_447EAA9E329B__INCLUDED_)
#define AFX_AVERSION_H__AA379D4C_FA4A_4E18_86C5_447EAA9E329B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define REG_KEY_VER	"Software\\AMT\\AMT8520\\Version\\"
#define REG_VALUE_CREATE_VER	"Ver"
#define REG_VALUE_CREATE_DATE	"Date"

class AVersion  
{
public:
	AVersion();
	virtual ~AVersion();

	void CheckVersion();
	void UpdateVersion();

	void SetVersion();

	CString GetStrVersion();
	CString GetVersion() { return m_strVersion; }
	CString GetDate() { return m_strDate; }
protected:
	CString m_strUpdate;

	CString m_strHandlerName;
	CString m_strVersion;
	CString m_strDate;
};

extern AVersion g_ver;

#endif // !defined(AFX_AVERSION_H__AA379D4C_FA4A_4E18_86C5_447EAA9E329B__INCLUDED_)
