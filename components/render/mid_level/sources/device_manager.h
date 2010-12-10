//forward declarations
class GeometryManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DeviceManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceManager  (const LowLevelDevicePtr& device, const char* driver_name, const char* adapter_name);
    ~DeviceManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    low_level::IDevice& Device ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* DriverName  ();
    const char* AdapterName ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� � ������ ������ �� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::GeometryManager* GeometryManager    ();
    void                        SetGeometryManager (GeometryManager*);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
