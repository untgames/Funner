#ifndef RENDER_DX11_DRIVER_INPUT_MANAGER_HEADER
#define RENDER_DX11_DRIVER_INPUT_MANAGER_HEADER

#include <stl/auto_ptr.h>

#include <render/low_level/device.h>

#include <shared/common.h>
#include <shared/default_resources.h>
#include <shared/device_manager.h>
#include <shared/input_layout.h>

namespace render
{

namespace low_level
{

namespace dx11
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Состояние контекста менеджера входного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputManagerContextState  (const DeviceManager&);
    ~InputManagerContextState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление конфигурацией входных данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetInputLayout (InputLayout* state);
    InputLayout* GetInputLayout () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление вершинными буферами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void     SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer);
    IBuffer* GetVertexBuffer (size_t vertex_buffer_slot) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление индексным буфером
///////////////////////////////////////////////////////////////////////////////////////////////////
    void     SetIndexBuffer (IBuffer* buffer);
    IBuffer* GetIndexBuffer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление уровнем вывода вершин
///////////////////////////////////////////////////////////////////////////////////////////////////
    void     SOSetTargets      (size_t buffers_count, IBuffer** buffers, const size_t* offsets);
    void     SOSetTarget       (size_t stream_output_slot, IBuffer* buffer, size_t offset);
    IBuffer* SOGetTarget       (size_t stream_output_slot);
    size_t   SOGetTargetOffset (size_t stream_output_slot);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование состояния
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (const StateBlockMask&, InputManagerContextState& dst_state) const;

  protected:
    struct Impl;

    Impl& GetImpl () const;

    InputManagerContextState (Impl*);

  private:
    InputManagerContextState (const InputManagerContextState&); //no impl
    InputManagerContextState& operator = (const InputManagerContextState&); //no impl

  private:
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст менеджера входного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputManagerContext: public InputManagerContextState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputManagerContext  (const DeviceManager&, const DxContextPtr&, const DefaultResources&);
    ~InputManagerContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния уровня в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  protected:
    struct Impl;

    Impl& GetImpl () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер входного уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputManager  (const DeviceManager&);
    ~InputManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputLayout*  CreateInputLayout (const InputLayoutDesc& desc);
    IBuffer*      CreateBuffer      (const BufferDesc& desc, const void* data);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени семантики
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetVertexAttributeSemanticName (VertexAttributeSemantic semantic);

  private:
    InputManager (const InputManager&); //no impl
    InputManager& operator = (const InputManager&); //no impl

  private:  
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
