/*
	platform.h

  Copyright (c) 2006-2009 Primate Labs

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef INC_PLATFORM_H
#define INC_PLATFORM_H

#if defined(__APPLE__)
    #define PLATFORM_MACOSX
    #define PLATFORM_UNIX
    #if defined(__POWERPC__)
        #define ARCH_PPC
        #if defined(__ppc64__) 
			#define ARCH_64BIT
        #else
			#define ARCH_32BIT
		#endif
    #elif defined(__i386__)
        #define ARCH_X86
		#define ARCH_32BIT
	#elif defined(__x86_64)
        #define ARCH_X86
		#define ARCH_64BIT
	#else
        #error Unsupported architecture
    #endif
#elif defined(WIN32)
    #define PLATFORM_WINDOWS
	#define ARCH_X86
    #if defined(_WIN64)
       	#define ARCH_64BIT
    #else
		#define ARCH_32BIT
	#endif
#elif defined(__linux__)
    #define PLATFORM_LINUX
    #define PLATFORM_UNIX
	#if defined(__x86_64)
        #define ARCH_X86
		#define ARCH_64BIT
    #elif defined(__i386__)
        #define ARCH_X86
		#define ARCH_32BIT
	#elif defined(__PPC64__)
		#define ARCH_PPC
		#define ARCH_64BIT
	#elif defined(__PPC__)
		#define ARCH_PPC
		#define ARCH_PPC32
		#define ARCH_32BIT
    #else
        #error Unsupported architecture
    #endif
#elif defined(__sun)
  #define PLATFORM_SOLARIS
  #define PLATFORM_UNIX
  #if defined(__i386)
    #define ARCH_X86
    #define ARCH_32BIT
  #elif defined(__amd64)
    #define ARCH_X86
    #define ARCH_64BIT
  #elif defined(__sparc)
    #define ARCH_SPARC
    #define ARCH_32BIT
  #else
    #error Unsupported architecture
  #endif
#else
  #error Unsupported platform
#endif

#if defined(ARCH_X86) 
  #define SIMD_MMX
  #define SIMD_SSE
#elif defined(ARCH_PPC) 
  #define SIMD_ALTIVEC
#elif defined(ARCH_SPARC)
  #define SIMD_NONE
#else
  #error Unsupported architecture
#endif

/*
	Sanity checks
*/

#if defined(ARCH_32BIT) && defined(ARCH_64BIT)
	#error Conflicting architectures
#endif

#if !defined(ARCH_32BIT) && !defined(ARCH_64BIT)
	#error No architecture.
#endif

#if defined(ARCH_PPC) && defined(ARCH_X86)
	#error Conflicting architectures
#endif

#if defined(PLATFORM_MACOSX) && (defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS))
	#error Conflicting platforms 
#endif

#if defined(PLATFORM_LINUX) && (defined(PLATFORM_MACOSX) || defined(PLATFORM_WINDOWS))
	#error Conflicting platforms 
#endif

#endif
