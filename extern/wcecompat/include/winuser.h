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
/*


Module Name: winuser.h

--*/
//	@doc BOTH EXTERNAL CEUSER

#ifndef _WINUSER_
#define _WINUSER_

#ifdef __cplusplus
extern "C" {
#endif

#include <windef.h>
#include <mmsystem.h>


typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
} MSG, *PMSG, *NPMSG, *LPMSG;

#ifndef NORESOURCE

#define MAKEINTRESOURCEA(i) (LPSTR)((DWORD)((WORD)(i)))
#define MAKEINTRESOURCEW(i) (LPWSTR)((DWORD)((WORD)(i)))
#ifdef UNICODE
#define MAKEINTRESOURCE  MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE

/*
 * Predefined Resource Types
 */
#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_FONTDIR          MAKEINTRESOURCE(7)
#define RT_FONT             MAKEINTRESOURCE(8)
#define RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define RT_RCDATA           MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)

#define DIFFERENCE          11
#define RT_GROUP_CURSOR     MAKEINTRESOURCE((DWORD)RT_CURSOR + DIFFERENCE)
#define RT_GROUP_ICON       MAKEINTRESOURCE((DWORD)RT_ICON + DIFFERENCE)
#define RT_VERSION          MAKEINTRESOURCE(16)
#define RT_DLGINCLUDE       MAKEINTRESOURCE(17)


#define	RT_CEUIEXPERIENCE	L"CEUX"
#define RN_HI_RES_AWARE		L"HI_RES_AWARE"



#endif /* !NORESOURCE */

typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);
// @CESYSGEN IF GWES_TIMER
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

/* WNDCLASS */
typedef struct tagWNDCLASSA {
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
} WNDCLASSA, *PWNDCLASSA, *LPWNDCLASSA;

typedef struct tagWNDCLASSW {
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCWSTR     lpszMenuName;
    LPCWSTR     lpszClassName;
} WNDCLASSW, *PWNDCLASSW, *LPWNDCLASSW;

#ifdef UNICODE
typedef WNDCLASSW WNDCLASS;
typedef PWNDCLASSW PWNDCLASS;
typedef LPWNDCLASSW LPWNDCLASS;
typedef PWNDCLASSW PWNDCLASS;
#else
typedef WNDCLASSA WNDCLASS;
typedef PWNDCLASSA PWNDCLASS;
typedef LPWNDCLASSA LPWNDCLASS;
typedef PWNDCLASSA PWNDCLASS;
#endif // UNICODE


// Shell support
#define HSHELL_WINDOWCREATED        1
#define HSHELL_WINDOWDESTROYED      2
#define HSHELL_WINDOWACTIVATED      4


#define POINTSTOPOINT(pt, pts)                          \
        { (pt).x = (LONG)(SHORT)LOWORD(*(LONG*)&pts);   \
          (pt).y = (LONG)(SHORT)HIWORD(*(LONG*)&pts); }

#define POINTTOPOINTS(pt)      (MAKELONG((short)((pt).x), (short)((pt).y)))

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WMBASE
// @CESYSGEN IF GWES_FOREGND
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_CURSOR || GWES_MCURSOR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_DLGMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_DLGMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_KBDUI
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_DLGMGR
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_TIMER
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_MENU
// @CESYSGEN ENDIF

#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209
#define WM_MOUSELAST                    0x0209

#define WM_MOUSEWHEEL                   0x020A
#define WM_XBUTTONDOWN                   0x020B
#define WM_XBUTTONUP                     0x020C
#define WM_XBUTTONDBLCLK                 0x020D

#undef WM_MOUSELAST
#define WM_MOUSELAST					0x020D

/* XButton values are WORD flags */
#define XBUTTON1      0x0001
#define XBUTTON2      0x0002

// @CESYSGEN IF GWES_MENU
// @CESYSGEN ENDIF
#define WM_CAPTURECHANGED               0x0215

// @CESYSGEN IF GWES_CLIPBD
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_PGDI || GWES_MGPAL
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_MSGBOX
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_EDCTL
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_LBCTL
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_BTNCTL
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_DLGMGR
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_SCBCTL
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_STCCTL
// @CESYSGEN ENDIF
#define WM_VKEYTOITEM                   0x002E
#define WM_CHARTOITEM                   0x002F
#define WM_QUERYDRAGICON                0x0037

// New for WINCE
#define WM_DBNOTIFICATION				0x03FD
#define WM_NETCONNECT                   0x03FE
#define WM_HIBERNATE                    0x03FF

#define WM_APP                          0x8000

/*
 * NOTE: All Message Numbers below 0x0400 are RESERVED.
 *
 * Private Window Messages Start Here:
 */
#define WM_USER                         0x0400

/*
 * Keyboard Layout API
 */

#define HKL_PREV            0
#define HKL_NEXT            1

/*
 * Used with WM_INPUTLANGCHANGEREQUEST
 */
#define INPUTLANGCHANGE_SYSCHARSET      0x0001
#define INPUTLANGCHANGE_FORWARD         0x0002
#define INPUTLANGCHANGE_BACKWARD        0x0004  

/*
 * ActivateKeyboardLayout flags
 */
#define KLF_ACTIVATE        0x00000001
#define KLF_SETFORPROCESS   0x00000100

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF GWES_WINMGR


// @CESYSGEN IF GWES_NCLIENT
// @CESYSGEN ENDIF GWES_NCLIENT



// @CESYSGEN IF GWES_CLIPBD
// @CESYSGEN ENDIF
/*
	WM_WININICHANGE section paramater
*/
#define INI_INTL			1			// Regional Setting changed


/*
 * WM_NCHITTEST and MOUSEHOOKSTRUCT Mouse Position Codes
 */
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT
#if(WINVER >= 0x0400)
#define HTOBJECT            19
#define HTCLOSE             20
#define HTHELP              21
#endif /* WINVER >= 0x0400 */

WINUSERAPI
UINT
WINAPI
RegisterWindowMessageA(
    LPCSTR lpString);
WINUSERAPI
UINT
WINAPI
RegisterWindowMessageW(
    LPCWSTR lpString);
#ifdef UNICODE
#define RegisterWindowMessage  RegisterWindowMessageW
#else
#define RegisterWindowMessage  RegisterWindowMessageA
#endif // !UNICODE


typedef struct tagCREATESTRUCTA {
    LPVOID      lpCreateParams;
    HINSTANCE   hInstance;
    HMENU       hMenu;
    HWND        hwndParent;
    int         cy;
    int         cx;
    int         y;
    int         x;
    LONG        style;
    LPCSTR      lpszName;
    LPCSTR      lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTA, *PCREATESTRUCTA, *LPCREATESTRUCTA;

typedef struct tagCREATESTRUCTW {
    LPVOID      lpCreateParams;
    HINSTANCE   hInstance;
    HMENU       hMenu;
    HWND        hwndParent;
    int         cy;
    int         cx;
    int         y;
    int         x;
    LONG        style;
    LPCWSTR     lpszName;
    LPCWSTR     lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTW, *PCREATESTRUCTW, *LPCREATESTRUCTW;

#ifdef UNICODE
typedef CREATESTRUCTW CREATESTRUCT;
typedef LPCREATESTRUCTW LPCREATESTRUCT;
typedef PCREATESTRUCTW LPCREATESTRUCT;
#else
typedef CREATESTRUCTA CREATESTRUCT;
typedef LPCREATESTRUCTA LPCREATESTRUCT;
typedef PCREATESTRUCTA LPCREATESTRUCT;
#endif // UNICODE

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF



#if(WINVER >= 0x0400)
#define EW_RESTARTWINDOWS    0x0042L
#define EW_REBOOTSYSTEM      0x0043L
#define EW_EXITANDEXECAPP    0x0044L
#endif /* WINVER >= 0x0400 */

#define EWX_LOGOFF   0
#define EWX_SHUTDOWN 1
#define EWX_REBOOT   2
#define EWX_FORCE    4
#define EWX_POWEROFF 8


/* MESSAGES */

// @CESYSGEN IF GWES_MSGQUE
// @CESYSGEN ENDIF

/* CAPTURE */


// @CESYSGEN IF GWES_FOREGND
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGWINMGR
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MENU
// @CESYSGEN IF GWES_COLUMN
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_LOADBMP
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_WMBASE
// @CESYSGEN IF GWES_ICONCURS
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MENU
// @CESYSGEN ENDIF


// @CESYSGEN IF COREDLL_ACCEL_C
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF

// @CESYSGEN IF COREDLL_RECTAPI
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_WMBASE
// @CESYSGEN IF COREDLL_WMGR_C
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_UIBASE
// @CESYSGEN ENDIF



// @CESYSGEN IF GWES_ACCEL
// @CESYSGEN ENDIF



// @CESYSGEN IF GWES_IDLE
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_DEFWNDPROC
// @CESYSGEN ENDIF




// @CESYSGEN IF GWES_GSETWINLONG
// @CESYSGEN ENDIF



// @CESYSGEN IF GWES_WINMGR
// @CESYSGEN ENDIF
/*
 * MessageBox() Flags
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#if(WINVER >= 0x0400)
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND
#endif /* WINVER >= 0x0400 */

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#if(WINVER >= 0x0400)
#define MB_DEFBUTTON4               0x00000300L
#endif /* WINVER >= 0x0400 */

#define MB_APPLMODAL                0x00000000L
#define MB_SETFOREGROUND            0x00010000L

#define MB_TOPMOST                  0x00040000L
#define MB_RTLREADING               0x00100000L

int
WINAPI
MessageBoxA(
    HWND hWnd ,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType);

int
WINAPI
MessageBoxW(
    HWND hWnd ,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType);
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif // !UNICODE

// @CESYSGEN IF GWES_MSGBEEP
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_CARET
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_FOREGND
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_KBDUI
// @CESYSGEN ENDIF



// @CESYSGEN IF GWES_STCCTL
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_BTNCTL
// @CESYSGEN ENDIF

#define SW_SCROLLCHILDREN   0x0001  /* Scroll children within *lprcScroll. */
#define SW_INVALIDATE       0x0002  /* Invalidate after scrolling */
#define SW_ERASE            0x0004  /* If SW_INVALIDATE, don't send WM_ERASEBACKGROUND */


#if(WINVER >= 0x0400)
typedef struct tagNMHDR
{
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;         // NM_ code
}   NMHDR;
typedef NMHDR FAR * LPNMHDR;
#endif

/*
 *  Owner draw control types
 */
// @CESYSGEN IF GWES_MENU
// @CESYSGEN ENDIF
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4

/*
 *  Owner draw actions
 */
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004

/*
 *  Owner draw state
 */
#define ODS_SELECTED    0x0001
#define ODS_GRAYED      0x0002
#define ODS_DISABLED    0x0004
#define ODS_CHECKED     0x0008
#define ODS_FOCUS       0x0010

/*
 * MEASUREITEMSTRUCT for ownerdraw
 */
typedef struct tagMEASUREITEMSTRUCT {
    UINT    CtlType;
    UINT    CtlID;
    UINT    itemID;
    UINT    itemWidth;
    UINT    itemHeight;
    UINT    itemData;
} MEASUREITEMSTRUCT, *PMEASUREITEMSTRUCT, *LPMEASUREITEMSTRUCT;

/*
 *  DRAWITEMSTRUCT for ownerdraw
 */
typedef struct tagDRAWITEMSTRUCT {
    UINT    CtlType;
    UINT    CtlID;
    UINT    itemID;
    UINT    itemAction;
    UINT    itemState;
    HWND    hwndItem;
    HDC     hDC;
    RECT    rcItem;
    DWORD   itemData;
} DRAWITEMSTRUCT, *PDRAWITEMSTRUCT, *LPDRAWITEMSTRUCT;

/*
 * DELETITEMSTRUCT for ownerdraw
 */
typedef struct tagDELETITEMSTRUCT {
    UINT    CtlType;
    UINT    CtlID;
    UINT    itemID;
    HWND    hwndItem;
    UINT    itemData;
} DELETEITEMSTRUCT, *PDELETEITEMSTRUCT, *LPDELETEITEMSTRUCT;

typedef struct tagCOMPAREITEMSTRUCT {
    UINT    CtlType;
    UINT    CtlID;
    HWND    hwndItem;
    UINT    itemID1;
    DWORD   itemData1;
    UINT    itemID2;
    DWORD   itemData2;
    DWORD   dwLocaleId;
} COMPAREITEMSTRUCT, *PCOMPAREITEMSTRUCT, *LPCOMPAREITEMSTRUCT;


// @CESYSGEN IF GWES_EDCTL
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_SBCMN
// @CESYSGEN ENDIF




// @CESYSGEN IF GWES_SCBCTL
// @CESYSGEN ENDIF

#ifndef NOKEYSTATES
/*
 * Key State Masks for Mouse Messages
 */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010

#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040

#endif /* !NOKEYSTATES */


/*
 *  Dialog Stuff
 */

/*
 * WARNING:
 * The following structures must NOT be DWORD padded because they are
 * followed by strings, etc that do not have to be DWORD aligned.
 */
#include "pshpack2.h"

/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#if(WINVER >= 0x0400)
#define IDCLOSE         8
#define IDHELP          9
#endif /* WINVER >= 0x0400 */

/*
 * Get/SetWindowWord/Long offsets for use with WC_DIALOG windows
 */
#define DWL_MSGRESULT   0
#define DWL_DLGPROC     4
#define DWL_USER        8

/*
 * original NT 32 bit dialog template:
 */
typedef struct {
    DWORD style;
    DWORD dwExtendedStyle;
    WORD cdit;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATE;
typedef DLGTEMPLATE *LPDLGTEMPLATEA;
typedef DLGTEMPLATE *LPDLGTEMPLATEW;
#ifdef UNICODE
typedef LPDLGTEMPLATEW LPDLGTEMPLATE;
#else
typedef LPDLGTEMPLATEA LPDLGTEMPLATE;
#endif // UNICODE
typedef CONST DLGTEMPLATE *LPCDLGTEMPLATEA;
typedef CONST DLGTEMPLATE *LPCDLGTEMPLATEW;
#ifdef UNICODE
typedef LPCDLGTEMPLATEW LPCDLGTEMPLATE;
#else
typedef LPCDLGTEMPLATEA LPCDLGTEMPLATE;
#endif // UNICODE

/*
 * 32 bit Dialog item template.
 */
typedef struct {
    DWORD style;
    DWORD dwExtendedStyle;
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
} DLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEA;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEW;
#ifdef UNICODE
typedef PDLGITEMTEMPLATEW PDLGITEMTEMPLATE;
#else
typedef PDLGITEMTEMPLATEA PDLGITEMTEMPLATE;
#endif // UNICODE
typedef DLGITEMTEMPLATE *LPDLGITEMTEMPLATEA;
typedef DLGITEMTEMPLATE *LPDLGITEMTEMPLATEW;
#ifdef UNICODE
typedef LPDLGITEMTEMPLATEW LPDLGITEMTEMPLATE;
#else
typedef LPDLGITEMTEMPLATEA LPDLGITEMTEMPLATE;
#endif // UNICODE

#include "poppack.h" /* Resume normal packing */

/*
 * Dialog Styles
 */
#define DS_LOCALEDIT        0x20L   /* Edit items get Local storage. */
#define DS_SETFONT          0x40L   /* User specified font for Dlg controls */
#define DS_MODALFRAME       0x80L   /* Can be combined with WS_CAPTION  */
#define DS_SETFOREGROUND    0x200L  /* not in Windows 3.1 */

#if(WINVER >= 0x0400)
#define DS_3DLOOK           0x0004L
#define DS_CONTROL          0x0400L
#define DS_CENTER           0x0800L
#define DS_NOCLOSEBTN		0x4000L

#endif /* WINVER >= 0x0400 */

#define DM_GETDEFID         (WM_USER+0)
#define DM_SETDEFID         (WM_USER+1)
// reserved value
#define DM_RESERVED0x2        (WM_USER+2)



/*
 * Returned in HIWORD() of DM_GETDEFID result if msg is supported
 */
#define DC_HASDEFID         0x534B

#define DLGC_WANTARROWS         0x0001
#define DLGC_WANTTAB            0x0002
#define DLGC_WANTALLKEYS        0x0004
#define DLGC_WANTMESSAGE        0x0004
#define DLGC_HASSETSEL      	0x0008
#define DLGC_DEFPUSHBUTTON      0x0010
#define DLGC_UNDEFPUSHBUTTON    0x0020
#define DLGC_RADIOBUTTON        0x0040
#define DLGC_WANTCHARS          0x0080
#define DLGC_STATIC             0x0100
#define DLGC_BUTTON             0x2000

WINUSERAPI
HWND
WINAPI
CreateDialogIndirectParamA(
    HINSTANCE hInstance,
    LPCDLGTEMPLATEA lpTemplate,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
WINUSERAPI
HWND
WINAPI
CreateDialogIndirectParamW(
    HINSTANCE hInstance,
    LPCDLGTEMPLATEW lpTemplate,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
#ifdef UNICODE
#define CreateDialogIndirectParam  CreateDialogIndirectParamW
#else
#define CreateDialogIndirectParam  CreateDialogIndirectParamA
#endif // !UNICODE


/*
	@func HWND | CreateDialogIndirect | Creates a modeless dialog box from a dialog box
	template resource.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPCDLGTEMPLATE | lpTemplate | Ptr to loaded dialog box template resource
    @parm HWND | hwndOwner | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
    @rdesc returns the window handle for the created dialog
	@comm Implemented as a macro in terms of <f CreateDialogIndirectParam>
*/
#define CreateDialogIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    CreateDialogIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define CreateDialogIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    CreateDialogIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define CreateDialogIndirect  CreateDialogIndirectW
#else
#define CreateDialogIndirect  CreateDialogIndirectA
#endif // !UNICODE

/*
	@func HWND | CreateDialogParam | Creates a modeless dialog box from a dialog box
	resource ID.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPSTR | lpTemplateName | Ptr to dialog box resource name
    @parm HWND | hwndOwner | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
    @parm LPARAM | dwInitParam | Initialization parameter
    @rdesc returns the window handle for the created dialog
	@comm Implemented as a macro in terms of <f CreateDialogIndirectParam>
*/
#define CreateDialogParamA(hInstance, lpszDlgName, hwndOwner, lpfnDialog, lParam)    \
    CreateDialogIndirectParamA(hInstance,   \
        (LPCDLGTEMPLATEA)LoadResource(hInstance, FindResource(hInstance, lpszDlgName, RT_DIALOG)), \
        hwndOwner, lpfnDialog, lParam)
#define CreateDialogParamW(hInstance, lpszDlgName, hwndOwner, lpfnDialog, lParam)    \
    CreateDialogIndirectParamW(hInstance,   \
        (LPCDLGTEMPLATEW)LoadResource(hInstance, FindResource(hInstance, lpszDlgName, RT_DIALOG)), \
        hwndOwner, lpfnDialog, lParam)
#ifdef UNICODE
#define CreateDialogParam  CreateDialogParamW
#else
#define CreateDialogParam  CreateDialogParamA
#endif // !UNICODE

/*
	@func HWND | CreateDialog | Creates a modeless dialog box from a dialog box
	resource ID.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPSTR | lpTemplateName | Ptr to dialog box resource name
    @parm HWND | hwndOwner | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
    @rdesc returns the window handle for the created dialog
	@comm Implemented as a macro in terms of <f CreateDialogIndirectParam>
*/
#define CreateDialogA(hInstance, lpName, hWndParent, lpDialogFunc) \
    CreateDialogParamA(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#define CreateDialogW(hInstance, lpName, hWndParent, lpDialogFunc) \
    CreateDialogParamW(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define CreateDialog  CreateDialogW
#else
#define CreateDialog  CreateDialogA
#endif // !UNICODE

int
WINAPI
DialogBoxIndirectParamA(
    HINSTANCE hInstance,
    LPCDLGTEMPLATEA hDialogTemplate,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
int
WINAPI
DialogBoxIndirectParamW(
    HINSTANCE hInstance,
    LPCDLGTEMPLATEW hDialogTemplate,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
#ifdef UNICODE
#define DialogBoxIndirectParam  DialogBoxIndirectParamW
#else
#define DialogBoxIndirectParam  DialogBoxIndirectParamA
#endif // !UNICODE

/*
	@func int | DialogBoxIndirect | Creates a modal dialog box from a dialog box
	template resource.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPCDLGTEMPLATE | lpTemplate | identifies loaded dialog box template resource
    @parm HWND | hwndParent | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
	@comm Implemented as a macro in terms of <f DialogBoxIndirectParam>
*/
#define DialogBoxIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    DialogBoxIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define DialogBoxIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    DialogBoxIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define DialogBoxIndirect  DialogBoxIndirectW
#else
#define DialogBoxIndirect  DialogBoxIndirectA
#endif // !UNICODE

/*
	@func int | DialogBoxParam | Creates a modal dialog box from a dialog box
	template resource.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPCTSTR | lpTemplateName | identifies dialog box template
    @parm HWND | hwndParent | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
    @parm LPARAM | dwInitParam | initialization value
	@comm Implemented as a macro in terms of <f DialogBoxIndirectParam>
*/
#define DialogBoxParamA(hInstance, lpszDlgName, hwndOwner, lpfnDialog, lParam)    \
    DialogBoxIndirectParamA(hInstance,   \
        (LPCDLGTEMPLATEA)LoadResource(hInstance, FindResource(hInstance, lpszDlgName, RT_DIALOG)), \
        hwndOwner, lpfnDialog, lParam)
#define DialogBoxParamW(hInstance, lpszDlgName, hwndOwner, lpfnDialog, lParam)    \
    DialogBoxIndirectParamW(hInstance,   \
        (LPCDLGTEMPLATEW)LoadResource(hInstance, FindResource(hInstance, lpszDlgName, RT_DIALOG)), \
        hwndOwner, lpfnDialog, lParam)
#ifdef UNICODE
#define DialogBoxParam  DialogBoxParamW
#else
#define DialogBoxParam  DialogBoxParamA
#endif // !UNICODE

/*
	@func int | DialogBox | Creates a modal dialog box from a dialog box
	template resource.
    @parm HINSTANCE | hInstance | handle of application instance
    @parm LPCTSTR | lpTemplateName | identifies dialog box template
    @parm HWND | hwndParent | handle of owner window
    @parm DLGPROC | lpDialogFunc | address of dialog box procedure
	@comm Implemented as a macro in terms of <f DialogBoxIndirectParam>
*/
#define DialogBoxA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    DialogBoxParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define DialogBoxW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
    DialogBoxParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define DialogBox  DialogBoxW
#else
#define DialogBox  DialogBoxA
#endif // !UNICODE

// @CESYSGEN IF GWES_DLGMGR
// @CESYSGEN ENDIF GWES_DLGMGR

// @CESYSGEN IF GWES_LBCTL
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_CMBCTL
// @CESYSGEN ENDIF


/*
 * Clipboard Manager Functions
 */

// @CESYSGEN IF GWES_CLIPBD
// @CESYSGEN ENDIF

/* 3D Border Styles */
#define BDR_RAISEDOUTER     0x0001
#define BDR_SUNKENOUTER     0x0002
#define BDR_RAISEDINNER     0x0004
#define BDR_SUNKENINNER     0x0008

#define BDR_OUTER           0x0003
#define BDR_INNER           0x000C
#define BDR_RAISED          0x0005
#define BDR_SUNKEN          0x000A

#define EDGE_RAISED         (BDR_RAISEDOUTER | BDR_RAISEDINNER)
#define EDGE_SUNKEN         (BDR_SUNKENOUTER | BDR_SUNKENINNER)
#define EDGE_ETCHED         (BDR_SUNKENOUTER | BDR_RAISEDINNER)
#define EDGE_BUMP           (BDR_RAISEDOUTER | BDR_SUNKENINNER)

/* Border Flags */
#define BF_LEFT             0x0001
#define BF_TOP              0x0002
#define BF_RIGHT            0x0004
#define BF_BOTTOM           0x0008

#define BF_DIAGONAL     	0x0010

#define BF_DIAGONAL_ENDTOPRIGHT     (BF_DIAGONAL | BF_TOP | BF_RIGHT)
#define BF_DIAGONAL_ENDTOPLEFT      (BF_DIAGONAL | BF_TOP | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMLEFT   (BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMRIGHT  (BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)

#define BF_TOPLEFT          (BF_TOP | BF_LEFT)
#define BF_TOPRIGHT         (BF_TOP | BF_RIGHT)
#define BF_BOTTOMLEFT       (BF_BOTTOM | BF_LEFT)
#define BF_BOTTOMRIGHT      (BF_BOTTOM | BF_RIGHT)
#define BF_RECT             (BF_LEFT | BF_RIGHT | BF_TOP | BF_BOTTOM)

#define BF_MIDDLE   	    0x0800

#define BF_SOFT             0x1000
#define BF_ADJUST           0x2000
#define BF_FLAT             0x4000
#define BF_MONO             0x8000


/*
 *  Color Types
 */
#define CTLCOLOR_MSGBOX         0
#define CTLCOLOR_EDIT           1
#define CTLCOLOR_LISTBOX        2
#define CTLCOLOR_BTN            3
#define CTLCOLOR_SCOLLBAR       5
#define CTLCOLOR_STATIC         6
#define CTLCOLOR_MAX            7



// @CESYSGEN IF GWES_CURSOR || GWES_MCURSOR
// @CESYSGEN ENDIF







// @CESYSGEN IF GWES_TIMER
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_KBDUI
// @CESYSGEN IF GWES_HOTKEY
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF


#define SPIF_UPDATEINIFILE	0x0001
#define SPIF_SENDCHANGE		0x0002

// @CESYSGEN IF GWES_CURSOR || GWES_MCURSOR
// @CESYSGEN ENDIF

#define SPI_SETDESKWALLPAPER       20
#define SPI_SETDESKPATTERN         21

#define SPI_SETWORKAREA            47
#define SPI_GETWORKAREA            48

// @CESYSGEN IF GWES_ACCESSIB
// @CESYSGEN ENDIF

#define SPI_GETSHOWSOUNDS          56
#define SPI_SETSHOWSOUNDS          57

#define SPI_GETDEFAULTINPUTLANG    89

#define SPI_SETLANGTOGGLE          91

#define SPI_GETWHEELSCROLLLINES   104
#define SPI_SETWHEELSCROLLLINES   105

#define SPI_GETFONTSMOOTHINGCONTRAST 0x200C
#define SPI_SETFONTSMOOTHINGCONTRAST 0x200D

#define SPI_GETFONTSMOOTHING         0x004A
#define SPI_SETFONTSMOOTHING         0x004B

// @CESYSGEN IF GWES_IDLE
// @CESYSGEN ENDIF

//The following flags also used with WM_SETTINGCHANGE
// so don't use the values for future SPI_*

//#define SETTINGCHANGE_START  		0x3001
//#define SETTINGCHANGE_RESET  		0x3002
//#define SETTINGCHANGE_END    		0x3003

#define SPI_GETPLATFORMTYPE 257
#define SPI_GETOEMINFO 258

WINUSERAPI
BOOL
WINAPI
SystemParametersInfoA(
    UINT uiAction,
    UINT uiParam,
    PVOID pvParam,
    UINT fWinIni);

WINUSERAPI
BOOL
WINAPI
SystemParametersInfoW(
    UINT uiAction,
    UINT uiParam,
    PVOID pvParam,
    UINT fWinIni);
#ifdef UNICODE
#define SystemParametersInfo  SystemParametersInfoW
#else
#define SystemParametersInfo  SystemParametersInfoA
#endif // !UNICODE


// @CESYSGEN IF GWES_ACCESSIB
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_DEFWNDPROC
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_ICONCMN
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_ICON
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_LOADIMG
// @CESYSGEN IF GWES_LOADBMP
// @CESYSGEN ENDIF
// @CESYSGEN IF GWES_ICONCMN
// @CESYSGEN ENDIF
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_ICONCURS
// @CESYSGEN ENDIF



// MultiMonitor API
#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif

#define MONITOR_DEFAULTTONULL              0x00000000
#define MONITOR_DEFAULTTOPRIMARY           0x00000001
#define MONITOR_DEFAULTTONEAREST           0x00000002

HMONITOR
WINAPI
MonitorFromPoint(
    IN POINT pt,
    IN DWORD dwFlags);

HMONITOR
WINAPI
MonitorFromRect(
    IN LPCRECT lprc,
    IN DWORD dwFlags);

HMONITOR
WINAPI
MonitorFromWindow( IN HWND hwnd, IN DWORD dwFlags);

#define MONITORINFOF_PRIMARY        0x00000001

#ifndef CCHDEVICENAME
#define CCHDEVICENAME 32
#endif

typedef struct tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;

#ifdef __cplusplus
typedef struct tagMONITORINFOEX : public tagMONITORINFO
{
      TCHAR        szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#else // ndef __cplusplus
typedef struct tagMONITORINFOEX
{
    MONITORINFO;
	TCHAR        szDevice[CCHDEVICENAME];
} MONITORINFOEX, *LPMONITORINFOEX;
#endif

BOOL
WINAPI
GetMonitorInfo(
    IN HMONITOR hMonitor,
    IN LPMONITORINFO lpmi);


typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);


BOOL
WINAPI
EnumDisplayMonitors(
    IN HDC             hdc,
    IN LPCRECT         lprcClip,
    IN MONITORENUMPROC lpfnEnum,
    IN LPARAM          dwData);


// @CESYSGEN IF GWES_MGBASE
// @CESYSGEN ENDIF GWES_MGBASE

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#ifdef WINCEOEM
#include <pwinuser.h>	// internal defines
#endif


#endif /* _WINUSER_ */
