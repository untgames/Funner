class InputLayoutManager;
class ProgramParametersManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DeviceManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceManager  (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver, const SettingsPtr& settings, const CacheManagerPtr& cache_manager);
    ~DeviceManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    low_level::IDevice&        Device           ();
    low_level::IDeviceContext& ImmediateContext ();
    low_level::IDriver&        Driver           ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const low_level::DeviceCaps& DeviceCaps ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::CacheManager& CacheManager ();
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::InputLayoutManager& InputLayoutManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::ProgramParametersManager& ProgramParametersManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::Settings& Settings ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
