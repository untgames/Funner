###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += MEDIA.RFX.SOURCES
TARGETS += MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES
TARGETS += MEDIA.RFX.TESTS
TARGETS += MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS
TARGETS += MEDIA.RFX.MATERIALS_PROCESSOR.SOURCES
TARGETS += MEDIA.RFX.MATERIALS_PROCESSOR.TESTS
TARGETS += MEDIA.RFX.INFO

#Цель №1 - sources
MEDIA.RFX.SOURCES.TYPE             := static-lib
MEDIA.RFX.SOURCES.NAME             := funner.media.rfx
MEDIA.RFX.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.RFX.SOURCES.IMPORTS          := compile.media.rfx compile.media.shared

#Цель №2 - Default serializers sources
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.TYPE             := static-lib
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.NAME             := funner.media.rfx.default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.SOURCE_DIRS      := sources/default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.IMPORTS          := compile.media.rfx

#Цель - tests
MEDIA.RFX.TESTS.TYPE             := test-suite
MEDIA.RFX.TESTS.SOURCE_DIRS      := tests/core
MEDIA.RFX.TESTS.IMPORTS          := link.media.rfx compile.media.rfx

#Цель - XMTL tests
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.TYPE             := test-suite
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.SOURCE_DIRS      := tests/default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.IMPORTS          := link.media.rfx.default_serializers compile.media.rfx

#Цель - materials processor
MEDIA.RFX.MATERIALS_PROCESSOR.SOURCES.TYPE        := application
MEDIA.RFX.MATERIALS_PROCESSOR.SOURCES.NAME        := materials-processor
MEDIA.RFX.MATERIALS_PROCESSOR.SOURCES.SOURCE_DIRS := utils/materials_processor
MEDIA.RFX.MATERIALS_PROCESSOR.SOURCES.IMPORTS     := compile.media.rfx link.media.rfx.default_serializers

#Цель - materials processor tests
MEDIA.RFX.MATERIALS_PROCESSOR.TESTS.TYPE              := test-suite
MEDIA.RFX.MATERIALS_PROCESSOR.TESTS.SOURCE_DIRS       := tests/materials_processor
MEDIA.RFX.MATERIALS_PROCESSOR.TESTS.EXECUTION_DIR     := tests/materials_processor
MEDIA.RFX.MATERIALS_PROCESSOR.TESTS.USED_APPLICATIONS := materials-processor

#Цель - сборка документации
MEDIA.RFX.INFO.TYPE        := doxygen-info
MEDIA.RFX.INFO.CHM_NAME    := funner.media.rfx
MEDIA.RFX.INFO.SOURCE_DIRS := include
MEDIA.RFX.INFO.IMPORTS     := compile.media.rfx
