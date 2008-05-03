#ifndef SYSLIB_KEY_DEFINITIONS_HEADER
#define SYSLIB_KEY_DEFINITIONS_HEADER

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Коды клавиш клавиатуры
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Key
{
  Key_Unknown,

  Key_F1,
  Key_F2, 
  Key_F3, 
  Key_F4, 
  Key_F5, 
  Key_F6, 
  Key_F7, 
  Key_F8, 
  Key_F9, 
  Key_F10, 
  Key_F11, 
  Key_F12,

  Key_Up,
  Key_Down,
  Key_Left,
  Key_Right,
  Key_PageUp,
  Key_PageDown,
  Key_Delete,
  Key_Insert,
  Key_Home,
  Key_End,

  Key_Enter,
  Key_Escape,
  Key_Pause,
  Key_Tab,
  Key_Space,
  Key_BackSpace,

  Key_Shift,
  Key_Control,
  Key_Alt,

  Key_NumLock,
  Key_ScrollLock,
  Key_CapsLock,
  
  Key_Tilda,        //~
  Key_Comma,        //,
  Key_Dot,          //.
  Key_Slash,        //'/'
  Key_BackSlash,    //'\'
  Key_Semicolon,    //;
  Key_Apostrophe,   //'
  Key_LeftBracket,  //[
  Key_RightBracket, //]
  Key_Plus,         //+
  Key_Minus,        //-

  Key_A, 
  Key_B, 
  Key_C, 
  Key_D, 
  Key_E, 
  Key_F, 
  Key_G, 
  Key_H, 
  Key_I, 
  Key_J, 
  Key_K,
  Key_L, 
  Key_M, 
  Key_N, 
  Key_O, 
  Key_P, 
  Key_Q, 
  Key_R, 
  Key_S, 
  Key_T, 
  Key_U, 
  Key_V,
  Key_W, 
  Key_X, 
  Key_Y, 
  Key_Z, 

  Key_0,
  Key_1, 
  Key_2, 
  Key_3, 
  Key_4, 
  Key_5, 
  Key_6, 
  Key_7, 
  Key_8, 
  Key_9,

  Key_NumPad0,
  Key_NumPad1, 
  Key_NumPad2, 
  Key_NumPad3, 
  Key_NumPad4, 
  Key_NumPad5, 
  Key_NumPad6, 
  Key_NumPad7, 
  Key_NumPad8, 
  Key_NumPad9,

  Key_NumPadPlus,      // +
  Key_NumPadMinus,     // -
  Key_NumPadMultiply,  // *
  Key_NumPadDivide,    // /
  Key_NumPadDot,       // .

  Key_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение скэн-кода
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ScanCode
{
  ScanCode_First,

  ScanCode_Num = 512
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени клавиши
///////////////////////////////////////////////////////////////////////////////////////////////////
const char* get_key_name      (Key);
const char* get_key_scan_name (ScanCode key_scan_code);

}

#endif
