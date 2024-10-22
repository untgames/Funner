#ifndef MEDIA_RMS_MANAGER_HEADER
#define MEDIA_RMS_MANAGER_HEADER

#include <media/rms/server.h>

namespace media
{

namespace rms
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер управления ресурсами
///////////////////////////////////////////////////////////////////////////////////////////////////
class ResourceManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание с серверами ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Binding CreateBinding (const Group& group);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор групп
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t           ServerGroupsCount ();
    static rms::ServerGroup ServerGroup       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс неиспользуемых ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void FlushUnusedResources ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание завершения асинхронных операций
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void WaitForAsyncComplete ();
};

}

}

#endif
