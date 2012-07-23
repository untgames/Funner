###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.IMAGE.SOURCES MEDIA.XATLAS.SOURCES MEDIA.LEFT_BOTTOM_PACKER.SOURCES MEDIA.PVR_LOADER.SOURCES  \
           MEDIA.DDS_LOADER.SOURCES MEDIA.IMAGE.ANI_CURSORS MEDIA.IMAGE.TESTS \
           MEDIA.IMAGE.PSD_EXPORTER.SOURCES MEDIA.IMAGE.PSD_EXPORTER.TESTS \
           MEDIA.XATLAS_BUILDER.SOURCES MEDIA.XATLAS_BUILDER.TESTS \
           MEDIA.INTERLACER.SOURCES MEDIA.INTERLACER.TESTS \
           MEDIA.TEXTURE_CONVERTER.SOURCES

ifneq (,$(filter win32,$(PROFILES))$(filter macosx,$(PROFILES)))
  TARGETS += MEDIA.QUALCOMM_TEXTURE_CONVERTER.TESTS
endif

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
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.INCLUDE_DIRS := ../../math/bound_volumes/include
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.SOURCE_DIRS  := sources/packers/left_bottom
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.IMPORTS      := compile.media.image

#Цель - PVR loader
MEDIA.PVR_LOADER.SOURCES.TYPE         := static-lib
MEDIA.PVR_LOADER.SOURCES.NAME         := funner.media.image.pvr_loader
MEDIA.PVR_LOADER.SOURCES.SOURCE_DIRS  := sources/compressed_image_loaders/pvr
MEDIA.PVR_LOADER.SOURCES.IMPORTS      := compile.media.image

#Цель - DDS loader
MEDIA.DDS_LOADER.SOURCES.TYPE         := static-lib
MEDIA.DDS_LOADER.SOURCES.NAME         := funner.media.image.dds_loader
MEDIA.DDS_LOADER.SOURCES.SOURCE_DIRS  := sources/compressed_image_loaders/dds
MEDIA.DDS_LOADER.SOURCES.IMPORTS      := compile.media.image

#Цель - ANI cursors
MEDIA.IMAGE.ANI_CURSORS.TYPE        := static-lib
MEDIA.IMAGE.ANI_CURSORS.NAME        := funner.media.image.ani_cursors
MEDIA.IMAGE.ANI_CURSORS.SOURCE_DIRS := sources/ani_cursors
MEDIA.IMAGE.ANI_CURSORS.IMPORTS     := compile.media.image

#Цель - Image tests
MEDIA.IMAGE.TESTS.TYPE         := test-suite
MEDIA.IMAGE.TESTS.INCLUDE_DIRS := ../../math/bound_volumes/include
MEDIA.IMAGE.TESTS.SOURCE_DIRS  := tests/image tests/atlas
MEDIA.IMAGE.TESTS.IMPORTS      := compile.media.image link.media.image link.media.image.xatlas \
                                  link.media.image.left_bottom_packer run.extern.devil link.media.image.pvr link.media.image.dds \
                                  link.media.image.ani_cursor_loader

#Цель - PSD exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.TYPE        := application
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.NAME        := psd-exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.SOURCE_DIRS := utils/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.SOURCES.IMPORTS     := compile.media.image link.media.image compile.extern.libpsd link.extern.libpsd

#Цель - PSD exporter tests
MEDIA.IMAGE.PSD_EXPORTER.TESTS.TYPE              := test-suite
MEDIA.IMAGE.PSD_EXPORTER.TESTS.SOURCE_DIRS       := tests/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.TESTS.EXECUTION_DIR     := tests/psd_exporter
MEDIA.IMAGE.PSD_EXPORTER.TESTS.USED_APPLICATIONS := psd-exporter

#Цель - XAtlas builder
MEDIA.XATLAS_BUILDER.SOURCES.TYPE          := application
MEDIA.XATLAS_BUILDER.SOURCES.NAME          := atlas-builder
MEDIA.XATLAS_BUILDER.SOURCES.SOURCE_DIRS   := utils/atlas_builder
MEDIA.XATLAS_BUILDER.SOURCES.IMPORTS       := compile.common compile.media.image link.media.image link.media.image.xatlas link.media.image.left_bottom_packer

#Цель - XAtlas builder tests
MEDIA.XATLAS_BUILDER.TESTS.TYPE              := test-suite
MEDIA.XATLAS_BUILDER.TESTS.SOURCE_DIRS       := tests/atlas_builder
MEDIA.XATLAS_BUILDER.TESTS.USED_APPLICATIONS := atlas-builder

#Цель - Interlacer
MEDIA.INTERLACER.SOURCES.TYPE          := application
MEDIA.INTERLACER.SOURCES.NAME          := interlacer
MEDIA.INTERLACER.SOURCES.SOURCE_DIRS   := utils/interlacer
MEDIA.INTERLACER.SOURCES.IMPORTS       := compile.common compile.media.image link.media.image

#Цель - Interlacer
MEDIA.INTERLACER.TESTS.TYPE              := test-suite
MEDIA.INTERLACER.TESTS.SOURCE_DIRS       := tests/interlacer
MEDIA.INTERLACER.TESTS.USED_APPLICATIONS := interlacer

#Цель
MEDIA.TEXTURE_CONVERTER.SOURCES.TYPE           := application
MEDIA.TEXTURE_CONVERTER.SOURCES.NAME           := texture-converter
MEDIA.TEXTURE_CONVERTER.SOURCES.SOURCE_DIRS    := utils/texture_converter
MEDIA.TEXTURE_CONVERTER.SOURCES.IMPORTS        := compile.common compile.media.image link.media.image link.media.image.dds
MEDIA.TEXTURE_CONVERTER.SOURCES.win32.IMPORTS  := compile.extern.qualcomm_texture_converter link.extern.qualcomm_texture_converter
MEDIA.TEXTURE_CONVERTER.SOURCES.macosx.IMPORTS := compile.extern.qualcomm_texture_converter link.extern.qualcomm_texture_converter

#Цель
MEDIA.QUALCOMM_TEXTURE_CONVERTER.TESTS.TYPE              := test-suite
MEDIA.QUALCOMM_TEXTURE_CONVERTER.TESTS.SOURCE_DIRS       := tests/qualcomm_texture_converter
MEDIA.QUALCOMM_TEXTURE_CONVERTER.TESTS.USED_APPLICATIONS := texture_converter

