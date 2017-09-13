TARGETS := MEDIA.PARTICLES.SOURCES MEDIA.PARTICLES.PARTICLE_DESIGNER_SERIALIZER.SOURCES MEDIA.PARTICLES.TESTS MEDIA.PARTICLES.SERIALIZERS.TESTS

MEDIA.PARTICLES.SOURCES.TYPE        := static-lib
MEDIA.PARTICLES.SOURCES.NAME        := funner.media.particles
MEDIA.PARTICLES.SOURCES.SOURCE_DIRS := sources/core
MEDIA.PARTICLES.SOURCES.IMPORTS     := compile.media.particles compile.common

MEDIA.PARTICLES.PARTICLE_DESIGNER_SERIALIZER.SOURCES.TYPE        := static-lib
MEDIA.PARTICLES.PARTICLE_DESIGNER_SERIALIZER.SOURCES.NAME        := funner.media.particles.particle_designer_serializer
MEDIA.PARTICLES.PARTICLE_DESIGNER_SERIALIZER.SOURCES.SOURCE_DIRS := sources/particle_designer_serializer
MEDIA.PARTICLES.PARTICLE_DESIGNER_SERIALIZER.SOURCES.IMPORTS     := compile.media.particles compile.common

MEDIA.PARTICLES.TESTS.TYPE        := test-suite
MEDIA.PARTICLES.TESTS.SOURCE_DIRS := tests/core
MEDIA.PARTICLES.TESTS.IMPORTS     := compile.media.particles link.media.particles compile.xtl

MEDIA.PARTICLES.SERIALIZERS.TESTS.TYPE        := test-suite
MEDIA.PARTICLES.SERIALIZERS.TESTS.SOURCE_DIRS := tests/serializers
MEDIA.PARTICLES.SERIALIZERS.TESTS.IMPORTS     := compile.media.particles link.media.particles.particle_designer_serializer compile.common
