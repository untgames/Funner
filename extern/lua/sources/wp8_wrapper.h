#ifndef LUA_WP8_WRAPPER
#define LUA_WP8_WRAPPER

#ifdef WP8

#define system(X) -1
#define getenv(X) ""

#define _popen(L,c,m)  ((void)((void)c, m),  \
    luaL_error(L, LUA_QL("popen") " not supported"), (FILE*)0)
#define _pclose(L,file)    ((void)((void)L, file), 0)

#endif

#endif
