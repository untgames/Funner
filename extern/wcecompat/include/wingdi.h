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
/*++


Module Name: wingdi.h

--*/
#ifndef _WINGDI_H_
#define _WINGDI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <windef.h>

#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

/* field selection bits */
#define DM_ORIENTATION      0x00000001L
#define DM_PAPERSIZE        0x00000002L
#define DM_PAPERLENGTH      0x00000004L
#define DM_PAPERWIDTH       0x00000008L
#define DM_SCALE            0x00000010L
#define DM_COPIES           0x00000100L
#define DM_DEFAULTSOURCE    0x00000200L
#define DM_PRINTQUALITY     0x00000400L
#define DM_COLOR            0x00000800L
#define DM_DUPLEX           0x00001000L
#define DM_YRESOLUTION      0x00002000L
#define DM_TTOPTION         0x00004000L
#define DM_COLLATE          0x00008000L
#define DM_FORMNAME         0x00010000L
#define DM_LOGPIXELS        0x00020000L
#define DM_BITSPERPEL       0x00040000L
#define DM_PELSWIDTH        0x00080000L
#define DM_PELSHEIGHT       0x00100000L
#define DM_DISPLAYFLAGS     0x00200000L
#define DM_DISPLAYFREQUENCY 0x00400000L
#define DM_DISPLAYORIENTATION 0x00800000L
#define DM_DISPLAYQUERYORIENTATION 0x01000000L


#define WINGDIAPI

/* size of a device name string */
#define CCHDEVICENAME 32

/* size of a form name string */
#define CCHFORMNAME 32

typedef struct _devicemodeA {
    BYTE   dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    short dmOrientation;
    short dmPaperSize;
    short dmPaperLength;
    short dmPaperWidth;
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    BYTE   dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    DWORD  dmDisplayFlags;
    DWORD  dmDisplayFrequency;
    DWORD  dmICMMethod;
    DWORD  dmICMIntent;
    DWORD  dmMediaType;
    DWORD  dmDitherType;
    DWORD  dmICCManufacturer;
    DWORD  dmICCModel;
    DWORD  dmPanningWidth;
    DWORD  dmPanningHeight;
    DWORD  dmDisplayOrientation;
} DEVMODEA, *PDEVMODEA, *NPDEVMODEA, *LPDEVMODEA;

typedef struct _devicemodeW {
    WCHAR  dmDeviceName[CCHDEVICENAME];
    WORD dmSpecVersion;
    WORD dmDriverVersion;
    WORD dmSize;
    WORD dmDriverExtra;
    DWORD dmFields;
    short dmOrientation;
    short dmPaperSize;
    short dmPaperLength;
    short dmPaperWidth;
    short dmScale;
    short dmCopies;
    short dmDefaultSource;
    short dmPrintQuality;
    short dmColor;
    short dmDuplex;
    short dmYResolution;
    short dmTTOption;
    short dmCollate;
    WCHAR  dmFormName[CCHFORMNAME];
    WORD   dmLogPixels;
    DWORD  dmBitsPerPel;
    DWORD  dmPelsWidth;
    DWORD  dmPelsHeight;
    DWORD  dmDisplayFlags;
    DWORD  dmDisplayFrequency;
    DWORD  dmDisplayOrientation;
// #if (WINVER >= 0x0400)
//     DWORD  dmICMMethod;
//     DWORD  dmICMIntent;
//     DWORD  dmMediaType;
//     DWORD  dmDitherType;
//     DWORD  dmReserved1;
//     DWORD  dmReserved2;
// #endif /* WINVER */
} DEVMODEW, *PDEVMODEW, *NPDEVMODEW, *LPDEVMODEW;

#ifdef UNICODE
typedef DEVMODEW DEVMODE;
typedef PDEVMODEW PDEVMODE;
typedef NPDEVMODEW NPDEVMODE;
typedef LPDEVMODEW LPDEVMODE;
#else
typedef DEVMODEA DEVMODE;
typedef PDEVMODEA PDEVMODE;
typedef NPDEVMODEA NPDEVMODE;
typedef LPDEVMODEA LPDEVMODE;
#endif


typedef struct tagPALETTEENTRY {
    BYTE        peRed;
    BYTE        peGreen;
    BYTE        peBlue;
    BYTE        peFlags;
} PALETTEENTRY, *PPALETTEENTRY, FAR *LPPALETTEENTRY;

/* Logical Palette */
typedef struct tagLOGPALETTE {
    WORD        palVersion;
    WORD        palNumEntries;
    PALETTEENTRY        palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, NEAR *NPLOGPALETTE, FAR *LPLOGPALETTE;


#define TRUETYPE_FONTTYPE       0x0004  
#define LF_FACESIZE				32

typedef struct tagLOGFONTA
{
    LONG      lfHeight;
    LONG      lfWidth;
    LONG      lfEscapement;
    LONG      lfOrientation;
    LONG      lfWeight;
    BYTE      lfItalic;
    BYTE      lfUnderline;
    BYTE      lfStrikeOut;
    BYTE      lfCharSet;
    BYTE      lfOutPrecision;
    BYTE      lfClipPrecision;
    BYTE      lfQuality;
    BYTE      lfPitchAndFamily;
    CHAR      lfFaceName[LF_FACESIZE];
} LOGFONTA, *PLOGFONTA, NEAR *NPLOGFONTA, FAR *LPLOGFONTA;

typedef struct tagLOGFONTW
{
    LONG      lfHeight;
    LONG      lfWidth;
    LONG      lfEscapement;
    LONG      lfOrientation;
    LONG      lfWeight;
    BYTE      lfItalic;
    BYTE      lfUnderline;
    BYTE      lfStrikeOut;
    BYTE      lfCharSet;
    BYTE      lfOutPrecision;
    BYTE      lfClipPrecision;
    BYTE      lfQuality;
    BYTE      lfPitchAndFamily;
    WCHAR     lfFaceName[LF_FACESIZE];
} LOGFONTW, *PLOGFONTW, NEAR *NPLOGFONTW, FAR *LPLOGFONTW;


#define WINUSERAPI


// @CESYSGEN IF GWES_PGDI || GWES_MGBASE
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_DIBPATTERNPT     6

#define PS_SOLID            0
#define PS_DASH             1
#define PS_NULL             5

typedef struct tagLOGBRUSH {
    UINT        lbStyle;
    COLORREF    lbColor;
    LONG        lbHatch;
} LOGBRUSH, *PLOGBRUSH, NEAR *NPLOGBRUSH, FAR *LPLOGBRUSH;

typedef struct tagLOGPEN {
    UINT        lopnStyle;
    POINT       lopnWidth;
    COLORREF    lopnColor;
} LOGPEN, *PLOGPEN, NEAR *NPLOGPEN, FAR *LPLOGPEN;

WINGDIAPI HBRUSH   WINAPI CreateDIBPatternBrushPt(CONST VOID *,UINT);
WINGDIAPI HPEN     WINAPI CreatePenIndirect(CONST LOGPEN *);
WINGDIAPI HBRUSH   WINAPI CreateSolidBrush(COLORREF);
WINGDIAPI BOOL     WINAPI DrawEdge(HDC,RECT *,UINT,UINT);
WINGDIAPI BOOL     WINAPI DrawFocusRect(HDC,CONST RECT *);
WINGDIAPI BOOL     WINAPI Ellipse(HDC,int,int,int,int);
WINGDIAPI int      WINAPI FillRect(HDC,CONST RECT *,HBRUSH);
WINGDIAPI COLORREF WINAPI GetPixel(HDC,int,int);
WINGDIAPI HBRUSH   WINAPI GetSysColorBrush(int);
WINGDIAPI BOOL     WINAPI Polygon( __in HDC hdc, __in_ecount(c) CONST POINT * lppt, int c);
WINGDIAPI BOOL     WINAPI Polyline(__in HDC hdc,__in_ecount(c) CONST POINT * lppt, int c);
WINGDIAPI BOOL     WINAPI Rectangle(HDC,int,int,int,int);
WINGDIAPI BOOL     WINAPI RoundRect(HDC,int,int,int,int,int,int);
WINGDIAPI BOOL     WINAPI SetBrushOrgEx( __in HDC hdc, int x, int y, __out_opt LPPOINT lppt);
WINGDIAPI COLORREF WINAPI SetPixel(HDC,int,int,COLORREF);
WINGDIAPI BOOL     WINAPI MoveToEx(HDC,int,int,LPPOINT);
WINGDIAPI BOOL     WINAPI LineTo(HDC,int,int);
WINGDIAPI BOOL     WINAPI GetCurrentPositionEx(HDC,LPPOINT);
// @CESYSGEN ENDIF

WINGDIAPI  BOOL      WINAPI DeleteObject(HGDIOBJ);

// @CESYSGEN IF GWES_MGBASE
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGDRAW
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGDRAW
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGBITMAP
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGBITMAP || GWES_MGDX
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGRGN
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGRGN
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGRGN
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGBLT
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGBLT2
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_PGDI || GWES_MGDIBSEC
// @CESYSGEN ENDIF

// @CESYSGEN IF GWES_MGBLT2
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGTT || GWES_MGRAST
// @CESYSGEN ENDIF

typedef struct tagTEXTMETRICA
{
    LONG        tmHeight;
    LONG        tmAscent;
    LONG        tmDescent;
    LONG        tmInternalLeading;
    LONG        tmExternalLeading;
    LONG        tmAveCharWidth;
    LONG        tmMaxCharWidth;
    LONG        tmWeight;
    LONG        tmOverhang;
    LONG        tmDigitizedAspectX;
    LONG        tmDigitizedAspectY;
    char        tmFirstChar;
    char        tmLastChar;
    char        tmDefaultChar;
    char        tmBreakChar;
    BYTE        tmItalic;
    BYTE        tmUnderlined;
    BYTE        tmStruckOut;
    BYTE        tmPitchAndFamily;
    BYTE        tmCharSet;
} TEXTMETRICA, *PTEXTMETRICA, NEAR *NPTEXTMETRICA, FAR *LPTEXTMETRICA;

typedef struct tagTEXTMETRICW
{
    LONG        tmHeight;
    LONG        tmAscent;
    LONG        tmDescent;
    LONG        tmInternalLeading;
    LONG        tmExternalLeading;
    LONG        tmAveCharWidth;
    LONG        tmMaxCharWidth;
    LONG        tmWeight;
    LONG        tmOverhang;
    LONG        tmDigitizedAspectX;
    LONG        tmDigitizedAspectY;
    WCHAR       tmFirstChar;
    WCHAR       tmLastChar;
    WCHAR       tmDefaultChar;
    WCHAR       tmBreakChar;
    BYTE        tmItalic;
    BYTE        tmUnderlined;
    BYTE        tmStruckOut;
    BYTE        tmPitchAndFamily;
    BYTE        tmCharSet;
} TEXTMETRICW, *PTEXTMETRICW, NEAR *NPTEXTMETRICW, FAR *LPTEXTMETRICW;

#ifdef UNICODE
typedef TEXTMETRICW TEXTMETRIC;
typedef PTEXTMETRICW PTEXTMETRIC;
typedef NPTEXTMETRICW NPTEXTMETRIC;
typedef LPTEXTMETRICW LPTEXTMETRIC;
#else
typedef TEXTMETRICA TEXTMETRIC;
typedef PTEXTMETRICA PTEXTMETRIC;
typedef NPTEXTMETRICA NPTEXTMETRIC;
typedef LPTEXTMETRICA LPTEXTMETRIC;
#endif // UNICODE

#define PAN_ANY                         0 /* Any                            */
#define PAN_NO_FIT                      1 /* No Fit                         */

#define PAN_FAMILY_TEXT_DISPLAY         2 /* Text and Display               */
#define PAN_FAMILY_SCRIPT               3 /* Script                         */
#define PAN_FAMILY_DECORATIVE           4 /* Decorative                     */
#define PAN_FAMILY_PICTORIAL            5 /* Pictorial                      */

#define PAN_SERIF_COVE                  2 /* Cove                           */
#define PAN_SERIF_OBTUSE_COVE           3 /* Obtuse Cove                    */
#define PAN_SERIF_SQUARE_COVE           4 /* Square Cove                    */
#define PAN_SERIF_OBTUSE_SQUARE_COVE    5 /* Obtuse Square Cove             */
#define PAN_SERIF_SQUARE                6 /* Square                         */
#define PAN_SERIF_THIN                  7 /* Thin                           */
#define PAN_SERIF_BONE                  8 /* Bone                           */
#define PAN_SERIF_EXAGGERATED           9 /* Exaggerated                    */
#define PAN_SERIF_TRIANGLE             10 /* Triangle                       */
#define PAN_SERIF_NORMAL_SANS          11 /* Normal Sans                    */
#define PAN_SERIF_OBTUSE_SANS          12 /* Obtuse Sans                    */
#define PAN_SERIF_PERP_SANS            13 /* Prep Sans                      */
#define PAN_SERIF_FLARED               14 /* Flared                         */
#define PAN_SERIF_ROUNDED              15 /* Rounded                        */

#define PAN_WEIGHT_VERY_LIGHT           2 /* Very Light                     */
#define PAN_WEIGHT_LIGHT                3 /* Light                          */
#define PAN_WEIGHT_THIN                 4 /* Thin                           */
#define PAN_WEIGHT_BOOK                 5 /* Book                           */
#define PAN_WEIGHT_MEDIUM               6 /* Medium                         */
#define PAN_WEIGHT_DEMI                 7 /* Demi                           */
#define PAN_WEIGHT_BOLD                 8 /* Bold                           */
#define PAN_WEIGHT_HEAVY                9 /* Heavy                          */
#define PAN_WEIGHT_BLACK               10 /* Black                          */
#define PAN_WEIGHT_NORD                11 /* Nord                           */

#define PAN_PROP_OLD_STYLE              2 /* Old Style                      */
#define PAN_PROP_MODERN                 3 /* Modern                         */
#define PAN_PROP_EVEN_WIDTH             4 /* Even Width                     */
#define PAN_PROP_EXPANDED               5 /* Expanded                       */
#define PAN_PROP_CONDENSED              6 /* Condensed                      */
#define PAN_PROP_VERY_EXPANDED          7 /* Very Expanded                  */
#define PAN_PROP_VERY_CONDENSED         8 /* Very Condensed                 */
#define PAN_PROP_MONOSPACED             9 /* Monospaced                     */

#define PAN_CONTRAST_NONE               2 /* None                           */
#define PAN_CONTRAST_VERY_LOW           3 /* Very Low                       */
#define PAN_CONTRAST_LOW                4 /* Low                            */
#define PAN_CONTRAST_MEDIUM_LOW         5 /* Medium Low                     */
#define PAN_CONTRAST_MEDIUM             6 /* Medium                         */
#define PAN_CONTRAST_MEDIUM_HIGH        7 /* Mediim High                    */
#define PAN_CONTRAST_HIGH               8 /* High                           */
#define PAN_CONTRAST_VERY_HIGH          9 /* Very High                      */

#define PAN_STROKE_GRADUAL_DIAG         2 /* Gradual/Diagonal               */
#define PAN_STROKE_GRADUAL_TRAN         3 /* Gradual/Transitional           */
#define PAN_STROKE_GRADUAL_VERT         4 /* Gradual/Vertical               */
#define PAN_STROKE_GRADUAL_HORZ         5 /* Gradual/Horizontal             */
#define PAN_STROKE_RAPID_VERT           6 /* Rapid/Vertical                 */
#define PAN_STROKE_RAPID_HORZ           7 /* Rapid/Horizontal               */
#define PAN_STROKE_INSTANT_VERT         8 /* Instant/Vertical               */

#define PAN_STRAIGHT_ARMS_HORZ          2 /* Straight Arms/Horizontal       */
#define PAN_STRAIGHT_ARMS_WEDGE         3 /* Straight Arms/Wedge            */
#define PAN_STRAIGHT_ARMS_VERT          4 /* Straight Arms/Vertical         */
#define PAN_STRAIGHT_ARMS_SINGLE_SERIF  5 /* Straight Arms/Single-Serif     */
#define PAN_STRAIGHT_ARMS_DOUBLE_SERIF  6 /* Straight Arms/Double-Serif     */
#define PAN_BENT_ARMS_HORZ              7 /* Non-Straight Arms/Horizontal   */
#define PAN_BENT_ARMS_WEDGE             8 /* Non-Straight Arms/Wedge        */
#define PAN_BENT_ARMS_VERT              9 /* Non-Straight Arms/Vertical     */
#define PAN_BENT_ARMS_SINGLE_SERIF     10 /* Non-Straight Arms/Single-Serif */
#define PAN_BENT_ARMS_DOUBLE_SERIF     11 /* Non-Straight Arms/Double-Serif */

#define PAN_LETT_NORMAL_CONTACT         2 /* Normal/Contact                 */
#define PAN_LETT_NORMAL_WEIGHTED        3 /* Normal/Weighted                */
#define PAN_LETT_NORMAL_BOXED           4 /* Normal/Boxed                   */
#define PAN_LETT_NORMAL_FLATTENED       5 /* Normal/Flattened               */
#define PAN_LETT_NORMAL_ROUNDED         6 /* Normal/Rounded                 */
#define PAN_LETT_NORMAL_OFF_CENTER      7 /* Normal/Off Center              */
#define PAN_LETT_NORMAL_SQUARE          8 /* Normal/Square                  */
#define PAN_LETT_OBLIQUE_CONTACT        9 /* Oblique/Contact                */
#define PAN_LETT_OBLIQUE_WEIGHTED      10 /* Oblique/Weighted               */
#define PAN_LETT_OBLIQUE_BOXED         11 /* Oblique/Boxed                  */
#define PAN_LETT_OBLIQUE_FLATTENED     12 /* Oblique/Flattened              */
#define PAN_LETT_OBLIQUE_ROUNDED       13 /* Oblique/Rounded                */
#define PAN_LETT_OBLIQUE_OFF_CENTER    14 /* Oblique/Off Center             */
#define PAN_LETT_OBLIQUE_SQUARE        15 /* Oblique/Square                 */

#define PAN_MIDLINE_STANDARD_TRIMMED    2 /* Standard/Trimmed               */
#define PAN_MIDLINE_STANDARD_POINTED    3 /* Standard/Pointed               */
#define PAN_MIDLINE_STANDARD_SERIFED    4 /* Standard/Serifed               */
#define PAN_MIDLINE_HIGH_TRIMMED        5 /* High/Trimmed                   */
#define PAN_MIDLINE_HIGH_POINTED        6 /* High/Pointed                   */
#define PAN_MIDLINE_HIGH_SERIFED        7 /* High/Serifed                   */
#define PAN_MIDLINE_CONSTANT_TRIMMED    8 /* Constant/Trimmed               */
#define PAN_MIDLINE_CONSTANT_POINTED    9 /* Constant/Pointed               */
#define PAN_MIDLINE_CONSTANT_SERIFED   10 /* Constant/Serifed               */
#define PAN_MIDLINE_LOW_TRIMMED        11 /* Low/Trimmed                    */
#define PAN_MIDLINE_LOW_POINTED        12 /* Low/Pointed                    */
#define PAN_MIDLINE_LOW_SERIFED        13 /* Low/Serifed                    */

#define PAN_XHEIGHT_CONSTANT_SMALL      2 /* Constant/Small                 */
#define PAN_XHEIGHT_CONSTANT_STD        3 /* Constant/Standard              */
#define PAN_XHEIGHT_CONSTANT_LARGE      4 /* Constant/Large                 */
#define PAN_XHEIGHT_DUCKING_SMALL       5 /* Ducking/Small                  */
#define PAN_XHEIGHT_DUCKING_STD         6 /* Ducking/Standard               */
#define PAN_XHEIGHT_DUCKING_LARGE       7 /* Ducking/Large                  */

typedef struct tagPANOSE
{
    BYTE    bFamilyType;
    BYTE    bSerifStyle;
    BYTE    bWeight;
    BYTE    bProportion;
    BYTE    bContrast;
    BYTE    bStrokeVariation;
    BYTE    bArmStyle;
    BYTE    bLetterform;
    BYTE    bMidline;
    BYTE    bXHeight;
} PANOSE, * LPPANOSE;

typedef struct _OUTLINETEXTMETRICA {
    UINT    otmSize;
    TEXTMETRICW otmTextMetrics;
    BYTE    otmFiller;
    PANOSE  otmPanoseNumber;
    UINT    otmfsSelection;
    UINT    otmfsType;
     int    otmsCharSlopeRise;
     int    otmsCharSlopeRun;
     int    otmItalicAngle;
    UINT    otmEMSquare;
     int    otmAscent;
     int    otmDescent;
    UINT    otmLineGap;
    UINT    otmsCapEmHeight;
    UINT    otmsXHeight;
    RECT    otmrcFontBox;
     int    otmMacAscent;
     int    otmMacDescent;
    UINT    otmMacLineGap;
    UINT    otmusMinimumPPEM;
    POINT   otmptSubscriptSize;
    POINT   otmptSubscriptOffset;
    POINT   otmptSuperscriptSize;
    POINT   otmptSuperscriptOffset;
    UINT    otmsStrikeoutSize;
     int    otmsStrikeoutPosition;
     int    otmsUnderscoreSize;
     int    otmsUnderscorePosition;
    PSTR    otmpFamilyName;
    PSTR    otmpFaceName;
    PSTR    otmpStyleName;
    PSTR    otmpFullName;
} OUTLINETEXTMETRICA, *POUTLINETEXTMETRICA, NEAR *NPOUTLINETEXTMETRICA, FAR *LPOUTLINETEXTMETRICA;

typedef struct _OUTLINETEXTMETRICW {
    UINT    otmSize;
    TEXTMETRICW otmTextMetrics;
    BYTE    otmFiller;
    PANOSE  otmPanoseNumber;
    UINT    otmfsSelection;
    UINT    otmfsType;
     int    otmsCharSlopeRise;
     int    otmsCharSlopeRun;
     int    otmItalicAngle;
    UINT    otmEMSquare;
     int    otmAscent;
     int    otmDescent;
    UINT    otmLineGap;
    UINT    otmsCapEmHeight;
    UINT    otmsXHeight;
    RECT    otmrcFontBox;
     int    otmMacAscent;
     int    otmMacDescent;
    UINT    otmMacLineGap;
    UINT    otmusMinimumPPEM;
    POINT   otmptSubscriptSize;
    POINT   otmptSubscriptOffset;
    POINT   otmptSuperscriptSize;
    POINT   otmptSuperscriptOffset;
    UINT    otmsStrikeoutSize;
     int    otmsStrikeoutPosition;
     int    otmsUnderscoreSize;
     int    otmsUnderscorePosition;
    PSTR    otmpFamilyName;
    PSTR    otmpFaceName;
    PSTR    otmpStyleName;
    PSTR    otmpFullName;
} OUTLINETEXTMETRICW, *POUTLINETEXTMETRICW, NEAR *NPOUTLINETEXTMETRICW, FAR *LPOUTLINETEXTMETRICW;

#ifdef UNICODE
typedef OUTLINETEXTMETRICW OUTLINETEXTMETRIC;
typedef POUTLINETEXTMETRICW POUTLINETEXTMETRIC;
typedef NPOUTLINETEXTMETRICW NPOUTLINETEXTMETRIC;
typedef LPTEXTMETRICW LPTEXTMETRIC;
#else
typedef OUTLINETEXTMETRICA OUTLINETEXTMETRIC;
typedef POUTLINETEXTMETRICA POUTLINETEXTMETRIC;
typedef NPOUTLINETEXTMETRICA NPOUTLINETEXTMETRIC;
typedef LPOUTLINETEXTMETRICA LPOUTLINETEXTMETRIC;
#endif // UNICODE


// @CESYSGEN IF GWES_PGDI || GWES_MGTT || GWES_MGRAST
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGDRWTXT
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGTT || GWES_MGRAST2
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGTT
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGTCI
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGDIBSEC
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGDC
// @CESYSGEN ENDIF

WINGDIAPI  BOOL      WINAPI DeleteDC(HDC);

WINGDIAPI  HGDIOBJ   WINAPI SelectObject(HDC,HGDIOBJ);

// @CESYSGEN IF GWES_MGDC
WINGDIAPI HDC  WINAPI CreateDCA(LPCSTR, LPCSTR , LPCSTR , CONST DEVMODEA *);
WINGDIAPI HDC  WINAPI CreateDCW(LPCWSTR, LPCWSTR , LPCWSTR , CONST DEVMODEW *);
WINGDIAPI int  WINAPI ExtEscape(HDC, int, int, LPCSTR, int, LPSTR);

#ifdef UNICODE
#define CreateDC  CreateDCW
#else
#define CreateDC  CreateDCA
#endif // !UNICODE
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGPAL
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_PGDI || GWES_MGPAL
// @CESYSGEN ENDIF


// @CESYSGEN IF GWES_MGPRINT
// @CESYSGEN ENDIF

#ifdef UNDER_NT
            int         WINAPI MulDiv(int,int,int);
WINGDIAPI	int			WINAPI SetMapMode(HDC, int);
WINGDIAPI	BOOL		WINAPI SetViewportExtEx(HDC, int, int, LPSIZE);
WINGDIAPI	BOOL		WINAPI SetViewportOrgEx(HDC, int, int, LPPOINT);
WINGDIAPI	int			WINAPI GetDIBits(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
WINGDIAPI	LONG		WINAPI GetBitmapBits(HBITMAP, LONG, LPVOID);
WINGDIAPI	UINT		WINAPI GetDIBColorTable(HDC, UINT, UINT, RGBQUAD *);
WINGDIAPI	int			WINAPI StretchDIBits(HDC, int, int, int, int, int, int, int, 
								int, CONST VOID *, CONST BITMAPINFO *, UINT, DWORD);
WINGDIAPI	BOOL		WINAPI SetWindowExtEx(HDC, int, int, LPSIZE);
WINGDIAPI	BOOL		WINAPI SetWindowOrgEx(HDC, int, int, LPPOINT);
WINGDIAPI	int			WINAPI SetStretchBltMode(HDC, int);

#define COLORONCOLOR   3
#define MM_ANISOTROPIC 8
#endif // UNDER_NT



typedef USHORT COLOR16;

typedef struct _TRIVERTEX
{
	LONG x;
	LONG y;
	COLOR16 Red;
	COLOR16 Green;
	COLOR16 Blue;
	COLOR16 Alpha;
	
}TRIVERTEX,*PTRIVERTEX, *LPTRIVERTEX;


// @CESYSGEN IF GWES_MGGRADFILL
// @CESYSGEN ENDIF 

typedef struct _BLENDFUNCTION
{
    BYTE   BlendOp;
    BYTE   BlendFlags;
    BYTE   SourceConstantAlpha;
    BYTE   AlphaFormat;
}BLENDFUNCTION,*PBLENDFUNCTION;


//
// currentlly defined blend function
//

#define AC_SRC_OVER                 0x00

//
// alpha format flags
//

#define AC_SRC_ALPHA                0x01

// @CESYSGEN IF GWES_MGALPHABLEND
// @CESYSGEN ENDIF

#ifdef __cplusplus
}
#endif


#ifdef WINCEOEM
#include <pwingdi.h>	// internal defines 
#endif


#endif /* _WINGDI_H_ */
