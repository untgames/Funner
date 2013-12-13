###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOCIAL.FACEBOOK.SOURCES SOCIAL.FACEBOOK.TESTS
TARGETS.android += SOCIAL.ANDROID_SESSION.JAVA

#Цель - sources
SOCIAL.FACEBOOK.SOURCES.TYPE                := static-lib
SOCIAL.FACEBOOK.SOURCES.NAME                := funner.social.facebook
SOCIAL.FACEBOOK.SOURCES.INCLUDE_DIRS        += sources/platform
SOCIAL.FACEBOOK.SOURCES.SOURCE_DIRS         := sources/shared sources/platform/default
SOCIAL.FACEBOOK.SOURCES.android.SOURCE_DIRS += sources/platform/android
SOCIAL.FACEBOOK.SOURCES.iphone.SOURCE_DIRS  += sources/platform/ios
SOCIAL.FACEBOOK.SOURCES.IMPORTS             := compile.social.core compile.system

#Цель - java
SOCIAL.ANDROID_SESSION.JAVA.TYPE        := android-jar
SOCIAL.ANDROID_SESSION.JAVA.NAME        := funner.facebook_session
SOCIAL.ANDROID_SESSION.JAVA.SOURCE_DIRS := java_sources
SOCIAL.ANDROID_SESSION.JAVA.JARS        := funner.application
SOCIAL.ANDROID_SESSION.JAVA.IMPORTS     := compile.extern.facebook_sdk compile.extern.android_support_v4

#Цель - tests
SOCIAL.FACEBOOK.TESTS.TYPE        := test-suite
SOCIAL.FACEBOOK.TESTS.SOURCE_DIRS := tests
SOCIAL.FACEBOOK.TESTS.IMPORTS     := compile.social.core compile.common compile.system link.social.facebook link.common.json
