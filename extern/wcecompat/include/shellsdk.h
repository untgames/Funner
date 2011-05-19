//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
#ifndef _SHELLSDK_H_
#define _SHELLSDK_H_

// shellsdk.h - exported functions from aygshell.dll
#include <windows.h>
#include <sipapi.h>
#include <prsht.h>
#include <shsdkstc.h>

//
// Define API decoration for direct importing of DLL references.
//
//#ifndef WINSHELLAPI
//#if !defined(_SHELL32_)
//#define WINSHELLAPI DECLSPEC_IMPORT
//#else
#undef WINSHELLAPI
#define WINSHELLAPI
//#endif
//#endif // WINSHELLAPI

#ifdef __cplusplus
extern "C" {
#endif

//++++++
//
// SHCloseApps
//

BOOL WINAPI SHCloseApps( DWORD dwMemSought );

//
// End SHCloseApps
//
//------

//++++++
//
// SHCreateMenuBar
//

//++++++
//
// Shell Menubar support
//

#define NOMENU 0xFFFF
#define IDC_COMMANDBANDS    100

// These defines MUST be < 100.  This is so apps can use these defines
// to get strings from the shell.
#define IDS_SHNEW           1
#define IDS_SHEDIT          2
#define IDS_SHTOOLS         3
#define IDS_SHVIEW          4
#define IDS_SHFILE          5
#define IDS_SHGO            6
#define IDS_SHFAVORITES     7
#define IDS_SHOPEN          8

//
// Shared New menu support
//
#define  IDM_SHAREDNEW        10
#define  IDM_SHAREDNEWDEFAULT 11

//
// Valid dwFlags
//
#define SHCMBF_EMPTYBAR      0x0001
#define SHCMBF_HIDDEN        0x0002 // create it hidden
#define SHCMBF_HIDESIPBUTTON 0x0004
#define SHCMBF_COLORBK       0x0008
#define SHCMBF_HMENU         0x0010 // specify an hmenu for resource rather than toolbar info
// HPC ONLY
#define IDC_MENUBAND				   1000
#define IDC_TOOLBARBAND				   1001

typedef struct tagSHMENUBARINFO
{
    DWORD cbSize;               // IN  - Indicates which members of struct are valid
    HWND hwndParent;            // IN
    DWORD dwFlags;              // IN  - Some features we want
    UINT nToolBarId;            // IN  - Which toolbar are we using
    HINSTANCE hInstRes;         // IN  - Instance that owns the resources
    int nBmpId;
    int cBmpImages;             // IN  - Count of bitmap images
    HWND hwndMB;                // OUT
    COLORREF clrBk;             // IN  - background color of the menu bar (excluding sip)
} SHMENUBARINFO, *PSHMENUBARINFO;

WINSHELLAPI BOOL  SHCreateMenuBar(SHMENUBARINFO *pmbi);


#define SHCMBM_SETSUBMENU   (WM_USER + 400) // wparam == id of button, lParam == hmenu, return is old hmenu
#define SHCMBM_GETSUBMENU   (WM_USER + 401) // lParam == ID
#define SHCMBM_GETMENU      (WM_USER + 402) // get the owning hmenu (as specified in the load resource)

// Convenience Macros
#define SHGetMenu(hWndMB)            (HMENU)SendMessage((hWndMB), SHCMBM_GETMENU, (WPARAM)0, (LPARAM)0);
#define SHGetSubMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), SHCMBM_GETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);
#define SHSetSubMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), SHCMBM_SETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);

//Alternate definition
#define SHMenuBar_GetMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), SHCMBM_GETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);

//
// End SHCreateMenuBar
//
//------

//++++++
//
// SHCreateNewItem
//
//    Creates a New item, as if an item were chosen from the
//    global New menu dropdown.

HRESULT SHCreateNewItem (HWND hwndOwner, REFCLSID clsid);

//
// End SHCreateNewItem
//
//------


//++++++
//
// SHDoneButton
//

BOOL SHDoneButton(HWND hwndRequester, DWORD dwState);


//
// Valid states
//

#define SHDB_SHOW                   0x0001
#define SHDB_HIDE                   0x0002


//
// End SHDoneButton
//
//------

//++++++
//
// SHFindMenuBar
//
// Returns menubar owned by a window
//

HWND WINAPI SHFindMenuBar(HWND hwnd);

//
// End SHFindMenuBar
//
//------

//++++++
//
// SHFreeContextMenuExtensions
//

//    Releases memory allocated for context menu processing.

BOOL SHFreeContextMenuExtensions(HANDLE hCMExtensions);

//
// End SHFreeContextMenuExtensions
//
//------

//++++++
//
// SHFullScreen
//

BOOL SHFullScreen(HWND hwndRequester, DWORD dwState);


//
// Valid states
//

#define SHFS_SHOWTASKBAR            0x0001
#define SHFS_HIDETASKBAR            0x0002
#define SHFS_SHOWSIPBUTTON          0x0004
#define SHFS_HIDESIPBUTTON          0x0008
#define SHFS_SHOWSTARTICON          0x0010
#define SHFS_HIDESTARTICON          0x0020

//
// End SHFullScreen
//
//------

//++++++
//
//  SHGetAppKeyAssoc
//
//       Gets which key (VK_APP1-6) is assigned to this app.

BYTE WINAPI SHGetAppKeyAssoc(LPCTSTR ptszApp);

//
// End SHGetAppKeyAssoc
//
//------

//++++++
//
// SHGetAutoRunPath
//	pAutoRunPath must be at least MAX_PATH long

BOOL SHGetAutoRunPath( LPTSTR pAutoRunPath );

//
// End SHGetAutoRunPath
//
//------

//++++++
//
//  SHGetEmergencyCallList
//
//       Gets a list of emergency calls

HRESULT SHGetEmergencyCallList(TCHAR *pwszBuffer, UINT uLenBuf);

//
// End SHGetEmergencyCallList
//
//------

//++++++
//
// SHHandleWMActivate
//

typedef struct
{
    DWORD cbSize;
    HWND hwndLastFocus;
    UINT fSipUp :1;
    UINT fSipOnDeactivation :1;
    UINT fActive :1;
    UINT fReserved :29;
} SHACTIVATEINFO, *PSHACTIVATEINFO;

#define SHA_INPUTDIALOG 0x00000001

WINSHELLAPI BOOL SHHandleWMActivate(HWND hwnd, WPARAM wParam, LPARAM lParam, SHACTIVATEINFO* psai, DWORD dwFlags);
WINSHELLAPI BOOL SHHandleWMSettingChange(HWND hwnd, WPARAM wParam, LPARAM lParam, SHACTIVATEINFO* psai);

//
// End SHHandleWMActivate
//
//------

//++++++
//
// SHInitDialog 
//

typedef struct tagSHINITDLGINFO
{
    DWORD dwMask;
    HWND  hDlg;
    DWORD dwFlags;
} SHINITDLGINFO, *PSHINITDLGINFO;


//
// The functions
//

BOOL SHInitDialog(PSHINITDLGINFO pshidi);

//
// Valid mask values
//

#define SHIDIM_FLAGS                0x0001

//
// Valid flags
//

#define SHIDIF_DONEBUTTON           0x0001
#define SHIDIF_SIZEDLG              0x0002
#define SHIDIF_SIZEDLGFULLSCREEN    0x0004
#define SHIDIF_SIPDOWN              0x0008
#define SHIDIF_FULLSCREENNOMENUBAR  0x0010
#define SHIDIF_EMPTYMENU            0x0020

//
// End SHInitDialog
//
//------


//++++++
//
// SHInitExtraControls 
//
#define WC_SIPPREF    L"SIPPREF"

BOOL SHInitExtraControls(void);

//
// End SHInitExtraControls
//
//------

//++++++
//
// SHInputDialog
//

void SHInputDialog(HWND hwnd, UINT uMsg, WPARAM wParam);

//
// End SHInputDialog
//
//------

//++++++
//
// SHInvokeContextMenuCommand
//
//    Invokes a command from a context menu.  Issues the command in the
//    extension that added it to the menu.

BOOL SHInvokeContextMenuCommand(HWND hwndOwner, UINT idCmd,
        HANDLE hCMExtensions);

//
// End SHInvokeContextMenuCommand
//
//------

//++++++
//
// SHLoadContextMenuExtensions
//
//    Loads context menu extensions from handlers listed in the registry for
//    the context/class pair specified.  Menu items are added to hmenu in the
//    range [idCmdFirst, idCmdLast].  A handle to the context menu extensions
//    abstraction object is returned in *phCMExtensions.  It must be freed by
//    a call to SHFreeContextMenuExtensions.

BOOL SHLoadContextMenuExtensions(IUnknown *punkOwner, LPCTSTR pszContext,
    LPCTSTR pszClass, HMENU hmenu, UINT idCmdFirst, UINT idCmdLast,
    HANDLE *phCMExtensions);

//
// End SHLoadContextMenuExtensions
//
//------

//++++++
//
// SHNavigateBack APIs
//

void WINAPI SHNavigateBack();
void WINAPI SHSetBack(int, HWND);

//
// End SHNavigateBack APIs
//
//------

//////////////////////////////////////////////////////////////////////////////
//
// SHNAPI - Shell Notification API
//
//

// notification update mask

//#define SHN_FIRST (0U-1000U) // Shell reserved

#define SHNN_FIRST  (0U-1000U) // Shell Notifications
#define SHNN_LAST   (0U-1020U) // Shell Notifications

//#define SHN_LAST (0U-11000U)

#define SHNUM_PRIORITY     0x0001
#define SHNUM_DURATION     0x0002
#define SHNUM_ICON         0x0004
#define SHNUM_HTML         0x0008
#define SHNUM_TITLE        0x0010

// Flags

// For SHNP_INFORM priority and above, don't display the notification bubble
// when it's initially added; the icon will display for the duration then it
// will go straight into the tray.  The user can view the icon / see the
// bubble by opening the tray.
#define SHNF_STRAIGHTTOTRAY  0x00000001

// Critical information - highlights the border and title of the bubble.
#define SHNF_CRITICAL        0x00000002

// Force the message (bubble) to display even if settings says not to.
#define SHNF_FORCEMESSAGE    0x00000008

// Force the display to turn on for notification.
#define SHNF_DISPLAYON       0x00000010


// notification message and codes for window-based notification
// the notification's dwID is in hdr.idFrom

typedef struct _NMSHN
{
    NMHDR   hdr;
    LPARAM lParam;
    DWORD dwReturn;
    union
    {
        LPCTSTR pszLink;
        BOOL    fTimeout;
        POINT   pt;
    };
} NMSHN;

#define SHNN_LINKSEL            (SHNN_FIRST-0)
// nmshn.pszLink contains the link text of the choice that was selected

#define SHNN_DISMISS            (SHNN_FIRST-1)
// nmshn.fTimeout is TRUE if duration expired, FALSE if user tapped away

#define SHNN_SHOW               (SHNN_FIRST-2)
// nmshn.pt contains the point to which the bubble points, under HPC
// the point is zero

//===========================================================================
//
// Interface: IShellNotificationCallback
//
//  The IShellNotificationCallback interface is used by the Shell to advise
// the notification owner of actions taken on the notification.
//
// [Member functions]
//
// IShellNotificationCallback::OnShow
//
//  Reserved.  Return E_NOTIMPL.
//
// IShellNotificationCallback::OnCommandSelected
//
//  This member function is called when the user selects a link of the form
// <A HREF="cmd:#">link</A>.
//
//  Parameters:
//   dwID       -- the identifier of the notification
//   wCmdID     -- this is the # in the link
//
// IShellNotificationCallback::OnLinkSelected
//
//  This member function is called when the user selects one of the action
// choice links in the notification bubble window.
//
//  Parameters:
//   dwID       -- the identifier of the notification
//   pszLink    -- the link content that was selected
//   lParam     -- the lParam of the notification
//
// IShellNotificationCallback::OnDismiss
//
//  This member function is called when the user taps away from the bubble
// window or if a SHNP_INFORM priority notification's duration expires.
//
//  Parameters:
//   dwID       -- the identifier of the notification
//   fTimeout   -- the notification timed out (SHNP_INFORM only)
//   lParam     -- the lParam of the notification
//   
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellNotificationCallback

DECLARE_INTERFACE_(IShellNotificationCallback, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IShellNotificationCallback methods ***
    STDMETHOD(OnShow)(THIS_ DWORD dwID, POINT pt, LPARAM lParam) PURE;
    STDMETHOD(OnCommandSelected)(THIS_ DWORD dwID, WORD wCmdID)
        PURE;
    STDMETHOD(OnLinkSelected)(THIS_ DWORD dwID, LPCTSTR pszLink, LPARAM lParam)
        PURE;
    STDMETHOD(OnDismiss)(THIS_ DWORD dwID, BOOL fTimeout, LPARAM lParam) PURE;
};

//++++++
//
// SHNotificationAdd
//
//   Add a notification.

LRESULT SHNotificationAdd(SHNOTIFICATIONDATA *pndAdd);

//
// End SHNotificationAdd
//
//------


//++++++
//
// SHNotificationGetData
//
//   Get the data for a notification.  Used by a handler to get information
//   stored in the notification by the poster.

LRESULT SHNotificationGetData(const CLSID *pclsid, DWORD dwID,
    SHNOTIFICATIONDATA *pndBuffer);

//
// End SHNotificationGetData
//
//------


//++++++
//
// SHNotificationRemove
//
//   Remove a notification.  This is usually in reponse to some
//   action taken on the data outside of the notification system - for example
//   if a message is read or deleted.

LRESULT SHNotificationRemove(const CLSID *pclsid, DWORD dwID);

//
// End SHNotificationRemove
//
//------


//++++++
//
// SHNotificationUpdate
//
//   Update aspects of a pending notification.

LRESULT SHNotificationUpdate(DWORD grnumUpdateMask, SHNOTIFICATIONDATA *pndNew);

//
// End SHNotificationUpdate
//
//------

// }
//
// end SHNAPI
//
//////////////////////////////////////////////////////////////////////////////

//++++++
//
// SHRecognizeGesture structs
//

typedef struct tagSHRGI {
    DWORD cbSize;
    HWND hwndClient;
    POINT ptDown;
    DWORD dwFlags;
} SHRGINFO, *PSHRGINFO;


//
// Gesture notifications
//
#define  GN_CONTEXTMENU       1000


//
// Gesture flags
//
#define  SHRG_RETURNCMD       0x00000001
#define  SHRG_NOTIFYPARENT    0x00000002
// use the longer (mixed ink) delay timer
// useful for cases where you might click down first, verify you're
// got the right spot, then start dragging... and it's not clear
// you wanted a context menu
#define  SHRG_LONGDELAY       0x00000008 
#define  SHRG_NOANIMATION     0x00000010

//
// Struct sent through WM_NOTIFY when SHRG_NOTIFYPARENT is used
//
typedef struct tagNMRGINFO 
{
    NMHDR hdr;
    POINT ptAction;
    DWORD dwItemSpec;
} NMRGINFO, *PNMRGINFO;

WINSHELLAPI DWORD SHRecognizeGesture(SHRGINFO *shrg);

//
// End SHRecognizeGesture
//
//------

//++++++
//
// SHSetAppKeyWndAssoc
//

BOOL WINAPI SHSetAppKeyWndAssoc(BYTE bVk, HWND hwnd);

//
// End SHSetAppKeyWndAssoc
//
//------

//++++++
//
// SHSetNavBarText
//

BOOL WINAPI SHSetNavBarText( HWND hwnd, LPCTSTR pszText );

//
// End SHSetNavBarText
//
//------

//++++++
//
// SHSipInfo
//

//
// Supported system parameters not defined in sipapi.h
//
#define SPI_APPBUTTONCHANGE     228
#define SPI_RESERVED            229
#define SPI_SYNCSETTINGSCHANGE  230

WINSHELLAPI BOOL SHSipInfo( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni );

//
// End SHSipInfo
//
//------

//++++++
//
// SHSipPreference
//

BOOL WINAPI SHSipPreference(HWND hwnd, SIPSTATE st);

//
// End SHSipPreference
//
//------

//====== File System Notification APIs ===============================
//

//
//  File System Notification flags
//

// see shlobj.h for other SHCNE and SHCNF defines

#define SHCNE_RENAME	          0x00000001L   // GOING AWAY
#define WM_FILECHANGEINFO   (WM_APP + 0x101)

//
//  APIs
//
BOOL WINAPI SHChangeNotifyRegister( HWND hwnd, SHCHANGENOTIFYENTRY *pshcne );
BOOL WINAPI SHChangeNotifyDeregister( HWND hwnd );
void WINAPI SHChangeNotifyFree( FILECHANGENOTIFY *pfcn );



#ifdef __cplusplus
}
#endif

#endif // _SHELLSDK_H_
