///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������: ��������� ����������� �� ������� ��������� � �������� ���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ForEachLightTechnique: public Technique
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ForEachLightTechnique  (RenderManager& manager, common::ParseNode& node);
    ~ForEachLightTechnique ();    
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateFrameCore (Scene& scene, Frame& frame, ITraverseResultCache& traverse_result_cache);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ResetPropertiesCore  ();
    void UpdatePropertiesCore (const common::PropertyMap&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};