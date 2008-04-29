#include <syslib/keydefs.h>
#include <platform/platform.h>

namespace syslib
{

const size_t KEY_NAME_MAP_STRING_SIZE = ScanCode_Num * 8; //длина строки имЄн клавиш

/*
    ѕолучение имени клавиши
*/

const char* get_key_scan_name (ScanCode scan_code)
{
  struct NameMap
  {
    char* names [ScanCode_Num];                   //имена клавиш
    char  name_string [KEY_NAME_MAP_STRING_SIZE]; //строка с именами

    NameMap ()
    {
      char *pos = name_string, *end = pos + KEY_NAME_MAP_STRING_SIZE;

      for (size_t i=0; i<ScanCode_Num; i++)
      {
        size_t max_size = end - pos,
               size     = Platform::GetKeyName ((ScanCode)i, max_size, pos);

        names [i] = size ? pos : "Unknown";

        pos += size + 1;
      }
    }
  };

  static NameMap name_map;

  return scan_code >= 0 && scan_code < ScanCode_Num ? name_map.names [scan_code] : "Unknown";
}

const char* get_key_name (Key key)
{
  switch (key)
  {
    case Key_F1:              return "F1";
    case Key_F2:              return "F2";
    case Key_F3:              return "F3";
    case Key_F4:              return "F4";
    case Key_F5:              return "F5";
    case Key_F6:              return "F6";
    case Key_F7:              return "F7";
    case Key_F8:              return "F8";
    case Key_F9:              return "F9";
    case Key_F10:             return "F10";
    case Key_F11:             return "F11";
    case Key_F12:             return "F12";

    case Key_Up:              return "Up";
    case Key_Down:            return "Down";
    case Key_Left:            return "Left";
    case Key_Right:           return "Right";
    case Key_PageUp:          return "Page Up";
    case Key_PageDown:        return "Page Down";
    case Key_Delete:          return "Delete";
    case Key_Insert:          return "Insert";
    case Key_Home:            return "Home";
    case Key_End:             return "End";

    case Key_Enter:           return "Enter"; 
    case Key_Escape:          return "Escape";
    case Key_Pause:           return "Pause";
    case Key_Tab:             return "Tab";
    case Key_Space:           return "Space";
    case Key_BackSpace:       return "Backspace";

    case Key_Shift:           return "Shift";
    case Key_Control:         return "Control";
    case Key_Alt:             return "Alt";

    case Key_NumLock:         return "Num Lock";
    case Key_ScrollLock:      return "Scroll Lock";
    case Key_CapsLock:        return "Caps Lock";
    
    case Key_Tilda:           return "~";
    case Key_Comma:           return ",";
    case Key_Dot:             return ".";
    case Key_Slash:           return "/";
    case Key_BackSlash:       return "\\";
    case Key_Semicolon:       return "\"";
    case Key_Apostrophe:      return "'";
    case Key_LeftBracket:     return "[";
    case Key_RightBracket:    return "]";
    case Key_Plus:            return "+";
    case Key_Minus:           return "-";

    case Key_A:               return "A";
    case Key_B:               return "B";
    case Key_C:               return "C";
    case Key_D:               return "D";
    case Key_E:               return "E";
    case Key_F:               return "F";
    case Key_G:               return "G";
    case Key_H:               return "H";
    case Key_I:               return "I";
    case Key_J:               return "J";
    case Key_K:               return "K";
    case Key_L:               return "L";
    case Key_M:               return "M";
    case Key_N:               return "N";
    case Key_O:               return "O";
    case Key_P:               return "P";
    case Key_Q:               return "Q";
    case Key_R:               return "R";
    case Key_S:               return "S";
    case Key_T:               return "T";
    case Key_U:               return "U";
    case Key_V:               return "V";
    case Key_W:               return "W";
    case Key_X:               return "X";
    case Key_Y:               return "Y";
    case Key_Z:               return "Z";

    case Key_0:               return "0";
    case Key_1:               return "1";
    case Key_2:               return "2";
    case Key_3:               return "3";
    case Key_4:               return "4";
    case Key_5:               return "5";
    case Key_6:               return "6";
    case Key_7:               return "7";
    case Key_8:               return "8";
    case Key_9:               return "9";

    case Key_NumPad0:         return "Num 0";
    case Key_NumPad1:         return "Num 1";
    case Key_NumPad2:         return "Num 2";
    case Key_NumPad3:         return "Num 3";
    case Key_NumPad4:         return "Num 4";
    case Key_NumPad5:         return "Num 5";
    case Key_NumPad6:         return "Num 6";
    case Key_NumPad7:         return "Num 7";
    case Key_NumPad8:         return "Num 8";
    case Key_NumPad9:         return "Num 9";

    case Key_NumPadPlus:      return "Num +";
    case Key_NumPadMinus:     return "Num -";
    case Key_NumPadMultiply:  return "Num *";
    case Key_NumPadDivide:    return "Num /";
    case Key_NumPadDot:       return "Num .";

    default:                  return "Unknown";
  }
}

}
