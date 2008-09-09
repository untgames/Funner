#ifndef RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER
#define RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER

#include <render/low_level/driver.h>

#include <shared/gl_entries.h>
#include <shared/extensions.h>
#include <shared/context_caps.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы уровней устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum Stage
{
  Stage_Common,
  Stage_Input,
  Stage_Shading,
  Stage_Output,
  Stage_TextureManager,
  Stage_QueryManager,
  Stage_RenderTargetManager,

  Stage_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов, резервируемых в кэше состояния контекста для каждого из уровней
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t CONTEXT_CACHE_STAGE_ENTRIES_NUM = 64;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы кэшируемых состояний контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CacheEntry
{
  CacheEntry_SharedFirst = 0,

  CacheEntry_ActiveTextureSlot, //номер текущего активного слота текстурироания
  CacheEntry_EnabledTextures,   //маска: установлена ли текстура в определенном слоте
  CacheEntry_ColorWriteMask,    //маска записи в буфер цвета
  CacheEntry_DepthWriteEnable,  //включена ли запись в буфер глубины
  CacheEntry_StencilWriteMask,  //маска записи в буфер трафарета  
  CacheEntry_ScissorEnable,     //включен ли тест отсечения    

  CacheEntry_SharedLast = CacheEntry_SharedFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_InputStagePrivateFirst = CacheEntry_SharedLast,
  CacheEntry_InputStagePrivateLast  = CacheEntry_InputStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_ShadingStagePrivateFirst = CacheEntry_InputStagePrivateLast,
  CacheEntry_ShadingStagePrivateLast  = CacheEntry_ShadingStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_OutputStagePrivateFirst = CacheEntry_ShadingStagePrivateLast,
  CacheEntry_OutputStagePrivateLast  = CacheEntry_OutputStagePrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_TextureManagerPrivateFirst = CacheEntry_OutputStagePrivateLast,
  CacheEntry_TextureManagerPrivateLast  = CacheEntry_TextureManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_QueryManagerPrivateFirst = CacheEntry_TextureManagerPrivateLast,
  CacheEntry_QueryManagerPrivateLast  = CacheEntry_QueryManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_RenderTargetManagerPrivateFirst = CacheEntry_QueryManagerPrivateLast,
  CacheEntry_RenderTargetManagerPrivateLast  = CacheEntry_RenderTargetManagerPrivateFirst + CONTEXT_CACHE_STAGE_ENTRIES_NUM,

  CacheEntry_Num = CacheEntry_RenderTargetManagerPrivateLast
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер контекстов OpenGL (политика копирования - подсчёт ссылок)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextManager
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;    
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextManager  (ISwapChain* swap_chain, const char* init_string);
    ContextManager  (const ContextManager&);
    ~ContextManager ();

    ContextManager& operator = (const ContextManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание совместимой цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*);
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*, const SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выбор текущей цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetSwapChain (ISwapChain* swap_chain);
    ISwapChain* GetSwapChain () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Активация текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeContextCurrent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа кэшем текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    const size_t* GetContextCache      () const;
          size_t* GetContextCache      ();
          void    SetContextCacheValue (size_t entry_id, size_t value);
          size_t  GetContextCacheValue (size_t entry_id) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение информации о текущей реализации OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetExtensions () const;
    const char* GetVersion    () const;
    const char* GetVendor     () const;
    const char* GetRenderer   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение поддержки расширения контекстом
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSupported (const Extension& extension) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поулчение аппаратно поддерживаемых возможностей контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ContextCaps& GetCaps () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка совместимости контекста и цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (ISwapChain* swap_chain) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка совместимости менеджеров контекстов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (const ContextManager&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogPrintf  (const char* format, ...) const;
    void LogVPrintf (const char* format, va_list args) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка ошибок OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetValidationState (bool state); //включение / отключение проверки ошибок
    bool GetValidationState () const;
    void CheckErrors        (const char* source) const;
    void RaiseError         (const char* source) const;
    void ClearErrors        () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение о необходимости ребинда уровня / очистка флагов ребиндинга / проверка необходимости ребинда уровней
///////////////////////////////////////////////////////////////////////////////////////////////////
    void StageRebindNotify        (Stage);
    void ResetRebindNotifications ();
    bool NeedStageRebind          (Stage) const;

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
