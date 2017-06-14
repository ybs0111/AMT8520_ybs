; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CXgemInterfaceDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "xgeminterface.h"
LastPage=0
CDK=Y

ClassCount=9
Class1=CClientSocket
Class2=CColorListBox
Class3=CGradientStatic
Class4=CServerItem
Class5=CServerSocket
Class6=CXgemInterfaceApp
Class7=CAboutDlg
Class8=CXgemInterfaceDlg
Class9=CXPGroupBox

ResourceCount=6
Resource1=IDR_MENU
Resource2=IDD_ABOUTBOX
Resource3=IDR_MENU (English (U.S.))
Resource4=IDD_ABOUTBOX (Korean)
Resource5=IDD_XGEMINTERFACE_DIALOG (Korean)
Resource6=IDD_XGEMINTERFACE_DIALOG

[CLS:CClientSocket]
Type=0
BaseClass=CSocket
HeaderFile=ClientSocket.h
ImplementationFile=ClientSocket.cpp

[CLS:CColorListBox]
Type=0
BaseClass=CListBox
HeaderFile=ColorListBox.h
ImplementationFile=ColorListBox.cpp

[CLS:CGradientStatic]
Type=0
BaseClass=CStatic
HeaderFile=GradientStatic.h
ImplementationFile=GradientStatic.cpp

[CLS:CServerItem]
Type=0
BaseClass=CSocket
HeaderFile=ServerItem.h
ImplementationFile=ServerItem.cpp
Filter=N
VirtualFilter=uq
LastObject=CServerItem

[CLS:CServerSocket]
Type=0
BaseClass=CSocket
HeaderFile=ServerSocket.h
ImplementationFile=ServerSocket.cpp

[CLS:CXgemInterfaceApp]
Type=0
BaseClass=CWinApp
HeaderFile=XgemInterface.h
ImplementationFile=XgemInterface.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=XgemInterfaceDlg.cpp
ImplementationFile=XgemInterfaceDlg.cpp
LastObject=CAboutDlg

[CLS:CXgemInterfaceDlg]
Type=0
BaseClass=CDialog
HeaderFile=XgemInterfaceDlg.h
ImplementationFile=XgemInterfaceDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CXgemInterfaceDlg

[CLS:CXPGroupBox]
Type=0
BaseClass=CButton
HeaderFile=XPGroupBox.h
ImplementationFile=XPGroupBox.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_XGEMINTERFACE_DIALOG]
Type=1
Class=CXgemInterfaceDlg
ControlCount=7
Control1=IDC_MSG_HOST_CONNECTOR,static,1350697473
Control2=IDC_MSG_EQ_CONNECTOR,static,1350697473
Control3=IDC_MSG_MACHINE_STATUS,static,1350697473
Control4=IDC_EXGEMCTRL_GEM,{2220F20C-2FA3-461C-84D4-948B6D07E79B},1342242816
Control5=IDC_LIST_DATA,listbox,1345388801
Control6=IDC_BTN_HIDE,button,1342242816
Control7=IDC_BUTTON1,button,1342242816

[MNU:IDR_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_VIEW
Command2=ID_EXIT
CommandCount=2

[DLG:IDD_XGEMINTERFACE_DIALOG (Korean)]
Type=1
Class=?
ControlCount=6
Control1=IDC_MSG_HOST_CONNECTOR,static,1350697473
Control2=IDC_MSG_EQ_CONNECTOR,static,1350697473
Control3=IDC_MSG_MACHINE_STATUS,static,1350697473
Control4=IDC_EXGEMCTRL_GEM,{2220F20C-2FA3-461C-84D4-948B6D07E79B},1342242816
Control5=IDC_LIST_DATA,listbox,1345388801
Control6=IDC_BTN_HIDE,button,1342242816

[DLG:IDD_ABOUTBOX (Korean)]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MENU]
Type=1
Class=?
Command1=ID_VIEW
Command2=ID_EXIT
CommandCount=2

