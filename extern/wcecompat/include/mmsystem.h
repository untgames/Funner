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


Module Name:mmsystem.h -- Include file for Multimedia API's  

 *    If defined, the following flags inhibit inclusion
 *    of the indicated items:
 *
 *      MMNOSOUND       Sound support
 *      MMNOWAVE        Waveform support
 *      MMNOMCI         MCI API
 *      MMNOMMIO        file I/O
 *
 */

#ifndef _INC_MMSYSTEM
#define _INC_MMSYSTEM   // to prevent multiple inclusion of mmsystem.h

#include "pshpack1.h"   // Assume byte packing throughout

#ifdef __cplusplus
extern "C" {            // Assume C declarations for C++
#endif  // __cplusplus

#ifndef _WINMM_
#define WINMMAPI        DECLSPEC_IMPORT
#else
#define WINMMAPI        __declspec(dllexport)
#endif
#define _loadds
#define _huge

/****************************************************************************

                    General constants and data types

****************************************************************************/

/* general constants */
#define MAXPNAMELEN      32     // max product name length (including NULL)
#define MAXERRORLENGTH   128    // max error text length (including final NULL)

/*
 *  Microsoft Manufacturer and Product ID's
 *
    Used with wMid and wPid fields in WAVEOUTCAPS, WAVEINCAPS,
    MIDIOUTCAPS, MIDIINCAPS, AUXCAPS, JOYCAPS structures.
 */
/*
 *  (these have been moved to
 *  MMREG.H for Windows 4.00 and above).
 */
/* manufacturer IDs */
#ifndef MM_MICROSOFT
#define MM_MICROSOFT            1        // Microsoft Corporation
#endif // MM_MICROSOFT



/* general data types */
typedef UINT    MMVERSION;      // major (high byte), minor (low byte)

typedef UINT    MMRESULT;       // error return code, 0 means no error
                                // call as if(err=xxxx(...)) Error(err); else

#define _MMRESULT_              // ??? Not needed for NT, but probably harmless.

typedef UINT       *LPUINT;


/* MMTIME data structure */
typedef struct mmtime_tag
{
    UINT            wType;      /* indicates the contents of the union */
    union
    {
    DWORD       ms;         /* milliseconds */
    DWORD       sample;     /* samples */
    DWORD       cb;         /* byte count */
    DWORD       ticks;      /* ticks in MIDI stream */

    /* SMPTE */
    struct
    {
        BYTE    hour;       /* hours */
        BYTE    min;        /* minutes */
        BYTE    sec;        /* seconds */
        BYTE    frame;      /* frames  */
        BYTE    fps;        /* frames per second */
        BYTE    dummy;      /* pad */
#ifdef _WIN32
        BYTE    pad[2];
#endif
    } smpte;

    /* MIDI */
    struct
    {
        DWORD songptrpos;   /* song pointer position */
    } midi;
    } u;
} MMTIME, *PMMTIME, NEAR *NPMMTIME, FAR *LPMMTIME;

/* types for wType field in MMTIME struct */
#define TIME_MS         0x0001  /* time in milliseconds */
#define TIME_SAMPLES    0x0002  /* number of wave samples */
#define TIME_BYTES      0x0004  /* current byte offset */
#define TIME_SMPTE      0x0008  /* SMPTE time */
#define TIME_MIDI       0x0010  /* MIDI time */
#define TIME_TICKS      0x0020  /* Ticks within MIDI stream */

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)

/****************************************************************************

            Multimedia Extensions Window Messages

****************************************************************************/

#define MM_MCINOTIFY        0x3B9           /* MCI */

#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD
#define MM_WOM_ATTENUATED   0x3D8           /* gain class support - stream attenuated */

#define MM_WIM_OPEN         0x3BE           /* waveform input */
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

#define MM_MIM_OPEN         0x3C1           /* MIDI input */
#define MM_MIM_CLOSE        0x3C2
#define MM_MIM_DATA         0x3C3
#define MM_MIM_LONGDATA     0x3C4
#define MM_MIM_ERROR        0x3C5
#define MM_MIM_LONGERROR    0x3C6

#define MM_MOM_OPEN         0x3C7           /* MIDI output */
#define MM_MOM_CLOSE        0x3C8
#define MM_MOM_DONE         0x3C9

/* these are also in msvideo.h */
#ifndef MM_DRVM_OPEN
 #define MM_DRVM_OPEN       0x3D0           /* installable drivers */
 #define MM_DRVM_CLOSE      0x3D1
 #define MM_DRVM_DATA       0x3D2
 #define MM_DRVM_ERROR      0x3D3
#endif

#define MM_MCISYSTEM_STRING 0x3CA       /* internal */

/* these are used by msacm.h */
#define MM_STREAM_OPEN      0x3D4
#define MM_STREAM_CLOSE     0x3D5
#define MM_STREAM_DONE      0x3D6
#define MM_STREAM_ERROR     0x3D7

#if(WINVER >= 0x0400)
#define MM_MOM_POSITIONCB   0x3CA           /* Callback for MEVT_POSITIONCB */

#ifndef MM_MCISIGNAL
 #define MM_MCISIGNAL        0x3CB
#endif

#define MM_MIM_MOREDATA      0x3CC          /* MIM_DONE w/ pending events */


#endif /* WINVER >= 0x0400 */
#define MM_MIXM_LINE_CHANGE     0x3D0       /* mixer line change notify */
#define MM_MIXM_CONTROL_CHANGE  0x3D1       /* mixer control change notify */


/****************************************************************************

        String resource number bases (internal use)

****************************************************************************/

#define MMSYSERR_BASE          0
#define WAVERR_BASE            32
#define MIDIERR_BASE           64
#define TIMERR_BASE            96
#define MCIERR_BASE            256
#define MIXERR_BASE            1024


#define MCI_STRING_OFFSET      512
#define MCI_VD_OFFSET          1024
#define MCI_CD_OFFSET          1088
#define MCI_WAVE_OFFSET        1152
#define MCI_SEQ_OFFSET         1216

/****************************************************************************

            General error return values

****************************************************************************/

/* general error return values */
#define MMSYSERR_NOERROR      0                    /* no error */
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  /* unspecified error */
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  /* device ID out of range */
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  /* driver failed enable */
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  /* device already allocated */
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  /* device handle is invalid */
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  /* no device driver present */
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  /* memory allocation error */
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  /* function isn't supported */
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  /* error value out of range */
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) /* invalid flag passed */
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) /* invalid parameter passed */
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) /* handle being used */
                                                   /* simultaneously on another */
                                                   /* thread (eg callback) */
#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) /* specified alias not found */
#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) /* bad registry database */
#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) /* registry key not found */
#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) /* registry read error */
#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) /* registry write error */
#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) /* registry delete error */
#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) /* registry value not found */
#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) /* driver does not call DriverCallback */
#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 20) /* last error in range */

DECLARE_HANDLE(HDRVR);

#ifndef MMNODRV


/****************************************************************************

                        Installable driver support

****************************************************************************/

#ifdef _WIN32
typedef struct DRVCONFIGINFOEX {
    DWORD   dwDCISize;
    LPCWSTR  lpszDCISectionName;
    LPCWSTR  lpszDCIAliasName;
    DWORD    dnDevNode;
} DRVCONFIGINFOEX, *PDRVCONFIGINFOEX, NEAR *NPDRVCONFIGINFOEX, FAR *LPDRVCONFIGINFOEX;

#else
typedef struct DRVCONFIGINFOEX {
    DWORD   dwDCISize;
    LPCSTR  lpszDCISectionName;
    LPCSTR  lpszDCIAliasName;
    DWORD    dnDevNode;
} DRVCONFIGINFOEX, *PDRVCONFIGINFOEX, NEAR *NPDRVCONFIGINFOEX, FAR *LPDRVCONFIGINFOEX;
#endif

#if (WINVER < 0x030a) || defined(_WIN32)

#ifndef DRV_LOAD

/* Driver messages */
#define DRV_LOAD                0x0001
#define DRV_ENABLE              0x0002
#define DRV_OPEN                0x0003
#define DRV_CLOSE               0x0004
#define DRV_DISABLE             0x0005
#define DRV_FREE                0x0006
#define DRV_CONFIGURE           0x0007
#define DRV_QUERYCONFIGURE      0x0008
#define DRV_INSTALL             0x0009
#define DRV_REMOVE              0x000A
#define DRV_EXITSESSION         0x000B
#define DRV_POWER               0x000F
#define DRV_RESERVED            0x0800
#define DRV_USER                0x4000

/* LPARAM of DRV_CONFIGURE message */
#ifdef _WIN32
typedef struct tagDRVCONFIGINFO {
    DWORD   dwDCISize;
    LPCWSTR  lpszDCISectionName;
    LPCWSTR  lpszDCIAliasName;
} DRVCONFIGINFO, *PDRVCONFIGINFO, NEAR *NPDRVCONFIGINFO, FAR *LPDRVCONFIGINFO;
#else
typedef struct tagDRVCONFIGINFO {
    DWORD   dwDCISize;
    LPCSTR  lpszDCISectionName;
    LPCSTR  lpszDCIAliasName;
} DRVCONFIGINFO, *PDRVCONFIGINFO, NEAR *NPDRVCONFIGINFO, FAR *LPDRVCONFIGINFO;
#endif

/* Supported return values for DRV_CONFIGURE message */
#define DRVCNF_CANCEL           0x0000
#define DRVCNF_OK               0x0001
#define DRVCNF_RESTART          0x0002

/* installable driver function prototypes */
#ifdef _WIN32

typedef LRESULT (CALLBACK* DRIVERPROC)(DWORD, HDRVR, UINT, LPARAM, LPARAM);

WINMMAPI LRESULT   WINAPI CloseDriver(HDRVR hDriver, LONG lParam1, LONG lParam2);
WINMMAPI HDRVR     WINAPI OpenDriver(LPCWSTR szDriverName, LPCWSTR szSectionName, LONG lParam2);
WINMMAPI LRESULT   WINAPI SendDriverMessage(HDRVR hDriver, UINT message, LONG lParam1, LONG lParam2);
WINMMAPI HMODULE   WINAPI DrvGetModuleHandle(HDRVR hDriver);
WINMMAPI HMODULE   WINAPI GetDriverModuleHandle(HDRVR hDriver);
WINMMAPI LRESULT   WINAPI DefDriverProc(DWORD dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
#else
LRESULT   WINAPI DrvClose(HDRVR hdrvr, LPARAM lParam1, LPARAM lParam2);
HDRVR     WINAPI DrvOpen(LPCSTR szDriverName, LPCSTR szSectionName, LPARAM lParam2);
LRESULT   WINAPI DrvSendMessage(HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
HINSTANCE WINAPI DrvGetModuleHandle(HDRVR hdrvr);
LRESULT   WINAPI DrvDefDriverProc(DWORD dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
#define DefDriverProc DrvDefDriverProc
#endif /* ifdef _WIN32 */
#endif /* DRV_LOAD */
#endif /* ifdef (WINVER < 0x030a) || defined(_WIN32) */

#if (WINVER >= 0x030a)
/* return values from DriverProc() function */
#define DRV_CANCEL             DRVCNF_CANCEL
#define DRV_OK                 DRVCNF_OK
#define DRV_RESTART            DRVCNF_RESTART

#endif /* ifdef WINVER >= 0x030a */

#define DRV_MCI_FIRST          DRV_RESERVED
#define DRV_MCI_LAST           (DRV_RESERVED + 0xFFF)

#endif  /* ifndef MMNODRV */


/****************************************************************************

              Driver callback support

****************************************************************************/

/* flags used with waveOutOpen(), waveInOpen(), midiInOpen(), and */
/* midiOutOpen() to specify the type of the dwCallback parameter. */

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */
#define CALLBACK_THREAD     (CALLBACK_TASK)/* thread ID replaces 16 bit task */
#define CALLBACK_EVENT      0x00050000l    /* dwCallback is an EVENT Handle */
#define CALLBACK_MSGQUEUE   0x00060000l    /* dwCallback is HANDLE returned by CreateMsgQueue or OpenMsgQueue (new in Windows CE 5.0) */


typedef void (CALLBACK DRVCALLBACK)(HANDLE hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef DRVCALLBACK FAR *LPDRVCALLBACK;
typedef DRVCALLBACK     *PDRVCALLBACK;

/* CALLBACK_MSGQUEUE - client process sets up a MsgQueuue that receives WAVEMSG structures.
*  Note that structure fields are identical to arguments to a callback function
*  but we put the message field first to allow for multi-functional message queues.
*/
typedef struct _tag_WAVEMSG
{
    UINT    uMsg;       // WOM_OPEN, WIM_OPEN, WOM_DONE, WIM_DATA, etc.
    HANDLE  hWav;       // stream handle returned by waveInOpen or waveOutOpen
    DWORD   dwInstance; // value of dwInstance argument passed into waveInOpen or waveOutOpen
    DWORD   dwParam1;   // completed WAVEHDR for WIM_DATA, WOM_DONE, reserved elsewhere
    DWORD   dwParam2;   // reserved
} WAVEMSG, * PWAVEMSG;

#ifndef MMNOSOUND

BOOL APIENTRY sndPlaySoundW( LPCWSTR lpszSoundName, UINT fuSound );
#ifdef UNICODE
#define sndPlaySound  sndPlaySoundW
#else
#define sndPlaySound  sndPlaySoundA
#endif // !UNICODE

BOOL WINAPI PlaySoundA( LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
BOOL WINAPI PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound);
#ifdef UNICODE
#define PlaySound  PlaySoundW
#else
#define PlaySound  PlaySoundA
#endif // !UNICODE



/*
 *  flag values for fuSound arguments on [snd]PlaySound
 *  or dwFlags for PlaySound
 */

/* sndAlias creates the alias identifier */
#define sndAlias( ch0, ch1 ) \
                ( SND_ALIAS_START + (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ))

#define SND_ALIAS_SYSTEMASTERISK        sndAlias('S', '*')
#define SND_ALIAS_SYSTEMQUESTION        sndAlias('S', '?')
#define SND_ALIAS_SYSTEMHAND            sndAlias('S', 'H')
#define SND_ALIAS_SYSTEMEXIT            sndAlias('S', 'E')
#define SND_ALIAS_SYSTEMSTART           sndAlias('S', 'S')
#define SND_ALIAS_SYSTEMWELCOME         sndAlias('S', 'W')
#define SND_ALIAS_SYSTEMEXCLAMATION     sndAlias('S', '!')
#define SND_ALIAS_SYSTEMDEFAULT         sndAlias('S', 'D')

#define SND_ALIAS      0x00010000   // name is a WIN.INI [sounds] entry
#define SND_FILENAME   0x00020000   // name is a file name
#define SND_RESOURCE   0x00040004   // name is a resource name or atom

#define SND_SYNC       0x00000000   // play synchronously (default)
#define SND_ASYNC      0x00000001   // play asynchronously
#define SND_NODEFAULT  0x00000002   // silence not default, if sound not found
#define SND_MEMORY     0x00000004   // lpszSoundName points to a memory file
#define SND_LOOP       0x00000008   // loop the sound until next sndPlaySound
#define SND_NOSTOP     0x00000010   // don't stop any currently playing sound

#define SND_NOWAIT     0x00002000   // don't wait if the driver is busy
#define SND_VALIDFLAGS 0x0017201F   // Set of valid flag bits.  Anything outside
                                    // this range will raise an error
#define SND_RESERVED   0xFF000000   // In particular these flags are reserved
#define SND_TYPE_MASK  0x00170007
#define SND_ALIAS_ID   0x00110000   // name is a WIN.INI [sounds] entry identifier
#define SND_ALIAS_START  0      // ??? must be > 4096 to keep strings in same section of resource file

#endif  // ifndef MMNOSOUND

#ifndef MMNOWAVE
/****************************************************************************

                        Waveform audio support

****************************************************************************/

/* waveform audio error return values */
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    // unsupported wave format
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    // still something playing
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    // header not prepared
#define WAVERR_SYNC           (WAVERR_BASE + 3)    // device is synchronous
#define WAVERR_LASTERROR      (WAVERR_BASE + 3)    // last error in range

/* waveform audio data types */
DECLARE_HANDLE(HWAVE);
DECLARE_HANDLE(HWAVEIN);
DECLARE_HANDLE(HWAVEOUT);

typedef HWAVEIN FAR *LPHWAVEIN;
typedef HWAVEOUT FAR *LPHWAVEOUT;
typedef DRVCALLBACK WAVECALLBACK;
typedef WAVECALLBACK FAR *LPWAVECALLBACK;

/* wave callback messages */
#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

/* device ID for wave device mapper */
/* device ID for wave device mapper */
#define WAVE_MAPPER     ((DWORD)(-1))

/* flags for dwFlags parameter in waveOutOpen() and waveInOpen() */
#define  WAVE_FORMAT_QUERY     0x00000001
#define  WAVE_ALLOWSYNC        0x00000002
#define  WAVE_MAPPED           0x00000004
#define  WAVE_FORMAT_DIRECT    0x00000008
#define  WAVE_FORMAT_DIRECT_QUERY  (WAVE_FORMAT_QUERY | WAVE_FORMAT_DIRECT)
#define  WAVE_NOMIXER          0x00000080           /* Windows CE only - bypass software mixer */


/* general waveform format (information common to all formats) */
/* as read from a file.  See Multimedia file formats in Programmer Reference */
typedef struct {
    WORD    wFormatTag;        // format type
    WORD    nChannels;         // number of channels (i.e. mono, stereo, etc.)
    DWORD   nSamplesPerSec;    // sample rate
    DWORD   nAvgBytesPerSec;   // for buffer estimation
    WORD    nBlockAlign;       // block size of data
} WAVEFORMAT;
typedef WAVEFORMAT       *PWAVEFORMAT;
typedef WAVEFORMAT      *NPWAVEFORMAT;
typedef WAVEFORMAT      *LPWAVEFORMAT;
typedef CONST WAVEFORMAT  *LPCWAVEFORMAT;

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1  // Needed in resource files so outside #ifndef RC_INVOKED #endif



/* specific waveform format for PCM data */
typedef struct tagPCMWAVEFORMAT {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;        // corresponds to MCI_WAVE_SET_.... structure
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;
typedef PCMWAVEFORMAT      *NPPCMWAVEFORMAT;
typedef PCMWAVEFORMAT      *LPPCMWAVEFORMAT;


/* wave data block header */
typedef struct wavehdr_tag {
    LPSTR       lpData;                 /* pointer to locked data buffer */
    DWORD       dwBufferLength;         /* length of data buffer */
    DWORD       dwBytesRecorded;        /* used for input only */
    DWORD       dwUser;                 /* for client's use */
    DWORD       dwFlags;                /* assorted flags (see defines) */
    DWORD       dwLoops;                /* loop control counter */
    struct wavehdr_tag FAR *lpNext;     /* reserved for driver */
    DWORD       reserved;               /* reserved for driver */
} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

/* flags for dwFlags field of WAVEHDR */
#define WHDR_DONE       0x00000001  /* done bit */
#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
#define WHDR_ENDLOOP    0x00000008  /* loop end block */
#define WHDR_INQUEUE    0x00000010  /* reserved for driver */

/* waveform output device capabilities structure */
typedef struct tagWAVEOUTCAPS {
    WORD    wMid;                  /* manufacturer ID */
    WORD    wPid;                  /* product ID */
    MMVERSION vDriverVersion;      /* version of the driver */
    TCHAR   szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    DWORD   dwFormats;             /* formats supported */
    WORD    wChannels;             /* number of sources supported */
    WORD    wReserved1;            /* packing */
    DWORD   dwSupport;             /* functionality supported by driver */
} WAVEOUTCAPS, *PWAVEOUTCAPS, *NPWAVEOUTCAPS, *LPWAVEOUTCAPS;


/* flags for dwSupport field of WAVEOUTCAPS */
#define WAVECAPS_PITCH          0x0001   /* supports pitch control */
#define WAVECAPS_PLAYBACKRATE   0x0002   /* supports playback rate control */
#define WAVECAPS_VOLUME         0x0004   /* supports volume control */
#define WAVECAPS_LRVOLUME       0x0008   /* separate left-right volume control */
//#define WAVECAPS_SYNC           0x0010 /* Windows CE only supports asynchronous audio devices */
#define WAVECAPS_SAMPLEACCURATE 0x0020
#define WAVECAPS_DIRECTSOUND    0x0040

/* waveform input device capabilities structure */
typedef struct tagWAVEINCAPS {
    WORD    wMid;                    /* manufacturer ID */
    WORD    wPid;                    /* product ID */
    MMVERSION vDriverVersion;        /* version of the driver */
    TCHAR   szPname[MAXPNAMELEN];    /* product name (NULL terminated string) */
    DWORD   dwFormats;               /* formats supported */
    WORD    wChannels;               /* number of channels supported */
    WORD    wReserved1;              /* structure packing */
} WAVEINCAPS, *PWAVEINCAPS, *NPWAVEINCAPS, *LPWAVEINCAPS;

/* defines for dwFormat field of WAVEINCAPS and WAVEOUTCAPS */
#define WAVE_INVALIDFORMAT     0x00000000       /* invalid format */
#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, Mono,   16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, Stereo, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, Mono,   8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, Mono,   16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, Stereo, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, Mono,   8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, Mono,   16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, Stereo, 16-bit */

/* property information for audio gain classes */

/* audio gain class property sets */
#define MM_PROPSET_GAINCLASS_CLASS \
    { 0xe7e569a5, 0x8498, 0x43fe, { 0x80, 0x75, 0x33, 0xd1, 0xfd, 0xab, 0x15, 0xef } }
#define MM_PROPSET_GAINCLASS_STREAM \
    { 0x40e953ae, 0xee3e, 0x493a, { 0x93, 0xee, 0xda, 0x3e, 0x30, 0x76, 0x43, 0x90 } }

/* MM_PROPSET_GAINCLASS_CLASS property IDs */
#define MM_PROP_GAINCLASS_CLASS     1

/* MM_PROPSET_GAINCLASS_STREAM property IDs */
#define MM_PROP_GAINCLASS_STREAM    1

/* structure to describe audio gain classes */
typedef struct tagAUDIOGAINCLASS {
    DWORD   dwPriority;
    DWORD   dwRelativeGain;
} AUDIOGAINCLASS, * PAUDIOGAINCLASS;
typedef AUDIOGAINCLASS const * CPAUDIOGAINCLASS;

/* values for AUDIOGAINCLASS.dwPriority */
#define WAGC_PRIORITY_CRITICAL  6
#define WAGC_PRIORITY_HIGH      4
#define WAGC_PRIORITY_NORMAL    2

/* wave stream properties structure */
typedef struct tagSTREAMPROPS {
    DWORD   dwClassID;
    DWORD   dwFlags;
} STREAMPROPS, *PSTREAMPROPS;
typedef STREAMPROPS const * CPSTREAMPROPS;

/* flag definitions for STREAMPROPS.dwFlags */
#define SPSFL_NOTIFY_GAIN_CHANGE   0x00000001      /* callback gets MM_WOM_ATTENUATED messages */
#define SPSFL_EXCLUDE              0x00000002      /* stream does not participate in gain-class operations */

/* values for STREAMPROPS.dwClassid */
#define WAGC_CLASS_DEFAULT              0
#define WAGC_CLASS_NORMAL               1
#define WAGC_CLASS_AUDIO_ALERT          2
#define WAGC_CLASS_SPEECH_ALERT         3
#define WAGC_CLASS_SPEECH_NOTIFICATION  4
#define WAGC_CLASS_AUDIO_NOTIFICATION   5
#define WAGC_CLASS_FORCE_MUTE           6
/* classes 7-15 are reserved for future use */
#define WAGC_CLASS_MAX                  15 /* class ID's larger than this are illegal */

/* Discontinuity detection property set */
#define MM_PROPSET_DISCONTINUITY \
    { 0xb4389733, 0x868e, 0x4d06, { 0xb0, 0x8, 0x9a, 0x6f, 0xc8, 0xce, 0x85, 0x2e } }

/* MM_PROPSET_DISCONTINUITY property IDs */
#define MM_PROP_DISCONTINUITY   1

/* bit values for MM_PROP_DISCONTINUITY parameter */
#define WDSC_APP        1
#define WDSC_SWMIXER    2
#define WDSC_DRIVER     4

/* structure to receive discontinuity information */
typedef struct tagAUDIODISCONTINUITY {
    DWORD dwMask;       // bitmask shows which values were set (see WDSC_* values above)
    DWORD dwApp;        // # discontinuities detected at the application level
    DWORD dwSwMixer;    // # discontinuities detected at the software mixer level
    DWORD dwDriver;     // # discontinuities detected at the driver level
} AUDIODISCONTINUITY, * PAUDIODISCONTINUITY;


#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

/*
 *  extended waveform format structure used for all non-PCM formats. this
 *  structure is common to all non-PCM formats.
 */
typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         /* format type */
    WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD       nSamplesPerSec;     /* sample rate */
    DWORD       nAvgBytesPerSec;    /* for buffer estimation */
    WORD        nBlockAlign;        /* block size of data */
    WORD        wBitsPerSample;     /* number of bits per sample of mono data */
    WORD        cbSize;             /* the count in bytes of the size of */
                    /* extra information (after cbSize) */
} WAVEFORMATEX, *PWAVEFORMATEX, NEAR *NPWAVEFORMATEX, FAR *LPWAVEFORMATEX;
typedef const WAVEFORMATEX FAR *LPCWAVEFORMATEX;

#endif /* _WAVEFORMATEX_ */


/* waveform audio function prototypes */
UINT     WINAPI waveOutGetNumDevs(void);
MMRESULT WINAPI waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI waveOutClose(HWAVEOUT hwo);
MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveOutPause(HWAVEOUT hwo);
MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo);
MMRESULT WINAPI waveOutReset(HWAVEOUT hwo);
MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwo);
MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID);
MMRESULT WINAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
MMRESULT WINAPI waveOutGetProperty(UINT uDeviceID, const GUID* pPropSetId, ULONG ulPropId,
    LPVOID pvPropParams, ULONG cbPropParams, LPVOID pvPropData, ULONG cbPropData, PULONG pcbReturn);
MMRESULT WINAPI waveOutSetProperty(UINT uDeviceID, const GUID* pPropSetId, ULONG ulPropId,
    LPVOID pvPropParams, ULONG cbPropParams, LPVOID pvPropData, ULONG cbPropData);


UINT     WINAPI waveInGetNumDevs(void);
MMRESULT WINAPI waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI waveInClose(HWAVEIN hwi);
MMRESULT WINAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI waveInStart(HWAVEIN hwi);
MMRESULT WINAPI waveInStop(HWAVEIN hwi);
MMRESULT WINAPI waveInReset(HWAVEIN hwi);
MMRESULT WINAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI waveInGetID(HWAVEIN hwi, LPUINT puDeviceID);
MMRESULT WINAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
MMRESULT WINAPI waveInGetProperty(UINT uDeviceID, const GUID* pPropSetId, ULONG ulPropId,
    LPVOID pvPropParams, ULONG cbPropParams, LPVOID pvPropData, ULONG cbPropData, PULONG pcbReturn);
MMRESULT WINAPI waveInSetProperty(UINT uDeviceID, const GUID* pPropSetId, ULONG ulPropId,
    LPVOID pvPropParams, ULONG cbPropParams, LPVOID pvPropData, ULONG cbPropData);



#endif  /* ifndef MMNOWAVE */


#ifndef MMNOMIDI
/****************************************************************************

                            MIDI audio support

****************************************************************************/

/* MIDI error return values */
#define MIDIERR_UNPREPARED    (MIDIERR_BASE + 0)   /* header not prepared */
#define MIDIERR_STILLPLAYING  (MIDIERR_BASE + 1)   /* still something playing */
#define MIDIERR_NOMAP         (MIDIERR_BASE + 2)   /* no configured instruments */
#define MIDIERR_NOTREADY      (MIDIERR_BASE + 3)   /* hardware is still busy */
#define MIDIERR_NODEVICE      (MIDIERR_BASE + 4)   /* port no longer connected */
#define MIDIERR_INVALIDSETUP  (MIDIERR_BASE + 5)   /* invalid MIF */
#define MIDIERR_BADOPENMODE   (MIDIERR_BASE + 6)   /* operation unsupported w/ open mode */
#define MIDIERR_DONT_CONTINUE (MIDIERR_BASE + 7)   /* thru device 'eating' a message */
#define MIDIERR_LASTERROR     (MIDIERR_BASE + 7)   /* last error in range */

/* MIDI audio data types */
DECLARE_HANDLE(HMIDI);
DECLARE_HANDLE(HMIDIIN);
DECLARE_HANDLE(HMIDIOUT);
DECLARE_HANDLE(HMIDISTRM);
typedef HMIDI FAR *LPHMIDI;
typedef HMIDIIN FAR *LPHMIDIIN;
typedef HMIDIOUT FAR *LPHMIDIOUT;
typedef HMIDISTRM FAR *LPHMIDISTRM;
typedef DRVCALLBACK MIDICALLBACK;
typedef MIDICALLBACK FAR *LPMIDICALLBACK;
#define MIDIPATCHSIZE   128
typedef WORD PATCHARRAY[MIDIPATCHSIZE];
typedef WORD FAR *LPPATCHARRAY;
typedef WORD KEYARRAY[MIDIPATCHSIZE];
typedef WORD FAR *LPKEYARRAY;

/* MIDI callback messages */
#define MIM_OPEN        MM_MIM_OPEN
#define MIM_CLOSE       MM_MIM_CLOSE
#define MIM_DATA        MM_MIM_DATA
#define MIM_LONGDATA    MM_MIM_LONGDATA
#define MIM_ERROR       MM_MIM_ERROR
#define MIM_LONGERROR   MM_MIM_LONGERROR
#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE

#if(WINVER >= 0x0400)
#define MIM_MOREDATA      MM_MIM_MOREDATA
#define MOM_POSITIONCB    MM_MOM_POSITIONCB
#endif /* WINVER >= 0x0400 */

/* device ID for MIDI mapper */
#define MIDIMAPPER     ((UINT)-1)
#define MIDI_MAPPER    ((UINT)-1)

#if(WINVER >= 0x0400)
/* flags for dwFlags parm of midiInOpen() */
#define MIDI_IO_STATUS      0x00000020L
#endif /* WINVER >= 0x0400 */
#define MIDI_IO_CONTROL     0x00000008L                         /* Internal */
#define MIDI_IO_INPUT       0x00000010L  /*future*/             /* Internal */
#define MIDI_IO_OWNED       0x00004000L                         /* Internal */
#define MIDI_IO_SHARED      0x00008000L                         /* Internal */
#define MIDI_I_VALID        0xC027                              /* Internal */
#define MIDI_O_VALID        0xC00E                              /* Internal */


/* flags for wFlags parm of midiOutCachePatches(), midiOutCacheDrumPatches() */
#define MIDI_CACHE_ALL      1
#define MIDI_CACHE_BESTFIT  2
#define MIDI_CACHE_QUERY    3
#define MIDI_UNCACHE        4
#define MIDI_CACHE_VALID    (MIDI_CACHE_ALL | MIDI_CACHE_BESTFIT | MIDI_CACHE_QUERY | MIDI_UNCACHE)     /* Internal */


/* MIDI output device capabilities structure */
#ifdef _WIN32

typedef struct tagMIDIOUTCAPSA {
    WORD    wMid;                  /* manufacturer ID */
    WORD    wPid;                  /* product ID */
    MMVERSION vDriverVersion;      /* version of the driver */
    CHAR    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    WORD    wTechnology;           /* type of device */
    WORD    wVoices;               /* # of voices (internal synth only) */
    WORD    wNotes;                /* max # of notes (internal synth only) */
    WORD    wChannelMask;          /* channels used (internal synth only) */
    DWORD   dwSupport;             /* functionality supported by driver */
} MIDIOUTCAPSA, *PMIDIOUTCAPSA, *NPMIDIOUTCAPSA, *LPMIDIOUTCAPSA;
typedef struct tagMIDIOUTCAPSW {
    WORD    wMid;                  /* manufacturer ID */
    WORD    wPid;                  /* product ID */
    MMVERSION vDriverVersion;      /* version of the driver */
    WCHAR   szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    WORD    wTechnology;           /* type of device */
    WORD    wVoices;               /* # of voices (internal synth only) */
    WORD    wNotes;                /* max # of notes (internal synth only) */
    WORD    wChannelMask;          /* channels used (internal synth only) */
    DWORD   dwSupport;             /* functionality supported by driver */
} MIDIOUTCAPSW, *PMIDIOUTCAPSW, *NPMIDIOUTCAPSW, *LPMIDIOUTCAPSW;
#ifdef UNICODE
typedef MIDIOUTCAPSW MIDIOUTCAPS;
typedef PMIDIOUTCAPSW PMIDIOUTCAPS;
typedef NPMIDIOUTCAPSW NPMIDIOUTCAPS;
typedef LPMIDIOUTCAPSW LPMIDIOUTCAPS;
#else
typedef MIDIOUTCAPSA MIDIOUTCAPS;
typedef PMIDIOUTCAPSA PMIDIOUTCAPS;
typedef NPMIDIOUTCAPSA NPMIDIOUTCAPS;
typedef LPMIDIOUTCAPSA LPMIDIOUTCAPS;
#endif // UNICODE

#else
typedef struct midioutcaps_tag {
    WORD    wMid;                  /* manufacturer ID */
    WORD    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
    WORD    wTechnology;           /* type of device */
    WORD    wVoices;               /* # of voices (internal synth only) */
    WORD    wNotes;                /* max # of notes (internal synth only) */
    WORD    wChannelMask;          /* channels used (internal synth only) */
    DWORD   dwSupport;             /* functionality supported by driver */
} MIDIOUTCAPS, *PMIDIOUTCAPS, NEAR *NPMIDIOUTCAPS, FAR *LPMIDIOUTCAPS;
#endif

/* flags for wTechnology field of MIDIOUTCAPS structure */
#define MOD_MIDIPORT    1  /* output port */
#define MOD_SYNTH       2  /* generic internal synth */
#define MOD_SQSYNTH     3  /* square wave internal synth */
#define MOD_FMSYNTH     4  /* FM internal synth */
#define MOD_MAPPER      5  /* MIDI mapper */

/* flags for dwSupport field of MIDIOUTCAPS structure */
#define MIDICAPS_VOLUME          0x0001  /* supports volume control */
#define MIDICAPS_LRVOLUME        0x0002  /* separate left-right volume control */
#define MIDICAPS_CACHE           0x0004
#if(WINVER >= 0x0400)
#define MIDICAPS_STREAM          0x0008  /* driver supports midiStreamOut directly */
#endif /* WINVER >= 0x0400 */


/* MIDI input device capabilities structure */
#ifdef _WIN32

typedef struct tagMIDIINCAPSA {
    WORD        wMid;                   /* manufacturer ID */
    WORD        wPid;                   /* product ID */
    MMVERSION   vDriverVersion;         /* version of the driver */
    CHAR        szPname[MAXPNAMELEN];   /* product name (NULL terminated string) */
#if (WINVER >= 0x0400)
    DWORD   dwSupport;             /* functionality supported by driver */
#endif
} MIDIINCAPSA, *PMIDIINCAPSA, *NPMIDIINCAPSA, *LPMIDIINCAPSA;
typedef struct tagMIDIINCAPSW {
    WORD        wMid;                   /* manufacturer ID */
    WORD        wPid;                   /* product ID */
    MMVERSION   vDriverVersion;         /* version of the driver */
    WCHAR       szPname[MAXPNAMELEN];   /* product name (NULL terminated string) */
#if (WINVER >= 0x0400)
    DWORD   dwSupport;             /* functionality supported by driver */
#endif
} MIDIINCAPSW, *PMIDIINCAPSW, *NPMIDIINCAPSW, *LPMIDIINCAPSW;
#ifdef UNICODE
typedef MIDIINCAPSW MIDIINCAPS;
typedef PMIDIINCAPSW PMIDIINCAPS;
typedef NPMIDIINCAPSW NPMIDIINCAPS;
typedef LPMIDIINCAPSW LPMIDIINCAPS;
#else
typedef MIDIINCAPSA MIDIINCAPS;
typedef PMIDIINCAPSA PMIDIINCAPS;
typedef NPMIDIINCAPSA NPMIDIINCAPS;
typedef LPMIDIINCAPSA LPMIDIINCAPS;
#endif // UNICODE

#else
typedef struct midiincaps_tag {
    WORD    wMid;                  /* manufacturer ID */
    WORD    wPid;                  /* product ID */
    VERSION vDriverVersion;        /* version of the driver */
    char    szPname[MAXPNAMELEN];  /* product name (NULL terminated string) */
#if (WINVER >= 0x0400)
    DWORD   dwSupport;             /* functionality supported by driver */
#endif
} MIDIINCAPS, *PMIDIINCAPS, NEAR *NPMIDIINCAPS, FAR *LPMIDIINCAPS;
#endif


/* MIDI data block header */
typedef struct midihdr_tag {
    LPSTR       lpData;               /* pointer to locked data block */
    DWORD       dwBufferLength;       /* length of data in data block */
    DWORD       dwBytesRecorded;      /* used for input only */
    DWORD       dwUser;               /* for client's use */
    DWORD       dwFlags;              /* assorted flags (see defines) */
    struct midihdr_tag far *lpNext;   /* reserved for driver */
    DWORD       reserved;             /* reserved for driver */
#if (WINVER >= 0x0400)
    DWORD       dwOffset;             /* Callback offset into buffer */
    DWORD       dwReserved[8];        /* Reserved for MMSYSTEM */
#endif
} MIDIHDR, *PMIDIHDR, NEAR *NPMIDIHDR, FAR *LPMIDIHDR;


#if(WINVER >= 0x0400)
typedef struct midievent_tag
{
    DWORD       dwDeltaTime;          /* Ticks since last event */
    DWORD       dwStreamID;           /* Reserved; must be zero */
    DWORD       dwEvent;              /* Event type and parameters */
    DWORD       dwParms[1];           /* Parameters if this is a long event */
} MIDIEVENT;

typedef struct midistrmbuffver_tag
{
    DWORD       dwVersion;                  /* Stream buffer format version */
    DWORD       dwMid;                      /* Manufacturer ID as defined in MMREG.H */
    DWORD       dwOEMVersion;               /* Manufacturer version for custom ext */
} MIDISTRMBUFFVER;
#endif /* WINVER >= 0x0400 */

/* flags for dwFlags field of MIDIHDR structure */
#define MHDR_DONE       0x00000001       /* done bit */
#define MHDR_PREPARED   0x00000002       /* set if header prepared */
#define MHDR_INQUEUE    0x00000004       /* reserved for driver */
#define MHDR_ISSTRM     0x00000008       /* Buffer is stream buffer */
#define MHDR_SENDING    0x00000020       /* Internal */
#define MHDR_MAPPED     0x00001000       /* thunked header */   /* Internal */
#define MHDR_SHADOWHDR  0x00002000       /* MIDIHDR is 16-bit shadow */ /* Internal */
#define MHDR_VALID      0xe000302F       /* valid flags */      /* Internal */

#define MHDR_SAVE       0xe0003000       /* Save these flags */  /* Internal */
                                         /* past driver calls */ /* Internal */

/* Dreamcast-only flags for tonebank support */
#define MHDR_WRITEABLE    0xe0000000
#define MHDR_GENERALMIDI  0x80000000     /* Indicates a tonebank that is to be used */
                                         /* by General MIDI stream ports */
#define MHDR_GMDRUM       0x40000000     /* In combination with GENERALMIDI, indicates */
                                         /* a GM drum tonebank.  By itself, indicates an */
							             /* "alternate" GM drum tonebank */
#define MHDR_MAPPINGTABLE 0x20000000     /* Indicates that this MIDIHDR is for a */
                                         /* program mapping table rather than an actual */
							             /* tonebank */

#if(WINVER >= 0x0400)
/* */
/* Type codes which go in the high byte of the event DWORD of a stream buffer */
/* */
/* Type codes 00-7F contain parameters within the low 24 bits */
/* Type codes 80-FF contain a length of their parameter in the low 24 */
/* bits, followed by their parameter data in the buffer. The event */
/* DWORD contains the exact byte length; the parm data itself must be */
/* padded to be an even multiple of 4 bytes long. */
/* */

#define MEVT_F_SHORT        0x00000000L
#define MEVT_F_LONG         0x80000000L
#define MEVT_F_CALLBACK     0x40000000L

#define MEVT_EVENTTYPE(x)   ((BYTE)(((x)>>24)&0xFF))
#define MEVT_EVENTPARM(x)   ((DWORD)((x)&0x00FFFFFFL))

#define MEVT_SHORTMSG       ((BYTE)0x00)    /* parm = shortmsg for midiOutShortMsg */
#define MEVT_TEMPO          ((BYTE)0x01)    /* parm = new tempo in microsec/qn     */
#define MEVT_NOP            ((BYTE)0x02)    /* parm = unused; does nothing         */

/* 0x04-0x7F reserved */

#define MEVT_LONGMSG        ((BYTE)0x80)    /* parm = bytes to send verbatim       */
#define MEVT_COMMENT        ((BYTE)0x82)    /* parm = comment data                 */
#define MEVT_VERSION        ((BYTE)0x84)    /* parm = MIDISTRMBUFFVER struct       */

/* 0x81-0xFF reserved */

#define MIDISTRM_ERROR      (-2)

/* */
/* Structures and defines for midiStreamProperty */
/* */
#define MIDIPROP_SET        0x80000000L
#define MIDIPROP_GET        0x40000000L

/* These are intentionally both non-zero so the app cannot accidentally */
/* leave the operation off and happen to appear to work due to default */
/* action. */

#define MIDIPROP_PROPERTY(mp) ((mp) & ~(MIDIPROP_SET | MIDIPROP_GET))

#define MIDIPROP_TIMEDIV     0x00000001L
#define MIDIPROP_TEMPO       0x00000002L
#define MIDIPROP_SMF         0x00000003L
#define MIDIPROP_BUFFERED    0x00000004L
#define MIDIPROP_LOOPING     0x00000005L
#define MIDIPROP_GENERALMIDI 0x00000006L

/* Some useful defines to help with Standard MIDI File Format stuff */
#define SMF_TIMEDIV_QUARTERNOTE 0
#define SMF_TIMEDIV_SECONDS 1
#define SMF_TIMEDIV(format, division) (((format) << 15) | (division))
#define SMF_TIMEDIV_SMPTE(smpte, division) (((smpte) << 8) | (division))
#define SMF_TIMEDIV_ISSMPTE(dw) ((dw) >> 15)
#define SMF_TIMEDIV_GETSMPTE(dw) ((char)(((dw) >> 8) & 0xff))
#define SMF_TIMEDIV_GETTPF(dw) ((dw) & 0xff)
#define SMF_TIMEDIV_GETTPQN(dw) ((dw) & 0x7fff)

typedef struct midiproptimediv_tag
{
    DWORD       cbStruct;
    DWORD       dwTimeDiv;
} MIDIPROPTIMEDIV, FAR *LPMIDIPROPTIMEDIV;

typedef struct midiproptempo_tag
{
    DWORD       cbStruct;
    DWORD       dwTempo;
} MIDIPROPTEMPO, FAR *LPMIDIPROPTEMPO;

#endif /* WINVER >= 0x0400 */
#define MIDIPROP_PROPVAL    0x3FFFFFFFL /* Internal */

/* MIDI function prototypes */
WINMMAPI UINT WINAPI midiOutGetNumDevs(void);
#if(WINVER >= 0x0400)
WINMMAPI MMRESULT WINAPI midiStreamOpen(LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiStreamClose(HMIDISTRM hms);

WINMMAPI MMRESULT WINAPI midiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty);
WINMMAPI MMRESULT WINAPI midiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt);

WINMMAPI MMRESULT WINAPI midiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiStreamPause(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamRestart(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamStop(HMIDISTRM hms);

#ifdef _WIN32
WINMMAPI MMRESULT WINAPI midiConnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
WINMMAPI MMRESULT WINAPI midiDisconnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
#endif
#endif /* WINVER >= 0x0400 */

#ifdef _WIN32

WINMMAPI MMRESULT WINAPI midiOutGetDevCapsA(UINT uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc);
WINMMAPI MMRESULT WINAPI midiOutGetDevCapsW(UINT uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc);
#ifdef UNICODE
#define midiOutGetDevCaps  midiOutGetDevCapsW
#else
#define midiOutGetDevCaps  midiOutGetDevCapsA
#endif // !UNICODE

#else
MMRESULT WINAPI midiOutGetDevCaps(UINT uDeviceID, LPMIDIOUTCAPS pmoc, UINT cbmoc);
#endif

#if (WINVER >= 0x0400)
WINMMAPI MMRESULT WINAPI midiOutGetVolume(HMIDIOUT hmo, LPDWORD pdwVolume);
WINMMAPI MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume);
#else
WINMMAPI MMRESULT WINAPI midiOutGetVolume(UINT uId, LPDWORD pdwVolume);
WINMMAPI MMRESULT WINAPI midiOutSetVolume(UINT uId, DWORD dwVolume);
#endif

#ifdef _WIN32

WINMMAPI MMRESULT WINAPI midiOutGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText);
WINMMAPI MMRESULT WINAPI midiOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
#ifdef UNICODE
#define midiOutGetErrorText  midiOutGetErrorTextW
#else
#define midiOutGetErrorText  midiOutGetErrorTextA
#endif // !UNICODE

#else
WINMMAPI MMRESULT WINAPI midiOutGetErrorText(MMRESULT mmrError, LPSTR pszText, UINT cchText);
#endif

WINMMAPI MMRESULT WINAPI midiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);
WINMMAPI MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutCachePatches(HMIDIOUT hmo, UINT uBank, LPWORD pwpa, UINT fuCache);
WINMMAPI MMRESULT WINAPI midiOutCacheDrumPatches(HMIDIOUT hmo, UINT uPatch, LPWORD pwkya, UINT fuCache);
WINMMAPI MMRESULT WINAPI midiOutGetID(HMIDIOUT hmo, LPUINT puDeviceID);

#if (WINVER >= 0x030a)
#ifdef _WIN32
WINMMAPI MMRESULT WINAPI midiOutMessage(HMIDIOUT hmo, UINT uMsg, DWORD dw1, DWORD dw2);
#else
DWORD WINAPI midiOutMessage(HMIDIOUT hmo, UINT uMsg, DWORD dw1, DWORD dw2);
#endif
#endif /* ifdef WINVER >= 0x030a */

WINMMAPI UINT WINAPI midiInGetNumDevs(void);

#ifdef _WIN32

WINMMAPI MMRESULT WINAPI midiInGetDevCapsA(UINT uDeviceID, LPMIDIINCAPSA pmic, UINT cbmic);
WINMMAPI MMRESULT WINAPI midiInGetDevCapsW(UINT uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic);
#ifdef UNICODE
#define midiInGetDevCaps  midiInGetDevCapsW
#else
#define midiInGetDevCaps  midiInGetDevCapsA
#endif // !UNICODE
WINMMAPI MMRESULT WINAPI midiInGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText);
WINMMAPI MMRESULT WINAPI midiInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText);
#ifdef UNICODE
#define midiInGetErrorText  midiInGetErrorTextW
#else
#define midiInGetErrorText  midiInGetErrorTextA
#endif // !UNICODE

#else
MMRESULT WINAPI midiInGetDevCaps(UINT uDeviceID, LPMIDIINCAPS pmic, UINT cbmic);
WINMMAPI MMRESULT WINAPI midiInGetErrorText(MMRESULT mmrError, LPSTR pszText, UINT cchText);
#endif

WINMMAPI MMRESULT WINAPI midiInOpen(LPHMIDIIN phmi, UINT uDeviceID,
        DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiInClose(HMIDIIN hmi);
WINMMAPI MMRESULT WINAPI midiInPrepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiInUnprepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiInAddBuffer(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiInStart(HMIDIIN hmi);
WINMMAPI MMRESULT WINAPI midiInStop(HMIDIIN hmi);
WINMMAPI MMRESULT WINAPI midiInReset(HMIDIIN hmi);
WINMMAPI MMRESULT WINAPI midiInGetID(HMIDIIN hmi, LPUINT puDeviceID);

#if (WINVER >= 0x030a)
#ifdef _WIN32
WINMMAPI MMRESULT WINAPI midiInMessage(HMIDIIN hmi, UINT uMsg, DWORD dw1, DWORD dw2);
#else
DWORD WINAPI midiInMessage(HMIDIIN hmi, UINT uMsg, DWORD dw1, DWORD dw2);
#endif
#endif /* ifdef WINVER >= 0x030a */


#endif  /* ifndef MMNOMIDI */

#ifndef MMNOMIXER
/****************************************************************************

			    Mixer Support

****************************************************************************/


DECLARE_HANDLE(HMIXEROBJ);
typedef HMIXEROBJ FAR *LPHMIXEROBJ;

DECLARE_HANDLE(HMIXER);
typedef HMIXER     FAR *LPHMIXER;

#define MIXER_SHORT_NAME_CHARS   16
#define MIXER_LONG_NAME_CHARS    64

/* */
/*  MMRESULT error return values specific to the mixer API */
/* */
/* */

#define MIXERR_INVALLINE            (MIXERR_BASE + 0)
#define MIXERR_INVALCONTROL         (MIXERR_BASE + 1)
#define MIXERR_INVALVALUE           (MIXERR_BASE + 2)
#define MIXERR_LASTERROR            (MIXERR_BASE + 2)

#define MIXER_OBJECTF_HANDLE    0x80000000L
#define MIXER_OBJECTF_MIXER     0x00000000L
#define MIXER_OBJECTF_HMIXER    (MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIXER)
#define MIXER_OBJECTF_WAVEOUT   0x10000000L
#define MIXER_OBJECTF_HWAVEOUT  (MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_WAVEOUT)
#define MIXER_OBJECTF_WAVEIN    0x20000000L
#define MIXER_OBJECTF_HWAVEIN   (MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_WAVEIN)
#define MIXER_OBJECTF_MIDIOUT   0x30000000L
#define MIXER_OBJECTF_HMIDIOUT  (MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIDIOUT)
#define MIXER_OBJECTF_MIDIIN    0x40000000L
#define MIXER_OBJECTF_HMIDIIN   (MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIDIIN)
#define MIXER_OBJECTF_AUX       0x50000000L

UINT WINAPI mixerGetNumDevs(void);

typedef struct tagMIXERCAPS {
    WORD            wMid;                   /* manufacturer id */
    WORD            wPid;                   /* product id */
    MMVERSION       vDriverVersion;         /* version of the driver */
    TCHAR            szPname[MAXPNAMELEN];   /* product name */
    DWORD           fdwSupport;             /* misc. support bits */
    DWORD           cDestinations;          /* count of destinations */
} MIXERCAPS, *PMIXERCAPS, *LPMIXERCAPS;

MMRESULT WINAPI mixerGetDevCaps(UINT uMxId, LPMIXERCAPS pmxcaps, UINT cbmxcaps);

MMRESULT WINAPI mixerOpen(LPHMIXER phmx, UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

MMRESULT WINAPI mixerClose(HMIXER hmx);

DWORD WINAPI mixerMessage(HMIXER hmx, UINT uMsg, DWORD dwParam1, DWORD dwParam2);

typedef struct tMIXERLINE {
    DWORD       cbStruct;               /* size of MIXERLINE structure */
    DWORD       dwDestination;          /* zero based destination index */
    DWORD       dwSource;               /* zero based source index (if source) */
    DWORD       dwLineID;               /* unique line id for mixer device */
    DWORD       fdwLine;                /* state/information about line */
    DWORD       dwUser;                 /* driver specific information */
    DWORD       dwComponentType;        /* component type line connects to */
    DWORD       cChannels;              /* number of channels line supports */
    DWORD       cConnections;           /* number of connections [possible] */
    DWORD       cControls;              /* number of controls at this line */
    TCHAR       szShortName[MIXER_SHORT_NAME_CHARS];
    TCHAR       szName[MIXER_LONG_NAME_CHARS];
    struct {
	DWORD   dwType;                 /* MIXERLINE_TARGETTYPE_xxxx */
	DWORD   dwDeviceID;             /* target device ID of device type */
	WORD    wMid;                   /* of target device */
	WORD    wPid;                   /*      " */
	MMVERSION vDriverVersion;         /*      " */
	TCHAR   szPname[MAXPNAMELEN];   /*      " */
    } Target;
} MIXERLINE, *PMIXERLINE, FAR *LPMIXERLINE;

/* */
/*  MIXERLINE.fdwLine */
/* */
/* */
#define MIXERLINE_LINEF_ACTIVE              0x00000001L
#define MIXERLINE_LINEF_DISCONNECTED        0x00008000L
#define MIXERLINE_LINEF_SOURCE              0x80000000L

/* */
/*  MIXERLINE.dwComponentType */
/* */
/*  component types for destinations and sources */
/* */
/* */
#define MIXERLINE_COMPONENTTYPE_DST_FIRST       0x00000000L
#define MIXERLINE_COMPONENTTYPE_DST_UNDEFINED   (MIXERLINE_COMPONENTTYPE_DST_FIRST + 0)
#define MIXERLINE_COMPONENTTYPE_DST_DIGITAL     (MIXERLINE_COMPONENTTYPE_DST_FIRST + 1)
#define MIXERLINE_COMPONENTTYPE_DST_LINE        (MIXERLINE_COMPONENTTYPE_DST_FIRST + 2)
#define MIXERLINE_COMPONENTTYPE_DST_MONITOR     (MIXERLINE_COMPONENTTYPE_DST_FIRST + 3)
#define MIXERLINE_COMPONENTTYPE_DST_SPEAKERS    (MIXERLINE_COMPONENTTYPE_DST_FIRST + 4)
#define MIXERLINE_COMPONENTTYPE_DST_HEADPHONES  (MIXERLINE_COMPONENTTYPE_DST_FIRST + 5)
#define MIXERLINE_COMPONENTTYPE_DST_TELEPHONE   (MIXERLINE_COMPONENTTYPE_DST_FIRST + 6)
#define MIXERLINE_COMPONENTTYPE_DST_WAVEIN      (MIXERLINE_COMPONENTTYPE_DST_FIRST + 7)
#define MIXERLINE_COMPONENTTYPE_DST_VOICEIN     (MIXERLINE_COMPONENTTYPE_DST_FIRST + 8)
#define MIXERLINE_COMPONENTTYPE_DST_LAST        (MIXERLINE_COMPONENTTYPE_DST_FIRST + 8)

#define MIXERLINE_COMPONENTTYPE_SRC_FIRST       0x00001000L
#define MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED   (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 0)
#define MIXERLINE_COMPONENTTYPE_SRC_DIGITAL     (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 1)
#define MIXERLINE_COMPONENTTYPE_SRC_LINE        (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 2)
#define MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE  (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 3)
#define MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 4)
#define MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 5)
#define MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE   (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 6)
#define MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER   (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 7)
#define MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT     (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 8)
#define MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY   (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 9)
#define MIXERLINE_COMPONENTTYPE_SRC_ANALOG      (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 10)
#define MIXERLINE_COMPONENTTYPE_SRC_LAST        (MIXERLINE_COMPONENTTYPE_SRC_FIRST + 10)

/* */
/*  MIXERLINE.Target.dwType */
/* */
/* */
#define MIXERLINE_TARGETTYPE_UNDEFINED      0
#define MIXERLINE_TARGETTYPE_WAVEOUT        1
#define MIXERLINE_TARGETTYPE_WAVEIN         2
#define MIXERLINE_TARGETTYPE_MIDIOUT        3
#define MIXERLINE_TARGETTYPE_MIDIIN         4
#define MIXERLINE_TARGETTYPE_AUX            5


MMRESULT WINAPI mixerGetLineInfo(HMIXEROBJ hmxobj, LPMIXERLINE pmxl, DWORD fdwInfo);

#define MIXER_GETLINEINFOF_DESTINATION      0x00000000L
#define MIXER_GETLINEINFOF_SOURCE           0x00000001L
#define MIXER_GETLINEINFOF_LINEID           0x00000002L
#define MIXER_GETLINEINFOF_COMPONENTTYPE    0x00000003L
#define MIXER_GETLINEINFOF_TARGETTYPE       0x00000004L

#define MIXER_GETLINEINFOF_QUERYMASK        0x0000000FL

MMRESULT WINAPI mixerGetID(HMIXEROBJ hmxobj, UINT FAR *puMxId, DWORD fdwId);

/* */
/*  MIXERCONTROL */
/* */
/* */
typedef struct tMIXERCONTROL {
    DWORD           cbStruct;           /* size in bytes of MIXERCONTROL */
    DWORD           dwControlID;        /* unique control id for mixer device */
    DWORD           dwControlType;      /* MIXERCONTROL_CONTROLTYPE_xxx */
    DWORD           fdwControl;         /* MIXERCONTROL_CONTROLF_xxx */
    DWORD           cMultipleItems;     /* if MIXERCONTROL_CONTROLF_MULTIPLE set */
    TCHAR           szShortName[MIXER_SHORT_NAME_CHARS];
    TCHAR           szName[MIXER_LONG_NAME_CHARS];
    union {
	struct {
	    LONG    lMinimum;           /* signed minimum for this control */
	    LONG    lMaximum;           /* signed maximum for this control */
	};
	struct {
	    DWORD   dwMinimum;          /* unsigned minimum for this control */
	    DWORD   dwMaximum;          /* unsigned maximum for this control */
	};
	DWORD       dwReserved[6];
    } Bounds;
    union {
	DWORD       cSteps;             /* # of steps between min & max */
	DWORD       cbCustomData;       /* size in bytes of custom data */
	DWORD       dwReserved[6];      /* !!! needed? we have cbStruct.... */
    } Metrics;
} MIXERCONTROL, *PMIXERCONTROL, FAR *LPMIXERCONTROL;

/* */
/*  MIXERCONTROL.fdwControl */
/* */
/* */
#define MIXERCONTROL_CONTROLF_UNIFORM   0x00000001L
#define MIXERCONTROL_CONTROLF_MULTIPLE  0x00000002L
#define MIXERCONTROL_CONTROLF_DISABLED  0x80000000L

/* */
/*  MIXERCONTROL_CONTROLTYPE_xxx building block defines */
/* */
/* */
#define MIXERCONTROL_CT_CLASS_MASK          0xF0000000L
#define MIXERCONTROL_CT_CLASS_CUSTOM        0x00000000L
#define MIXERCONTROL_CT_CLASS_METER         0x10000000L
#define MIXERCONTROL_CT_CLASS_SWITCH        0x20000000L
#define MIXERCONTROL_CT_CLASS_NUMBER        0x30000000L
#define MIXERCONTROL_CT_CLASS_SLIDER        0x40000000L
#define MIXERCONTROL_CT_CLASS_FADER         0x50000000L
#define MIXERCONTROL_CT_CLASS_TIME          0x60000000L
#define MIXERCONTROL_CT_CLASS_LIST          0x70000000L

#define MIXERCONTROL_CT_SUBCLASS_MASK       0x0F000000L

#define MIXERCONTROL_CT_SC_SWITCH_BOOLEAN   0x00000000L
#define MIXERCONTROL_CT_SC_SWITCH_BUTTON    0x01000000L

#define MIXERCONTROL_CT_SC_METER_POLLED     0x00000000L

#define MIXERCONTROL_CT_SC_TIME_MICROSECS   0x00000000L
#define MIXERCONTROL_CT_SC_TIME_MILLISECS   0x01000000L

#define MIXERCONTROL_CT_SC_LIST_SINGLE      0x00000000L
#define MIXERCONTROL_CT_SC_LIST_MULTIPLE    0x01000000L

#define MIXERCONTROL_CT_UNITS_MASK          0x00FF0000L
#define MIXERCONTROL_CT_UNITS_CUSTOM        0x00000000L
#define MIXERCONTROL_CT_UNITS_BOOLEAN       0x00010000L
#define MIXERCONTROL_CT_UNITS_SIGNED        0x00020000L
#define MIXERCONTROL_CT_UNITS_UNSIGNED      0x00030000L
#define MIXERCONTROL_CT_UNITS_DECIBELS      0x00040000L /* in 10ths */
#define MIXERCONTROL_CT_UNITS_PERCENT       0x00050000L /* in 10ths */

/* */
/*  Commonly used control types for specifying MIXERCONTROL.dwControlType */
/* */

#define MIXERCONTROL_CONTROLTYPE_CUSTOM         (MIXERCONTROL_CT_CLASS_CUSTOM | MIXERCONTROL_CT_UNITS_CUSTOM)
#define MIXERCONTROL_CONTROLTYPE_BOOLEANMETER   (MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_SIGNEDMETER    (MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_PEAKMETER      (MIXERCONTROL_CONTROLTYPE_SIGNEDMETER + 1)
#define MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER  (MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_BOOLEAN        (MIXERCONTROL_CT_CLASS_SWITCH | MIXERCONTROL_CT_SC_SWITCH_BOOLEAN | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_ONOFF          (MIXERCONTROL_CONTROLTYPE_BOOLEAN + 1)
#define MIXERCONTROL_CONTROLTYPE_MUTE           (MIXERCONTROL_CONTROLTYPE_BOOLEAN + 2)
#define MIXERCONTROL_CONTROLTYPE_MONO           (MIXERCONTROL_CONTROLTYPE_BOOLEAN + 3)
#define MIXERCONTROL_CONTROLTYPE_LOUDNESS       (MIXERCONTROL_CONTROLTYPE_BOOLEAN + 4)
#define MIXERCONTROL_CONTROLTYPE_STEREOENH      (MIXERCONTROL_CONTROLTYPE_BOOLEAN + 5)
#define MIXERCONTROL_CONTROLTYPE_BUTTON         (MIXERCONTROL_CT_CLASS_SWITCH | MIXERCONTROL_CT_SC_SWITCH_BUTTON | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_DECIBELS       (MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_DECIBELS)
#define MIXERCONTROL_CONTROLTYPE_SIGNED         (MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_UNSIGNED       (MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_PERCENT        (MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_PERCENT)
#define MIXERCONTROL_CONTROLTYPE_SLIDER         (MIXERCONTROL_CT_CLASS_SLIDER | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_PAN            (MIXERCONTROL_CONTROLTYPE_SLIDER + 1)
#define MIXERCONTROL_CONTROLTYPE_QSOUNDPAN      (MIXERCONTROL_CONTROLTYPE_SLIDER + 2)
#define MIXERCONTROL_CONTROLTYPE_FADER          (MIXERCONTROL_CT_CLASS_FADER | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_VOLUME         (MIXERCONTROL_CONTROLTYPE_FADER + 1)
#define MIXERCONTROL_CONTROLTYPE_BASS           (MIXERCONTROL_CONTROLTYPE_FADER + 2)
#define MIXERCONTROL_CONTROLTYPE_TREBLE         (MIXERCONTROL_CONTROLTYPE_FADER + 3)
#define MIXERCONTROL_CONTROLTYPE_EQUALIZER      (MIXERCONTROL_CONTROLTYPE_FADER + 4)
#define MIXERCONTROL_CONTROLTYPE_SINGLESELECT   (MIXERCONTROL_CT_CLASS_LIST | MIXERCONTROL_CT_SC_LIST_SINGLE | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_MUX            (MIXERCONTROL_CONTROLTYPE_SINGLESELECT + 1)
#define MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT (MIXERCONTROL_CT_CLASS_LIST | MIXERCONTROL_CT_SC_LIST_MULTIPLE | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_MIXER          (MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT + 1)
#define MIXERCONTROL_CONTROLTYPE_MICROTIME      (MIXERCONTROL_CT_CLASS_TIME | MIXERCONTROL_CT_SC_TIME_MICROSECS | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_MILLITIME      (MIXERCONTROL_CT_CLASS_TIME | MIXERCONTROL_CT_SC_TIME_MILLISECS | MIXERCONTROL_CT_UNITS_UNSIGNED)

/* */
/*  MIXERLINECONTROLS */
/* */
typedef struct tMIXERLINECONTROLS {
    DWORD           cbStruct;       /* size in bytes of MIXERLINECONTROLS */
    DWORD           dwLineID;       /* line id (from MIXERLINE.dwLineID) */
    union {
	DWORD       dwControlID;    /* MIXER_GETLINECONTROLSF_ONEBYID */
	DWORD       dwControlType;  /* MIXER_GETLINECONTROLSF_ONEBYTYPE */
    };
    DWORD           cControls;      /* count of controls pmxctrl points to */
    DWORD           cbmxctrl;       /* size in bytes of _one_ MIXERCONTROL */
    LPMIXERCONTROL  pamxctrl;       /* pointer to first MIXERCONTROL array */
} MIXERLINECONTROLS, *PMIXERLINECONTROLS, FAR *LPMIXERLINECONTROLS;

MMRESULT WINAPI mixerGetLineControls(HMIXEROBJ hmxobj, LPMIXERLINECONTROLS pmxlc, DWORD fdwControls);

#define MIXER_GETLINECONTROLSF_ALL          0x00000000L
#define MIXER_GETLINECONTROLSF_ONEBYID      0x00000001L
#define MIXER_GETLINECONTROLSF_ONEBYTYPE    0x00000002L

#define MIXER_GETLINECONTROLSF_QUERYMASK    0x0000000FL

typedef struct tMIXERCONTROLDETAILS {
    DWORD           cbStruct;       /* size in bytes of MIXERCONTROLDETAILS */
    DWORD           dwControlID;    /* control id to get/set details on */
    DWORD           cChannels;      /* number of channels in paDetails array */
    union {
	HWND        hwndOwner;      /* for MIXER_SETCONTROLDETAILSF_CUSTOM */
	DWORD       cMultipleItems; /* if _MULTIPLE, the number of items per channel */
    };
    DWORD           cbDetails;      /* size of _one_ details_XX struct */
    LPVOID          paDetails;      /* pointer to array of details_XX structs */
} MIXERCONTROLDETAILS, *PMIXERCONTROLDETAILS, FAR *LPMIXERCONTROLDETAILS;

/* */
/*  MIXER_GETCONTROLDETAILSF_LISTTEXT */
/* */
/* */

typedef struct tMIXERCONTROLDETAILS_LISTTEXT {
    DWORD           dwParam1;
    DWORD           dwParam2;
    TCHAR           szName[MIXER_LONG_NAME_CHARS];
} MIXERCONTROLDETAILS_LISTTEXT, *PMIXERCONTROLDETAILS_LISTTEXT, FAR *LPMIXERCONTROLDETAILS_LISTTEXT;

/* */
/*  MIXER_GETCONTROLDETAILSF_VALUE */
/* */
/* */
typedef struct tMIXERCONTROLDETAILS_BOOLEAN {
    LONG            fValue;
}       MIXERCONTROLDETAILS_BOOLEAN,
      *PMIXERCONTROLDETAILS_BOOLEAN,
 FAR *LPMIXERCONTROLDETAILS_BOOLEAN;

typedef struct tMIXERCONTROLDETAILS_SIGNED {
    LONG            lValue;
}       MIXERCONTROLDETAILS_SIGNED,
      *PMIXERCONTROLDETAILS_SIGNED,
 FAR *LPMIXERCONTROLDETAILS_SIGNED;

typedef struct tMIXERCONTROLDETAILS_UNSIGNED {
    DWORD           dwValue;
}       MIXERCONTROLDETAILS_UNSIGNED,
      *PMIXERCONTROLDETAILS_UNSIGNED,
 FAR *LPMIXERCONTROLDETAILS_UNSIGNED;

MMRESULT WINAPI mixerGetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);

#define MIXER_GETCONTROLDETAILSF_VALUE      0x00000000L
#define MIXER_GETCONTROLDETAILSF_LISTTEXT   0x00000001L

#define MIXER_GETCONTROLDETAILSF_QUERYMASK  0x0000000FL

MMRESULT WINAPI mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);

#define MIXER_SETCONTROLDETAILSF_VALUE      0x00000000L
#define MIXER_SETCONTROLDETAILSF_CUSTOM     0x00000001L

#define MIXER_SETCONTROLDETAILSF_QUERYMASK  0x0000000FL

#endif /* ifndef MMNOMIXER */

#ifndef MMNOTIMER
/****************************************************************************

                            Timer support

****************************************************************************/

/* timer error return values */
#define TIMERR_NOERROR        (0)                  /* no error */
#define TIMERR_NOCANDO        (TIMERR_BASE+1)      /* request not completed */
#define TIMERR_STRUCT         (TIMERR_BASE+33)     /* time struct size */

/* timer data types */
typedef void (CALLBACK TIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef TIMECALLBACK FAR *LPTIMECALLBACK;

/* flags for fuEvent parameter of timeSetEvent() function */
#define TIME_ONESHOT    0x0000   /* program timer for single event */
#define TIME_PERIODIC   0x0001   /* program for continuous periodic event */

#ifdef _WIN32
#define TIME_CALLBACK_FUNCTION      0x0000  /* callback is function */
#define TIME_CALLBACK_EVENT_SET     0x0010  /* callback is event - use SetEvent */
#define TIME_CALLBACK_EVENT_PULSE   0x0020  /* callback is event - use PulseEvent */
#define TIME_CALLBACK_TYPEMASK      0x00F0  /* Internal */
#endif


/* timer device capabilities data structure */
typedef struct timecaps_tag {
    UINT    wPeriodMin;     /* minimum period supported  */
    UINT    wPeriodMax;     /* maximum period supported  */
} TIMECAPS, *PTIMECAPS, NEAR *NPTIMECAPS, FAR *LPTIMECAPS;

/* timer function prototypes */
WINMMAPI MMRESULT WINAPI timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt);
WINMMAPI DWORD WINAPI timeGetTime(void);
WINMMAPI MMRESULT WINAPI timeSetEvent(UINT uDelay, UINT uResolution,
    LPTIMECALLBACK fptc, DWORD dwUser, UINT fuEvent);
WINMMAPI MMRESULT WINAPI timeKillEvent(UINT uTimerID);
WINMMAPI MMRESULT WINAPI timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc);
WINMMAPI MMRESULT WINAPI timeBeginPeriod(UINT uPeriod);
WINMMAPI MMRESULT WINAPI timeEndPeriod(UINT uPeriod);
WINMMAPI DWORD WINAPI timeGetTimeSinceInterrupt(void);
WINMMAPI DWORD WINAPI timeGetHardwareFrequency(void);

#endif  /* ifndef MMNOTIMER */


#ifndef MMNOMMIO
/****************************************************************************

                        Multimedia File I/O support

****************************************************************************/

/* MMIO error return values */
#define MMIOERR_BASE                256
#define MMIOERR_FILENOTFOUND        (MMIOERR_BASE + 1)  /* file not found */
#define MMIOERR_OUTOFMEMORY         (MMIOERR_BASE + 2)  /* out of memory */
#define MMIOERR_CANNOTOPEN          (MMIOERR_BASE + 3)  /* cannot open */
#define MMIOERR_CANNOTCLOSE         (MMIOERR_BASE + 4)  /* cannot close */
#define MMIOERR_CANNOTREAD          (MMIOERR_BASE + 5)  /* cannot read */
#define MMIOERR_CANNOTWRITE         (MMIOERR_BASE + 6)  /* cannot write */
#define MMIOERR_CANNOTSEEK          (MMIOERR_BASE + 7)  /* cannot seek */
#define MMIOERR_CANNOTEXPAND        (MMIOERR_BASE + 8)  /* cannot expand file */
#define MMIOERR_CHUNKNOTFOUND       (MMIOERR_BASE + 9)  /* chunk not found */
#define MMIOERR_UNBUFFERED          (MMIOERR_BASE + 10) /*  */
#define MMIOERR_PATHNOTFOUND        (MMIOERR_BASE + 11) /* path incorrect */
#define MMIOERR_ACCESSDENIED        (MMIOERR_BASE + 12) /* file was protected */
#define MMIOERR_SHARINGVIOLATION    (MMIOERR_BASE + 13) /* file in use */
#define MMIOERR_NETWORKERROR        (MMIOERR_BASE + 14) /* network not responding */
#define MMIOERR_TOOMANYOPENFILES    (MMIOERR_BASE + 15) /* no more file handles  */
#define MMIOERR_INVALIDFILE         (MMIOERR_BASE + 16) /* default error file error */

/* MMIO constants */
#define CFSEPCHAR       '+'             /* compound file name separator char. */

/* MMIO data types */
typedef DWORD           FOURCC;         /* a four character code */
typedef char _huge *    HPSTR;          /* a huge version of LPSTR */
DECLARE_HANDLE(HMMIO);                  /* a handle to an open file */
typedef LRESULT (CALLBACK MMIOPROC)(LPSTR lpmmioinfo, UINT uMsg,
            LPARAM lParam1, LPARAM lParam2);
typedef MMIOPROC FAR *LPMMIOPROC;


/* general MMIO information data structure */
typedef struct _MMIOINFO
{
        /* general fields */
        DWORD           dwFlags;        /* general status flags */
        FOURCC          fccIOProc;      /* pointer to I/O procedure */
        LPMMIOPROC      pIOProc;        /* pointer to I/O procedure */
        UINT            wErrorRet;      /* place for error to be returned */
        HTASK           htask;          /* alternate local task */

        /* fields maintained by MMIO functions during buffered I/O */
        LONG            cchBuffer;      /* size of I/O buffer (or 0L) */
        HPSTR           pchBuffer;      /* start of I/O buffer (or NULL) */
        HPSTR           pchNext;        /* pointer to next byte to read/write */
        HPSTR           pchEndRead;     /* pointer to last valid byte to read */
        HPSTR           pchEndWrite;    /* pointer to last byte to write */
        LONG            lBufOffset;     /* disk offset of start of buffer */

        /* fields maintained by I/O procedure */
        LONG            lDiskOffset;    /* disk offset of next read or write */
        DWORD           adwInfo[3];     /* data specific to type of MMIOPROC */

        /* other fields maintained by MMIO */
        DWORD           dwReserved1;    /* reserved for MMIO use */
        DWORD           dwReserved2;    /* reserved for MMIO use */
        HMMIO           hmmio;          /* handle to open file */
} MMIOINFO, *PMMIOINFO, NEAR *NPMMIOINFO, FAR *LPMMIOINFO;
typedef const MMIOINFO FAR *LPCMMIOINFO;

/* RIFF chunk information data structure */
typedef struct _MMCKINFO
{
        FOURCC          ckid;           /* chunk ID */
        DWORD           cksize;         /* chunk size */
        FOURCC          fccType;        /* form type or list type */
        DWORD           dwDataOffset;   /* offset of data portion of chunk */
        DWORD           dwFlags;        /* flags used by MMIO functions */
} MMCKINFO, *PMMCKINFO, NEAR *NPMMCKINFO, FAR *LPMMCKINFO;
typedef const MMCKINFO *LPCMMCKINFO;

/* bit field masks */
#define MMIO_RWMODE     0x00000003      /* open file for reading/writing/both */
#define MMIO_SHAREMODE  0x00000070      /* file sharing mode number */

/* constants for dwFlags field of MMIOINFO */
#define MMIO_CREATE     0x00001000      /* create new file (or truncate file) */
#define MMIO_PARSE      0x00000100      /* parse new file returning path */
#define MMIO_DELETE     0x00000200      /* create new file (or truncate file) */
#define MMIO_EXIST      0x00004000      /* checks for existence of file */
#define MMIO_ALLOCBUF   0x00010000      /* mmioOpen() should allocate a buffer */
#define MMIO_GETTEMP    0x00020000      /* mmioOpen() should retrieve temp name */

#define MMIO_DIRTY      0x10000000      /* I/O buffer is dirty */

#define MMIO_OPEN_VALID 0x0003FFFF      /* valid flags for mmioOpen */ /* Internal */
#define MMIO_FLUSH_VALID MMIO_EMPTYBUF  /* valid flags for mmioFlush */ /* Internal */
#define MMIO_ADVANCE_VALID (MMIO_WRITE | MMIO_READ)     /* valid flags for mmioAdvance */ /* Internal */
#define MMIO_FOURCC_VALID MMIO_TOUPPER  /* valid flags for mmioStringToFOURCC */ /* Internal */
#define MMIO_DESCEND_VALID (MMIO_FINDCHUNK | MMIO_FINDRIFF | MMIO_FINDLIST) /* Internal */
#define MMIO_CREATE_VALID (MMIO_CREATERIFF | MMIO_CREATELIST)   /* Internal */


/* read/write mode numbers (bit field MMIO_RWMODE) */
#define MMIO_READ       0x00000000      /* open file for reading only */
#define MMIO_WRITE      0x00000001      /* open file for writing only */
#define MMIO_READWRITE  0x00000002      /* open file for reading and writing */

/* share mode numbers (bit field MMIO_SHAREMODE) */
#define MMIO_COMPAT     0x00000000      /* compatibility mode */
#define MMIO_EXCLUSIVE  0x00000010      /* exclusive-access mode */
#define MMIO_DENYWRITE  0x00000020      /* deny writing to other processes */
#define MMIO_DENYREAD   0x00000030      /* deny reading to other processes */
#define MMIO_DENYNONE   0x00000040      /* deny nothing to other processes */

/* various MMIO flags */
#define MMIO_FHOPEN             0x0010  /* mmioClose: keep file handle open */
#define MMIO_EMPTYBUF           0x0010  /* mmioFlush: empty the I/O buffer */
#define MMIO_TOUPPER            0x0010  /* mmioStringToFOURCC: to u-case */
#define MMIO_INSTALLPROC    0x00010000  /* mmioInstallIOProc: install MMIOProc */
#define MMIO_GLOBALPROC     0x10000000  /* mmioInstallIOProc: install globally */
#define MMIO_REMOVEPROC     0x00020000  /* mmioInstallIOProc: remove MMIOProc */
#define MMIO_UNICODEPROC    0x01000000  /* mmioInstallIOProc: Unicode MMIOProc */
#define MMIO_FINDPROC       0x00040000  /* mmioInstallIOProc: find an MMIOProc */
#define MMIO_FINDCHUNK          0x0010  /* mmioDescend: find a chunk by ID */
#define MMIO_FINDRIFF           0x0020  /* mmioDescend: find a LIST chunk */
#define MMIO_FINDLIST           0x0040  /* mmioDescend: find a RIFF chunk */
#define MMIO_CREATERIFF         0x0020  /* mmioCreateChunk: make a LIST chunk */
#define MMIO_CREATELIST         0x0040  /* mmioCreateChunk: make a RIFF chunk */

#define MMIO_VALIDPROC      0x10070000  /* valid for mmioInstallIOProc */ /* Internal */

/* message numbers for MMIOPROC I/O procedure functions */
#define MMIOM_READ      MMIO_READ       /* read */
#define MMIOM_WRITE    MMIO_WRITE       /* write */
#define MMIOM_SEEK              2       /* seek to a new position in file */
#define MMIOM_OPEN              3       /* open file */
#define MMIOM_CLOSE             4       /* close file */
#define MMIOM_WRITEFLUSH        5       /* write and flush */

#if (WINVER >= 0x030a)
#define MMIOM_RENAME            6       /* rename specified file */
#endif /* ifdef WINVER >= 0x030a */

#define MMIOM_USER         0x8000       /* beginning of user-defined messages */

/* standard four character codes */
#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')

/* four character codes used to identify standard built-in I/O procedures */
#define FOURCC_DOS      mmioFOURCC('D', 'O', 'S', ' ')
#define FOURCC_MEM      mmioFOURCC('M', 'E', 'M', ' ')

/* flags for mmioSeek() */
#ifndef SEEK_SET
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */
#endif  /* ifndef SEEK_SET */

/* other constants */
#define MMIO_DEFAULTBUFFER      8192    /* default buffer size */

/* MMIO macros */
#define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)

#endif  /* ifndef MMNOMMIO */



#ifdef __cplusplus
}                       // End of extern "C" {
#endif  // __cplusplus

#include "poppack.h"        /* Revert to default packing */

#endif // _INC_MMSYSTEM
