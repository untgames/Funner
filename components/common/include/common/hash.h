#ifndef COMMONLIB_HASH_HEADER
#define COMMONLIB_HASH_HEADER

#include <stddef.h>

namespace common
{

/*
        CRC32
*/

size_t crc32 (const void* data,size_t size,size_t init_value=0xFFFFFFFF);

/*
        Adler32
*/

size_t adler32 (const void* data,size_t size,size_t init_value=1);

/*
        MD5
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст для последовательного хэширования
///////////////////////////////////////////////////////////////////////////////////////////////////
class MD5Context
{
  public:
     MD5Context  ();
     ~MD5Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление состояния контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
     void Update (const void* buf,size_t len);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс контекста с формированием результирующего хэш-значения
///////////////////////////////////////////////////////////////////////////////////////////////////     
     void Finish (unsigned char result_hash_value [16]);
     
  private:     
    struct Impl;
    Impl* impl;
};

//хеширование блока памяти
void md5 (unsigned char result_hash_value [16],const void* buf,size_t len);

/*
    Хэширование строк
*/

size_t strhash  (const char*,size_t init_hash=0xFFFFFFFF);
size_t strihash (const char*,size_t init_hash=0xFFFFFFFF);
size_t strhash  (const wchar_t*,size_t init_hash=0xFFFFFFFF);
size_t strihash (const wchar_t*,size_t init_hash=0xFFFFFFFF);
size_t strnhash  (const char*,size_t length,size_t init_hash=0xFFFFFFFF);
size_t strnihash (const char*,size_t length,size_t init_hash=0xFFFFFFFF);
size_t strnhash  (const wchar_t*,size_t length,size_t init_hash=0xFFFFFFFF);
size_t strnihash (const wchar_t*,size_t length,size_t init_hash=0xFFFFFFFF);


}

#endif
