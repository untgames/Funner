#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::System;

namespace syslib
{

namespace win8
{

/// Отображение кнопки 
Key get_key_code (VirtualKey key)
{
  switch (key)
  {
    default: return Key_Unknown;
    case VirtualKey::Back: return Key_Back;
    case VirtualKey::Tab: return Key_Tab;
    case VirtualKey::Enter: return Key_Enter;
    case VirtualKey::Shift: return Key_Shift;
    case VirtualKey::Control: return Key_Control;
    case VirtualKey::Menu: return Key_Menu;
    case VirtualKey::Pause: return Key_Pause;
    case VirtualKey::CapitalLock: return Key_CapsLock;
    case VirtualKey::Escape: return Key_Escape;
    case VirtualKey::Space: return Key_Space;
    case VirtualKey::PageUp: return Key_PageUp;
    case VirtualKey::PageDown: return Key_PageDown;
    case VirtualKey::End: return Key_End;
    case VirtualKey::Home: return Key_Home;
    case VirtualKey::Left: return Key_Left;
    case VirtualKey::Up: return Key_Up;
    case VirtualKey::Right: return Key_Right;
    case VirtualKey::Down: return Key_Down;
    case VirtualKey::Insert: return Key_Insert;
    case VirtualKey::Delete: return Key_Delete;
    case VirtualKey::Number0: return Key_0;
    case VirtualKey::Number1: return Key_1;
    case VirtualKey::Number2: return Key_2;
    case VirtualKey::Number3: return Key_3;
    case VirtualKey::Number4: return Key_4;
    case VirtualKey::Number5: return Key_5;
    case VirtualKey::Number6: return Key_6;
    case VirtualKey::Number7: return Key_7;
    case VirtualKey::Number8: return Key_8;
    case VirtualKey::Number9: return Key_9;
    case VirtualKey::A: return Key_A;
    case VirtualKey::B: return Key_B;
    case VirtualKey::C: return Key_C;
    case VirtualKey::D: return Key_D;
    case VirtualKey::E: return Key_E;
    case VirtualKey::F: return Key_F;
    case VirtualKey::G: return Key_G;
    case VirtualKey::H: return Key_H;
    case VirtualKey::I: return Key_I;
    case VirtualKey::J: return Key_J;
    case VirtualKey::K: return Key_K;
    case VirtualKey::L: return Key_L;
    case VirtualKey::M: return Key_M;
    case VirtualKey::N: return Key_N;
    case VirtualKey::O: return Key_O;
    case VirtualKey::P: return Key_P;
    case VirtualKey::Q: return Key_Q;
    case VirtualKey::R: return Key_R;
    case VirtualKey::S: return Key_S;
    case VirtualKey::T: return Key_T;
    case VirtualKey::U: return Key_U;
    case VirtualKey::V: return Key_V;
    case VirtualKey::W: return Key_W;
    case VirtualKey::X: return Key_X;
    case VirtualKey::Y: return Key_Y;
    case VirtualKey::Z: return Key_Z;
    case VirtualKey::NumberPad0: return Key_NumPad0;
    case VirtualKey::NumberPad1: return Key_NumPad1;
    case VirtualKey::NumberPad2: return Key_NumPad2;
    case VirtualKey::NumberPad3: return Key_NumPad3;
    case VirtualKey::NumberPad4: return Key_NumPad4;
    case VirtualKey::NumberPad5: return Key_NumPad5;
    case VirtualKey::NumberPad6: return Key_NumPad6;
    case VirtualKey::NumberPad7: return Key_NumPad7;
    case VirtualKey::NumberPad8: return Key_NumPad8;
    case VirtualKey::NumberPad9: return Key_NumPad9;
    case VirtualKey::Multiply: return Key_NumPadMultiply;
    case VirtualKey::Add: return Key_NumPadPlus;
    case VirtualKey::Separator: return Key_Slash;
    case VirtualKey::Subtract: return Key_NumPadMinus;
    case VirtualKey::Decimal: return Key_NumPadDot;
    case VirtualKey::Divide: return Key_NumPadDivide;
    case VirtualKey::F1: return Key_F1;
    case VirtualKey::F2: return Key_F2;
    case VirtualKey::F3: return Key_F3;
    case VirtualKey::F4: return Key_F4;
    case VirtualKey::F5: return Key_F5;
    case VirtualKey::F6: return Key_F6;
    case VirtualKey::F7: return Key_F7;
    case VirtualKey::F8: return Key_F8;
    case VirtualKey::F9: return Key_F9;
    case VirtualKey::F10: return Key_F10;
    case VirtualKey::F11: return Key_F11;
    case VirtualKey::F12: return Key_F12;
    case VirtualKey::NumberKeyLock: return Key_NumLock;
    case VirtualKey::Scroll: return Key_ScrollLock;
    case VirtualKey::LeftShift:
    case VirtualKey::RightShift: return Key_Shift;
    case VirtualKey::LeftControl: 
    case VirtualKey::RightControl: return Key_Control;
    case VirtualKey::LeftMenu:
    case VirtualKey::RightMenu: return Key_Menu;
  }
}

}

}
