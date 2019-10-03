In order to use Qualcomm texture conversion routines, you should:
1) Download Adreno SDK from https://developer.qualcomm.com/software/adreno-gpu-sdk/tools
2) Put files QCompressLib.h and TextureConverter.h into include folder
3) For win32 builds - put files TextureConverter.dll and TextureConverter.lib into lib/win32 folder
4) For win64 builds - put files TextureConverter.dll and TextureConverter.lib into lib/win64 folder
5) For macosx builds - put file libTextureConverter.dylib into lib/osx folder
6) For linux x86 builds - put file libQCompressLib.a into lib/linux folder and files libASTCLibrary.a and libTextureConverter.so into lib/linux/i386 folder
7) For linux x64 builds - put file libQCompressLib.a into lib/linux folder and files libASTCLibrary.a and libTextureConverter.so into lib/linux/x64 folder
8) export environment variable QUALCOMM_TEXTURE_CONVERTER_ENABLED