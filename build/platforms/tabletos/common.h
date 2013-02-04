#ifndef TABLETOS_FUNNER_COMMON_PLATFORM_PART_HEADER
#define TABLETOS_FUNNER_COMMON_PLATFORM_PART_HEADER

#ifdef __cplusplus

extern "C" __attribute__ ((visibility("default"))) int main ();

#else

__attribute__ ((visibility("default"))) extern int main ();

#endif

#endif
