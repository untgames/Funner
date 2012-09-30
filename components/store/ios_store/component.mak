###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STORE.IOS_STORE.SOURCES

#Цель - sources
STORE.IOS_STORE.SOURCES.TYPE        := static-lib
STORE.IOS_STORE.SOURCES.NAME        := funner.store.ios_store
STORE.IOS_STORE.SOURCES.SOURCE_DIRS := sources
STORE.IOS_STORE.SOURCES.IMPORTS			:= compile.store.core compile.common
