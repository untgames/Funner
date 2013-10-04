namespace detail
{

template <class T>
inline Technique* create_technique (RenderManager& manager, const common::ParseNode& configuration)
{
  return new T (manager, configuration);
}

}

template <class T>
inline TechniqueRegistrator<T>::TechniqueRegistrator ()
{
  TechniqueManager::RegisterTechnique (T::ClassName (), &detail::create_technique<T>);
}

template <class T>
inline TechniqueComponentRegistrator<T>::TechniqueComponentRegistrator ()
  : ComponentRegistrator (T::ComponentName ())
{
}
