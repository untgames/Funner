###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.IMAGE.SOURCES MEDIA.XATLAS.SOURCES MEDIA.LEFT_BOTTOM_PACKER.SOURCES MEDIA.IMAGE.TESTS \
           MEDIA.IMAGE.ANIMATION_ENHANCER MEDIA.IMAGE.SPRITE_COMPOSER MEDIA.IMAGE.PSD_EXPORTER.SOURCES \
           MEDIA.IMAGE.PSD_EXPORTER.TESTS

#Цель - Image sources
MEDIA.IMAGE.SOURCES.TYPE        := static-lib
MEDIA.IMAGE.SOURCES.NAME        := funner.media.image
MEDIA.IMAGE.SOURCES.SOURCE_DIRS := sources/image sources/atlas
MEDIA.IMAGE.SOURCES.IMPORTS     := compile.media.image compile.extern.devil

#Цель - XAtlas sources
MEDIA.XATLAS.SOURCES.TYPE        := static-lib
MEDIA.XATLAS.SOURCES.NAME        := funner.media.image.xatlas
MEDIA.XATLAS.SOURCES.SOURCE_DIRS := sources/xatlas
MEDIA.XATLAS.SOURCES.IMPORTS     := compile.media.image

#Цель - Left-bottom packer sources
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.TYPE         := static-lib
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.NAME         := funner.media.image.left_bottom_packer
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.INCLUDE_DIRS := ../../bound_volumes/include
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.SOURCE_DIRS  := sources/packers/left_bottom
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.IMPORTS      := compile.media.image

#Цель - Image tests
MEDIA.IMAGE.TESTS.TYPE         := test-suite
MEDIA.IMAGE.TESTS.INCLUDE_DIRS := ../../bound_volumes/include
MEDIA.IMAGE.TESTS.SOURCE_DIRS  := tests/image tests/atlas
MEDIA.IMAGE.TESTS.IMPORTS      := compile.media.image link.media.image link.media.image.xatlas link.media.image.left_bottom_packer run.extern.devil

#Цель - Animation enhancer sources
MEDIA.IMAGE.ANIMATION_ENHANCER.TYPE        := application
MEDIA.IMAGE.ANIMATION_ENHANCER.NAME        := animation_enhancer
MEDIA.IMAGE.ANIMATION_ENHANCER.SOURCE_DIRS := utils/animation_enhancer
MEDIA.IMAGE.ANIMATION_ENHANCER.IMPORTS     := compile.media.image link.media.image

#Цель - Sprite composer sources
MEDIA.IMAGE.SPRITE_COMPOSER.TYPE        := application
MEDIA.IMAGE.SPRITE_COMPOSER.NAME        := sprite_composer
MEDIA.IMAGE.SPRITE_COMPOSER.SOURCE_DIRS := utils/sprite_composer
MEDIA.IMAGE.SPRITE_COMPOSER.IMPORTS     := compile.media.image link.media.image

#Цель - PSD exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.TYPE        := application
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.NAME        := psd-exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.SOURCE_DIRS := utils/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.IMPORTS     := compile.media.image link.media.image compile.extern.libpsd link.extern.libpsd

#Цель - PSD exporter tests
MEDIA.IMAGE.PSD_EXPORTER.TESTS.TYPE          := test-suite
MEDIA.IMAGE.PSD_EXPORTER.TESTS.SOURCE_DIRS   := tests/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.TESTS.EXECUTION_DIR := tests/psd_exporter
