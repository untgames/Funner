#include <stdio.h>
#include <exception>
#include <common/strlib.h>

using namespace common;

int main ()
{
  printf ("Results of base64_test:\n");
  
  try
  {
    static const char* for_encoding [] = {
      "Hello world!",
      "",
      "So?<p>This 4, 5, 6, 7, 8, 9, z, {, |, } tests Base64 encoder. Show me: @, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, [, \\, ], ^, _, `, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s.",
    };
    
    for (size_t i=0; i<sizeof (for_encoding)/sizeof (*for_encoding); i++)
    {
      stl::string result;
      
      encode_base64 (strlen (for_encoding [i]), for_encoding [i], result);
      
      printf ("encoding (%s)='%s'\n", for_encoding [i], result.c_str ());
    }    
    
    static const char* for_decoding [] = {
      "SGVsbG8gd29ybGQh",
      "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0\nNTY3ODksLg=="
    };
    
    for (size_t i=0; i<sizeof (for_decoding)/sizeof (*for_decoding); i++)
    {
      stl::string result;
      
      decode_base64 (strlen (for_decoding [i]), for_decoding [i], result);
      
      printf ("decoding (%s)='%s'\n", for_decoding [i], result.c_str ());
    }        
  }    
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
