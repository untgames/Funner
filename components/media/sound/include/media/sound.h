#ifndef MEDIALIB_SOUND_HEADER
#define MEDIALIB_SOUND_HEADER

#include <stl/auto_ptr>

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

namespace media
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
class ISoundInputStream
{
  public:
//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� size ������� ������� � position ������ � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read (size_t first_sample, size_t samples_count, void* data) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������, ������������ ��� ����������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ISoundInputStream* Clone () = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~ISoundInputStream () {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
//////////////////////////////////////////////////////////////////////////////////////////////////
struct SoundSampleInfo
{
  size_t samples_count;    //���������� ������� � �����
  size_t frequency;        //������� �������������
  size_t channels;         //���������� �������
  size_t bits_per_sample;  //���������� ����� �� ���� �����
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///����
//////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSample
{
  public:
             SoundSample  ();
             SoundSample  (ISoundInputStream*);
             SoundSample  (const SoundSample& source);
    explicit SoundSample  (const char* file_name);
             ~SoundSample ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SoundSample& operator = (const SoundSample&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����� ������������� ����� ������ (������ ���������� ���������� � ������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SamplesToBytes   (size_t sample_count) const;   //������� ���������� ������� � ������ � ������
    size_t BytesToSamples   (size_t byte_count)   const;   //������� ���������� ���� � ������ � �������
    double SamplesToSeconds (size_t sample_count) const;   //������� ���������� ������� � ������ � ��������
    size_t SecondsToSamples (double second_count) const;   //����������������� ������ � ������ � �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SamplesCount  () const; //���������� �������
    size_t SizeInBytes   () const; //������ ����� � ������
    size_t Frequency     () const; //���������� ������� � ������� (�������)
    size_t Channels      () const; //���������� �������
    size_t BitsPerSample () const; //���������� ����� �� �����
    double Duration      () const { return SamplesToSeconds (SamplesCount ()); } //�����������������
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data); //ret: ����������� ���������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Save (const char* file_name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SoundSample&);

  private:
    struct Impl;

    stl::auto_ptr<Impl> impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SoundSample&,SoundSample&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �������
//////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<SoundCodec* (const char* file_name, SoundSampleInfo& sound_sample_info), 
                                          void (const char* file_name, const SoundSample& sound_sample)> SoundSampleManager;

}

#endif
