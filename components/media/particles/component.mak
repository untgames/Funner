TARGETS := MEDIA.PARTICLES.SOURCES MEDIA.PARTICLES.TESTS

MEDIA.PARTICLES.SOURCES.TYPE        := static-lib
MEDIA.PARTICLES.SOURCES.NAME        := funner.media.particles
MEDIA.PARTICLES.SOURCES.SOURCE_DIRS := sources/core
MEDIA.PARTICLES.SOURCES.IMPORTS     := compile.media.particles compile.common

MEDIA.PARTICLES.TESTS.TYPE        := test-suite
MEDIA.PARTICLES.TESTS.SOURCE_DIRS := tests/particles
MEDIA.PARTICLES.TESTS.IMPORTS     := compile.media.particles link.media.particles
