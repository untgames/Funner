#ifndef INPUT_SYSTEM_MANAGER_SHARED_HEADER
#define INPUT_SYSTEM_MANAGER_SHARED_HEADER

#include <stdio.h>

#include <stl/vector>
#include <stl/hash_map>
#include <stl/string>

#include <xtl/reference_counter.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>
#include <xtl/iterator.h>
#include <xtl/connection.h>
#include <xtl/signal.h>
#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/token_parser.h>
#include <xtl/common_exceptions.h>

#include <common/singleton.h>
#include <common/strlib.h>
#include <common/parser.h>
#include <common/xml_writer.h>
#include <common/component.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>
#include <input/translation_map.h>
#include <input/action_map.h>
#include <input/controls_detector.h>
#include <input/events_source.h>

namespace input
{

/*
    Транслятор событий
*/

class EventTranslator: public TranslationMap::Translator
{
  public:
      //конструктор/деструктор
    EventTranslator (const char* input_event, const char* event_replacement, const char* tag="");
    ~EventTranslator ();

      //выполнение замены
    bool Replace (const stl::vector<stl::string>& event_components, stl::string& result);

      //переопределение виртуальный функций - информация об объекте
    const char* InputEvent  () { return str_event_wildcard.c_str (); }
    const char* Replacement () { return str_event_replacement.c_str (); }
    const char* Tag         () { return str_tag.c_str (); }

  private:
      //Разбиение строки замены на составляющие
    void ParseFormatString (const char* prefix, const char* replacement_tag);

  private:
    struct Token;
    typedef stl::vector<Token> TokenArray;  

  private:
    stl::vector<stl::string> event_wildcard;
    TokenArray               replacement_tokens;
    stl::string              str_event_wildcard;
    stl::string              str_event_replacement;
    stl::string              str_tag;
};

void split_event (const char* event, stl::vector<stl::string>& target_arguments);

}

#endif
