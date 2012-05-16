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

// @CESYSGEN IF GWES_ICONCMN
#define IMAGE_ICON          1
#define IMAGE_CURSOR		2
// @CESYSGEN ENDIF

/* Flags for SetWindowPos API */
#define HWND_TOP        ((HWND)0)
#define HWND_TOPMOST    ((HWND)-1)
#define HWND_NOTOPMOST    ((HWND)-2)
#define HWND_BOTTOM     ((HWND)1)

#define SWP_NOSIZE			0x0001
#define SWP_NOMOVE			0x0002
#define SWP_NOZORDER		0x0004
#define SWP_NOACTIVATE		0x0010
#define SWP_FRAMECHANGED	0x0020  /* The frame changed: send WM_NCCALCSIZE */
#define SWP_SHOWWINDOW		0x0040
#define SWP_HIDEWINDOW		0x0080
#define SWP_NOOWNERZORDER   0x0200  /* Don't do owner Z ordering */
#define SWP_DRAWFRAME       SWP_FRAMECHANGED
#define SWP_NOREPOSITION    SWP_NOOWNERZORDER
#define SWP_NOSTARTUP		0x04000000
#define SWP_STARTUP			0x08000000

BOOL
WINAPI
SetWindowTextA(
    HWND hWnd,
    LPCSTR lpString);

BOOL
WINAPI
SetWindowTextW(
    HWND hWnd,
    LPCWSTR lpString);
#ifdef UNICODE
#define SetWindowText  SetWindowTextW
#else
#define SetWindowText  SetWindowTextA
#endif // !UNICODE

int
WINAPI
GetWindowTextA(
    HWND hWnd,
    LPSTR lpString,
    int nMaxCount);

int
WINAPI
GetWindowTextW(
    HWND hWnd,
    __out_ecount(nMaxCount) LPWSTR lpString,
    int nMaxCount);
#ifdef UNICODE
#define GetWindowText  GetWindowTextW
#else
#define GetWindowText  GetWindowTextA
#endif // !UNICODE

/* Flags for GetWindow() api */
#define GW_HWNDFIRST        0
#define GW_HWNDLAST         1
#define GW_HWNDNEXT         2
#define GW_HWNDPREV         3
#define GW_OWNER            4
#define GW_CHILD            5
#define GW_MAX              5

BOOL
WINAPI
SetWindowPos (
    HWND hwnd,
    HWND hwndInsertAfter,
    int x,
    int y,
    int dx,
    int dy,
    UINT fuFlags
    );

BOOL
WINAPI
GetWindowRect (
    HWND hwnd,
    LPRECT prc
    );

int
WINAPI
GetWindowTextLengthA(
    HWND hWnd);

int
WINAPI
GetWindowTextLengthW(
    HWND hWnd);
#ifdef UNICODE
#define GetWindowTextLength  GetWindowTextLengthW
#else
#define GetWindowTextLength  GetWindowTextLengthA
#endif


HANDLE
WINAPI
LoadImageA(
    HINSTANCE,
    LPCSTR,
    UINT,
    int,
    int,
    UINT);

HANDLE
WINAPI
LoadImageW(
    HINSTANCE,
    LPCWSTR,
    UINT,
    int,
    int,
    UINT);
#ifdef UNICODE
#define LoadImage  LoadImageW
#else
#define LoadImage  LoadImageA
#endif // !UNICODE

/*
 * Special HWND value for use with PostMessage() and SendMessage()
 */
#define HWND_BROADCAST  ((HWND)0xffff)

WINUSERAPI
BOOL
WINAPI
PostMessageA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
PostMessageW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
#ifdef UNICODE
#define PostMessage  PostMessageW
#else
#define PostMessage  PostMessageA
#endif // !UNICODE


#define CW_USEDEFAULT       ((int)0x80000000)

HWND
WINAPI
CreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent ,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);

HWND
WINAPI
CreateWindowExW(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent ,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam);


BOOL
WINAPI
ShowWindow (
    HWND hwnd,
    INT nCmdShow
    );

BOOL
WINAPI
AdjustWindowRectEx (
    LPRECT prc,
    DWORD  dwStyle,
    BOOL   bMenu,
    DWORD  dwExStyle
    );

// @CESYSGEN IF GWES_FOREGND
HWND
WINAPI
SetActiveWindow(
	HWND	hWnd);

HWND
WINAPI
GetForegroundWindow(
	VOID);

BOOL
WINAPI
SetForegroundWindow(
	HWND hWnd);


HWND
WINAPI
SetFocus(
    HWND hWnd
    );

HWND
WINAPI
GetActiveWindow(
    VOID
    );

HWND
WINAPI
GetFocus(
    VOID
    );


// @CESYSGEN ENDIF

HWND
WINAPI
SetParent (
    HWND hwnd,
    HWND hwndParent
    );

HWND
WINAPI
GetParent (
    HWND hwnd
    );


BOOL
WINAPI
InvalidateRect (
    HWND hwnd,
    LPCRECT prc,
    BOOL fErase
    );

BOOL
WINAPI
ClientToScreen(
    HWND hWnd,
    LPPOINT lpPoint);

/*
 * Window Styles
 */
#ifdef UNDER_NT
#define WS_OVERLAPPED       0x00000000L
#else
// @CESYSGEN IF GWES_NCLIENT
#define WS_OVERLAPPED       WS_BORDER | WS_CAPTION
// @CESYSGEN ENDIF
#endif // UNDER_NT
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
// @CESYSGEN IF GWES_NCLIENT
#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME		0x00040000L
#define WS_MAXIMIZEBOX		0x00020000L
#define WS_MINIMIZEBOX		0x00010000L
#define WS_SIZEBOX          WS_THICKFRAME
// @CESYSGEN ENDIF
#define WS_POPUP            0x80000000L

#define WS_CHILD            0x40000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L


#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L

#define WS_EX_NOACTIVATE    0x08000000L


typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);
typedef VOID (CALLBACK* TIMERPROC)(HWND, UINT, UINT, DWORD);

// @CESYSGEN IF GWES_DEFWNDPROC

LRESULT
WINAPI
DefWindowProcA(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);

LRESULT
WINAPI
DefWindowProcW(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
#ifdef UNICODE
#define DefWindowProc  DefWindowProcW
#else
#define DefWindowProc  DefWindowProcA
#endif // !UNICODE

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

// @CESYSGEN IF COREDLL_WMGR_C
/* RegisterClass */

ATOM
WINAPI
RegisterClassA(
    CONST WNDCLASSA *lpWndClass
    );

ATOM
WINAPI
RegisterClassW (
    CONST WNDCLASSW *lpWndClass
    );

#ifdef UNICODE
#define RegisterClass  RegisterClassW
#else
#define RegisterClass  RegisterClassA
#endif // !UNICODE
// @CESYSGEN ENDIF


/*
 * Color Types
 */
#ifdef UNDER_NT
#define SYS_COLOR_INDEX_FLAG        0
#else
#define SYS_COLOR_INDEX_FLAG        0x40000000
#endif // UNDER_NT
#define COLOR_WINDOW              ( 5 | SYS_COLOR_INDEX_FLAG)

HICON
WINAPI
LoadIconA(
    HINSTANCE hInstance,
    LPCSTR lpIconName);

HICON
WINAPI
LoadIconW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName);
#ifdef UNICODE
#define LoadIcon  LoadIconW
#else
#define LoadIcon  LoadIconA
#endif // !UNICODE

/*
 * Class styles
 */
#define CS_DBLCLKS          0x0008
#define CS_GLOBALCLASS      0x4000

#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_DBLCLKS          0x0008
#define CS_PARENTDC         0x0080
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800
#define CS_GLOBALCLASS      0x4000
#define CS_IME              0x00010000

/* Value for rolling one detent */
#define WHEEL_DELTA                     120
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

/* Setting to scroll one page for SPI_GET/SETWHEELSCROLLLINES */
#define WHEEL_PAGESCROLL                (UINT_MAX)


SHORT
WINAPI
GetKeyState(
    int nVirtKey);


typedef struct _PAINTSTRUCT {
    HDC     hdc;                /* display DC to be used for painting */
    BOOL    fErase;             /* Indicates whether background needs erased */
    RECT    rcPaint;            /* Rectangle where painting required */
    BOOL    fRestore;           /* RESERVED */
    BOOL    fIncUpdate;         /* RESERVED */
    BYTE    rgbReserved[32];    /* RESERVED */
} PAINTSTRUCT;

typedef PAINTSTRUCT *LPPAINTSTRUCT;
typedef PAINTSTRUCT *PPAINTSTRUCT;
typedef const PAINTSTRUCT *LPCPAINTSTRUCT;
typedef const PAINTSTRUCT *PCPAINTSTRUCT;

HDC
WINAPI
BeginPaint (
    HWND hwnd,
    LPPAINTSTRUCT pps
    );

BOOL
WINAPI
EndPaint (
    HWND hwnd,
    LPPAINTSTRUCT pps
    );

BOOL
WINAPI
ScreenToClient(
    HWND hWnd,
    LPPOINT lpPoint);

BOOL
WINAPI
GetClientRect (
    HWND hwnd,
    LPRECT prc
    );

BOOL
WINAPI
UpdateWindow (
    HWND hwnd
    );

BOOL
WINAPI
DestroyWindow (
    HWND hwnd
    );


#define IDC_ARROW           MAKEINTRESOURCE(32512) // Arrow cursor for emulation

HDC
WINAPI
GetDC (
    HWND hwnd
    );

HDC
WINAPI
GetWindowDC (
    HWND hwnd
    );

int
WINAPI
ReleaseDC (
    HWND hwnd,
    HDC hdc
    );

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

// @CESYSGEN IF GWES_TIMER
UINT WINAPI SetTimer(
    HWND hwnd,
    UINT idTimer,
    UINT uTimeOut,
    TIMERPROC pfnTimerProc);

BOOL WINAPI KillTimer(
    HWND hwnd,
    UINT idEvent);
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_WINMGR
#ifdef UNDER_NT

#ifndef UNICODE_ONLY
WINUSERAPI
HCURSOR
WINAPI
LoadCursorA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName);
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINUSERAPI
HCURSOR
WINAPI
LoadCursorW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName);
#endif //!ANSI_ONLY
#ifdef UNICODE
#define LoadCursor  LoadCursorW
#else
#define LoadCursor  LoadCursorA
#endif // !UNICODE

#define IDC_ARROW           MAKEINTRESOURCE(32512) // Arrow cursor for emulation

#else // UNDER_NT

WINUSERAPI
HCURSOR
WINAPI
LoadCursorA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName);
WINUSERAPI
HCURSOR
WINAPI
LoadCursorW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName);
#undef LoadCursor
#ifdef UNICODE
#define LoadCursor  LoadCursorW
#else
#define LoadCursor  LoadCursorA
#endif // !UNICODE


#endif // UNDER_NT

/*
 * Standard Cursor IDs
 */
#define IDC_ARROW           MAKEINTRESOURCE(32512)
#define IDC_IBEAM           MAKEINTRESOURCE(32513)
#define IDC_WAIT            MAKEINTRESOURCE(32514)
#define IDC_CROSS           MAKEINTRESOURCE(32515)
#define IDC_UPARROW         MAKEINTRESOURCE(32516)
#define IDC_SIZE            MAKEINTRESOURCE(32646)
#define IDC_ICON            MAKEINTRESOURCE(32512)
#define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
#define IDC_SIZENESW        MAKEINTRESOURCE(32643)
#define IDC_SIZEWE          MAKEINTRESOURCE(32644)
#define IDC_SIZENS          MAKEINTRESOURCE(32645)
#define IDC_SIZEALL         MAKEINTRESOURCE(32646)
#define IDC_NO              MAKEINTRESOURCE(32648)
#define IDC_APPSTARTING     MAKEINTRESOURCE(32650)
#define IDC_HELP            MAKEINTRESOURCE(32651)
#define IDC_HAND			MAKEINTRESOURCE(32649)

WINUSERAPI
int
WINAPI
ShowCursor(
    BOOL bShow);

WINUSERAPI
BOOL
WINAPI
SetCursorPos(
    int X,
    int Y);

WINUSERAPI
HCURSOR
WINAPI
SetCursor(
    HCURSOR hCursor);

WINUSERAPI
BOOL
WINAPI
GetCursorPos(
    LPPOINT lpPoint);

WINUSERAPI
BOOL
WINAPI
ClipCursor(
    CONST RECT *lpRect);

WINUSERAPI
BOOL
WINAPI
GetClipCursor(
    LPRECT lpRect);

WINUSERAPI
HCURSOR
WINAPI
GetCursor(
    VOID);

WINUSERAPI
HCURSOR
WINAPI
LoadAnimatedCursor(
    HINSTANCE	hInstance,
    DWORD		ResourceId,
	int			cFrames,
	int			FrameTimeInterval
	);

// @CESYSGEN ENDIF


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
#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002

#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005

#define WM_CLOSE                        0x0010
#define WM_QUIT                         0x0012


#define WM_COPYDATA                     0x004A

#define WM_ACTIVATE                     0x0006
/*
 * WM_ACTIVATE state values
 */
#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008

#define WM_PAINT                        0x000F

#define WM_ERASEBKGND                   0x0014

#define WM_SHOWWINDOW                   0x0018

// @CESYSGEN IF GWES_CURSOR || GWES_MCURSOR
#define WM_SETCURSOR                    0x0020
// @CESYSGEN ENDIF

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

// @CESYSGEN IF GWES_WINMGR
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_KBDUI
#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107
#define WM_KEYLAST                      0x0108

#define WM_IM_INFO			0x010C

#define WM_IME_STARTCOMPOSITION			0x010D
#define WM_IME_ENDCOMPOSITION			0x010E
#define WM_IME_COMPOSITION				0x010F
#define WM_IME_KEYLAST					0x010F

#define WM_IME_SETCONTEXT				0x0281
#define WM_IME_NOTIFY					0x0282
#define WM_IME_CONTROL					0x0283
#define WM_IME_COMPOSITIONFULL			0x0284
#define WM_IME_SELECT					0x0285
#define WM_IME_CHAR						0x0286
#define WM_IME_SYSTEM					0x0287
#define WM_IME_REQUEST                  0x0288
#define WM_IME_KEYDOWN					0x0290
#define WM_IME_KEYUP					0x0291

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

#define WM_CLOSE                        0x0010
#define WM_QUIT                         0x0012

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

/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */

#define VK_BACK           0x08
#define VK_TAB            0x09

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define	VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define	VK_KANJI	      0x19

#define VK_ESCAPE         0x1B

#define	VK_CONVERT	     0x1c
#define	VK_NOCONVERT	  0x1d

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

/* VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D

#define VK_SLEEP          0x5F

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#define	VK_EXTEND_BSLASH  0xE2
#define	VK_OEM_102        0xE2

#define VK_PROCESSKEY     0xE5

#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_PA1            0xFD
#define VK_OEM_CLEAR      0xFE


#define VK_SEMICOLON		0xBA
#define VK_EQUAL			0xBB
#define VK_COMMA			0xBC
#define VK_HYPHEN			0xBD
#define VK_PERIOD			0xBE
#define VK_SLASH			0xBF
#define VK_BACKQUOTE		0xC0

#define VK_BROWSER_BACK                  0xA6
#define VK_BROWSER_FORWARD               0xA7
#define VK_BROWSER_REFRESH               0xA8
#define VK_BROWSER_STOP                  0xA9
#define VK_BROWSER_SEARCH                0xAA
#define VK_BROWSER_FAVORITES             0xAB
#define VK_BROWSER_HOME                  0xAC
#define VK_VOLUME_MUTE                   0xAD
#define VK_VOLUME_DOWN                   0xAE
#define VK_VOLUME_UP                     0xAF
#define VK_MEDIA_NEXT_TRACK              0xB0
#define VK_MEDIA_PREV_TRACK              0xB1
#define VK_MEDIA_STOP                    0xB2
#define VK_MEDIA_PLAY_PAUSE              0xB3
#define VK_LAUNCH_MAIL                   0xB4
#define VK_LAUNCH_MEDIA_SELECT           0xB5
#define VK_LAUNCH_APP1                   0xB6
#define VK_LAUNCH_APP2                   0xB7

#define VK_LBRACKET			0xDB
#define VK_BACKSLASH		0xDC
#define VK_RBRACKET			0xDD
#define VK_APOSTROPHE		0xDE
#define VK_OFF              0xDF



#define VK_DBE_ALPHANUMERIC              0x0f0
#define VK_DBE_KATAKANA                  0x0f1
#define VK_DBE_HIRAGANA                  0x0f2
#define VK_DBE_SBCSCHAR                  0x0f3
#define VK_DBE_DBCSCHAR                  0x0f4
#define VK_DBE_ROMAN                     0x0f5
#define VK_DBE_NOROMAN                   0x0f6
#define VK_DBE_ENTERWORDREGISTERMODE     0x0f7
#define VK_DBE_ENTERIMECONFIGMODE        0x0f8
#define VK_DBE_FLUSHSTRING               0x0f9
#define VK_DBE_CODEINPUT                 0x0fa
#define VK_DBE_NOCODEINPUT               0x0fb
#define VK_DBE_DETERMINESTRING           0x0fc
#define VK_DBE_ENTERDLGCONVERSIONMODE    0x0fd

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
BOOL
WINAPI
PostThreadMessageA(
    DWORD idThread,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
PostThreadMessageW(
    DWORD idThread,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam);
#ifdef UNICODE
#define PostThreadMessage  PostThreadMessageW
#else
#define PostThreadMessage  PostThreadMessageA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
PeekMessageW(
    PMSG pMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg);
WINUSERAPI
BOOL
WINAPI
PeekMessageA(
    PMSG pMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax,
    UINT wRemoveMsg);
#ifdef UNICODE
#define PeekMessage  PeekMessageW
#else
#define PeekMessage  PeekMessageA
#endif // !UNICODE

/*
 * PeekMessage() Options
 */
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002

WINUSERAPI
BOOL
WINAPI
GetMessageA(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax);
WINUSERAPI
BOOL
WINAPI
GetMessageW(
    LPMSG lpMsg,
    HWND hWnd ,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax);
#ifdef UNICODE
#define GetMessage  GetMessageW
#else
#define GetMessage  GetMessageA
#endif // !UNICODE

BOOL
WINAPI
TranslateMessage(
    CONST MSG   *pMsg
    );

WINUSERAPI
LONG
WINAPI
DispatchMessageA(
    CONST MSG *lpMsg);
WINUSERAPI
LONG
WINAPI
DispatchMessageW(
    CONST MSG *lpMsg);
#ifdef UNICODE
#define DispatchMessage  DispatchMessageW
#else
#define DispatchMessage  DispatchMessageA
#endif // !UNICODE

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
typedef struct _DISPLAY_DEVICE
{
    DWORD   cb;
    TCHAR   DeviceName[32];
    TCHAR   DeviceString[128];
    DWORD   StateFlags;
    TCHAR   DeviceID[128];
    TCHAR   DeviceKey[128];
} DISPLAY_DEVICE, *PDISPLAY_DEVICE, *LPDISPLAY_DEVICE;


BOOL
WINAPI
EnumDisplaySettings(
    IN  LPCTSTR    lpszDeviceName,
    IN  DWORD      iModeNum,
    OUT LPDEVMODEW lpDevMode);

BOOL
WINAPI
EnumDisplayDevices(
    IN  LPCTSTR         lpDevice,
    IN  DWORD           iDevNum,
    OUT PDISPLAY_DEVICE lpDisplayDevice,
    IN  DWORD           dwFlags);


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



LONG
WINAPI
ChangeDisplaySettingsEx(
	IN LPCTSTR         lpszDeviceName,
	IN LPDEVMODE       lpDevMode,
	IN HWND            hwnd,
	IN DWORD           dwflags,
	LPVOID             lParam
	);


/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5
#if(_WIN32_WINNT >= 0x0501)
#define DISP_CHANGE_BADDUALVIEW     -6
#endif /* _WIN32_WINNT >= 0x0501 */

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


// @CESYSGEN IF GWES_GSETWINLONG
/*
 *  Window field offsets for GetWindowLong()
 */
#define GWL_WNDPROC         (-4)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)


LONG
WINAPI
GetWindowLongA(
    HWND hWnd,
    int nIndex);

LONG
WINAPI
GetWindowLongW(
    HWND hWnd,
    int nIndex);
#ifdef UNICODE
#define GetWindowLong  GetWindowLongW
#else
#define GetWindowLong  GetWindowLongA
#endif // !UNICODE


LONG
WINAPI
SetWindowLongA(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);

LONG
WINAPI
SetWindowLongW(
    HWND hWnd,
    int nIndex,
    LONG dwNewLong);
#ifdef UNICODE
#define SetWindowLong  SetWindowLongW
#else
#define SetWindowLong  SetWindowLongA
#endif // !UNICODE

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


#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWNA           8
#define SW_SHOWMAXIMIZED	11
#define SW_MAXIMIZE			12
#define SW_RESTORE			13

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
#define ENUM_CURRENT_SETTINGS     ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS    ((DWORD)-2)

// @CESYSGEN ENDIF GWES_MGBASE

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#ifdef WINCEOEM
#include <pwinuser.h>	// internal defines
#endif


#endif /* _WINUSER_ */
