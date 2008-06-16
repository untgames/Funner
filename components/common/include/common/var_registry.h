#ifndef COMMONLIB_VAR_REGISTRY_HEADER
#define COMMONLIB_VAR_REGISTRY_HEADER

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomVarRegistry
{
  public:
    virtual const char* GetValue (const char* var_name) = 0;
    virtual       void  SetValue (const char* var_name, const char* value) = 0;

    typedef xtl::function<void (const char* var_name)> EnumHandler;

    virtual void ForEach (const EnumHandler& handler) = 0;

    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ICustomVarRegistry ();
};

class VarRegistry
{
  public:
    VarRegistry ();
    VarRegistry (ICustomVarRegistry* registry, bool add_ref = true);
    VarRegistry (const VarRegistry& source);
    ~VarRegistry ();
    
    VarRegistry& operator = (const VarRegistry& source);

    const char* GetValue (const char* var_name) const;
          void  SetValue (const char* var_name, const char* value);

    bool HasVariable (const char* var_name) const;

    bool IsBinded () const;

    void Bind   (ICustomVarRegistry* registry);
    void Unbind ();

    typedef ICustomVarRegistry::EnumHandler EnumHandler;

    void ForEach (const EnumHandler& handler);
    void ForEach (const char* mask, const EnumHandler& handler);

  private:
    ICustomVarRegistry* impl;
};

class RegistryReference : public VarRegistry
{
  RegistryReference (const char* prefix, const VarRegistry& source);
};

class StringRegistry
{
  public:
    const char* GetValue (const char* var_name) const;
          void  SetValue (const char* var_name, const char* value);

    bool HasVariable (const char* var_name) const;


};

class CompositeVarRegistry
{
  public:
    void Attach (const char* prefix, const VarRegistry& registry);
    void Detach (const char* prefix);

    VarRegistry GetComposite () const;

  private:
    struct Impl;
    Impl* impl;
};

}

#endif