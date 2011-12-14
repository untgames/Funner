///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Cache: public xtl::noncopyable
{
  friend class CacheManager;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Cache  (const CacheManagerPtr&);
    ~Cache ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CurrentTime  () { return current_time; }    
    size_t CurrentFrame () { return current_frame; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t TimeDelay  ();
    size_t FrameDelay ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void FlushCache () = 0;
    
  private:
    CacheManagerPtr manager;
    size_t          current_time;
    size_t          current_frame;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class CacheManager: public Object
{
  friend class Cache;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CacheManager  ();
    ~CacheManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetTimeDelay  (size_t milliseconds);
    void   SetFrameDelay (size_t frames_count);
    size_t TimeDelay     ();
    size_t FrameDelay    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateMarkers ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void FlushCaches ();
    
  private:
    void AddCache    (Cache&);
    void RemoveCache (Cache&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
