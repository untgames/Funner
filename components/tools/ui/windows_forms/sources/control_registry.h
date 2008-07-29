///////////////////////////////////////////////////////////////////////////////////////////////////
///Реестр контролов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Control> class ControlRegistry
{
  public:
    typedef xtl::intrusive_ptr<Control> ControlPtr;

///Регистрация контрола
    void Register (const char* id, const ControlPtr& control)
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ControlRegistry<T>::Register";

      if (!id)
        throw xtl::make_null_argument_exception (METHOD_NAME, "id");

      ControlMap::iterator iter = controls.find (id);

      if (iter != controls.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "id", id, "Control has already registered");

      controls.insert_pair (id, control);
    }
    
///Отмена регистрации контрола
    void Unregister (const char* id)
    {
      if (!id)
        return;

      controls.erase (id);
    }
    
///Поиск контрола
    Control* Find (const char* id) const
    {
      if (!id)
        return 0;
        
      ControlMap::const_iterator iter = controls.find (id);
      
      if (iter == controls.end ())
        return 0;
        
      return iter->second.get ();
    }
    
    ControlPtr Item (const char* id) const
    {
      static const char* METHOD_NAME = "tools::ui::windows_forms::ControlRegistry<T>::Item";

      if (!id)
        throw xtl::make_argument_exception (METHOD_NAME, "id");

      ControlMap::const_iterator iter = controls.find (id);

      if (iter == controls.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "id", id, "No control with this name");

      return iter->second.get ();
    }
    
///Количество контролов
    size_t Size () { return controls.size (); }

///Регистрация шлюзов
    static void RegisterInvokers (script::Environment& environment, const char* library_name)
    {
      using namespace script;

      InvokerRegistry& lib = environment.CreateLibrary (library_name);

        //регистрация свойств

      lib.Register ("get_ItemsCount", make_invoker (&ControlRegistry::Size));

        //регистрация методов

      lib.Register ("Find", make_invoker (&ControlRegistry::Find));
      lib.Register ("Item", make_invoker (&ControlRegistry::Item));

        //регистрация типов данных

      environment.RegisterType<ControlRegistry> (library_name);
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ControlPtr> ControlMap;

  private:
    ControlMap controls; //контролы
};
