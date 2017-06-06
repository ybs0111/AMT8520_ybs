// DialogKeyPad.cpp : implementation file
//

#include "stdafx.h"
#include "KeyBoard.h"
#include "DialogKeyPad.h"
#include "KeyVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogKeyPad dialog


CDialogKeyPad::CDialogKeyPad(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogKeyPad::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogKeyPad)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogKeyPad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogKeyPad)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_MSG_MIN,			m_msgMin);
	DDX_Control(pDX, IDC_MSG_MIN_DATA,		m_msgMinData);
	DDX_Control(pDX, IDC_MSG_MAX,			m_msgMax);
	DDX_Control(pDX, IDC_MSG_MAX_DATA,		m_msgMaxData);
	DDX_Control(pDX, IDC_MSG_CURRENT,		m_msgCurrent);
	DDX_Control(pDX, IDC_MSG_CURRENT_DATA,	m_msgCurrentData);
	DDX_Control(pDX, IDC_MSG_NEW,			m_msgNew);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogKeyPad, CDialog)
	//{{AFX_MSG_MAP(CDialogKeyPad)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogKeyPad message handlers

BOOL CDialogKeyPad::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(200);

	return CDialog::DestroyWindow();
}

BOOL CDialogKeyPad::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CFont* pFont;

	KeyInit();

	pFont = new CFont;
	pFont->CreateFont(28, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGUL_CHARSET, 3, 2, 1,VARIABLE_PITCH | FF_ROMAN, _T("Courier"));
	GetDlgItem(IDC_EDIT_NEW)->SetFont(pFont, TRUE);	
	pFont->Detach();	
	pFont->DeleteObject();
	
	delete pFont;
	pFont = NULL;
/*
	m_bmOk.LoadBitmap(IDB_BITMAP_OK);
	m_btnOk.SetBitmap(m_bmOk);

	m_bmCancel.LoadBitmap(IDB_BITMAP_CANCEL);
	m_btnCancel.SetBitmap(m_bmCancel);

	m_bmClear.LoadBitmap(IDB_BITMAP_CLEAR);
	m_btnClear.SetBitmap(m_bmClear);
*/
	LabelInit();

	SetTimer(100, 200, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogKeyPad::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect client;
	CRect rc;
	
	GetClientRect(client);
	int x = 3;
	int y = -m_nKeyHeight + 5;
	
	CClientDC dc(this);
	
	tagKeyPad * key;
	
	POSITION pos = m_pKeys.GetHeadPosition();
	
	while(pos)
	{
		key = m_pKeys.GetNext(pos);
		
		if((key->cNormal == 0x00) && (key->cShifted == 0x00))
		{
			x = 3;
			y += m_nKeyHeight;
		}
		else
		{
			int width = ((abs(key->cWidth) * m_nKeyWidth) / 10);
			CRect rc(x,y,x + width, y + m_nKeyHeight);
			if(key->cWidth < 0)
			{
				rc.right = client.right;
			}
			x += width;
			if(rc.PtInRect(point))
			{
				KeySend(&dc, rc, TRUE, key);
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CDialogKeyPad::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect client;
	
	CRect rectArea;
	
	int width;
	
	GetClientRect(client);
	
	int x = 3;
	int y = -m_nKeyHeight + 5;
	
	CClientDC dc(this);
	
	tagKeyPad * key;
	
	POSITION pos = m_pKeys.GetHeadPosition();
	
	while(pos)
	{
		key = m_pKeys.GetNext(pos);
		
		if((key->cNormal == 0x00) && (key->cShifted == 0x00))
		{
			x = 3;
			y += m_nKeyHeight;
		}
		else
		{
			width = ((abs(key->cWidth) * m_nKeyWidth) / 10);
			rectArea.SetRect(x, y, x + width, y + m_nKeyHeight);
			if(key->cWidth < 0)
			{
				rectArea.right = client.right;
			}
			x += width;
			if(rectArea.PtInRect(point))
			{
				KeyDraw(&dc, rectArea, key, FALSE);
			}
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}


void CDialogKeyPad::LabelInit()
{
	CFont *pFont;

	pFont = new CFont;
	pFont->CreateFont(18, 0, 0, 0, FW_BOLD, 0, 0, 0, HANGUL_CHARSET, 3, 2, 1,VARIABLE_PITCH | FF_ROMAN, _T("Courier"));

	m_msgMin.SetFont(pFont);
	m_msgMin.SetWindowText(_T("Min"));
	m_msgMin.SetCenterText();
	m_msgMin.SetColor(WHITE_C);
	m_msgMin.SetGradientColor(RED_L);
	m_msgMin.SetTextColor(BLACK_C);	

	m_msgMinData.SetFont(pFont);
	m_msgMinData.SetWindowText(m_strMin);
	m_msgMinData.SetCenterText();
	m_msgMinData.SetColor(WHITE_C);
	m_msgMinData.SetGradientColor(YELLOW_L);
	m_msgMinData.SetTextColor(BLACK_C);	

	m_msgMax.SetFont(pFont);
	m_msgMax.SetWindowText(_T("Min"));
	m_msgMax.SetCenterText();
	m_msgMax.SetColor(WHITE_C);
	m_msgMax.SetGradientColor(RED_L);
	m_msgMax.SetTextColor(BLACK_C);	

	m_msgMaxData.SetFont(pFont);
	m_msgMaxData.SetWindowText(m_strMax);
	m_msgMaxData.SetCenterText();
	m_msgMaxData.SetColor(WHITE_C);
	m_msgMaxData.SetGradientColor(YELLOW_L);
	m_msgMaxData.SetTextColor(BLACK_C);	

	m_msgCurrent.SetFont(pFont);
	m_msgCurrent.SetWindowText(_T("Current"));
	m_msgCurrent.SetCenterText();
	m_msgCurrent.SetColor(WHITE_C);
	m_msgCurrent.SetGradientColor(GREEN_L);
	m_msgCurrent.SetTextColor(BLACK_C);	

	m_msgCurrentData.SetFont(pFont);
	m_msgCurrentData.SetWindowText(m_strCurrent);
	m_msgCurrentData.SetCenterText();
	m_msgCurrentData.SetColor(WHITE_C);
	m_msgCurrentData.SetGradientColor(YELLOW_L);
	m_msgCurrentData.SetTextColor(BLACK_C);	

	m_msgNew.SetFont(pFont);
	m_msgNew.SetWindowText(_T("New"));
	m_msgNew.SetCenterText();
	m_msgNew.SetColor(WHITE_C);
	m_msgNew.SetGradientColor(GREEN_L);
	m_msgNew.SetTextColor(BLACK_C);	

	delete pFont;
	pFont = NULL;
}


void CDialogKeyPad::KeyInit()
{
	KeyAdd(0x00,0x00,0); //signal row change
	//KeyAdd('`','~',10);
	KeyAdd('7','7',10);
	KeyAdd('8','8',10);
	KeyAdd('9','9',10);
	//KeyAdd('+','+',10);

	KeyAdd(0x00,0x00,1); //signal row change
	//KeyAdd('`','~',10);
	KeyAdd('4','4',10);
	KeyAdd('5','5',10);
	KeyAdd('6','6',10);

	KeyAdd(0x00,0x00,2); //signal row change
	//KeyAdd('`','~',10);
	KeyAdd('1','1',10);
	KeyAdd('2','2',10);
	KeyAdd('3','3',10);

	KeyAdd(0x00,0x00,3); //signal row change
	//KeyAdd('`','~',10);
	KeyAdd('-','-',10);
	KeyAdd('0','0',10);
	KeyAdd('.','.',10);

/*	KeyAdd(0x00,0x00,0); //signal row change
	//KeyAdd(0x00,ESC,10);
	for(int fnkey = 0x70; fnkey < 0x7C; fnkey++)
	{
		KeyAdd(0x00,fnkey & 0xFF,10);
	}
	KeyAdd(0x00,INS,14);
	KeyAdd(0x00,DEL,-10);

	//first row
	KeyAdd(0x00,0x00,1); //signal row change
	//KeyAdd('`','~',10);
	KeyAdd('1','!',10);
	KeyAdd('2','@',10);
	KeyAdd('3','#',10);
	KeyAdd('4','$',10);
	KeyAdd('5','%',10);
	KeyAdd('6','^',10);
	KeyAdd('7','&',10);
	KeyAdd('8','*',10);
	KeyAdd('9','(',10);
	KeyAdd('0',')',10);
	KeyAdd('-','_',10);
	KeyAdd('=','+',10);
	KeyAdd(0x00,BSPACE,14); //backspace
	KeyAdd(0x00,PUP,-12);

	//second row
	KeyAdd(0x00,0x00,2); //signal row change
	KeyAdd(0x00,TAB,14); //tab
	KeyAdd('q','Q',10);
	KeyAdd('w','W',10);
	KeyAdd('e','E',10);
	KeyAdd('r','R',10);
	KeyAdd('t','T',10);
	KeyAdd('y','Y',10);
	KeyAdd('u','U',10);
	KeyAdd('i','I',10);
	KeyAdd('o','O',10);
	KeyAdd('p','P',10);
	KeyAdd('[','{',10);
	KeyAdd(']','}',10);
	KeyAdd('\\','|',10);
	KeyAdd(0x00,PDN,-12);

	//third row
	KeyAdd(0x00,0x00,3); //signal row change
	KeyAdd(0x00,CAPSLOCK,18); //caps lock
	KeyAdd('a','A',10);
	KeyAdd('s','S',10);
	KeyAdd('d','D',10);
	KeyAdd('f','F',10);
	KeyAdd('g','G',10);
	KeyAdd('h','H',10);
	KeyAdd('j','J',10);
	KeyAdd('k','K',10);
	KeyAdd('l','L',10);
	KeyAdd(';',':',10);
	KeyAdd('\'','\"',10);
	KeyAdd(0x00,ENTER,16); //enter
	KeyAdd(0x00,HOME,-10);

	//fourth row
	KeyAdd(0x00,0x00,4); //signal row change
	KeyAdd(0x00,SHIFT,22); //shift
	KeyAdd('z','Z',10);
	KeyAdd('x','X',10);
	KeyAdd('c','C',10);
	KeyAdd('v','V',10);
	KeyAdd('b','B',10);
	KeyAdd('n','N',10);
	KeyAdd('m','M',10);
	KeyAdd(',','<',10);
	KeyAdd('.','>',10);
	KeyAdd('/','?',10);
	KeyAdd(0x00,SHIFT,22); //shift
	KeyAdd(0x00,END,-10);

	//fifth row
	KeyAdd(0x00,0x00,5); //signal row change
	KeyAdd(0x00,CTRL,12); //ctrl
	KeyAdd(0x00,WINDOW,12); //window
	KeyAdd(0x00,ALT,12); //alt
	KeyAdd(' ',' ',60);
//	KeyAdd(0x00,LEFT,12); //left
//	KeyAdd(0x00,UP,12); //up
//	KeyAdd(0x00,DOWN,12); //down
//	KeyAdd(0x00,RIGHT,12); //right
//	KeyAdd(0x00,WWW,-10); //http://WWW
*/
	KeyArea();
}

void CDialogKeyPad::KeyAdd(char pNormal,char pShifted,int pWidth)
{
	tagKeyPad * key = new tagKeyPad;

	key->cNormal = pNormal;
	key->cShifted = pShifted;
	key->cWidth = pWidth;

	m_pKeys.AddTail(key);
}

void CDialogKeyPad::KeyArea()
{
	m_nKeyWidth = 0;
	m_nKeyHeight = 0;

	tagKeyPad * key;

	POSITION pos = m_pKeys.GetHeadPosition();

	long totalWidth = 0;
	long longest	= 0;
	long rows		= 0;

	CRect rect;

	while(pos)
	{
		key = m_pKeys.GetNext(pos);
		//if((key->cNormal == 0x00) && (key->cShifted == 0x00))
		if((key->cNormal == 0x00))
		{
			rows++;
			if(totalWidth > longest)
			{
				longest = totalWidth;
			}
			totalWidth = 0;
		}
		else
		{
			totalWidth += abs(key->cWidth);
		}
	}

	GetDlgItem(IDC_MSG_KEYPAD)->GetWindowRect(rect);
	ScreenToClient(rect);

	m_nKeyHeight	= (rect.Height()) / rows;
	m_nKeyWidth		= (int)(((double)(rect.Width())) / ((((double)(longest)) / 10)));
}

void CDialogKeyPad::KeyDraw()
{
	CRect rectArea[4];
	CRect rc;

	CRgn clipRgn;

	int x = 10;
	int y = -m_nKeyHeight + 5;
	int state;	
	int width;
	
	BOOL hilight;

	tagKeyPad * key;

	CClientDC dc(this);

	GetClientRect(rectArea[0]);
	clipRgn.CreateRectRgnIndirect(&rectArea[0]);

	dc.SelectClipRgn(&clipRgn);
	dc.SelectObject(GetStockObject(DEFAULT_GUI_FONT));
	dc.SetBkMode(TRANSPARENT);

	POSITION pos = m_pKeys.GetHeadPosition();

	m_nKeyState = KeyState();
	state		= m_nKeyState;

	while(pos)
	{
		key = m_pKeys.GetNext(pos);
		if((key->cNormal == 0x00) && (key->cShifted == 0x00))
		{
			x = 3;
			y += m_nKeyHeight;
		}
		else
		{
			width = ((abs(key->cWidth) * m_nKeyWidth) / 10);
			rectArea[1].SetRect(x, y, x + width, y + m_nKeyHeight);

			x += width;

			if(key->cWidth < 0)
			{
				rectArea[1].right = rectArea[0].right; 
			}
			hilight = FALSE;

			if(key->cNormal == 0x00)
			{
				switch(key->cShifted)
				{
					case CAPSLOCK:
						if(state & 0x01)
						{
							hilight = TRUE;
						}
						break;
					case SHIFT:
						if(state & 0x02)
						{
							hilight = TRUE;
						}
						break;
					case CTRL:
						if(state & 0x04)
						{
							hilight = TRUE;
						}
						break;

					default:
	
						break;
				}
			}
			KeyDraw(&dc, rectArea[1], key, hilight);
		}
	}
}

void CDialogKeyPad::KeyDraw(CDC * dc, CRect & rc, tagKeyPad * key, BOOL cHilight)
{
	CString strTemp;

	CSize tsize;

	int cx;
	int cy;
	int siz;

	if(!cHilight)
	{
		rc.DeflateRect(1,1);
		dc->FillSolidRect(rc, BLACK_C);
		dc->Draw3dRect(rc,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DDKSHADOW));
		rc.DeflateRect(1,1);
		dc->Draw3dRect(rc,GetSysColor(COLOR_3DLIGHT),GetSysColor(COLOR_3DSHADOW));

		dc->SetTextColor(RGB(255, 255, 255));
		dc->SetBkColor(BLACK_C);
	}
	else
	{
		rc.DeflateRect(1,1);
		dc->FillSolidRect(rc, BLACK_L);
		dc->Draw3dRect(rc,GetSysColor(COLOR_3DLIGHT),GetSysColor(COLOR_3DSHADOW));
		rc.DeflateRect(1,1);
		dc->Draw3dRect(rc,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DDKSHADOW));

		dc->SetBkColor(BLACK_L);
		dc->SetTextColor(RGB(255, 255, 255));
	}

	rc.DeflateRect(3,1);
	if(key->cNormal == 0x00)
	{
		CString label = _T(" ");
		BOOL special = FALSE;
		if(key->cShifted > 0x6F) //is a function
		{
			int fkeynum = key->cShifted - 0x6F;
			label.Format(_T("F%d"),fkeynum);
		}
		else
		{
			switch(key->cShifted)
			{
			case TAB:    // 0x02
				label = _T("Tab->");
				break;
			case CAPSLOCK:    // 0x03
				label = _T("Caps");
				break;
			case SHIFT:    // 0x04
				label = _T("Shift");
				break;
			case CTRL:    // 0x05
				label = _T("Ctrl");
				break;
			case BSPACE:    // 0x06
				label = _T("<-Bspc");
				break;
			case ENTER:    // 0x07
				label = _T("Enter");
				break;
			case ALT:    // 0x08
				label = _T("Alt");
				break;
			case WINDOW:    // 0x9
				label = _T("Wnd");
				break;
			case ESC:
				label = _T("Esc");
				break;
			case LEFT:    // 0x0A
				special = TRUE;
				{
					cx = (rc.left + rc.right)/2;
					cy = (rc.top + rc.bottom)/2;
					siz = rc.Width()/3;

					dc->MoveTo(cx+siz, cy);
					dc->LineTo(cx-siz, cy);
					dc->LineTo(cx-siz + (siz*4)/5, cy - siz/2);
					dc->MoveTo(cx-siz, cy);
					dc->LineTo(cx-siz + (siz*4)/5, cy + siz/2);
				}
				break;
			case RIGHT:    // 0x0B
				special = TRUE;
				{
					cx = (rc.left + rc.right)/2;
					cy = (rc.top + rc.bottom)/2;
					siz = rc.Width()/3;

					dc->MoveTo(cx-siz, cy);
					dc->LineTo(cx+siz, cy);
					dc->LineTo(cx+siz - (siz*4)/5, cy - siz/2);
					dc->MoveTo(cx+siz, cy);
					dc->LineTo(cx+siz - (siz*4)/5, cy + siz/2);
				}
				break;
			case UP:    // 0x0C
				special = TRUE;
				{
					cx = (rc.left + rc.right)/2;
					cy = (rc.top + rc.bottom)/2;
					siz = rc.Width()/3;

					dc->MoveTo(cx, cy+siz);
					dc->LineTo(cx, cy-siz);
					dc->LineTo(cx - siz/2, cy-siz + (siz*4)/5);
					dc->LineTo(cx, cy-siz);
					dc->LineTo(cx + siz/2, cy-siz + (siz*4)/5);
				}
				break;
			case DOWN:    // 0x0D
				special = TRUE;
				{
					cx = (rc.left + rc.right)/2;
					cy = (rc.top + rc.bottom)/2;
					siz = rc.Width()/3;

					dc->MoveTo(cx, cy-siz);
					dc->LineTo(cx, cy+siz);
					dc->LineTo(cx - siz/2, cy+siz - (siz*4)/5);
					dc->LineTo(cx, cy+siz);
					dc->LineTo(cx + siz/2, cy+siz - (siz*4)/5);
				}
				break;
			case PUP: //0x0F
				label = _T("PgUp");
				break;
			case PDN: //0x10
				label = _T("PgDn");
				break;
			case HOME: //0x11
				label = _T("Home");
				break;
			case END: //0x12
				label = _T("End");
				break;
			case INS: //0x13
				label = _T("Ins");
				break;
			case DEL: //0x14
				label = _T("Del");
				break;
			case WWW:
				label = _T("WWW");
				break;
			default:
//				ASSERT(FALSE);
//				label = _T("#ERR#");
				break;
			}
		}
		if(!special)
		{
			strTemp.Format(_T("%s"), label);
			tsize = dc->GetTextExtent(strTemp);
			dc->SetTextColor(WHITE_C);
			dc->TextOut(
				((rc.left + rc.right)/2) - (tsize.cx/2), 
				((rc.top + rc.bottom)/2) - (tsize.cy/2), 
				strTemp);
		}
	}
	else
	{
		if((key->cShifted >= 'A') && (key->cShifted <= 'Z'))
		{
			strTemp.Format(_T("%c"), key->cShifted);
			tsize = dc->GetTextExtent(strTemp);
			//dc->SetTextColor(RGB(255, 255, 255));
			dc->TextOut(
				((rc.left + rc.right)/2) - (tsize.cx/2), 
				((rc.top + rc.bottom)/2) - (tsize.cy/2), 
				strTemp);
		}
		else
		{
			//tsize = dc->GetTextExtent(_T("M"));
			strTemp.Format(_T("%c"), key->cShifted);
			tsize = dc->GetTextExtent(strTemp);
			//dc->SetTextColor(RGB(255, 255, 255));
			dc->TextOut(
				((rc.left + rc.right)/2) - (tsize.cx/2), 
				((rc.top + rc.bottom)/2) - (tsize.cy/2), 
				strTemp);
		}
	}
}

int CDialogKeyPad::KeyState()
{
	int state = 0;

	short ks;

	ks = GetKeyState(VK_CAPITAL);
	if(ks & 0x000F) state += 0x01;
	ks = GetKeyState(VK_SHIFT);
	if(ks & 0xF000) state += 0x02;
	ks = GetKeyState(VK_CONTROL);
	if(ks & 0xF000) state += 0x04;

	return state;
}

void CDialogKeyPad::KeySend(CDC *dc, CRect rectArea, BOOL bFlag, tagKeyPad * keydef)
{
	int vk;

	char pChar;

	SHORT ks;

	BYTE key;

	BOOL uptoo;

	unsigned char cvk;

	if(keydef->cNormal == 0x00)
	{

		uptoo = TRUE;
		if(keydef->cShifted > 0x6F) //is a function key
		{
			vk = keydef->cShifted;
		}
		else
		{
			switch(keydef->cShifted)
			{
			case TAB:    // 0x02
				vk = VkKeyScan(0x09) & 0xFF;
				break;
			case CAPSLOCK:    // 0x03
				vk = VK_CAPITAL;
				break;
			case SHIFT:    // 0x04
				vk = VK_SHIFT;
				if(GetKeyState(vk) & 0xF000) return;
				uptoo = FALSE;
				break;
			case CTRL:    // 0x05
				vk = VK_CONTROL;
				if(GetKeyState(vk) & 0xF000) return;
				uptoo = FALSE;
				break;
			case BSPACE:    // 0x06
				vk = VK_BACK;
				break;
			case ENTER:    // 0x07
				vk = VkKeyScan(0x0D) & 0xFF;
				break;
			case ALT:    // 0x08
				vk = VK_MENU;
				break;
			case WINDOW:    // 0x9
				vk = VK_LWIN;
				break;
			case LEFT:    // 0x0A
				vk = VK_LEFT;
				break;
			case RIGHT:    // 0x0B
				vk = VK_RIGHT;
				break;
			case UP:    // 0x0C
				vk = VK_UP;
				break;
			case DOWN:    // 0x0D
				vk = VK_DOWN;
				break;
			case PUP: //0x0F
				vk = VK_PRIOR;
				break;
			case PDN: //0x10
				vk = VK_NEXT;
				break;
			case HOME: //0x11
				vk = VK_HOME;
				break;
			case END: //0x12
				vk = VK_END;
				break;
			case INS: //0x13
				vk = VK_INSERT;
				break;
			case DEL: //0x14
				vk = VK_DELETE;
				break;
			case WWW:
/*
				key.cNormal = key.cShifted = 'H';
				KeySend(&key);
				key.cNormal = key.cShifted = 'T';
				KeySend(&key);
				KeySend(&key);
				key.cNormal = key.cShifted = 'P';
				KeySend(&key);
				key.cNormal = key.cShifted = ':';
				KeySend(&key);
				key.cNormal = key.cShifted = '/';
				KeySend(&key);
				KeySend(&key);
				key.cNormal = key.cShifted = 'W';
				KeySend(&key);
				KeySend(&key);
				KeySend(&key);
				vk = VkKeyScan('.') & 0xFF;*/
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		keybd_event(vk,0,0,0);
		if(uptoo)
		{
			keybd_event(vk,0,KEYEVENTF_KEYUP,0);
		}
	}
	else
	{
		pChar	= keydef->cNormal;
		ks		= VkKeyScan(pChar);
		key		= ks & 0xFF;

		keybd_event(key,0,0,0);
		keybd_event(key,0,KEYEVENTF_KEYUP,0);

		//turn off the control and shift if they were down
		cvk = VK_SHIFT;
		if(GetKeyState(cvk) & 0xF000)
		{
			keybd_event(cvk,0,KEYEVENTF_KEYUP,0);
		}
		cvk = VK_CONTROL;
		if(GetKeyState(cvk) & 0xF000) //allow toggle
		{
			keybd_event(cvk,0,KEYEVENTF_KEYUP,0);
		}
	}

	KeyDraw(dc, rectArea, keydef, bFlag);
}

void CDialogKeyPad::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int state = KeyState();
	
	if(state != m_nKeyState)
	{
		KeyDraw();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDialogKeyPad::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages

	KeyDraw();
	GetDlgItem(IDC_EDIT_NEW)->SetFocus();
}

void CDialogKeyPad::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_NEW)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_NEW)->SetFocus();
}

void CDialogKeyPad::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	int nVal;
	double dVal;
	
	UpdateData(TRUE);
	
	GetDlgItem(IDC_EDIT_NEW)->GetWindowText(m_strKeyData);

	if (m_nMode == 0)
	{
		int nMin;
		int nMax;
		
		nVal = atoi(m_strKeyData);
		
		nMin = atoi(m_strMin);
		if (nVal < nMin)
		{
			AfxMessageBox(_T("Input Data is Limit Under."));
			
			GetDlgItem(IDC_EDIT_NEW)->SetWindowText(_T(""));
			GetDlgItem(IDC_EDIT_NEW)->SetFocus();
			
			return;
		}
		
		nMax = atoi(m_strMax);
		if (nVal > nMax)
		{
			AfxMessageBox(_T("Input Data is Limit Over."));
			
			GetDlgItem(IDC_EDIT_NEW)->SetWindowText(_T(""));
			GetDlgItem(IDC_EDIT_NEW)->SetFocus();
			
			return;
		}
	}
	else
	{
		double dMin;
		double dMax;
		
		dVal = atof(m_strKeyData);
		
		dMin = atof(m_strMin);
		if (dVal < dMin)
		{
			AfxMessageBox(_T("Input Data is Limit Under."));
			
			GetDlgItem(IDC_EDIT_NEW)->SetWindowText(_T(""));
			GetDlgItem(IDC_EDIT_NEW)->SetFocus();
			
			return;
		}
		
		dMax = atof(m_strMax);
		if (dVal > dMax)
		{
			AfxMessageBox(_T("Input Data is Limit Over."));
			
			GetDlgItem(IDC_EDIT_NEW)->SetWindowText(_T(""));
			GetDlgItem(IDC_EDIT_NEW)->SetFocus();
			
			return;
		}
	}
	
	POSITION pos = m_pKeys.GetHeadPosition();
	
	while(pos)
	{
		delete m_pKeys.GetNext(pos);
	}

	CDialog::OnOK();
}

void CDialogKeyPad::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_pKeys.GetHeadPosition();
	
	while(pos)
	{
		delete m_pKeys.GetNext(pos);
	}

	CDialog::OnCancel();
}
