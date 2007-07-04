#ifndef MEDIALIB_SOUND_HEADER
#define MEDIALIB_SOUND_HEADER

#include <stl/memory> // � ������� design/pointer.h
#include <design/function.h>
#include <common/file.h>

namespace medialib
{

//implementation forwards
class SoundSampleImpl;

//////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////
class SoundCodec
{
  public:
    virtual ~SoundCodec () {}

//////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������������� ����� ����������� size ������� ������� � position ������ � data
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read (size_t first_sample, size_t samples_count, void* data) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������, ������������ ��� ����������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////
    virtual SoundCodec* Clone () = 0;
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
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read (size_t first_sample, size_t samples_count, void* data); //ret: ����������� ���������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SoundSample&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ����� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static SoundCodec* DefaultWavLoader (const char* file_name, SoundSampleInfo& sound_sample_info);    
    static SoundCodec* DefaultOggLoader (const char* file_name, SoundSampleInfo& sound_sample_info);    

  private:
    SoundSample (SoundSampleImpl*);

    stl::auto_ptr<SoundSampleImpl> impl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///�����
//////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SoundSample&,SoundSample&);

//////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �������
//////////////////////////////////////////////////////////////////////////////////////////////////
class SoundSampleSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������������� ������� ���� �����-���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetDebugLog (const design::function<void (const char* message)>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef design::function<SoundCodec* (const char* file_name, SoundSampleInfo&)> CodecLoadFunc; //��������� ���������� � ����� � ������������� ������

    static bool RegisterCodec       (const char* extension, const CodecLoadFunc& loader);  //���������� true ��� �������� �����������, � false, ���� ����� ���������� ��� ����������������
    static void UnregisterCodec     (const char* extension);
    static void UnregisterAllCodecs ();
};

}

#endif
