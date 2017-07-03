// APartShortkey.cpp: implementation of the APartShortkey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "handler.h"
#include "APartShortkey.h"
#include "..\SrcBase\ALocalization.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APartShortkey	g_ShortKey;

APartShortkey::APartShortkey()
{
	for( int i=0; i<256; i++ )
	{
		m_bControl[i] = false;
	}
}

APartShortkey::~APartShortkey()
{
}

void APartShortkey::CheckKeyboard( MSG* pMsg )
{
	// Alt + ??
	if( pMsg->message == WM_SYSKEYDOWN )
	{
		switch( pMsg->wParam )
		{

		case 'K':	g_local.SetLocal( LOCAL_KOR );	break;
		case 'E':	g_local.SetLocal( LOCAL_ENG );	break;
		case 'C':	g_local.SetLocal( LOCAL_CHN );	break;

		case 'L':	g_local.SetNextLocal();			break;

		}
		return;
	}


	if( pMsg->message == WM_KEYDOWN )
	{
		m_bControl[pMsg->wParam] = true;
	}
	else if( pMsg->message == WM_KEYUP )
	{
		m_bControl[pMsg->wParam] = false;
		switch( pMsg->wParam )
		{
		case 'T':
			{
				if( st_handler.cwnd_main )
					st_handler.cwnd_main->PostMessage( WM_CHANGE_VIEW_TYPE, ECVT_TESTSITE, 0 );
			}
			break;
			
		case 'Y':
			{
				if( st_handler.cwnd_main )
					st_handler.cwnd_main->PostMessage( WM_CHANGE_VIEW_TYPE, ECVT_MODULE, 0 );
			}
			break;	
		}
	}

}
