#include "shared.h"

#import <UIApplication.h>
#import <UIGeometry.h>
#import <UITextField.h>
#import <UIWindow.h>

using namespace input::low_level::iphone_driver;

namespace
{

NSString* EMPTY_STRING_REPLACE = @" ";

const size_t MESSAGE_BUFFER_SIZE = 16;
const float  EPSILON             = 0.01f;

const char* ORIENTATION_PROPERTY = "Orientation";

const char* PROPERTIES [] = {
  ORIENTATION_PROPERTY
};

const size_t PROPERTIES_COUNT = sizeof (PROPERTIES) / sizeof (*PROPERTIES);

enum Orientation
{
  Orientation_Portrait,
  Orientation_LandscapeLeft,
  Orientation_LandscapeRight
};

bool float_compare (float value1, float value2)
{
  return (value1 > (value2 - EPSILON)) && (value1 < (value2 + EPSILON));
}

UIInterfaceOrientation get_uikit_orientation (Orientation orientation)
{
  switch (orientation)
  {
    case Orientation_Portrait:       return UIInterfaceOrientationPortrait;
    case Orientation_LandscapeLeft:  return UIInterfaceOrientationLandscapeLeft;
    case Orientation_LandscapeRight: return UIInterfaceOrientationLandscapeRight;
    default: throw xtl::make_argument_exception ("input::low_level::iphone_driver::get_iukit_orientation", "orientation", orientation);
  }
}

Orientation get_orientation (UIInterfaceOrientation orientation)
{
  switch (orientation)
  {
    case UIInterfaceOrientationPortrait:       return Orientation_Portrait;
    case UIInterfaceOrientationLandscapeLeft:  return Orientation_LandscapeLeft;
    case UIInterfaceOrientationLandscapeRight: return Orientation_LandscapeRight;
    default: throw xtl::make_argument_exception ("input::low_level::iphone_driver::get_orientation", "orientation", orientation);
  }
}

// ласс, подписывающийс€ на обновление текста
class KeyboardListener
{
  public:
    //вызываетс€ при обновлении текста
    virtual void OnChar (unsigned short new_char) = 0;

    //вызываетс€ при обновлении текста
    virtual void KeyboardWasShown (float center_x, float center_y, float width, float height) = 0;
};

}

@interface TextFieldListener : NSObject <UITextFieldDelegate>
{
  @private
    UITextField*      text_field;
    KeyboardListener* listener;
    NSString*         last_text;            //текст, содержащийс€ в строке во врем€ последнего обновлени€
}

@property (nonatomic, assign) KeyboardListener *listener;
@property (nonatomic, copy)   NSString         *last_text;

- (id)initWithTextField:(UITextField*)in_text_field listener:(KeyboardListener*)in_listener;

@end

@implementation TextFieldListener

@synthesize listener, last_text;

- (id)init
{
  return [self initWithTextField:0 listener:0];
}

- (id)initWithTextField:(UITextField*)in_text_field listener:(KeyboardListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  if (!in_text_field)
  {
    [self release];
    return nil;
  }

  text_field     = in_text_field;
  listener       = in_listener;

  text_field.text = EMPTY_STRING_REPLACE;             //инициализаци€ текста в поле дл€ ввода, дл€ обработки нажати€ backspace
  self.last_text  = text_field.text;

  NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];

  [notificationCenter addObserver:self selector:@selector(keyboardWasShown:) name:UIKeyboardDidShowNotification object:nil];
  [notificationCenter addObserver:self selector:@selector(keyboardTextChanged) name:UITextFieldTextDidChangeNotification object:text_field];

  return self;
}

- (void)dealloc
{
  self.last_text = nil;

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

- (void)keyboardWasShown:(NSNotification*)notification
{
  if (!listener)
    return;

  NSDictionary* keyboard_info   = [notification userInfo];
  CGSize        keyboard_size   = [[keyboard_info objectForKey:UIKeyboardBoundsUserInfoKey] CGRectValue].size;
  CGPoint       keyboard_center = [[keyboard_info objectForKey:UIKeyboardCenterEndUserInfoKey] CGPointValue];

  listener->KeyboardWasShown (keyboard_center.x, keyboard_center.y, keyboard_size.width, keyboard_size.height);
}

- (void)keyboardTextChanged
{
  size_t new_text_length  = [text_field.text length],
         last_text_length = [last_text length];

  if (new_text_length == last_text_length)
    return;

  if (listener)
  {
    if (last_text_length > new_text_length)   //Backspace
    {
      listener->OnChar (8);

      if (![text_field.text length])
      {
        self.last_text  = EMPTY_STRING_REPLACE;
        text_field.text = EMPTY_STRING_REPLACE;  //поддерживаем текстовую строку всегда наполненной, чтобы всегда можно было обработать нажатие backspace
      }
    }
    else                                             //New char
      listener->OnChar ([text_field.text characterAtIndex:(new_text_length - 1)]);
  }

  self.last_text = text_field.text;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
  if (listener)
    listener->OnChar (13);

  return NO;
}

@end

struct IPhoneKeyboard::Impl : public KeyboardListener
{
  typedef xtl::signal<void (const char*)> DeviceSignal;

  stl::string       name;                  //им€ устройства
  stl::string       full_name;             //полное им€ устройства
  stl::string       properties;            //настройки
  DeviceSignal      signals;               //обработчики событий
  stl::wstring      control_name;          //им€ контрола
  UITextField       *text_field;           //вид, с помощью которого реализуетс€ работа с клавиатурой
  TextFieldListener *text_field_listener;  //слушатель сообщений текстового пол€

  /// онструктор / деструктор
  Impl (const char* in_name, const char* in_full_name)
    : name (in_name), full_name (in_full_name), text_field (0), text_field_listener (0)
  {
    try
    {
      for (size_t i = 0; i < PROPERTIES_COUNT; i++)
      {
        properties.append (PROPERTIES[i]);
        properties.append (" ");
      }

      if (!properties.empty ())
        properties.pop_back ();

      CGRect frame_rect;

      frame_rect.origin.x    = 1000;
      frame_rect.origin.y    = 1000;
      frame_rect.size.width  = 1;
      frame_rect.size.height = 1;

      text_field = [[UITextField alloc] initWithFrame:frame_rect];

      if (!text_field)
        throw xtl::format_operation_exception ("", "Can't create needed text field");

      [[UIApplication sharedApplication].keyWindow addSubview:text_field];

      text_field_listener = [[TextFieldListener alloc] initWithTextField:text_field listener:this];

      ShowKeyboard ();

      text_field.delegate = text_field_listener;
    }
    catch (xtl::exception& e)
    {
      [text_field release];
      [text_field_listener release];
      e.touch ("input::low_level::iphone_driver::IPhoneKeyboard::IPhoneKeyboard");
    }
  }

  ~Impl ()
  {
    text_field.delegate = nil;

    [text_field_listener release];

    try
    {
      HideKeyboard ();
    }
    catch (...)
    {
      //подавление исключений
    }

    [text_field release];
  }

  void ShowKeyboard ()
  {
    if (![text_field becomeFirstResponder])
      throw xtl::format_operation_exception ("input::low_level::iphone_driver::IPhoneKeyboard::ShowKeyboard", "Can't show keyboard");
  }

  void HideKeyboard ()
  {
    if (![text_field resignFirstResponder])
      xtl::format_operation_exception ("input::low_level::iphone_driver::IPhoneKeyboard::HideKeyboard", "Can't hide keyboard");
  }

  void OnChar (unsigned short new_char)
  {
    static char message[MESSAGE_BUFFER_SIZE];

    xtl::xsnprintf (message, MESSAGE_BUFFER_SIZE, "CharCode %u", new_char);
    signals (message);
  }

  void KeyboardWasShown (float center_x, float center_y, float width, float height)
  {
    //TODO set this values as properties
  }
};

/*
    онструктор/деструктор
*/

IPhoneKeyboard::IPhoneKeyboard (const char* name, const char* full_name)
  : impl (new Impl (name, full_name))
  {}

IPhoneKeyboard::~IPhoneKeyboard ()
{
  delete impl;
}

/*
   ѕолучение имени устройства
*/

const char* IPhoneKeyboard::GetName ()
{
  return impl->name.c_str ();
}

/*
   ѕолное им€ устройства (тип.им€.идентификатор)
*/

const char* IPhoneKeyboard::GetFullName ()
{
  return impl->full_name.c_str ();
}

/*
   ѕолучение имени контрола
*/

const wchar_t* IPhoneKeyboard::GetControlName (const char* control_id)
{
  impl->control_name = common::towstring (control_id);

  return impl->control_name.c_str ();
}

/*
   ѕодписка на событи€ устройства
*/

xtl::connection IPhoneKeyboard::RegisterEventHandler (const input::low_level::IDevice::EventHandler& handler)
{
  return impl->signals.connect (handler);
}

/*
   Ќастройки устройства
*/

const char* IPhoneKeyboard::GetProperties ()
{
  return impl->properties.c_str ();
}

void IPhoneKeyboard::SetProperty (const char* name, float value)
{
  static const char* METHOD_NAME = "input::low_level::iphone_driver::IPhoneKeyboard::SetProperty";

  if (!xtl::xstrcmp (ORIENTATION_PROPERTY, name))
  {
    if (float_compare (get_orientation ([UIApplication sharedApplication].statusBarOrientation), value))
      return;

    Orientation new_orientation;

    if (float_compare (Orientation_Portrait, value))
      new_orientation = Orientation_Portrait;
    else if (float_compare (Orientation_LandscapeLeft, value))
      new_orientation = Orientation_LandscapeLeft;
    else if (float_compare (Orientation_LandscapeRight, value))
      new_orientation = Orientation_LandscapeRight;
    else
      throw xtl::make_argument_exception (METHOD_NAME, "value", value);

    impl->HideKeyboard ();

    [UIApplication sharedApplication].statusBarOrientation = get_uikit_orientation (new_orientation);

    impl->ShowKeyboard ();
  }
  else
    throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

float IPhoneKeyboard::GetProperty (const char* name)
{
  if (!xtl::xstrcmp (ORIENTATION_PROPERTY, name))
    return get_orientation ([UIApplication sharedApplication].statusBarOrientation);
  else
    throw xtl::make_argument_exception ("input::low_level::iphone_driver::IPhoneKeyboard::GetProperty", "name", name);
}
