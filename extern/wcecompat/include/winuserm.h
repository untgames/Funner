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

#ifndef _WINUSERM_H_
#define _WINUSERM_H_
//***   winuserm.h 
// DESCRIPTION
//  Mobile extensions to winuser.h

#ifdef __cplusplus
extern "C" {
#endif

//***  VK_T* Virtual keycodes for telephone keyboard

#if 0
// the phone keypad maps to VK_* as shown below.
// some keys might not be present, e.g. VK_TFLIP
    sofkey1     softkey2    VK_TSOFT1, VK_TSOFT2
            ^               VK_TUP
        <   +   >           VK_TLEFT, VK_TACTION, VK_TRIGHT
            v               VK_TDOWN
    home        back        VK_THOME, VK_TBACK
    talk        end         VK_TTALK, VK_TEND
    1       2       3       VK_T0..VK_T9
    4       5       6       ...
    7       8       9       ...
    *       0       #       VK_TSTAR, VK_TPOUND
// other buttons include
    VK_TRECORD
    VK_TPOWER, VK_TVOLUMEUP, VK_TVOLUMEDOWN
    VK_TFLIP
#endif

#define VK_TSOFT1   VK_F1               // Softkey 1
#define VK_TSOFT2   VK_F2               // Softkey 2
#define VK_TTALK    VK_F3               // Talk           
#define VK_TEND     VK_F4               // End           
#define VK_THOME    VK_LWIN             // Home         
#define VK_TBACK    VK_ESCAPE           // Back       

#define VK_TACTION      VK_RETURN       // Action       
#define VK_TRECORD      VK_F10          // Record
#define VK_TFLIP        VK_F17          // Flip
#define VK_TPOWER       VK_F18          // Power
#define VK_TVOLUMEUP    VK_F6           // Volume Up       
#define VK_TVOLUMEDOWN  VK_F7           // Volume Down       

#define VK_TUP      VK_UP               // Up 
#define VK_TDOWN    VK_DOWN             // Down             
#define VK_TLEFT    VK_LEFT             // Left
#define VK_TRIGHT   VK_RIGHT            // Right

#define VK_T0       L'0'                // 0 .. 9 keys
#define VK_T1       L'1'
#define VK_T2       L'2'
#define VK_T3       L'3'
#define VK_T4       L'4'
#define VK_T5       L'5'
#define VK_T6       L'6'
#define VK_T7       L'7'
#define VK_T8       L'8'
#define VK_T9       L'9'
#define VK_TSTAR    VK_F8               // *           
#define VK_TPOUND   VK_F9               // #   

#define VK_SYMBOL   VK_F11              // Symbol (SYM) key

#define VK_REDKEY   VK_F19		// Sent by a keypad that has a special red function key
#define VK_ROCKER   VK_F20              // Arrow keys came from a Rocker
#define VK_DPAD     VK_F21              // Arrow keys came from a DPAD
#define VK_ACTION   VK_F23              // Sent with VK_RETURN when doing Action on PPC rockers


// Chorded keys.  DONE is VK_RWIN + VK_F6 and MOJI is VK_RWIN + VK_F7 
#define VK_DONE     VK_F6                 // <OK/Close> hotkey code
#define MOD_DONE    (MOD_WIN | MOD_KEYUP) // <OK/Close> hotkey modifiers

#define VK_MOJI     VK_F7                  // Key used to switch between FE language layouts
#define MOD_MOJI    (MOD_WIN | MOD_KEYUP)  //Modifiers so MOJI goes to the shell

    
// App keys
#define VK_APP1     0xC1 
#define VK_APP2     0xC2 
#define VK_APP3     0xC3
#define VK_APP4     0xC4
#define VK_APP5     0xC5
#define VK_APP6     0xC6

// Specifies keys whose autorepeats should be silent. (Used by the keyboard driver.)
// All keys except the nav keys should have silent autorepeats.
#define SILENT_REPEAT(uVKey) (uVKey < VK_LEFT || uVKey > VK_DOWN)

// Edit control extensions for Mobile
#define EM_GETINPUTMODE       0x00DD    // If lParam is FALSE, gets default input mode. If lParam is TRUE, gets actual input mode if control has focus, else gets default input mode. Returns EIM_*|EIMMF_*
#define EM_SETINPUTMODE       0x00DE    // Sets default input mode when control gets focus. lParam should be be EIM_*|EIMMF_*.
#define EM_SETSYMBOLS         0x00DF    // Sets symbols that are displayed when the user presses the 1 key in Multipress mode. lParam is a null-teminated string containing the symbols or NULL to revert to standard symbols.
#define EM_SETEXTENDEDSTYLE   0x00E0    // Sets extended styles. wParam = Mask. lParam = Extended styles. Returns old styles.
#define EM_GETEXTENDEDSTYLE   0x00E1    // Gets extended styles.

#define Edit_GetInputMode(hwndCtl, fActual)     ((int)(DWORD)SNDMSG((hwndCtl), EM_GETINPUTMODE, 0L, (LPARAM)(BOOL)(fActual)))
#define Edit_SetInputMode(hwndCtl, nInputMode)  ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_SETINPUTMODE, 0L, (LPARAM)(int)(nInputMode)))
#define Edit_SetSymbols(hwndCtl, pszSymbols)    ((BOOL)(DWORD)SNDMSG((hwndCtl), EM_SETSYMBOLS, 0L, (LPARAM)(LPCTSTR)(pszSymbols)))
#define Edit_SetExtendedStyle(hwndCtl, dwMask, dwExStyle)    ((DWORD)SNDMSG((hwndCtl), EM_SETEXTENDEDSTYLE, (WPARAM)(DWORD)(dwMask), (LPARAM)(DWORD)(dwExStyle)))
#define Edit_GetExtendedStyle(hwndCtl)                       ((DWORD)SNDMSG((hwndCtl), EM_GETEXTENDEDSTYLE, 0L, 0L))

// Edit control extended styles
#define ES_EX_CLEARONBACKPRESSHOLD      0x00000001  // Clear all contents when user does Press&Hold of Back key.

// IME Input Modes
#define IM_SPELL           0
#define IM_AMBIG           1
#define IM_NUMBERS         2
#define IM_LAST            IM_NUMBERS
#define IM_MASK            0x0000FFFF       // Mask to access mode.

// IME Input Mode Modifier Flags
#define IMMF_SETCLR_SHIFT       0x00010000  // In. Used to set/clear Shift.
#define IMMF_SETCLR_CAPSLOCK    0x00020000  // In. Used to set/clear Capslock.   
#define IMMF_SHIFT              0x01000000  // In/Out. Shift state. 
#define IMMF_CAPSLOCK           0x02000000  // In/Out. CapsLock state.
#define IMMF_MASK               0xFFFF0000  // Mask to access modifier flags.

// Input modes for EM_SETINPUTMODE/EM_GETINPUTMODE
#define EIM_SPELL                   IM_SPELL                // Spell input mode                        
#define EIM_AMBIG                   IM_AMBIG                // Ambiguous mode if input engine supports this mode. Else defaults to Spell mode.
#define EIM_NUMBERS                 IM_NUMBERS              // Numbers mode.
#define EIM_TEXT                    (IM_LAST + 1)           // User's preferred mode. This is the user's last Spell or Ambig mode selection 
                                                                // in an EIM_TEXT edit control.
#define EIM_MASK                    IM_MASK                 // Mask to access mode.
#define EIM_MODE(x)                 (x & EIM_MASK)          // Macro to access mode

// Input mode modifier flags
#define EIMMF_SETCLR_SHIFT          IMMF_SETCLR_SHIFT       // In. Used to set/clear Shift.
#define EIMMF_SETCLR_CAPSLOCK       IMMF_SETCLR_CAPSLOCK    // In. Used to set/clear Capslock.
#define EIMMF_SHIFT                 IMMF_SHIFT              // In/Out. Shift state.    
#define EIMMF_CAPSLOCK              IMMF_CAPSLOCK           // In/Out. CapsLock state.
#define EIMMF_MASK                  IMMF_MASK               // Mask to access modifier flags.
#define EIM_MODIFIERS(x)            (x & EIMMF_MASK)        // Macro to access modifier flags

// Listbox control extensions for Mobile. Supported only for single selection listboxes.
#define LB_GETINPUTMODE       0x01C0    // If lParam is FALSE, gets default input mode. If lParam is TRUE, gets actual input mode if control has focus, else gets default input mode. Returns LIM_*
#define LB_SETINPUTMODE       0x01C1    // Sets default input mode when control gets focus. lParam should be be LIM_*.

#define ListBox_GetInputMode(hwndCtl, fActual)              ((int)(DWORD)SNDMSG((hwndCtl), LB_GETINPUTMODE, 0L, (WPARAM)(BOOL)(fActual)))
#define ListBox_SetInputMode(hwndCtl, nInputMode)           ((BOOL)(DWORD)SNDMSG((hwndCtl), LB_SETINPUTMODE, 0L, (LPARAM)(int)(nInputMode)))

// Input modes for LB_SETINPUTMODE/LB_GETINPUTMODE
#define LIM_SPELL                   IM_SPELL                // Spell input mode                        
#define LIM_NUMBERS                 IM_NUMBERS              // Numbers mode.

// wParam of  WM_IME_REQUEST. Extension for Mobile.
#define IMR_ISIMEAWARE           0x1000  // Is window IME aware?
// Return values of IMR_ISIMEAWARE
#define IMEAF_AWARE         0x00000001 // Window is IME aware
#define IMEAF_SMART_CAPS    0x00000002 // Window uses smart caps.

// Extension of WM_HOTKEY MOD_* values for Mobile
#define MOD_HOLD                0x8000 // Was WM_HOTKEY caused by Press&Hold

// Dialog message extensions
//
// Reset scroll info. To be sent by a dialog if it does a relayout of child
// controls after WM_INITDIALOG. Supported only if the dialog returns DLGC_SCROLL
// on WM_GETDLGCODE.
// wParam: TRUE to scroll back to top if user scrolled dialog. FALSE to leave 
// dialog in position that user scrolled to.
// lParam: TRUE to recompute scroll information. FALSE to use previously computed
// scroll information. 
#define DM_RESETSCROLL          (WM_USER+2) 

// This is temporarily placed here until OS starts handling this.
//           If you see this, and its late in the '04 release, tell JasonFul or EdERaffo
#ifndef MIIM_FULLSTR
#define MIIM_FULLSTR            0x10000000L
#else
#pragma error("OS now supports MIIM_FULLSTR. Get rid of this check/definition.")
#endif // MIIM_FULLSTR

// Predefined Clipboard Formats
#define CF_HDROP      15


#ifdef __cplusplus
}
#endif

#endif // _WINUSERM_H_
