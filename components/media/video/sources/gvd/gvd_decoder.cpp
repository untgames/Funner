#include <cstring>

#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>

#include <common/component.h>
#include <common/file.h>

#include <media/video.h>

using namespace media;

#ifdef _MSC_VER
  typedef unsigned __int64 uint64;
#else
  typedef unsigned long long uint64;
#endif

namespace
{

/*
    Декодер кадра
*/

class GGFrameDecoder
{
  struct HuffTable;
  public:
    bool GetImageInfo
     (unsigned char* pInBuf,      //in, source data, in jpp format
      int            cbInBuf,     //in, count bytes for in buffer
      int&           nWidth,      //out, image width in pixels
      int&           nHeight,     //out, image height
      int&           nHeadSize)   //out, header size in bytes
    {
      //  image width and height, 4 bytes
      memcpy (&_nWidth, pInBuf, 2);
      
      pInBuf += 2;
      
      memcpy (&_nHeight, pInBuf, 2);
      
      pInBuf += 2;

      //  Write quality factor, 2 byte
      memcpy (&_nQuality, pInBuf, 2);
      
      pInBuf       += 2;
      _nDataBytes  = cbInBuf - 6;
      nHeadSize     = 6;

      if (_nWidth <= 0 || _nHeight <= 0)
        return false;

      nWidth  = _nWidth;
      nHeight = _nHeight;

      InitDecoder (); //on each call???

      return true;
    }
    
    //  DecompressImage(), the main function in this class
    //  IMPORTANT: You should call GetImageInfo() to get image width and height,
    //        Then allocate (_nWidth * _nHeight * 4) bytes for pOutBuf

    bool DecompressImage
     (unsigned char *pInBuf,   //in, source data, in jpg format
      int            outPitch, // in, dest buffer pitch
      unsigned char *pOutBuf)  //out, bgra format, (_nWidth * _nHeight * 4) bytes
    {
      //  Error handling

      if (pInBuf == 0 || pOutBuf == 0)
        return false;

      //  declares
      int xPixel, yPixel, xTile, yTile, cxTile, cyTile, cxBlock, cyBlock, y, nTrueRows, nTrueCols, nRowBytes;
      unsigned char byTile [1024], *pTileRow;

      //  horizontal and vertical count of tile, macroblocks, or MCU(Minimum Coded Unit), in 16*16 pixels
      cxTile = (_nWidth + 15) >> 4;  
      cyTile = (_nHeight + 15) >> 4;

      //  horizontal and vertical count of block, in 8*8 pixels
      cxBlock = cxTile << 1;
      cyBlock = cyTile << 1;

      //  BMP row width, must be divided by 4
      //  nRowBytes = (_nWidth * 4 + 3) / 4 * 4;
      nRowBytes = outPitch;

      _pData = pInBuf;

      for (yTile=0; yTile<cyTile; yTile++)
      {
        for (xTile=0; xTile<cxTile; xTile++)
        {
          //  Decompress one macroblock started from _pData;
          //  This function will push _pData ahead
          //  Result is storing in byTile
          
          if (!DecompressOneTile (byTile))
            return false;

          //  Get tile starting pixel position
          xPixel = xTile << 4;
          yPixel = yTile << 4;

          //  Get the true number of tile columns and rows
          nTrueRows = 16;
          nTrueCols = 16;
          
          if (yPixel + nTrueRows > _nHeight) nTrueRows = _nHeight - yPixel;
          if (xPixel + nTrueCols > _nWidth)  nTrueCols = _nWidth - xPixel;

          //  Write the output rgb data
          pTileRow = pOutBuf + (_nHeight - 1 - yPixel) * nRowBytes + xPixel * 4;
          
          for( y = 0; y < nTrueRows; y ++ )
          {            
            uint64 *s, *d;
            
            s = (uint64*)(byTile + y * 64);
            d = (uint64*)(pTileRow);

            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;

            pTileRow -= nRowBytes;      
          }
        }
      }
      return true;
    }    
    
  private:    

    /*
      Prepare for all the tables needed, eg. quantization tables, huff tables, color convert tables
      1 <= nQuality <= 100, is used for quantization scaling
      Computing once, and reuse them again and again
    */

    void InitDecoder ()
    {
      _nGetBits = 0;
      _nGetBuff = 0;

      _dcY = _dcA = _dcCb = _dcCr = 0;

      //  prepare range limiting table to limit idct outputs
      SetRangeTable ();

      //  prepare color convert table, from bgr to ycbcr
      InitColorTable ();

      //  prepare two quant tables, one for Y, and another for CbCr
      InitQuantTable ();

      //  prepare four huffman tables: 
      InitHuffmanTable ();
    }


    /*
      prepare_range_limit_table(): Set _tblRange[5*256+128 = 1408]
      range table is used for range limiting of idct results
      
      On most machines, particularly CPUs with pipelines or instruction prefetch,
      a (subscript-check-less) C table lookup                                    
          x = sample_range_limit[x];                                             
      is faster than explicit tests                                              
          if (x < 0)  x = 0;                                                     
          else if (x > MAXJSAMPLE)  x = MAXJSAMPLE;                                          
    */

    void SetRangeTable ()
    {
      unsigned char *tbl;

      //  _tblRange[0, ..., 255], limit[x] = 0 for x < 0
      memset (_tblRange, 0, 256);

      //  _tblRange[256, ..., 511], limit[x] = x
      tbl = _tblRange + 256;
      
      for (int i=0; i<256; i++)
        *(tbl++) = (unsigned char)i;

      // _tblRange[512, ..., 895]: first half of post-IDCT table
      tbl = _tblRange + 512;
      for (int i=128; i<512; i++)
        *(tbl++) = 255;

      //  _tblRange[896, ..., 1280]: Second half of post-IDCT table
      memset (_tblRange + 896, 0, 384);

      // [1280, 1407] = [256, 384]
      memcpy (_tblRange + 1280, _tblRange + 256, 128);
    }

    //YCbCr -> RGB conversion: most common case

    /*
       YCbCr is defined per CCIR 601-1, except that Cb and Cr are
       normalized to the range 0..MAXJSAMPLE rather than -0.5 .. 0.5.
       The conversion equations to be implemented are therefore
        R = Y                + 1.40200 * Cr
        G = Y - 0.34414 * Cb - 0.71414 * Cr
        B = Y + 1.77200 * Cb
       where Cb and Cr represent the incoming values less CENTERJSAMPLE.
       (These numbers are derived from TIFF 6.0 section 21, dated 3-June-92.)
      
       To avoid floating-point arithmetic, we represent the fractional constants
       as integers scaled up by 2^16 (about 4 digits precision); we have to divide
       the products by 2^16, with appropriate rounding, to get the correct answer.
       Notice that Y, being an integral input, does not contribute any fraction
       so it need not participate in the rounding.
      
       For even more speed, we avoid doing any multiplications in the inner loop
       by precalculating the constants times Cb and Cr for all possible values.
       For 8-bit JSAMPLEs this is very reasonable (only 256 entries per table);
       for 12-bit samples it is still acceptable.  It's not very reasonable for
       16-bit samples, but if you want lossless storage you shouldn't be changing
       colorspace anyway.
       The Cr=>R and Cb=>B values can be rounded to integers in advance; the
       values for the G calculation are left scaled up, since we must add them
       together before rounding.
     */

    void InitColorTable ()
    {
      int i, x;
      int nScale  = 1L << 16;   //equal to power(2,16)
      int nHalf = nScale >> 1;

      #define FIX(x) ((int) ((x) * nScale + 0.5))

      for (i = 0, x = -128; i < 256; i++, x++) 
      {
        _CrToR[i] = (int) ( FIX(1.40200) * x + nHalf ) >> 16;
        _CbToB[i] = (int) ( FIX(1.77200) * x + nHalf ) >> 16;
        _CrToG[i] = (int) (- FIX(0.71414) * x );
        _CbToG[i] = (int) (- FIX(0.34414) * x + nHalf );
      }
      
      #undef FIX
    }


    //InitQuantTable will produce customized quantization table into: _tblYQuant[0..63] and _tblCbCrQuant[0..63]
    void InitQuantTable ()
    {
      // These are the sample quantization tables given in JPEG spec section K.1.
      // The spec says that the values given produce "good" quality, and
      // when divided by 2, "very good" quality.  

      static unsigned short std_luminance_quant_tbl [64] = 
      {
          16,  11,  10,  16,  24,  40,  51,  61,
          12,  12,  14,  19,  26,  58,  60,  55,
          14,  13,  16,  24,  40,  57,  69,  56,
          14,  17,  22,  29,  51,  87,  80,  62,
          18,  22,  37,  56,  68, 109, 103,  77,
          24,  35,  55,  64,  81, 104, 113,  92,
          49,  64,  78,  87, 103, 121, 120, 101,
          72,  92,  95,  98, 112, 100, 103,  99
      };
      static unsigned short std_chrominance_quant_tbl [64] = 
      {
          17,  18,  24,  47,  99,  99,  99,  99,
          18,  21,  26,  66,  99,  99,  99,  99,
          24,  26,  56,  99,  99,  99,  99,  99,
          47,  66,  99,  99,  99,  99,  99,  99,
          99,  99,  99,  99,  99,  99,  99,  99,
          99,  99,  99,  99,  99,  99,  99,  99,
          99,  99,  99,  99,  99,  99,  99,  99,
          99,  99,  99,  99,  99,  99,  99,  99
      };


      /*  For AA&N IDCT method, divisors are equal to quantization coefficients scaled by scalefactor[row]*scalefactor[col], 
          where scalefactor[0] = 1, scalefactor[k] = cos(k*PI/16) * sqrt(2) for k=1..7
          We apply a further scale factor of 8.
      */  
      static unsigned short aanscales [64] = {
          /* precomputed values scaled up by 14 bits */
          16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
          22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
          21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
          19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
          16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
          12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
           8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
           4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
      };
      
      // Safety checking. Convert 0 to 1 to avoid zero divide. 
      _nScale = _nQuality;
      
      if (_nScale <= 0)  _nScale = 1;
      if (_nScale > 100) _nScale = 100;
      
      //  Non-linear map: 1->5000, 10->500, 25->200, 50->100, 75->50, 100->0
      if (_nScale < 50) _nScale = 5000 / _nScale;
      else               _nScale = 200 - _nScale*2;

      //  Scale the Y and CbCr quant table, respectively
      ScaleQuantTable (_qtblY,  std_luminance_quant_tbl, aanscales);
      ScaleQuantTable (_qtblCbCr, std_chrominance_quant_tbl, aanscales);
    }

    void ScaleQuantTable 
     (unsigned short* tblRst,   //result quant table
      unsigned short* tblStd,   //standard quant table
      unsigned short* tblAan)   //scale factor for AAN dct
    {
      int i, temp, half = 1<<11;

      for (i=0; i<64; i++) 
      {
        // (1) user scale up
        temp = (int)((_nScale * tblStd [i] + 50) / 100);

        // limit to baseline range 
        if (temp <= 0)  temp = 1;
        if (temp > 255) temp = 255;   

        // (2) scaling needed for AA&N algorithm
        tblRst [i] = (unsigned short)((temp * tblAan [i] + half ) >> 12);
      }
    }

    //Prepare four Huffman tables: HuffTable _htblYDC, _htblYAC, _htblCbCrDC, _htblCbCrAC;
    void InitHuffmanTable( void )
    {
      //  Y dc component
      static unsigned char bitsYDC [17] =  {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
      static unsigned char valYDC [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

      //  CbCr dc
      static unsigned char bitsCbCrDC [17] = {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
      static unsigned char valCbCrDC [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; 

      //  Y ac component
      static unsigned char bitsYAC [17] = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
      static unsigned char valYAC [] = { 
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
        0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
      };

      //  CbCr ac
      static unsigned char bitsCbCrAC[17] = {0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
      static unsigned char valCbCrAC[] = {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
        0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
      };

      //  Compute four derived Huffman tables

      ComputeHuffmanTable (bitsYDC, valYDC, &_htblYDC);
      ComputeHuffmanTable (bitsYAC, valYAC, &_htblYAC);
      ComputeHuffmanTable (bitsCbCrDC, valCbCrDC, &_htblCbCrDC);
      ComputeHuffmanTable (bitsCbCrAC, valCbCrAC, &_htblCbCrAC);
    }

    //  Compute the derived values for a Huffman table. 
    void ComputeHuffmanTable (unsigned char* pBits, unsigned char* pVal, HuffTable* dtbl)
    {
      int p, i, l, si;
      int lookbits, ctr;
      char huffsize[257];
      unsigned int huffcode[257];
      unsigned int code;

      memcpy( dtbl->bits, pBits, 17 );
      memcpy( dtbl->huffval, pVal, 256 );
      
      /* Figure C.1: make table of Huffman code length for each symbol */
      /* Note that this is in code-length order. */
      
      p = 0;
      
      for (l=1; l<=16; l++)
      {
        for (i=1; i<=(int)pBits[l]; i++)
          huffsize [p++] = (char) l;
      }

      huffsize [p] = 0;

      /* Figure C.2: generate the codes themselves */
      /* Note that this is in code-length order. */

      code = 0;
      si   = huffsize[0];
      p    = 0;

      while (huffsize[p]) 
      {
        while (((int)huffsize [p]) == si)
        {
          huffcode [p++] = code;
          code++;
        }

        code <<= 1;

        si++;
      }
      
      /* Figure F.15: generate decoding tables for bit-sequential decoding */
      
      p = 0;

      for (l = 1; l <= 16; l++)
      {
        if (pBits [l])
        {
          dtbl->valptr  [l] = p; /* huffval[] index of 1st symbol of code length l */
          dtbl->mincode [l] = huffcode[p]; /* minimum code of length l */

          p += pBits [l];

          dtbl->maxcode [l] = huffcode [p-1]; /* maximum code of length l */
        }
        else
        {
          dtbl->maxcode [l] = -1;  /* -1 if no codes of this length */
        }
      }
      
      dtbl->maxcode [17] = 0xFFFFFL; /* ensures jpeg_huff_decode terminates */
      
      /* Compute lookahead tables to speed up decoding.
         First we set all the table entries to 0, indicating "too long";
         then we iterate through the Huffman codes that are short enough and
         fill in all the entries that correspond to bit sequences starting
         with that code.   */
      
      memset (dtbl->look_nbits, 0, sizeof (int) * 256);

      int HUFF_LOOKAHEAD = 8;

      p = 0;

      for (l=1; l<=HUFF_LOOKAHEAD; l++) 
      {
        for (i=1; i<=(int) pBits[l]; i++, p++) 
        {
          /* l = current code's length, 
             p = its index in huffcode[] & huffval[]. Generate left-justified
             code followed by all possible bit sequences */

          lookbits = huffcode [p] << (HUFF_LOOKAHEAD-l);

          for (ctr = 1 << (HUFF_LOOKAHEAD-l); ctr > 0; ctr--) 
          {
            dtbl->look_nbits [lookbits] = l;
            dtbl->look_sym [lookbits]   = pVal [p];

            lookbits++;
          }
        }
      }
    }

    // function Purpose: decompress one 16*16 pixels, source is _pData;
    // This function will push _pData ahead for next tile

    bool DecompressOneTile (unsigned char* pBgra) //out, in BGR format, 16*16*4
    {
      unsigned char pYACbCr [448];//Three color components, 256 + 64 + 64 + 64 bytes 
      short coef [64]; 

      //Do Y/A/Cb/Cr components, Y: 4 blocks; A: 1 block; Cb: 1 block; Cr: 1 block
      for (int i=0; i<7; i++)
      {
        HuffmanDecode (coef, i); //source is _pData; coef is result
        InverseDct (coef, pYACbCr + i*64, i);  //De-scale and inverse dct    
      }

      //Color conversion and up-sampling

      YACbCrToBGRAEx (pYACbCr, pBgra);

      return true;
    }
    
    //Color conversion and up-sampling
    void YACbCrToBGRAEx
      (unsigned char* pYACbCr,  //in, Y: 256 bytes; A: 64 bytes; Cb: 64 bytes; Cr: 64 bytes 
       unsigned char* pBgra)    //out, BGR format, 16*16*3 = 768 bytes
    {
      int i, j;
      unsigned char y, cb, cr, a, *pByte, *py[4], *pa, *pcb, *pcr;

      pByte = pBgra;

      for (i=0; i<4; i++)
        py [i] = pYACbCr + i * 64;

      pa  = pYACbCr + 256;
      pcb = pYACbCr + 320;
      pcr = pYACbCr + 384;

      unsigned char* range_limit = _tblRange + 256;
      
      for (j=0; j<16; j++) //vertical axis
      {
        for (i=0; i<16; i++) //horizontal axis
        {     
          //  block number is ((j/8) * 2 + i/8): {0, 1, 2, 3}
          
          y  = *(py [(j>>3) * 2 + (i>>3)]++);        
          cb = pcb [j/2 * 8 + i/2];
          cr = pcr [j/2 * 8 + i/2];
          a  = pa [j/2 * 8 + i/2];

          *(pByte++) = range_limit[ y + _CrToR[cr] ]; //red          
          *(pByte++) = range_limit [y + ((_CbToG [cb] + _CrToG [cr])>>16)]; //green
          *(pByte++) = range_limit [y + _CbToB [cb]]; //blue
          *(pByte++) = a;
        }
      }
    }

    //AA&N DCT algorithm implemention
    void InverseDct
     (short*         coef,    //in, dct coefficients, length = 64
      unsigned char* data,    //out, 64 bytes   
      int            nBlock)  //block index: 0~3:Y; 4:A; 5:Cb; 6:Cr
    {
      #define FIX_1_082392200  ((int)277)   /* FIX(1.082392200) */
      #define FIX_1_414213562  ((int)362)   /* FIX(1.414213562) */
      #define FIX_1_847759065  ((int)473)   /* FIX(1.847759065) */
      #define FIX_2_613125930  ((int)669)   /* FIX(2.613125930) */
      #define MULTIPLY(var,cons)  (int)(((cons) * (var)) >> 8 )

      int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
      int tmp10, tmp11, tmp12, tmp13;
      int z5, z10, z11, z12, z13;
      int workspace [64];    /* buffers data between passes */

      short* inptr = coef;
      unsigned short* quantptr; 
      int* wsptr = workspace;
      unsigned char* outptr;
      unsigned char* range_limit = &(_tblRange[256+128]);
      int ctr, dcval, DCTSIZE = 8;

      if( nBlock < 5 ) quantptr = _qtblY;
      else             quantptr = _qtblCbCr;
      
      //Pass 1: process columns from input (inptr), store into work array(wsptr)
      
      for (ctr = 8; ctr > 0; ctr--)
      {
        /* Due to quantization, we will usually find that many of the input
           coefficients are zero, especially the AC terms.  We can exploit this
           by short-circuiting the IDCT calculation for any column in which all
           the AC terms are zero.  In that case each output is equal to the
           DC coefficient (with scale factor as needed).
           With typical images and quantization tables, half or more of the
           column DCT calculations can be simplified this way.
        */
        
        if ((inptr [DCTSIZE*1] | inptr [DCTSIZE*2] | inptr [DCTSIZE*3] | inptr [DCTSIZE*4] | inptr [DCTSIZE*5] | 
             inptr[DCTSIZE*6] | inptr[DCTSIZE*7]) == 0) 
        {
          /* AC terms all zero */
          dcval = (int)(inptr [DCTSIZE*0] * quantptr [DCTSIZE*0]);

          wsptr [DCTSIZE*0] = dcval;
          wsptr [DCTSIZE*1] = dcval;
          wsptr [DCTSIZE*2] = dcval;
          wsptr [DCTSIZE*3] = dcval;
          wsptr [DCTSIZE*4] = dcval;
          wsptr [DCTSIZE*5] = dcval;
          wsptr [DCTSIZE*6] = dcval;
          wsptr [DCTSIZE*7] = dcval;
          
          inptr++;      //advance pointers to next column
          quantptr++;
          wsptr++;

          continue;
        }
        
        /* Even part */
        
        tmp0 = inptr [DCTSIZE*0] * quantptr [DCTSIZE*0];
        tmp1 = inptr [DCTSIZE*2] * quantptr [DCTSIZE*2];
        tmp2 = inptr [DCTSIZE*4] * quantptr [DCTSIZE*4];
        tmp3 = inptr [DCTSIZE*6] * quantptr [DCTSIZE*6];
        
        tmp10 = tmp0 + tmp2;  /* phase 3 */
        tmp11 = tmp0 - tmp2;
        
        tmp13 = tmp1 + tmp3;  /* phases 5-3 */
        tmp12 = MULTIPLY (tmp1 - tmp3, FIX_1_414213562) - tmp13; /* 2*c4 */
        
        tmp0 = tmp10 + tmp13; /* phase 2 */
        tmp3 = tmp10 - tmp13;
        tmp1 = tmp11 + tmp12;
        tmp2 = tmp11 - tmp12;
        
        /* Odd part */
        
        tmp4 = inptr [DCTSIZE*1] * quantptr [DCTSIZE*1];
        tmp5 = inptr [DCTSIZE*3] * quantptr [DCTSIZE*3];
        tmp6 = inptr [DCTSIZE*5] * quantptr [DCTSIZE*5];
        tmp7 = inptr [DCTSIZE*7] * quantptr [DCTSIZE*7];
        
        z13 = tmp6 + tmp5; /* phase 6 */
        z10 = tmp6 - tmp5;
        z11 = tmp4 + tmp7;
        z12 = tmp4 - tmp7;
        
        tmp7  = z11 + z13; /* phase 5 */        
        tmp11 = MULTIPLY (z11 - z13, FIX_1_414213562); /* 2*c4 */
        z5    = MULTIPLY(z10 + z12, FIX_1_847759065); /* 2*c2 */
        tmp10 = MULTIPLY(z12, FIX_1_082392200) - z5; /* 2*(c2-c6) */
        tmp12 = MULTIPLY(z10, - FIX_2_613125930) + z5; /* -2*(c2+c6) */
        
        tmp6 = tmp12 - tmp7;  /* phase 2 */
        tmp5 = tmp11 - tmp6;
        tmp4 = tmp10 + tmp5;
        
        wsptr [DCTSIZE*0] = tmp0 + tmp7;
        wsptr [DCTSIZE*7] = tmp0 - tmp7;
        wsptr [DCTSIZE*1] = tmp1 + tmp6;
        wsptr [DCTSIZE*6] = tmp1 - tmp6;
        wsptr [DCTSIZE*2] = tmp2 + tmp5;
        wsptr [DCTSIZE*5] = tmp2 - tmp5;
        wsptr [DCTSIZE*4] = tmp3 + tmp4;
        wsptr [DCTSIZE*3] = tmp3 - tmp4;
        
        inptr++; //advance pointers to next column
        quantptr++;
        wsptr++;
      }

      /*
        Pass 2: process rows from work array, store into output array.      
        Note that we must descale the results by a factor of 8 == 2**3,        
        and also undo the PASS1_BITS scaling.
      */

      int RANGE_MASK = 1023; //2 bits wider than legal samples

      #define PASS1_BITS  2
      #define IDESCALE(x,n) ((int)((x)>>(n)))

      wsptr = workspace;

      for (ctr=0; ctr<DCTSIZE; ctr++)
      {
        outptr = data + ctr * 8;
      
        /* Rows of zeroes can be exploited in the same way as we did with columns.
           However, the column calculation has created many nonzero AC terms, so
           the simplification applies less often (typically 5% to 10% of the time).
           On machines with very fast multiplication, it's possible that the
           test takes more time than it's worth.  In that case this section
           may be commented out.
        */
        
        if ((wsptr [1] | wsptr [2] | wsptr [3] | wsptr [4] | wsptr [5] | wsptr [6] | wsptr [7]) == 0)
        {
          /* AC terms all zero */
          dcval      = (int)range_limit [(wsptr[0] >> 5) & RANGE_MASK];          
          outptr [0] = dcval;
          outptr [1] = dcval;
          outptr [2] = dcval;
          outptr [3] = dcval;
          outptr [4] = dcval;
          outptr [5] = dcval;
          outptr [6] = dcval;
          outptr [7] = dcval;
          
          wsptr += DCTSIZE;   /* advance pointer to next row */
          continue;
        }

        /* Even part */

        tmp10 = ((int)wsptr [0] + (int)wsptr [4]);
        tmp11 = ((int)wsptr [0] - (int)wsptr [4]);        
        tmp13 = ((int)wsptr [2] + (int)wsptr [6]);        
        tmp12 = MULTIPLY ((int)wsptr [2] - (int) wsptr [6], FIX_1_414213562) - tmp13;
        tmp0  = tmp10 + tmp13;
        tmp3  = tmp10 - tmp13;
        tmp1  = tmp11 + tmp12;
        tmp2  = tmp11 - tmp12;

        /* Odd part */

        z13   = (int)wsptr [5] + (int)wsptr [3];
        z10   = (int)wsptr [5] - (int)wsptr [3];
        z11   = (int)wsptr [1] + (int)wsptr [7];
        z12   = (int)wsptr [1] - (int)wsptr [7];        
        tmp7  = z11 + z13;   /* phase 5 */
        tmp11 = MULTIPLY (z11 - z13, FIX_1_414213562); /* 2*c4 */        
        z5    = MULTIPLY (z10 + z12, FIX_1_847759065); /* 2*c2 */
        tmp10 = MULTIPLY (z12, FIX_1_082392200) - z5; /* 2*(c2-c6) */
        tmp12 = MULTIPLY (z10, - FIX_2_613125930) + z5; /* -2*(c2+c6) */        
        tmp6  = tmp12 - tmp7;  /* phase 2 */
        tmp5  = tmp11 - tmp6;
        tmp4  = tmp10 + tmp5;

        /* Final output stage: scale down by a factor of 8 and range-limit */

        outptr [0] = range_limit [IDESCALE (tmp0 + tmp7, PASS1_BITS+3) & RANGE_MASK];
        outptr [7] = range_limit [IDESCALE (tmp0 - tmp7, PASS1_BITS+3) & RANGE_MASK];
        outptr [1] = range_limit [IDESCALE (tmp1 + tmp6, PASS1_BITS+3) & RANGE_MASK];
        outptr [6] = range_limit [IDESCALE (tmp1 - tmp6, PASS1_BITS+3) & RANGE_MASK];
        outptr [2] = range_limit [IDESCALE (tmp2 + tmp5, PASS1_BITS+3) & RANGE_MASK];
        outptr [5] = range_limit [IDESCALE (tmp2 - tmp5, PASS1_BITS+3) & RANGE_MASK];
        outptr [4] = range_limit [IDESCALE (tmp3 + tmp4, PASS1_BITS+3) & RANGE_MASK];
        outptr [3] = range_limit [IDESCALE (tmp3 - tmp4, PASS1_BITS+3) & RANGE_MASK];

        wsptr += DCTSIZE;   /* advance pointer to next row */
      }
    }

    //  Below are difficult and complex HUFFMAN decoding !!!!!
    //  HuffmanDecode( coef, i ); //source is _pData; coef is result
    void HuffmanDecode
     (short* coef,   //out, DCT coefficients
      int    iBlock) //0,1,2,3:Y; 4: A; 5:Cb; 6:Cr
    { 
      /*
        jpeg_natural_order[i] is the natural-order position of the i'th 
        element of zigzag order.
       
        When reading corrupted data, the Huffman decoders could attempt
        to reference an entry beyond the end of this array (if the decoded
        zero run length reaches past the end of the block).  To prevent
        wild stores without adding an inner-loop test, we put some extra
        "63"s after the real entries.  This will cause the extra coefficient
        to be stored in location 63 of the block, not somewhere random.
        The worst case would be a run-length of 15, which means we need 16
        fake entries.
      */  
      static const int jpeg_natural_order [64 + 16] = {
          0,  1,  8, 16,  9,  2,  3, 10,
          17, 24, 32, 25, 18, 11,  4,  5,
          12, 19, 26, 33, 40, 48, 41, 34,
          27, 20, 13,  6,  7, 14, 21, 28,
          35, 42, 49, 56, 57, 50, 43, 36,
          29, 22, 15, 23, 30, 37, 44, 51,
          58, 59, 52, 45, 38, 31, 39, 46,
          53, 60, 61, 54, 47, 55, 62, 63,
          63, 63, 63, 63, 63, 63, 63, 63, //extra entries for safety
          63, 63, 63, 63, 63, 63, 63, 63
      };

      int* pLastDC;
      int s, k, r;

      HuffTable *dctbl, *actbl;

      if (iBlock < 5)
      {
        dctbl = &_htblYDC;
        actbl = &_htblYAC;
        
        if (iBlock < 4) pLastDC = &_dcY;
        else            pLastDC = &_dcA;
      }
      else
      {
        dctbl = &_htblCbCrDC;
        actbl = &_htblCbCrAC;

        if (iBlock == 5) pLastDC = &_dcCb;
        else             pLastDC = &_dcCr;
      }

      memset (coef, 0, sizeof (short) * 64);

      /* Section F.2.2.1: decode the DC coefficient difference */

      s = GetCategory (dctbl); //get dc category number, s

      if (s)
      {
        r = GetBits(s);         //get offset in this dc category
        s = ValueFromCategory(s, r);  //get dc difference value
      }
      
      /* Convert DC difference to actual value, update last_dc_val */

      s        += *pLastDC;
      *pLastDC  = s;

      /* Output the DC coefficient (assumes jpeg_natural_order[0] = 0) */

      coef [0] = (short)s;  

      /* Section F.2.2.2: decode the AC coefficients */
      /* Since zeroes are skipped, output area must be cleared beforehand */

      for (k = 1; k<64; k++) 
      {
        s = GetCategory (actbl); //s: (run, category)
        r = s >> 4;              //r: run length for ac zero, 0 <= r < 16
        s &= 15;                 //s: category for this non-zero ac

        if (s) 
        {
          k += r;                        //  k: position for next non-zero ac
          r  = GetBits (s);              //  r: offset in this ac category
          s  = ValueFromCategory (s, r); //  s: ac value

          coef [jpeg_natural_order [k]] = (short)s;
        } 
        else // s = 0, means ac value is 0 ? Only if r = 15.  
        {
          if (r != 15)  //means all the left ac are zero
            break;

          k += 15;
        }
      }   
    }

    /*
      get category number for dc, or (0 run length, ac category) for ac    
      The max length for Huffman codes is 15 bits; so we use 32 bits buffer   
      _nGetBuff, with the validated length is _nGetBits.                    
      Usually, more than 95% of the Huffman codes will be 8 or fewer bits long
      To speed up, we should pay more attention on the codes whose length <= 8      
    */
    inline int GetCategory (HuffTable* htbl)
    {
      //If left bits < 8, we should get more data
      if (_nGetBits < 8)
        FillBitBuffer ();

      //Call special process if data finished; min bits is 1
      if (_nGetBits < 8)
        return SpecialDecode (htbl, 1);

      //Peek the first valid byte 
      int look = (_nGetBuff >> (_nGetBits - 8)) & 0xFF, nb = htbl->look_nbits [look];

      if (nb) 
      { 
        _nGetBits -= nb;

        return htbl->look_sym [look]; 
      } 
      else return SpecialDecode (htbl, 9); //Decode long codes with length >= 9
    }

    void FillBitBuffer ()
    {
      unsigned char uc;

      while (_nGetBits < 25) //#define MIN_GET_BITS  (32-7)
      {
        if (_nDataBytes > 0) //Are there some data?
        {
          uc         = *_pData++;
          _nGetBuff = (_nGetBuff << 8) | ((int) uc);
          _nGetBits += 8;

          _nDataBytes --;
        }
        else break;
      }
    }

    inline int GetBits (int nbits) 
    {
      if (_nGetBits < nbits)//we should read nbits bits to get next data
        FillBitBuffer ();
        
      _nGetBits -= nbits;
      
      return (int)(_nGetBuff >> _nGetBits) & ((1 << nbits)-1);
    }

    /*
        Special Huffman decode:    
          (1) For codes with length > 8        
          (2) For codes with length < 8 while data is finished          
    */
    int SpecialDecode (HuffTable* htbl, int nMinBits)
    {      
      int l = nMinBits;
      int code;
      
      /* HUFF_DECODE has determined that the code is at least min_bits */
      /* bits long, so fetch that many bits in one swoop. */

      code = GetBits (l);
      
      /* Collect the rest of the Huffman code one bit at a time. */
      /* This is per Figure F.16 in the JPEG spec. */
      while (code > htbl->maxcode [l])
      {
        code <<= 1;
        code  |= GetBits(1);
        l++;
      }
      
      /* With garbage input we may reach the sentinel value l = 17. */

      if (l > 16)
        return 0;     /* fake a zero as the safest result */
      
      return htbl->huffval[ htbl->valptr[l] + (int)(code - htbl->mincode[l]) ];
    }

    //To find dc or ac value according to category and category offset
    inline int ValueFromCategory(int nCate, int nOffset)
    {
      //If (nOffset < half[nCate]), then value is below zero
      //Otherwise, value is above zero, and just the nOffset
      static const int half [16] =   /* entry n is 2**(n-1) */
      {0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
       0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000};
      
      //start[i] is the starting value in this category; surely it is below zero
      static const int start[16] =  /* entry n is (-1 << n) + 1 */
      { 0, ((-1)<<1) + 1, ((-1)<<2) + 1, ((-1)<<3) + 1, ((-1)<<4) + 1,
        ((-1)<<5) + 1, ((-1)<<6) + 1, ((-1)<<7) + 1, ((-1)<<8) + 1,
        ((-1)<<9) + 1, ((-1)<<10) + 1, ((-1)<<11) + 1, ((-1)<<12) + 1,
        ((-1)<<13) + 1, ((-1)<<14) + 1, ((-1)<<15) + 1 }; 

      return (nOffset < half [nCate] ? nOffset + start [nCate] : nOffset); 
    }

  private:
    // Derived data constructed for each Huffman table 
    struct HuffTable
    {
      int           mincode [17];     // smallest code of length k 
      int           maxcode [18];     // largest code of length k (-1 if none) 
      int           valptr [17];      // huffval[] index of 1st symbol of length k
      unsigned char bits [17];        // bits[k] = # of symbols with codes of 
      unsigned char huffval [256];    // The symbols, in order of incr code length 
      int           look_nbits [256]; // # bits, or 0 if too long
      unsigned char look_sym [256];   // symbol, or unused
    };
    
  private:    
    unsigned short _nQuality, _nScale;                                      //Following are initialized when create a new decoder
    unsigned char  _tblRange [5*256+128];
    int            _CrToR [256], _CrToG [256], _CbToB [256], _CbToG [256];  //To speed up, we save YCbCr=>RGB color map tables    
    unsigned short _qtblY [64], _qtblCbCr [64];                             //To speed up, we precompute two DCT quant tables    
    HuffTable      _htblYDC, _htblYAC, _htblCbCrDC, _htblCbCrAC;
    unsigned short _nWidth, _nHeight;                                       //Following data are initialized for decoding every image
    int            _dcY, _dcA, _dcCb, _dcCr;
    int            _nGetBits, _nGetBuff, _nDataBytes;
    unsigned char* _pData;
};

/*
    Реализации декодера видео
*/

class GVDDecoder: public IVideoDecoder
{
  public:
    GVDDecoder (const char* file_name)
    {
      try
      {
        common::InputFile file (file_name);        
        
        _data.resize (file.Size ());
        
        if (file.Read (_data.data (), _data.size ()) != _data.size ())
          throw xtl::format_operation_exception ("", "Data read error");
          
        _video_file_name = file_name;          

        size_t framesCount = 0;

        memcpy (&framesCount, _data.data (), 4);

        int int0 = 0, int1 = 0;

        memcpy (&int0, _data.data () + 8, 4);
        memcpy (&int1, _data.data () + 12, 4);

        int x, y, orgW, orgH;

        x    = int0 >> 16;
        y    = int0 & 0xffff;
        orgW = int1 >> 16;
        orgH = int1 & 0xffff;

        _width           = (size_t)orgW;
        _height          = (size_t)orgH;
        _frames_offset_x = (size_t)x;
        _frames_offset_y = (size_t)y;
        _frames_offset   = (size_t)(y * orgW + x) * sizeof (Pixel);
        _pitch           = (size_t)orgW * sizeof (Pixel);

        size_t offset = 16;
        unsigned char* framesHeaderData = _data.data () + offset;
        size_t         frameStructSize  = sizeof (VideoFrameHeader);

        _frames.clear ();
        _frames.reserve (framesCount);

        for (size_t i=0; i!= framesCount; ++i)
        {
          VideoFrameHeader header;
          
          unsigned char* frameHeaderPos = framesHeaderData + i * frameStructSize;

          memcpy (&header, frameHeaderPos, frameStructSize);

          _frames.push_back (header);
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::GVDDecoder::GVDDecoder");
        throw;
      }
    }

    float GetFramesPerSecond ()
    {
      return 1.0f;
    }

    size_t GetWidth ()
    {
      return _width;
    }
    
    size_t GetHeight ()
    {
      return _height;
    }
    
    size_t GetFramesCount ()
    {
      return _frames.size ();
    }
    
    float GetPixelAspectRatio ()
    {
      return 1.0f;
    }
    
    void Decode (size_t frame, Pixel* data_buffer)
    {
      static const char* METHOD_NAME = "media::GVDDecoder::Decode";
      
      if (frame >= _frames.size ())
        throw xtl::make_range_exception (METHOD_NAME, "frame", frame, _frames.size ());
        
      if (!data_buffer)
        throw xtl::make_null_argument_exception (METHOD_NAME, "data_buffer");
        
      const VideoFrameHeader& frame_desc = _frames [frame];
      
      int head_size = 0, frame_width = 0, frame_height = 0;
      
      if (!_decoder.GetImageInfo (_data.data () + frame_desc.Offset, frame_desc.Size, frame_width, frame_height, head_size))
        throw xtl::format_operation_exception (METHOD_NAME, "Error at get image info for frame %u (video='%s')", 
        frame, _video_file_name.c_str ());
        
      if (_frames_offset_x + frame_width > _width)   frame_width  = _width - _frames_offset_x;
      if (_frames_offset_y + frame_height > _height) frame_height = _height - _frames_offset_y;
      if ((size_t)frame_width > _width)              frame_width  = 0;
      if ((size_t)frame_height > _height)            frame_height = 0;            

      if (_width != (size_t)frame_width || _height != (size_t)frame_height)
      {
        memset (data_buffer, 0, _width * _height * sizeof (Pixel));
      }
      
      if (frame_width && frame_height)
      {        
        unsigned char* frame_buffer_pos = (unsigned char*)data_buffer + _frames_offset;
        
        if (!_decoder.DecompressImage (_data.data () + frame_desc.Offset + head_size, _pitch, frame_buffer_pos))
          throw xtl::format_operation_exception (METHOD_NAME, "Error at decompress image for frame %u (video='%s')",
          frame, _video_file_name.c_str ());
      }
    }

  private:
    struct VideoFrameHeader
    {
      size_t Offset, Size;
    };
    
    typedef xtl::uninitialized_storage<unsigned char> Buffer;
    typedef stl::vector<VideoFrameHeader>             FrameList;

  private:
    stl::string    _video_file_name;
    GGFrameDecoder _decoder;
    FrameList      _frames;
    Buffer         _data;
    size_t         _frames_offset, _pitch, _frames_offset_x, _frames_offset_y, _width, _height;
};

/*
   Компонент декодирования видео
*/

class Component
{
  public:
    Component ()
    {
      VideoStreamManager::RegisterLoader ("gvd", &Component::CreateDecoder);
    }
    
  private:
    static IVideoDecoder* CreateDecoder (const char* file_name, VideoQuality)
    {
      if (!file_name)
        throw xtl::make_null_argument_exception ("media::theora_loader", "file_name");

      return new GVDDecoder (file_name);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> GvdDecoder ("media.video.decoders.gvd");

}

}
