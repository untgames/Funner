#ifndef SOUNDLIB_SHARED_HEADER
#define SOUNDLIB_SHARED_HEADER

#include <vorbis/vorbisfile.h>

#include <common/exception.h>
#include <common/singleton.h>
#include <common/file.h>
#include <common/xml_writer.h>

#include <stl/vector>
#include <stl/string>
#include <stl/hash_set> //??убрать!!!
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <media/sound.h>
#include <media/sound_declaration.h>

#include <shared/clone.h>
#include <shared/resource_library.h>
#include <shared/resource_manager.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации звукового сэмпла
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSampleImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    stl::string               str_name;  //SoundSample name
    SoundSampleInfo           info;      //Информация о файле
    stl::auto_ptr<SoundCodec> codec;     //Кодек
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация звуковой системы
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSampleSystemImpl
{
  public:
    typedef media::SoundSampleSystem::CodecLoadFunc CodecLoadFunc;
    typedef media::SoundSampleSystem::DebugLogFunc  DebugLogFunc;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundSampleSystemImpl ();
    ~SoundSampleSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация открытых звуков / закрытие всех открытых звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterSoundSample   (media::SoundSample&);
    void UnregisterSoundSample (media::SoundSample&);
    void CloseAllSoundSamples  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка и вызов пользовательской функции лога дебаг-сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDebugLog (const DebugLogFunc&);
    void DebugLog    (const char* debug_message);   

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация и получение пользовательских функций загрузки и чтения звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool           RegisterLoadFunc   (const char* extension, const CodecLoadFunc& load_codec);
    void           UnregisterLoadFunc (const char* extension);
    void           UnregisterAllFuncs ();
    media::SoundSampleSystem::CodecLoadFunc* GetLoadFunc(const char* extension);

  private:
    typedef stl::hash_set<media::SoundSample*>        OpenSoundSamplesSet;
    typedef stl::hash_map<stl::string, CodecLoadFunc> LoadCodecs;

  private:    
    DebugLogFunc        log_function;         //польовательская функция дебаг-лога
    OpenSoundSamplesSet open_sound_samples;   //список открытых звуков
    LoadCodecs          load_codecs;          //список пользовательских функций загрузки
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синглтон звуковой системы
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::Singleton<SoundSampleSystemImpl> SoundSystemSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера библиотек звуков
///////////////////////////////////////////////////////////////////////////////////////////////////
class SoundDeclarationLibraryManagerImpl: public ResourceManager<SoundDeclarationLibraryManager::LoadHandler, SoundDeclarationLibraryManager::SaveHandler>
{
  public:
    SoundDeclarationLibraryManagerImpl ();
};

typedef common::Singleton<SoundDeclarationLibraryManagerImpl> SoundDeclarationLibraryManagerSingleton;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение и загрузка SoundDeclarationLibrary
///////////////////////////////////////////////////////////////////////////////////////////////////
void snddecl_save_library (const char* file_name, const SoundDeclarationLibrary& library);
void snddecl_load_library (const char* file_name, SoundDeclarationLibrary& library);

}

#endif
