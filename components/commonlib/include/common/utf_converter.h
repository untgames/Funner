#ifndef COMMONLIB_UTF_CONVERTER_HEADER
#define COMMONLIB_UTF_CONVERTER_HEADER

namespace common
{
   class UtfConverter
   {
   public:
      enum EncodingType
      {
         ENCODING_ASCII7,
         ENCODING_UTF8,
         ENCODING_UTF16BE,
         ENCODING_UTF16LE,
         ENCODING_UTF32BE,
         ENCODING_UTF32LE
      };
	UtfConverter();
	bool decode(const void* src, int srcLen, int* srcBytes, Char32* dst, int src_type, int dst_type);
	bool encode(void* dst, int dstLen, int* dstBytes, Char32 src, int src_type, int dst_type);
	bool decode(const void* src,Char32* dst, int src_type, int dst_type;
	bool encode(void* dst,int* dstBytes, int src_type, int dst_type);
   };
}
#endif 
