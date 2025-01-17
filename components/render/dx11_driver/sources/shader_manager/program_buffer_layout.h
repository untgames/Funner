///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание буфера программы
///////////////////////////////////////////////////////////////////////////////////////////////////
class ProgramBufferLayout: public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ProgramBufferLayout (const ConstantBufferLayoutPtr& in_layout, size_t reserved_count = 0)
      : layout (in_layout)
    {
      slots.reserve (reserved_count);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ConstantBufferLayout& GetLayout () const { return *layout; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание слота
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct Slot
    {      
      ShaderType type;
      size_t     slot;

      Slot (ShaderType in_type, size_t in_slot)
        : type (in_type)
        , slot (in_slot)
      {
      }
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление слотов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetSlotsCount () const { return slots.size (); }
    const Slot* GetSlots      () const { return slots.empty () ? (Slot*)0 : &slots [0]; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление слотов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSlot (const Slot& slot)
    {
      slots.push_back (slot);
    }

  private:
    ProgramBufferLayout (const ProgramBufferLayout&); //no implementation
    ProgramBufferLayout& operator = (const ProgramBufferLayout&); //no implementation

  private:
    typedef stl::vector<Slot> SlotArray;

  private:
    ConstantBufferLayoutPtr layout; //исходный лэйаут
    SlotArray               slots;  //слоты
};

typedef xtl::intrusive_ptr<ProgramBufferLayout> ProgramBufferLayoutPtr;
