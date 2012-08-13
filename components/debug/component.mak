TARGETS += DEBUG.SOURCES
TARGETS += DEBUG.TESTS
TARGETS += DEBUG.UTILS.ANDROID_CRASH_RESOLVER
TARGETS += DEBUG.TESTS.ANDROID_CRASH_RESOLVER

DEBUG.SOURCES.TYPE        := static-lib
DEBUG.SOURCES.NAME        := funner.debug
DEBUG.SOURCES.SOURCE_DIRS := sources/map_file/core sources/map_file/parsers/gcc
DEBUG.SOURCES.IMPORTS     := compile.debug

DEBUG.TESTS.TYPE        := test-suite
DEBUG.TESTS.SOURCE_DIRS := tests/map_file
DEBUG.TESTS.IMPORTS     := compile.debug link.debug

DEBUG.UTILS.ANDROID_CRASH_RESOLVER.TYPE          := application
DEBUG.UTILS.ANDROID_CRASH_RESOLVER.NAME          := android-crash-resolver
DEBUG.UTILS.ANDROID_CRASH_RESOLVER.SOURCE_DIRS   := utils/android_crash_resolver
DEBUG.UTILS.ANDROID_CRASH_RESOLVER.IMPORTS       := compile.debug link.debug

DEBUG.TESTS.ANDROID_CRASH_RESOLVER.TYPE              := test-suite
DEBUG.TESTS.ANDROID_CRASH_RESOLVER.SOURCE_DIRS       := tests/android_crash_resolver
DEBUG.TESTS.ANDROID_CRASH_RESOLVER.USED_APPLICATIONS := android-crash-resolver
