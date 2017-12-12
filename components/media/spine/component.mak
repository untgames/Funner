TARGETS := MEDIA.SPINE.CORE MEDIA.SPINE.LOADER_3_6 MEDIA.SPINE.TESTS

#Core sources
MEDIA.SPINE.CORE.TYPE         := static-lib
MEDIA.SPINE.CORE.NAME         := funner.media.spine.core
MEDIA.SPINE.CORE.INCLUDE_DIRS := sources/shared
MEDIA.SPINE.CORE.SOURCE_DIRS  := sources/core
MEDIA.SPINE.CORE.IMPORTS      := compile.media.spine

#Impl for spine 3.6 sources
MEDIA.SPINE.LOADER_3_6.TYPE         := static-lib
MEDIA.SPINE.LOADER_3_6.NAME         := funner.media.spine.loader_3.6
MEDIA.SPINE.LOADER_3_6.INCLUDE_DIRS := sources/shared
MEDIA.SPINE.LOADER_3_6.SOURCE_DIRS  := sources/loader
MEDIA.SPINE.LOADER_3_6.IMPORTS      := compile.media.spine compile.extern.spine_3.6

#Tests
MEDIA.SPINE.TESTS.TYPE        := test-suite
MEDIA.SPINE.TESTS.SOURCE_DIRS := tests
MEDIA.SPINE.TESTS.IMPORTS     := compile.media.spine compile.extern.spine_3.6 link.extern.spine_3.6
