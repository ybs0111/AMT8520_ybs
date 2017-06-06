; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CManagerSocketApp
LastClass=CDialogManager
NewFileInclude2=#include "ManagerSocket.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Resource1=IDD_DIALOG_ADD
Class2=CDialogAdd
LastTemplate=CDialog
Class3=CDialogManager
Resource2=IDD_DIALOG_MANAGER_SOCKET

[CLS:CManagerSocketApp]
Type=0
HeaderFile=ManagerSocket.h
ImplementationFile=ManagerSocket.cpp
Filter=N
LastObject=CManagerSocketApp
BaseClass=CWinApp
VirtualFilter=AC

[DLG:IDD_DIALOG_ADD]
Type=1
Class=CDialogAdd
ControlCount=8
Control1=IDC_BTN_ADD,button,1342242817
Control2=IDC_BTN_EXIT,button,1342242816
Control3=IDC_MSG_SERIAL,static,1350697473
Control4=IDC_MSG_SERIAL_DATA,static,1350697473
Control5=IDC_MSG_HIFIX,static,1350697473
Control6=IDC_MSG_HIFIX_DATA,static,1350697473
Control7=IDC_MSG_COUNT,static,1350697473
Control8=IDC_MSG_COUNT_DATA,static,1350697473

[DLG:IDD_DIALOG_MANAGER_SOCKET]
Type=1
Class=CDialogManager
ControlCount=22
Control1=IDC_MSG_HIFIX,static,1350697473
Control2=IDC_MSG_HIFIX_DATA,static,1350697729
Control3=IDC_GROUP_SOCKET,button,1342177287
Control4=IDC_MSG_SERIAL,static,1350697473
Control5=IDC_MSG_SERIAL_DATA,static,1350697729
Control6=IDC_MSG_SLOT,static,1350697473
Control7=IDC_MSG_SLOT_DATA,static,1350697729
Control8=IDC_MSG_COUNT_1,static,1350697473
Control9=IDC_MSG_COUNT_DATA_1,static,1350697729
Control10=IDC_BTN_SOCKET_INSERT,button,1342242816
Control11=IDC_BTN_SOCKET_REMOVE,button,1342242816
Control12=IDC_BTN_ADD_SOCKET,button,1342242816
Control13=IDC_BTN_DELETE_SOCKET,button,1342242816
Control14=IDC_GROUP_MANAGER,button,1342177287
Control15=IDC_MSG_HIFIX_NUM,static,1216479745
Control16=IDC_MSG_HIFIX_NUM_DATA,static,1216479745
Control17=IDC_CUSTOM_MANAGER,fpSpread70,1350631424
Control18=IDC_CUSTOM_SOCKET,fpSpread70,1350631424
Control19=IDC_CUSTOM_HIFIX,fpSpread70,1350631424
Control20=IDC_COMBO_HIFIX,combobox,1344339971
Control21=IDC_BTN_APPLY,button,1342242816
Control22=IDC_BTN_CHANGE_SOCKET,button,1342242816

[CLS:CDialogAdd]
Type=0
HeaderFile=DialogAdd.h
ImplementationFile=DialogAdd.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_MSG_HIFIX

[CLS:CDialogManager]
Type=0
HeaderFile=DialogManager.h
ImplementationFile=DialogManager.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_BTN_APPLY
VirtualFilter=dWC

