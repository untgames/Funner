/*
    ComponentRegistrator
*/

/*
    Конструктор / деструктор    
*/

template <class Component>
ComponentRegistrator<Component>::ComponentRegistrator (const char* name)
  : component (0)
{
  try
  {
    ComponentManager::Register (name ? name : "", this);
  }
  catch (...)
  {  
  }
}

template <class Component>
ComponentRegistrator<Component>::~ComponentRegistrator ()
{
  try
  {
    ComponentManager::Unregister (this);
  }
  catch (...)
  {
  }
}

/*
    Загрузка / выгрузка по умолчанию
*/

template <class Component>
void ComponentRegistrator<Component>::Load ()
{
  if (component)
    return;
    
  component = new Component;
}

template <class Component>
void ComponentRegistrator<Component>::Unload ()
{
  if (!component)
    return;

  delete component;

  component = 0;
}

/*
    ComponentLoader
*/

inline ComponentLoader::ComponentLoader (const char* wc_mask)
{
  ComponentManager::Load (wc_mask);
}
