#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Внутренние структуры данных
*/

//вершинный атрибут в интерпретации OpenGL
struct InputLayout::GlVertexAttribute
{
  VertexAttributeSemantic semantic;   //семантика атрибута
  size_t                  components; //количество компонент
  GLenum                  type;       //тип элементов
  size_t                  offset;     //смещение относительно начала вершинного буфера
  size_t                  stride;     //шаг между атрибутами
};

//группа вершинных атрибутов в интерпретации OpenGL
struct InputLayout::GlVertexAttributeGroup
{
  size_t             slot;             //номер слота с вершинным буфером
  size_t             attributes_count; //количество вершинных атрибутов
  GlVertexAttribute* attributes;       //вершинные атрибуты
};

namespace
{

//предикат сравнения вершинных атрибутов
class VertexAttributeComparator
{
  public:
    VertexAttributeComparator (size_t in_position_slot) : position_slot (in_position_slot) {}

    bool operator () (const VertexAttribute* va1, const VertexAttribute* va2) const
    {
      size_t va1_slot = GetSlotIndex (va1->slot), va2_slot = GetSlotIndex (va2->slot);

      if (va1_slot == va2_slot)
        return va1->semantic > va2->semantic;

      return va1_slot < va2_slot;
    }

  private:
    size_t GetSlotIndex (size_t slot) const
    {
      static size_t MAX_SLOT_INDEX = DEVICE_VERTEX_BUFFER_SLOTS_COUNT + 1;

      return slot != position_slot ? slot : MAX_SLOT_INDEX;
    }

  private:  
    size_t position_slot;
};

//таблица свойств семантики вершинных атрибутов
class VertexAttributeSemanticTraits
{
  typedef common::Singleton<VertexAttributeSemanticTraits> ThisSingleton;
  public:
      //конструктор
    VertexAttributeSemanticTraits ()
    {
        //очистка таблицы совместимости

      memset (type_compatibility, 0, sizeof type_compatibility);
      memset (format_compatibility, 0, sizeof format_compatibility);

        //установка флагов совместимости

      Set (VertexAttributeSemantic_Position, InputDataType_Short);
      Set (VertexAttributeSemantic_Position, InputDataType_Int);
      Set (VertexAttributeSemantic_Position, InputDataType_Float);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector2);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector3);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector4);
      
      Set (VertexAttributeSemantic_Normal, InputDataType_Byte);
      Set (VertexAttributeSemantic_Normal, InputDataType_Short);
      Set (VertexAttributeSemantic_Normal, InputDataType_Int);
      Set (VertexAttributeSemantic_Normal, InputDataType_Float);
      Set (VertexAttributeSemantic_Normal, InputDataFormat_Vector3);
      
      Set (VertexAttributeSemantic_Color, InputDataType_Byte);
      Set (VertexAttributeSemantic_Color, InputDataType_UByte);
      Set (VertexAttributeSemantic_Color, InputDataType_Short);
      Set (VertexAttributeSemantic_Color, InputDataType_UShort);
      Set (VertexAttributeSemantic_Color, InputDataType_Int);
      Set (VertexAttributeSemantic_Color, InputDataType_UInt);
      Set (VertexAttributeSemantic_Color, InputDataType_Float);
      Set (VertexAttributeSemantic_Color, InputDataFormat_Vector3);
      Set (VertexAttributeSemantic_Color, InputDataFormat_Vector4);
      
      for (int i=VertexAttributeSemantic_TexCoord0; i<=VertexAttributeSemantic_TexCoord7; i++)
      {
        VertexAttributeSemantic semantic = (VertexAttributeSemantic)i;
        
        Set (semantic, InputDataType_Short);
        Set (semantic, InputDataType_Int);
        Set (semantic, InputDataType_Float);
        Set (semantic, InputDataFormat_Vector1);
        Set (semantic, InputDataFormat_Vector2);
        Set (semantic, InputDataFormat_Vector3);
        Set (semantic, InputDataFormat_Vector4);
      }
    }  
  
      //проверка совместимости семантики и типа данных элементов
    static bool IsCompatible (VertexAttributeSemantic semantic, InputDataType type)
    {
      return ThisSingleton::Instance ().type_compatibility [semantic][type];
    }

      //проверка совместимости семантики и формата элементов
    static bool IsCompatible (VertexAttributeSemantic semantic, InputDataFormat format)
    {
      return ThisSingleton::Instance ().format_compatibility [semantic][format];
    }

  private:
    void Set (VertexAttributeSemantic semantic, InputDataType type)
    {
      type_compatibility [semantic][type] = true;
    }
    
    void Set (VertexAttributeSemantic semantic, InputDataFormat format)
    {
      format_compatibility [semantic][format] = true;
    }  

  private:
    bool type_compatibility [VertexAttributeSemantic_Num][InputDataType_Num];
    bool format_compatibility [VertexAttributeSemantic_Num][InputDataFormat_Num];
};
  
}

/*
    Конструктор / деструктор
*/

InputLayout::InputLayout (const ContextManager& context_manager, const InputLayoutDesc& desc, size_t in_tex_units_count)
  : ContextObject (context_manager), tex_units_count (in_tex_units_count)
{
  try
  {
    SetDesc (desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputLayout::InputLayout");
    throw;
  }
}

InputLayout::~InputLayout ()
{
}

/*
    Установка дескриптора
*/

void InputLayout::SetDesc (const InputLayoutDesc& desc)
{ 
  static const char* METHOD_NAME = "render::low_level::InputLayout::SetDesc";
  
    //преобразование конфигурации индексного буфера
    
  GLenum new_index_data_type;
  size_t new_index_size;

  switch (desc.index_type)
  {
    case InputDataType_UByte:
      new_index_data_type = GL_UNSIGNED_BYTE;
      new_index_size      = sizeof (unsigned char);
      break;
    case InputDataType_UShort:
      new_index_data_type = GL_UNSIGNED_SHORT;
      new_index_size      = sizeof (unsigned short);
      break;
    case InputDataType_UInt:
      new_index_data_type = GL_UNSIGNED_INT;
      new_index_size      = sizeof (unsigned int);
      break;
    case InputDataType_Byte:
    case InputDataType_Short:
    case InputDataType_Int:
    case InputDataType_Float:
      RaiseNotSupported (METHOD_NAME, "desc.index_type", get_name (desc.index_type));
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.index_type", desc.index_type);
      break;
  }

      //создание нового массива вершинных атрибутов и массива групп вершинных атрибутов

  GlVertexAttributeArray      new_vertex_attributes;
  GlVertexAttributeGroupArray new_vertex_attribute_groups;

    //проверка наличия вершинных атрибутов
    
  if (desc.vertex_attributes_count)
  {
    if (!desc.vertex_attributes)
      RaiseNullArgument (METHOD_NAME, "desc.vertex_attributes");

      //установка текущего контекста
      
    MakeContextCurrent ();
    
      //проверка необходимых расширений
      
    static Extension ARB_multitexture = "GL_ARB_multitexture",
                     Version_1_3      = "GL_VERSION_1_3"; 

    bool has_arb_multitexture = IsSupported (ARB_multitexture) || IsSupported (Version_1_3);

      //предварительная проверка и сортировка вершинных атрибутов

    typedef xtl::array<size_t, VertexAttributeSemantic_Num> SemanticAttribute;
    typedef stl::vector<const VertexAttribute*>             VertexAttributePtrArray;

    VertexAttributePtrArray va_ptrs;
    SemanticAttribute       semantic_attribute;
    
    static const size_t NO_ATTRIBUTE = size_t (-1);
    
    semantic_attribute.assign (NO_ATTRIBUTE);

    va_ptrs.reserve (desc.vertex_attributes_count);
   
    for (size_t i=0; i<desc.vertex_attributes_count; i++)
    {
      const VertexAttribute& va = desc.vertex_attributes [i];

        //проверка корректности вершинной семантики

      switch (va.semantic)
      {
        case VertexAttributeSemantic_Position:
        case VertexAttributeSemantic_Normal:
        case VertexAttributeSemantic_Color:
        case VertexAttributeSemantic_TexCoord0:
          break;
        case VertexAttributeSemantic_TexCoord1:
        case VertexAttributeSemantic_TexCoord2:
        case VertexAttributeSemantic_TexCoord3:
        case VertexAttributeSemantic_TexCoord4:
        case VertexAttributeSemantic_TexCoord5:
        case VertexAttributeSemantic_TexCoord6:
        case VertexAttributeSemantic_TexCoord7:
        {
          if (!has_arb_multitexture)
            RaiseNotSupported (METHOD_NAME, "Bad desc.vertex_attribute[%u].semantic=%s (GL_ARB_multitexture & GL_VERSION_1_3 not supported)",
                               i, get_name (va.semantic));

          size_t tex_unit = va.semantic - VertexAttributeSemantic_TexCoord0;

          if (tex_unit >= tex_units_count)
            RaiseNotSupported (METHOD_NAME, "Bad desc.vertex_attribute[%u].semantic=%s (only %u texture units supported)",
                               i, get_name (va.semantic), tex_units_count);

          break;
        }
        default:
          Raise<ArgumentException> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].semantic=%d", i, va.semantic);
          break;
      }

        //отсечение повторной установки вершинной семантики           

      if (semantic_attribute [va.semantic] != NO_ATTRIBUTE)
        Raise<ArgumentException> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].semantic=%s (semantic has been "
          "already defined in attribute %u)", i, get_name (va.semantic), semantic_attribute [va.semantic]);

      semantic_attribute [va.semantic] = i;

        //проверка корректности формата данных и типа элементов

      switch (va.format)
      {
        case InputDataFormat_Value:
        case InputDataFormat_Vector2:
        case InputDataFormat_Vector3:
        case InputDataFormat_Vector4:
          if (!VertexAttributeSemanticTraits::IsCompatible (va.semantic, va.format))
            Raise<NotSupportedException> (METHOD_NAME, "Bad desc.vertex_attribute[%u] (semantic %s incompatible with format %s)", i,
                                          get_name (va.semantic), get_name (va.format));

          break;
        default:
          Raise<ArgumentException> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].format=%d", i, va.format);
          break;
      }
      
      switch (va.type)
      {
        case InputDataType_Byte:
        case InputDataType_UByte:
        case InputDataType_Short:
        case InputDataType_UShort:
        case InputDataType_Int:
        case InputDataType_UInt:
        case InputDataType_Float:
          if (!VertexAttributeSemanticTraits::IsCompatible (va.semantic, va.type))
            Raise<NotSupportedException> (METHOD_NAME, "Bad desc.vertex_attribute[%u] (semantic %s incompatible with type %s)", i,
                                          get_name (va.semantic), get_name (va.type));

          break;
        default:
          Raise<ArgumentException> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].type=%d", i, va.type);
          break;
      }
      
        //проверка корректности номера слота

      if (va.slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
        Raise<ArgumentOutOfRangeException> (METHOD_NAME, "Argument desc.vertex_attributes[%u].slot=%u is out of range [0;%u)",
                                            i, va.slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

        //добавление указателя на атрибут в массив

      va_ptrs.push_back (&va);    
    }
    
      //если среди вершинных атрибутов отсутствует атрибут с семантикой VertexAttributeSemantic_Position - установка невозможна
      
    size_t position_attribute = semantic_attribute [VertexAttributeSemantic_Position];

    if (position_attribute == NO_ATTRIBUTE)
      Raise<ArgumentException> (METHOD_NAME, "Invalid argument desc.vertex_attributes. Attribute with semantic 'VertexAttributeSemantic_Position' not found");

      //сортировка вершинных атрибутов для оптимизации установки в контекст OpenGL

    stl::sort (va_ptrs.begin (), va_ptrs.end (), VertexAttributeComparator (desc.vertex_attributes [position_attribute].slot));

      //преобразование вершинных атрибутов      

    new_vertex_attributes.resize (desc.vertex_attributes_count);
    new_vertex_attribute_groups.reserve (DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

    size_t current_slot = DEVICE_VERTEX_BUFFER_SLOTS_COUNT + 1; //номер текущего слота (для отслеживания изменения группы вершинных атрибутов)

    for (size_t i=0; i<desc.vertex_attributes_count; i++)
    {
        //преобразование вершинного атрибута

      const VertexAttribute& va    = *va_ptrs [i];
      GlVertexAttribute&     gl_va = new_vertex_attributes [i];

      gl_va.semantic = va.semantic;

      switch (va.format)
      {
        case InputDataFormat_Value:   gl_va.components = 1; break;
        case InputDataFormat_Vector2: gl_va.components = 2; break;
        case InputDataFormat_Vector3: gl_va.components = 3; break;
        case InputDataFormat_Vector4: gl_va.components = 4; break;
          break;
        default:
          RaiseInvalidOperation (METHOD_NAME, "Internal error at vertex attribute format convertation");
          break;
      }
      
      size_t type_size = 0;

      switch (va.type)
      {
        case InputDataType_Byte:
          gl_va.type = GL_BYTE;
          type_size  = sizeof (char);
          break;
        case InputDataType_UByte:
          gl_va.type = GL_UNSIGNED_BYTE;
          type_size  = sizeof (unsigned char);
          break;
        case InputDataType_Short:
          gl_va.type = GL_SHORT;
          type_size  = sizeof (short);
          break;
        case InputDataType_UShort:
          gl_va.type = GL_UNSIGNED_SHORT;
          type_size  = sizeof (unsigned short);
          break;
        case InputDataType_Int:
          gl_va.type = GL_INT;
          type_size  = sizeof (int);
          break;
        case InputDataType_UInt:
          gl_va.type = GL_UNSIGNED_INT;
          type_size  = sizeof (unsigned int);
          break;
        case InputDataType_Float:
          gl_va.type = GL_FLOAT;
          type_size  = sizeof (float);
          break;
        default:
          RaiseInvalidOperation (METHOD_NAME, "Internal error at vertex attribute type convertation");
          break;
      }
      
      gl_va.offset = va.offset;
      gl_va.stride = va.stride ? va.stride : type_size * gl_va.components;

        //отслеживание появления новой группы

      if (current_slot != va.slot)
      {
        new_vertex_attribute_groups.push_back ();

        GlVertexAttributeGroup& new_group = new_vertex_attribute_groups.back ();
        
        new_group.slot             = va.slot;
        new_group.attributes       = &gl_va;
        new_group.attributes_count = 0;
        current_slot               = va.slot;
      }
    }

      //установка размеров групп вершинных атрибутов

    if (new_vertex_attribute_groups.empty ())
      RaiseInvalidOperation (METHOD_NAME, "Internal error at build vertex attribute groups");

    for (GlVertexAttributeGroupArray::iterator iter=new_vertex_attribute_groups.begin (), end=new_vertex_attribute_groups.end ()-1; iter!=end; ++iter)
      iter->attributes_count = iter [1].attributes - iter [0].attributes;

    new_vertex_attribute_groups.back ().attributes_count = new_vertex_attributes.end () - new_vertex_attribute_groups.back ().attributes;    
  }  

    //установка текущего состояния объекта

  index_data_type     = new_index_data_type;
  index_size          = new_index_size;
  index_buffer_offset = desc.index_buffer_offset;

  new_vertex_attributes.swap (vertex_attributes);
  new_vertex_attribute_groups.swap (vertex_attribute_groups);
}

/*
    Установка состояния в контекст OpenGL
*/

void InputLayout::Bind
 (size_t         base_vertex,
  size_t         base_index,
  BufferPtr*     vertex_buffers,
  BufferPtr      index_buffer,
  IndicesLayout* out_indices_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::InputLayout::Bind";

    //установка текущего контекста

  MakeContextCurrent ();
  
    //проверка поддержки необходимых расширений
    
  static Extension ARB_multitexture = "GL_ARB_multitexture",
                   Version_1_3      = "GL_VERSION_1_3"; 
  
  bool                          has_arb_multitexture     = IsSupported (ARB_multitexture) || IsSupported (Version_1_3);
  PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture_fn = 0;
  
  if (has_arb_multitexture)
  {
    if (glClientActiveTexture) glClientActiveTexture_fn = glClientActiveTexture;
    else                       glClientActiveTexture_fn = glClientActiveTextureARB;
  }
  
    //проверка возможности установки вершинных буферов
  
  for (GlVertexAttributeGroupArray::iterator iter=vertex_attribute_groups.begin (), end=vertex_attribute_groups.end (); iter!=end; ++iter)
    if (!vertex_buffers [iter->slot])
      RaiseInvalidOperation (METHOD_NAME, "Null vertex buffer #%u", iter->slot);
      
    //отключение всех вершинных массивов !!! заменить на отключение только тех, которые не будут использованы - кеширование!!!
    
  static const GLenum clients_states [] = {GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_COLOR_ARRAY};
    
  for (size_t i=0; i<sizeof clients_states / sizeof *clients_states; i++)
    glDisableClientState (clients_states [i]);
    
  if (has_arb_multitexture)
  {
    for (size_t i=VertexAttributeSemantic_TexCoord0; i<=VertexAttributeSemantic_TexCoord7; i++)
    {
      glClientActiveTexture_fn (GL_TEXTURE0 + i - VertexAttributeSemantic_TexCoord0);
      glDisableClientState     (GL_TEXTURE_COORD_ARRAY);
    }
  }
  else
  {
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
  }  

    //настройка вершинных буферов    
    
  for (GlVertexAttributeGroupArray::iterator group_iter=vertex_attribute_groups.begin (), group_end=vertex_attribute_groups.end (); group_iter!=group_end; ++group_iter)
  {
      //установка вершинного буфера
      
    Buffer& vb = *vertex_buffers [group_iter->slot];
    
    vb.Bind ();
    
    const char* base_offset = (const char*)vb.GetDataPointer ();    
    
      //настройка вершинных атрибутов
      
    for (GlVertexAttribute *va_iter=group_iter->attributes, *va_end=va_iter+group_iter->attributes_count; va_iter!=va_end; ++va_iter)
    {
      const GlVertexAttribute& va     = *va_iter;
      const void*              offset = base_offset + va.offset + base_vertex * va.stride; //расчёт начального смещения вершиных атрибутов
      
      switch (va.semantic)
      {
        case VertexAttributeSemantic_Position:
          glVertexPointer (va.components, va.type, va.stride, offset);          
          glEnableClientState (GL_VERTEX_ARRAY);
          break;
        case VertexAttributeSemantic_Normal:
          glNormalPointer (va.type, va.stride, offset);
          glEnableClientState (GL_NORMAL_ARRAY);
          break;
        case VertexAttributeSemantic_Color:
          glColorPointer (va.components, va.type, va.stride, offset);
          glEnableClientState (GL_COLOR_ARRAY);
          break;
        case VertexAttributeSemantic_TexCoord0:
        case VertexAttributeSemantic_TexCoord1:
        case VertexAttributeSemantic_TexCoord2:
        case VertexAttributeSemantic_TexCoord3:
        case VertexAttributeSemantic_TexCoord4:
        case VertexAttributeSemantic_TexCoord5:
        case VertexAttributeSemantic_TexCoord6:
        case VertexAttributeSemantic_TexCoord7:          
        {
          if (has_arb_multitexture)
          {
            size_t tex_unit = va.semantic - VertexAttributeSemantic_TexCoord0;

            glClientActiveTexture_fn (GL_TEXTURE0 + tex_unit);
          }

          glTexCoordPointer (va.components, va.type, va.stride, offset);
          glEnableClientState (GL_TEXTURE_COORD_ARRAY);
          break;
        }
        default:
          continue;
      }
    }
  }

    //установка индексного буфера
  
  if (out_indices_layout)
  {
      //установка индексного буфера в контекст OpenGL

    if (!index_buffer)
      RaiseInvalidOperation (METHOD_NAME, "Null index buffer");

    index_buffer->Bind ();

      //расчёт смещения первого индекса относительно начала индексного буфера

    out_indices_layout->type = index_data_type;
    out_indices_layout->data = (char*)index_buffer->GetDataPointer () + index_buffer_offset + base_index * index_size;
  }
  
    //проверка ошибок
    
  CheckErrors (METHOD_NAME);
}
