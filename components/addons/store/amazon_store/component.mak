###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STORE.AMAZON_STORE.SOURCES STORE.AMAZON_STORE.JAVA

#Цель - sources
STORE.AMAZON_STORE.SOURCES.TYPE        := static-lib
STORE.AMAZON_STORE.SOURCES.NAME        := funner.store.amazon_store
STORE.AMAZON_STORE.SOURCES.SOURCE_DIRS := sources
STORE.AMAZON_STORE.SOURCES.IMPORTS     := compile.store.core compile.system

#Цель - java
STORE.AMAZON_STORE.JAVA.TYPE        := android-jar
STORE.AMAZON_STORE.JAVA.NAME        := funner.amazon_store
STORE.AMAZON_STORE.JAVA.SOURCE_DIRS := java_sources
STORE.AMAZON_STORE.JAVA.JARS        := funner.application
STORE.AMAZON_STORE.JAVA.IMPORTS     := compile.extern.amazon_in_app_purchasing
