/*
   Реализация базового реестра переменных
*/

namespace detail
{

class VarRegistryContainerImpl : public ICustomVarRegistry, public xtl::reference_counter
{
  public:
    ~VarRegistryContainerImpl ()
    {
      RemoveAllVariables ();
    }

/*
   Получение/установка данных
*/
    xtl::any& GetValueCore (const char* var_name)
    {
      static const char* METHOD_NAME = "VarRegistryContainer::GetValue";

      if (!var_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "var_name");

      VarMap::iterator iter = vars.find (var_name);

      if (iter == vars.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "var_name", var_name, "Variable not found");              

      return iter->second;
    }

    xtl::any GetValue (const char* var_name)
    {
      return GetValueCore (var_name);
    }

    xtl::any& SetValue (const char* var_name)
    {
      static const char* METHOD_NAME = "VarRegistryContainer::SetValue";

      if (!var_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "var_name");

      VarMap::iterator iter = vars.find (var_name);
      
      if (iter == vars.end ())
        Notify (var_name, VarRegistryEvent_OnCreateVar);
      else
        Notify (var_name, VarRegistryEvent_OnChangeVar);

      return vars[var_name];
    }

    void SetValue (const char* var_name, const xtl::any& value)
    {
      static const char* METHOD_NAME = "VarRegistryContainer::SetValue";

      if (!var_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "var_name");

      VarMap::iterator iter = vars.find (var_name);
      
      if (iter == vars.end ())
      {
        vars.insert_pair (var_name, value);

        Notify (var_name, VarRegistryEvent_OnCreateVar);

        return;
      }            

      vars [var_name] = value;

      Notify (var_name, VarRegistryEvent_OnChangeVar);
    }

/*
   Удаление переменной
*/
    void RemoveVariable (const char* var_name)
    {
      if (!var_name)
        return;

      VarMap::iterator iter = vars.find (var_name);

      if (iter == vars.end ())
        return;

      vars.erase (iter);

      Notify (var_name, VarRegistryEvent_OnDeleteVar);
    }

    void RemoveAllVariables ()
    {
      for (VarMap::iterator iter=vars.begin (); iter!=vars.end (); ++iter)
        Notify (iter->first.c_str (), VarRegistryEvent_OnDeleteVar);

      vars.clear ();
    }

/*
   Проверка наличия переменной
*/
    bool HasVariable (const char* var_name)
    {
      if (!var_name)
        return false;

      return vars.count (var_name) != 0;
    }

/*
   Обход всех переменных
*/
    typedef ICustomVarRegistry::EnumHandler EnumHandler;

    void EnumerateVars (const EnumHandler& handler)
    {
      for (VarMap::iterator iter=vars.begin (); iter!=vars.end (); ++iter)
        handler (iter->first.c_str ());
    }

/*
   Добавление/удаление ссылки
*/
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

/*
   Подписка на добавление/изменение/удаление переменных
*/
    typedef ICustomVarRegistry::EventHandler EventHandler;

    void SetEventHandler (const EventHandler& handler)
    {
      event_handler = handler;
    }

    const EventHandler& GetEventHandler ()
    {
      return event_handler;
    }

  private:
    void Notify (const char* var_name, VarRegistryEvent event)
    {
      if (!event_handler)
        return;

      try
      {
        event_handler (var_name, event);
      }
      catch (...)
      {
      }
    }

  private:
    typedef stl::hash_map<stl::string, xtl::any> VarMap;

  private:  
    VarMap       vars;
    EventHandler event_handler;   
};

}

/*
   Базовый реестр переменных
*/

/*
   Конструкторы / деструкторы
*/

template <class T>
VarRegistryContainer<T>::VarRegistryContainer ()
  : impl (new detail::VarRegistryContainerImpl ())
  {}

template <class T>
VarRegistryContainer<T>::VarRegistryContainer (const VarRegistryContainer<T>& source)
  : impl (source.impl)
{
  addref (impl);
}

template <class T>
VarRegistryContainer<T>::~VarRegistryContainer ()
{
  release (impl);
}

/*
   Копирование
*/

template <class T>
VarRegistryContainer<T>& VarRegistryContainer<T>::operator = (const VarRegistryContainer<T>& source)
{
  VarRegistryContainer<T> (source).Swap (*this);

  return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение/установка данных
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
const T& VarRegistryContainer<T>::GetValue (const char* var_name) const
{
  return any_cast<T&> (impl->GetValueCore (var_name));
}

template <class T>
void VarRegistryContainer<T>::SetValue (const char* var_name, const T& value)
{
  any_cast<T&> (impl->SetValue (var_name)) = value;
}

/*
   Удаление переменной
*/
template <class T>
void VarRegistryContainer<T>::RemoveVariable (const char* var_name)
{
  impl->RemoveVariable (var_name);
}

template <class T>
void VarRegistryContainer<T>::RemoveAllVariables ()
{
  impl->RemoveAllVariables ();
}

/*
   Проверка наличия переменной
*/
template <class T>
bool VarRegistryContainer<T>::HasVariable (const char* var_name) const
{
  return impl->HasVariable (var_name);
}

/*
   Присоединение/отсоединение реестра
*/
template <class T>
void VarRegistryContainer<T>::Mount (const char* branch_name)
{
  VarRegistrySystem::Mount (branch_name, impl);
}

template <class T>
void VarRegistryContainer<T>::Unmount (const char* branch_name)
{
  VarRegistrySystem::Unmount (branch_name, impl);
}

template <class T>
void VarRegistryContainer<T>::UnmountAll ()
{
  VarRegistrySystem::UnmountAll (impl);
}

/*
   Обмен
*/
template <class T>
void VarRegistryContainer<T>::Swap (VarRegistryContainer& source)
{
  stl::swap (impl, source.impl);
}

/*
   Обмен
*/

template <class T>
void swap (VarRegistryContainer<T>& source1, VarRegistryContainer<T>& source2)
{
  source1.Swap (source2);
}
