###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STORE.ANDROID_STORE.SOURCES

#Цель - sources
STORE.ANDROID_STORE.SOURCES.TYPE        := static-lib
STORE.ANDROID_STORE.SOURCES.NAME        := funner.store.android_store
STORE.ANDROID_STORE.SOURCES.SOURCE_DIRS := sources
STORE.ANDROID_STORE.SOURCES.IMPORTS			:= compile.store.core compile.system
