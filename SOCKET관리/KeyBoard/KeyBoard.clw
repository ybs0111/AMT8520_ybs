; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CKeyBoardApp
LastClass=CDialogKeyPad
NewFileInclude2=#include "KeyBoard.h"
ResourceCount=3
NewFileInclude1=#include "stdafx.h"
Resource1=IDD_DIALOG_KEYPAD
Resource2=IDD_DIALOG1
Class2=CDialogKeyBoard
LastTemplate=CDialog
Class3=CDialogKeyPad
Resource3=IDD_DIALOG_KEYBOARD

[CLS:CKeyBoardApp]
Type=0
HeaderFile=KeyBoard.h
ImplementationFile=KeyBoard.cpp
Filter=N

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[DLG:IDD_DIALOG_KEYPAD]
Type=1
Class=CDialogKeyPad
ControlCount=12
Control1=IDC_MSG_KEYPAD,static,1342177287
Control2=IDC_MSG_MIN,static,1350697729
Control3=IDC_MSG_MIN_DATA,static,1350697729
Control4=IDC_MSG_MAX,static,1350697729
Control5=IDC_MSG_MAX_DATA,static,1350697729
Control6=IDC_MSG_CURRENT,static,1350697729
Control7=IDC_MSG_CURRENT_DATA,static,1350697729
Control8=IDC_MSG_NEW,static,1350697729
Control9=IDC_EDIT_NEW,edit,1350631552
Control10=IDC_BTN_CLEAR,button,1342242816
Control11=IDC_BTN_OK,button,1342242816
Control12=IDC_BTN_CANCEL,button,1342242816

[DLG:IDD_DIALOG_KEYBOARD]
Type=1
Class=CDialogKeyBoard
ControlCount=4
Control1=IDC_EDIT_KEYBOARD,edit,1350631552
Control2=IDC_BTN_OK,button,1342242816
Control3=IDC_BTN_CANCEL,button,1342242816
Control4=IDC_MSG_KEYBOARD,static,1342177287

[CLS:CDialogKeyBoard]
Type=0
HeaderFile=DialogKeyBoard.h
ImplementationFile=DialogKeyBoard.cpp
BaseClass=CDialog
Filter=D

[CLS:CDialogKeyPad]
Type=0
HeaderFile=DialogKeyPad.h
ImplementationFile=DialogKeyPad.cpp
BaseClass=CDialog
Filter=D

