#include "shared.h"

using namespace syslib;

namespace
{

//constants
struct PlatformSpecificKeyboardDesc
{
  const char*    name;           //name which is used for keyboard creation
  UIKeyboardType type;           //native keyboard type
  NSString*      min_os_version; //if not null - keyboard supported from this version and higher
};

const PlatformSpecificKeyboardDesc SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS [] = {
 { "Default",      UIKeyboardTypeDefault,      0 },
 { "URL",          UIKeyboardTypeURL,          0 },
 { "PhonePad",     UIKeyboardTypePhonePad,     0 },
 { "NamePhonePad", UIKeyboardTypeNamePhonePad, 0 },
 { "EmailAddress", UIKeyboardTypeEmailAddress, 0 },
 { "DecimalPad",   UIKeyboardTypeDecimalPad,   0 },
 { "Twitter",      UIKeyboardTypeTwitter,      @"5.0" } };

}

NSString* EMPTY_STRING_REPLACE = @" ";

//Text fields changes listener
@interface TextFieldListener : NSObject <UITextFieldDelegate>
{
  @private
    UITextField*     text_field;    //text field
    UIWindowWrapper* parent_window; //events listener window
    NSString*        last_text;     //previous text_field's text
}

@property (nonatomic, retain) NSString *last_text;

@end

@implementation TextFieldListener

@synthesize last_text;

- (id)initWithTextField:(UITextField*)in_text_field parentWindow:(UIWindowWrapper*)in_parent_window
{
  self = [super init];

  if (!self)
    return nil;

  if (!in_text_field)
  {
    [self release];
    return nil;
  }

  text_field    = [in_text_field retain];
  parent_window = [in_parent_window retain];

  text_field.text = EMPTY_STRING_REPLACE;             //always keep text field with text, to handle backspace events
  self.last_text  = text_field.text;

  NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

  [notificationCenter addObserver:self selector:@selector(keyboardTextChanged) name:UITextFieldTextDidChangeNotification object:text_field];

  return self;
}

- (void)dealloc
{
  self.last_text = nil;

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [text_field    release];
  [parent_window release];

  [super dealloc];
}

- (void)keyboardTextChanged
{
  size_t new_text_length  = [text_field.text length],
         last_text_length = [last_text length];

  if (new_text_length == last_text_length)
    return;

  if (last_text_length > new_text_length)   //Backspace
  {
    [parent_window onCharInput:8];

    if (![text_field.text length])
    {
      self.last_text  = EMPTY_STRING_REPLACE;
      text_field.text = EMPTY_STRING_REPLACE;  //always keep text field with text, to handle backspace events
    }
  }
  else                                             //New char
    [parent_window onCharInput:[text_field.text characterAtIndex:(new_text_length - 1)]];

  self.last_text = text_field.text;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
  [parent_window onCharInput:13];

  return NO;
}

@end

namespace syslib
{

///keyboard descriptor
struct screen_keyboard_handle
{
  UITextField*       text_field;           //input field
  TextFieldListener* text_field_listener;  //text_field's events listener

  screen_keyboard_handle (window_t in_window, ScreenKeyboardType type, const char* platform_specific)
    : text_field (0)
    , text_field_listener (0)
  {
    try
    {
      CGRect frame_rect;

      frame_rect.origin.x    = -2;
      frame_rect.origin.y    = -2;
      frame_rect.size.width  = 1;
      frame_rect.size.height = 1;

      text_field = [[UITextField alloc] initWithFrame:frame_rect];

      if (!text_field)
        throw xtl::format_operation_exception ("", "Can't create needed text field");

      UIKeyboardType ui_keyboard_type;

      switch (type)
      {
        case ScreenKeyboardType_Ascii:
        case ScreenKeyboardType_AsciiAutocapitalized:
          ui_keyboard_type = UIKeyboardTypeASCIICapable;
          break;
        case ScreenKeyboardType_Number:
          ui_keyboard_type = UIKeyboardTypeNumberPad;
          break;
        case ScreenKeyboardType_NumberPunctuation:
          ui_keyboard_type = UIKeyboardTypeNumbersAndPunctuation;
          break;
        case ScreenKeyboardType_PlatformSpecific:
        {
          common::PropertyMap init_properties = common::parse_init_string (platform_specific);

          const char* name = init_properties.GetString ("name");

          if (!IPhoneScreenKeyboardManager::IsScreenKeyboardSupported (name))
            throw xtl::format_operation_exception ("", "Keyboard with name '%s' is not supported", name);

          for (size_t i = 0, count = sizeof (SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS) / sizeof (*SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS); i < count; i++)
          {
            const PlatformSpecificKeyboardDesc& keyboard_desc = SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS [i];

            if (!xtl::xstrcmp (keyboard_desc.name, name))
            {
              ui_keyboard_type = keyboard_desc.type;
              break;
            }
          }

          break;
        }
        default:
          throw xtl::make_argument_exception ("", "type", type);
      }

      text_field.autocapitalizationType = type == ScreenKeyboardType_AsciiAutocapitalized ? UITextAutocapitalizationTypeSentences : UITextAutocapitalizationTypeNone;
      text_field.autocorrectionType     = UITextAutocorrectionTypeNo;
      text_field.keyboardType           = ui_keyboard_type;

      UIWindowWrapper* window = (UIWindowWrapper*)in_window;

      [window addSubview:text_field];

      text_field_listener = [[TextFieldListener alloc] initWithTextField:text_field parentWindow:window];

      text_field.delegate = text_field_listener;
    }
    catch (xtl::exception& e)
    {
      [text_field release];
      [text_field_listener release];
      e.touch ("syslib::screen_keyboard_handle::screen_keyboard_handle");
      throw;
    }
  }

  //destructor
  ~screen_keyboard_handle ()
  {
    text_field.delegate = nil;

    [text_field_listener release];

    try
    {
      Hide ();
    }
    catch (...)
    {
      //ignore exceptions
    }

    [text_field release];
  }

  //show keyboard
  void Show ()
  {
    if (![text_field becomeFirstResponder])
      throw xtl::format_operation_exception ("syslib::screen_keyboard_handle::Show", "Can't show keyboard");
  }

  //hide keyboard
  void Hide ()
  {
    if (![text_field resignFirstResponder])
      xtl::format_operation_exception ("syslib::screen_keyboard_handle::Hide", "Can't hide keyboard");
  }
};

}

/*
   Создание и уничтожение экранной клавиатуры
*/

screen_keyboard_t IPhoneScreenKeyboardManager::CreateScreenKeyboard (window_t window, ScreenKeyboardType type, const char* platform_specific)
{
  return new screen_keyboard_handle (window, type, platform_specific);
}

void IPhoneScreenKeyboardManager::DestroyScreenKeyboard (screen_keyboard_t handle)
{
  delete handle;
}

/*
   Показ и скрытие клавиатуры
*/

void IPhoneScreenKeyboardManager::ShowScreenKeyboard (screen_keyboard_t handle)
{
  handle->Show ();
}

void IPhoneScreenKeyboardManager::HideScreenKeyboard (screen_keyboard_t handle)
{
  handle->Hide ();
}

/*
   Проверка поддержки клавиатуры
*/

bool IPhoneScreenKeyboardManager::IsScreenKeyboardSupported (ScreenKeyboardType type)
{
  switch (type)
  {
    case ScreenKeyboardType_Ascii:
    case ScreenKeyboardType_AsciiAutocapitalized:
    case ScreenKeyboardType_Number:
    case ScreenKeyboardType_NumberPunctuation:
    case ScreenKeyboardType_PlatformSpecific:
      return true;
    default:
      return false;
  }
}

bool IPhoneScreenKeyboardManager::IsScreenKeyboardSupported (const char* keyboard_name)
{
  for (size_t i = 0, count = sizeof (SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS) / sizeof (*SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS); i < count; i++)
  {
    const PlatformSpecificKeyboardDesc& keyboard_desc = SUPPORTED_PLATFORM_SPECIFIC_KEYBOARDS [i];

    if (!xtl::xstrcmp (keyboard_desc.name, keyboard_name))
    {
      if (!keyboard_desc.min_os_version)
        return true;

      NSString* system_version = [[UIDevice currentDevice] systemVersion];

      return [system_version compare:keyboard_desc.min_os_version options:NSNumericSearch] != NSOrderedAscending;
    }
  }

  return false;
}
